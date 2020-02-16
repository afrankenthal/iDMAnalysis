void PlotMuonPtResolution(TString filename, bool fSave=0) {
    //TH1::AddDirectory(kFALSE);
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");
    gStyle->SetOptStat("nmr");

    TFile * file = TFile::Open(filename);

    TTree * dsa = (TTree*)file->Get("RECO_dsa/recoEffiForMuTrack");
    TTree * gm = (TTree*)file->Get("RECO_gbm/recoEffiForMuTrack");

    //ROOT::EnableImplicitMT();
    ROOT::RDataFrame df_dsa(*dsa);
    ROOT::RDataFrame df_gm(*gm);

    auto calcResolution = [&](vector<float> recoMuPt, vector<float> genMuPt) { return recoMuPt[0]-genMuPt[0]; };

    auto df_dsa_new = df_dsa.Filter("nMatched > 0 && genPt[0] > 5").
        Define("muPtResolution", calcResolution, {"recoPt", "genPt"});

    auto df_gm_new = df_gm.Filter("nMatched > 0 && genPt[0] > 5").
        Define("muPtResolution", calcResolution, {"recoPt", "genPt"});

    auto h_dsa = (TH1F*)df_dsa_new.Histo1D({"dSA", "", 100, -50, 200}, "muPtResolution")->Clone();
    auto h_gm = (TH1F*)df_gm_new.Histo1D({"GM", "", 100, -50, 200}, "muPtResolution")->Clone();

    TCanvas * c = new TCanvas();
    c->SetLogy();

    h_dsa->Scale(1/h_dsa->Integral());
    h_gm->Scale(1/h_gm->Integral());

    h_dsa->GetXaxis()->SetTitle("Reco muon p_{T} - Gen muon p_{T} [GeV]");
    h_dsa->GetYaxis()->SetTitle("A. U.");
    h_dsa->GetYaxis()->SetTitleOffset(1.3);
    h_dsa->SetLineColor(kRed);
    h_dsa->Draw("HIST");
    h_dsa->SetDirectory(0);
    gPad->Update();
    auto * stats = (TPaveStats*)h_dsa->FindObject("stats");
    stats->SetTextColor(kRed);
    stats->SetX1NDC(0.6); stats->SetX2NDC(0.8);
    stats->SetY1NDC(0.8); stats->SetY2NDC(0.9);

    h_gm->Draw("HIST SAMES");
    h_gm->SetDirectory(0);
    gPad->Update();
    auto * stats2 = (TPaveStats*)h_gm->FindObject("stats");
    stats2->SetX1NDC(0.6); stats2->SetX2NDC(0.8);
    stats2->SetY1NDC(0.65); stats2->SetY2NDC(0.75);

    gPad->Modified();

    if (fSave) {
        c->SaveAs("../../images/muon_pt_resolution.pdf");
    }

    file->Close();
}
