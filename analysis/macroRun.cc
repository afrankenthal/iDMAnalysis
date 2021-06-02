#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <unordered_map>

#include <TROOT.h>
#include <TDatime.h>
#include <TCollection.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TString.h>
#include <TChain.h>
#include <TApplication.h>
#include <TCut.h>
#include <TH1F.h>
#include <TPaveText.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLegend.h>


#include "utils/common.h"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"

using std::cout, std::endl, std::map, std::vector;

using FUNCPTR =  bool(*)(map<TString, SampleInfo>, vector<CutInfo>, json); // make map of macros
std::unordered_map<std::string, FUNCPTR> macro_map;

int main(int argc, char ** argv) {
    TDatime time_begin;

    cxxopts::Options options("macroRun", "Run configurable macros with common IO interface");
    options.add_options()
        ("s,sample", "Sample config json(s) to set MC/data samples, if any", cxxopts::value<vector<std::string>>()->default_value("")) //configs/thirdrun/signal.json"))
        ("m,macro", "Macro config json to set the macros to invoke", cxxopts::value<std::string>()->default_value("")) //configs/macros/nminus1.json"))
        ("c,cuts", "Cuts config json to set the cutflow", cxxopts::value<std::string>()->default_value("")) //configs/cuts/CR_nJets.json"))
        ("p,plots", "Plots config json(s) to set which plots to produce, if any", cxxopts::value<vector<std::string>>()->default_value("")) // configs/plots/Nominal_2D.json"))
        ("y,years", "Year(s) to process: 2016, 2017, 2018, 161718, 1618, 1718, 1617", cxxopts::value<std::string>()->default_value(""))
        ("o,outfile", "Output file or directory (depends on macro)", cxxopts::value<std::string>()->default_value(""))
        ("i,infile", "Input file(s) (for macros that use results of other macros)", cxxopts::value<vector<std::string>>()->default_value({}))
        ("l,filelist", "File(s) with list of input files (one input file per line)", cxxopts::value<vector<std::string>>()->default_value({}))
        ("k,force_overwrite", "Force overwrite of plot directory", cxxopts::value<bool>()->default_value("false")->implicit_value("false"))
        ("t,systematic", "Systematic variation (please refer to utils/ScaleFactors.h for options)", cxxopts::value<int>()->default_value("0"))
        ("h,help", "Print help and exit.")
    ;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        cout << options.help() << std::endl;
        exit(0);
    }

    // Program options
    vector<std::string> sample_config_filenames = result["sample"].as<vector<std::string>>();
    TString macro_filename = TString(result["macro"].as<std::string>());
    TString cuts_filename = TString(result["cuts"].as<std::string>());
    vector<std::string> plot_config_filenames = result["plots"].as<vector<std::string>>();
    TString years = TString(result["years"].as<std::string>());
    TString out_filename = TString(result["outfile"].as<std::string>()); // default is empty string ""
    vector<std::string> in_filenames = result["infile"].as<vector<std::string>>(); // default is length-zero vector with {}
    vector<std::string> list_filenames = result["filelist"].as<vector<std::string>>(); // default is length-zero vector with {}
    bool force_overwrite = result["force_overwrite"].as<bool>();
    int systematic = result["systematic"].as<int>();

    // add 
    if (list_filenames.size() > 0) {
        for (auto list_filename : list_filenames) {
            std::ifstream infile(list_filename);
            std::string file;
            while (infile >> file)
                in_filenames.push_back(file);
        }
    }

    // Process samples config json
    map<TString, SampleInfo> samples;
    for (auto config_filename : sample_config_filenames) { 

        std::ifstream config_file(config_filename);
        json configs;
        config_file >> configs;
        int color = 1;
        for (auto const & [sample, cfg] : configs.items()) {
            vector<TString> filelist{};
            for (auto dir : cfg["dir"].get<vector<std::string>>()) {
                vector<TString> newlist = listFiles(dir.c_str());
                filelist.insert(filelist.end(), newlist.begin(), newlist.end());
            }
            if (filelist.size() == 0)
                cout << "WARNING! List of files for sample " << sample << " is empty! Make sure you specfied the right directory." << endl;
            samples[TString(sample)] = SampleInfo{
                filelist,
                sample, // plot label
                cfg["xsec"].get<float>(), // xsec
                cfg["sum_gen_wgt"].get<float>(), // sum_gen_wgt
                cfg["limit_num_files"].get<int>(), // limit_num_files
                1, // weight
                cfg["year"].get<int>(), // year 
                (cfg.find("lumi") != cfg.end() ? cfg["lumi"].get<float>() : -1.0f), // custom lumi
                TString(cfg["group"].get<std::string>()), // sample group
                mapMODE[TString(cfg["mode"].get<std::string>())], // mode: 0 = BKG, 1 = DATA, 2 = SIGNAL
                (cfg.find("color") != cfg.end() ? cfg["color"].get<int>() : color++), // line color
                (mapMODE[TString(cfg["mode"].get<std::string>())] > 0 ? 1 : 1) // line style (bkg vs signal)
            };
        }
    }


    // Process cuts config json
    vector<CutInfo> cuts_info;
    if (cuts_filename != TString("")) {
        std::ifstream cut_configs_file(cuts_filename.Data());
        json cut_configs;
        cut_configs_file >> cut_configs;

        for (auto const & item : cut_configs) {
            auto cut = TString(item["cut"].get<std::string>());
            auto description = TString(item["description"].get<std::string>());

            TString inclusive("yes");
            if (item.find("inclusive") != item.end())
                inclusive = TString(item["inclusive"].get<std::string>());

            TString efficiency("none");
            if (item.find("efficiency") != item.end())
                efficiency = TString(item["efficiency"].get<std::string>());

            TString special("no");
            if (item.find("special") != item.end())
                special = TString(item["special"].get<std::string>());

            TString book_plot("yes");
            if (item.find("book_plot") != item.end())
                book_plot = TString(item["book_plot"].get<std::string>());

            TString print_events("no");
            if (item.find("print_events") != item.end())
                print_events = TString(item["print_events"].get<std::string>());

            cuts_info.push_back(CutInfo{cut, description, inclusive, efficiency, special, book_plot, print_events});
        }
    }

    // Process plots config json
    json plots_config = json::array();
    for (auto plot_config_filename : plot_config_filenames) {
        std::ifstream plots_config_file(plot_config_filename);
        json j;
        plots_config_file >> j;
        for (json::iterator it = j.begin(); it != j.end(); ++it)
            plots_config.push_back(*it);
    }

    // Process macro config json and invoke configured macros
    if (macro_filename == TString("")) {
        cout << "Error! Need to specify a macro config json. Exiting..." << endl;
        printTimeElapsed(time_begin);
        return 1;
    }
    std::ifstream macro_config_file(macro_filename.Data());
    json macro_configs;
    macro_config_file >> macro_configs;
    for (auto const & item : macro_configs) {
        auto macro = item["name"].get<std::string>();
        auto cfg = item["config"];

        cfg["plots"] = plots_config;
        cfg["years"] = years;
        cfg["outfilename"] = out_filename.Data();
        cfg["infilenames"] = in_filenames;
        cfg["sample_config_filenames"] = sample_config_filenames;
        if (cfg.find("systematic") == cfg.end())
            cfg["systematic"] = systematic;
        if (cfg.find("force_overwrite") == cfg.end())
            cfg["force_overwrite"] = force_overwrite;

        // Load macro .so dynamically
        void * handle;
        bool (*process)(map<TString, SampleInfo>, vector<CutInfo>, json);
        if ((handle = dlopen(Form("lib%s.so", macro.c_str()), RTLD_NOW)) == NULL) {
            cout << "Error! Could not find lib" << macro << ".so" << endl;
            cout << "dlerror(): " << dlerror() << endl;
            return 1;
        }
        dlerror();
        process = (FUNCPTR)dlsym(handle, "process");
        if (dlerror() != NULL) {
            cout << "Error! Could not locate function 'process' inside lib" << macro << ".so" << endl;
            dlclose(handle);
            return 2;
        }

        cout << endl << ">>>>>>>>> Starting macro " << macro << " with config:" << endl << endl;
        cout << cfg.dump(4) << endl << endl;
        process(samples, cuts_info, cfg);
        dlclose(handle);
    }

    cout << endl;
    printTimeElapsed(time_begin);
    return 0;
}
