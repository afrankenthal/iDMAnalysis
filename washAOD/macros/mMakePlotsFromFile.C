#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <TDatime.h>
#include <TCollection.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TString.h>
#include <TChain.h>
#include <TApplication.h>
#include <TCut.h>
#include <TH1F.h>
#include <TSelector.h>

#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"
#include "utils/CMS_lumi.C"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    bool mMakePlotsFromFile([[maybe_unused]] map<TString, SampleInfo> samples, json cfg) {
        setTDRStyle();

        // macro options
        TString in_filename = TString(cfg["infilename"].get<std::string>());
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TString out_filename;
        if (cfg.find("outfilename") != cfg.end()) 
            out_filename = TString(cfg["outfilename"].get<std::string>());
        else 
            out_filename = in_filename;

        TFile * in_file, * out_file;
        if (out_filename == "" || out_filename == in_filename) {
            in_file = new TFile(in_filename, "UPDATE");
            out_file = in_file;
        }
        else {
            in_file = new TFile(in_filename);
            out_file = new TFile(out_filename, "RECREATE");
        }
        
        map<TString, std::unique_ptr<TCanvas>> canvases;
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "THStack") continue;
            TString hs_name = TString(key->GetName());
            cout << "Processing " << hs_name << ", class " << key->GetClassName() << endl;
            THStack * hs = (THStack*)in_file->Get(hs_name);
            TString hs_basename = ((TObjString*)(hs_name.Tokenize("-")->At(0)))->String();
            TString hs_suffix = ((TObjString*)(hs_name.Tokenize("-")->At(1)))->String();
            bool newCanvas = false;
            if (canvases.find(hs_basename) == canvases.end()) {
                canvases[hs_basename] = std::make_unique<TCanvas>(Form("canvas_%s", hs_basename.Data()));
                newCanvas = true;
            }
            auto * c = canvases[hs_basename].get();
            c->cd();
            TString drawOption = "";
            if (hs_suffix == "SIG")
                drawOption += "HIST NOSTACK";
            else if (hs_suffix == "BKG")
                drawOption += "HIST";
            else if (hs_suffix == "DATA")
                drawOption += "E P";
            if (!newCanvas)
                drawOption += " SAME";
            hs->Draw(drawOption.Data());
            if (newCanvas) {
                hs->GetXaxis()->SetTitle(hs->GetTitle());
                hs->GetYaxis()->SetTitle("Events");
                hs->SetTitle("");
                writeExtraText = true;
                //lumi_13TeV = "59.97 fb^{-1}";
                lumi_13TeV = "29.41 fb^{-1}";
                CMS_lumi(c, 4);
                c->SetLogy();
            }
            //hs->GetStack()->Last()->Draw("E");
            //canvases.push_back(std::move(c));
        }
        // Build legend for all canvases
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            c->cd();
            TLegend * legend = new TLegend(0.55, 0.65, 0.9, 0.90);
            legend->SetFillStyle(0);
            TIter next((TList*)c->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("THStack")) continue;
                TIter next2((TList*)(((THStack*)hs)->GetHists()));
                TH1F * h;
                while ((h = (TH1F*)next2())) {
                    if (TString(hs->GetName()).Contains("SIG"))
                        legend->AddEntry((TH1F*)h, "", "l");
                    else if (TString(hs->GetName()).Contains("BKG"))
                        legend->AddEntry((TH1F*)h, "", "f");
                    else if (TString(hs->GetName()).Contains("DATA"))
                        legend->AddEntry((TH1F*)h, "", "lep");
                }
            }
            legend->Draw();
        }
        
        // Figure out which THStack has highest maximum
        map<TString, Double_t> maxima;
        for (auto & pair : canvases) {
            maxima[pair.first] = -1000;
            auto * c = pair.second.get();
            TIter next((TList*)c->GetListOfPrimitives());
            TObject * h;
            while ((h = next())) {
                if (TString(h->ClassName()) != TString("THStack")) continue;
                if (((THStack*)h)->GetMaximum() > maxima[pair.first]) maxima[pair.first] = ((THStack*)h)->GetMaximum();
            }
        }
        // Set this maximum to all THStacks
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            TIter next((TList*)c->GetListOfPrimitives());
            TObject * h;
            while ((h = next())) {
                if (TString(h->ClassName()) != TString("THStack")) continue;
                ((THStack*)h)->SetMaximum(5*maxima[pair.first]);
                ((THStack*)h)->SetMinimum(0.01);
            }
            //c->RedrawAxis();
            c->Modified();
        }
        // Save canvases
        for (auto & pair : canvases) {
            pair.second.get()->Write();
            //delete c;
        }

        //out_file->Write();
        out_file->Close();
        if (out_file != in_file)
            in_file->Close();

        return 0;
    }
}
