#include "mMake2DPlotsFromFile.h"
#include <TH2D.h>

//TString lumi_13TeV = "59.74 fb^{-1} ";

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();

        // for this macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! mMake2DPlotsFromFile requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }
        
        // macro options
        TString in_filename = TString(cfg["infilename"].get<std::string>());
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TString out_filename;
        if (cfg.find("outfilename") != cfg.end()) 
            out_filename = TString(cfg["outfilename"].get<std::string>());
        else 
            out_filename = in_filename;

        TFile * in_file, * out_file;
        if (out_filename == "" || out_filename == in_filename) {
            in_file = new TFile(in_filename, "UPDATE");
            out_file = in_file;
        }
        else {
            in_file = new TFile(in_filename);
            out_file = new TFile(out_filename, "RECREATE");
        }
        
        map<TString, std::unique_ptr<TCanvas>> canvases;
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "THStack") continue;
            TString hs_name = TString(key->GetName());
            if (!hs_name.Contains("_vs_")) continue; // mMake1DPlotsFromFile handles these
            cout << "Processing " << hs_name << ", class " << key->GetClassName() << endl;
            THStack * hs = (THStack*)in_file->Get(hs_name);
            TString hs_basename = ((TObjString*)(hs_name.Tokenize("-")->At(0)))->String();
            TString hs_suffix = ((TObjString*)(hs_name.Tokenize("-")->At(1)))->String();
            canvases[hs_name] = std::make_unique<TCanvas>(Form("canvas2D_%s", hs_name.Data()));
            auto * c = canvases[hs_name].get();
            //c->Range(0,0,1,1);
            c->cd();
            gPad->Draw(); 
            gPad->cd();
            //gPad->SetTopMargin(0.1);
            //gPad->SetBottomMargin(0.1);
            //gPad->SetRightMargin(0.1);
            //gPad->SetFillStyle(0);
            TH2D * hsum = (TH2D*)(hs->GetStack()->Last()->Clone()); // Only draw the sum of the stack for 2D
            hsum->Draw("COLZ");
            hsum->SetTitle("");
            TString title = hs->GetTitle();
            Ssiz_t from = 0;
            TString tok;
            if (title.Tokenize(tok, from, " vs "))
                hsum->GetXaxis()->SetTitle(tok);
            if (title.Tokenize(tok, from, " vs "))
                hsum->GetYaxis()->SetTitle(tok);
            //hs->GetXaxis()->SetTitle(hs->GetTitle());
            //hs->GetYaxis()->SetTitle();
            //hs->GetXaxis()->SetTitleSize(0.00);
            //hs->GetHistogram()->GetYaxis()->SetLabelSize(0.05);
            //hs->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
            hsum->GetYaxis()->SetTitleOffset(1.3);
            //hs->SetTitle("");
            //hs->GetHistogram()->SetLabelSize(0.0);
            const bool writeExtraText = true;
            //lumi_13TeV = "29.41 fb^{-1} ";
            //CMS_lumi(c, 4);
            //c->SetLogy();
            CMS_lumi((TPad*)gPad, 4, 0, cfg["year"].get<int>());
            gPad->SetLogz();
            gPad->Modified();
            gPad->Update();
            // Make cut description label
            int cut;
            //TString tok;
            //Ssiz_t from = 0;
            from = 0;
            while (hs_basename.Tokenize(tok, from, "_")) 
              if (tok.Contains("cut")) 
                  cut = (((TObjString*)(tok.Tokenize("t")->At(1)))->String()).Atoi();
            TLatex cut_label;
            cut_label.SetNDC();
            cut_label.SetTextSize(0.04);

            if (cuts_info[cut].special == TString("yes"))
                cut_label.SetTextColor(kRed);

            cut_label.DrawLatexNDC(0.25, 0.90, cuts_info[cut].description.Data()); //common::cut_descriptions[cut].c_str());
            //hs->GetStack()->Last()->Draw("E");
            //canvases.push_back(std::move(c));
        }
        // Save canvases
        for (auto & pair : canvases) {
            pair.second.get()->Write();
            //delete c;
        }

        //out_file->Write();
        out_file->Close();
        if (out_file != in_file)
            in_file->Close();

        return 0;
    }
}
