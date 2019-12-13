#include "mMake1DEffPlotsFromFile.h"
#include <string.h>
//TString lumi_13TeV = "59.74 fb^{-1} ";


int gyear;
namespace macro {
    void canvasDraw(TString hs_basename, TString hs_suffix,TEfficiency *hs, TCanvas *c,bool newCanvas, bool zoom, int year){
	cout << "formatting canvas and drawing histograms function " << hs_basename<<hs_suffix<<hs->GetName()<<endl;
	TString x_title = TString(hs->GetPassedHistogram()->GetXaxis()->GetTitle());
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
		
		c->cd(1);
		TString drawopt = "E P SAME";
		TH1F* background = (TH1F*)((TEfficiency*)hs->GetTotalHistogram())->Clone();	
		background->Reset();
			
		background->SetMaximum(1.1);
		background->SetMinimum(0.1);
		background->SetTitle("");
		//Set Y axis range for a zoomed in ploe
		if(zoom){
		background->SetMaximum(1.02);
		background->SetMinimum(0.92);
		}
		background->Draw();
                hs->SetTitle(hs_suffix.Append(";").Append(x_title));
                hs->Draw(drawopt.Data());
            	gPad->Update();
		
		// Set Labels using the empty background TH1
                background->GetYaxis()->SetTitle("Efficiency");
                background->GetXaxis()->SetTitleSize(0.00);
                background->GetYaxis()->SetLabelSize(0.05);
                background->GetYaxis()->SetTitleSize(0.06);
                background->GetYaxis()->SetTitleOffset(0.76);
                background->GetXaxis()->SetTitle(x_title);
		background->SetLabelSize(0.0);
            	gPad->Update();
                const bool writeExtraText = true;
                CMS_lumi((TPad*)gPad, 4, 0,year);
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
        if (!newCanvas){
            	c->cd(1); // top pad, the plot one
                hs->SetTitle(hs_suffix.Append(";").Append(x_title));
            	hs->Draw("E P SAME");
            	gPad->Update();
	}
    }
    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();
	//year = cfg["year"].get<int>();
	bool fit = true;
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
	    if (!(continue_num && continue_denom)) {/*cout<<"passing" <<endl; */continue;}
	    
	    cout<<"continuing"<<endl;
	    continue_num = false;
	    continue_denom = false;
	    TString type;
	    int year;
	    TString yearname;
	    if (hs_name.Contains("1718")) { continue;}
	    if (hs_name.Contains("2017")) { year=2017;yearname="_2017";}
	    if (hs_name.Contains("2018")) { year=2018;yearname="_2018";}
	    if (hs_name.Contains("2016")) { year=2016;yearname="_2016";}
	    cout << "year: " << year<<endl;
	    if (hs_name.Contains("SIGNAL"))
	        type = TString("SIGNAL");
	    else if (hs_name.Contains("DATA"))
	        type = TString("DATA");
	    else 
	        {type = TString("BKG");
	    	TH1F * MCTotal_num = (TH1F*)(((TH1F*)hsn->GetStack()->Last())->Clone());
	    	TH1F * MCTotal_denom = (TH1F*)(((TH1F*)hsd->GetStack()->Last())->Clone());
		TObjArray* name_tolks = TString(MCTotal_denom->GetName()).Tokenize("_");
		MCTotal_denom->SetName((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(3)))->String())).Append(yearname).Append(TString("_MCTotal")));
		//MCTotal_denom->SetName((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(3)))->String())).Append("_MCTotal").Append(TString("_MCTotal")));
		cout<<"name "<<MCTotal_denom->GetName()<<endl;
		
		for (int i =0; i<MCTotal_num->GetNbinsX();i++){	
		//	if(MCTotal_denom->GetBinContent(i)<0){
		//		MCTotal_denom->SetBinContent(i,abs(MCTotal_denom->GetBinContent(i)));
		//	}
		//	if(MCTotal_num->GetBinContent(i)<0){
		//		MCTotal_num->SetBinContent(i,abs(MCTotal_num->GetBinContent(i)));	
		//	}
			if(MCTotal_num->GetBinContent(i)>MCTotal_denom->GetBinContent(i)){
				cout <<"bin: "<<i<<" "<<MCTotal_num->GetBinContent(i)<<" "<<MCTotal_num->GetBinContent(i) <<endl;
				cout<<"resetting bin content"<<endl;
				MCTotal_denom->SetBinContent(i,MCTotal_num->GetBinContent(i));
			}
		}
		MCTotal_num->SetMinimum(0.8);
		MCTotal_denom->SetMinimum(0.8);
            	MCTotal_num->GetXaxis()->SetTitle(hsd->GetTitle());	
            	MCTotal_denom->GetXaxis()->SetTitle(hsd->GetTitle());	
		TEfficiency* mc_eff = new TEfficiency(*MCTotal_num,*MCTotal_denom);
		mc_eff->SetLineColor(common::group_plot_info["MCTotal"].color);
		mc_eff->SetMarkerColor(common::group_plot_info["MCTotal"].color);
		mc_eff->SetFillColor(common::group_plot_info["MCTotal"].color);
		
		if(fit){
		TF1* f1 = new TF1("f1","[2]+[3]*TMath::Erf((x-[0])/[1])",0,400);
		f1->SetParameters(25,36,0.5,0.5);
		f1->SetLineColor(mc_eff->GetLineColor());
		mc_eff->Fit(f1,"SAME R+");
		}

		canvases[MCTotal_denom->GetName()] = std::make_unique<TCanvas>(Form("canvas_%s_%s", MCTotal_denom->GetName(),yearname.Data()));
                auto * c = canvases[MCTotal_denom->GetName()].get();
	    	canvasDraw(MCTotal_denom->GetName(),TString("BKG"),mc_eff,c,true,false,year);	
                canvases[TString(MCTotal_denom->GetName()).Append("_zoom")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_zoom", MCTotal_denom->GetName(),yearname.Data()));
                auto * cz = canvases[TString(MCTotal_denom->GetName()).Append("_zoom")].get();
	    	canvasDraw(MCTotal_denom->GetName(),TString("BKG"),mc_eff,cz,true,true,year);	
	    }
	    TIter hsn_it((TList*)(((THStack*)hsn)->GetHists()));
	    TH1F *hsnx;
	    while((hsnx = (TH1F*)hsn_it())){
	    	TString n1 = TString(hsnx->GetName());	
	    	TString n1x0 = ((TObjString*)(n1.Tokenize("_")->At(2)))->String();
	    	TString n1x1 = ((TObjString*)(n1.Tokenize("_")->At(5)))->String();
	        TIter hsd_it((TList*)(((THStack*)hsd)->GetHists()));
	        TH1F *hsdx;
	    	while((hsdx = (TH1F*)hsd_it())){
	    		TString n2 = TString(hsdx->GetName());	
	   	 	TString n2x0 = ((TObjString*)(n2.Tokenize("_")->At(2)))->String();
	    		TString n2x1 = ((TObjString*)(n2.Tokenize("_")->At(5)))->String();
	    		if (((n1x0.Contains(n2x0)) && (n1x1.Contains(n2x1)))) {
	    			cout <<"stacking "<<n1x1<<n2x1 <<endl;
				for (int i =0; i<hsnx->GetNbinsX();i++){	
				//	if(hsdx->GetBinContent(i)<0){
				//		hsdx->SetBinContent(i,abs(hsdx->GetBinContent(i)));
				//	}
				//	if(hsnx->GetBinContent(i)<0){
				//		hsnx->SetBinContent(i,abs(hsnx->GetBinContent(i)));	
				//	}
			 		if(hsnx->GetBinContent(i)>hsdx->GetBinContent(i)){
						cout <<"bin: "<<i<<" "<<hsnx->GetBinContent(i)<<" "<<hsdx->GetBinContent(i) <<endl;
						cout<<"resetting bin content"<<endl;
						hsdx->SetBinContent(i,hsnx->GetBinContent(i));
					}
				}
            		        hsdx->GetXaxis()->SetTitle(hsd->GetTitle());	
            		        hsnx->GetXaxis()->SetTitle(hsd->GetTitle());	
				TEfficiency* hs = new TEfficiency(*hsnx,*hsdx);
				hs->SetLineColor(common::group_plot_info[n1x1].color);
				hs->SetMarkerColor(common::group_plot_info[n1x1].color);
				hs->SetFillStyle(common::group_plot_info[n1x1].style);

		if(fit){
		TF1* f1 = new TF1("f1","[2]+[3]*TMath::Erf((x-[0])/[1])",0,400);
		f1->SetParameters(25,36,0.5,0.5);
		f1->SetLineColor(hs->GetLineColor());
		hs->Fit(f1,"SAME R+");
		}
			
	     
	    TString hs_basename = n1x0;
	    hs_basename.Append(yearname);
            TString hs_suffix = n1x1; 
            bool newCanvas = false;
            if (canvases.find(hs_basename) == canvases.end()) {
                newCanvas = true;
                canvases[hs_basename] = std::make_unique<TCanvas>(Form("canvas_%s_%s", hs_basename.Data(),yearname.Data()));
                auto * c = canvases[hs_basename].get();
                canvases[TString(hs_basename).Append("_zoom")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_zoom", hs_basename.Data(),yearname.Data()));
                auto * cz = canvases[TString(hs_basename).Append("_zoom")].get();
            }
            auto * c = canvases[hs_basename].get();
	    canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),c,newCanvas,false,year);	
            auto * cz = canvases[TString(hs_basename).Append("_zoom")].get();
	    canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),cz,newCanvas,true,year);	
	    if(type.Contains("DATA")){
		TObjArray* dataname_tolks = TString(hs->GetName()).Tokenize("_");
		//TString MCTotal_dataname = TString((((TObjString*)(dataname_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(dataname_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(3)))->String())).Append("_MCTotal").Append(TString("_MCTotal")));
		TString MCTotal_dataname = TString((((TObjString*)(dataname_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(dataname_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(3)))->String())).Append(yearname).Append(TString("_MCTotal")));
               
		auto * c1 = canvases[MCTotal_dataname].get();
	    	canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),c1,newCanvas,false,year);	
          	auto * cz1 = canvases[TString(MCTotal_dataname).Append("_zoom")].get();
	    	canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),cz1,newCanvas,true,year);	
	     }
       }}}} 
        
	// Make ratio subplot
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            TPad * top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));
	    //c->ls();
            TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
	    bool zoom = canvas_name.Contains("zoom")? true: false;
            TEfficiency * data_hist;
            vector<TEfficiency*> MC_hists;
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
		data_ratio->SetBinContent(i,data_hist->GetEfficiency(i));
		}
                data_ratio->Sumw2();
		bool first = true;
		for( auto MC_hist : MC_hists){
		TH1F* ratio_hist = (TH1F*)data_ratio->Clone();
		TH1F* MC_ratio = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();	
		ratio_hist->GetXaxis()->SetTitle(MC_hist->GetPassedHistogram()->GetXaxis()->GetTitle());
		MC_ratio->Reset();
		for (int i =0; i<nbins;i++){	
		MC_ratio->SetBinContent(i,MC_hist->GetEfficiency(i));
		}
                MC_ratio->Sumw2();
                ratio_hist->Divide(MC_ratio);
                ratio_hist->ResetAttFill();
		if (zoom){
                ratio_hist->SetMaximum(1.02);
                ratio_hist->SetMinimum(0.90);
		}
		else{
                ratio_hist->SetMaximum(2.1);
                ratio_hist->SetMinimum(0.1);
		}
                
		ratio_hist->SetTitle("");
                ratio_hist->GetXaxis()->SetTitleSize(0.12);
                ratio_hist->GetXaxis()->SetLabelSize(0.11);
                ratio_hist->GetYaxis()->SetTitle("Data/MC");
                ratio_hist->GetYaxis()->SetTitleSize(0.12);
                ratio_hist->GetYaxis()->SetLabelSize(0.11);
                ratio_hist->GetYaxis()->SetNdivisions(5);
                ratio_hist->GetYaxis()->SetTitleOffset(0.35);
		cout <<"TEST "<<MC_hist->GetName()<<endl;
		ratio_hist->SetMarkerColor(common::group_plot_info[((TObjString*)(TString(MC_hist->GetName()).Tokenize("_")->At(5)))->String()].color);
                ratio_hist->Draw("EP SAME");
                
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
		cout<<hs->GetName()<<endl;
                // TODO Handle 2D plots
                if (TString(hs->GetName()).Contains("_vs_")) continue; // skip 2D plots for now
                if(!TString(hs->GetName()).Contains("_")) continue;
		cout<<hs->GetName()<<endl;
                TString group = ((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(5)))->String();
		float param = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParameter(2) + ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParameter(3);
		float paramerr = ((
				(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(2))*
				(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(2))) + 
				((((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(3))*
				(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(3)))); 
				
		
		std::string paramsum = Form(": %.4e +/- %.4e",param,paramerr);
		const char* leg = common::group_plot_info[group].legend.Data();
		std::string both1 = leg + paramsum;
		const char* both =both1.c_str();
                if (TString(hs->GetName()).Contains("Signal"))
                    legend->AddEntry(hs, "", "l");
                else if (TString(hs->GetName()).Contains("data")){
                    legend->AddEntry(hs, both, "lep");
                    }
                else{
                    legend->AddEntry(hs, both, "lep");
                    }
            }
            legend->Draw();
            gPad->Modified();
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
