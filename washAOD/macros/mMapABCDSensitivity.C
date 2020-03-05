#include "mMapABCDSensitivity.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        cout << "Warning: re-scaling signal yields from 2018 to 2016+2017+2018 (factor of x2.30)" << endl;
        cout << "Make sure data comparison includes all years, otherwise it will be wrong!" << endl;

        // macro options
        TString in_filename_signal = TString(cfg["infilename_signal"].get<std::string>());
        TString in_filename_VR = TString(cfg["infilename_VR"].get<std::string>());
        TString in_filename_template = TString(cfg["infilename_template"].get<std::string>());
        TString in_filename_SR = TString(cfg["infilename_SR"].get<std::string>());
        if (in_filename_signal == TString("") || in_filename_template == TString("") || in_filename_VR == TString("") || in_filename_SR == TString("")) {
            cout << "ERROR! Need 4 input filenames in config. Exiting..." << endl;
            return 0;
        }
        TFile * out_file = new TFile("sensitivity_map.root", "RECREATE");
        TFile * in_file_signal = new TFile(in_filename_signal, "READ");
        TFile * in_file_VR = new TFile(in_filename_VR, "READ");
        TFile * in_file_template = new TFile(in_filename_template, "READ");
        TFile * in_file_SR = new TFile(in_filename_SR, "READ");

        json out_json;

        gROOT->SetBatch(kTRUE);

        for (auto && keyAsObj : *in_file_VR->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            if (!canvas_name.Contains("canvas2D")) continue;
            if (!canvas_name.Contains("-DATA")) continue;
            if (!canvas_name.Contains("34") && !canvas_name.Contains("36") && 
                    !canvas_name.Contains("37") && !canvas_name.Contains("38") && 
                    !canvas_name.Contains("39")) continue;

            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;

            // Get TH2D
            TString h_name = canvas_name;
            h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
            cout << h_name << endl;
            TCanvas * c_VR = (TCanvas*)in_file_VR->Get(canvas_name);
            TCanvas * c_SR = (TCanvas*)in_file_SR->Get(canvas_name);
            TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
            TH2D * h_SR = (TH2D*)c_SR->FindObject(h_name);

            TH1D * template_px = (TH1D*)in_file_template->Get(h_name + TString("_px"));
            TH1D * template_py = (TH1D*)in_file_template->Get(h_name + TString("_py"));

            template_px->Scale(1/template_px->Integral());
            template_py->Scale(1/template_py->Integral());

            canvas_name.ReplaceAll("-DATA", "_sig_");
            cout << canvas_name << endl;
            for (auto && keyAsObj2 : *in_file_signal->GetListOfKeys()) {
                auto key2 = (TKey*)keyAsObj2;
                if (TString(key2->GetClassName()) != "TCanvas") continue;
                if (!TString(key2->GetName()).Contains(canvas_name)) continue;

                TCanvas * c_sig = (TCanvas*)in_file_signal->Get(key2->GetName());

                TString h_sig_name = TString(key2->GetName());
                h_sig_name.Remove(0, 9);
                TH2D * h_sig = (TH2D*)c_sig->FindObject(h_sig_name);
                TH2D * h_map_sig = (TH2D*)h_sig->Clone();
                h_map_sig->Reset();
                h_map_sig->SetName("SensivityMap_" + h_sig_name);

                float max_ZA_sig = -1.0;
                float max_x_sig, max_y_sig;

                float max_A_sig, max_B_sig, max_C_sig, max_D_sig;
                float max_A_SR, max_B_SR_pred, max_C_SR_pred, max_D_SR_pred;

                for (int i = 0; i <= h_VR->GetNbinsX()+1; i++) {
                    for (int j = 0; j <= h_VR->GetNbinsY()+1; j++) {
                        float A_SR = h_SR->Integral(0, i-1, 0, j-1);
                        // Template method
                        float c1 = template_px->Integral(i, template_px->GetNbinsX()+1);
                        float c2 = template_py->Integral(j, template_py->GetNbinsX()+1);
                        float B_SR_pred = A_SR * c1;
                        float C_SR_pred = A_SR * c2;
                        float D_SR_pred = A_SR * c1 * c2;

                        float C_SR_clos_err = C_SR_pred * 0.14;
                        float C_SR_err = C_SR_clos_err;

                        float rescale_years = 2.30;
                        float A_sig = h_sig->Integral(0, i-1, 0, j-1);
                        float B_sig = h_sig->Integral(i, h_sig->GetNbinsX()+1, 0, j-1);
                        float C_sig = h_sig->Integral(0, i-1, j, h_sig->GetNbinsY()+1);
                        float D_sig = h_sig->Integral(i, h_sig->GetNbinsX()+1, j, h_sig->GetNbinsY()+1);
                        A_sig *= rescale_years;
                        B_sig *= rescale_years;
                        C_sig *= rescale_years;
                        D_sig *= rescale_years;

                        auto calc_ZA = [&](float s, float b, float b_err) {
                            return sqrt(2.0*((s+b)*log((s+b)*(b+b_err*b_err)/(b*b+(s+b)*b_err*b_err)) - (b*b/(b_err*b_err))*log(1+(b_err*b_err*s)/(b*(b+b_err*b_err)))));
                        };

                        float ZA_sig = calc_ZA(C_sig, C_SR_pred, C_SR_err);
                        h_map_sig->SetBinContent(i, j, ZA_sig);

                        if (ZA_sig > max_ZA_sig) {
                            max_ZA_sig = ZA_sig;
                            max_x_sig = h_map_sig->GetXaxis()->GetBinCenter(i);
                            max_y_sig = h_map_sig->GetYaxis()->GetBinCenter(j);

                            max_A_sig = A_sig;
                            max_B_sig = B_sig;
                            max_C_sig = C_sig;
                            max_D_sig = D_sig;

                            max_A_SR = A_SR;
                            max_B_SR_pred = B_SR_pred;
                            max_C_SR_pred = C_SR_pred;
                            max_D_SR_pred = D_SR_pred;
                        }
                    }
                }

                cout << "Maximum sign = " << max_ZA_sig << endl;
                cout << "For x, y = " << max_x_sig << ", " << max_y_sig << endl;
                cout << "Signal MC (52p5, 100 mm) A, B, C, D = " << max_A_sig << ", " << max_B_sig << ", " << max_C_sig << ", " << max_D_sig << endl;
                cout << "A in SR = " << max_A_SR << endl;
                cout << "B, C, D prediction in SR = " << max_B_SR_pred << ", " << max_C_SR_pred << ", " << max_D_SR_pred << endl;

                out_json[h_sig_name.Data()] = {
                    {"A_bkg", max_A_SR},
                    {"B_bkg", max_B_SR_pred},
                    {"C_bkg", max_C_SR_pred},
                    {"D_bkg", max_D_SR_pred},
                    {"A_sig", max_A_sig},
                    {"B_sig", max_B_sig},
                    {"C_sig", max_C_sig},
                    {"D_sig", max_D_sig},
                    {"x_edge", max_x_sig},
                    {"y_edge", max_y_sig},
                    {"ZA_signif", max_ZA_sig}
                };

                out_file->cd();
                h_map_sig->Write();
            }


            //h_name = canvas_name;
            //h_name.Remove(0, 9);
            //TCanvas * c_sig1 = (TCanvas*)in_file_signal->Get(canvas_name);
            //TH2D * h_sig1 = (TH2D*)c_sig1->FindObject(h_name);
            //TH2D * h_map_sig1 = (TH2D*)h_sig1->Clone();
            //h_map_sig1->Reset();
            //h_map_sig1->SetName("SensitivityMap_" + h_name);

            //canvas_name.ReplaceAll("52p5_100", "6p0_10");
            //h_name = canvas_name;
            //h_name.Remove(0, 9);
            //TCanvas * c_sig2 = (TCanvas*)in_file_signal->Get(canvas_name);
            //TH2D * h_sig2 = (TH2D*)c_sig2->FindObject(h_name);
            //TH2D * h_map_sig2 = (TH2D*)h_sig2->Clone();
            //h_map_sig2->Reset();
            //h_map_sig2->SetName("SensitivityMap_" + h_name);

            //canvas_name.ReplaceAll("6p0_10", "60p0_1");
            //h_name = canvas_name;
            //h_name.Remove(0, 9);
            //TCanvas * c_sig3 = (TCanvas*)in_file_signal->Get(canvas_name);
            //TH2D * h_sig3 = (TH2D*)c_sig3->FindObject(h_name);
            //TH2D * h_map_sig3 = (TH2D*)h_sig3->Clone();
            //h_map_sig3->Reset();
            //h_map_sig3->SetName("SensitivityMap_" + h_name);

            //float max_ZA_sig1 = -1.0;
            //float max_x_sig1;
            //float max_y_sig1;

            //float max_A_sig1;
            //float max_B_sig1;
            //float max_C_sig1;
            //float max_D_sig1;

            //float max_A_SR;
            //float max_B_SR_pred;
            //float max_C_SR_pred;
            //float max_D_SR_pred;
            //float sf;

            //for (int i = 0; i <= h_VR->GetNbinsX()+1; i++) {
            //    //if (h_VR->GetXaxis()->GetBinCenter(i) < 0.2) continue;

            //    for (int j = 0; j <= h_VR->GetNbinsY()+1; j++) {
            //        //if (h_VR->GetYaxis()->GetBinCenter(j) < 20) continue;

            //        float A_SR = h_SR->Integral(0, i-1, 0, j-1);
            //        // Template method
            //        float c1 = template_px->Integral(i, template_px->GetNbinsX()+1);
            //        float c2 = template_py->Integral(j, template_py->GetNbinsX()+1);
            //        float B_SR_pred = A_SR * c1;
            //        float C_SR_pred = A_SR * c2;
            //        float D_SR_pred = A_SR * c1 * c2;
            //        
            //        float A_VR = h_VR->Integral(0, i-1, 0, j-1);
            //        float B_VR = h_VR->Integral(i, h_VR->GetNbinsX()+1, 0, j-1);
            //        float C_VR = h_VR->Integral(0, i-1, j, h_VR->GetNbinsY()+1);
            //        float D_VR = h_VR->Integral(i, h_VR->GetNbinsX()+1, j, h_VR->GetNbinsY()+1);
            //        
            //        // Ratio of SR/VR total yield method
            //        float total_VR = h_VR->Integral(0, h_VR->GetNbinsX()+1, 0, h_VR->GetNbinsY()+1);
            //        float total_SR = h_SR->Integral(0, h_SR->GetNbinsX()+1, 0, h_SR->GetNbinsY()+1);
            //        sf = total_SR / total_VR;
            //        //float B_SR_pred = B_VR * sf; //B_SR / B_VR;
            //        //float C_SR_pred = C_VR * sf; //B_SR / B_VR;
            //        //float D_SR_pred = D_VR * sf; //B_SR / B_VR;

            //        float C_SR_norm_err = C_SR_pred * sqrt(1/C_VR + 1/total_SR + 1/total_VR); // 1/B_SR + 1/B_VR);
            //        float C_SR_clos_err = C_SR_pred * 0.14;
            //        float C_SR_err = C_SR_clos_err;
            //        //float C_SR_err = sqrt(pow(C_SR_norm_err,2) + pow(C_SR_clos_err,2));

            //        float rescale = 2.30;
            //        float A_sig1 = rescale * h_sig1->Integral(0, i-1, 0, j-1);
            //        float B_sig1 = rescale * h_sig1->Integral(i, h_sig1->GetNbinsX()+1, 0, j-1);
            //        float C_sig1 = rescale * h_sig1->Integral(0, i-1, j, h_sig1->GetNbinsY()+1);
            //        float D_sig1 = rescale * h_sig1->Integral(i, h_sig1->GetNbinsX()+1, j, h_sig1->GetNbinsY()+1);
            //        float C_sig2 = rescale * h_sig2->Integral(0, i-1, j, h_sig2->GetNbinsY()+1);
            //        float C_sig3 = rescale * h_sig3->Integral(0, i-1, j, h_sig3->GetNbinsY()+1);

            //        auto calc_ZA = [&](float s, float b, float b_err) {
            //            return sqrt(2.0*((s+b)*log((s+b)*(b+b_err*b_err)/(b*b+(s+b)*b_err*b_err)) - (b*b/(b_err*b_err))*log(1+(b_err*b_err*s)/(b*(b+b_err*b_err)))));
            //        };
            //        auto calc_ZA_noerr = [&](float s, float b) {
            //            return sqrt(2.0*(s+b)*log(s/b + 1) - s);
            //        };
            //            
            //        float ZA_sig1 = calc_ZA(C_sig1, C_SR_pred, C_SR_err);
            //        float ZA_sig2 = calc_ZA(C_sig2, C_SR_pred, C_SR_err);
            //        float ZA_sig3 = calc_ZA(C_sig3, C_SR_pred, C_SR_err);
            //        //float ZA_sig1 = calc_ZA_noerr(C_sig1, C_SR_pred);
            //        //float ZA_sig2 = calc_ZA_noerr(C_sig2, C_SR_pred);
            //        //float ZA_sig3 = calc_ZA_noerr(C_sig3, C_SR_pred);

            //        h_map_sig1->SetBinContent(i, j, ZA_sig1);
            //        h_map_sig2->SetBinContent(i, j, ZA_sig2);
            //        h_map_sig3->SetBinContent(i, j, ZA_sig3);
            //        
            //        if (ZA_sig1 > max_ZA_sig1) {
            //            max_ZA_sig1 = ZA_sig1;
            //            max_x_sig1 = h_map_sig1->GetXaxis()->GetBinCenter(i);
            //            max_y_sig1 = h_map_sig1->GetYaxis()->GetBinCenter(j);

            //            max_A_sig1 = A_sig1;
            //            max_B_sig1 = B_sig1;
            //            max_C_sig1 = C_sig1;
            //            max_D_sig1 = D_sig1;

            //            max_A_SR = A_SR;
            //            max_B_SR_pred = B_SR_pred;
            //            max_C_SR_pred = C_SR_pred;
            //            max_D_SR_pred = D_SR_pred;
            //        }
            //    }
            //}

            //cout << "Maximum sign = " << max_ZA_sig1 << endl;
            //cout << "scale factor SR_total / VR_total = " << sf << endl;
            //cout << "For x, y = " << max_x_sig1 << ", " << max_y_sig1 << endl;
            //cout << "Signal MC (52p5, 100 mm) A, B, C, D = " << max_A_sig1 << ", " << max_B_sig1 << ", " << max_C_sig1 << ", " << max_D_sig1 << endl;
            //cout << "A in SR = " << max_A_SR << endl;
            //cout << "B, C, D prediction in SR = " << max_B_SR_pred << ", " << max_C_SR_pred << ", " << max_D_SR_pred << endl;

        }

        gROOT->SetBatch(kFALSE);

        std::ofstream out_json_file("data_for_combine.json");
        out_json_file << std::setw(4) << out_json << std::endl;
        out_json_file.close();

        in_file_signal->Close();
        in_file_VR->Close();
        in_file_SR->Close();
        out_file->Close();
        return 0;
    }
} // namespace macro
