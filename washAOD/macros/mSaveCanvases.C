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
	    TString year;
	    std::cout<<"name: "<< canvas_name<<std::endl;
	    if      (canvas_name.Contains("161718")) {year = "_161718";}
	    else if (canvas_name.Contains("1718")) {year = "_1718";}
	    else if (canvas_name.Contains("1618")) {year = "_1618";}
	    else if (canvas_name.Contains("1617")) {year = "_1617";}
	    else if (canvas_name.Contains("2017")) {year = "_2017";}
	    else if (canvas_name.Contains("2018")) {year = "_2018";}
	    else if (canvas_name.Contains("2016")) {year = "_2016";}
	    else{continue;}
	    std::cout<<"year: "<< year.Data()<<std::endl;
            while (canvas_name.Tokenize(tok, from, "_cut")) {
                if (tok.Contains("canvas2D")) {
                    if (canvas_name.Contains("DATA"))
                        canvas_subdir = Form("%s_DATA", tok.Data());
                    else if (canvas_name.Contains("BKG"))
                        canvas_subdir = Form("%s_BKG", tok.Data());
                    else if (canvas_name.Contains("SIGNAL"))
                        canvas_subdir = Form("%s_SIGNAL", tok.Data());
                }
                else if (tok.Contains("canvas"))
                    canvas_subdir = tok;
                else
                    cut_num = tok.Atoi();
            }
	    canvas_subdir.Append(year);
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
