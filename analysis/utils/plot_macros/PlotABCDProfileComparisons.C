void PlotABCDProfileComparisons() {

    gROOT->LoadMacro("../tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle()");
    gStyle->SetPalette(1);

    TFile * file0 = TFile::Open("../../bin/PlotsTwelthRunApril27TestUseVxy/plots_CR_nJets_twelthrun_noData_2DHists_TestUseVxy_bkg_161718.root");
    TFile * file1 = TFile::Open("../../bin/PlotsTwelthRunApril27TestUseVxy/plots_SR_twelthrun_noData_2DHists_TestUseVxy_bkg_161718.root");
    TFile * file2 = TFile::Open("../../bin/PlotsTwelthRunApril27TestUseVxy/plots_CR_nJets_twelthrun_incompleteData_2DHists_TestUseVxy_161718.root");

    // 0 match nJets

    TCanvas * c0x = new TCanvas();

    THStack * hs0_0match = (THStack*)file0->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-BKG");
    TH2D * h0_0match = (TH2D*)hs0_0match->GetStack()->Last();
    TH1D * h0_X_0match = (TH1D*)h0_0match->ProjectionX();
    h0_X_0match->Rebin(4);
    h0_X_0match->SetMaximum(200);
    h0_X_0match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    h0_X_0match->GetXaxis()->SetTitleOffset(1.2);
    h0_X_0match->GetYaxis()->SetTitle("A. U.");
    h0_X_0match->SetLineWidth(2);
    h0_X_0match->SetMarkerSize(0.9);
    h0_X_0match->DrawNormalized("");

    THStack * hs1_0match = (THStack*)file1->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-BKG");
    TH2D * h1_0match = (TH2D*)hs1_0match->GetStack()->Last();
    TH1D * h1_X_0match = (TH1D*)h1_0match->ProjectionX();
    h1_X_0match->Rebin(4);
    h1_X_0match->SetLineColor(kRed);
    h1_X_0match->SetMarkerColor(kRed);
    h1_X_0match->SetLineWidth(2);
    h1_X_0match->SetMarkerSize(0.9);
    h1_X_0match->DrawNormalized("SAME");

    THStack * hs2_0match = (THStack*)file2->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-DATA");
    TH2D * h2_0match = (TH2D*)hs2_0match->GetStack()->Last();
    TH1D * h2_X_0match = (TH1D*)h2_0match->ProjectionX();
    h2_X_0match->Rebin(4);
    h2_X_0match->SetLineColor(kGreen+1);
    h2_X_0match->SetMarkerColor(kGreen+1);
    h2_X_0match->SetLineWidth(2);
    h2_X_0match->SetMarkerSize(0.9);
    h2_X_0match->DrawNormalized("SAME");

    TLegend * leg = c0x->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c0x->Modified();
    c0x->Update();

    TCanvas * c0y = new TCanvas();

    TH1D * h0_Y_0match = (TH1D*)h0_0match->ProjectionY();
    h0_Y_0match->Rebin(4);
    h0_Y_0match->SetMaximum(200);
    h0_Y_0match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    h0_Y_0match->GetXaxis()->SetTitleOffset(1.2);
    h0_Y_0match->GetYaxis()->SetTitle("A. U.");
    h0_Y_0match->SetLineWidth(2);
    h0_Y_0match->SetMarkerSize(0.9);
    h0_Y_0match->DrawNormalized("");

    TH1D * h1_Y_0match = (TH1D*)h1_0match->ProjectionY();
    h1_Y_0match->Rebin(4);
    h1_Y_0match->SetLineColor(kRed);
    h1_Y_0match->SetMarkerColor(kRed);
    h1_Y_0match->SetLineWidth(2);
    h1_Y_0match->SetMarkerSize(0.9);
    h1_Y_0match->DrawNormalized("SAME");

    TH1D * h2_Y_0match = (TH1D*)h2_0match->ProjectionY();
    h2_Y_0match->Rebin(4);
    h2_Y_0match->SetLineColor(kGreen+1);
    h2_Y_0match->SetMarkerColor(kGreen+1);
    h2_Y_0match->SetLineWidth(2);
    h2_Y_0match->SetMarkerSize(0.9);
    h2_Y_0match->DrawNormalized("SAME");

    leg = c0y->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c0y->Modified();
    c0y->Update();
    
    // 1 match nJets

    TCanvas * c1x = new TCanvas();

    THStack * hs0_1match = (THStack*)file0->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut29-BKG");
    TH2D * h0_1match = (TH2D*)hs0_1match->GetStack()->Last();
    TH1D * h0_X_1match = (TH1D*)h0_1match->ProjectionX();
    h0_X_1match->Rebin(4);
    h0_X_1match->SetMaximum(300);
    h0_X_1match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    h0_X_1match->GetXaxis()->SetTitleOffset(1.2);
    h0_X_1match->GetYaxis()->SetTitle("A. U.");
    h0_X_1match->SetLineWidth(2);
    h0_X_1match->SetMarkerSize(0.9);
    h0_X_1match->DrawNormalized("");

    THStack * hs1_1match = (THStack*)file1->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut29-BKG");
    TH2D * h1_1match = (TH2D*)hs1_1match->GetStack()->Last();
    TH1D * h1_X_1match = (TH1D*)h1_1match->ProjectionX();
    h1_X_1match->Rebin(4);
    h1_X_1match->SetLineColor(kRed);
    h1_X_1match->SetMarkerColor(kRed);
    h1_X_1match->SetLineWidth(2);
    h1_X_1match->SetMarkerSize(0.9);
    h1_X_1match->DrawNormalized("SAME");

    THStack * hs2_1match = (THStack*)file2->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut29-DATA");
    TH2D * h2_1match = (TH2D*)hs2_1match->GetStack()->Last();
    TH1D * h2_X_1match = (TH1D*)h2_1match->ProjectionX();
    h2_X_1match->Rebin(4);
    h2_X_1match->SetLineColor(kGreen+1);
    h2_X_1match->SetMarkerColor(kGreen+1);
    h2_X_1match->SetLineWidth(2);
    h2_X_1match->SetMarkerSize(0.9);
    h2_X_1match->DrawNormalized("SAME");

    leg = c1x->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c1x->Modified();
    c1x->Update();

    TCanvas * c1y = new TCanvas();

    TH1D * h0_Y_1match = (TH1D*)h0_1match->ProjectionY();
    h0_Y_1match->Rebin(4);
    h0_Y_1match->SetMaximum(200);
    h0_Y_1match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    h0_Y_1match->GetXaxis()->SetTitleOffset(1.2);
    h0_Y_1match->GetYaxis()->SetTitle("A. U.");
    h0_Y_1match->SetLineWidth(2);
    h0_Y_1match->SetMarkerSize(0.9);
    h0_Y_1match->DrawNormalized("");

    TH1D * h1_Y_1match = (TH1D*)h1_1match->ProjectionY();
    h1_Y_1match->Rebin(4);
    h1_Y_1match->SetLineColor(kRed);
    h1_Y_1match->SetMarkerColor(kRed);
    h1_Y_1match->SetLineWidth(2);
    h1_Y_1match->SetMarkerSize(0.9);
    h1_Y_1match->DrawNormalized("SAME");

    TH1D * h2_Y_1match = (TH1D*)h2_1match->ProjectionY();
    h2_Y_1match->Rebin(4);
    h2_Y_1match->SetLineColor(kGreen+1);
    h2_Y_1match->SetMarkerColor(kGreen+1);
    h2_Y_1match->SetLineWidth(2);
    h2_Y_1match->SetMarkerSize(0.9);
    h2_Y_1match->DrawNormalized("SAME");

    leg = c1y->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c1y->Modified();
    c1y->Update();
    
    // 2 match nJets

    TCanvas * c2x = new TCanvas();

    THStack * hs0_2match = (THStack*)file0->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_zoom_cut30-BKG");
    TH2D * h0_2match = (TH2D*)hs0_2match->GetStack()->Last();
    TH1D * h0_X_2match = (TH1D*)h0_2match->ProjectionX();
    h0_X_2match->Rebin(4);
    h0_X_2match->SetMaximum(150);
    h0_X_2match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    h0_X_2match->GetXaxis()->SetTitleOffset(1.2);
    h0_X_2match->GetYaxis()->SetTitle("A. U.");
    h0_X_2match->SetLineWidth(2);
    h0_X_2match->SetMarkerSize(0.9);
    h0_X_2match->DrawNormalized("");

    THStack * hs1_2match = (THStack*)file1->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_zoom_cut30-BKG");
    TH2D * h1_2match = (TH2D*)hs1_2match->GetStack()->Last();
    TH1D * h1_X_2match = (TH1D*)h1_2match->ProjectionX();
    h1_X_2match->Rebin(4);
    h1_X_2match->SetLineColor(kRed);
    h1_X_2match->SetMarkerColor(kRed);
    h1_X_2match->SetLineWidth(2);
    h1_X_2match->SetMarkerSize(0.9);
    h1_X_2match->DrawNormalized("SAME");

    THStack * hs2_2match = (THStack*)file2->Get("reco_METmu_dphi_vs_matched_muon_vtx_vxy_zoom_cut30-DATA");
    TH2D * h2_2match = (TH2D*)hs2_2match->GetStack()->Last();
    TH1D * h2_X_2match = (TH1D*)h2_2match->ProjectionX();
    h2_X_2match->Rebin(4);
    h2_X_2match->SetLineColor(kGreen+1);
    h2_X_2match->SetMarkerColor(kGreen+1);
    h2_X_2match->SetLineWidth(2);
    h2_X_2match->SetMarkerSize(0.9);
    h2_X_2match->DrawNormalized("SAME");

    leg = c2x->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c2x->Modified();
    c2x->Update();

    TCanvas * c2y = new TCanvas();

    TH1D * h0_Y_2match = (TH1D*)h0_2match->ProjectionY();
    h0_Y_2match->Rebin(4);
    h0_Y_2match->SetMaximum(100);
    h0_Y_2match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    h0_Y_2match->GetXaxis()->SetTitleOffset(1.2);
    h0_Y_2match->GetYaxis()->SetTitle("A. U.");
    h0_Y_2match->SetLineWidth(2);
    h0_Y_2match->SetMarkerSize(0.9);
    h0_Y_2match->DrawNormalized("");

    TH1D * h1_Y_2match = (TH1D*)h1_2match->ProjectionY();
    h1_Y_2match->Rebin(4);
    h1_Y_2match->SetLineColor(kRed);
    h1_Y_2match->SetMarkerColor(kRed);
    h1_Y_2match->SetLineWidth(2);
    h1_Y_2match->SetMarkerSize(0.9);
    h1_Y_2match->DrawNormalized("SAME");

    TH1D * h2_Y_2match = (TH1D*)h2_2match->ProjectionY();
    h2_Y_2match->Rebin(4);
    h2_Y_2match->SetLineColor(kGreen+1);
    h2_Y_2match->SetMarkerColor(kGreen+1);
    h2_Y_2match->SetLineWidth(2);
    h2_Y_2match->SetMarkerSize(0.9);
    h2_Y_2match->DrawNormalized("SAME");

    leg = c2y->BuildLegend(0.5, 0.6, 0.8, 0.9);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("nJets VR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("SR MC");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("nJets VR data");

    c2y->Modified();
    c2y->Update();

}
