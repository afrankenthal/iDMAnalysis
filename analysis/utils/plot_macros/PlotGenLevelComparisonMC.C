#include "../json.hpp"
using json = nlohmann::json;
#include "../CMS_lumi.C"
using std::map, std::vector;

void PlotGenLevelComparisonMC(TString config_filename, bool fSave=0, bool fShow=1) {
    gROOT->LoadMacro("../tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");

    gROOT->SetBatch(!fShow);
    //std::vector<TString> observables = { "pt", "pt_zoom", "eta", "phi", "dr", "vxy" };

    map<TString, map<TString, TH1F*>> hists;
    map<TString, TCanvas*> canvases;
    map<TString, vector<float>> bins;
    map<TString, TString> vars;
    map<TString, TString> axis_label;
    map<TString, TString> plot_filenames;

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

    bins["gen_vxy"] = {};
    for (auto i = 0; i <= 5000; i += 1) bins["gen_vxy"].push_back(i);
    vars["gen_vxy"] = "gen.gen_vxy";
    axis_label["gen_vxy"] = "Muon pair vertex v_{xy} (gen) [cm]";
    plot_filenames["gen_vxy"] = "muon_gen_vxy.pdf";

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
        TH1F * frame = canvases[obs]->DrawFrame(0.1, 1e-6, bins[obs][bins[obs].size()-1], 1e1);
        frame->GetXaxis()->SetTitle(axis_label[obs].Data());
        frame->GetXaxis()->SetTitleOffset(1.2);
        canvases[obs]->SetLogx();
        frame->GetYaxis()->SetTitle("A. U.");
        canvases[obs]->SetLogy();
        canvases[obs]->Update();
        TLegend * legend = new TLegend(0.55, 0.55, 0.8, 0.9);
        legend->AddEntry((TObject*)0, config["canvas_label"].get<std::string>().c_str(), "");

        for (auto const & [sample, sample_cfg] : config["samples"].items()) {

            hists[obs][sample] = new TH1F(Form("h_%s_%s", obs.Data(), sample.c_str()), Form("h_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);

            for (auto filename : sample_cfg["files"].get<vector<std::string>>()) {

                TFile * file = TFile::Open(filename.c_str(), "READ");
                TTree * reco_tree = (TTree*)file->Get("ntuples_gbm/recoT");
                TTree * gen_tree = (TTree*)file->Get("ntuples_gbm/genT");
                reco_tree->AddFriend(gen_tree, "gen");

                TH1F * h_temp = (TH1F*)hists[obs][sample]->Clone();
                h_temp->Reset();
                reco_tree->Draw(Form("%s>>+h_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "gen.gen_ID==13", "goff");
                hists[obs][sample]->Add(h_temp);
            }

            hists[obs][sample]->SetLineColor(sample_cfg["color"]);
            hists[obs][sample]->SetMarkerColor(sample_cfg["color"]);
            hists[obs][sample]->DrawNormalized("E SAME");
            legend->AddEntry(hists[obs][sample], sample_cfg["legend"].get<std::string>().c_str(), "lep");
        }
        legend->Draw("SAME");

        //CMS_lumi( canvases[obs], 0, 0, "2018");

        gPad->RedrawAxis();

        if (fSave) {
            canvases[obs]->SaveAs(plot_filenames[obs].Data());
        }
    }

    gROOT->SetBatch(0);
}
