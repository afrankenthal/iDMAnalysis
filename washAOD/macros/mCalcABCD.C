#include "mCalcABCD.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename = TString(cfg["infilename"].get<std::string>());
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TFile * in_file = new TFile(in_filename, "READ");

        float lowX = cfg["lowX"].get<float>(), highX = cfg["highX"].get<float>();
        float lowY = cfg["lowY"].get<float>(), highY = cfg["highY"].get<float>();

        gROOT->SetBatch(kTRUE);

        for (auto && keyAsObj : *in_file->GetListOfKeys()) {
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
                    TCanvas * c = (TCanvas*)in_file->Get(canvas_name);
                    TH2D * h = (TH2D*)c->FindObject(h_name);
                    cout << "Integral: " << h->Integral() << endl;
                    int lowXBin = h->GetXaxis()->FindBin(lowX), highXBin = h->GetXaxis()->FindBin(highX);
                    int lowYBin = h->GetYaxis()->FindBin(lowY), highYBin = h->GetYaxis()->FindBin(highY);
                    //if (lowXBin == 0) lowXBin = 1;
                    //if (lowYBin == 0) lowYBin = 1;
                    //if (highXBin == h->GetNbinsX()) highXBin--;
                    //if (highYBin == h->GetNbinsY()) highYBin--;
                    // X = dPhi, Y = vtx
                    // C: low dPhi, high vtx; B: high dPhi, low vtx; A: low dPhi, low vtx; D: high dPhi, high vtx
                    float A = h->Integral(lowXBin, highXBin-1, lowYBin, highYBin-1);
                    float B = h->Integral(highXBin, h->GetNbinsX(), lowYBin, highYBin-1);
                    float C = h->Integral(lowXBin, highXBin-1, highYBin, h->GetNbinsY());
                    float D = h->Integral(highXBin, h->GetNbinsX(), highYBin, h->GetNbinsY());
                    cout << "A: " << A << endl;
                    cout << "B: " << B << endl;
                    cout << "C: " << C << endl;
                    cout << "D: " << D << endl;
                    cout << "A*D/B: " << A*D/B << ", C: " << C << endl;
                }
            }
        }

        gROOT->SetBatch(kFALSE);

        in_file->Close();
        return 0;
    }
} // namespace macro
