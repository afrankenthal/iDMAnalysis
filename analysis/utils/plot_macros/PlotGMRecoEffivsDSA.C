#include "../utils/json.hpp"
using json = nlohmann::json;

void PlotGMRecoEffivsDSA(TString config_filename, int year=2018, bool fSave=0, bool fShow=1) {
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");

    gROOT->SetBatch(!fShow);
    //std::vector<TString> observables = { "pt", "pt_zoom", "eta", "phi", "dr", "vxy" };

    std::map<TString, map<TString, map<int, TH1F*>>> nums;
    std::map<TString, map<TString, TH1F*>> denoms;
    std::map<TString, map<TString, map<int, TEfficiency*>>> effs;
    std::map<TString, TCanvas*> canvases;
    std::map<TString, std::vector<float>> bins;
    std::map<TString, TString> vars;
    std::map<TString, TString> axis_label;
    std::map<TString, TString> plot_filenames;

    bins["pt"] = { 0, 2, 4, 6, 8, 10,
        15, 20, 25, 30, 50, 70 };
    vars["pt"] = "genPt";
    axis_label["pt"] = "Muon p_{T} [GeV]";
    plot_filenames["pt"] = Form("%s_reco_eff_vs_pt_2018.pdf", "muon"); // collection.Data());

    bins["pt_zoom"] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20};
    vars["pt_zoom"] = "genPt";
    axis_label["pt_zoom"] = "Muon p_{T} [GeV]";
    plot_filenames["pt_zoom"] = Form("%s_reco_eff_vs_zoompt_2018.pdf", "muon"); // collection.Data());

    bins["eta"] = {};
    for (auto i = -2.5; i <= 2.7; i += 0.2) bins["eta"].push_back(i);
    vars["eta"] = "genEta";
    axis_label["eta"] = "Muon #eta";
    plot_filenames["eta"] = Form("%s_reco_eff_vs_eta_2018.pdf", "muon"); //  collection.Data());

    bins["phi"] = {};
    for (auto i = -3.2; i <= 3.4; i += 0.2) bins["phi"].push_back(i);
    vars["phi"] = "genPhi";
    axis_label["phi"] = "Muon #phi";
    plot_filenames["phi"] = Form("%s_reco_eff_vs_phi_2018.pdf", "muon"); // collection.Data());

    bins["dr"] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.5, 4.0, 4.5,
        5.0, 6.0};
    vars["dr"] = "genDr";
    axis_label["dr"] = "Muon pair dR (gen)";
    plot_filenames["dr"] = Form("%s_reco_eff_vs_dr_2018.pdf", "muon"); // collection.Data());

    bins["vxy"] = { 0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350 };
    vars["vxy"] = "genVxy";
    axis_label["vxy"] = "Muon pair vertex v_{xy} (gen) [cm]";
    plot_filenames["vxy"] = Form("%s_reco_eff_vs_vxy_2018.pdf", "muon"); //  collection.Data());

    bins["vz"] = { 0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350 };
    vars["vz"] = "genVxy";
    axis_label["vz"] = "Muon pair vertex v_{z} (gen) [cm]";
    plot_filenames["vz"] = Form("%s_reco_eff_vs_vz_2018.pdf", "muon"); //  collection.Data());

    bins["vxy_zoom"] = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50,
        55, 60, 65, 70, 75, 80, 85, 90, 95, 100 } ;
    vars["vxy_zoom"] = "genVxy";
    axis_label["vxy_zoom"] = "Muon pair vertex v_{xy} (gen) [cm]";
    plot_filenames["vxy_zoom"] = Form("%s_reco_eff_vs_vxy_zoom_2018.pdf", "muon"); //  collection.Data());
    
    std::ifstream config_file(config_filename.Data());
    json config;
    config_file >> config;

    for (auto const & [obs_cppstr, obs_cfg] : config["observables"].items()) {
        TString obs(obs_cppstr.c_str());

        float bins_temp[bins[obs].size()];
        std::copy(bins[obs].begin(), bins[obs].end(), bins_temp);

        canvases[obs] = new TCanvas();
        TH1F * frame = canvases[obs]->DrawFrame(bins[obs][0], 0, bins[obs][bins[obs].size()-1], 1);
        frame->GetXaxis()->SetTitle(axis_label[obs].Data());
        frame->GetYaxis()->SetTitle("Fraction");
        canvases[obs]->Update();
        TLegend * legend = new TLegend(0.55, 0.5, 0.8, 0.65);

        for (auto const & [sample, sample_cfg] : config["samples"].items()) {

            nums[obs][sample][0] = new TH1F(Form("hnum0GM_%s_%s", obs.Data(), sample.c_str()), Form("hnum0GM_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);
            nums[obs][sample][1] = new TH1F(Form("hnum1GM_%s_%s", obs.Data(), sample.c_str()), Form("hnum1GM_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);
            nums[obs][sample][2] = new TH1F(Form("hnum2GM_%s_%s", obs.Data(), sample.c_str()), Form("hnum2GM_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);
            denoms[obs][sample] = new TH1F(Form("hdenom_%s_%s", obs.Data(), sample.c_str()), Form("hdenom_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);

            for (auto filename : sample_cfg["files"].get<std::vector<std::string>>()) {

                TFile * file = TFile::Open(filename.c_str(), "READ");
                TTree * dsa_tree = (TTree*)file->Get(Form("RECO_%s/recoEffiForMuTrack", "dsa"));
                TTree * gm_tree = (TTree*)file->Get(Form("RECO_%s/recoEffiForMuTrack", "gbm"));
                dsa_tree->AddFriend(gm_tree, "gm");

                TH1F * num_temp = (TH1F*)nums[obs][sample][0]->Clone();
                num_temp->Reset();
                dsa_tree->Draw(Form("%s[0]>>+hnum0GM_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched==2 && gm.nMatched==0 && abs(genEta) > 1.2", "goff");
                nums[obs][sample][0]->Add(num_temp);
                num_temp = (TH1F*)nums[obs][sample][1]->Clone();
                num_temp->Reset();
                dsa_tree->Draw(Form("%s[0]>>+hnum1GM_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched==2 && gm.nMatched==1 && abs(genEta) > 1.2", "goff");
                nums[obs][sample][1]->Add(num_temp);
                num_temp = (TH1F*)nums[obs][sample][2]->Clone();
                num_temp->Reset();
                dsa_tree->Draw(Form("%s[0]>>+hnum2GM_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched==2 && gm.nMatched==2 && abs(genEta) > 1.2", "goff");
                nums[obs][sample][2]->Add(num_temp);
                TH1F * denom_temp = (TH1F*)denoms[obs][sample]->Clone();
                denom_temp->Reset();
                dsa_tree->Draw(Form("%s[0]>>+hdenom_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched==2 && abs(genEta) > 1.2", "goff");
                denoms[obs][sample]->Add(denom_temp);
            }

            effs[obs][sample][0] = new TEfficiency(*nums[obs][sample][0], *denoms[obs][sample]);
            effs[obs][sample][0]->SetName("0GM");
            effs[obs][sample][0]->SetMarkerColor(kBlack);
            effs[obs][sample][0]->SetLineColor(kBlack);
            effs[obs][sample][0]->Draw("E SAME");
            legend->AddEntry(effs[obs][sample][0], "0 GM reco'd", "lep");

            effs[obs][sample][1] = new TEfficiency(*nums[obs][sample][1], *denoms[obs][sample]);
            effs[obs][sample][1]->SetName("1GM");
            effs[obs][sample][1]->SetMarkerColor(kBlue);
            effs[obs][sample][1]->SetLineColor(kBlue);
            effs[obs][sample][1]->Draw("E SAME");
            legend->AddEntry(effs[obs][sample][1], "1 GM reco'd", "lep");

            effs[obs][sample][2] = new TEfficiency(*nums[obs][sample][2], *denoms[obs][sample]);
            effs[obs][sample][2]->SetName("2GM");
            effs[obs][sample][2]->SetMarkerColor(kRed);
            effs[obs][sample][2]->SetLineColor(kRed);
            effs[obs][sample][2]->Draw("E SAME");
            legend->AddEntry(effs[obs][sample][2], "2 GM reco'd", "lep");
        }
        legend->Draw("SAME");

        TLatex latex;
        latex.SetTextSize(0.04);
        latex.DrawLatexNDC(0.25, 0.9, Form("%d", year));

        if (fSave) {
            canvases[obs]->SaveAs(plot_filenames[obs].Data());
        }
    }

    gROOT->SetBatch(0);
}
