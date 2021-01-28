#include "../macros/utils/json.hpp"
using json = nlohmann::json;

void PlotMuonRecoEffi(TString config_filename, bool fSave=0, bool fShow=1) {
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");

    gROOT->SetBatch(!fShow);
    //std::vector<TString> observables = { "pt", "pt_zoom", "eta", "phi", "dr", "vxy" };

    std::map<TString, map<TString, TH1F*>> nums, denoms;
    std::map<TString, map<TString, TEfficiency*>> effs;
    std::map<TString, TCanvas*> canvases;
    std::map<TString, std::vector<float>> bins;
    std::map<TString, TString> vars;
    std::map<TString, TString> axis_label;
    std::map<TString, TString> plot_filenames;

    bins["pt"] = { 0, 2, 4, 6, 8, 10,
        15, 20, 25, 30, 40, 
        60, 80, 100 };
    vars["pt"] = "genPt";
    axis_label["pt"] = "Muon p_{T} [GeV]";
    plot_filenames["pt"] = Form("%s_reco_eff_vs_pt_2018.pdf", "muon"); // collection.Data());

    bins["pt_zoom"] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20};
    vars["pt_zoom"] = "genPt";
    axis_label["pt_zoom"] = "Muon p_{T} [GeV]";
    plot_filenames["pt_zoom"] = Form("%s_reco_eff_vs_zoompt_2018.pdf", "muon"); // collection.Data());

    bins["eta"] = {};
    for (auto i = -2.5; i <= 2.5; i += 0.2) bins["eta"].push_back(i);
    vars["eta"] = "genEta";
    axis_label["eta"] = "Muon #eta";
    plot_filenames["eta"] = Form("%s_reco_eff_vs_eta_2018.pdf", "muon"); //  collection.Data());

    bins["phi"] = {};
    for (auto i = -3.2; i <= 3.2; i += 0.2) bins["phi"].push_back(i);
    vars["phi"] = "genPhi";
    axis_label["phi"] = "Muon #phi";
    plot_filenames["phi"] = Form("%s_reco_eff_vs_phi_2018.pdf", "muon"); // collection.Data());

    bins["dr"] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.5, 4.0, 4.5,
        5.0, 6.0};
    vars["dr"] = "genDr";
    axis_label["dr"] = "Muon pair dR (gen)";
    plot_filenames["dr"] = Form("%s_reco_eff_vs_dr_2018.pdf", "muon"); // collection.Data());

    bins["vxy"] = { 0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500,
        550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
    vars["vxy"] = "genVxy";
    axis_label["vxy"] = "Muon pair vertex v_{xy} (gen) [cm]";
    plot_filenames["vxy"] = Form("%s_reco_eff_vs_vxy_2018.pdf", "muon"); //  collection.Data());
    
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
        frame->GetYaxis()->SetTitle("Efficiency");
        canvases[obs]->Update();
        TLegend * legend = new TLegend(0.55, 0.75, 0.8, 0.9);

        for (auto const & [sample, sample_cfg] : config["samples"].items()) {

            nums[obs][sample] = new TH1F(Form("hnum_%s_%s", obs.Data(), sample.c_str()), Form("hnum_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);
            denoms[obs][sample] = new TH1F(Form("hdenom_%s_%s", obs.Data(), sample.c_str()), Form("hdenom_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);

            for (auto filename : sample_cfg["files"].get<std::vector<std::string>>()) {

                TFile * file = TFile::Open(filename.c_str(), "READ");
                TTree * tree = (TTree*)file->Get(Form("RECO_%s/recoEffiForMuTrack", sample_cfg["collection"].get<std::string>().c_str()));

                TH1F * num_temp = (TH1F*)nums[obs][sample]->Clone();
                num_temp->Reset();
                tree->Draw(Form("%s[0]>>+hnum_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched>0", "goff");
                tree->Draw(Form("%s[1]>>+hnum_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched>1", "goff");
                nums[obs][sample]->Add(num_temp);
                TH1F * denom_temp = (TH1F*)denoms[obs][sample]->Clone();
                denom_temp->Reset();
                tree->Draw(Form("%s>>+hdenom_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "", "goff");
                denoms[obs][sample]->Add(denom_temp);
            }

            effs[obs][sample] = new TEfficiency(*nums[obs][sample], *denoms[obs][sample]);
            effs[obs][sample]->SetName(sample_cfg["legend"].get<std::string>().c_str());
            effs[obs][sample]->SetMarkerColor(sample_cfg["color"].get<int>());
            effs[obs][sample]->SetLineColor(sample_cfg["color"].get<int>());
            effs[obs][sample]->Draw("E SAME");
            legend->AddEntry(effs[obs][sample], sample_cfg["legend"].get<std::string>().c_str(), "lep");
        }
        legend->Draw("SAME");

        if (fSave) {
            canvases[obs]->SaveAs(plot_filenames[obs].Data());
        }
    }

    gROOT->SetBatch(0);
}
