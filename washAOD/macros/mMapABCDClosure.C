#include "mMapABCDClosure.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename = TString(cfg["infilename"].get<std::string>());
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TFile * out_file = new TFile("closure_map.root", "RECREATE");
        TFile * in_file = new TFile(in_filename, "READ");

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
                    cout << "Integral with over+underflow: " << h->Integral(0, h->GetNbinsX()+1, 0, h->GetNbinsY()+1) << endl;

                    TH2D * h_map = (TH2D*)h->Clone();
                    h_map->Reset();
                    h_map->SetName("ClosureMap_" + h_name);
                    double avg_closure = 0.0;
                    double avg_closure_sqrd = 0.0;
                    int n_bins = 0;

                    for (int i = 0; i <= h->GetNbinsX()+1; i++) {
                        for (int j = 0; j <= h->GetNbinsY()+1; j++) {
                            float A = h->Integral(0, i-1, 0, j-1);
                            float B = h->Integral(i, h->GetNbinsX()+1, 0, j-1);
                            float C = h->Integral(0, i-1, j, h->GetNbinsY()+1);
                            float D = h->Integral(i, h->GetNbinsX()+1, j, h->GetNbinsY()+1);
                            float C_pred = A*D/B;
                            float C_pred_syst_err = C_pred * sqrt(1/A + 1/B + 1/D);
                            float C_pred_stat_err = sqrt(C_pred);
                            float DeltaC = abs(C - C_pred)/C;
                            if (C_pred_syst_err < 0.5*C_pred) {
                                h_map->SetBinContent(i, j, DeltaC);
                                avg_closure += DeltaC;
                                avg_closure_sqrd += DeltaC*DeltaC;
                                n_bins++;
                            }
                            else
                                h_map->SetBinContent(i, j, 0);
                        }
                    }
                    avg_closure /= n_bins;
                    avg_closure_sqrd /= n_bins;
                    cout << "Average closure: " << avg_closure << ", spread: " << std::sqrt(std::abs(avg_closure_sqrd-avg_closure*avg_closure)) << endl;

                    out_file->cd();
                    h_map->Write();
                }
            }
        }

        gROOT->SetBatch(kFALSE);

        in_file->Close();
        out_file->Close();
        return 0;
    }
} // namespace macro
