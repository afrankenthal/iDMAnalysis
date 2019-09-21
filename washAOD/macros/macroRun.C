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

#include "mCutflowTables.C"
#include "mSumGenWgts.C"
#include "mSROptimization.C"
#include "mNminus1Plots.C"
#include "mMainAnalysis.C"
#include "mMakePlotsFromFile.C"
#include "mSaveCanvases.C"

#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"

using std::cout, std::endl, std::map, std::vector;

using FUNCPTR =  bool(*)(map<TString, SampleInfo>, json); // make map of macros
std::unordered_map<std::string, FUNCPTR> macro_map;

int main(int argc, char ** argv) {
    TDatime time_begin;

    cxxopts::Options options("macroRun", "Run configurable macros with common IO interface");
    options.add_options()
        ("s,signal", "Signal sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/thirdrun/signal.json"))
        ("b,background", "Background sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/thirdrun/backgrounds_subset.json"))
        ("d,data", "Data sample config file to use", cxxopts::value<std::string>()->default_value("")) //configs/data.json"))
        ("m,macro", "Macro config file to use", cxxopts::value<std::string>()->default_value("")) //configs/macros/nminus1.json"))
        ("o,outfile", "Output file or directory (depends on macro)", cxxopts::value<std::string>()->default_value(""))
        ("i,infile", "Input file", cxxopts::value<std::string>()->default_value(""))
        ("h,help", "Print help and exit.")
    ;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        cout << options.help() << std::endl;
        exit(0);
    }

    // Program options
    //TString which_cutflow = TString(result["which"].as<std::string>());
    vector<TString> sample_config_filenames {
        TString(result["background"].as<std::string>()),
        TString(result["signal"].as<std::string>()),
        TString(result["data"].as<std::string>())
    };
    TString macro_filename = TString(result["macro"].as<std::string>());
    TString out_filename = TString(result["outfile"].as<std::string>());
    TString in_filename = TString(result["infile"].as<std::string>());

    // Map between macro name and actual function reference
    macro_map["mCutflowTables"] = &macro::mCutflowTables;
    macro_map["mSumGenWgts"] = &macro::mSumGenWgts;
    macro_map["mSROptimization"] = &macro::mSROptimization;
    macro_map["mNminus1Plots"] = &macro::mNminus1Plots;
    macro_map["mMainAnalysis"] = &macro::mMainAnalysis;
    macro_map["mMakePlotsFromFile"] = &macro::mMakePlotsFromFile;
    macro_map["mSaveCanvases"] = &macro::mSaveCanvases;
    
    map<TString, SampleInfo> samples;

    for (auto config_filename : sample_config_filenames) { 
        if (config_filename == "") continue;

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
            samples[TString(sample)] = SampleInfo{
                filelist,
                //listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames
                sample, // plot label
                cfg["xsec"].get<float>(), // xsec
                cfg["sum_gen_wgt"].get<float>(), // sum_gen_wgt
                cfg["limit_num_files"].get<int>(), // limit_num_files
                1, // weight
                TString(cfg["group"].get<std::string>()), // sample group
                common::mapMODE[TString(cfg["mode"].get<std::string>())], // mode: 0 = SIGNAL, 1 = BKG, 2 = DATA
                (cfg.find("color") != cfg.end() ? cfg["color"].get<int>() : color++), // line color
                (config_filename.Contains("signal") ? 1 : 1) // line style (bkg vs signal)
            };
        }
    }

    std::ifstream macro_config_file(macro_filename.Data());
    json macro_configs;
    macro_config_file >> macro_configs;

    for (auto const & [macro, cfg] : macro_configs.items()) {
        if (macro == "mMainAnalysis")
            cfg["outfilename"] = out_filename.Data();
        if (macro == "mMakePlotsFromFile")
            cfg["infilename"] = in_filename.Data();

        if (macro == "mSumGenWgts") 
            cfg = json({{"sample_config_filename",sample_config_filenames[0].Data()}});

        if (macro == "mSaveCanvases") {
            cfg["outdir"] = out_filename.Data();
            cfg["infilename"] = in_filename.Data();
        }

        macro_map[macro](samples, cfg); // invoke macro with samples from above and cfg from json
    }

    cout << endl;
    printTimeElapsed(time_begin);

    return 0;
}
