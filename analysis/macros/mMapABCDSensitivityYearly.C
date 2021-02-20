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
#include <TH1D.h>
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

        auto calc_ZA = [&](float s, float b, float b_err) {
            return sqrt(2.0*((s+b)*log((s+b)*(b+b_err*b_err)/(b*b+(s+b)*b_err*b_err)) - (b*b/(b_err*b_err))*log(1+(b_err*b_err*s)/(b*(b+b_err*b_err)))));
        };

        // macro options
        TString in_filename_signal_161718 = TString(cfg["infilename_signal_161718"].get<std::string>());
        TString in_filename_VR_161718 = TString(cfg["infilename_VR_161718"].get<std::string>());
        TString in_filename_VR_data_161718 = TString(cfg["infilename_VR_data_161718"].get<std::string>());
        TString in_filename_SR_161718 = TString(cfg["infilename_SR_161718"].get<std::string>());
        TString in_filename_signal_2016 = TString(cfg["infilename_signal_2016"].get<std::string>());
        TString in_filename_VR_2016 = TString(cfg["infilename_VR_2016"].get<std::string>());
        TString in_filename_VR_data_2016 = TString(cfg["infilename_VR_data_2016"].get<std::string>());
        TString in_filename_SR_2016 = TString(cfg["infilename_SR_2016"].get<std::string>());
        TString in_filename_signal_2017 = TString(cfg["infilename_signal_2017"].get<std::string>());
        TString in_filename_VR_2017 = TString(cfg["infilename_VR_2017"].get<std::string>());
        TString in_filename_VR_data_2017 = TString(cfg["infilename_VR_data_2017"].get<std::string>());
        TString in_filename_SR_2017 = TString(cfg["infilename_SR_2017"].get<std::string>());
        TString in_filename_signal_2018 = TString(cfg["infilename_signal_2018"].get<std::string>());
        TString in_filename_VR_2018 = TString(cfg["infilename_VR_2018"].get<std::string>());
        TString in_filename_VR_data_2018 = TString(cfg["infilename_VR_data_2018"].get<std::string>());
        TString in_filename_SR_2018 = TString(cfg["infilename_SR_2018"].get<std::string>());
        TString in_filename_template = TString(cfg["infilename_template"].get<std::string>());

        std::vector<std::vector<double>> vxy_bin_edges;
        if (cfg.find("vxy_bin_edges_cut28") != cfg.end()) {
            vxy_bin_edges.push_back(cfg["vxy_bin_edges_cut28"].get<std::vector<double>>());
            vxy_bin_edges.push_back(cfg["vxy_bin_edges_cut29"].get<std::vector<double>>());
            vxy_bin_edges.push_back(cfg["vxy_bin_edges_cut30"].get<std::vector<double>>());
        }
        std::vector<double> dphi_bin_edge;
        if (cfg.find("dphi_bin_edge_cut28") != cfg.end()) {
            dphi_bin_edge.push_back(cfg["dphi_bin_edge_cut28"].get<double>());
            dphi_bin_edge.push_back(cfg["dphi_bin_edge_cut29"].get<double>());
            dphi_bin_edge.push_back(cfg["dphi_bin_edge_cut30"].get<double>());
        }

        //TString in_filename_template = TString(cfg["infilename_template"].get<std::string>());
        //if (in_filename_signal_2016 == TString("") || in_filename_template == TString("") || in_filename_VR == TString("") || in_filename_SR == TString("")) {
        //    cout << "ERROR! Need 4 input filenames in config. Exiting..." << endl;
        //    return 0;
        //}
        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == TString(""))
            out_filename = "sensitivity_map.root";

        TFile * out_file = new TFile(out_filename, "RECREATE");
        TFile * in_file_signal_161718 = new TFile(in_filename_signal_161718, "READ");
        TFile * in_file_VR_161718 = new TFile(in_filename_VR_161718, "READ");
        TFile * in_file_VR_data_161718 = new TFile(in_filename_VR_data_161718, "READ");
        TFile * in_file_SR_161718 = new TFile(in_filename_SR_161718, "READ");
        TFile * in_file_signal_2016 = new TFile(in_filename_signal_2016, "READ");
        TFile * in_file_VR_2016 = new TFile(in_filename_VR_2016, "READ");
        TFile * in_file_VR_data_2016 = new TFile(in_filename_VR_data_2016, "READ");
        TFile * in_file_SR_2016 = new TFile(in_filename_SR_2016, "READ");
        TFile * in_file_signal_2017 = new TFile(in_filename_signal_2017, "READ");
        TFile * in_file_VR_2017 = new TFile(in_filename_VR_2017, "READ");
        TFile * in_file_VR_data_2017 = new TFile(in_filename_VR_data_2017, "READ");
        TFile * in_file_SR_2017 = new TFile(in_filename_SR_2017, "READ");
        TFile * in_file_signal_2018 = new TFile(in_filename_signal_2018, "READ");
        TFile * in_file_VR_2018 = new TFile(in_filename_VR_2018, "READ");
        TFile * in_file_VR_data_2018 = new TFile(in_filename_VR_data_2018, "READ");
        TFile * in_file_SR_2018 = new TFile(in_filename_SR_2018, "READ");
        TFile * in_file_template = new TFile(in_filename_template, "READ");

        std::vector<std::string> cuts_to_keep = cfg["cuts_to_keep"].get<std::vector<std::string>>();
        
        //float rescale_years = 2.30;
        float rescale_years = 1.00;
        if (cfg.find("rescale_years") != cfg.end())
                rescale_years = cfg["rescale_years"].get<float>();

        if (fabs(rescale_years - 1.00) > 0.1) {
            cout << "Warning: re-scaling signal yields from 2018 to 2016+2017+2018 (factor of x2.30)" << endl;
            cout << "Make sure data comparison includes all years, otherwise it will be wrong!" << endl;
        }

        json out_json;

        gROOT->SetBatch(kTRUE);

        for (auto && keyAsObj : *in_file_VR_161718->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            if (!canvas_name.Contains("canvas2D")) continue;
            //if (!canvas_name.Contains("-DATA")) continue;
            if (!canvas_name.Contains("-BKG")) continue;

            bool cut_contained = false;
            for (auto cut : cuts_to_keep)
                cut_contained = cut_contained || canvas_name.Contains(TString(cut.c_str()));
            if (!cut_contained)
                continue;
                
            //if (!canvas_name.Contains("34") && !canvas_name.Contains("36") && 
            //        !canvas_name.Contains("37") && !canvas_name.Contains("38") && 
            //        !canvas_name.Contains("39")) continue;

            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl << endl;

            // Get TH2D
            TString h_name = canvas_name;
            h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
            TCanvas * c_VR = (TCanvas*)in_file_VR_161718->Get(canvas_name);
            TCanvas * c_SR = (TCanvas*)in_file_SR_161718->Get(canvas_name);
            TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
            TH2D * h_SR = (TH2D*)c_SR->FindObject(h_name);

            h_name.ReplaceAll("-BKG", "-DATA");
            TH1D * template_px = (TH1D*)in_file_template->Get(h_name + TString("_px"));
            TH1D * template_py = (TH1D*)in_file_template->Get(h_name + TString("_py"));

            canvas_name.ReplaceAll("-BKG", "-DATA");
            TCanvas * c_VR_data = (TCanvas*)in_file_VR_data_161718->Get(canvas_name);
            TH2D * h_VR_data = (TH2D*)c_VR_data->FindObject(h_name);

            if (template_px->Integral(0, -1) != template_py->Integral(0, -1))
                cout << "ERROR! X and Y templates have different number of events in nJets data: " 
                     << template_px->Integral(0, -1) << " and " << template_py->Integral(0, -1) << endl;

            int data_nJets_N = template_px->Integral(0, -1);

            template_px->Scale(1/template_px->Integral(0, template_px->GetNbinsX()+1));
            template_py->Scale(1/template_py->Integral(0, template_py->GetNbinsX()+1));

            canvas_name.ReplaceAll("-DATA", "_sig_");
            for (auto && keyAsObj2 : *in_file_signal_161718->GetListOfKeys()) {
                auto key2 = (TKey*)keyAsObj2;
                if (TString(key2->GetClassName()) != "TCanvas") continue;
                if (!TString(key2->GetName()).Contains(canvas_name)) continue;

                cout << "Signal histogram: " << key2->GetName() << endl;

                TCanvas * c_sig = (TCanvas*)in_file_signal_161718->Get(key2->GetName());

                TString h_sig_name = TString(key2->GetName());
                h_sig_name.Remove(0, 9);
                TH2D * h_sig = (TH2D*)c_sig->FindObject(h_sig_name);
                TH2D * h_map_sig = (TH2D*)h_sig->Clone();
                h_map_sig->Reset();
                h_map_sig->SetName("SensitivityMap_" + h_sig_name);

                float max_ZA_sig = -1.0;
                float max_x_sig, max_y_sig;

                float max_A_sig, max_B_sig, max_C_sig, max_D_sig;
                float max_A_SR_pred, max_B_SR_pred, max_C_SR_pred, max_D_SR_pred;

                float MC_norm_ratio = h_SR->Integral(0, -1, 0, -1) / h_VR->Integral(0, -1, 0, -1);

                if (dphi_bin_edge.size() == 0) { // bin edges not provided, figure it out

                    for (int i = 1; i <= h_VR->GetNbinsX()+1; i++) {
                        for (int j = 1; j <= h_VR->GetNbinsY()+1; j++) {
                            // Template method
                            //float A_SR = h_SR->Integral(0, i-1, 0, j-1);
                            //float c1 = template_px->Integral(i, template_px->GetNbinsX()+1)/template_px->Integral(0,i-1);
                            //float c2 = template_py->Integral(j, template_py->GetNbinsX()+1)/template_py->Integral(0,j-1);
                            //float B_SR_pred = A_SR * c1;
                            //float C_SR_pred = A_SR * c2;
                            //float D_SR_pred = A_SR * c1 * c2;
                            
                            // New template method using MC for normalization
                            int nbinsx = template_px->GetNbinsX()+1;
                            int nbinsy = template_px->GetNbinsY()+1;
                            
                            //float A_SR_pred = MC_norm_ratio * data_nJets_N * template_px->Integral(0, i-1) * template_py->Integral(0, j-1);
                            //float B_SR_pred = MC_norm_ratio * data_nJets_N * template_px->Integral(i, nbinsx) * template_py->Integral(0, j-1);
                            //float C_SR_pred = MC_norm_ratio * data_nJets_N * template_px->Integral(0, i-1) * template_py->Integral(j, nbinsy);
                            //float D_SR_pred = MC_norm_ratio * data_nJets_N * template_px->Integral(i, nbinsx) * template_py->Integral(j, nbinsy);

                            // Test using ABCD yields directly from nJets VR, no template
                            float A_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, i-1, 0, j-1);
                            float B_SR_pred = MC_norm_ratio * h_VR_data->Integral(i, -1, 0, j-1);
                            float C_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, i-1, j, -1);
                            float D_SR_pred = MC_norm_ratio * h_VR_data->Integral(i, -1, j, -1);

                            if (A_SR_pred < 1e-3 || B_SR_pred < 1e-3 || C_SR_pred < 1e-3 || D_SR_pred < 1e-3)
                                continue;

                            float C_SR_clos_err = C_SR_pred * 0.14; // average closure error
                            float C_SR_stat_err = sqrt(C_SR_pred);
                            
                            //float C_SR_err = C_SR_clos_err;
                            float C_SR_err = C_SR_stat_err;

                            float A_sig = h_sig->Integral(0, i-1, 0, j-1);
                            float B_sig = h_sig->Integral(i, h_sig->GetNbinsX()+1, 0, j-1);
                            float C_sig = h_sig->Integral(0, i-1, j, h_sig->GetNbinsY()+1);
                            float D_sig = h_sig->Integral(i, h_sig->GetNbinsX()+1, j, h_sig->GetNbinsY()+1);
                            A_sig *= rescale_years;
                            B_sig *= rescale_years;
                            C_sig *= rescale_years;
                            D_sig *= rescale_years;

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

                                max_A_SR_pred = A_SR_pred;
                                max_B_SR_pred = B_SR_pred;
                                max_C_SR_pred = C_SR_pred;
                                max_D_SR_pred = D_SR_pred;
                            }
                        }
                    }
                }
                else { // bin edges provided, just use them

                    TString name = h_sig_name;
                    
                    int cut_idx = -1;
                    if (name.Contains("cut29"))
                            cut_idx = 0;
                    else if (name.Contains("cut30"))
                            cut_idx = 1;
                    else if (name.Contains("cut31"))
                            cut_idx = 2;

                    float current_vxy_bin_edge = -1.0;
                    if (name.EndsWith("_1"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][0];
                    else if (name.EndsWith("_10"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][1];
                    else if (name.EndsWith("_100"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][2];
                    else if (name.EndsWith("_1000"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][3];

                    int biny = h_VR_data->GetYaxis()->FindBin(current_vxy_bin_edge);
                    int binx = h_VR_data->GetXaxis()->FindBin(dphi_bin_edge[cut_idx]);
                    
                    // Test using ABCD yields directly from nJets VR, no template
                    float A_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, binx-1, 0, biny-1);
                    float B_SR_pred = MC_norm_ratio * h_VR_data->Integral(binx, -1, 0, biny-1);
                    float C_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, binx-1, biny, -1);
                    float D_SR_pred = MC_norm_ratio * h_VR_data->Integral(binx, -1, biny, -1);

                    float C_SR_clos_err = C_SR_pred * 0.14; // average closure error
                    float C_SR_stat_err = sqrt(C_SR_pred);

                    //float C_SR_err = C_SR_clos_err;
                    float C_SR_err = C_SR_stat_err;

                    float A_sig = h_sig->Integral(0, binx-1, 0, biny-1);
                    float B_sig = h_sig->Integral(binx, -1, 0, biny-1);
                    float C_sig = h_sig->Integral(0, binx-1, biny, -1);
                    float D_sig = h_sig->Integral(binx, -1, biny, -1);
                    A_sig *= rescale_years;
                    B_sig *= rescale_years;
                    C_sig *= rescale_years;
                    D_sig *= rescale_years;

                    float ZA_sig = calc_ZA(C_sig, C_SR_pred, C_SR_err);

                    max_ZA_sig = ZA_sig;
                    max_x_sig = h_map_sig->GetXaxis()->GetBinCenter(binx);
                    max_y_sig = h_map_sig->GetYaxis()->GetBinCenter(biny);

                    max_A_sig = A_sig;
                    max_B_sig = B_sig;
                    max_C_sig = C_sig;
                    max_D_sig = D_sig;

                    max_A_SR_pred = A_SR_pred;
                    max_B_SR_pred = B_SR_pred;
                    max_C_SR_pred = C_SR_pred;
                    max_D_SR_pred = D_SR_pred;
                }

                cout << "Year: 161718" << endl;
                cout << "Maximum significance = " << max_ZA_sig << " @ (x, y) = (" << max_x_sig << ", " << max_y_sig << ")" << endl;
                cout << "Signal MC A, B, C, D = " << max_A_sig << ", " << max_B_sig << ", " << max_C_sig << ", " << max_D_sig << endl;
                cout << "Bkg predicted A, B, C, D  = " << max_A_SR_pred << ", " << max_B_SR_pred << ", " << max_C_SR_pred << ", " << max_D_SR_pred << endl << endl;
                //cout << "A observed in SR = " << max_A_SR << endl << endl;

                out_json[TString(h_sig_name + "_161718").Data()] = {
                    {"A_bkg", max_A_SR_pred},
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
        }

        // Now use optimal x_edge and y_edge from combined file to calc A, B, C, D for single years
        // 2016
        for (auto && keyAsObj : *in_file_VR_2016->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            if (!canvas_name.Contains("canvas2D")) continue;
            //if (!canvas_name.Contains("-DATA")) continue;
            if (!canvas_name.Contains("-BKG")) continue;

            bool cut_contained = false;
            for (auto cut : cuts_to_keep)
                cut_contained = cut_contained || canvas_name.Contains(TString(cut.c_str()));
            if (!cut_contained)
                continue;

            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl << endl;

            // Get TH2D
            TString h_name = canvas_name;
            h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
            TCanvas * c_VR = (TCanvas*)in_file_VR_2016->Get(canvas_name);
            TCanvas * c_SR = (TCanvas*)in_file_SR_2016->Get(canvas_name);
            TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
            TH2D * h_SR = (TH2D*)c_SR->FindObject(h_name);

            h_name.ReplaceAll("-BKG", "-DATA");
            TH1D * template_px = (TH1D*)in_file_template->Get(h_name + TString("_px"));
            TH1D * template_py = (TH1D*)in_file_template->Get(h_name + TString("_py"));

            canvas_name.ReplaceAll("-BKG", "-DATA");
            TCanvas * c_VR_data = (TCanvas*)in_file_VR_data_2016->Get(canvas_name);
            TH2D * h_VR_data = (TH2D*)c_VR_data->FindObject(h_name);

            if (template_px->Integral(0, -1) != template_py->Integral(0, -1))
                cout << "ERROR! X and Y templates have different number of events in nJets data: " 
                     << template_px->Integral(0, -1) << " and " << template_py->Integral(0, -1) << endl;

            int data_nJets_N = template_px->Integral(0, -1);

            template_px->Scale(1/template_px->Integral(0, template_px->GetNbinsX()+1));
            template_py->Scale(1/template_py->Integral(0, template_py->GetNbinsX()+1));

            canvas_name.ReplaceAll("-DATA", "_sig_");
            for (auto && keyAsObj2 : *in_file_signal_2016->GetListOfKeys()) {
                auto key2 = (TKey*)keyAsObj2;
                if (TString(key2->GetClassName()) != "TCanvas") continue;
                if (!TString(key2->GetName()).Contains(canvas_name)) continue;

                cout << "Signal histogram: " << key2->GetName() << endl;

                TCanvas * c_sig = (TCanvas*)in_file_signal_2016->Get(key2->GetName());

                TString h_sig_name = TString(key2->GetName());
                h_sig_name.Remove(0, 9);
                TH2D * h_sig = (TH2D*)c_sig->FindObject(h_sig_name);

                int bin_x  = -2, bin_y = -2;
                if (dphi_bin_edge.size() == 0) { // bin edges not provided, use computed ones

                    float x_edge = out_json[TString(h_sig_name + "_161718").Data()]["x_edge"];
                    float y_edge = out_json[TString(h_sig_name + "_161718").Data()]["y_edge"];

                    bin_x = h_VR->GetXaxis()->FindBin(x_edge);
                    bin_y = h_VR->GetYaxis()->FindBin(y_edge);
                }
                else { // bin edges provided, use them

                    TString name = h_sig_name;

                    int cut_idx;
                    if (name.Contains("cut29"))
                            cut_idx = 0;
                    else if (name.Contains("cut30"))
                            cut_idx = 1;
                    else if (name.Contains("cut31"))
                            cut_idx = 2;
                    
                    float current_vxy_bin_edge = -1.0;
                    if (name.EndsWith("_1"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][0];
                    else if (name.EndsWith("_10"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][1];
                    else if (name.EndsWith("_100"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][2];
                    else if (name.EndsWith("_1000"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][3];

                    bin_y = h_VR_data->GetYaxis()->FindBin(current_vxy_bin_edge);
                    bin_x = h_VR_data->GetXaxis()->FindBin(dphi_bin_edge[cut_idx]);
                }

                float MC_norm_ratio = h_SR->Integral(0, -1, 0, -1) / h_VR->Integral(0, -1, 0, -1);
                
                // Test using ABCD yields directly from nJets VR, no template
                float A_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, 0, bin_y-1);
                float B_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, 0, bin_y-1);
                float C_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, bin_y, -1);
                float D_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, bin_y, -1);

                float C_SR_clos_err = C_SR_pred * 0.14; // average closure error
                float C_SR_stat_err = sqrt(C_SR_pred);

                //float C_SR_err = C_SR_clos_err;
                float C_SR_err = C_SR_stat_err;

                float A_sig = h_sig->Integral(0, bin_x-1, 0, bin_y-1);
                float B_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, 0, bin_y-1);
                float C_sig = h_sig->Integral(0, bin_x-1, bin_y, h_sig->GetNbinsY()+1);
                float D_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, bin_y, h_sig->GetNbinsY()+1);
                A_sig *= rescale_years;
                B_sig *= rescale_years;
                C_sig *= rescale_years;
                D_sig *= rescale_years;

                float ZA_sig = calc_ZA(C_sig, C_SR_pred, C_SR_err);

                cout << "Year: 2016" << endl;
                cout << "Significance = " << ZA_sig << " @ (x, y) = (" << h_sig->GetXaxis()->GetBinCenter(bin_x) << ", " << h_sig->GetYaxis()->GetBinCenter(bin_y) << ")" << endl;
                cout << "Signal MC A, B, C, D = " << A_sig << ", " << B_sig << ", " << C_sig << ", " << D_sig << endl;
                cout << "Bkg predicted A, B, C, D  = " << A_SR_pred << ", " << B_SR_pred << ", " << C_SR_pred << ", " << D_SR_pred << endl << endl;
                //cout << "A observed in SR = " << A_SR << endl << endl;

                out_json[TString(h_sig_name + "_2016").Data()] = {
                    {"A_bkg", A_SR_pred},
                    {"B_bkg", B_SR_pred},
                    {"C_bkg", C_SR_pred},
                    {"D_bkg", D_SR_pred},
                    {"A_sig", A_sig},
                    {"B_sig", B_sig},
                    {"C_sig", C_sig},
                    {"D_sig", D_sig},
                    {"x_edge", h_sig->GetXaxis()->GetBinCenter(bin_x)},
                    {"y_edge", h_sig->GetYaxis()->GetBinCenter(bin_y)},
                    {"ZA_signif", ZA_sig}
                };
            }
        } // end 2016
        
        // 2017
        for (auto && keyAsObj : *in_file_VR_2017->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            if (!canvas_name.Contains("canvas2D")) continue;
            //if (!canvas_name.Contains("-DATA")) continue;
            if (!canvas_name.Contains("-BKG")) continue;

            bool cut_contained = false;
            for (auto cut : cuts_to_keep)
                cut_contained = cut_contained || canvas_name.Contains(TString(cut.c_str()));
            if (!cut_contained)
                continue;

            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl << endl;

            // Get TH2D
            TString h_name = canvas_name;
            h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
            TCanvas * c_VR = (TCanvas*)in_file_VR_2017->Get(canvas_name);
            TCanvas * c_SR = (TCanvas*)in_file_SR_2017->Get(canvas_name);
            TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
            TH2D * h_SR = (TH2D*)c_SR->FindObject(h_name);

            h_name.ReplaceAll("-BKG", "-DATA");
            TH1D * template_px = (TH1D*)in_file_template->Get(h_name + TString("_px"));
            TH1D * template_py = (TH1D*)in_file_template->Get(h_name + TString("_py"));

            canvas_name.ReplaceAll("-BKG", "-DATA");
            TCanvas * c_VR_data = (TCanvas*)in_file_VR_data_2017->Get(canvas_name);
            TH2D * h_VR_data = (TH2D*)c_VR_data->FindObject(h_name);

            if (template_px->Integral(0, -1) != template_py->Integral(0, -1))
                cout << "ERROR! X and Y templates have different number of events in nJets data: " 
                     << template_px->Integral(0, -1) << " and " << template_py->Integral(0, -1) << endl;

            int data_nJets_N = template_px->Integral(0, -1);

            template_px->Scale(1/template_px->Integral(0, template_px->GetNbinsX()+1));
            template_py->Scale(1/template_py->Integral(0, template_py->GetNbinsX()+1));

            canvas_name.ReplaceAll("-DATA", "_sig_");
            for (auto && keyAsObj2 : *in_file_signal_2017->GetListOfKeys()) {
                auto key2 = (TKey*)keyAsObj2;
                if (TString(key2->GetClassName()) != "TCanvas") continue;
                if (!TString(key2->GetName()).Contains(canvas_name)) continue;

                cout << "Signal histogram: " << key2->GetName() << endl;

                TCanvas * c_sig = (TCanvas*)in_file_signal_2017->Get(key2->GetName());

                TString h_sig_name = TString(key2->GetName());
                h_sig_name.Remove(0, 9);
                TH2D * h_sig = (TH2D*)c_sig->FindObject(h_sig_name);

                int bin_x  = -2, bin_y = -2;
                if (dphi_bin_edge.size() == 0) { // bin edges not provided, use computed ones

                    float x_edge = out_json[TString(h_sig_name + "_161718").Data()]["x_edge"];
                    float y_edge = out_json[TString(h_sig_name + "_161718").Data()]["y_edge"];

                    bin_x = h_VR->GetXaxis()->FindBin(x_edge);
                    bin_y = h_VR->GetYaxis()->FindBin(y_edge);
                }
                else { // bin edges provided, use them

                    TString name = h_sig_name;

                    int cut_idx;
                    if (name.Contains("cut29"))
                            cut_idx = 0;
                    else if (name.Contains("cut30"))
                            cut_idx = 1;
                    else if (name.Contains("cut31"))
                            cut_idx = 2;

                    float current_vxy_bin_edge = -1.0;
                    if (name.EndsWith("_1"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][0];
                    else if (name.EndsWith("_10"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][1];
                    else if (name.EndsWith("_100"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][2];
                    else if (name.EndsWith("_1000"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][3];

                    bin_y = h_VR_data->GetYaxis()->FindBin(current_vxy_bin_edge);
                    bin_x = h_VR_data->GetXaxis()->FindBin(dphi_bin_edge[cut_idx]);
                }

                float MC_norm_ratio = h_SR->Integral(0, -1, 0, -1) / h_VR->Integral(0, -1, 0, -1);

                // Test using ABCD yields directly from nJets VR, no template
                float A_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, 0, bin_y-1);
                float B_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, 0, bin_y-1);
                float C_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, bin_y, -1);
                float D_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, bin_y, -1);

                float C_SR_clos_err = C_SR_pred * 0.14; // average closure error
                float C_SR_stat_err = sqrt(C_SR_pred);

                //float C_SR_err = C_SR_clos_err;
                float C_SR_err = C_SR_stat_err;

                float A_sig = h_sig->Integral(0, bin_x-1, 0, bin_y-1);
                float B_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, 0, bin_y-1);
                float C_sig = h_sig->Integral(0, bin_x-1, bin_y, h_sig->GetNbinsY()+1);
                float D_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, bin_y, h_sig->GetNbinsY()+1);
                A_sig *= rescale_years;
                B_sig *= rescale_years;
                C_sig *= rescale_years;
                D_sig *= rescale_years;

                float ZA_sig = calc_ZA(C_sig, C_SR_pred, C_SR_err);

                cout << "Year: 2017" << endl;
                cout << "Significance = " << ZA_sig << " @ (x, y) = (" << h_sig->GetXaxis()->GetBinCenter(bin_x) << ", " << h_sig->GetYaxis()->GetBinCenter(bin_y) << ")" << endl;
                cout << "Signal MC A, B, C, D = " << A_sig << ", " << B_sig << ", " << C_sig << ", " << D_sig << endl;
                cout << "Bkg predicted A, B, C, D  = " << A_SR_pred << ", " << B_SR_pred << ", " << C_SR_pred << ", " << D_SR_pred << endl << endl;
                //cout << "A observed in SR = " << A_SR << endl << endl;

                out_json[TString(h_sig_name + "_2017").Data()] = {
                    {"A_bkg", A_SR_pred},
                    {"B_bkg", B_SR_pred},
                    {"C_bkg", C_SR_pred},
                    {"D_bkg", D_SR_pred},
                    {"A_sig", A_sig},
                    {"B_sig", B_sig},
                    {"C_sig", C_sig},
                    {"D_sig", D_sig},
                    {"x_edge", h_sig->GetXaxis()->GetBinCenter(bin_x)},
                    {"y_edge", h_sig->GetYaxis()->GetBinCenter(bin_y)},
                    {"ZA_signif", ZA_sig}
                };
            }
        } // end 2017
        
        // 2018
        for (auto && keyAsObj : *in_file_VR_2018->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            if (!canvas_name.Contains("canvas2D")) continue;
            if (!canvas_name.Contains("-BKG")) continue;
            //if (!canvas_name.Contains("-DATA")) continue;

            bool cut_contained = false;
            for (auto cut : cuts_to_keep)
                cut_contained = cut_contained || canvas_name.Contains(TString(cut.c_str()));
            if (!cut_contained)
                continue;

            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl << endl;

            // Get TH2D
            TString h_name = canvas_name;
            h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
            TCanvas * c_VR = (TCanvas*)in_file_VR_2018->Get(canvas_name);
            TCanvas * c_SR = (TCanvas*)in_file_SR_2018->Get(canvas_name);
            TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
            TH2D * h_SR = (TH2D*)c_SR->FindObject(h_name);

            h_name.ReplaceAll("-BKG", "-DATA");
            TH1D * template_px = (TH1D*)in_file_template->Get(h_name + TString("_px"));
            TH1D * template_py = (TH1D*)in_file_template->Get(h_name + TString("_py"));

            canvas_name.ReplaceAll("-BKG", "-DATA");
            TCanvas * c_VR_data = (TCanvas*)in_file_VR_data_2018->Get(canvas_name);
            TH2D * h_VR_data = (TH2D*)c_VR_data->FindObject(h_name);

            if (template_px->Integral(0, -1) != template_py->Integral(0, -1))
                cout << "ERROR! X and Y templates have different number of events in nJets data: " 
                     << template_px->Integral(0, -1) << " and " << template_py->Integral(0, -1) << endl;

            int data_nJets_N = template_px->Integral(0, -1);

            template_px->Scale(1/template_px->Integral(0, template_px->GetNbinsX()+1));
            template_py->Scale(1/template_py->Integral(0, template_py->GetNbinsX()+1));

            canvas_name.ReplaceAll("-DATA", "_sig_");
            for (auto && keyAsObj2 : *in_file_signal_2018->GetListOfKeys()) {
                auto key2 = (TKey*)keyAsObj2;
                if (TString(key2->GetClassName()) != "TCanvas") continue;
                if (!TString(key2->GetName()).Contains(canvas_name)) continue;

                cout << "Signal histogram: " << key2->GetName() << endl;

                TCanvas * c_sig = (TCanvas*)in_file_signal_2018->Get(key2->GetName());

                TString h_sig_name = TString(key2->GetName());
                h_sig_name.Remove(0, 9);
                TH2D * h_sig = (TH2D*)c_sig->FindObject(h_sig_name);

                int bin_x  = -2, bin_y = -2;
                if (dphi_bin_edge.size() == 0) { // bin edges not provided, use computed ones

                    float x_edge = out_json[TString(h_sig_name + "_161718").Data()]["x_edge"];
                    float y_edge = out_json[TString(h_sig_name + "_161718").Data()]["y_edge"];

                    bin_x = h_VR->GetXaxis()->FindBin(x_edge);
                    bin_y = h_VR->GetYaxis()->FindBin(y_edge);
                }
                else { // bin edges provided, use them

                    TString name = h_sig_name;

                    int cut_idx;
                    if (name.Contains("cut29"))
                            cut_idx = 0;
                    else if (name.Contains("cut30"))
                            cut_idx = 1;
                    else if (name.Contains("cut31"))
                            cut_idx = 2;

                    float current_vxy_bin_edge = -1.0;
                    if (name.EndsWith("_1"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][0];
                    else if (name.EndsWith("_10"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][1];
                    else if (name.EndsWith("_100"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][2];
                    else if (name.EndsWith("_1000"))
                        current_vxy_bin_edge = vxy_bin_edges[cut_idx][3];

                    bin_y = h_VR_data->GetYaxis()->FindBin(current_vxy_bin_edge);
                    bin_x = h_VR_data->GetXaxis()->FindBin(dphi_bin_edge[cut_idx]);
                }

                float MC_norm_ratio = h_SR->Integral(0, -1, 0, -1) / h_VR->Integral(0, -1, 0, -1);

                // Test using ABCD yields directly from nJets VR, no template
                float A_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, 0, bin_y-1);
                float B_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, 0, bin_y-1);
                float C_SR_pred = MC_norm_ratio * h_VR_data->Integral(0, bin_x-1, bin_y, -1);
                float D_SR_pred = MC_norm_ratio * h_VR_data->Integral(bin_x, -1, bin_y, -1);

                float C_SR_clos_err = C_SR_pred * 0.14; // average closure error
                float C_SR_stat_err = sqrt(C_SR_pred);

                //float C_SR_err = C_SR_clos_err;
                float C_SR_err = C_SR_stat_err;

                float A_sig = h_sig->Integral(0, bin_x-1, 0, bin_y-1);
                float B_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, 0, bin_y-1);
                float C_sig = h_sig->Integral(0, bin_x-1, bin_y, h_sig->GetNbinsY()+1);
                float D_sig = h_sig->Integral(bin_x, h_sig->GetNbinsX()+1, bin_y, h_sig->GetNbinsY()+1);
                A_sig *= rescale_years;
                B_sig *= rescale_years;
                C_sig *= rescale_years;
                D_sig *= rescale_years;

                float ZA_sig = calc_ZA(C_sig, C_SR_pred, C_SR_err);

                cout << "Year: 2018" << endl;
                cout << "Significance = " << ZA_sig << " @ (x, y) = (" << h_sig->GetXaxis()->GetBinCenter(bin_x) << ", " << h_sig->GetYaxis()->GetBinCenter(bin_y) << ")" << endl;
                cout << "Signal MC A, B, C, D = " << A_sig << ", " << B_sig << ", " << C_sig << ", " << D_sig << endl;
                cout << "Bkg predicted A, B, C, D  = " << A_SR_pred << ", " << B_SR_pred << ", " << C_SR_pred << ", " << D_SR_pred << endl << endl;
                //cout << "A observed in SR = " << A_SR << endl << endl;

                out_json[TString(h_sig_name + "_2018").Data()] = {
                    {"A_bkg", A_SR_pred},
                    {"B_bkg", B_SR_pred},
                    {"C_bkg", C_SR_pred},
                    {"D_bkg", D_SR_pred},
                    {"A_sig", A_sig},
                    {"B_sig", B_sig},
                    {"C_sig", C_sig},
                    {"D_sig", D_sig},
                    {"x_edge", h_sig->GetXaxis()->GetBinCenter(bin_x)},
                    {"y_edge", h_sig->GetYaxis()->GetBinCenter(bin_y)},
                    {"ZA_signif", ZA_sig}
                };
            }
        } // end 2018

        gROOT->SetBatch(kFALSE);

        std::ofstream out_json_file("data_for_combine.json");
        out_json_file << std::setw(4) << out_json << std::endl;
        out_json_file.close();

        in_file_signal_161718->Close();
        in_file_VR_161718->Close();
        in_file_VR_data_161718->Close();
        in_file_SR_161718->Close();
        in_file_signal_2016->Close();
        in_file_VR_2016->Close();
        in_file_VR_data_2016->Close();
        in_file_SR_2016->Close();
        in_file_signal_2017->Close();
        in_file_VR_2017->Close();
        in_file_VR_data_2017->Close();
        in_file_SR_2017->Close();
        in_file_signal_2018->Close();
        in_file_VR_2018->Close();
        in_file_VR_data_2018->Close();
        in_file_SR_2018->Close();
        in_file_template->Close();
        out_file->Close();

        if (fabs(rescale_years - 1.00) > 0.1) {
            cout << "Warning: re-scaling signal yields from 2018 to 2016+2017+2018 (factor of x2.30)" << endl;
            cout << "Make sure data comparison includes all years, otherwise it will be wrong!" << endl;
        }

        return 0;
    }
} // namespace macro
