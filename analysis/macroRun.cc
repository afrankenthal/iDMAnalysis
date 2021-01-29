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
        ("s,signal", "Signal sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/thirdrun/signal.json"))
        ("b,background", "Background sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/thirdrun/backgrounds_subset.json"))
        ("d,data", "Data sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/data.json"))
        ("m,macro", "Macro config file to use", cxxopts::value<std::string>()->default_value("")) //configs/macros/nminus1.json"))
        ("c,cuts", "Cuts config file to use", cxxopts::value<std::string>()->default_value("")) //configs/cuts/CR_nJets.json"))
        ("y,years", "Year(s) to process: 2016, 2017, 2018, 161718, 1618, 1718, 1617", cxxopts::value<std::string>()->default_value(""))
        ("o,outfile", "Output file or directory (depends on macro)", cxxopts::value<std::string>()->default_value(""))
        ("i,infile", "Input file(s) (for macros that use results of other macros)", cxxopts::value<std::vector<std::string>>()->default_value({""}))
        ("h,help", "Print help and exit.")
    ;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        cout << options.help() << std::endl;
        exit(0);
    }

    // Program options
    vector<TString> sample_config_filenames {};
    if (TString(result["background"].as<std::string>()) != "")
        sample_config_filenames.push_back(TString(result["background"].as<std::string>()));
    if (TString(result["signal"].as<std::string>()) != "")
        sample_config_filenames.push_back(TString(result["signal"].as<std::string>()));
    if (TString(result["data"].as<std::string>()) != "")
        sample_config_filenames.push_back(TString(result["data"].as<std::string>()));
    TString macro_filename = TString(result["macro"].as<std::string>());
    TString cuts_filename = TString(result["cuts"].as<std::string>());
    TString out_filename = TString(result["outfile"].as<std::string>()); // default is empty string ""
    std::vector<std::string> in_filenames = result["infile"].as<std::vector<std::string>>(); // default is length-one vector with {""}
    if (in_filenames.size() == 0)
        in_filenames.push_back("");
    TString years = TString(result["years"].as<std::string>());
 
    map<TString, SampleInfo> samples;

    for (auto config_filename : sample_config_filenames) { 

        std::ifstream config_file(config_filename.Data());
        json configs;
        config_file >> configs;
        int color = 1;
        for (auto const & [sample, cfg] : configs.items()) {
            vector<TString> filelist{};
            for (auto dir : cfg["dir"].get<std::vector<std::string>>()) {
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
                (config_filename.Contains("signal") ? 1 : 1) // line style (bkg vs signal)
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

            TString inclusive;
            if (item.find("inclusive") != item.end())
                inclusive = TString(item["inclusive"].get<std::string>());
            else
                inclusive = TString("yes");

            TString efficiency;
            if (item.find("efficiency") != item.end())
                efficiency = TString(item["efficiency"].get<std::string>());
            else
                efficiency = TString("none");

            TString special;
            if (item.find("special") != item.end())
                special = TString(item["special"].get<std::string>());
            else
                special = TString("no");

            TString book_plot;
            if (item.find("book_plot") != item.end())
                book_plot = TString(item["book_plot"].get<std::string>());
            else
                book_plot = TString("yes");

            cuts_info.push_back(CutInfo{cut, description, inclusive, efficiency, special, book_plot});
        }
    }

    // Process macro config json and invoke configured macros
    if (macro_filename == TString("")) {
        cout << "Error! Need to specify a macro config json file. Exiting." << endl;
        printTimeElapsed(time_begin);
        return 1;
    }
    std::ifstream macro_config_file(macro_filename.Data());
    json macro_configs;
    macro_config_file >> macro_configs;

    //for (auto const & [macro, cfg] : macro_configs.items()) {
    for (auto const & item : macro_configs) {

        auto macro = item["name"].get<std::string>();
        auto cfg = item["config"];

        if (years != TString(""))
            cfg["years"] = years;

        cfg["outfilename"] = out_filename.Data();
        cfg["infilenames"] = in_filenames;

        if (macro == "mSumGenWgts") {
            if (sample_config_filenames.size() != 1) {
                cout << "Can only run mSumGenWgts with 1 sample config!" << endl;
                break;
            }
            cfg["sample_config_filename"] = sample_config_filenames[0].Data();
        }

        if (macro == "mSaveCanvases")
            cfg["outdir"] = out_filename.Data();

        // Load macro .so dynamically
        void * handle;
        bool (*process)(map<TString, SampleInfo>, vector<CutInfo>, json);
        if ( (handle = dlopen(Form("./lib%s.so", macro.c_str()), RTLD_NOW)) == NULL) {
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
