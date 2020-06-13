#include "../CMS_lumi.C"

void PlotCosmicMuonEfficiencyMC(int year=2016, bool fSave=0) {
    //TH1::AddDirectory(kFALSE);
    gROOT->LoadMacro("../tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");
    gStyle->SetOptStat("nmr");

    TFile *_file0;
    TFile *_file1;
    TFile *_file2;
    TFile *_file3;

    if (year == 2016) {
        _file0 = TFile::Open("../../bin/PlotsCosmicsStudiesJune11TighterID/plots_Cosmics_twelthrun_efficiency_pt25_dzlt100_MC_2016.root");
        _file1 = TFile::Open("../../bin/PlotsCosmicsStudiesJune11TighterID/plots_Cosmics_twelthrun_efficiency_pt10_dzlt100_MC_2016.root");
        _file2 = TFile::Open("../../bin/PlotsCosmicsStudiesJune11TighterID/plots_Cosmics_twelthrun_efficiency_pt25_dzlt200_MC_2016.root");
        _file3 = TFile::Open("../../bin/PlotsCosmicsStudiesJune11TighterID/plots_Cosmics_twelthrun_efficiency_pt10_dzlt200_MC_2016.root");
    }
    else if (year == 2017) {
        _file0 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt25_dzlt100_2017.root");
        _file1 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt10_dzlt100_2017.root");
        _file2 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt25_dzlt200_2017.root");
        _file3 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt10_dzlt200_2017.root");
    }
    else if (year == 2018) {
        _file0 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt25_dzlt100_2018.root");
        _file1 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt10_dzlt100_2018.root");
        _file2 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt25_dzlt200_2018.root");
        _file3 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_efficiency_pt10_dzlt200_2018.root");
    }

    TString obs = "eta";
    THStack * hs0_num = (THStack*)_file0->Get(Form("tag_muon_%s_numcut5-DATA", obs.Data()));
    THStack * hs0_denom = (THStack*)_file0->Get(Form("tag_muon_%s_denomcut4-DATA", obs.Data()));
    THStack * hs1_num = (THStack*)_file1->Get(Form("tag_muon_%s_numcut5-DATA", obs.Data()));
    THStack * hs1_denom = (THStack*)_file1->Get(Form("tag_muon_%s_denomcut4-DATA", obs.Data()));
    THStack * hs2_num = (THStack*)_file2->Get(Form("tag_muon_%s_numcut5-DATA", obs.Data()));
    THStack * hs2_denom = (THStack*)_file2->Get(Form("tag_muon_%s_denomcut4-DATA", obs.Data()));
    THStack * hs3_num = (THStack*)_file3->Get(Form("tag_muon_%s_numcut5-DATA", obs.Data()));
    THStack * hs3_denom = (THStack*)_file3->Get(Form("tag_muon_%s_denomcut4-DATA", obs.Data()));

    TH1F * obs0_num = (TH1F*)hs0_num->GetStack()->Last();
    TH1F * obs0_denom = (TH1F*)hs0_denom->GetStack()->Last();
    TEfficiency * obs0_eff = new TEfficiency(*obs0_num, *obs0_denom);
    TH1F * obs1_num = (TH1F*)hs1_num->GetStack()->Last();
    TH1F * obs1_denom = (TH1F*)hs1_denom->GetStack()->Last();
    TEfficiency * obs1_eff = new TEfficiency(*obs1_num, *obs1_denom);
    TH1F * obs2_num = (TH1F*)hs2_num->GetStack()->Last();
    TH1F * obs2_denom = (TH1F*)hs2_denom->GetStack()->Last();
    TEfficiency * obs2_eff = new TEfficiency(*obs2_num, *obs2_denom);
    TH1F * obs3_num = (TH1F*)hs3_num->GetStack()->Last();
    TH1F * obs3_denom = (TH1F*)hs3_denom->GetStack()->Last();
    TEfficiency * obs3_eff = new TEfficiency(*obs3_num, *obs3_denom);

    TCanvas * c0 = new TCanvas();
    TH1F * frame0;
    if (obs == "dxy")
        frame0 = c0->DrawFrame(0, 0, 300, 1.3, ";Lower leg (tag) dxy [cm];Efficiency");
    else if (obs == "eta")
        frame0 = c0->DrawFrame(-1.2, 0, 1.2, 1.3, ";Lower leg (tag) eta;Efficiency");
    else if (obs == "pt")
        frame0 = c0->DrawFrame(0, 0, 100, 1.3, ";Lower leg (tag) pT [GeV];Efficiency");
    frame0->GetYaxis()->SetTitleOffset(1.2);

    obs0_eff->SetLineColor(kBlue);
    obs0_eff->SetLineWidth(2);
    obs0_eff->SetMarkerStyle(20);
    obs0_eff->SetMarkerColor(kBlue);
    obs0_eff->SetMarkerSize(1.0);
    obs0_eff->Draw("EPZ SAME");

    obs1_eff->SetLineColor(kRed);
    obs1_eff->SetLineWidth(2);
    obs1_eff->SetMarkerStyle(20);
    obs1_eff->SetMarkerColor(kRed);
    obs1_eff->SetMarkerSize(1.0);
    obs1_eff->Draw("EPZ SAME");

    obs2_eff->SetLineColor(kGreen+1);
    obs2_eff->SetLineWidth(2);
    obs2_eff->SetMarkerStyle(20);
    obs2_eff->SetMarkerColor(kGreen+1);
    obs2_eff->SetMarkerSize(1.0);
    obs2_eff->Draw("EPZ SAME");

    obs3_eff->SetLineColor(kViolet);
    obs3_eff->SetLineWidth(2);
    obs3_eff->SetMarkerStyle(20);
    obs3_eff->SetMarkerColor(kViolet);
    obs3_eff->SetMarkerSize(1.0);
    obs3_eff->Draw("EPZ SAME");

    TLegend * leg0 = new TLegend(0.3, 0.77, 0.7, 0.92);
    leg0->AddEntry(obs0_eff, Form("%d, p_{T} > 25 GeV, |dz| < 100 cm", year), "lep");
    leg0->AddEntry(obs1_eff, Form("%d, p_{T} > 10 GeV, |dz| < 100 cm", year), "lep");
    leg0->AddEntry(obs2_eff, Form("%d, p_{T} > 25 GeV, |dz| < 200 cm", year), "lep");
    leg0->AddEntry(obs3_eff, Form("%d, p_{T} > 10 GeV, |dz| < 200 cm", year), "lep");
    leg0->Draw();

    //CMS_lumi(c, 4, 0);
    c0->Modified();
    c0->Update();

    if (fSave) {
        c0->SaveAs(Form("../../images/cosmics_efficiency_vs_dxy_%d.pdf", year));
    }
    
    // nHits

    //hs0 = (THStack*)_file0->Get("tag_muon_nhits_cut5-DATA");
    //hs1 = (THStack*)_file1->Get("tag_muon_nhits_cut5-DATA");
    //hs2 = (THStack*)_file2->Get("tag_muon_nhits_cut5-DATA");
    //hs3 = (THStack*)_file3->Get("tag_muon_nhits_cut5-DATA");

    //TH1F * nhits0 = (TH1F*)hs0->GetStack()->Last();
    //TH1F * nhits1 = (TH1F*)hs1->GetStack()->Last();
    //TH1F * nhits2 = (TH1F*)hs2->GetStack()->Last();
    //TH1F * nhits3 = (TH1F*)hs3->GetStack()->Last();

    //TCanvas * c1 = new TCanvas();
    //auto * frame1 = c1->DrawFrame(0, 0, 60, 0.25, ";Lower leg (tag) nHits;A. U.");
    //frame1->GetYaxis()->SetTitleOffset(1.3);

    //nhits0->SetLineColor(kBlue);
    //nhits0->SetLineWidth(2);
    //nhits0->DrawNormalized("HIST SAME");
    //nhits0->SetAxisRange(0, 1.2, "Y");
    //nhits1->SetLineColor(kOrange);
    //nhits1->SetLineWidth(2);
    //nhits1->DrawNormalized("HIST SAME");
    //nhits2->SetLineColor(kViolet);
    //nhits2->SetLineWidth(2);
    //nhits2->DrawNormalized("HIST SAME");
    //nhits3->SetLineColor(kGreen);
    //nhits3->SetLineWidth(2);
    //nhits3->DrawNormalized("HIST SAME");

    //TLegend * leg1 = new TLegend(0.5, 0.7, 0.8, 0.9);
    //leg1->AddEntry(nhits0, Form("%d, 0 < dxy < 50 cm", year), "l");
    //leg1->AddEntry(nhits1, Form("%d, 50 < dxy < 100 cm", year), "l");
    //leg1->AddEntry(nhits2, Form("%d, 100 < dxy < 150 cm", year), "l");
    //leg1->AddEntry(nhits3, Form("%d, 150 < dxy < 250 cm", year), "l");
    //leg1->Draw();


    ////CMS_lumi(c, 4, 0);

    //c1->Modified();
    //c1->Update();

    //if (fSave) {
    //    c1->SaveAs("../../images/muon_pt_resolution.pdf");
    //}

    _file0->Close();
}
