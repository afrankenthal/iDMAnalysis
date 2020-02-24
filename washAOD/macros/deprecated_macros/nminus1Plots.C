#include "utils/tdrstyle.C"
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/common.C"
using namespace common;

using std::cout, std::endl, std::map, std::vector;

void nminus1Plots(bool fSave=false, bool test=true) {
    TDatime time_begin;

    vector<int> colors{kBlack, kRed, kMagenta, kBlue,
        kCyan, kGreen, kYellow+1};

    map<TString, TString> group_legend{
        {"DY", "Z(ll)+Jets"},
        {"QCD", "QCD"},
        {"Diboson", "Di-boson"},
        {"ZJets", "Z(#nu#nu)+Jets"},
        {"WJets", "W(l#nu)+Jets"},
        {"TTbar", "t#bar{t}"}
    };
    
    map<TString, SampleInfo> samples;

    TString config_file;
    if (test)
        config_file = TString("configs/backgrounds_subset.json");
    else
        config_file = TString("configs/backgrounds_full.json");

    std::ifstream bkgs_file(config_file.Data());
    json bkgs_cfg;
    bkgs_file >> bkgs_cfg;
    int color = 1;
    for (auto const & [bkg, cfg] : bkgs_cfg.items()) {
        samples[TString(bkg)] = SampleInfo{
            listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames
            bkg, // plot label
            cfg["xsec"].get<float>(), // xsec
            cfg["sum_gen_wgt"], // sum_gen_wgt
            cfg["limit_num_files"], // limit_num_files
            1, // weight
            TString(cfg["group"].get<std::string>()), // plot group
            color++, // line color
            1 // line style
        };
    }

    //gROOT->LoadMacro("tdrstyle.C");
    setTDRStyle();

    map<TString, SignalInfo> mchis{
        {"5p25", SignalInfo{"5", "0p5", "0.1", kYellow}},
        {"52p5", SignalInfo{"50", "5p0", "0.1", kBlue}},
        {"6p0", SignalInfo{"5", "2p0", "0.4", kGreen}},
        {"60p0", SignalInfo{"50", "20p0", "0.4", kRed}}
    };
    vector<TString> ctaus{"1", "10", "100", "1000"};
    for (auto ctau : ctaus) 
        for (auto const & [mchi, cfg] : mchis) 
            samples[Form("%s_%s", mchi.Data(), ctau.Data())] = SampleInfo{
                vector<TString>{Form("../Mchi-%s_dMchi-%s_ctau-%s.root", mchi.Data(), cfg.dmchi.Data(), ctau.Data())}, // list of filenames
                Form("m1 = %s GeV, #Delta = %s, c#tau = %s mm", cfg.m1.Data(), cfg.delta.Data(), ctau.Data()), // plot label
                1, // xsec
                0.0, // sum_gen_wgt
                -1, // limit_num_files
                1, // weight
                "", // plot group
                cfg.color + 1*(int)(distance(ctaus.begin(), find(ctaus.begin(), ctaus.end(), ctau))), // line color
                2 // line style
            };

    for (auto const & [sample, props] : samples) {
        cout << sample << endl;
        if (!props.filenames.empty())
            cout << props.filenames.at(0) << endl;
    }

    map<TString, map<TString, TH1F *>> hists;
    int numCuts_ = 19;

    typedef struct CutInfo {
        TCut tcut;
        TString plot_var;
        TString plot_description;
        Int_t nbins;
        Float_t lowX;
        Float_t highX;
        TString label;
        vector<TString> sample_list;
    } CutInfo;

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
    TCut nMinusDPhiJetMet = bits[1] && bits[2] && bits[3]; // noSelection; // initialSelection; // && bits[4] && muon2 && bits[15] && bits[16];

    //TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_QCD = initialSelection && "!cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_EW = initialSelection && "cutsVec[3] && !cutsVec[9] && !cutsVec[10] && !cutsVec[11] && !cutsVec[12] && !cutsVec[14]";
    //TCut controlRegion_TTbar = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && !cutsVec[14]";

    vector<TString> groups_to_plot{"5p25_100", "60p0_1", "Diboson", "QCD", "DY", "TTbar", "ZJets", "WJets"};

    map<TString, CutInfo> cuts{};
    cuts["nMinusDr"] = CutInfo{nMinusDr, "reco_vtx_dR", "Di-muon dR", 10, 0, 6, "N-1 (dR)", groups_to_plot};
    cuts["nMinusMmumu"] = CutInfo{nMinusMmumu, "reco_Mmumu", "M_{#mu#mu} [GeV]", 60, 0, 120, "N-1 (M_{#mu#mu})", groups_to_plot};
    cuts["nMinusNJets"] = CutInfo{nMinusNJets, "reco_PF_n_highPt_jets", "Number of high pT (> 30 GeV) jets", 9, 1, 10, "N-1 (nJets)", groups_to_plot};
    cuts["nMinusGoodMu"] = CutInfo{nMinusGoodMu, "reco_n_good_dsa", "Number of quality dSA muons", 9, 1, 10, "N-1 (nGoodMuons)", groups_to_plot};
    cuts["nMinusDPhiJetMet"] = CutInfo{nMinusDPhiJetMet, "reco_PF_METjet_dphi", "#Delta#Phi(leading jet, MET)", 20, -3.2, 3.2, "N-1 (dPhiJetMet)", groups_to_plot};

    map<TString, TCanvas *> canvases;

    for (auto const & [sample, props] : samples) {
        int count = 0;
        TChain * data_gen = new TChain("ntuples_gbm/genT");
        TChain * data_reco = new TChain("ntuples_gbm/recoT");
        for (auto const & filename : props.filenames) {
            if (count++ >= props.limit_num_files && props.limit_num_files != -1) continue;
            data_gen->Add(filename);
            data_reco->Add(filename);
        }
        Float_t gen_wgt;
        data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
        data_gen->GetEntries();
        data_reco->GetEntries();
        data_gen->UseCurrentStyle();
        data_reco->UseCurrentStyle();
        data_reco->AddFriend(data_gen, "ntuples_gbm/genT");

        cout << "sample: " << sample << ", sum_gen_wgt: " << props.sum_gen_wgt << endl;

        float lumi = 59.97 * 1000; // 1/pb

        for (auto const & [cut_name, cut] : cuts) {
            if (std::find(cut.sample_list.begin(), cut.sample_list.end(), sample) != cut.sample_list.end()) {
                if (hists[sample].find(cut_name) == hists[sample].end())
                    hists[sample][cut_name] = new TH1F(Form("h%s_%s", cut_name.Data(), sample.Data()), "", cut.nbins, cut.lowX, cut.highX);
                data_reco->Draw(Form("%s>>+h%s_%s", cut.plot_var.Data(), cut_name.Data(), sample.Data()), cut.tcut, "goff");
                hists[sample][cut_name] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut_name.Data(), sample.Data()));
            }
            else if (std::find(cut.sample_list.begin(), cut.sample_list.end(), props.group) != cut.sample_list.end()) {
                if (hists[props.group].find(cut_name) == hists[props.group].end())
                    hists[props.group][cut_name] = new TH1F(Form("h%s_%s", cut_name.Data(), props.group.Data()), "", cut.nbins, cut.lowX, cut.highX);
                data_reco->Draw(Form("%s>>+h%s_%s", cut.plot_var.Data(), cut_name.Data(), props.group.Data()), cut.tcut * Form("gen_wgt * %f * %f / %f", lumi, props.xsec, props.sum_gen_wgt), "goff");
            }
        }
    }

    for (auto const & [cut_name, cut] : cuts) {
        THStack * hs = new THStack(Form("%s", cut_name.Data()), Form("%s", cut.label.Data()));
        auto legend = new TLegend(0.45,0.7,0.85,0.9);
        legend->SetFillStyle(0);
        //auto legend = new TLegend(0.25,0.2);
        color = 1;
        for (auto & sample : cut.sample_list) {
            if (hists[sample][cut_name]->Integral() < 0.00001) continue;
            hists[sample][cut_name]->Scale(1/hists[sample][cut_name]->Integral(), "width");
            if (samples.find(sample) != samples.end()) {
                hists[sample][cut_name]->SetLineColor(samples[sample].color);
                hists[sample][cut_name]->SetLineStyle(samples[sample].style);
            }
            else {
                hists[sample][cut_name]->SetLineColor(colors[color++]);
            }
            hists[sample][cut_name]->SetLineWidth(2);
            hs->Add(hists[sample][cut_name]);
            if (samples.find(sample) != samples.end())
                legend->AddEntry(hists[sample][cut_name], samples[sample].label.Data());
            else // group instead
                legend->AddEntry(hists[sample][cut_name], group_legend[sample]);
        }

        if (hs->GetNhists() == 0) continue;
        
        canvases[cut_name] = new TCanvas();
        canvases[cut_name]->cd();
        hs->Draw("E HIST nostack");
        TPaveText *text = new TPaveText(0.25,0.8,0.4,1.0, "NDC");
        text->AddText(Form("%s", cut.label.Data()));
        text->SetBorderSize(0);
        text->SetFillColor(0);
        text->SetTextFont(53); 
        text->SetTextSize(20);
        text->SetFillStyle(0);
        hs->GetXaxis()->SetTitle(Form("%s", cut.plot_description.Data()));
        hs->GetYaxis()->SetTitle("A. U.");
        hs->GetYaxis()->SetTitleOffset(1.4);
        gPad->Modified();
        legend->Draw();
        text->Draw();
        if (fSave) {
            canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.pdf", cut.plot_var.Data(), cut_name.Data()));
            canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.png", cut.plot_var.Data(), cut_name.Data()));
            canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.root", cut.plot_var.Data(), cut_name.Data()));
        }
    }

    printTimeElapsed(time_begin);
}
