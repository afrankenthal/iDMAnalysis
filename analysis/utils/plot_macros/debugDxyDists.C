void debugDxyDists() {
    gStyle->SetOptStat(0);

    TFile *_file0 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2016/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-1.root");
    TFile *_file1 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2016/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-10.root");

    TFile *_file2 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2017/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-1.root");
    TFile *_file3 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2017/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-10.root");

    TFile *_file4 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-1.root");
    TFile *_file5 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_eleventhrun/iDMAnalysis_Mchi-10p5_dMchi-1p0_ctau-10.root");

    TFile *_file6 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2016/backgrounds_seventhrun/ZJetsToNuNu_HT-200To400_13TeV-madgraph/crab_iDMAnalysis_ZJetsToNuNu_HT-200To400_v3/191217_194120/0000/output_31.root");
    TFile *_file7 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2017/backgrounds_seventhrun/ZJetsToNuNu_HT-200To400_13TeV-madgraph/crab_iDMAnalysis_ZJetsToNuNu_HT-200To400_v2/191217_160234/0000/output_88.root");
    TFile *_file8 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_seventhrun/ZJetsToNuNu_HT-200To400_13TeV-madgraph/crab_iDMAnalysis_ZJetsToNuNu_HT-200To400/191103_081822/0000/output_99.root");

    TFile *_file9 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2016/data_seventhrun/MET/crab_iDMAnalysis_MET_2016RunF_v3/191217_201558/0000/output_49.root");
    TFile *_file10 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2017/data_seventhrun/MET/crab_iDMAnalysis_MET_2017RunF_v2/191126_154824/0000/output_100.root");
    TFile *_file11 = TFile::Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/data_seventhrun/MET/crab_iDMAnalysis_MET_Run2018D_v4/191127_191525/0000/output_99.root");

    auto * t16_1 = (TTree*)_file0->Get("ntuples_gbm/recoT");
    t16_1->SetLineColor(kRed);
    auto * t16_10 = (TTree*)_file1->Get("ntuples_gbm/recoT");
    t16_10->SetLineColor(kGreen);
    auto * t17_1 = (TTree*)_file2->Get("ntuples_gbm/recoT");
    t17_1->SetLineColor(kRed);
    auto * t17_10 = (TTree*)_file3->Get("ntuples_gbm/recoT");
    t17_10->SetLineColor(kGreen);
    auto * t18_1 = (TTree*)_file4->Get("ntuples_gbm/recoT");
    t18_1->SetLineColor(kRed);
    auto * t18_10 = (TTree*)_file5->Get("ntuples_gbm/recoT");
    t18_10->SetLineColor(kGreen);
    auto * t16_ZJets = (TTree*)_file6->Get("ntuples_gbm/recoT");
    auto * t17_ZJets = (TTree*)_file7->Get("ntuples_gbm/recoT");
    auto * t18_ZJets = (TTree*)_file8->Get("ntuples_gbm/recoT");
    auto * t16_data = (TTree*)_file9->Get("ntuples_gbm/recoT");
    t16_data->SetLineColor(kBlack);
    auto * t17_data = (TTree*)_file10->Get("ntuples_gbm/recoT");
    t17_data->SetLineColor(kBlack);
    auto * t18_data = (TTree*)_file11->Get("ntuples_gbm/recoT");
    t18_data->SetLineColor(kBlack);

    TCanvas * c1 = new TCanvas();

    t16_ZJets->Draw("abs(reco_gm_dxy)>>h(100,0,0.5)", "", "NORM HIST");
    TH1F *h = (TH1F*)gDirectory->Get("h");
    h->SetMaximum(0.13);
    h->SetTitle("2016");
    h->GetXaxis()->SetTitle("Global muon dxy [cm]");
    h->GetYaxis()->SetTitle("A. U.");
    t16_1->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t16_10->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t16_data->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");

    TLegend * leg1 = new TLegend(0.6, 0.65, 0.85, 0.85);
    leg1->AddEntry(t16_ZJets, "ZJetsToNuNu");
    leg1->AddEntry(t16_1, "signal 1mm");
    leg1->AddEntry(t16_10, "signal 10mm");
    leg1->AddEntry(t16_data, "data");
    leg1->Draw();

    TCanvas * c2 = new TCanvas();

    t17_ZJets->Draw("abs(reco_gm_dxy)>>h2(100,0,0.5)", "", "NORM HIST");
    TH1F *h2 = (TH1F*)gDirectory->Get("h2");
    h2->SetMaximum(0.13);
    h2->SetTitle("2017");
    h2->GetXaxis()->SetTitle("Global muon dxy [cm]");
    h2->GetYaxis()->SetTitle("A. U.");
    t17_1->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t17_10->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t17_data->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");

    TLegend * leg2 = new TLegend(0.6, 0.65, 0.85, 0.85);
    leg2->AddEntry(t17_ZJets, "ZJetsToNuNu");
    leg2->AddEntry(t17_1, "signal 1mm");
    leg2->AddEntry(t17_10, "signal 10mm");
    leg2->AddEntry(t17_data, "data");
    leg2->Draw();

    TCanvas * c3 = new TCanvas();

    t18_ZJets->Draw("abs(reco_gm_dxy)>>h3(100,0,0.5)", "", "NORM HIST");
    TH1F *h3 = (TH1F*)gDirectory->Get("h3");
    h3->SetMaximum(0.13);
    h3->SetTitle("2018");
    h3->GetXaxis()->SetTitle("Global muon dxy [cm]");
    h3->GetYaxis()->SetTitle("A. U.");
    t18_1->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t18_10->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");
    t18_data->Draw("abs(reco_gm_dxy)", "", "NORM HIST SAME");

    TLegend * leg3 = new TLegend(0.6, 0.65, 0.85, 0.85);
    leg3->AddEntry(t18_ZJets, "ZJetsToNuNu");
    leg3->AddEntry(t18_1, "signal 1mm");
    leg3->AddEntry(t18_10, "signal 10mm");
    leg3->AddEntry(t18_data, "data");
    leg3->Draw();
}
