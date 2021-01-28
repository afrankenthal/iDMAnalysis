#include "mMake1DPlotsFromFile.h"

namespace macro {

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();
        
        // for this macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! mMake1DPlotsFromFile requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }

        // macro options
        vector<std::string> in_filenames = cfg["infilenames"].get<std::vector<std::string>>();
        if (in_filenames.size() < 1) {
            cout << "ERROR! Need at least 1 input file to process. Exiting..." << endl;
            return 0;
        }
        
        //TString in_filename = TString(cfg["infilenames"].get<std::vector<std::string>>()[0]);
        //if (in_filename == TString("")) {
        //    cout << "ERROR! No input filename. Exiting..." << endl;
        //    return 0;
        //}
        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == TString(""))
            out_filename = in_filenames[0];

        TFile * in_file, * out_file;
        if (out_filename == in_filenames[0]) {
            in_file = new TFile(in_filenames[0].c_str(), "UPDATE");
            out_file = in_file;
        }
        else {
            //in_file = new TFile(in_filenames[0].c_str());
            out_file = new TFile(out_filename, "RECREATE");
        }

        TString years("");
        if (cfg.find("years") != cfg.end())
            years = TString(cfg["years"].get<std::string>());
        else {
            // TODO: extract years from filename itself
            cout << "ERROR: Year(s) can only be specified as a macro config or in the command line currently (not via filename)! Exiting..." << endl;
            return 0;
        }
        	
        map<TString, std::unique_ptr<TCanvas>> canvases;

        for (auto in_filename : in_filenames) {
            cout << endl << "Opening file " << in_filename << endl << endl;
            TFile * in_file_tmp;
            if (in_filename != out_filename)
                in_file_tmp = new TFile(in_filename.c_str());
            else
                in_file_tmp = out_file;

            for (auto && keyAsObj : *in_file_tmp->GetListOfKeys()){
                auto key = (TKey*)keyAsObj;
                if (TString(key->GetClassName()) != "THStack") continue;
                TString hs_name = TString(key->GetName());
                if (hs_name.Contains("_vs_")) continue; // mMake2DPlotsFromFile handles these
                if (hs_name.Contains("num") or hs_name.Contains("denom")) continue; // mMake1DEffPlotsFromFile handles these            
                cout << "Processing " << hs_name << ", class " << key->GetClassName() << endl;
                THStack * hs = (THStack*)in_file_tmp->Get(hs_name);
                TString hs_basename = ((TObjString*)(hs_name.Tokenize("-")->At(0)))->String();
                TString hs_suffix = ((TObjString*)(hs_name.Tokenize("-")->At(1)))->String();
                bool newCanvas = false;
                if (canvases.find(hs_basename) == canvases.end()) {
                    newCanvas = true;
                    canvases[hs_basename] = std::make_unique<TCanvas>(Form("canvas_%s", hs_basename.Data()));
                    auto * c = canvases[hs_basename].get();
                    c->SetMargin(0.1, 0.05, 0.1, 0.05);
                    c->Range(0,0,1,1);
                    c->Divide(1,2);
                    // top pad
                    c->cd(1);
                    gPad->SetPad(0.01, 0.33, 0.99, 0.99);
                    gPad->Draw(); 
                    gPad->cd();
                    gPad->SetTopMargin(0.1);
                    gPad->SetBottomMargin(0.01);
                    gPad->SetRightMargin(0.1);
                    gPad->SetFillStyle(0);
                    // bottom pad
                    c->cd(2);
                    gPad->SetPad(0.01, 0.01, 0.99, 0.32);
                    gPad->Draw();
                    gPad->cd();
                    gPad->SetTopMargin(0.01);
                    gPad->SetBottomMargin(0.3);
                    gPad->SetRightMargin(0.1);
                    gPad->SetFillStyle(0);
                }
                auto * c = canvases[hs_basename].get();
                c->cd(1); // top pad, the plot one
                TString drawOption = "";
                if (hs_suffix == "SIGNAL")
                    drawOption += "E HIST NOSTACK";
                else if (hs_suffix == "BKG")
                    drawOption += "HIST";
                else if (hs_suffix == "DATA")
                    drawOption += "E P";
                if (!newCanvas)
                    drawOption += " SAME";
                hs->Draw(drawOption.Data());
                if (hs_suffix == "BKG") {
                    TH1F * sum_hist = ((TH1F*)(hs->GetStack()->Last()));
                    sum_hist->SetMarkerSize(0);
                    sum_hist->SetFillStyle(3254);
                    sum_hist->SetFillColor(kGray+3);
                    sum_hist->Draw("E2 SAME");
                }
                if (newCanvas) {
                    hs->GetXaxis()->SetTitle(hs->GetTitle());
                    hs->GetYaxis()->SetTitle("Events");
                    hs->GetXaxis()->SetTitleSize(0.00);
                    hs->GetHistogram()->GetYaxis()->SetLabelSize(0.05);
                    hs->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
                    hs->GetHistogram()->GetYaxis()->SetTitleOffset(0.76);
                    hs->SetTitle("");
                    hs->GetHistogram()->SetLabelSize(0.0);
                    const bool writeExtraText = true;
                    //lumi_13TeV = "29.41 fb^{-1} ";
                    //CMS_lumi(c, 4);
                    //c->SetLogy();
                    CMS_lumi((TPad*)gPad, 4, 0, years);
                    gPad->SetLogy();
                    // Make cut description label
                    int cut;
                    TString tok;
                    Ssiz_t from = 0;
                    while (hs_basename.Tokenize(tok, from, "_")) 
                        if (tok.Contains("cut")) 
                            cut = (((TObjString*)(tok.Tokenize("t")->At(1)))->String()).Atoi();
                    TLatex cut_label;
                    cut_label.SetNDC();

                    cut_label.SetTextSize(0.05);

                    if (cuts_info[cut].special == TString("yes"))
                        cut_label.SetTextColor(kRed);

                    cut_label.DrawLatexNDC(0.25, 0.85, cuts_info[cut].description.Data()); //common::cut_descriptions[cut].c_str());
                }
                //hs->GetStack()->Last()->Draw("E");
                //canvases.push_back(std::move(c));
            }
            //in_file->Close();
        }
        // Make ratio subplot
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            //c->cd(1); // second subpad, the ratio one
            //TPad * top_pad = (TPad*)c->GetPad(1);
            //c->ls();
            TPad * top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));
            TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
	    
            THStack * data_hist = (THStack*)top_pad->GetPrimitive(Form("%s-DATA", canvas_name.Data()));
            THStack * bkg_hist = (THStack*)top_pad->GetPrimitive(Form("%s-BKG", canvas_name.Data()));
            THStack * signal_hist = (THStack*)top_pad->GetPrimitive(Form("%s-SIGNAL", canvas_name.Data()));
            TH1F * ratio_hist;
            c->cd(2); // switch to bottom pad
            if (data_hist && bkg_hist) {
                ratio_hist = (TH1F*)(((TH1F*)data_hist->GetStack()->Last())->Clone());
                ratio_hist->SetDirectory(0);
                TH1F * h2 = (TH1F*)(bkg_hist->GetStack()->Last());
                ratio_hist->Divide(h2);
                ratio_hist->GetXaxis()->SetTitle(bkg_hist->GetHistogram()->GetXaxis()->GetTitle());
                ratio_hist->Draw("EP");
            }
            else if (bkg_hist) { // only background, no data --> ratio is 1
                TH1F * h2 = (TH1F*)(bkg_hist->GetStack()->Last());
                ratio_hist = (TH1F*)h2->Clone();
                ratio_hist->SetDirectory(0);
                ratio_hist->Divide(h2);
                ratio_hist->GetXaxis()->SetTitle(bkg_hist->GetHistogram()->GetXaxis()->GetTitle());
                ratio_hist->SetMarkerSize(0);
                ratio_hist->SetFillStyle(3254);
                ratio_hist->SetFillColor(kGray+3);
                ratio_hist->Draw("E2");
            }
            else { // no background MC --> just use stacked signal MC to set ratio to 1
                TH1F * h2 = (TH1F*)(signal_hist->GetStack()->Last());
                ratio_hist = (TH1F*)h2->Clone();
                ratio_hist->SetDirectory(0);
                ratio_hist->Divide(h2);
                ratio_hist->GetXaxis()->SetTitle(signal_hist->GetHistogram()->GetXaxis()->GetTitle());
                ratio_hist->SetMarkerSize(0);
                ratio_hist->SetFillStyle(3254);
                ratio_hist->SetFillColor(kGray+3);
                ratio_hist->Draw("E2");
                //std::cout << "ERROR! No background MC in 1D plot... Ratio hist will fail. Are you sure this is intended?" << std::endl;
            }
            ratio_hist->SetMaximum(2.1);
            ratio_hist->SetMinimum(-0.1);
            ratio_hist->SetTitle("");
            ratio_hist->GetXaxis()->SetTitleSize(0.12);
            ratio_hist->GetXaxis()->SetLabelSize(0.11);
            ratio_hist->GetYaxis()->SetTitle("Data/MC");
            ratio_hist->GetYaxis()->SetTitleSize(0.12);
            ratio_hist->GetYaxis()->SetLabelSize(0.11);
            ratio_hist->GetYaxis()->SetNdivisions(5);
            ratio_hist->GetYaxis()->SetTitleOffset(0.35);
                
            // ratio line
            TLine * ratio_line = new TLine();
            ratio_line->SetLineColor(kRed);
            ratio_line->SetLineWidth(2);
            // set line params
            ratio_line->SetX1(ratio_hist->GetXaxis()->GetXmin());
            ratio_line->SetX2(ratio_hist->GetXaxis()->GetXmax());
            ratio_line->SetY1(1.0);
            ratio_line->SetY2(1.0);
            ratio_line->Draw("SAME");
            //ratio_hist->Draw("EP SAME");

            // MC errors
            if (bkg_hist) {
                TH1F * h2 = (TH1F*)(bkg_hist->GetStack()->Last());
                TH1F * MC_error = (TH1F*)h2->Clone();
                MC_error->Divide(MC_error);
                MC_error->SetMarkerSize(0);
                MC_error->SetFillStyle(3254);
                MC_error->SetFillColor(kGray+3);
                MC_error->Draw("SAME E2");
            }
            //else if (bkg_hist || signal_hist) { // likely blinded SR (no data) just plot MC
            //    c->cd(1);
            //    //gPad->SetPad(0.01, 0.01, 0.99, 0.99);
            //    gPad->SetBottomMargin(0.2);
            //    if (bkg_hist) {
            //        bkg_hist->GetXaxis()->SetTitle(bkg_hist->GetHistogram()->GetXaxis()->GetTitle());
            //        bkg_hist->GetXaxis()->SetTitleSize(0.06);
            //        bkg_hist->GetXaxis()->SetLabelSize(0.055);
            //    }
            //    else {
            //        signal_hist->GetXaxis()->SetTitle(signal_hist->GetHistogram()->GetXaxis()->GetTitle());
            //        signal_hist->GetXaxis()->SetTitleSize(0.06);
            //        signal_hist->GetXaxis()->SetLabelSize(0.055);
            //    }
            //    gPad->Modified();
            //    gPad->Update();
            //}
            gPad->Modified();
            gPad->Update();
        }
        // Build legend for all canvases
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            c->cd(1);
            //TLegend * legend = new TLegend(0.62, 0.45, 0.92, 0.85);
            TLegend * legend = new TLegend(0.35, 0.60, 0.88, 0.85);
            legend->SetNColumns(2);
            legend->SetFillStyle(0);
            legend->SetTextSize(0.037);
            TIter next((TList*)gPad->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("THStack")) continue;
                // mMake2DPlotsFromFile.C handles 2D plots
                if (TString(hs->GetName()).Contains("_vs_")) continue;
                TIter next2((TList*)(((THStack*)hs)->GetHists()));
                TH1F * h;
                while ((h = (TH1F*)next2())) {
                    if (TString(hs->GetName()).Contains("SIGNAL"))
                        legend->AddEntry((TH1F*)h, "", "l");
                    else if (TString(hs->GetName()).Contains("BKG"))
                        legend->AddEntry((TH1F*)h, "", "f");
                    else if (TString(hs->GetName()).Contains("DATA"))
                        legend->AddEntry((TH1F*)h, "", "lep");
                }
            }
            legend->Draw();
            gPad->Modified();
        }
        // Figure out which THStack has highest maximum
        map<TString, Double_t> maxima;
        for (auto & pair : canvases) {
            maxima[pair.first] = -1000;
            auto * c = pair.second.get();
            c->cd(1);
            TIter next((TList*)gPad->GetListOfPrimitives());
            TObject * h;
            while ((h = next())) {
                if (TString(h->ClassName()) != TString("THStack")) continue;
                if (((THStack*)h)->GetMaximum() > maxima[pair.first]) maxima[pair.first] = ((THStack*)h)->GetMaximum();
            }
        }
        // Set this maximum to all THStacks
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            c->cd(1);
            TIter next((TList*)gPad->GetListOfPrimitives());
            TObject * h;
            while ((h = next())) {
                if (TString(h->ClassName()) != TString("THStack")) continue;
                //((THStack*)h)->SetMaximum(30*maxima[pair.first]);
                ((THStack*)h)->SetMaximum(1000000*maxima[pair.first]);
                ((THStack*)h)->SetMinimum(0.01);
            }
            //c->RedrawAxis();
            //gPad->Modified();
            c->Modified();
        }
        // Write canvases to ROOT file
        out_file->cd();
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
