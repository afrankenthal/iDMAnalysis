#include "tdrstyle.C"

using std::cout, std::endl, std::map, std::vector;

bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

//void nminus1Plots(TString variable, TString description, int nbins, float lowx, float highx, bool fSave=false) {
void nminus1Plots() {
    bool fSave = false;
    //gROOT->LoadMacro("tdrstyle.C");
    setTDRStyle();

    map<TString, TString> plot_variables {
        {"leading_mu_pt", "reco_mu_pt[0]"},
        {"subleading_mu_pt", "reco_mu_pt[1]"},
        {"leading_mu_eta", "reco_mu_eta[0]"},
        {"subleading_mu_eta", "reco_mu_eta[1]"},
        {"leading_mu_phi", "reco_mu_phi[0]"},
        {"subleading_mu_phi", "reco_mu_phi[1]"},
        {"mu_dR", "reco_vtx_dR"},
        {"mu_M2", "sqrt(reco_mu_M2)"},
        {"leading_jet_pt", "reco_PF_jet_pt[0]"},
        {"leading_jet_eta", "reco_PF_jet_eta[0]"},
        {"leading_jet_phi", "reco_PF_jet_phi[0]"},
        {"MET_pt", "reco_PF_MET_pt"},
        {"MET_phi", "reco_PF_MET_phi"},
        {"MET_mu_dphi", "reco_MET_mu_DeltaPhi"},
    };

    typedef struct sample_info {
        TString filename;
        TString label;
        int color;
    } sample_info;

    vector<TString> ctaus{"1", "10", "100", "1000"};
    vector<TString> mchis{"5p25", "52p5", "6p0", "60p0"};
    map<TString, TString> dmchis{{"5p25","0p5"},{"52p5","5p0"},{"6p0","2p0"},{"60p0","20p0"}};
    map<TString, TString> m1s{{"5p25", "5"},{"52p5", "50"},{"6p0", "5"},{"60p0", "50"}};
    map<TString, TString> deltas{{"5p25", "0.1"},{"52p5", "0.1"}, {"6p0", "0.4"},{"60p0", "0.4"}};
    map<TString, int> colors{{"5p25", kYellow},{"52p5", kBlue},{"6p0", kGreen},{"60p0", kRed}};
    map<TString, sample_info> samples;
    for (auto ctau : ctaus) 
        for (auto mchi : mchis) 
                samples.insert(std::make_pair(Form("%s_%s", mchi.Data(), ctau.Data()),
                        sample_info{Form("../Mchi-%s_dMchi-%s_ctau-%s.root", mchi.Data(), dmchis[mchi].Data(), ctau.Data()),
                        Form("m1 = %s GeV, #Delta = %s, c#tau = %s mm", m1s[mchi].Data(), deltas[mchi].Data(), ctau.Data()),
                        colors[mchi] + 1*(int)(distance(ctaus.begin(),find(ctaus.begin(), ctaus.end(), ctau)))}));

   // map<TString, sample_info> samples {
   //     {"5p25", sample_info{
   //             "../Mchi-5p25_dMchi-0p5_ctau-1.root",
   //             "m1 = 5 GeV, #Delta = 0.1",
   //             kRed+1}},
   //     {"52p5", sample_info{
   //             "../Mchi-52p5_dMchi-5p0_ctau-1.root",
   //             "m1 = 50 GeV, #Delta = 0.1",
   //             kBlue+1}},
   //     {"6p0", sample_info{
   //             "../Mchi-6p0_dMchi-2p0_ctau-1.root",
   //             "m1 = 5 GeV, #Delta = 0.4",
   //             kGreen+1}},
   //     {"60p0", sample_info{
   //             "../Mchi-60p0_dMchi-20p0_ctau-1.root",
   //             "m1 = 50 GeV, #Delta = 0.4",
   //             kBlack}}
   // };

    map<TString, map<TString, TH1F *>> hists;
    int numCuts_ = 19;

    typedef struct cut_info {
        TCut cut;
        TString plot_var;
        TString plot_description;
        Int_t nbins;
        Float_t lowX;
        Float_t highX;
        TString label;
        vector<TString> samples;
    } cut_info;

    vector<TCut> bits;
    for (int bit = 0; bit < numCuts_; bit++)
        bits.push_back(TCut(Form("(cuts & (1 << %d))", bit)));

    TCut noSelection = "";
    TCut muon1 = bits[5] && bits[6] && bits[7] && bits[8] && bits[9];
    TCut muon2 = bits[10] && bits[11] && bits[12] && bits[13] && bits[14];
    TCut initialSelection = bits[1] && bits[2] && bits[3] && muon1;
    TCut nMinusDr = initialSelection && bits[4] && muon2 && bits[16];
    TCut nMinusMmumu = initialSelection && bits[4] && muon2 && bits[15];
    TCut nMinusNJets = initialSelection && muon2 && bits[15] && bits[16];
    TCut nMinusGoodMu = bits[1] && bits[2] && bits[3] && bits[4];

    //TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_QCD = initialSelection && "!cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_EW = initialSelection && "cutsVec[3] && !cutsVec[9] && !cutsVec[10] && !cutsVec[11] && !cutsVec[12] && !cutsVec[14]";
    //TCut controlRegion_TTbar = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && !cutsVec[14]";

    map<TString, cut_info> cuts {
        //{"no_selection", cut_info{noSelection, "No Selection"}},
        //{"initial_selection", cut_info{initialSelection, "Initial Selection"}},
        {"nMinusDr", cut_info{nMinusDr, "reco_vtx_dR", "Di-muon v_{xy} [cm]", 20, 0, 6, "N-1 (dR)",
                             vector<TString>{"5p25_10", "52p5_100", "60p0_1", "6p0_1000"}}},
        {"nMinusMmumu", cut_info{nMinusMmumu, "reco_Mmumu", "M_{#mu#mu} [GeV]", 50, 0, 50, "N-1 (M_{#mu#mu})",
                             vector<TString>{"5p25_10", "52p5_10", "6p0_10", "60p0_10"}}},
        {"nMinusNJets", cut_info{nMinusNJets, "reco_PF_n_highPt_jets", "Number of high pT (> 30 GeV) jets", 9, 1, 10, "N-1 (nJets)",
                             vector<TString>{"5p25_10", "52p5_10", "6p0_10", "60p0_10"}}},
        {"nMinusGoodMu", cut_info{nMinusGoodMu, "reco_n_good_dsa", "Number of quality dSA muons", 9, 1, 10, "N-1 (nGoodMuons)",
                             vector<TString>{"5p25_10", "52p5_100", "60p0_1", "6p0_1000"}}}
        //{"signal_region", cut_info{signalRegion, "Signal Region"}},
        //{"control_region_QCD", cut_info{controlRegion_QCD, "QCD Control Region"}},
        //{"control_region_EW", cut_info{controlRegion_EW, "EW Control Region"}},
        //{"control_region_TTbar", cut_info{controlRegion_TTbar, "TTbar Control Region"}}
    };

    map<TString, TCanvas *> canvases;

    for (auto cut : cuts) {
        for (auto & sample : cut.second.samples) {

            TFile * f = TFile::Open(samples[sample].filename.Data());
            TTree * recoT = (TTree*)f->Get("ntuples_gbm/recoT");
            recoT->UseCurrentStyle();

            recoT->Draw(Form("%s>>h%s_%s(%d,%f,%f)", cut.second.plot_var.Data(), cut.first.Data(), sample.Data(), cut.second.nbins, cut.second.lowX, cut.second.highX), cut.second.cut, "goff");
            hists[sample][cut.first] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut.first.Data(), sample.Data()));
        }
    }
    for (auto cut : cuts) {
        THStack * hs = new THStack(Form("%s", cut.first.Data()), Form("%s", cut.second.label.Data()));
        auto legend = new TLegend(0.45,0.7,0.85,0.9);
        legend->SetFillStyle(0);
        //auto legend = new TLegend(0.25,0.2);
        for (auto & sample : cut.second.samples) {
            hists[sample][cut.first]->Scale(1/hists[sample][cut.first]->Integral(), "width");
            hists[sample][cut.first]->SetLineColor(samples[sample].color);
            hists[sample][cut.first]->SetLineWidth(2);
            hs->Add(hists[sample][cut.first]);
            legend->AddEntry(hists[sample][cut.first], samples[sample].label.Data());
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
        hs->GetXaxis()->SetTitle(Form("%s", cut.second.plot_description.Data()));
        hs->GetYaxis()->SetTitle("A. U.");
        hs->GetYaxis()->SetTitleOffset(1.4);
        gPad->Modified();
        legend->Draw();
        text->Draw();
        if (fSave) {
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.pdf", cut.second.plot_var.Data(), cut.first.Data()));
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.png", cut.second.plot_var.Data(), cut.first.Data()));
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.root", cut.second.plot_var.Data(), cut.first.Data()));
        }
    }
}
