#include "mCalcABCDRatios.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename = TString(cfg["infilenames"].get<std::vector<std::string>>()[0]);
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TFile * in_file = new TFile(in_filename, "READ");

        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == "")
            out_filename = "ABCD_ratios.root";

        TFile * out_file = new TFile(out_filename, "RECREATE");

        float lowX = cfg["lowX"].get<float>(), highX = cfg["highX"].get<float>();
        float lowY = cfg["lowY"].get<float>(), highY = cfg["highY"].get<float>();
        std::vector<float> ratioRangesX = cfg["ratioRangesX"].get<std::vector<float>>();
        std::vector<float> ratioRangesY = cfg["ratioRangesY"].get<std::vector<float>>();

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
                    int lowXBin = h->GetXaxis()->FindBin(lowX), highXBin = h->GetXaxis()->FindBin(highX);
                    int lowYBin = h->GetYaxis()->FindBin(lowY), highYBin = h->GetYaxis()->FindBin(highY);
                    cout << "Low X bin: " << lowXBin << ", high X bin: " << highXBin << endl;
                    cout << "Low Y bin: " << lowYBin << ", high Y bin: " << highYBin << endl;

                    //TGraph * hRatioX = new TGraph(ratioRangesX.size()-1);
                    TH1F * hRatioX = new TH1F(Form("ratioX_%s", h_name.Data()), "", ratioRangesX.size()-1, ratioRangesX.data());
                    for (auto i = 0; i < ratioRangesX.size()-1; i++) {
                        int lowxbin = h->GetXaxis()->FindBin(ratioRangesX[i]);
                        int highxbin = h->GetXaxis()->FindBin(ratioRangesX[i+1]);
                        auto num = h->Integral(lowxbin, highxbin-1, highYBin, -1);
                        auto denom = h->Integral(lowxbin, highxbin-1, lowYBin, highYBin-1);
                        auto ratio = num / denom;
                        auto ratio_error = ratio * sqrt(1/num + 1/denom);
                        hRatioX->SetBinContent(1+i, ratio);
                        hRatioX->SetBinError(1+i, ratio_error);
                        //hRatioX->SetPoint(i, (ratioRangesX[i]+ratioRangesX[i+1])/2, ratio);
                    }
                    TH1F * hRatioY = new TH1F(Form("ratioY_%s", h_name.Data()), "", ratioRangesY.size()-1, ratioRangesY.data());
                    //TGraph * hRatioY = new TGraph(ratioRangesY.size()-1);
                    //hRatioY->SetName(Form("ratioY_%s", h_name.Data())); //, "", ratioRangesX.size()-1); //, ratioRangesX.data());
                    for (auto i = 0; i < ratioRangesY.size()-1; i++) {
                        int lowybin = h->GetYaxis()->FindBin(ratioRangesY[i]);
                        int highybin = h->GetYaxis()->FindBin(ratioRangesY[i+1]);
                        auto num = h->Integral(highXBin, -1, lowybin, highybin-1);
                        auto denom = h->Integral(lowXBin, highXBin-1, lowybin, highybin-1);
                        auto ratio = num/denom;
                        auto ratio_error = ratio * sqrt(1/num + 1/denom);
                        hRatioY->SetBinContent(1+i, ratio);
                        hRatioY->SetBinError(1+i, ratio_error);
                        //hRatioY->SetPoint(i, (ratioRangesY[i]+ratioRangesY[i+1])/2, ratio);
                    }
                    out_file->cd();
                    hRatioX->Write();
                    hRatioY->Write();
                    //if (lowXBin == 0) lowXBin = 1;
                    //if (lowYBin == 0) lowYBin = 1;
                    //if (highXBin == h->GetNbinsX()) highXBin--;
                    //if (highYBin == h->GetNbinsY()) highYBin--;
                    // X = dPhi, Y = vtx
                    // C: low dPhi, high vtx; B: high dPhi, low vtx; A: low dPhi, low vtx; D: high dPhi, high vtx
                    
                    //float A = h->Integral(lowXBin, highXBin-1, lowYBin, highYBin-1);
                    //float B = h->Integral(highXBin, h->GetNbinsX()+1, lowYBin, highYBin-1);
                    //float C = h->Integral(lowXBin, highXBin-1, highYBin, h->GetNbinsY()+1);
                    //float D = h->Integral(highXBin, h->GetNbinsX()+1, highYBin, h->GetNbinsY()+1);
                    //float C_pred = A*D/B;
                    //float C_pred_syst_err = C_pred * sqrt(1/A + 1/B + 1/D);
                    //float C_pred_stat_err = sqrt(C_pred);
                    //cout << "A: " << A << " +/- " << sqrt(A) << endl;
                    //cout << "B: " << B << " +/- " << sqrt(B) << endl;
                    //cout << "C: " << C << " +/- " << sqrt(C) << endl;
                    //cout << "D: " << D << " +/- " << sqrt(D) << endl;
                    //cout << "A*D/B: " << C_pred << " +/- "
                    //    <<  C_pred_stat_err << " (stat.) +/- " << C_pred_syst_err << " (syst.)" << endl;
                    //cout << "C: " << C << " +/- " << sqrt(C) << endl;
                    //cout << endl;
                }
            }
        }

        gROOT->SetBatch(kFALSE);

        in_file->Close();
        out_file->Close();
        return 0;
    }
} // namespace macro
