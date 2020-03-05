#include "mMakeTemplates.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename_VR = TString(cfg["infilename_VR"].get<std::string>());
        if (in_filename_VR == TString("")) {
            cout << "ERROR! Need an input filename in config. Exiting..." << endl;
            return 0;
        }
        TFile * out_file = new TFile("ABCD_template.root", "RECREATE");
        TFile * in_file_VR = new TFile(in_filename_VR, "READ");

        gROOT->SetBatch(kTRUE);

        for (auto && keyAsObj : *in_file_VR->GetListOfKeys()) {
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;

            if (canvas_name.Contains("canvas2D")) {
                if (canvas_name.Contains("-DATA")) {
                    // Get TH2D
                    TString h_name = canvas_name;
                    h_name.Remove(0, 9); // remove "Canvas2D_" to get TH2D name
                    cout << h_name << endl;
                    TCanvas * c_VR = (TCanvas*)in_file_VR->Get(canvas_name);
                    TH2D * h_VR = (TH2D*)c_VR->FindObject(h_name);
                    
                    TH1D * h_VR_px = (TH1D*)h_VR->ProjectionX();
                    TH1D * h_VR_py = (TH1D*)h_VR->ProjectionY();

                    //h_VR_px->Scale(1.0/h_VR_px->Integral(0, h_VR_px->GetNbinsX()+1));
                    //h_VR_py->Scale(1.0/h_VR_py->Integral(0, h_VR_py->GetNbinsX()+1));

                    TGraph * g_VR_px = new TGraph(h_VR_px);
                    TGraph * g_VR_py = new TGraph(h_VR_py);

                    TSpline3 * spl_VR_px = new TSpline3(g_VR_px->GetName() + TString("_spline"), g_VR_px);
                    TSpline3 * spl_VR_py = new TSpline3(g_VR_py->GetName() + TString("_spline"), g_VR_py);

                    out_file->cd();
                    h_VR_px->Write();
                    h_VR_py->Write();
                    spl_VR_px->Write();
                    spl_VR_py->Write();
                }
            }
        }

        gROOT->SetBatch(kFALSE);

        in_file_VR->Close();
        out_file->Close();
        return 0;
    }
} // namespace macro
