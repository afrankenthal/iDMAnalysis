#include "ScaleFactors.h"

vector<ScaleFactors::bTagSF_t> ScaleFactors::buildBTagSF(rapidcsv::Document btag_file, ScaleFactors::SYST syst) {
    std::string btag_syst = " central";
    if (syst == ScaleFactors::BTAG_DOWN)
        btag_syst = " down";
    else if (syst == ScaleFactors::BTAG_UP)
        btag_syst = " up";

    vector<ScaleFactors::bTagSF_t> bTagSF;
    for (size_t i = 0; i < btag_file.GetRowCount(); i++) {
        int OperatingPoint = btag_file.GetCell<int>(0, i);
        std::string measurementType = btag_file.GetCell<std::string>(1, i);
        std::string sysType = btag_file.GetCell<std::string>(2, i);
        int jetFlavor = btag_file.GetCell<int>(3, i);
        float etaMin = btag_file.GetCell<float>(4, i);
        float etaMax = btag_file.GetCell<float>(5, i);
        float ptMin = btag_file.GetCell<float>(6, i);
        float ptMax = btag_file.GetCell<float>(7, i);
        int discrMin = btag_file.GetCell<int>(8, i);
        int discrMax = btag_file.GetCell<int>(9, i);
        TString formula = TString(btag_file.GetCell<std::string>(10, i));
        formula.ReplaceAll(" ", "").ReplaceAll("\"", "");
        // TODO: add "up" and "down" for systematics
        if (OperatingPoint == 1 && measurementType == " comb" && sysType == btag_syst.c_str() && jetFlavor == 0) {
            std::unique_ptr<TFormula> f = std::make_unique<TFormula>("f", formula.Data());
            bTagSF.push_back({ptMin, ptMax, std::move(f)});
        }
    }
    return bTagSF;
}

std::unique_ptr<TH1> ScaleFactors::buildSF(TString filename, TString histoname) {
    auto file = TFile::Open(filename);
    auto * hist = (TH1*)(((TH1*)file->Get(histoname))->Clone());
    std::unique_ptr<TH1> histo(hist);
    histo->SetDirectory(0);
    file->Close();
    return std::move(histo);
}

bool ScaleFactors::buildScaleFactors(ScaleFactors::SYST syst /*= ScaleFactors::NOMINAL*/) {

    const char* data_path_c = std::getenv("ANALYSIS_DATA_PATH");
    if (!data_path_c) {
        cout << "ERROR! Could not find environment variable $ANALYSIS_DATA_PATH.";
        cout << " Make sure you have sourced setup.sh before proceeding." << endl;
        return false;
    }
    TString data_path(data_path_c);

    // bTag SF (need CSV reader)
    rapidcsv::Document btag_file_2016(std::string(data_path + "/bTagSF_2016.csv"));
    btag_sfs[2016] = buildBTagSF(btag_file_2016, syst);
    rapidcsv::Document btag_file_2017(std::string(data_path + "/bTagSF_2017.csv"));
    btag_sfs[2017] = buildBTagSF(btag_file_2017, syst);
    rapidcsv::Document btag_file_2018(std::string(data_path + "/bTagSF_2018.csv"));
    btag_sfs[2018] = buildBTagSF(btag_file_2018, syst);

    // GM low pT SF (hard-coded)
    // eta_min, eta_max, central, down, up
    gmlowpt_sfs[2016] = {
        gmLowPtSF_t{-2.4, -1.8, 1.003, 0.025, 0.013},
        gmLowPtSF_t{-1.8, -1.2, 0.983, 0.010, 0.008},
        gmLowPtSF_t{-1.2, -0.8, 0.998, 0.006, 0.008},
        gmLowPtSF_t{-0.8, -0.3, 1.003, 0.004, 0.005},
        gmLowPtSF_t{-0.3, +0.3, 0.999, 0.004, 0.004},
        gmLowPtSF_t{+0.3, +0.8, 1.005, 0.004, 0.004},
        gmLowPtSF_t{+0.8, +1.2, 0.996, 0.007, 0.006},
        gmLowPtSF_t{+1.8, +1.8, 1.001, 0.008, 0.008},
        gmLowPtSF_t{+1.8, +2.4, 0.954, 0.049, 0.029}
    };
    gmlowpt_sfs[2017] = {
        gmLowPtSF_t{-2.4, -1.8, 0.986, 0.006, 0.007},
        gmLowPtSF_t{-1.8, -1.2, 0.997, 0.002, 0.003},
        gmLowPtSF_t{-1.2, -0.8, 0.994, 0.003, 0.004},
        gmLowPtSF_t{-0.8, -0.3, 0.995, 0.002, 0.002},
        gmLowPtSF_t{-0.3, +0.3, 0.996, 0.002, 0.002},
        gmLowPtSF_t{+0.3, +0.8, 0.997, 0.002, 0.002},
        gmLowPtSF_t{+0.8, +1.2, 0.995, 0.003, 0.003},
        gmLowPtSF_t{+1.8, +1.8, 0.997, 0.002, 0.003},
        gmLowPtSF_t{+1.8, +2.4, 0.996, 0.005, 0.007}
    };
    gmlowpt_sfs[2018] = {
        gmLowPtSF_t{-2.4, -1.6, 0.986, 0.013, 0.014},
        gmLowPtSF_t{-1.6, -0.9, 0.991, 0.006, 0.005},
        gmLowPtSF_t{-0.9, -0.3, 0.996, 0.004, 0.004},
        gmLowPtSF_t{-0.3, +0.3, 0.995, 0.005, 0.004},
        gmLowPtSF_t{+0.3, +0.9, 0.997, 0.005, 0.004},
        gmLowPtSF_t{+0.9, +1.6, 0.985, 0.007, 0.007},
        gmLowPtSF_t{+1.6, +2.4, 0.996, 0.013, 0.014}
    };

    // all other SFs
    
    // Pileup SF
    all_sfs["pileup"][2016] = buildSF(data_path + TString("/pileup/PUWeights_2016.root"), "puWeights");
    all_sfs["pileup"][2017] = buildSF(data_path + TString("/pileup/puWeights_90x_41ifb.root"), "puWeights"); // /pileup/puWeights_2017.root
    all_sfs["pileup"][2018] = buildSF(data_path + TString("/pileup/PUWeights_2018.root"), "puWeights"); // /pileup/puWeights_10x_56ifb.root

    // all_sfs["pileup_ZJets_HT-100To200"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio100.root"), "PUwgt_cut1_data_yr2017");
    // all_sfs["pileup_ZJets_HT-200To400"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio200.root"), "PUwgt_cut1_data_yr2017");
    // ////// FIXME FIXME FIXME zjetratio400.root does not exist!
    // all_sfs["pileup_ZJets_HT-400To600"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio200.root"), "PUwgt_cut1_data_yr2017");
    // all_sfs["pileup_ZJets_HT-600To800"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio600.root"), "PUwgt_cut1_data_yr2017");
    // all_sfs["pileup_ZJets_HT-800To1200"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio800.root"), "PUwgt_cut1_data_yr2017");
    // all_sfs["pileup_ZJets_HT-1200To2500"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio1200.root"), "PUwgt_cut1_data_yr2017");
    // all_sfs["pileup_ZJets_HT-2500ToInf"][2017] = buildSF(data_path + TString("/zjetpileup/zjetratio2500.root"), "PUwgt_cut1_data_yr2017");

    // QCD and EWK SF
    // Do we need to enable SetDirectory(0) for these? keep in mind if errors
    auto z_qcd = buildSF(data_path + TString("/kfactors.root"), "ZJets_012j_NLO/nominal");
    auto z_ewk = buildSF(data_path + TString("/kfactors.root"), "EWKcorr/Z");
    auto z_lo = buildSF(data_path + TString("/kfactors.root"), "ZJets_LO/inv_pt");
    auto * temp = ((TH1F*)z_qcd->Clone());
    temp->Divide(z_lo.get());
    all_sfs["z_qcd"][-1] = std::make_unique<TH1F>(*temp);
    temp = ((TH1F*)z_ewk->Clone());
    temp->Divide(z_lo.get());
    all_sfs["z_ewk"][-1] = std::make_unique<TH1F>(*temp);
    auto w_qcd = buildSF(data_path + TString("/kfactors.root"), "WJets_012j_NLO/nominal");
    auto w_ewk = buildSF(data_path + TString("/kfactors.root"), "EWKcorr/W");
    auto w_lo = buildSF(data_path + TString("/kfactors.root"), "WJets_LO/inv_pt");
    temp = ((TH1F*)w_qcd->Clone());
    temp->Divide(w_lo.get());
    all_sfs["w_qcd"][-1] = std::make_unique<TH1F>(*temp);
    temp = ((TH1F*)w_ewk->Clone());
    temp->Divide(w_lo.get());
    all_sfs["w_ewk"][-1] = std::make_unique<TH1F>(*temp);

    // Electron veto SF
    // May be wrong since it's a TH2F but let's see
    all_sfs["veto_e"][2016] = buildSF(data_path + TString("/ElectronWPVeto_80X_2016.root"), "EGamma_SF2D");
    all_sfs["veto_e"][2017] = buildSF(data_path + TString("/ElectronWPVeto_Fall17V2_2017.root"), "EGamma_SF2D");
    all_sfs["veto_e"][2018] = buildSF(data_path + TString("/ElectronWPVeto_Fall17V2_2018.root"), "EGamma_SF2D");

    // Photon veto SF
    // May be wrong since it's a TH2F but let's see
    all_sfs["veto_ph"][2016] = buildSF(data_path + TString("/PhotonsLoose_2016_Fall17V2.root"), "EGamma_SF2D");
    all_sfs["veto_ph"][2017] = buildSF(data_path + TString("/PhotonsLoose_2017.root"), "EGamma_SF2D");
    all_sfs["veto_ph"][2018] = buildSF(data_path + TString("/PhotonsLoose_2018.root"), "EGamma_SF2D");

    // Global muon SF from Z
    all_sfs["gm_ID_16BF"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys.root"), "NUM_LooseID_DEN_genTracks_eta_pt");
    all_sfs["gm_ID_16BF"][2016]->Scale(20.0/35.92);
    all_sfs["gm_ID_16GH"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys_GH.root"), "NUM_LooseID_DEN_genTracks_eta_pt");
    all_sfs["gm_ID_16GH"][2016]->Scale(15.92/35.92);
    TH2F * temp2 = (TH2F*)all_sfs["gm_ID_16BF"][2016]->Clone();
    temp2->Add(all_sfs["gm_ID_16GH"][2016].get());
    all_sfs["gm_ID"][2016] = std::make_unique<TH2F>(*temp2);
    all_sfs["gm_ID"][2017] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2017sys.root"), "NUM_LooseID_DEN_genTracks_pt_abseta");
    all_sfs["gm_ID"][2018] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2018ID.root"), "NUM_LooseID_DEN_TrackerMuons_pt_abseta"); // TrackerMuons in 2018
    all_sfs["gm_ID_stat_16BF"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys.root"), "NUM_LooseID_DEN_genTracks_eta_pt_stat");
    all_sfs["gm_ID_stat_16BF"][2016]->Scale(20.0/35.92);
    all_sfs["gm_ID_stat_16GH"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys_GH.root"), "NUM_LooseID_DEN_genTracks_eta_pt_stat");
    all_sfs["gm_ID_stat_16GH"][2016]->Scale(15.92/35.92);
    temp2 = (TH2F*)all_sfs["gm_ID_stat_16BF"][2016]->Clone();
    temp2->Add(all_sfs["gm_ID_stat_16GH"][2016].get());
    all_sfs["gm_ID_stat"][2016] = std::make_unique<TH2F>(*temp2);
    all_sfs["gm_ID_stat"][2017] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2017sys.root"), "NUM_LooseID_DEN_genTracks_pt_abseta_stat");
    all_sfs["gm_ID_stat"][2018] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2018ID.root"), "NUM_LooseID_DEN_TrackerMuons_pt_abseta_stat"); // TrackerMuons in 2018
    all_sfs["gm_ID_syst_16BF"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys.root"), "NUM_LooseID_DEN_genTracks_eta_pt_syst");
    all_sfs["gm_ID_syst_16BF"][2016]->Scale(20.0/35.92);
    all_sfs["gm_ID_syst_16GH"][2016] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2016sys_GH.root"), "NUM_LooseID_DEN_genTracks_eta_pt_syst");
    all_sfs["gm_ID_syst_16GH"][2016]->Scale(15.92/35.92);
    temp2 = (TH2F*)all_sfs["gm_ID_syst_16BF"][2016]->Clone();
    temp2->Add(all_sfs["gm_ID_syst_16GH"][2016].get());
    all_sfs["gm_ID_syst"][2016] = std::make_unique<TH2F>(*temp2);
    all_sfs["gm_ID_syst"][2017] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2017sys.root"), "NUM_LooseID_DEN_genTracks_pt_abseta_syst");
    all_sfs["gm_ID_syst"][2018] = buildSF(data_path + TString("/idm_gm_scalefactors/GM_SF_2018ID.root"), "NUM_LooseID_DEN_TrackerMuons_pt_abseta_syst"); // TrackerMuons in 2018

    // dSA muon ID SF from Z
    auto dsa_filename = data_path + TString("/idm_dsa_scalefactors/high_pt/NUM_DisplacedID_DEN_dSAMuons_abseta_pt_");

    all_sfs["dsa_ID_Z"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");
    all_sfs["dsa_ID_Z"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");
    all_sfs["dsa_ID_Z"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");

    all_sfs["dsa_ID_Z_stat"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");
    all_sfs["dsa_ID_Z_stat"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");
    all_sfs["dsa_ID_Z_stat"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");

    all_sfs["dsa_ID_Z_syst"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");
    all_sfs["dsa_ID_Z_syst"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");
    all_sfs["dsa_ID_Z_syst"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");
    
    // dSA muon ID SF from JPsi
    dsa_filename = data_path + TString("/idm_dsa_scalefactors/low_pt/NUM_DisplacedID_DEN_dSAMuons_abseta_pt_");

    all_sfs["dsa_ID_JPsi"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");
    all_sfs["dsa_ID_JPsi"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");
    all_sfs["dsa_ID_JPsi"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt");

    all_sfs["dsa_ID_JPsi_stat"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");
    all_sfs["dsa_ID_JPsi_stat"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");
    all_sfs["dsa_ID_JPsi_stat"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_stat");

    all_sfs["dsa_ID_JPsi_syst"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");
    all_sfs["dsa_ID_JPsi_syst"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");
    all_sfs["dsa_ID_JPsi_syst"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_DisplacedID_DEN_dSAMuons_abseta_pt_syst");

    // dSA muon reco SF from Z
    dsa_filename = data_path + TString("/idm_dsa_scalefactors/high_pt/NUM_dSAMuons_DEN_genTracks_abseta_pt_");

    all_sfs["dsa_reco_Z"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");
    all_sfs["dsa_reco_Z"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");
    all_sfs["dsa_reco_Z"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");

    all_sfs["dsa_reco_Z_stat"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");
    all_sfs["dsa_reco_Z_stat"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");
    all_sfs["dsa_reco_Z_stat"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");

    all_sfs["dsa_reco_Z_stat"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");
    all_sfs["dsa_reco_Z_stat"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");
    all_sfs["dsa_reco_Z_stat"][2018]= buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");

    // dSA muon reco SF from JPsi
    dsa_filename = data_path + TString("/idm_dsa_scalefactors/low_pt/NUM_dSAMuons_DEN_genTracks_abseta_pt_");

    all_sfs["dsa_reco_JPsi"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");
    all_sfs["dsa_reco_JPsi"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");
    all_sfs["dsa_reco_JPsi"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt");

    all_sfs["dsa_reco_JPsi_stat"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");
    all_sfs["dsa_reco_JPsi_stat"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");
    all_sfs["dsa_reco_JPsi_stat"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_stat");
    
    all_sfs["dsa_reco_JPsi_syst"][2016] = buildSF(dsa_filename + TString("2016.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");
    all_sfs["dsa_reco_JPsi_syst"][2017] = buildSF(dsa_filename + TString("2017.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");
    all_sfs["dsa_reco_JPsi_syst"][2018] = buildSF(dsa_filename + TString("2018.root"), "NUM_dSAMuons_DEN_genTracks_abseta_pt_syst");

    // Trigger SF
    all_sfs["trigger"][2016] = buildSF(data_path + TString("/trig_weights/trig_weights_2016.root"), "weights_reco_PF_MET_pt_2016_MCTotal_2016_ratio");
    all_sfs["trigger"][2017] = buildSF(data_path + TString("/trig_weights/trig_weights_2017.root"), "weights_reco_PF_MET_pt_2017_MCTotal_2017_ratio");
    all_sfs["trigger"][2018] = buildSF(data_path + TString("/trig_weights/trig_weights_2018.root"), "weights_reco_PF_MET_pt_2018_MCTotal_2018_ratio");

    return true;
}