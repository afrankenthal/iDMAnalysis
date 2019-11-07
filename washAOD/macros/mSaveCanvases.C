#include "mSaveCanvases.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // macro options
        TString in_filename = TString(cfg["infilename"].get<std::string>());
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TFile * in_file = new TFile(in_filename, "READ");

        TString out_dir;
        // Use output dir if provided
        if (cfg.find("outdir") != cfg.end() && cfg["outdir"] != std::string("")) 
            out_dir = TString(cfg["outdir"].get<std::string>());
        // or just strip the filename's extension and use it as dir
        else 
            out_dir = Form("../plots/%s", in_filename.ReplaceAll(".root", "").Data());

        fs::path dir(out_dir.Data());
        if (fs::exists(dir)) {
            cout << "Error! Output directory " << out_dir.Data() << " already exists! Not overwriting." << endl;
            return 0;
        }
        fs::create_directories(dir);

        gROOT->SetBatch(kTRUE);
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "TCanvas") continue;
            TString canvas_name = TString(key->GetName());
            cout << "Processing " << canvas_name << ", class " << key->GetClassName() << endl;
            TString tok;
            Ssiz_t from = 0;
            TString canvas_subdir;
            int cut_num = -1;
            while (canvas_name.Tokenize(tok, from, "_cut")) {
                if (tok.Contains("canvas"))
                    canvas_subdir = tok;
                else
                    cut_num = tok.Atoi();
            }
            fs::create_directories(Form("%s/%s", out_dir.Data(), canvas_subdir.Data()));
            TCanvas * c = (TCanvas*)in_file->Get(canvas_name);
            c->cd();
            c->SaveAs(Form("%s/%s/%s_cut%02d.pdf", out_dir.Data(), canvas_subdir.Data(), canvas_subdir.Data(), cut_num));
            TImage *img = TImage::Create();
            img->FromPad(c);
            img->WriteImage(Form("%s/%s/%s_cut%02d.png", out_dir.Data(), canvas_subdir.Data(), canvas_subdir.Data(), cut_num));
            //c->SaveAs(Form("%s/%s/%s_cut%02d.png", out_dir.Data(), canvas_subdir.Data(), canvas_subdir.Data(), cut_num));
            c->SaveAs(Form("%s/%s/%s_cut%02d.root", out_dir.Data(), canvas_subdir.Data(), canvas_subdir.Data(), cut_num));
            //c->SaveAs(Form("%s/%s.C", out_dir.Data(), canvas_name.Data())); // FIXME Getting error on this one
        }
        gROOT->SetBatch(kFALSE);

        in_file->Close();

        return 0;
    }
} // namespace macro
