#include "utils/tdrstyle.C"

void cutflowPlots(TString variable, TString description, int nbins, float lowx, float highx, bool fSave=false) {
    //gROOT->LoadMacro("tdrstyle.C");
    setTDRStyle();

    std::map<TString, TString> plot_variables {
        {"leading_mu_pt", "reco_mu_pt[0]"},
        {"subleading_mu_pt", "reco_mu_pt[1]"},
        {"leading_mu_eta", "reco_mu_eta[0]"},
        {"subleading_mu_eta", "reco_mu_eta[1]"},
        {"leading_mu_phi", "reco_mu_phi[0]"},
        {"subleading_mu_phi", "reco_mu_phi[1]"},
        {"mu_dR", "reco_vertex_dR"},
        {"mu_M2", "sqrt(reco_mu_M2)"},
        {"leading_jet_pt", "reco_PF_jet_pt[0]"},
        {"leading_jet_eta", "reco_PF_jet_eta[0]"},
        {"leading_jet_phi", "reco_PF_jet_phi[0]"},
        {"MET_pt", "reco_PF_MET_pt"},
        {"MET_phi", "reco_PF_MET_phi"},
        {"MET_mu_dphi", "reco_MET_mu_DeltaPhi"}
    };

    typedef struct sample_info {
        TString filename;
        TString label;
        int color;
    } sample_info;

    std::map<TString, sample_info> samples {
        {"5p25", sample_info{
                "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-100.root",
                "m1 = 5 GeV, #Delta = 0.1",
                kRed+1}},
        {"52p5", sample_info{
                "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-100.root",
                "m1 = 50 GeV, #Delta = 0.1",
                kBlue+1}},
        {"6p0", sample_info{
                "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-100.root",
                "m1 = 5 GeV, #Delta = 0.4",
                kGreen+1}},
        {"60p0", sample_info{
                "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-100.root",
                "m1 = 50 GeV, #Delta = 0.4",
                kBlack}}
    };

    std::map<TString, std::map<TString, TH1F *> > hists;
    int numCuts_ = 16;

    typedef struct cut_info {
        TCut cut;
        TString label;
    } cut_info;

    TCut noSelection = "";
    TCut initialSelection = "cutsVec[1] && cutsVec[2] && cutsVec[4] && cutsVec[5] && cutsVec[6] && cutsVec[7]";
    TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    TCut controlRegion_QCD = initialSelection && "!cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    TCut controlRegion_EW = initialSelection && "cutsVec[3] && !cutsVec[9] && !cutsVec[10] && !cutsVec[11] && !cutsVec[12] && !cutsVec[14]";
    TCut controlRegion_TTbar = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && !cutsVec[14]";

    std::map<TString, cut_info> cuts {
        {"no_selection", cut_info{noSelection, "No Selection"}},
        {"initial_selection", cut_info{initialSelection, "Initial Selection"}},
        {"signal_region", cut_info{signalRegion, "Signal Region"}},
        {"control_region_QCD", cut_info{controlRegion_QCD, "QCD Control Region"}},
        {"control_region_EW", cut_info{controlRegion_EW, "EW Control Region"}},
        {"control_region_TTbar", cut_info{controlRegion_TTbar, "TTbar Control Region"}}
    };

    std::map<TString, TCanvas *> canvases;

    for (auto & sample : samples) {

        TFile * f = TFile::Open(sample.second.filename.Data());
        TTree * recoT = (TTree*)f->Get("SREffi_dsa/reco");
        recoT->UseCurrentStyle();

        for (auto cut : cuts) {
            recoT->Draw(Form("%s>>h%s_%s(%d,%f,%f)", plot_variables.at(variable).Data(), cut.first.Data(), sample.first.Data(), nbins, lowx, highx), cut.second.cut, "goff");
            hists[sample.first][cut.first] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut.first.Data(), sample.first.Data()));
        }
    }
    for (auto cut : cuts) {
        THStack * hs = new THStack(Form("%s", cut.first.Data()), Form("%s", cut.second.label.Data()));
        auto legend = new TLegend(0.5,0.7,0.8,0.9);
        //auto legend = new TLegend(0.25,0.2);
        for (auto & sample : samples) {
            hists[sample.first][cut.first]->Scale(1/hists[sample.first][cut.first]->Integral(), "width");
            hists[sample.first][cut.first]->SetLineColor(sample.second.color);
            hists[sample.first][cut.first]->SetLineWidth(2);
            hs->Add(hists[sample.first][cut.first]);
            legend->AddEntry(hists[sample.first][cut.first], sample.second.label.Data());
        }
        canvases[cut.first] = new TCanvas();
        canvases[cut.first]->cd();
        hs->Draw("E HIST nostack");
        TPaveText *text = new TPaveText(0.25,0.8,0.4,1.0, "NDC");
        text->AddText(Form("%s", cut.second.label.Data()));
        text->SetBorderSize(0);
        text->SetFillColor(0);
        text->SetTextFont(53); 
        text->SetTextSize(20);
        text->SetFillStyle(0);
        hs->GetXaxis()->SetTitle(Form("%s", description.Data()));
        hs->GetYaxis()->SetTitle("A. U.");
        hs->GetYaxis()->SetTitleOffset(1.4);
        gPad->Modified();
        legend->Draw();
        text->Draw();
        if (fSave) {
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.pdf", variable.Data(), cut.first.Data()));
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.png", variable.Data(), cut.first.Data()));
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.root", variable.Data(), cut.first.Data()));
        }
    }
}
