void PlotKfactors(bool fSave=0) {
    TH1::AddDirectory(kFALSE);

    TFile * file = TFile::Open("../../data/kfactors.root");

    auto * hZNLO = (TH1D*)file->Get("ZJets_012j_NLO/nominal");
    auto * hZEWK = (TH1D*)file->Get("EWKcorr/Z");
    auto * hZLO = (TH1D*)file->Get("ZJets_LO/inv_pt");

    auto * hZRatioQCD = (TH1D*)hZNLO->Clone();
    hZRatioQCD->Divide(hZLO);

    auto * hZRatioEWK = (TH1D*)hZEWK->Clone();
    hZRatioEWK->Divide(hZLO);

    gStyle->SetOptStat(0);
    auto * c = new TCanvas();
    c->SetTicks();
    c->SetMargin(0.1, 0.05, 0.1, 0.05);

    hZRatioQCD->Draw();
    hZRatioQCD->SetTitle("");
    hZRatioQCD->SetName("QCD weight");
    hZRatioQCD->GetXaxis()->SetTitle("Z p_{T} [GeV]");
    hZRatioQCD->GetYaxis()->SetTitle("Weight");
    hZRatioQCD->SetMarkerStyle(21);
    hZRatioQCD->SetMarkerColor(kBlue+1);
    hZRatioQCD->SetLineColor(kBlue+1);
    hZRatioQCD->SetMaximum(2.4);
    hZRatioQCD->SetMinimum(0.4);

    hZRatioEWK->SetLineColor(kRed+1);
    hZRatioEWK->SetMarkerColor(kRed+1);
    hZRatioEWK->SetMarkerStyle(20);
    hZRatioEWK->SetName("EWK weight");
    hZRatioEWK->SetTitle("");
    hZRatioEWK->Draw("SAME");

    auto * hZRatiosQCDEWK = (TH1D*)hZRatioQCD->Clone();
    hZRatiosQCDEWK->Multiply(hZRatioEWK);
    hZRatiosQCDEWK->SetName("QCD+EWK weights");
    hZRatiosQCDEWK->SetLineColor(kGreen+2);
    hZRatiosQCDEWK->SetMarkerColor(kGreen+2);
    hZRatiosQCDEWK->SetMarkerStyle(22);
    hZRatiosQCDEWK->Draw("SAME");

    c->BuildLegend(0.63, 0.68, 0.88, 0.88);

    TPaveText * text = new TPaveText(0.3, 0.8, 0.4, 0.9, "NDC");
    text->SetFillColor(kWhite);
    text->AddText("Z+Jets");
    text->Draw("SAME");

    c->Update();

    TLine *line = new TLine(c->GetUxmin(), 1, c->GetUxmax(), 1);
    line->SetLineStyle(kDashed);
    line->Draw();

    auto * hWNLO = (TH1D*)file->Get("WJets_012j_NLO/nominal");
    auto * hWEWK = (TH1D*)file->Get("EWKcorr/W");
    auto * hWLO = (TH1D*)file->Get("WJets_LO/inv_pt");

    auto * hWRatioQCD = (TH1D*)hWNLO->Clone();
    hWRatioQCD->Divide(hWLO);

    auto * hWRatioEWK = (TH1D*)hWEWK->Clone();
    hWRatioEWK->Divide(hWLO);

    auto * c2 = new TCanvas();
    c2->SetMargin(0.1, 0.05, 0.1, 0.05);
    c2->SetTicks();

    hWRatioQCD->Draw();
    hWRatioQCD->GetXaxis()->SetTitle("W p_{T} [GeV]");
    hWRatioQCD->GetYaxis()->SetTitle("Weight");
    hWRatioQCD->SetTitle("");
    hWRatioQCD->SetName("QCD weight");
    hWRatioQCD->SetMarkerStyle(21);
    hWRatioQCD->SetLineColor(kBlue+1);
    hWRatioQCD->SetMarkerColor(kBlue+1);
    hWRatioQCD->SetMaximum(2.4);
    hWRatioQCD->SetMinimum(0.4);

    hWRatioEWK->SetLineColor(kRed+1);
    hWRatioEWK->SetMarkerColor(kRed+1);
    hWRatioEWK->SetMarkerStyle(20);
    hWRatioEWK->SetName("EWK weight");
    hWRatioEWK->SetTitle("");
    hWRatioEWK->Draw("SAME");

    auto * hWRatiosQCDEWK = (TH1D*)hWRatioQCD->Clone();
    hWRatiosQCDEWK->Multiply(hWRatioEWK);
    hWRatiosQCDEWK->SetName("QCD+EWK weights");
    hWRatiosQCDEWK->SetLineColor(kGreen+2);
    hWRatiosQCDEWK->SetMarkerColor(kGreen+2);
    hWRatiosQCDEWK->SetMarkerStyle(22);
    hWRatiosQCDEWK->Draw("SAME");

    c2->BuildLegend(0.63, 0.68, 0.88, 0.88);

    TPaveText * text2 = new TPaveText(0.3, 0.8, 0.4, 0.9, "NDC");
    text2->SetFillColor(kWhite);
    text2->AddText("W+Jets");
    text2->Draw("SAME");

    c2->Update();

    TLine *line2 = new TLine(c2->GetUxmin(), 1, c2->GetUxmax(), 1);
    line2->SetLineStyle(kDashed);
    line2->Draw();

    if (fSave) {
        c->SaveAs("../../images/Z_Weights.pdf");
        c2->SaveAs("../../images/W_Weights.pdf");
    }
    file->Close();
}
