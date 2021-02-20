#include <algorithm>
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
#include <TH2D.h>
#include <THStack.h>
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
        vector<std::string> in_filenames = cfg["infilenames"].get<std::vector<std::string>>();

        if (in_filenames.size() < 2) {
            cout << "ERROR! Require 2 input filenames (one for signal and one for bkg). Exiting..." << endl;
            return 0;
        }
        TFile * in_file1 = new TFile(in_filenames[0].c_str(), "READ");
        TFile * in_file2 = new TFile(in_filenames[1].c_str(), "READ");

        gROOT->SetBatch(kTRUE);

        std::vector<TH2D*> hSignalVec;
        for (auto && keyAsObj : *in_file1->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;

            if (canvas_name.Contains("canvas2D")) {
                if (canvas_name.Contains("_sig_") || canvas_name.Contains("-BKG") || canvas_name.Contains("-DATA")) {
                    // Get TH2D
                    TString h_name = canvas_name;
                    h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
                    cout << h_name << endl;
                    TCanvas * c = (TCanvas*)in_file1->Get(canvas_name);
                    TH2D * h = (TH2D*)c->FindObject(h_name);
                    cout << "Integral with over+underflow: " << h->Integral(0, h->GetNbinsX()+1, 0, h->GetNbinsY()+1) << endl;

                    TH2D * hSignal = (TH2D*)h->Clone();
                    hSignal->Reset();

                    // loop over all possible boundaries

                    for (int xBound = 1; xBound <= h->GetNbinsX()+1; xBound++) {
                        for (int yBound = 1; yBound <= h->GetNbinsY()+1; yBound++) {
                            float A = h->Integral(0, xBound-1, 0, yBound-1);
                            float B = h->Integral(xBound, h->GetNbinsX()+1, 0, yBound-1);
                            float C = h->Integral(0, xBound-1, yBound, h->GetNbinsY()+1);
                            float D = h->Integral(xBound, h->GetNbinsX()+1, yBound, h->GetNbinsY()+1);
                            hSignal->SetBinContent(xBound, yBound, C);
                        }
                    }

                    hSignalVec.push_back(hSignal);
                }
            }
        }

        std::vector<TH2D*> hBkgVec;
        for (auto && keyAsObj : *in_file2->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;

            if (canvas_name.Contains("canvas2D")) {
                if (canvas_name.Contains("_sig_") || canvas_name.Contains("-BKG") || canvas_name.Contains("-DATA")) {
                    // Get TH2D
                    TString h_name = canvas_name;
                    h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
                    cout << h_name << endl;
                    TCanvas * c = (TCanvas*)in_file2->Get(canvas_name);
                    TH2D * h = (TH2D*)c->FindObject(h_name);
                    cout << "Integral with over+underflow: " << h->Integral(0, h->GetNbinsX()+1, 0, h->GetNbinsY()+1) << endl;

                    TH2D * hBkg = (TH2D*)h->Clone();
                    hBkg->Reset();

                    // loop over all possible boundaries
                    for (int xBound = 1; xBound <= h->GetNbinsX()+1; xBound++) {
                        for (int yBound = 1; yBound <= h->GetNbinsY()+1; yBound++) {
                            float A = h->Integral(0, xBound-1, 0, yBound-1);
                            float B = h->Integral(xBound, h->GetNbinsX()+1, 0, yBound-1);
                            float C = h->Integral(0, xBound-1, yBound, h->GetNbinsY()+1);
                            float D = h->Integral(xBound, h->GetNbinsX()+1, yBound, h->GetNbinsY()+1);
                            hBkg->SetBinContent(xBound, yBound, sqrt(C));
                        }
                    }
                    hBkgVec.push_back(hBkg);
                }
            }
        }

        TFile * file_out = new TFile("scan.root", "RECREATE");
        for (auto h : hSignalVec) {
            h->Write();
        }
        for (auto h : hBkgVec) {
            h->Write();
        }
        file_out->Close();

        gROOT->SetBatch(kFALSE);
        in_file1->Close();
        in_file2->Close();

        return 0;
    }

} // end namespace macro
