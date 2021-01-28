void PlotABCDProfiles() {

    gROOT->LoadMacro("../tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle()");
    gStyle->SetPalette(1);

    TFile * file0 = TFile::Open("../../bin/ABCD_profiles_nJets_0match.root");
    TFile * file1 = TFile::Open("../../bin/ABCD_profiles_nJets_1match.root");
    TFile * file2 = TFile::Open("../../bin/ABCD_profiles_nJets_2match.root");
    TFile * file3 = TFile::Open("../../bin/ABCD_profiles_dR_0match.root");

    // 0 match nJets

    TCanvas * c0x = new TCanvas();

    THStack * hs_X_0match = (THStack*)file0->Get("profilesX_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-DATA");
    hs_X_0match->SetMaximum(1);
    hs_X_0match->Draw("NOSTACK PMC");
    hs_X_0match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    hs_X_0match->GetXaxis()->SetTitleOffset(1.2);
    hs_X_0match->GetYaxis()->SetTitle("A. U.");

    TLegend * leg = c0x->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("20 < v_{xy} < 120 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("120 < v_{xy} < 220 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("220 < v_{xy} < 320 cm");

    c0x->Modified();
    c0x->Update();

    TCanvas * c0y = new TCanvas();

    THStack * hs_Y_0match = (THStack*)file0->Get("profilesY_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-DATA");
    hs_Y_0match->SetMaximum(1);
    hs_Y_0match->Draw("NOSTACK PMC");
    hs_Y_0match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    hs_Y_0match->GetXaxis()->SetTitleOffset(1.2);
    hs_Y_0match->GetYaxis()->SetTitle("A. U.");

    leg = c0y->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("0.4 < |#Delta#phi| < 1.2");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("1.2 < |#Delta#phi| < 1.8");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("1.8 < |#Delta#phi| < 2.4");

    // 1 match nJets

    TCanvas * c1x = new TCanvas();

    THStack * hs_X_1match = (THStack*)file1->Get("profilesX_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut29-DATA");
    hs_X_1match->SetMaximum(1);
    hs_X_1match->Draw("NOSTACK PMC");
    hs_X_1match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    hs_X_1match->GetXaxis()->SetTitleOffset(1.2);
    hs_X_1match->GetYaxis()->SetTitle("A. U.");

    leg = c1x->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("20 < v_{xy} < 120 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("120 < v_{xy} < 220 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("220 < v_{xy} < 320 cm");

    c1x->Modified();
    c1x->Update();

    TCanvas * c1y = new TCanvas();

    THStack * hs_Y_1match = (THStack*)file1->Get("profilesY_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut29-DATA");
    hs_Y_1match->SetMaximum(1);
    hs_Y_1match->Draw("NOSTACK PMC");
    hs_Y_1match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    hs_Y_1match->GetXaxis()->SetTitleOffset(1.2);
    hs_Y_1match->GetYaxis()->SetTitle("A. U.");

    leg = c1y->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("0.0 < |#Delta#phi| < 0.1");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("0.1 < |#Delta#phi| < 0.2");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("0.2 < |#Delta#phi| < 0.3");

    // 2 match nJets
    
    TCanvas * c2x = new TCanvas();

    THStack * hs_X_2match = (THStack*)file2->Get("profilesX_reco_METmu_dphi_vs_matched_muon_vtx_vxy_zoom_cut30-DATA");
    hs_X_2match->SetMaximum(1);
    hs_X_2match->Draw("NOSTACK PMC");
    hs_X_2match->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    hs_X_2match->GetXaxis()->SetTitleOffset(1.2);
    hs_X_2match->GetYaxis()->SetTitle("A. U.");

    leg = c2x->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("0 < v_{xy} < 5 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("5 < v_{xy} < 10 cm");

    c2x->Modified();
    c2x->Update();

    TCanvas * c2y = new TCanvas();

    THStack * hs_Y_2match = (THStack*)file2->Get("profilesY_reco_METmu_dphi_vs_matched_muon_vtx_vxy_zoom_cut30-DATA");
    hs_Y_2match->SetMaximum(1);
    hs_Y_2match->Draw("NOSTACK PMC");
    hs_Y_2match->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    hs_Y_2match->GetXaxis()->SetTitleOffset(1.2);
    hs_Y_2match->GetYaxis()->SetTitle("A. U.");

    leg = c2y->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("0.0 < |#Delta#phi| < 0.2");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("0.2 < |#Delta#phi| < 0.4");
    
    // 0 match dR

    TCanvas * c0x_dR = new TCanvas();

    THStack * hs_X_0match_dR = (THStack*)file3->Get("profilesX_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-DATA");
    hs_X_0match_dR->SetMaximum(1);
    hs_X_0match_dR->Draw("NOSTACK PMC");
    hs_X_0match_dR->GetXaxis()->SetTitle("|#Delta#phi(MET, muons)|");
    hs_X_0match_dR->GetXaxis()->SetTitleOffset(1.2);
    hs_X_0match_dR->GetYaxis()->SetTitle("A. U.");

    leg = c0x_dR->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("20 < v_{xy} < 120 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("120 < v_{xy} < 220 cm");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("220 < v_{xy} < 320 cm");

    c0x_dR->Modified();
    c0x_dR->Update();

    TCanvas * c0y_dR = new TCanvas();

    THStack * hs_Y_0match_dR = (THStack*)file3->Get("profilesY_reco_METmu_dphi_vs_matched_muon_vtx_vxy_cut28-DATA");
    hs_Y_0match_dR->SetMaximum(1);
    hs_Y_0match_dR->Draw("NOSTACK PMC");
    hs_Y_0match_dR->GetXaxis()->SetTitle("Muon vertex v_{xy} [cm]");
    hs_Y_0match_dR->GetXaxis()->SetTitleOffset(1.2);
    hs_Y_0match_dR->GetYaxis()->SetTitle("A. U.");

    leg = c0y_dR->BuildLegend(0.5, 0.7, 0.7, 0.85);
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(0))->SetLabel("0.4 < |#Delta#phi| < 1.2");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(1))->SetLabel("1.2 < |#Delta#phi| < 1.8");
    ((TLegendEntry*)leg->GetListOfPrimitives()->At(2))->SetLabel("1.8 < |#Delta#phi| < 2.4");

    // njets 0 match
    //"ratioRangesX": [
    //    0.4,
    //    1.2,
    //    1.8,
    //    2.4
    //],
    //"ratioRangesY": [
    //    20,
    //    120,
    //    220,
    //    320
    //]

    // njets 1 match
    //  "ratioRangesX": [
    //    0.0,
    //    0.1,
    //    0.2,
    //    0.3
    //],
    //"ratioRangesY": [
    //    20,
    //    120,
    //    220,
    //    320
    //]

    // njets 2 match
    //0.0,
    //    0.2,
    //    0.4
    //        ],
    //    "ratioRangesY": [
    //        0,
    //    5,
    //    10

}
