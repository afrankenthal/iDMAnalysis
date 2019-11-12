#include "mMake1DEffPlotsFromFile.h"
#include <string.h>
//TString lumi_13TeV = "59.74 fb^{-1} ";

namespace macro {
    void canvasDraw(bool newCanvas,TString type, TString hs_basename, TString hs_suffix,TEfficiency *hs, TCanvas *c){
	cout << "formatting canvas and drawing histograms function " << hs_basename<<hs_suffix<<hs->GetName()<<endl;
	if(newCanvas){
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
            c->cd(1); // top pad, the plot one
            TString drawOption = "";
            if (type.EqualTo("SIGNAL")){
                drawOption += "HIST NOSTACK";
	    }
            else if (type.EqualTo("BKG")){
                drawOption += "HIST";
            }
	    else if (type.EqualTo("DATA")){
                drawOption += "E P";
	    }
            if (!newCanvas){
		TString title = hs_suffix.Append(";").Append(hs_basename).Append(";Efficiency");
                hs->SetTitle(title);
                drawOption += " SAME";
            	hs->Draw(drawOption.Data());
	    }
            if (newCanvas) {
		TString title = hs_suffix.Append(";").Append(hs_basename).Append(";Efficiency");
                hs->SetTitle(title);
                hs->Draw();
            	gPad->Modified();
                //hs->GetPaintedGraph()->GetYaxis()->SetTitle("Efficiency!!!");
                //hs->GetXaxis()->SetTitleSize(0.00);
               // hs->GetHistogram()->GetYaxis()->SetLabelSize(0.05);
               // hs->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
               // hs->GetHistogram()->GetYaxis()->SetTitleOffset(0.76);
                //hs->SetTitle("");
                //hs->GetPaintedGraph()->GetXaxis()->SetTitle(n1x0);
		//hs->GetHistogram()->SetLabelSize(0.0);
                const bool writeExtraText = true;
                CMS_lumi((TPad*)gPad, 4, 0);
                // Make cut description label
                int cut1;
                int cut2;
 
		TString tok;
		TString tok2;
                Ssiz_t from = 0;
                TLatex cut_label;
                cut_label.SetNDC();
                cut_label.SetTextSize(0.05);
		cut_label.DrawLatexNDC(0.25, 0.85, "Trigger efficiency");
            }
	}
    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();

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
        
	bool continue_num = false;
	bool continue_denom = false;
        THStack * hsn;
        THStack * hsd;
        map<TString, std::unique_ptr<TCanvas>> canvases;
        for (auto && keyAsObj : *in_file->GetListOfKeys()){
	   // cout << "EFF TEST"<<endl;
            auto key = (TKey*)keyAsObj;
            if (TString(key->GetClassName()) != "THStack") continue;
            TString hs_name = TString(key->GetName());
            if (hs_name.Contains("_vs_")) continue; // mMake2DPlotsFromFile handles these
            if (hs_name.Contains("num")){
            cout << "Processing num " << hs_name << ", class " << key->GetClassName() << endl;
            hsn = (THStack*)in_file->Get(hs_name);
		continue_num = true;
	    }
            if (hs_name.Contains("denom")){ 
            cout << "Processing denom " << hs_name << ", class " << key->GetClassName() << endl;
            hsd = (THStack*)in_file->Get(hs_name);
		continue_denom = true;
		}
	    if (!(continue_num && continue_denom)) {cout<<"passing" <<endl; continue;}
	    
	    cout<<"continuing"<<endl;
	    continue_num = false;
	    continue_denom = false;
	    TString type;
	    if (hs_name.Contains("SIGNAL"))
	        type = TString("SIGNAL");
	    else if (hs_name.Contains("DATA"))
	        type = TString("DATA");
	    else 
	        {type = TString("BKG");
	    	TH1F * MCTotal_num = (TH1F*)(((TH1F*)hsn->GetStack()->Last())->Clone());
	    	TH1F * MCTotal_denom = (TH1F*)(((TH1F*)hsd->GetStack()->Last())->Clone());
		TObjArray* name_tolks = TString(MCTotal_denom->GetName()).Tokenize("_");
		MCTotal_denom->SetName((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(3)))->String())).Append(TString("_MCTotal")));
		//MCTotal_denom->SetName((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append(TString("_MCTotal")));
		cout<<"name "<<MCTotal_denom->GetName()<<endl;
				for (int i =0; i<MCTotal_num->GetNbinsX();i++){	
			 	if(MCTotal_num->GetBinContent(i)>MCTotal_denom->GetBinContent(i)){
				cout <<"bin: "<<i<<" "<<MCTotal_num->GetBinContent(i)<<" "<<MCTotal_num->GetBinContent(i) <<endl;
				cout<<"resetting bin content"<<endl;
				MCTotal_denom->SetBinContent(i,MCTotal_num->GetBinContent(i));
				}}
		TEfficiency* mc_eff = new TEfficiency(*MCTotal_num,*MCTotal_denom);
		mc_eff->SetLineColor(common::group_plot_info["MCTotal"].color);
		mc_eff->SetMarkerColor(common::group_plot_info["MCTotal"].color);
		mc_eff->SetFillColor(common::group_plot_info["MCTotal"].color);
                canvases[MCTotal_denom->GetName()] = std::make_unique<TCanvas>(Form("canvas_%s", MCTotal_denom->GetName()));
                auto * c = canvases[MCTotal_denom->GetName()].get();
	    	//test(true,type,((TObjString*)(hs_name.Tokenize("_")->At(0)))->String(),TString("BKG"),mc_eff,c);	
	    	canvasDraw(true,type,MCTotal_denom->GetName(),TString("BKG"),mc_eff,c);	
		}
	    TIter hsn_it((TList*)(((THStack*)hsn)->GetHists()));
	    TH1F *hsnx;
	    while((hsnx = (TH1F*)hsn_it())){
	    	TString n1 = TString(hsnx->GetName());	
	    	TString n1x0 = ((TObjString*)(n1.Tokenize("_")->At(2)))->String();
	    	//TString n1x1 = ((TObjString*)(n1.Tokenize("_")->At(2)))->String();
	    	TString n1x1 = ((TObjString*)(n1.Tokenize("_")->At(5)))->String();
	        TIter hsd_it((TList*)(((THStack*)hsd)->GetHists()));
	        TH1F *hsdx;
	    	while((hsdx = (TH1F*)hsd_it())){
	    		TString n2 = TString(hsdx->GetName());	
	   	 	TString n2x0 = ((TObjString*)(n2.Tokenize("_")->At(2)))->String();
	    		//TString n2x1 = ((TObjString*)(n2.Tokenize("_")->At(2)))->String();
	    		TString n2x1 = ((TObjString*)(n2.Tokenize("_")->At(5)))->String();
	    		if (((n1x0.Contains(n2x0)) && (n1x1.Contains(n2x1)))) {
	    			cout <<"stacking "<<n1x1<<n2x1 <<endl;
				for (int i =0; i<hsnx->GetNbinsX();i++){	
			 	if(hsnx->GetBinContent(i)>hsdx->GetBinContent(i)){
				cout <<"bin: "<<i<<" "<<hsnx->GetBinContent(i)<<" "<<hsdx->GetBinContent(i) <<endl;
				cout<<"resetting bin content"<<endl;
				hsdx->SetBinContent(i,hsnx->GetBinContent(i));
				}}
            			
				TEfficiency* hs = new TEfficiency(*hsnx,*hsdx);
				hs->SetLineColor(common::group_plot_info[n1x1].color);
				hs->SetMarkerColor(common::group_plot_info[n1x1].color);
				hs->SetFillStyle(common::group_plot_info[n1x1].style);
			
	     
	    TString hs_basename = n1x0;//((TObjString*)(hs_name.Tokenize("_")->At(0)))->String();
            TString hs_suffix = n1x1; //((TObjString*)(hs_name.Tokenize("_")->At(2)))->String();
            bool newCanvas = false;
            if (canvases.find(hs_basename) == canvases.end()) {
                newCanvas = true;
                canvases[hs_basename] = std::make_unique<TCanvas>(Form("canvas_%s", hs_basename.Data()));
                auto * c = canvases[hs_basename].get();
//                c->Range(0,0,1,1);
//                c->Divide(1,2);
//                // top pad
//                c->cd(1);
//                gPad->SetPad(0.01, 0.33, 0.99, 0.99);
//                gPad->Draw(); 
//                gPad->cd();
//                gPad->SetTopMargin(0.1);
//                gPad->SetBottomMargin(0.01);
//                gPad->SetRightMargin(0.1);
//                gPad->SetFillStyle(0);
//                // bottom pad
//                c->cd(2);
//                gPad->SetPad(0.01, 0.01, 0.99, 0.32);
//                gPad->Draw();
//                gPad->cd();
//                gPad->SetTopMargin(0.01);
//                gPad->SetBottomMargin(0.3);
//                gPad->SetRightMargin(0.1);
//                gPad->SetFillStyle(0);
            }
            auto * c = canvases[hs_basename].get();
	    canvasDraw(newCanvas,type,n1x0,n1x1,(TEfficiency*)hs->Clone(),c);	
	if(type.Contains("DATA")){
		TObjArray* dataname_tolks = TString(hs->GetName()).Tokenize("_");
		TString MCTotal_dataname = TString((((TObjString*)(dataname_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(dataname_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(3)))->String())).Append(TString("_MCTotal")));
		cout<<"dataname: "<<MCTotal_dataname<<endl;
          auto * c1 = canvases[MCTotal_dataname].get();
	    canvasDraw(newCanvas,type,n1x0,n1x1,(TEfficiency*)hs->Clone(),c1);	
	}
//            c->cd(1); // top pad, the plot one
//            TString drawOption = "";
//            if (type.EqualTo("SIGNAL")){
//                drawOption += "HIST NOSTACK";
//	    }
//            else if (type.EqualTo("BKG")){
//                drawOption += "HIST";
//            }
//	    else if (type.EqualTo("DATA")){
//                drawOption += "E P";
//	    }
//            if (!newCanvas){
//		TString title = n1x1.Append(";").Append(n1x0).Append(";Efficiency");
//                hs->SetTitle(title);
//                drawOption += " SAME";
//            	hs->Draw(drawOption.Data());
//	    }
//            if (newCanvas) {
//		TString title = n1x1.Append(";").Append(n1x0).Append(";Efficiency");
//                hs->SetTitle(title);
//                hs->Draw();
//            	gPad->Modified();
//                //hs->GetPaintedGraph()->GetYaxis()->SetTitle("Efficiency!!!");
//                //hs->GetXaxis()->SetTitleSize(0.00);
//               // hs->GetHistogram()->GetYaxis()->SetLabelSize(0.05);
//               // hs->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
//               // hs->GetHistogram()->GetYaxis()->SetTitleOffset(0.76);
//                //hs->SetTitle("");
//                //hs->GetPaintedGraph()->GetXaxis()->SetTitle(n1x0);
//		//hs->GetHistogram()->SetLabelSize(0.0);
//                const bool writeExtraText = true;
//                CMS_lumi((TPad*)gPad, 4, 0);
//                // Make cut description label
//                int cut1;
//                int cut2;
// 
//		TString tok;
//		TString tok2;
//                Ssiz_t from = 0;
//                TLatex cut_label;
//                cut_label.SetNDC();
//                cut_label.SetTextSize(0.05);
//		cut_label.DrawLatexNDC(0.25, 0.85, "Trigger efficiency");
//            }
       }}}} 
        // Make ratio subplot
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            //c->cd(1); // second subpad, the ratio one
            //TPad * top_pad = (TPad*)c->GetPad(1);
            TPad * top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));
	    cout<<"primitives"<<endl;
	    c->ls();
            TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
            TEfficiency * data_hist;// = (TH1F*)top_pad->GetPrimitive(Form("%s-DATA", canvas_name.Data()));
            vector<TEfficiency*> MC_hists;// = (THStack*)top_pad->GetPrimitive(Form("%s-BKG", canvas_name.Data()));
	    TIter next((TList*)top_pad->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("TEfficiency")) continue;
                if(!TString(hs->GetName()).Contains("_")) continue;
                if (TString(hs->GetName()).Contains("data")){
                 data_hist = (TEfficiency*)hs;
		}
		else{
		  MC_hists.push_back((TEfficiency*)hs);}
		}
            if (data_hist && !MC_hists.empty()) {
                c->cd(2); // switch to bottom pad
		int nbins= data_hist->GetPassedHistogram()->GetNbinsX();	
		TH1F* data_ratio = (TH1F*)((TH1F*)data_hist->GetPassedHistogram())->Clone();
		data_ratio->Reset();
		for (int i =0; i<nbins;i++){	
		data_ratio->SetBinContent(i,data_hist->GetEfficiency(i));///MC_hist->GetEfficiency(i));
		}
                data_ratio->Sumw2();
		bool first = true;
		for( auto MC_hist : MC_hists){
		TH1F* ratio_hist = (TH1F*)data_ratio->Clone();
		TH1F* MC_ratio = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();
		
		MC_ratio->Reset();
		for (int i =0; i<nbins;i++){	
		//cout <<"bin: "<<i<<" "<<MC_hist->GetEfficiency(i)<<endl;
		MC_ratio->SetBinContent(i,MC_hist->GetEfficiency(i));///MC_hist->GetEfficiency(i));
		}
                MC_ratio->Sumw2();
                ratio_hist->Divide(MC_ratio);
                ratio_hist->ResetAttFill();
                ratio_hist->SetMaximum(2.1);
                ratio_hist->SetMinimum(-0.1);
                ratio_hist->SetTitle("");
                //ratio_hist->GetXaxis()->SetTitle(data_hist->GetXaxis()->GetTitle());
                ratio_hist->GetXaxis()->SetTitleSize(0.12);
                ratio_hist->GetXaxis()->SetLabelSize(0.11);
                ratio_hist->GetYaxis()->SetTitle("Data/MC");
                ratio_hist->GetYaxis()->SetTitleSize(0.12);
                ratio_hist->GetYaxis()->SetLabelSize(0.11);
                ratio_hist->GetYaxis()->SetNdivisions(5);
                ratio_hist->GetYaxis()->SetTitleOffset(0.35);
		ratio_hist->SetMarkerColor(common::group_plot_info[((TObjString*)(TString(MC_hist->GetName()).Tokenize("_")->At(4)))->String()].color);
		//ratio_hist->SetMarkerColor(common::group_plot_info[((TObjString*)(TString(MC_hist->GetName()).Tokenize("_")->At(2)))->String()].color);
                ratio_hist->Draw("EP SAME");
            	//gPad->Modified();
            	//c->Modified();
		//}
                // ratio line
                //c->cd(2); // switch to bottom pad
                TLine * ratio_line = new TLine();
                ratio_line->SetLineColor(kRed);
                ratio_line->SetLineWidth(2);
                // set line params
                ratio_line->SetX1(ratio_hist->GetXaxis()->GetXmin());
                ratio_line->SetX2(ratio_hist->GetXaxis()->GetXmax());
                ratio_line->SetY1(1.0);
                ratio_line->SetY2(1.0);
                ratio_line->Draw("P SAME");
//
//                // MC errors
//                TH1F * MC_error = (TH1F*)h2->Clone();
//                MC_error->Divide(MC_error);
//                MC_error->SetMarkerSize(0);
//                MC_error->SetFillStyle(3254);
//                MC_error->SetFillColor(kGray+3);
//                MC_error->Draw("SAME E2");
            } }
        }
      
      // Build legend for all canvases
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            c->cd(1);
            TLegend * legend = new TLegend(0.62, 0.05, 0.85, 0.45);
            legend->SetFillStyle(0);
            legend->SetTextSize(0.037);
            
	    TIter next((TList*)gPad->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("TEfficiency")) continue;
                // TODO Handle 2D plots
                if (TString(hs->GetName()).Contains("_vs_")) continue; // skip 2D plots for now
                if(!TString(hs->GetName()).Contains("_")) continue;
                TString group = ((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(4)))->String();
                //TString group = ((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(2)))->String();
                if (TString(hs->GetName()).Contains("Signal"))
                    legend->AddEntry(hs, "", "l");
                else if (TString(hs->GetName()).Contains("data"))
                    legend->AddEntry(hs, common::group_plot_info[group].legend, "lep");
                else
                    legend->AddEntry(hs, common::group_plot_info[group].legend, "lep");		 
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
                ((THStack*)h)->SetMaximum(5*maxima[pair.first]);
                ((THStack*)h)->SetMinimum(0.01);
            }
            //c->RedrawAxis();
            //gPad->Modified();
            c->Modified();
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
