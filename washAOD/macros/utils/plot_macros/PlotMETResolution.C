void PlotMETResolution(TString filename, bool fSave=0) {
    //TH1::AddDirectory(kFALSE);
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");
    gStyle->SetOptStat("nmr");

    TFile * file = TFile::Open(filename);

    TTree * dsa = (TTree*)file->Get("RECO_dsa/recoEffiForMuTrack");
    TTree * gm = (TTree*)file->Get("RECO_gbm/recoEffiForMuTrack");
    dsa->AddFriend(gm, "gm");

    //ROOT::EnableImplicitMT();
    ROOT::RDataFrame df(*dsa);

    auto correctMETpx_DSA = [&](unsigned int nMatched, float MET_px, vector<float> muon_px) {
        float corr_MET_px = MET_px;
        for (unsigned int i = 0; i < nMatched; i++)
            corr_MET_px -= muon_px[i];
        return corr_MET_px;
    };
    auto correctMETpx_GM = [&](unsigned int nMatched, float MET_px, vector<float> muon_px) {
        float corr_MET_px = MET_px;
        for (unsigned int i = 0; i < nMatched; i++)
            corr_MET_px += muon_px[i];
        return corr_MET_px;
    };
    auto correctMETpy_DSA = [&](unsigned int nMatched, float MET_py, vector<float> muon_py) {
        float corr_MET_py = MET_py;
        for (unsigned int i = 0; i < nMatched; i++)
            corr_MET_py -= muon_py[i];
        return corr_MET_py;
    };
    auto correctMETpy_GM = [&](unsigned int nMatched, float MET_py, vector<float> muon_py) {
        float corr_MET_py = MET_py;
        for (unsigned int i = 0; i < nMatched; i++)
            corr_MET_py += muon_py[i];
        return corr_MET_py;
    };
    auto calcMETpt = [&](float MET_px, float MET_py) { return sqrt(MET_px*MET_px + MET_py*MET_py); };
    auto calcMETphi = [&](float MET_px, float MET_py) { return atan2(MET_py, MET_px); };
    auto METresolution = [&](float recoMET, float genMET) { return recoMET-genMET; };

    auto df_new = df.Filter("nMatched==2 && gm.nMatched==2").
        Define("METpxDSA", correctMETpx_DSA, {"nMatched", "recoPFMetPx", "recoPx"}).
        Define("METpyDSA", correctMETpy_DSA, {"nMatched", "recoPFMetPy", "recoPy"}).
        Define("CorrectedMETpx", correctMETpx_GM, {"gm.nMatched", "gm.recoPFMetPx", "gm.recoPx"}).
        Define("CorrectedMETpy", correctMETpy_GM, {"gm.nMatched", "gm.recoPFMetPy", "gm.recoPy"}).
        Define("CorrectedMETpt", calcMETpt, {"CorrectedMETpx", "CorrectedMETpy"}).
        Define("CorrectedMETphi", calcMETphi, {"CorrectedMETpx", "CorrectedMETpy"}).
        Define("unCorrectedPtResolution", METresolution, {"recoPFMetPt", "genLeadMetPt"}).
        Define("correctedPtResolution", METresolution, {"CorrectedMETpt", "genLeadMetPt"}).
        Define("unCorrectedPhiResolution", METresolution, {"recoPFMetPhi", "genLeadMetPhi"}).
        Define("correctedPhiResolution", METresolution, {"CorrectedMETphi", "genLeadMetPhi"});

    auto h_MET = (TH1F*)df_new.Histo1D({"PFMET", "", 100, -200, 200}, "unCorrectedPtResolution")->Clone();
    auto h_METcorrected = (TH1F*)df_new.Histo1D({"Corrected PFMET", "", 100, -200, 200}, "correctedPtResolution")->Clone();

    TCanvas * c = new TCanvas();

    h_MET->Scale(1/h_MET->Integral());
    h_METcorrected->Scale(1/h_METcorrected->Integral());

    h_METcorrected->GetXaxis()->SetTitle("Reco PFMET p_{T} - Gen MET p_{T} [GeV]");
    h_METcorrected->GetYaxis()->SetTitle("A. U.");
    h_METcorrected->GetYaxis()->SetTitleOffset(1.3);
    h_METcorrected->SetLineColor(kRed);
    h_METcorrected->Draw("HIST");
    h_METcorrected->SetDirectory(0);
    gPad->Update();
    auto * stats = (TPaveStats*)h_METcorrected->FindObject("stats");
    stats->SetTextColor(kRed);
    stats->SetX1NDC(0.6); stats->SetX2NDC(0.8);
    stats->SetY1NDC(0.8); stats->SetY2NDC(0.9);

    h_MET->Draw("HIST SAMES");
    h_MET->SetDirectory(0);
    gPad->Update();
    auto * stats2 = (TPaveStats*)h_MET->FindObject("stats");
    stats2->SetX1NDC(0.6); stats2->SetX2NDC(0.8);
    stats2->SetY1NDC(0.65); stats2->SetY2NDC(0.75);

    gPad->Modified();

    TCanvas * c2 = new TCanvas();
    
    auto h_MET_phi = (TH1F*)df_new.Histo1D({"PFMET #phi", "", 100, -3.2, 3.2}, "unCorrectedPhiResolution")->Clone();
    auto h_METcorrected_phi = (TH1F*)df_new.Histo1D({"Corrected PFMET #phi", "", 100, -3.2, 3.2}, "correctedPhiResolution")->Clone();

    h_MET_phi->Scale(1/h_MET_phi->Integral());
    h_METcorrected_phi->Scale(1/h_METcorrected_phi->Integral());

    h_METcorrected_phi->GetXaxis()->SetTitle("Reco PFMET #phi - Gen MET #phi");
    h_METcorrected_phi->GetYaxis()->SetTitle("A. U.");
    h_METcorrected_phi->GetYaxis()->SetTitleOffset(1.3);
    h_METcorrected_phi->SetLineColor(kRed);
    h_METcorrected_phi->Draw("HIST");
    h_METcorrected_phi->SetDirectory(0);
    gPad->Update();
    auto * stats3 = (TPaveStats*)h_METcorrected_phi->FindObject("stats");
    stats3->SetTextColor(kRed);
    stats3->SetX1NDC(0.6); stats3->SetX2NDC(0.8);
    stats3->SetY1NDC(0.8); stats3->SetY2NDC(0.9);
    h_MET_phi->Draw("HIST SAMES");
    h_MET_phi->SetDirectory(0);
    gPad->Update();
    auto * stats4 = (TPaveStats*)h_MET_phi->FindObject("stats");
    stats4->SetX1NDC(0.6); stats4->SetX2NDC(0.8);
    stats4->SetY1NDC(0.65); stats4->SetY2NDC(0.75);

    gPad->Modified();

    TCanvas * c3 = new TCanvas();

    auto h_MET_pt = (TH1F*)df_new.Histo1D({"PFMET", "", 100, 0, 500}, "recoPFMetPt")->Clone();
    auto h_METcorrected_pt = (TH1F*)df_new.Histo1D({"Corrected PFMET", "", 100, 0, 500}, "CorrectedMETpt")->Clone();

    h_MET_pt->Scale(1/h_MET_pt->Integral());
    h_METcorrected_pt->Scale(1/h_METcorrected_pt->Integral());

    h_METcorrected_pt->SetStats(0);
    h_METcorrected_pt->GetXaxis()->SetTitle("MET p_{T} [GeV]");
    h_METcorrected_pt->GetYaxis()->SetTitle("A. U.");
    h_METcorrected_pt->GetYaxis()->SetTitleOffset(1.3);
    h_METcorrected_pt->SetLineColor(kRed);
    h_METcorrected_pt->Draw("HIST");
    h_METcorrected_pt->SetDirectory(0);
    h_MET_pt->Draw("HIST SAME");
    h_MET_pt->SetDirectory(0);

    c3->BuildLegend(0.5, 0.6, 0.8, 0.85);

    if (fSave) {
        c->SaveAs("../../images/MET_pt_resolution.pdf");
        c2->SaveAs("../../images/MET_phi_resolution.pdf");
        c3->SaveAs("../../images/MET_pt_corr_vs_uncorr.pdf");
    }

    file->Close();
}
