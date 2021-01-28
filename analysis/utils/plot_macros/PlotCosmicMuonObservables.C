#include "../utils/CMS_lumi.C"

void PlotCosmicMuonObservables(int year=2016, bool fSave=0) {
    //TH1::AddDirectory(kFALSE);
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");
    gStyle->SetOptStat("nmr");

    TFile *_file0;
    TFile *_file1;
    TFile *_file2;
    TFile *_file3;
    TFile *_file4;

    if (year == 2016) {
        _file0 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy0to50_2016.root");
        _file1 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy50to100_2016.root");
        _file2 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy100to150_2016.root");
        _file3 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy150to250_2016.root");
        _file4 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxyInclusive_signal_2016.root");
    }
    else if (year == 2017) {
        _file0 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy0to50_2017.root");
        _file1 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy50to100_2017.root");
        _file2 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy100to150_2017.root");
        _file3 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy150to250_2017.root");
        _file4 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxyInclusive_signal_2017.root");
    }
    else if (year == 2018) {
        _file0 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy0to50_2018.root");
        _file1 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy50to100_2018.root");
        _file2 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy100to150_2018.root");
        _file3 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxy150to250_2018.root");
        _file4 = TFile::Open("PlotsCosmicsStudiesApril23/plots_Cosmics_eleventhrun_observables_dxyInclusive_signal_2018.root");
    }

    THStack * hs0 = (THStack*)_file0->Get("charge_product_cut5-DATA");
    THStack * hs1 = (THStack*)_file1->Get("charge_product_cut5-DATA");
    THStack * hs2 = (THStack*)_file2->Get("charge_product_cut5-DATA");
    THStack * hs3 = (THStack*)_file3->Get("charge_product_cut5-DATA");
    //THStack * hs4 = (THStack*)_file4->Get("charge_product_cut5-SIGNAL");

    TH1F * charge0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * charge1 = (TH1F*)hs1->GetStack()->Last();
    TH1F * charge2 = (TH1F*)hs2->GetStack()->Last();
    TH1F * charge3 = (TH1F*)hs3->GetStack()->Last();
    //TH1F * charge4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c0 = new TCanvas();
    auto * frame0 = c0->DrawFrame(-2, 0, 3, 1.2, ";Charge(upper) * Charge(lower);A. U.");
    frame0->GetYaxis()->SetTitleOffset(1.2);

    charge0->SetLineColor(kBlue);
    charge0->SetLineWidth(2);
    charge0->DrawNormalized("HIST SAME");
    charge0->SetAxisRange(0, 1.2, "Y");
    charge1->SetLineColor(kOrange);
    charge1->SetLineWidth(2);
    charge1->DrawNormalized("HIST SAME");
    charge2->SetLineColor(kViolet);
    charge2->SetLineWidth(2);
    charge2->DrawNormalized("HIST SAME");
    charge3->SetLineColor(kGreen+1);
    charge3->SetLineWidth(2);
    charge3->DrawNormalized("HIST SAME");
    //charge4->SetLineColor(kBlack);
    //charge4->SetLineWidth(2);
    //charge4->DrawNormalized("HIST SAME");

    TLegend * leg0 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg0->AddEntry(charge0, Form("%d, 0 < dxy < 50 cm", year), "l");
    leg0->AddEntry(charge1, Form("%d, 50 < dxy < 100 cm", year), "l");
    leg0->AddEntry(charge2, Form("%d, 100 < dxy < 150 cm", year), "l");
    leg0->AddEntry(charge3, Form("%d, 150 < dxy < 250 cm", year), "l");
    //leg0->AddEntry(charge4, Form("%d, signal inclusive", year), "l");
    leg0->Draw();

    //CMS_lumi(c, 4, 0);
    c0->Modified();
    c0->Update();

    if (fSave) {
        c0->SaveAs(Form("../../images/cosmics_observables_charge_product_%d.pdf", year));
    }
    
    // nHits
    hs0 = (THStack*)_file0->Get("tag_muon_nhits_cut5-DATA");
    hs1 = (THStack*)_file1->Get("tag_muon_nhits_cut5-DATA");
    hs2 = (THStack*)_file2->Get("tag_muon_nhits_cut5-DATA");
    hs3 = (THStack*)_file3->Get("tag_muon_nhits_cut5-DATA");
    THStack * hs4 = (THStack*)_file4->Get("tag_muon_nhits_cut5-SIGNAL");

    TH1F * nhits0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * nhits1 = (TH1F*)hs1->GetStack()->Last();
    TH1F * nhits2 = (TH1F*)hs2->GetStack()->Last();
    TH1F * nhits3 = (TH1F*)hs3->GetStack()->Last();
    TH1F * nhits4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c1 = new TCanvas();
    auto * frame1 = c1->DrawFrame(0, 0, 60, 0.25, ";Lower leg (tag) nHits;A. U.");
    frame1->GetYaxis()->SetTitleOffset(1.3);

    nhits0->SetLineColor(kBlue);
    nhits0->SetLineWidth(2);
    nhits0->DrawNormalized("HIST SAME");
    nhits0->SetAxisRange(0, 1.2, "Y");
    nhits1->SetLineColor(kOrange);
    nhits1->SetLineWidth(2);
    nhits1->DrawNormalized("HIST SAME");
    nhits2->SetLineColor(kViolet);
    nhits2->SetLineWidth(2);
    nhits2->DrawNormalized("HIST SAME");
    nhits3->SetLineColor(kGreen+1);
    nhits3->SetLineWidth(2);
    nhits3->DrawNormalized("HIST SAME");
    nhits4->SetLineColor(kBlack);
    nhits4->SetLineWidth(2);
    nhits4->DrawNormalized("HIST SAME");

    TLegend * leg1 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg1->AddEntry(nhits0, Form("%d, 0 < dxy < 50 cm", year), "l");
    leg1->AddEntry(nhits1, Form("%d, 50 < dxy < 100 cm", year), "l");
    leg1->AddEntry(nhits2, Form("%d, 100 < dxy < 150 cm", year), "l");
    leg1->AddEntry(nhits3, Form("%d, 150 < dxy < 250 cm", year), "l");
    leg1->AddEntry(nhits4, Form("%d signal, inclusive dxy", year), "l");
    leg1->Draw();


    //CMS_lumi(c, 4, 0);

    c1->Modified();
    c1->Update();

    if (fSave) {
        c1->SaveAs(Form("../../images/cosmics_observables_nhits_%d.pdf", year));
    }


    // nStations
    hs0 = (THStack*)_file0->Get("tag_muon_nplanes_cut5-DATA");
    hs1 = (THStack*)_file1->Get("tag_muon_nplanes_cut5-DATA");
    hs2 = (THStack*)_file2->Get("tag_muon_nplanes_cut5-DATA");
    hs3 = (THStack*)_file3->Get("tag_muon_nplanes_cut5-DATA");
    hs4 = (THStack*)_file4->Get("tag_muon_nplanes_cut5-SIGNAL");

    TH1F * nplanes0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * nplanes1 = (TH1F*)hs1->GetStack()->Last();
    TH1F * nplanes2 = (TH1F*)hs2->GetStack()->Last();
    TH1F * nplanes3 = (TH1F*)hs3->GetStack()->Last();
    TH1F * nplanes4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c2 = new TCanvas();
    auto * frame2 = c2->DrawFrame(0, 0, 7, 1, ";Lower leg (tag) nStations;A. U.");
    frame2->GetYaxis()->SetTitleOffset(1.3);

    nplanes0->SetLineColor(kBlue);
    nplanes0->SetLineWidth(2);
    nplanes0->DrawNormalized("HIST SAME");
    nplanes0->SetAxisRange(0, 1.2, "Y");
    nplanes1->SetLineColor(kOrange);
    nplanes1->SetLineWidth(2);
    nplanes1->DrawNormalized("HIST SAME");
    nplanes2->SetLineColor(kViolet);
    nplanes2->SetLineWidth(2);
    nplanes2->DrawNormalized("HIST SAME");
    nplanes3->SetLineColor(kGreen+1);
    nplanes3->SetLineWidth(2);
    nplanes3->DrawNormalized("HIST SAME");
    nplanes4->SetLineColor(kBlack);
    nplanes4->SetLineWidth(2);
    nplanes4->DrawNormalized("HIST SAME");

    TLegend * leg2 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg2->AddEntry(nplanes0, Form("%d, 0 < dxy < 50 cm", year), "l");
    leg2->AddEntry(nplanes1, Form("%d, 50 < dxy < 100 cm", year), "l");
    leg2->AddEntry(nplanes2, Form("%d, 100 < dxy < 150 cm", year), "l");
    leg2->AddEntry(nplanes3, Form("%d, 150 < dxy < 250 cm", year), "l");
    leg2->AddEntry(nplanes4, Form("%d signal, inclusive dxy", year), "l");
    leg2->Draw();


    //CMS_lumi(c, 4, 0);

    c2->Modified();
    c2->Update();

    if (fSave) {
        c2->SaveAs(Form("../../images/cosmics_observables_nplanes_%d.pdf", year));
    }
    
    // pt res
    hs0 = (THStack*)_file0->Get("tag_muon_ptres_cut5-DATA");
    hs1 = (THStack*)_file1->Get("tag_muon_ptres_cut5-DATA");
    hs2 = (THStack*)_file2->Get("tag_muon_ptres_cut5-DATA");
    hs3 = (THStack*)_file3->Get("tag_muon_ptres_cut5-DATA");
    hs4 = (THStack*)_file4->Get("tag_muon_ptres_cut5-SIGNAL");

    TH1F * ptres0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * ptres1 = (TH1F*)hs1->GetStack()->Last();
    TH1F * ptres2 = (TH1F*)hs2->GetStack()->Last();
    TH1F * ptres3 = (TH1F*)hs3->GetStack()->Last();
    TH1F * ptres4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c3 = new TCanvas();
    auto * frame3 = c3->DrawFrame(0, 0.01, 2, 1, ";Lower leg (tag) #sigma_{p_{T}}/p_{T};A. U.");
    frame3->GetYaxis()->SetTitleOffset(1.3);
    c3->SetLogy();

    ptres0->SetLineColor(kBlue);
    ptres0->SetLineWidth(2);
    ptres0->DrawNormalized("HIST SAME");
    ptres0->SetAxisRange(0, 1.2, "Y");
    ptres1->SetLineColor(kOrange);
    ptres1->SetLineWidth(2);
    ptres1->DrawNormalized("HIST SAME");
    ptres2->SetLineColor(kViolet);
    ptres2->SetLineWidth(2);
    ptres2->DrawNormalized("HIST SAME");
    ptres3->SetLineColor(kGreen+1);
    ptres3->SetLineWidth(2);
    ptres3->DrawNormalized("HIST SAME");
    ptres4->SetLineColor(kBlack);
    ptres4->SetLineWidth(2);
    ptres4->DrawNormalized("HIST SAME");

    TLegend * leg3 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg3->AddEntry(ptres0, Form("%d, 0 < dxy < 50 cm", year), "l");
    leg3->AddEntry(ptres1, Form("%d, 50 < dxy < 100 cm", year), "l");
    leg3->AddEntry(ptres2, Form("%d, 100 < dxy < 150 cm", year), "l");
    leg3->AddEntry(ptres3, Form("%d, 150 < dxy < 250 cm", year), "l");
    leg3->AddEntry(ptres4, Form("%d signal, inclusive dxy", year), "l");
    leg3->Draw();


    //CMS_lumi(c, 4, 0);

    c3->Modified();
    c3->Update();

    if (fSave) {
        c3->SaveAs(Form("../../images/cosmics_observables_ptres_%d.pdf", year));
    }
    
    // eta 
    hs0 = (THStack*)_file0->Get("tag_muon_eta_cut1-DATA");
    hs4 = (THStack*)_file4->Get("tag_muon_eta_cut1-SIGNAL");

    TH1F * eta0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * eta4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c4 = new TCanvas();
    auto * frame4 = c4->DrawFrame(-2.5, 0, 2.5, 0.25, ";#eta;A. U.");
    frame4->GetYaxis()->SetTitleOffset(1.3);

    eta0->SetLineColor(kBlue);
    eta0->SetLineWidth(2);
    eta0->DrawNormalized("HIST SAME");
    eta0->SetAxisRange(0, 1.2, "Y");
    eta4->SetLineColor(kBlack);
    eta4->SetLineWidth(2);
    eta4->DrawNormalized("HIST SAME");

    TLegend * leg4 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg4->AddEntry(eta0, Form("%d, inclusive dxy", year), "l");
    leg4->AddEntry(eta4, Form("%d signal, inclusive dxy", year), "l");
    leg4->Draw();


    //CMS_lumi(c, 4, 0);

    c4->Modified();
    c4->Update();

    if (fSave) {
        c4->SaveAs(Form("../../images/cosmics_observables_eta_%d.pdf", year));
    }
    
    // pt 
    hs0 = (THStack*)_file0->Get("tag_muon_pt_cut1-DATA");
    hs4 = (THStack*)_file4->Get("tag_muon_pt_cut1-SIGNAL");

    TH1F * pt0 = (TH1F*)hs0->GetStack()->Last();
    TH1F * pt4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c5 = new TCanvas();
    auto * frame5 = c5->DrawFrame(0, 0.0001, 500, 1, ";p_{T} [GeV];A. U.");
    frame5->GetYaxis()->SetTitleOffset(1.3);
    c5->SetLogy();

    pt0->SetLineColor(kBlue);
    pt0->SetLineWidth(2);
    pt0->DrawNormalized("HIST SAME");
    pt0->SetAxisRange(0, 1.2, "Y");
    pt4->SetLineColor(kBlack);
    pt4->SetLineWidth(2);
    pt4->DrawNormalized("HIST SAME");

    TLegend * leg5 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg5->AddEntry(pt0, Form("%d, inclusive dxy", year), "l");
    leg5->AddEntry(pt4, Form("%d signal, inclusive dxy", year), "l");
    leg5->Draw();


    //CMS_lumi(c, 4, 0);

    c5->Modified();
    c5->Update();

    if (fSave) {
        c5->SaveAs(Form("../../images/cosmics_observables_pt_%d.pdf", year));
    }
    
    // phi
    hs0 = (THStack*)_file0->Get("tag_muon_phi_cut1-DATA");
    //hs1 = (THStack*)_file1->Get("tag_muon_phi_cut1-DATA");
    //hs2 = (THStack*)_file2->Get("tag_muon_phi_cut1-DATA");
    //hs3 = (THStack*)_file3->Get("tag_muon_phi_cut1-DATA");
    hs4 = (THStack*)_file4->Get("tag_muon_phi_cut1-SIGNAL");

    TH1F * phi0 = (TH1F*)hs0->GetStack()->Last();
    //TH1F * phi1 = (TH1F*)hs1->GetStack()->Last();
    //TH1F * phi2 = (TH1F*)hs2->GetStack()->Last();
    //TH1F * phi3 = (TH1F*)hs3->GetStack()->Last();
    TH1F * phi4 = (TH1F*)hs4->GetStack()->Last();

    TCanvas * c6 = new TCanvas();
    auto * frame6 = c6->DrawFrame(-3.2, 0, 3.2, 0.15, ";#phi;A. U.");
    frame6->GetYaxis()->SetTitleOffset(1.3);

    phi0->SetLineColor(kBlue);
    phi0->SetLineWidth(2);
    phi0->DrawNormalized("HIST SAME");
    phi0->SetAxisRange(0, 1.2, "Y");
    //phi1->SetLineColor(kOrange);
    //phi1->SetLineWidth(2);
    //phi1->DrawNormalized("HIST SAME");
    //phi2->SetLineColor(kViolet);
    //phi2->SetLineWidth(2);
    //phi2->DrawNormalized("HIST SAME");
    //phi3->SetLineColor(kGreen+1);
    //phi3->SetLineWidth(2);
    //phi3->DrawNormalized("HIST SAME");
    phi4->SetLineColor(kBlack);
    phi4->SetLineWidth(2);
    phi4->DrawNormalized("HIST SAME");

    TLegend * leg6 = new TLegend(0.5, 0.7, 0.8, 0.9);
    leg6->AddEntry(phi0, Form("%d, inclusive dxy", year), "l");
    //leg6->AddEntry(phi1, Form("%d, 50 < dxy < 100 cm", year), "l");
    //leg6->AddEntry(phi2, Form("%d, 100 < dxy < 150 cm", year), "l");
    //leg6->AddEntry(phi3, Form("%d, 150 < dxy < 250 cm", year), "l");
    leg6->AddEntry(phi4, Form("%d signal, inclusive dxy", year), "l");
    leg6->Draw();


    //CMS_lumi(c, 4, 0);

    c6->Modified();
    c6->Update();

    if (fSave) {
        c6->SaveAs(Form("../../images/cosmics_observables_phi_%d.pdf", year));
    }

    _file0->Close();
    _file1->Close();
    _file2->Close();
    _file3->Close();
    _file4->Close();
}
