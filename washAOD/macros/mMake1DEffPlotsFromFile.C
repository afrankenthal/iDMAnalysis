#include "mMake1DEffPlotsFromFile.h"
#include <string.h>
//TString lumi_13TeV = "59.74 fb^{-1} ";


namespace macro {
    void canvasDraw(TString hs_basename, TString hs_suffix,TEfficiency *hs, TCanvas *c,bool newCanvas, bool zoom, TString year){
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
        // Set Labels using the empty background TH1
        TH1F* background = (TH1F*)((TEfficiency*)hs->GetTotalHistogram())->Clone(); 
        background->Reset();
        background->ResetAttFill();
            
        background->SetMaximum(1.1);
        background->SetMinimum(0.0);
        background->SetTitle("");
        //Set Y axis range for a zoomed in ploe
        if(zoom){
          background->SetMaximum(1.02);
          background->SetMinimum(0.92);
        }
        background->Draw();
        background->GetYaxis()->SetTitle("Efficiency");
        background->GetYaxis()->SetLabelSize(0.05);
        background->GetYaxis()->SetTitleSize(0.06);
        background->GetYaxis()->SetTitleOffset(0.76);
        background->GetXaxis()->SetTitleSize(0.05);
        background->GetXaxis()->SetLabelSize(0.06);
        background->GetXaxis()->SetTitle(x_title);
        background->SetLabelSize(0.0);
        gPad->Update();
        hs->SetTitle(hs_suffix.Append(";").Append(x_title));
        hs->Draw(drawopt.Data());
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
        // macro options
        TString in_filename = TString(cfg["infilenames"].get<std::vector<std::string>>()[0]);
        if (in_filename == TString("")) {
            cout << "ERROR! No input filename. Exiting..." << endl;
            return 0;
        }
        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == TString(""))
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
        TString year("");
        if (cfg.find("years") != cfg.end())
            year = TString(cfg["years"].get<std::string>());
        else {
            cout << "ERROR: Year(s) can only be specified as a macro config or in the command line currently (not via filename)! Exiting..." << endl;
            return 0;
        }
        
        bool fit = true;
        bool sigfit = false;
        int lowfit;int highfit;
        
        if(year.Contains("2016")){ lowfit = 175; highfit=600;}
        else if(year.Contains("2017")){ lowfit = 185; highfit=800;}
        else if(year.Contains("2018")){ lowfit = 175; highfit=600;}
        else{ lowfit=0; highfit=1000;}
        
        printf("low %d high %d",lowfit,highfit);
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
                hsn = (THStack*)in_file->Get(hs_name);
                continue_num = true;
            }
            if (hs_name.Contains("denom")){ 
                hsd = (THStack*)in_file->Get(hs_name);
                continue_denom = true;
            }
            if (!(continue_num && continue_denom)) {/*cout<<"passing" <<endl; */continue;}
        
            continue_num = false;
            continue_denom = false;
            TString type;
            if (hs_name.Contains("SIGNAL")){
                type = TString("SIGNAL");
                TIter hsn_it((TList*)(((THStack*)hsn)->GetHists()));
                TH1F *hsnx;
                TIter hsd_it((TList*)(((THStack*)hsd)->GetHists()));
                TH1F *hsdx;

                auto hsnx_60 = new THStack("hsnx_60p0","mass 60p0");
                auto hsdx_60 = new THStack("hsdx_60p0","mass 60p0");
                auto hsnx_6 = new THStack("hsnx_6p0","mass 6p0");
                auto hsdx_6 = new THStack("hsdx_6p0","mass 6p0");
                auto hsnx_52 = new THStack("hsnx_52p5","mass 52p5");
                auto hsdx_52 = new THStack("hsdx_52p5","mass 52p5");
                auto hsnx_5 = new THStack("hsnx_5p25","mass 5p25");
                auto hsdx_5 = new THStack("hsdx_5p25","mass 5p25");
                bool sigpass_60= false;
                bool sigpass_6 =false;
                bool sigpass_52= false;
                bool sigpass_5 = false;
                bool firstpass = true;
                TString SigTotal_name;
                TString SigTotal_xname = hsn->GetTitle(); 

                while((hsnx = (TH1F*)hsn_it())){
                    TString n1 = TString(hsnx->GetName());  
                    if (firstpass){ 
                        TObjArray* name_tolks = n1.Tokenize("_");
                        SigTotal_name = TString((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(3)))->String())).Append("_").Append(year).Append(TString("_SigTotal")));
                        firstpass=false;
                    }
                    if (n1.Contains("60p0")){hsnx_60->Add(hsnx); sigpass_60 =true;}
                    else if (n1.Contains("6p0")){hsnx_6->Add(hsnx);sigpass_6 =true;}
                    else if (n1.Contains("52p5")){hsnx_52->Add(hsnx);sigpass_52 =true;}
                    else if (n1.Contains("5p25")){hsnx_5->Add(hsnx);sigpass_5 =true;}
                }
                while((hsdx = (TH1F*)hsd_it())){
                    TString n2 = TString(hsdx->GetName());  
                    if (n2.Contains("60p0")){hsdx_60->Add(hsdx);}
                    else if (n2.Contains("6p0")){hsdx_6->Add(hsdx);}
                    else if (n2.Contains("52p5")){hsdx_52->Add(hsdx);}
                    else if (n2.Contains("5p25")){hsdx_5->Add(hsdx);}
                }

                canvases[SigTotal_name.Data()] = std::make_unique<TCanvas>(Form("canvas_%s_%s", SigTotal_name.Data(),year.Data()));
                auto * c = canvases[SigTotal_name.Data()].get();
                canvases[TString(SigTotal_name.Data()).Append("_zoom")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_zoom", SigTotal_name.Data(),year.Data()));
                auto * cz = canvases[TString(SigTotal_name.Data()).Append("_zoom")].get();
                bool firstdraw = true; 
                  TF1* f1 = new TF1("f1","0.5*[2]*(TMath::Erf((x-[0])/[1]) +1)",lowfit,highfit);
                  f1->SetParameters(33,36,.99);

                  TF1* f2 = new TF1("f2","0.5*[2]*(TMath::TanH((x-[0])/[1]) +1)",lowfit,highfit);
                  TF1* f3 = new TF1("f3","0.5*[2]*(TMath::ATan((x-[0])/[1]) +1.6)",lowfit,highfit);
                  TF1* f4 = new TF1("f4","0.5*[2]*(TMath::ATan(TMath::TanH((x-[0])/[1]) +1))",lowfit,highfit);
                  TF1* f5 = new TF1("f5","0.5*[2]*((1/(1+TMath::Exp(-1*(x-[0])/[1]))) +1)",lowfit,highfit);
                  f2->SetParameters(33,36,.99);
                  f2->SetLineColor(4);
                  f3->SetParameters(120,2,.42);
                  f3->SetLineColor(3);
                  f4->SetParameters(33,40,.99);
                  f4->SetLineColor(5);
                  f5->SetParameters(30,30,.89);
                  f5->SetLineColor(6);

                    if (sigpass_60){
                        TH1F * SigTotal_num_60 = (TH1F*)(((TH1F*)hsnx_60->GetStack()->Last())->Clone());
                        TH1F * SigTotal_denom_60 = (TH1F*)(((TH1F*)hsdx_60->GetStack()->Last())->Clone());
                        SigTotal_num_60->GetXaxis()->SetTitle(SigTotal_xname); 
                        SigTotal_denom_60->GetXaxis()->SetTitle(SigTotal_xname);   
                        for (int i =0; i<=SigTotal_num_60->GetNbinsX();i++){ 
//                          if(SigTotal_num_60->GetBinContent(i)== 0){SigTotal_num_60->SetBinContent(i,0.00000001);} //sig added
//                          if(SigTotal_denom_60->GetBinContent(i)== 0){SigTotal_denom_60->SetBinContent(i,1);} //sig added
                          if(SigTotal_num_60->GetBinContent(i)>SigTotal_denom_60->GetBinContent(i)){
                            SigTotal_denom_60->SetBinContent(i,SigTotal_num_60->GetBinContent(i));
                          }
                        }
                        TEfficiency* sig_eff_60 = new TEfficiency(*SigTotal_num_60,*SigTotal_denom_60);
                        sig_eff_60->SetLineColor(common::group_plot_info["sig_60p0_1"].color);
                        sig_eff_60->SetMarkerColor(common::group_plot_info["sig_60p0_1"].color);
                        sig_eff_60->SetFillColor(common::group_plot_info["sig_60p0_1"].color);
                        if (sigfit){ f1->SetLineColor(sig_eff_60->GetLineColor()); sig_eff_60->Fit(f1,"SAME ER+");}
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_60,c,firstdraw,false,year);   
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_60,cz,firstdraw,true,year);   
                        firstdraw = false;
                    }
                    if (sigpass_6){
                        TH1F * SigTotal_num_6 = (TH1F*)(((TH1F*)hsnx_6->GetStack()->Last())->Clone());
                        TH1F * SigTotal_denom_6 = (TH1F*)(((TH1F*)hsdx_6->GetStack()->Last())->Clone());
                        SigTotal_num_6->GetXaxis()->SetTitle(SigTotal_xname); 
                        SigTotal_denom_6->GetXaxis()->SetTitle(SigTotal_xname);   
                        for (int i =0; i<=SigTotal_num_6->GetNbinsX();i++){ 
//                          if(SigTotal_num_6->GetBinContent(i)== 0){SigTotal_num_6->SetBinContent(i,0.00000001);} //sig added
//                          if(SigTotal_denom_6->GetBinContent(i)== 0){SigTotal_denom_6->SetBinContent(i,1);} //sig added
                          if(SigTotal_num_6->GetBinContent(i)>SigTotal_denom_6->GetBinContent(i)){
                            SigTotal_denom_6->SetBinContent(i,SigTotal_num_6->GetBinContent(i));
                          }
                        }
                        TEfficiency* sig_eff_6 = new TEfficiency(*SigTotal_num_6,*SigTotal_denom_6);
                        sig_eff_6->SetLineColor(common::group_plot_info["sig_6p0_1"].color);
                        sig_eff_6->SetMarkerColor(common::group_plot_info["sig_6p0_1"].color);
                        sig_eff_6->SetFillColor(common::group_plot_info["sig_6p0_1"].color);
                        if (sigfit){ f1->SetLineColor(sig_eff_6->GetLineColor()); sig_eff_6->Fit(f1,"SAME ER+");}
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_6,c,firstdraw,false,year);   
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_6,cz,firstdraw,true,year);   
                        firstdraw = false;
                    }
                    if (sigpass_52){
                        TH1F * SigTotal_num_52 = (TH1F*)(((TH1F*)hsnx_52->GetStack()->Last())->Clone());
                        TH1F * SigTotal_denom_52 = (TH1F*)(((TH1F*)hsdx_52->GetStack()->Last())->Clone());
                        SigTotal_num_52->GetXaxis()->SetTitle(SigTotal_xname); 
                        SigTotal_denom_52->GetXaxis()->SetTitle(SigTotal_xname);   
                        for (int i =0; i<=SigTotal_num_52->GetNbinsX();i++){ 
//                          if(SigTotal_num_52->GetBinContent(i)== 0){SigTotal_num_52->SetBinContent(i,0.00000001);} //sig added
//                          if(SigTotal_denom_52->GetBinContent(i)== 0){SigTotal_denom_52->SetBinContent(i,1);} //sig added
                          if(SigTotal_num_52->GetBinContent(i)>SigTotal_denom_52->GetBinContent(i)){
                            SigTotal_denom_52->SetBinContent(i,SigTotal_num_52->GetBinContent(i));
                          }
                        }
                        TEfficiency* sig_eff_52 = new TEfficiency(*SigTotal_num_52,*SigTotal_denom_52);
                        sig_eff_52->SetLineColor(common::group_plot_info["sig_52p5_1"].color);
                        sig_eff_52->SetMarkerColor(common::group_plot_info["sig_52p5_1"].color);
                        sig_eff_52->SetFillColor(common::group_plot_info["sig_52p5_1"].color);
                        if (sigfit){ f1->SetLineColor(sig_eff_52->GetLineColor()); sig_eff_52->Fit(f1,"SAME ER+");}
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_52,c,firstdraw,false,year);   
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_52,cz,firstdraw,true,year);   
                        firstdraw = false;
                    }
                    if (sigpass_5){
                        TH1F * SigTotal_num_5 = (TH1F*)(((TH1F*)hsnx_5->GetStack()->Last())->Clone());
                        TH1F * SigTotal_denom_5 = (TH1F*)(((TH1F*)hsdx_5->GetStack()->Last())->Clone());
                        SigTotal_num_5->GetXaxis()->SetTitle(SigTotal_xname); 
                        SigTotal_denom_5->GetXaxis()->SetTitle(SigTotal_xname);  
                        //std::cout<<SigTotal_xname<<std::endl; 
                        for (int i =0; i<=SigTotal_num_5->GetNbinsX();i++){ 
//                          if(SigTotal_num_5->GetBinContent(i)== 0){SigTotal_num_5->SetBinContent(i,0.00000001);} //sig added
//                          if(SigTotal_denom_5->GetBinContent(i)== 0){SigTotal_denom_5->SetBinContent(i,1);} //sig added
                          if(SigTotal_num_5->GetBinContent(i)>SigTotal_denom_5->GetBinContent(i)){
                            SigTotal_denom_5->SetBinContent(i,SigTotal_num_5->GetBinContent(i));
                          }
                        }
                        TEfficiency* sig_eff_5 = new TEfficiency(*SigTotal_num_5,*SigTotal_denom_5);
                        sig_eff_5->SetLineColor(common::group_plot_info["sig_5p25_1"].color);
                        sig_eff_5->SetMarkerColor(common::group_plot_info["sig_5p25_1"].color);
                        sig_eff_5->SetFillColor(common::group_plot_info["sig_5p25_1"].color);
                        if (sigfit){ f1->SetLineColor(sig_eff_5->GetLineColor()); sig_eff_5->Fit(f1,"SAME ER+");}
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_5,c,firstdraw,false,year);   
                        canvasDraw(SigTotal_name.Data(),TString("SIGNAL"),sig_eff_5,cz,firstdraw,true,year);   
                        firstdraw = false;
                    }
            } // end sig Total
            else if (hs_name.Contains("DATA"))
                type = TString("DATA");
            else {
                type = TString("BKG");
                TH1F * MCTotal_num = (TH1F*)(((TH1F*)hsn->GetStack()->Last())->Clone());
                TH1F * MCTotal_denom = (TH1F*)(((TH1F*)hsd->GetStack()->Last())->Clone());
                TObjArray* name_tolks = TString(MCTotal_denom->GetName()).Tokenize("_");
                MCTotal_denom->SetName((((TObjString*)(name_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(name_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(name_tolks->At(3)))->String())).Append("_").Append(year).Append(TString("_MCTotal")));
            
                for (int i =0; i<=MCTotal_num->GetNbinsX();i++){ 
                  //if(MCTotal_num->GetBinContent(i)==0){MCTotal_num->SetBinContent(i,0.0000000001);}
                  //if(MCTotal_denom->GetBinContent(i)==0){MCTotal_denom->SetBinContent(i,1);}
                  if(MCTotal_num->GetBinContent(i)>MCTotal_denom->GetBinContent(i)){
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
                  TF1* f1 = new TF1("f1","0.5*[2]*(TMath::Erf((x-[0])/[1]) +1)",lowfit,highfit);
                  f1->SetParameters(33,36,.99);
                  f1->SetLineColor(mc_eff->GetLineColor());
                  mc_eff->Fit(f1,"SAME ER+");

                  TF1* f2 = new TF1("f2","0.5*[2]*(TMath::TanH((x-[0])/[1]) +1)",lowfit,highfit);
                  TF1* f3 = new TF1("f3","0.5*[2]*(TMath::ATan((x-[0])/[1]) +1.6)",lowfit,highfit);
                  TF1* f4 = new TF1("f4","0.5*[2]*(TMath::ATan(TMath::TanH((x-[0])/[1]) +1))",lowfit,highfit);
                  TF1* f5 = new TF1("f5","0.5*[2]*((1/(1+TMath::Exp(-1*(x-[0])/[1]))) +1)",lowfit,highfit);
                  f2->SetParameters(33,36,.99);
                  f2->SetLineColor(4);
                  f3->SetParameters(120,2,.42);
                  f3->SetLineColor(3);
                  f4->SetParameters(33,40,.99);
                  f4->SetLineColor(5);
                  f5->SetParameters(30,30,.89);
                  f5->SetLineColor(6);
                  //mc_eff->Fit(f2,"SAME ER+");
                  //mc_eff->Fit(f3,"SAME ER+");
                  //mc_eff->Fit(f4,"SAME ER+");
                  //mc_eff->Fit(f5,"SAME ER+");
                }

                canvases[MCTotal_denom->GetName()] = std::make_unique<TCanvas>(Form("canvas_%s_%s", MCTotal_denom->GetName(),year.Data()));
                auto * c = canvases[MCTotal_denom->GetName()].get();
                canvasDraw(MCTotal_denom->GetName(),TString("BKG"),mc_eff,c,true,false,year);   
                canvases[TString(MCTotal_denom->GetName()).Append("_zoom")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_zoom", MCTotal_denom->GetName(),year.Data()));
                auto * cz = canvases[TString(MCTotal_denom->GetName()).Append("_zoom")].get();
                canvasDraw(MCTotal_denom->GetName(),TString("BKG"),mc_eff,cz,true,true,year);   
            } // end MC Total

            fit = true;
            TIter hsn_it((TList*)(((THStack*)hsn)->GetHists()));
            TH1F *hsnx;
            while((hsnx = (TH1F*)hsn_it())){
                TString n1 = TString(hsnx->GetName());  
                if (n1.Contains("sig")) continue;
                TString n1x0 = ((TObjString*)(n1.Tokenize("_")->At(2)))->String();
                TString n1x1 = ((TObjString*)(n1.Tokenize("_")->At(5)))->String();
                TIter hsd_it((TList*)(((THStack*)hsd)->GetHists()));
                TH1F *hsdx;
                while((hsdx = (TH1F*)hsd_it())){
                    TString n2 = TString(hsdx->GetName());  
                    TString n2x0 = ((TObjString*)(n2.Tokenize("_")->At(2)))->String();
                    TString n2x1 = ((TObjString*)(n2.Tokenize("_")->At(5)))->String();
                    if (((n1x0.Contains(n2x0)) && (n1x1.Contains(n2x1)))) {
                      for (int i =0; i<=hsnx->GetNbinsX();i++){    
                          if(hsdx->GetBinContent(i)==0){hsdx->SetBinContent(i,1);}
                          if(hsnx->GetBinContent(i)==0){hsnx->SetBinContent(i,0.0000000001);}
                          if(hsnx->GetBinContent(i)>hsdx->GetBinContent(i)){
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
                          TF1* f1 = new TF1("f1","0.5*[2]*(TMath::Erf((x-[0])/[1]) +1)",lowfit,highfit);
                          f1->SetParameters(25,36,1.0);
                          f1->SetLineColor(hs->GetLineColor());
                          hs->Fit(f1,"SAME R+");

                          TF1* f2 = new TF1("f2","0.5*[2]*(TMath::TanH((x-[0])/[1]) +1)",lowfit,highfit);
                          TF1* f3 = new TF1("f3","0.5*[2]*(TMath::ATan((x-[0])/[1]) +1.6)",lowfit,highfit);
                          TF1* f4 = new TF1("f4","0.5*[2]*(2*TMath::ATan(TMath::TanH((x-[0])/[1]) +1))",lowfit,highfit);
                          TF1* f5 = new TF1("f5","0.5*[2]*((1/(1+TMath::Exp(-1*(x-[0])/[1]))) +1)",lowfit,highfit);
                          f2->SetParameters(33,36,.99);
                          f2->SetLineColor(3);
                          f3->SetParameters(120,2,.42);
                          f3->SetLineColor(4);
                          f4->SetParameters(33,40,.99);
                          f4->SetLineColor(5);
                          f5->SetParameters(30,30,.99);
                          f5->SetLineColor(6);
                          //hs->Fit(f2,"SAME ER+");
                          //hs->Fit(f3,"SAME ER+");
                          //hs->Fit(f4,"SAME ER+");
                          //hs->Fit(f5,"SAME ER+");
                      }
                
             
                      TString hs_basename = n1x0;
                      hs_basename.Append(year);
                      TString hs_suffix = n1x1; 
                      bool newCanvas = false;
                      if (canvases.find(hs_basename) == canvases.end()) {
                          newCanvas = true;
                          canvases[hs_basename] = std::make_unique<TCanvas>(Form("canvas_%s_%s", hs_basename.Data(),year.Data()));
                          auto * c = canvases[hs_basename].get();
                          canvases[TString(hs_basename).Append("_zoom")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_zoom", hs_basename.Data(),year.Data()));
                          auto * cz = canvases[TString(hs_basename).Append("_zoom")].get();
                      }
                      auto * c = canvases[hs_basename].get();
                      canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),c,newCanvas,false,year); 
                      auto * cz = canvases[TString(hs_basename).Append("_zoom")].get();
                      canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),cz,newCanvas,true,year); 
                      if(type.Contains("DATA")){
                          TObjArray* dataname_tolks = TString(hs->GetName()).Tokenize("_");
                          TString MCTotal_dataname = TString((((TObjString*)(dataname_tolks->At(0)))->String()).Append("_").Append((((TObjString*)(dataname_tolks->At(1)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(2)))->String())).Append("_").Append((((TObjString*)(dataname_tolks->At(3)))->String())).Append("_").Append(year).Append(TString("_MCTotal")));
                             
                          auto * c1 = canvases[MCTotal_dataname].get();
                          canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),c1,newCanvas,false,year);    
                          auto * cz1 = canvases[TString(MCTotal_dataname).Append("_zoom")].get();
                          canvasDraw(n1x0,n1x1,(TEfficiency*)hs->Clone(),cz1,newCanvas,true,year);    
                      }
                    }// end if same plot
                }// end while denominator
            }// end while numerator
      }// end for list of keys (all histograms) 
        

      // Make ratio subplot
        for (auto & pair : canvases) {
            auto * c = pair.second.get();
            TPad * top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));
            //c->ls();
            TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
            bool zoom = canvas_name.Contains("zoom")? true: false;
            TEfficiency * data_hist;
            bool sig =false;
            bool data = false;
            bool mc = false;
            vector<TEfficiency*> MC_hists;
            vector<TEfficiency*> sig_hists;
            TIter next((TList*)top_pad->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("TEfficiency")) continue;
                if (!TString(hs->GetName()).Contains("_")) continue;
                if (TString(hs->GetName()).Contains("data")){
                   data_hist = (TEfficiency*)hs;
                   data = true;
                }
                else if (TString(hs->GetName()).Contains("sig")){
                   sig_hists.push_back((TEfficiency*)hs);
                   sig= true;
                }
                else{
                    MC_hists.push_back((TEfficiency*)hs);
                    mc=true;
                }
            }
            if (data && mc) {
                c->cd(2); // switch to bottom pad
                int nbins;
                TH1F* data_ratio;
                   data_ratio = (TH1F*)((TH1F*)data_hist->GetPassedHistogram())->Clone();
                   nbins= data_hist->GetPassedHistogram()->GetNbinsX();
                   data_ratio->Reset();
                    for (int i =0; i<=nbins;i++){    
                        data_ratio->SetBinContent(i,data_hist->GetEfficiency(i));
                    }
                data_ratio->Sumw2();
                bool first = true;
                for( auto MC_hist : MC_hists){
                    TH1F* ratio_hist = (TH1F*)data_ratio->Clone();
                    TH1F* MC_ratio = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();    
                    ratio_hist->GetXaxis()->SetTitle(MC_hist->GetPassedHistogram()->GetXaxis()->GetTitle());
                    MC_ratio->Reset();
                    for (int i =0; i<=nbins;i++){    
                        MC_ratio->SetBinContent(i,MC_hist->GetEfficiency(i));
                    }
                    MC_ratio->Sumw2();
                    ratio_hist->Divide(MC_ratio);
                  
                    ratio_hist->ResetAttFill();
                    if (zoom){
                            ratio_hist->SetMaximum(1.01);
                            ratio_hist->SetMinimum(0.94);
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
//                            // MC errors
//                            TH1F * MC_error = (TH1F*)h2->Clone();
//                            MC_error->Divide(MC_error);
//                            MC_error->SetMarkerSize(0);
//                            MC_error->SetFillStyle(3254);
//                            MC_error->SetFillColor(kGray+3);
//                            MC_error->Draw("SAME E2");
                } 
            }
            if (sig){
                c->cd(2);
                for( auto sig_hist : sig_hists){
                    TH1F* ratio_hist = (TH1F*)((TH1F*)sig_hist->GetPassedHistogram())->Clone();    
                    TH1F* sig_ratio = (TH1F*)((TH1F*)sig_hist->GetPassedHistogram())->Clone();    
                    ratio_hist->GetXaxis()->SetTitle(sig_hist->GetPassedHistogram()->GetXaxis()->GetTitle());
                    sig_ratio->Reset();
                    int nbins = ratio_hist->GetNbinsX();
                    //for (int i =0; i<nbins;i++){    
                    //    sig_ratio->SetBinContent(i,sig_hist->GetEfficiency(i));
                    //    ratio_hist->SetBinContent(i,sig_hist->GetEfficiency(i));
                    //}
                    //sig_ratio->Sumw2();
                    //ratio_hist->Sumw2();
                    //ratio_hist->Divide(sig_ratio);
                    //for (int i =0; i<nbins;i++){    
                    //    std::cout<<ratio_hist->GetBinContent(i)<<std::endl;
                    //    ratio_hist->SetBinError(i,1);
                    //}
                  
                    ratio_hist->ResetAttFill();
                    if (zoom){
                            ratio_hist->SetMaximum(1.01);
                            ratio_hist->SetMinimum(0.94);
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
                    TString group = ((TObjString*)(TString(sig_hist->GetName()).Tokenize("_")->At(5)))->String();
                    group.Append("_").Append(((TObjString*)(TString(sig_hist->GetName()).Tokenize("_")->At(6)))->String()).Append("_").Append(((TObjString*)(TString(sig_hist->GetName()).Tokenize("_")->At(7)))->String());
                    ratio_hist->SetLineColor(common::group_plot_info[group].color);
                    ratio_hist->SetMarkerColor(common::group_plot_info[group].color);
                    ratio_hist->SetFillStyle(common::group_plot_info[group].style); 
                    ratio_hist->Draw("HIST P SAME"); //keep blank for now
                            
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
//                            // MC errors
//                            TH1F * MC_error = (TH1F*)h2->Clone();
//                            MC_error->Divide(MC_error);
//                            MC_error->SetMarkerSize(0);
//                            MC_error->SetFillStyle(3254);
//                            MC_error->SetFillColor(kGray+3);
//                            MC_error->Draw("SAME E2");
                } 
            }
        }

    // Make ratio main plots (weights)
   bool make_weights= false;
  if(make_weights){
        for (auto & pair : canvases) {

            auto * c = pair.second.get();
            //c->ls();
            TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
            if (canvas_name.Contains("ratio")){ continue;}
            if (!canvas_name.Contains("MCTotal")) {continue;}
            if (canvas_name.Contains("zoom")) {continue;}
            TPad* top_pad;
            //try{TPad * top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));}
            try{top_pad = (TPad*)c->GetPrimitive(Form("%s_1", c->GetName()));}
            catch(...){continue;}
        //c->ls();
            //TString canvas_name = TString(c->GetName()).ReplaceAll("canvas_", "");
            //if (canvas_name.Contains("ratio")){ continue;}
        //bool zoom = canvas_name.Contains("zoom")? true: false;
            TEfficiency * data_hist;
            vector<TEfficiency*> MC_hists;
        TIter next((TList*)top_pad->GetListOfPrimitives());
            TObject * hs;
            while ((hs = next())) {
                if (TString(hs->ClassName()) != TString("TEfficiency")) continue;
                if (!TString(hs->GetName()).Contains("_")) continue;
                if (TString(hs->GetName()).Contains("sig")) continue;
                if (TString(hs->GetName()).Contains("data")){
                    data_hist = (TEfficiency*)hs;
                }
                else if (TString(hs->GetName()).Contains("MC")){
                    MC_hists.push_back((TEfficiency*)hs);}
                }
            if (data_hist && !MC_hists.empty()) {
                //c->cd(2); // switch to bottom pad
            //canvases[canvas_name.Append("ratio_1")] = std::make_unique<TCanvas>(Form("canvas_%s_%s_ratio1", canvas_name.Data(),year.Data()));
                TCanvas* c1 = new TCanvas(Form("canvas_%s_%s_ratio1", canvas_name.Data(),year.Data()),Form("canvas_%s_%s_ratio1", canvas_name.Data(),year.Data()));
                TCanvas* c2 = new TCanvas(Form("canvas_%s_%s_ratio2", canvas_name.Data(),year.Data()),Form("canvas_%s_%s_ratio2", canvas_name.Data(),year.Data()));
                TCanvas* c3 = new TCanvas(Form("canvas_%s_%s_ratio3", canvas_name.Data(),year.Data()),Form("canvas_%s_%s_ratio3", canvas_name.Data(),year.Data()));
                TCanvas* c4 = new TCanvas(Form("canvas_%s_%s_ratio4", canvas_name.Data(),year.Data()),Form("canvas_%s_%s_ratio4", canvas_name.Data(),year.Data()));
                TCanvas* c5 = new TCanvas(Form("canvas_%s_%s_ratio5", canvas_name.Data(),year.Data()),Form("canvas_%s_%s_ratio5", canvas_name.Data(),year.Data()));
                //auto * c1 = canvases[canvas_name.Append("ratio_1")].get();
                //c1->cd(1);
                //gPad->SetPad(0.01, 0.33, 0.99, 0.99);
                gPad->Draw(); 
                gPad->cd();
                //c1->Divide(1,1);
        int nbins= data_hist->GetPassedHistogram()->GetNbinsX();    
        TH1F* data_ratio = (TH1F*)((TH1F*)data_hist->GetPassedHistogram())->Clone();
        TH1F* data_ratio_low = (TH1F*)((TH1F*)data_hist->GetPassedHistogram())->Clone();
        TH1F* data_ratio_high = (TH1F*)((TH1F*)data_hist->GetPassedHistogram())->Clone();
        data_ratio->Reset();
        data_ratio_low->Reset();
        data_ratio_high->Reset();
        for (int i =0; i<nbins;i++){    
        data_ratio->SetBinContent(i,data_hist->GetEfficiency(i));
        data_ratio_low->SetBinContent(i,data_hist->GetEfficiency(i)-data_hist->GetEfficiencyErrorLow(i));
        data_ratio_high->SetBinContent(i,data_hist->GetEfficiency(i)+data_hist->GetEfficiencyErrorUp(i));
        }
                //data_ratio->Sumw2();
                //data_ratio_low->Sumw2();
                //data_ratio_high->Sumw2();
        bool first = true;
        for( auto MC_hist : MC_hists){
        TH1F* ratio_hist = (TH1F*)data_ratio->Clone();
        TH1F* ratio_hist_Mlow = (TH1F*)data_ratio->Clone();
        TH1F* ratio_hist_Mhigh = (TH1F*)data_ratio->Clone();
        TH1F* ratio_hist_Dlow = (TH1F*)data_ratio_low->Clone();
        TH1F* ratio_hist_Dhigh = (TH1F*)data_ratio_high->Clone();
        TH1F* MC_ratio = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();    
        TH1F* MC_ratio_low = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();    
        TH1F* MC_ratio_high = (TH1F*)((TH1F*)MC_hist->GetPassedHistogram())->Clone();    
        ratio_hist->GetXaxis()->SetTitle(MC_hist->GetPassedHistogram()->GetXaxis()->GetTitle());
        MC_ratio->Reset();
        MC_ratio_low->Reset();
        MC_ratio_high->Reset();
        for (int i =0; i<nbins;i++){    
        MC_ratio->SetBinContent(i,MC_hist->GetEfficiency(i));
        MC_ratio_low->SetBinContent(i,MC_hist->GetEfficiency(i)-MC_hist->GetEfficiencyErrorLow(i));
        MC_ratio_high->SetBinContent(i,MC_hist->GetEfficiency(i)+MC_hist->GetEfficiencyErrorUp(i));
        }
                //MC_ratio->Sumw2();
                //MC_ratio_low->Sumw2();
                //MC_ratio_high->Sumw2();
                ratio_hist->Divide(MC_ratio);
                ratio_hist_Mlow->Divide(MC_ratio_low);
                ratio_hist_Mhigh->Divide(MC_ratio_high);
                ratio_hist_Dlow->Divide(MC_ratio);
                ratio_hist_Dhigh->Divide(MC_ratio);
                ratio_hist->ResetAttFill();
                ratio_hist_Mlow->ResetAttFill();
                ratio_hist_Mhigh->ResetAttFill();
                ratio_hist_Dlow->ResetAttFill();
                ratio_hist_Dhigh->ResetAttFill();
        //bool zoom = false;
        //if (zoom){
        //        ratio_hist->SetMaximum(1.01);
        //        ratio_hist->SetMinimum(0.94);
        //}
        //else{
        //        ratio_hist->SetMaximum(2.1);
        //        ratio_hist->SetMinimum(0.1);
                //ratio_hist->SetMinimum(0.5);
                ratio_hist->SetMinimum(0);
                ratio_hist->SetMaximum(1.01);
                ratio_hist_Mlow->SetMinimum(0);
                ratio_hist_Mlow->SetMaximum(1.01);
                ratio_hist_Mhigh->SetMinimum(0);
                ratio_hist_Mhigh->SetMaximum(1.01);
                ratio_hist_Dlow->SetMinimum(0);
                ratio_hist_Dlow->SetMaximum(1.01);
                ratio_hist_Dhigh->SetMinimum(0);
                ratio_hist_Dhigh->SetMaximum(1.01);
        //}
                
                ratio_hist->GetXaxis()->SetRangeUser(10,1000);
                ratio_hist_Dlow->GetXaxis()->SetRangeUser(10,1000);
                ratio_hist_Dhigh->GetXaxis()->SetRangeUser(10,1000);
                ratio_hist_Mlow->GetXaxis()->SetRangeUser(10,1000);
                ratio_hist_Mhigh->GetXaxis()->SetRangeUser(10,1000);
        ratio_hist->SetTitle("");
                //ratio_hist->GetXaxis()->SetTitleSize(0.12);
                //ratio_hist->GetXaxis()->SetLabelSize(0.11);
                ratio_hist->GetYaxis()->SetTitle("Data/MC");
                //ratio_hist->GetYaxis()->SetTitleSize(0.12);
                //ratio_hist->GetYaxis()->SetLabelSize(0.11);
                //ratio_hist->GetYaxis()->SetNdivisions(5);
                ratio_hist->GetYaxis()->SetTitleOffset(0.35);
        //ratio_hist->SetMarkerColor(common::group_plot_info[((TObjString*)(TString(MC_hist->GetName()).Tokenize("_")->At(5)))->String()].color);
        ratio_hist->SetMarkerColor(2);//red
        ratio_hist_Dlow->SetMarkerColor(3);//green
        ratio_hist_Dhigh->SetMarkerColor(4);//blue
        ratio_hist_Mlow->SetMarkerColor(6);//magenta
        ratio_hist_Mhigh->SetMarkerColor(7);//cyan
                c1->cd();
                c1->SetLogx();
                ratio_hist->Draw("EP SAME");
                c2->cd();
                c2->SetLogx();
                ratio_hist_Mlow->Draw("EP SAME");
                c3->cd();
                c3->SetLogx();
                ratio_hist_Mhigh->Draw("EP SAME");
                c4->cd();
                c4->SetLogx();
                ratio_hist_Dlow->Draw("EP SAME");
                c5->cd();
                c5->SetLogx();
                ratio_hist_Dhigh->Draw("EP SAME");
        if((canvas_name.Contains("MCTotal")) && !(canvas_name.Contains("zoom"))){        
        ratio_hist->      Write(Form("weights_%s_ratio",canvas_name.Data()));
        ratio_hist_Dhigh->Write(Form("weigts_%s_Dhigh" ,canvas_name.Data()));
        ratio_hist_Dlow-> Write(Form("weigts_%s_Dlow"  ,canvas_name.Data()));
        ratio_hist_Mhigh->Write(Form("weigts_%s_Mhigh" ,canvas_name.Data()));
        ratio_hist_Mlow-> Write(Form("weigts_%s_Mlow"  ,canvas_name.Data()));
        }   
        //TLine * ratio_line = new TLine();
        //        ratio_line->SetLineColor(kRed);
        //        ratio_line->SetLineWidth(2);
        //        // set line params
        //        ratio_line->SetX1(ratio_hist->GetXaxis()->GetXmin());
        //        ratio_line->SetX2(ratio_hist->GetXaxis()->GetXmax());
        //        ratio_line->SetY1(1.0);
        //        ratio_line->SetY2(1.0);
        //        ratio_line->Draw("P SAME");
//
//                // MC errors
//                TH1F * MC_error = (TH1F*)h2->Clone();
//                MC_error->Divide(MC_error);
//                MC_error->SetMarkerSize(0);
//                MC_error->SetFillStyle(3254);
//                MC_error->SetFillColor(kGray+3);
//                MC_error->Draw("SAME E2");
            auto legend = new TLegend(0.6,0.2,0.8,0.5);
            legend->AddEntry(ratio_hist,"weights", "lep");
            legend->AddEntry(ratio_hist_Mlow,"weights MC low", "lep");
            legend->AddEntry(ratio_hist_Mhigh,"weights MC high", "lep");
            legend->AddEntry(ratio_hist_Dlow,"weights data low", "lep");
            legend->AddEntry(ratio_hist_Dhigh,"weights data high", "lep");
            legend->Draw("SAME");
            } 
            c1->Write();
            c2->Write();
            c3->Write();
            c4->Write();
            c5->Write();

            }

        }
    }//end weights if
      
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
                //TString plateau; 
                //TString mu;
                //TString sigma; 

                if (TString(hs->GetName()).Contains("sig")){
                    TString group = ((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(5)))->String();
                    group.Append("_").Append(((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(6)))->String());
                    //std::cout<<"group: "<<group.Data()<<std::endl;
                    const char* leg = common::group_plot_info[group.Data()].legend.Data();
                    legend->AddEntry(hs, leg, "lep");
                }

                else{
                    TString group = ((TObjString*)(TString(hs->GetName()).Tokenize("_")->At(5)))->String();
                    const char* leg = common::group_plot_info[group].legend.Data();
                    float param0 = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParameter(0);
                    float param0err =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(0);
                    float param1 = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParameter(1);
                    float param1err =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(1);
                    float param2 = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParameter(2);
                    float param2err =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(0)))->GetParError(2);
                    TString plateau = TString(Form("#epsilon: %.4f +/- %.4f",param2,param2err));
                    TString mu =      TString(Form("#mu: %.2f +/- %.2f",param0,param0err));
                    TString sigma =   TString(Form("#sigma: %.2f +/- %.2f",param1,param1err));
        
        //float param0x = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParameter(0);
        //float param0errx =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParError(0);
        //float param1x = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParameter(1);
        //float param1errx =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParError(1);
        //float param2x = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParameter(2);
        //float param2errx =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1)))->GetParError(2);        
        //TString plateaux = TString(Form("#epsilon: %.4f +/- %.4f",param2x,param2errx));
        //TString mux =      TString(Form("#mu: %.2f +/- %.2f",param0x,param0errx));
        //TString sigmax =   TString(Form("#sigma: %.2f +/- %.2f",param1x,param1errx));
//
//        float param0y = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParameter(0);
//        float param0erry =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParError(0);
//        float param1y = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParameter(1);
//        float param1erry =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParError(1);
//        float param2y = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParameter(2);
//        float param2erry =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2)))->GetParError(2);
//        TString plateauy = TString(Form("#epsilon: %.4f +/- %.4f",param2y,param2erry));
//        TString muy =      TString(Form("#mu: %.2f +/- %.2f",param0y,param0erry));
//        TString sigmay =   TString(Form("#sigma: %.2f +/- %.2f",param1y,param1erry));
//
//        float param0z = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParameter(0);
//        float param0errz =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParError(0);
//        float param1z = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParameter(1);
//        float param1errz =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParError(1);
//        float param2z = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParameter(2);
//        float param2errz =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3)))->GetParError(2);
//        TString plateauz = TString(Form("#epsilon: %.4f +/- %.4f",param2z,param2errz));
//        TString muz =      TString(Form("#mu: %.2f +/- %.2f",param0z,param0errz));
//        TString sigmaz =   TString(Form("#sigma: %.2f +/- %.2f",param1z,param1errz));
//
//        float param0w = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParameter(0);
//        float param0errw =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParError(0);
//        float param1w = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParameter(1);
//        float param1errw =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParError(1);
//        float param2w = ((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParameter(2);
//        float param2errw =((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4)))->GetParError(2);
//        TString plateauw = TString(Form("#epsilon: %.4f +/- %.4f",param2w,param2errw));
//        TString muw =      TString(Form("#mu: %.2f +/- %.2f",param0w,param0errw));
//        TString sigmaw =   TString(Form("#sigma: %.2f +/- %.2f",param1w,param1errw));
                if (TString(hs->GetName()).Contains("data")){
                    legend->AddEntry(hs, Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}",leg,plateau.Data(),mu.Data(),sigma.Data()), "lep");
                    //legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1))), Form("#splitline{%s}{%s}","TanH",plateaux.Data()), "lep");
                    }
                else{
                    //legend->AddEntry(hs, Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}",leg,plateau.Data(),mu.Data(),sigma.Data()), "lep");
                    //legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1))), Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}","TanH",plateaux.Data(),mux.Data(),sigmax.Data()), "lep");
                    //legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2))), Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}","ATan",plateauy.Data(),muy.Data(),sigmay.Data()), "lep");
                    //legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3))), Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}","Gd",plateauz.Data(),muz.Data(),sigmaz.Data()), "lep");
                    //legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4))), Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}","Log",plateauw.Data(),muw.Data(),sigmaw.Data()), "lep");
                    //legend->AddEntry(hs, Form("#splitline{%s}{`%s}",leg,plateau.Data()), "lep");
                    legend->AddEntry(hs, Form("#splitline{%s}{#splitline{%s}{#splitline{%s}{%s}}}",leg,plateau.Data(),mu.Data(),sigma.Data()), "lep");
             //       legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(1))), Form("#splitline{%s}{%s}","TanH",plateaux.Data()), "lep");
//                    legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(2))), Form("#splitline{%s}{%s}","ATan",plateauy.Data()), "lep");
//                    legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(3))), Form("#splitline{%s}{%s}","Gd",plateauz.Data()), "lep");
//                    legend->AddEntry(((TF1*)(((TEfficiency*)hs)->GetListOfFunctions()->At(4))), Form("#splitline{%s}{%s}","Log",plateauw.Data()), "lep");
                    }
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
