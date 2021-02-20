#include <algorithm>
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
#include <TH2D.h>
#include <THStack.h>
#include <TKey.h>
#include <TLegend.h>
#include <TObjString.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../utils/CMS_lumi.h"
#include "../utils/common.h"
using namespace common;
#include "../utils/cxxopts.hpp"
#include "../utils/json.hpp"
using json = nlohmann::json;
#include "../utils/tdrstyle.h"

namespace macro {

    std::unique_ptr<TCanvas> makeCanvas(TH2D * hist, TString plot_name, TString sample_name, TString title, vector<CutInfo> cuts_info, int cut, TString years) {
        auto canvas = std::make_unique<TCanvas>(Form("canvas2D_%s", plot_name.Data()));
        auto * c = canvas.get();
        c->cd();
        gPad->Draw(); 
        gPad->cd();
        hist->Draw("COLZ");
        hist->SetTitle("");
        Ssiz_t from = 0;
        TString tok;
        if (title.Tokenize(tok, from, " vs "))
            hist->GetXaxis()->SetTitle(tok);
        if (title.Tokenize(tok, from, " vs "))
            hist->GetYaxis()->SetTitle(tok);
        hist->GetYaxis()->SetTitleOffset(1.3);
        const bool writeExtraText = true;
        CMS_lumi((TPad*)gPad, 4, 0, years);
        gPad->SetLogz();
        gPad->Modified();
        gPad->Update();
        
        // Make cut description label
        TLatex cut_label;
        cut_label.SetNDC();
        cut_label.SetTextSize(0.04);
        if (cuts_info[cut].special == TString("yes"))
            cut_label.SetTextColor(kRed);
        cut_label.DrawLatexNDC(0.25, 0.90, cuts_info[cut].description.Data());

        /////// No legend on 2D plots
        // Make legend
        //TLegend * legend = new TLegend();
        //legend->AddEntry(hist, sample_name, "f");
        //legend->Draw();

        return canvas;
    }

    extern "C" bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();

        // for this macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! mMake2DPlotsFromFile requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }
        
        // macro options

        vector<std::string> in_filenames = cfg["infilenames"].get<vector<std::string>>();
        if (in_filenames.size() == 0) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TString in_filename = TString(in_filenames[0]);

        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == TString(""))
            out_filename = in_filename;

        TFile * in_file, * out_file;
        if (out_filename == "" || out_filename == in_filename) {
            in_file = TFile::Open(in_filename, "UPDATE");
            out_file = in_file;
        }
        else {
            in_file = TFile::Open(in_filename);
            out_file = TFile::Open(out_filename, "RECREATE");
        }

        TString years = TString(cfg["years"].get<std::string>());
        if (years == "") {
            // Try to infer year from filenames
            vector<TString> year_set{"2016", "2017", "2018", "161718", "1617", "1618", "1718"};
            for (auto year : year_set) {
                if (in_filename.Contains(year)) {
                    years = year;
                    break;
                }
            }
            if (years == "") {
                cout << "ERROR: Year(s) must be specified! (Via macro config, command line, or filename.) Exiting..." << endl;
                return 0;
            }
        }

        if (cfg.find("custom_lumi") != cfg.end()) { // custom lumi (likely incomplete samples)
            cout << endl << "WARNING! Using custom luminosity from config..." << endl << endl;
            years = TString(cfg["custom_lumi"].get<std::string>());
        }

        // Loop over the keys
        
        map<TString, std::unique_ptr<TCanvas>> canvases;
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "THStack") continue;
            TString hs_name = TString(key->GetName());
            if (!hs_name.Contains("_vs_")) continue; // mMake1DPlotsFromFile handles these
            cout << "Processing " << hs_name << ", class " << key->GetClassName() << endl;
            THStack * hs = (THStack*)in_file->Get(hs_name);
            TString hs_basename = ((TObjString*)(hs_name.Tokenize("-")->At(0)))->String();
            TString hs_suffix = ((TObjString*)(hs_name.Tokenize("-")->At(1)))->String();

            int cut;
            TString tok;
            Ssiz_t from = 0;
            from = 0;
            while (hs_basename.Tokenize(tok, from, "_")) 
                if (tok.Contains("cut")) 
                    cut = (((TObjString*)(tok.Tokenize("t")->At(1)))->String()).Atoi();

            if (!hs_name.Contains("SIGNAL")) { // Only draw the sum of the stack for backgrounds in 2D
                TH2D * hsum = (TH2D*)(hs->GetStack()->Last()->Clone());
                cout << "hs_name: " << hs_name << endl;
                hsum->SetName(hs_name);
                canvases[hs_name] = makeCanvas(hsum, hs_name, hs_name, hs->GetTitle(), cuts_info, cut, years);
            }
            else { // for signal, want one canvas for each element of stack
                // iterate over hstack
                TList * histKeys = hs->GetHists();
                TIter next(histKeys);
                TObject * object = 0;
                while ((object = next())) {
                    TH2D * hist = (TH2D*)object;
                    canvases[hist->GetName()] = makeCanvas(hist, hist->GetName(), hist->GetName(), hs->GetTitle(), cuts_info, cut, years);
                }
            }
        }

        // Save canvases

        for (auto & pair : canvases) {
            pair.second.get()->Write();
            //delete c;
        }

        out_file->Close();
        if (out_file != in_file)
            in_file->Close();

        if (cfg.find("custom_lumi") != cfg.end()) { // custom lumi (likely incomplete samples)
            cout << endl << "WARNING! Using custom luminosity from config..." << endl << endl;
        }

        return 0;
    }
}
