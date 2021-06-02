#include <algorithm>
#include <cstdlib>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TFile.h>
#include <TH1F.h>
#include <TKey.h>
#include <TImage.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../utils/common.h"
using namespace common;
#include "../utils/cxxopts.hpp"
#include "../utils/json.hpp"
using json = nlohmann::json;

namespace macro {

    extern "C" bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options

        bool force_overwrite = cfg["force_overwrite"].get<bool>();

        vector<std::string> in_filenames = cfg["infilenames"].get<vector<std::string>>();
        if (in_filenames.size() == 0) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }

        TString in_filename = TString(in_filenames[0]);
        TFile * in_file = TFile::Open(in_filename, "READ");

        TString out_dir;
        // Use output dir if provided or just strip the filename's extension to use as dir
        if (cfg.find("outfilename") != cfg.end() && cfg["outfilename"] != std::string("")) 
            out_dir = TString(cfg["outfilename"].get<std::string>());
        else 
            out_dir = in_filename.ReplaceAll(".root", "");

        // fs::path dir(out_dir.Data());
        // if (fs::exists(dir) && !out_dir.Contains("root://") && force_overwrite == false) {
        //     cout << "Error! Local output directory " << out_dir.Data() << " already exists!" << endl;
        //     cout << "Set 'force_overwrite: true' in the config or --force_overwrite=true in the command line to overwrite." << endl;
        //     return 0;
        // }
        // fs::remove_all(dir);
        // fs::create_directories(dir);

        if (!fs::is_directory("plot_temp") || !fs::exists("plot_temp"))
            fs::create_directory("plot_temp");
        else {
            std::cout << "Error! Temporary plot directory plot_temp already exists. Not overwriting..." << endl;
            return 0;
        }

        gROOT->SetBatch(kTRUE);
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;
            
            // TString tok;
            // Ssiz_t from = 0;
            // int cut_num = 00;
            // while (canvas_name.Tokenize(tok, from, "_cut"))
            //     if (!tok.Contains("canvas"))
            //         cut_num = tok.Atoi();
            // fs::create_directories(Form("plot_temp/cut%02d", cut_num));

            TCanvas * c = (TCanvas*)in_file->Get(canvas_name);
            canvas_name.ReplaceAll("-", "_");
            c->Print(Form("plot_temp/%s.png", canvas_name.Data()));
            c->Print(Form("plot_temp/%s.pdf", canvas_name.Data()));
            c->SaveAs(Form("plot_temp/%s.root", canvas_name.Data()));
        }

        // Move newly-created plots to EOS using xrdcp and delete temporary dir
        auto command = Form("xrdcp --streams 8 --recursive %s plot_temp %s", (force_overwrite ? "--force" : ""), out_dir.Data());
        system(command);
        // fs::remove_all("./plot_temp");

        gROOT->SetBatch(kFALSE);

        in_file->Close();

        return 0;
    }

} // namespace macro
