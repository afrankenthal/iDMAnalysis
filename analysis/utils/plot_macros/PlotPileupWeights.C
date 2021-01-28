void PlotPileupWeights(TString year, bool fSave=0) {
    TH1::AddDirectory(kFALSE);

    TFile * file_MC = TFile::Open(TString("../../data/pileup/pileup_QCD_") + year + TString(".root"));
    TFile * file_data = TFile::Open(TString("../../data/pileup/pileup_data_") + year + TString(".root"));
    
    auto * h_data = (TH1D*)file_data->Get("pileup");
    h_data->Scale(1/h_data->Integral());

    h_data->SetLineColor(kRed);
    h_data->SetMarkerColor(kRed);
    //h_data->SetMarkerStyle(21);
    

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
    gPad->SetLogy();
    gPad->SetTicks();

    c->cd(1);

    h_data->Draw();
    h_data->SetName("Data");
    h_data->SetTitle("");
    h_data->GetYaxis()->SetTitle("A. U.");
    h_data->GetXaxis()->SetTitleSize(0.00);
    h_data->GetYaxis()->SetLabelSize(0.05);
    h_data->GetYaxis()->SetTitleSize(0.06);
    h_data->GetYaxis()->SetTitleOffset(0.76);
    h_data->SetTitle("");
    h_data->SetLabelSize(0.0);

    c->Update();
    auto * stats = (TPaveStats*)h_data->GetListOfFunctions()->FindObject("stats");
    stats->SetTextColor(kRed);
    stats->SetX1NDC(0.2); stats->SetX2NDC(0.4);
    stats->SetY1NDC(0.2); stats->SetY2NDC(0.4);
    //hZRatioQCD->SetName("QCD weight");
    //hZRatioQCD->GetXaxis()->SetTitle("Z p_{T} [GeV]");
    //hZRatioQCD->GetYaxis()->SetTitle("Weight");
    //hZRatioQCD->SetMarkerStyle(21);
    //hZRatioQCD->SetMarkerColor(kBlue+1);
    //hZRatioQCD->SetLineColor(kBlue+1);
    //hZRatioQCD->SetMaximum(2.4);
    //hZRatioQCD->SetMinimum(0.4);

    auto * hs = (THStack*)file_MC->Get("gen_pu_true_cut1-BKG");
    auto * h_MC = (TH1D*)hs->GetStack()->Last();
    h_MC->Scale(1/h_MC->Integral());
    h_MC->SetName("QCD");

    h_MC->Draw("SAMES");
    h_MC->SetStats(1);
    c->Update();
    auto * stats2 = (TPaveStats*)h_MC->FindObject("stats");
    stats2->SetTextColor(kBlack);
    stats2->SetX1NDC(0.2); stats2->SetX2NDC(0.4);
    stats2->SetY1NDC(0.5); stats2->SetY2NDC(0.7);

    TPaveText * text = new TPaveText(0.8, 0.8, 0.87, 0.87, "NDC");
    text->SetFillColor(kWhite);
    text->AddText(year);
    text->Draw("SAME");

    c->Modified();
    c->Update();

    c->cd(2);

    auto * h_ratio = (TH1D*)h_data->Clone();
    h_ratio->Divide(h_MC);
    h_ratio->SetName("puWeights");

    TFile * pu_wgt_file = new TFile(TString("../../data/pileup/PUWeights_") + year + ".root", TString("RECREATE"));
    h_ratio->Write();
    pu_wgt_file->Close();

    h_ratio->SetLineColor(kBlack);
    h_ratio->SetMarkerColor(kBlack);
    h_ratio->SetMarkerSize(10);
    h_ratio->SetStats(0);
    h_ratio->Draw("EP");

    h_ratio->GetXaxis()->SetTitleSize(0.12);
    h_ratio->GetXaxis()->SetLabelSize(0.11);
    h_ratio->GetYaxis()->SetTitle("Data/QCD");
    h_ratio->GetXaxis()->SetTitle("Pileup");
    h_ratio->GetYaxis()->SetTitleSize(0.12);
    h_ratio->GetYaxis()->SetLabelSize(0.11);
    h_ratio->GetYaxis()->SetNdivisions(5);
    h_ratio->GetYaxis()->SetTitleOffset(0.35);


    if (fSave) {
        c->SaveAs(TString("../images/pileup_") + year + TString(".pdf"));
    }

    file_MC->Close();
    file_data->Close();
}
