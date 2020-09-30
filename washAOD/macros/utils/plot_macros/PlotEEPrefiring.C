void PlotEEPrefiring(TString year="2017", bool fSave=0) {
    TH1::AddDirectory(kFALSE);

    TFile * file_EE = TFile::Open(TString("../../bin/PlotsTwelthRunJuly5thEEPrefiring/plots_SR_twelthrun_noData_2DHists_signal_" + year + ".root"));
    TFile * file_baseline = TFile::Open(TString("../../bin/PlotsTwelthRunJuly5thEEPrefiring/plots_SR_twelthrun_noData_2DHists_NoPrefiring_signal_" + year + ".root"));

    THStack * hs_EE = (THStack*)file_EE->Get("reco_METmu_dphi_v2_cut28-SIGNAL");
    THStack * hs_baseline = (THStack*)file_baseline->Get("reco_METmu_dphi_v2_cut28-SIGNAL");
    TString x_axis_title = "#Delta#phi(MET, muons)"; 
    //THStack * hs_EE = (THStack*)file_EE->Get("matched_muon_vtx_vxy_cut28-SIGNAL");
    //THStack * hs_baseline = (THStack*)file_baseline->Get("matched_muon_vtx_vxy_cut28-SIGNAL");
    //TString x_axis_title = "Muon vxy [cm]";
    
    auto * h_EE = (TH1D*)hs_EE->GetHists()->At(3);
    h_EE->Scale(1/h_EE->Integral(0,-1));
    auto * h_baseline = (TH1D*)hs_baseline->GetHists()->At(3);
    h_baseline->Scale(1/h_baseline->Integral(0,-1));

    h_EE->SetLineColor(kRed);
    h_EE->SetMarkerColor(kRed);

    gStyle->SetOptStat(1);
    auto * c = new TCanvas();
    c->SetMargin(0.1, 0.05, 0.1, 0.05);
    c->Range(0,0,1,1);
    c->Divide(1,2);
    c->SetTicks();
    // top pad
    c->cd(1);
    gPad->SetPad(0.01, 0.33, 0.99, 0.99);
    gPad->Draw();
    gPad->cd();
    gPad->SetTopMargin(0.1);
    gPad->SetBottomMargin(0.01);
    gPad->SetRightMargin(0.1);
    gPad->SetFillStyle(0);
    gPad->SetLogy();
    gPad->SetTicks();
    // bottom pad
    c->cd(2);
    gPad->SetPad(0.01, 0.01, 0.99, 0.32);
    gPad->Draw();
    gPad->cd();
    gPad->SetTopMargin(0.01);
    gPad->SetBottomMargin(0.3);
    gPad->SetRightMargin(0.1);
    gPad->SetFillStyle(0);
    //gPad->SetLogy();
    gPad->SetTicks();

    c->cd(1);

    h_EE->Draw();
    cout << h_EE->GetName() << endl;
    h_EE->SetName("EE prefiring selection");
    h_EE->SetTitle("");
    h_EE->GetYaxis()->SetTitle("A. U.");
    h_EE->GetXaxis()->SetTitleSize(0.00);
    h_EE->GetYaxis()->SetLabelSize(0.05);
    h_EE->GetYaxis()->SetTitleSize(0.06);
    h_EE->GetYaxis()->SetTitleOffset(0.76);
    h_EE->SetTitle("");
    h_EE->SetLabelSize(0.0);

    c->Update();
    //hZRatioQCD->SetName("QCD weight");
    //hZRatioQCD->GetXaxis()->SetTitle("Z p_{T} [GeV]");
    //hZRatioQCD->GetYaxis()->SetTitle("Weight");
    //hZRatioQCD->SetMarkerStyle(21);
    //hZRatioQCD->SetMarkerColor(kBlue+1);
    //hZRatioQCD->SetLineColor(kBlue+1);
    //hZRatioQCD->SetMaximum(2.4);
    //hZRatioQCD->SetMinimum(0.4);

    h_baseline->Draw("SAME");
    cout << h_baseline->GetName() << endl;
    h_baseline->SetName("Nominal selection");
    h_baseline->SetTitle("");
    c->Update();

    TPaveText * text = new TPaveText(0.8, 0.8, 0.87, 0.87, "NDC");
    text->SetFillColor(kWhite);
    text->AddText(year);
    text->Draw("SAME");

    gPad->BuildLegend(0.6, 0.5, 0.85, 0.7);
    c->Modified();
    c->Update();

    c->cd(2);

    auto * h_ratio = (TH1D*)h_EE->Clone();
    h_ratio->Divide(h_baseline);
    h_ratio->SetName("ratio");

    h_ratio->SetLineColor(kBlack);
    h_ratio->SetMarkerColor(kBlack);
    h_ratio->SetMarkerSize(1);
    h_ratio->SetStats(0);
    h_ratio->Draw("EP");

    h_ratio->GetXaxis()->SetTitleSize(0.12);
    h_ratio->GetXaxis()->SetLabelSize(0.11);
    h_ratio->GetYaxis()->SetTitle("EE/nominal");
    h_ratio->GetXaxis()->SetTitle(x_axis_title);
    h_ratio->GetYaxis()->SetTitleSize(0.12);
    h_ratio->GetYaxis()->SetLabelSize(0.11);
    h_ratio->GetYaxis()->SetNdivisions(5);
    h_ratio->GetYaxis()->SetTitleOffset(0.35);
    h_ratio->GetYaxis()->SetRangeUser(0.89, 1.11);


    if (fSave) {
        c->SaveAs(TString("../images/pileup_") + year + TString(".pdf"));
    }

    file_EE->Close();
    file_baseline->Close();
}
