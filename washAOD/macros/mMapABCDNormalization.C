#include "mMapABCDNormalization.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename1 = TString(cfg["infilename1"].get<std::string>());
        TString in_filename2 = TString(cfg["infilename2"].get<std::string>());
        if (in_filename1 == TString("") || in_filename2 == TString("")) {
            cout << "ERROR! Need 2 input filenames in config. Exiting..." << endl;
            return 0;
        }
        TFile * out_file = new TFile("normalization_map.root", "RECREATE");
        TFile * in_file1 = new TFile(in_filename1, "READ");
        TFile * in_file2 = new TFile(in_filename2, "READ");

        gROOT->SetBatch(kTRUE);

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
                    TCanvas * c1 = (TCanvas*)in_file1->Get(canvas_name);
                    TCanvas * c2 = (TCanvas*)in_file2->Get(canvas_name);
                    TH2D * h1 = (TH2D*)c1->FindObject(h_name);
                    TH2D * h2 = (TH2D*)c2->FindObject(h_name);
                    cout << "Integral of histogram 1 with over+underflow: " << h1->Integral(0, h1->GetNbinsX()+1, 0, h1->GetNbinsY()+1) << endl;
                    cout << "Integral of histogram 2 with over+underflow: " << h2->Integral(0, h2->GetNbinsX()+1, 0, h2->GetNbinsY()+1) << endl;

                    TH2D * h_map = (TH2D*)h1->Clone();
                    h_map->Reset();
                    h_map->SetName("NormMap_" + h_name);
                    double avg_ratio = 0.0;
                    int n_bins = 0;

                    for (int i = 0; i <= h1->GetNbinsX()+1; i++) {
                        for (int j = 0; j <= h1->GetNbinsY()+1; j++) {
                            float B1 = h1->Integral(i, h1->GetNbinsX()+1, 0, j-1);
                            float B2 = h2->Integral(i, h1->GetNbinsX()+1, 0, j-1);
                            float C1 = h1->Integral(0, i-1, j, h1->GetNbinsY()+1);
                            float C2 = h2->Integral(0, i-1, j, h1->GetNbinsY()+1);
                            if (B1 < 1 || B2 < 1 || C1 < 1 || C2 < 1) {
                                h_map->SetBinContent(i, j, 0);
                            }
                            else {
                                float ratio1 = C1/B1;
                                float ratio2 = C2/B2;
                                h_map->SetBinContent(i, j, ratio1/ratio2);
                                avg_ratio += ratio1/ratio2;
                                n_bins++;
                            }
                        }
                    }
                    avg_ratio /= n_bins;
                    cout << "Average ratio: " << avg_ratio << endl;

                    out_file->cd();
                    h_map->Write();
                }
            }
        }

        gROOT->SetBatch(kFALSE);

        in_file1->Close();
        in_file2->Close();
        out_file->Close();
        return 0;
    }
} // namespace macro
