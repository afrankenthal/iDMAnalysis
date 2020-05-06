#include <math.h>

#include <ROOT/RVec.hxx>
using namespace ROOT::VecOps;
#include <TFile.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>

#include "RDFAnalysis.h"

using std::map, std::vector, std::cout, std::endl;

void RDFAnalysis::Begin() {

    TFile * pileup_file;
    
    //pileup_file = TFile::Open("../../data/puWeights_80x_37ifb.root");
    pileup_file = TFile::Open("../../data/pileup/PUWeights_2016.root");
    pileup_2016 = (TH1F*)(((TH1F*)pileup_file->Get("puWeights"))->Clone());
    pileup_2016->SetDirectory(0);
    pileup_file->Close();

    pileup_file = TFile::Open("../../data/puWeights_90x_41ifb.root");
    //pileup_file = TFile::Open("../../data/pileup/PUWeights_2017.root");
    pileup_2017 = (TH1F*)(((TH1F*)pileup_file->Get("puWeights"))->Clone());
    pileup_2017->SetDirectory(0);
    pileup_file->Close();
    
    //pileup_file = TFile::Open("../../data/puWeights_10x_56ifb.root");
    pileup_file = TFile::Open("../../data/pileup/PUWeights_2018.root");
    pileup_2018 = (TH1F*)(((TH1F*)pileup_file->Get("puWeights"))->Clone());
    pileup_2018->SetDirectory(0);
    pileup_file->Close();

    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio100.root");
    pileup_ZJets_2017["HT-100To200"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-100To200"]->SetDirectory(0);
    pileup_file->Close();
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio200.root");
    pileup_ZJets_2017["HT-200To400"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-200To400"]->SetDirectory(0);
    pileup_file->Close();
    ////// FIXME FIXME FIXME zjetratio400.root does not exist!
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio200.root");
    pileup_ZJets_2017["HT-400To600"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-400To600"]->SetDirectory(0);
    pileup_file->Close();
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio600.root");
    pileup_ZJets_2017["HT-600To800"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-600To800"]->SetDirectory(0);
    pileup_file->Close();
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio800.root");
    pileup_ZJets_2017["HT-800To1200"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-800To1200"]->SetDirectory(0);
    pileup_file->Close();
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio1200.root");
    pileup_ZJets_2017["HT-1200To2500"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-1200To2500"]->SetDirectory(0);
    pileup_file->Close();
    pileup_file = TFile::Open("../../data/zjetpileup/zjetratio2500.root");
    pileup_ZJets_2017["HT-2500ToInf"] = (TH1F*)(((TH1F*)pileup_file->Get("PUwgt_cut1_data_yr2017"))->Clone());
    pileup_ZJets_2017["HT-2500ToInf"]->SetDirectory(0);
    pileup_file->Close();
    
    // Set up QCD and EWK corrections
    TFile * kfactors = TFile::Open("../../data/kfactors.root");
    TH1F * z_nlo = (TH1F*)kfactors->Get("ZJets_012j_NLO/nominal");
    TH1F * z_ewk = (TH1F*)kfactors->Get("EWKcorr/Z");
    TH1F * z_lo = (TH1F*)kfactors->Get("ZJets_LO/inv_pt");
    sf_z_qcd = (TH1F*)z_nlo->Clone();
    sf_z_qcd->Divide(z_lo);
    sf_z_ewk = (TH1F*)z_ewk->Clone();
    sf_z_ewk->Divide(z_lo);
    TH1F * w_nlo = (TH1F*)kfactors->Get("WJets_012j_NLO/nominal");
    TH1F * w_ewk = (TH1F*)kfactors->Get("EWKcorr/W");
    TH1F * w_lo = (TH1F*)kfactors->Get("WJets_LO/inv_pt");
    sf_w_qcd = (TH1F*)w_nlo->Clone();
    sf_w_qcd->Divide(w_lo);
    sf_w_ewk = (TH1F*)w_ewk->Clone();
    sf_w_ewk->Divide(w_lo);
    sf_z_qcd->SetDirectory(0);
    sf_z_ewk->SetDirectory(0);
    sf_w_qcd->SetDirectory(0);
    sf_w_ewk->SetDirectory(0);
    kfactors->Close();

    // Set trig weights
    TFile * trig_wgt_2016 = TFile::Open("../../data/trig_weights/trig_weights_2016.root");
    TFile * trig_wgt_2017 = TFile::Open("../../data/trig_weights/trig_weights_2017.root");
    TFile * trig_wgt_2018 = TFile::Open("../../data/trig_weights/trig_weights_2018.root");

    trig_hist_2016 = (TH1F*)(((TH1F*)trig_wgt_2016->Get("weigts_reco_PF_MetNoMu_pt_2016_MCTotal_2016_Dlow"))->Clone());
    trig_hist_2017 = (TH1F*)(((TH1F*)trig_wgt_2017->Get("weigts_reco_PF_MetNoMu_pt_2017_MCTotal_2017_Dlow"))->Clone());
    trig_hist_2018 = (TH1F*)(((TH1F*)trig_wgt_2018->Get("weigts_reco_PF_MetNoMu_pt_2018_MCTotal_2018_Dlow"))->Clone());

    trig_hist_2016->SetDirectory(0);
    trig_hist_2017->SetDirectory(0);
    trig_hist_2018->SetDirectory(0);

    trig_wgt_2016->Close();
    trig_wgt_2017->Close();
    trig_wgt_2018->Close();
}

void RDFAnalysis::SetMacroConfig(json macro_info) {
    macro_info_ = macro_info;
}

void RDFAnalysis::SetHistoConfig(map<TString, common::THInfo*> histos_info) {
    histos_info_ = histos_info;
}

void RDFAnalysis::SetCutConfig(vector<common::CutInfo> cuts_info) {
    cuts_info_ = cuts_info;
}

void RDFAnalysis::SetSampleConfig(common::SampleInfo sample_info) {
    sample_info_ = sample_info;
    name_ = sample_info_.label;
    mode_ = sample_info_.mode;
    group_ = sample_info_.group;
    sum_gen_wgt_ = sample_info_.sum_gen_wgt;
    xsec_ = sample_info_.xsec;
    year_ = sample_info_.year;
    custom_lumi_ = sample_info_.lumi;

    if (year_ == 2017) {
        lumi_ = 41.53 * 1000;
        trig_sf = trig_hist_2017;
        if (group_ == "ZJets") {
            if (name_.Contains("HT-100To200"))
                sf_pu = pileup_ZJets_2017["HT-100To200"];
            else if (name_.Contains("HT-200To400"))
                sf_pu = pileup_ZJets_2017["HT-200To400"];
            else if (name_.Contains("HT-400To600"))
                sf_pu = pileup_ZJets_2017["HT-400To600"];
            else if (name_.Contains("HT-600To800"))
                sf_pu = pileup_ZJets_2017["HT-600To800"];
            else if (name_.Contains("HT-800To1200"))
                sf_pu = pileup_ZJets_2017["HT-800To1200"];
            else if (name_.Contains("HT-1200To2500"))
                sf_pu = pileup_ZJets_2017["HT-1200To2500"];
            else if (name_.Contains("HT-2500ToInf"))
                sf_pu = pileup_ZJets_2017["HT-2500ToInf"];
            else {
                cout << "WARNING! Sample is from ZJets group but no pileup file could be found. Using HT-100To200 by default." << endl;
                sf_pu = pileup_ZJets_2017["HT-100To200"];
            }
        }
        else {
            sf_pu = pileup_2017;
        }
    }
    else if (year_ == 2016) {
        sf_pu = pileup_2016;
        lumi_ = 35.92 * 1000;
        trig_sf = trig_hist_2016;
    }
    else if (year_ == 2018) {
        sf_pu = pileup_2018;
        lumi_ = 59.74 * 1000;
        trig_sf = trig_hist_2018;
    }
    else {
        cout << "ERROR! Year not one of 2016/2017/2018. Exiting..." << endl;
    }
    // Check if we have a luminosity override
    if (custom_lumi_ > 0) 
        lumi_ = custom_lumi_ * 1000;

    cout << 
        "year: " << year_ << 
        ", sum_gen_wgt: " << sum_gen_wgt_ << 
        ", xsec: " << xsec_ << 
        " [pb], lumi: " << lumi_ << 
        " [1/pb] " << endl;
}

Bool_t RDFAnalysis::Process(TChain * chain) {

    if (sum_gen_wgt_ < 1e-10 && mode_ != common::DATA) return kFALSE;

    chain_ = chain;

    int num_cores = 0; // default is to use all available cores (never run during the day!)
    if (macro_info_.find("num_cores") != macro_info_.end())
        num_cores = macro_info_["num_cores"].get<int>();

    cout << "Creating dataframe..." << endl;

    ROOT::EnableImplicitMT(num_cores);
    ROOT::RDataFrame df(*chain_);

    const auto poolSize = ROOT::GetImplicitMTPoolSize();
    const auto nSlots = poolSize == 0 ? 1 : poolSize;
    int nEvents = chain_->GetEntries();
    std::string progressBar;
    //std::mutex barMutex; // Only one thread at a time can lock a mutex. Let's use this to avoid concurrent printing.
    // Magic numbers that yield good progress bars for nSlots = 1,2,4,8
    //const auto everyN = nSlots == 8 ? 1000 : 100ull * nSlots;
    const auto barWidth = 100; //nEvents / everyN;
    const int everyN = (nEvents / nSlots) / barWidth;
    cout << "everyN " << everyN << ", nSlots " << nSlots << endl;

    cout << "Total number of events to process: " << std::scientific << nEvents << endl;

    // First, define all the relevant weights

    auto calcZsf = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if ((group_ != "ZJets" && group_ != "DY") || gen_pt.size() != gen_ID.size()) return 1.0f;
        float Zpt = -1.0f; 
        Zpt = Min(gen_pt[abs(gen_ID) == 23]);
        if (Zpt < 0) return 1.0f;
        int binNum = sf_z_qcd->FindBin(Zpt);
        if (binNum == 0) binNum++;
        else if (binNum == sf_z_qcd->GetNbinsX()) binNum--;
        float sf = (float)sf_z_qcd->GetBinContent(binNum) * (float)sf_z_ewk->GetBinContent(binNum);
        return sf;
    };

    auto calcWsf = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if (group_ != "WJets" || gen_pt.size() != gen_ID.size()) return 1.0f;
        float Wpt = -1.0f; 
        Wpt = Min(gen_pt[abs(gen_ID) == 24]);
        if (Wpt < 0) return 1.0f;
        int binNum = sf_w_qcd->FindBin(Wpt);
        if (binNum == 0) binNum++;
        else if (binNum == sf_w_qcd->GetNbinsX()) binNum--;
        float sf = (float)sf_w_qcd->GetBinContent(binNum) * (float)sf_w_ewk->GetBinContent(binNum);
        // scale down WJets MC by 15% to match with data
        // (currently not enabled, just multiply by 1)
        sf *= 1.0;
        return sf; 
    };

    auto calcTsf = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if (group_ != "Top" || gen_pt.size() != gen_ID.size()) return 1.0f;
        int n_top = (abs(gen_ID) == 6).size();
        if (n_top != 2) return 1.0f;
        auto mask = gen_pt[abs(gen_ID)==6 && (gen_pt > 0 && gen_pt < 800)];
        auto sfs1 = Map(gen_pt[mask], [](float pt){ return exp(0.0615-0.0005*pt); });
        mask = gen_pt[abs(gen_ID)==6 && gen_pt >= 800];
        auto sfs2 = Map(gen_pt[mask], [](float pt){ return exp(0.0615-0.0005*800); });
        mask = gen_pt[abs(gen_ID)==6 && gen_pt <= 0];
        auto sfs3 = Map(gen_pt[mask], [](float pt){ return exp(0.0615-0.0005*0); });
        auto sfs = Concatenate(Concatenate(sfs1, sfs2), sfs3);
        float sf = sqrt(sfs[0] * sfs[1]);
        return sf;
    };

    auto calcTrigsf = [&](float met) {
        if (met < 120.0)
            return 0.3f;
        if (met > 800.0)
            return 1.0f;
        return (float)trig_sf->GetBinContent(trig_sf->FindBin(met));
    };

    auto calcPUsf = [&](int pileup) { 
        return (float)sf_pu->GetBinContent(sf_pu->FindBin((double)pileup)); 
    };

    auto calcTotalWgt = [&](float Zwgt, float Wwgt, float Twgt, float PUwgt, float trig_wgt, float gen_wgt) {
        double weight =  trig_wgt * Zwgt * Wwgt * PUwgt * xsec_ * lumi_ * gen_wgt / sum_gen_wgt_;
        // Warn if large weight *due only* to PU, Z, W, or genwgt / sum_gen_wgt_, but not due to xsec or lumi
        //if (trig_wgt > 3.0 || Zwgt > 3.0 || Wwgt > 3.0 || PUwgt > 3.0 || gen_wgt / sum_gen_wgt_ > 0.1) {
        //    cout << "WARNING! Very large weight: " << weight << endl;
        //    cout << "Trig wgt: " << trig_wgt << endl;
        //    cout << "Zwgt: " << Zwgt << endl;
        //    cout << "Wwgt: " << Wwgt << endl;
        //    cout << "PUwgt: " << PUwgt << endl;
        //    cout << "genwgt: " << gen_wgt << endl;
        //}
        return weight;
    };

    auto calcHemVeto = [&](bool hem_veto) { 
        return (year_ == 2018 ? !hem_veto : true);
    };

    auto calcNBTag = [&](RVec<float> jets_btag_id) {
        // pfDeepCSVJetTags:probb+probbb medium working points for 2018/17 and 2016
        float bTag_wp = (year_ == 2016 ? 0.6324 : 0.4184);
        auto jets_btagged = jets_btag_id[jets_btag_id > bTag_wp];
        return (int)jets_btagged.size();
    }; 

    // Need these to not segfault in case collection is empty
    // Also, RDataFrame.Histo1D() doesn't accept indexed vectors, so need to rename
    // e.g.: reco_PF_jet_pt[0] --> reco_PF_jet_pt0
    auto takeQuantity0 = [&](RVec<float> quant_vec) { return quant_vec.size() > 0 ? quant_vec[0] : -9999.f; };
    auto takeQuantity1 = [&](RVec<float> quant_vec) { return quant_vec.size() > 1 ? quant_vec[1] : -9999.f; };

    auto calcMETJetDphi = [&](RVec<float> jets_phi, float MET_phi) { 
        return !jets_phi.size() ? RVec<float>{-5.f} : abs(DeltaPhi(jets_phi, MET_phi));
    };

    auto findMetNoMu = [&](float MET_pt, float MET_phi, RVec<float> muons_pt, RVec<float> muons_phi) { 
        float metnomu_x = MET_pt * cos(MET_phi);
        float metnomu_y = MET_pt * sin(MET_phi);
        metnomu_x += Dot(muons_pt, cos(muons_phi));
        metnomu_y += Dot(muons_pt, sin(muons_phi));
        return sqrt(metnomu_x*metnomu_x + metnomu_y*metnomu_y);
    };

    auto findFakeMETCut = [&](float MET_pt, float MET_phi, float calomet_pt, float calomet_phi, float recoil) {
        return sqrt(
                ((MET_pt*cos(MET_phi) - calomet_pt*cos(calomet_phi)) *
                 (MET_pt*cos(MET_phi) - calomet_pt*cos(calomet_phi))) + 
                ((MET_pt*sin(MET_phi) - calomet_pt*sin(calomet_phi)) *
                 (MET_pt*sin(MET_phi) - calomet_pt*sin(calomet_phi)))
                )/recoil;
    };

    auto takeGenMuVxy = [&](RVec<float> gen_vxy, RVec<int> gen_ID) {
        if ((abs(gen_ID) == 13).size() == 0) return -9999.f;
        return float(Mean(gen_vxy[abs(gen_ID) == 13]));
    }; 

    auto calcGenMETmuDphi = [&](RVec<float> gen_pt, RVec<float> gen_phi, RVec<int> gen_ID, float MET_phi) {
        float px = Sum(gen_pt[gen_ID == 13] * cos(gen_phi[gen_ID == 13]));
        float py = Sum(gen_pt[gen_ID == 13] * sin(gen_phi[gen_ID == 13]));
        float mu_phi = atan2(py, px);
        return abs(DeltaPhi(MET_phi, mu_phi));
    };

    auto calcRecoMETmuDphi = [&](float sel_mu_pt0, float sel_mu_phi0, float sel_mu_pt1, float sel_mu_phi1, float MET_phi) {
        float px = sel_mu_pt0 * cos(sel_mu_phi0) + sel_mu_pt1 * cos(sel_mu_phi1);
        float py = sel_mu_pt0 * sin(sel_mu_phi0) + sel_mu_pt1 * sin(sel_mu_phi1);
        float mu_phi = atan2(py, px);
        return abs(DeltaPhi(MET_phi, mu_phi));
    };

    auto calcVtxSignificance = [&](float reco_vtx_vxy, float reco_vtx_sigmavxy) { return reco_vtx_vxy/reco_vtx_sigmavxy; };

    auto passMuonID = [&](RVec<float> trk_n_planes, RVec<float> trk_n_hits, RVec<float> trk_chi2, RVec<float> pt, RVec<float> eta, RVec<float> pt_err) {
        RVec<bool> pass = (trk_n_planes > 1) && (trk_n_hits > 11) && (trk_chi2 < 10) && (pt > 5) && (abs(eta) < 2.4);// && (pt_err/pt < 1);
        return pass;
    };

    auto passVtxID = [&](RVec<bool> muon_pass0, RVec<bool> muon_pass1, RVec<int> q0, RVec<int> q1, RVec<float> vtx_chi2) {
        // construct vertex id mask from each muon id mask
        // vertex index is 4 * muon0 + muon1
        RVec<bool> vtx_pass(16, 0);
        auto comb_idx = Combinations(4, 4);
        for (size_t i = 0; i < comb_idx[0].size(); i++)
            if (comb_idx[0][i] < muon_pass0.size() && comb_idx[1][i] < muon_pass1.size())
                vtx_pass[i] = (muon_pass0[comb_idx[0][i]] == 1) && (muon_pass1[comb_idx[1][i]] == 1); // && (q0[comb_idx[0][i]] + q1[comb_idx[1][i]] == 0) && (vtx_chi2[i] < 4);
        return vtx_pass;
    };

    auto pickBestVtx = [&](RVec<float> vtx_chi2, RVec<bool> vtx_pass) {
        // return the index in the collection with minimum chi2
        //RVec<float> inf(16, 100000.f);
        auto vtx_chi2_inf = Where(vtx_pass == 1, vtx_chi2, 999999.f);
        return ArgMin(vtx_chi2_inf); 
    };

    // for each dsa muon in best vtx, look for *best* gm match in dr
    auto findMuonMatch0 = [&](RVec<int> dsagm_match, RVec<float> dsagm_dR, RVec<bool> vtx_pass_dsagm, size_t dsa_index_0, size_t dsa_index_1) {
        size_t best_index = dsa_index_0; // if no match, just return dsa index
        float min_dR = 999999.f;
        for (size_t j = 0; j < 4; j++) {
            if (dsagm_match[4 * dsa_index_0 + j] == 1 && vtx_pass_dsagm[4 * dsa_index_1 + j] == 1 && abs(dsagm_dR[4 * dsa_index_0 + j]) < min_dR) {
                min_dR = abs(dsagm_dR[4 * dsa_index_0 + j]);
                best_index = 4 + j;
            }
        }
        return best_index;
    };

    auto findMuonMatch1 = [&](RVec<int> dsagm_match, RVec<float> dsagm_dR, RVec<bool> vtx_pass_dsagm, RVec<bool> vtx_pass_gmgm, size_t dsa_index_1, size_t best_muon_0) {
        size_t best_index = dsa_index_1; // if no match, just return dsa index
        float min_dR = 999999.f;
        for (size_t j = 0; j < 4; j++) {
            if (best_muon_0 > 3) {
                if (dsagm_match[4 * dsa_index_1 + j] == 1 && vtx_pass_gmgm[4 * (best_muon_0-4) + j] == 1 && 4 + j != best_muon_0 && abs(dsagm_dR[4 * dsa_index_1 + j]) < min_dR) {
                    min_dR = abs(dsagm_dR[4 * dsa_index_1 + j]);
                    best_index = 4 + j;
                }
            }
            else {
                if (dsagm_match[4 * dsa_index_1 + j] == 1 && vtx_pass_dsagm[4 * best_muon_0 + j] == 1 && abs(dsagm_dR[4 * dsa_index_1 + j]) < min_dR) {
                    min_dR = abs(dsagm_dR[4 * dsa_index_1 + j]);
                    best_index = 4 + j; // make gm index offset by 4
                }
            }
        }
        return best_index;
    };

    auto takeMatchedVtxQuantity = [&](RVec<float> quant_dsadsa, RVec<float> quant_gmgm, RVec<float> quant_dsagm, size_t best_muon_0, size_t best_muon_1) {
        if (quant_dsadsa.size() == 0) return -9999.f;
        float final_quant;
        // Check for each best muon to see if it's gm (offset by 4) or dsa
        if (best_muon_0 > 3 && best_muon_1 > 3)
            final_quant = quant_gmgm[4 * (best_muon_0-4) + (best_muon_1-4)];
        else if (best_muon_0 > 3)
            final_quant = quant_dsagm[4 * (best_muon_1) + (best_muon_0-4)];
        else if (best_muon_1 > 3)
            final_quant = quant_dsagm[4 * (best_muon_0) + (best_muon_1-4)];
        else
            final_quant = quant_dsadsa[4 * best_muon_0 + best_muon_1];
        return final_quant;
    };

    auto takeMatchedMuonQuantity = [&](RVec<float> quant_dsa, RVec<float> quant_gm, size_t best_muon) {
        if (quant_dsa.size() == 0) return -9999.f;
        return (best_muon > 3) ? quant_gm[best_muon-4] : quant_dsa[best_muon];
    };

    auto takeMatchedMuonQuantityInt = [&](RVec<int> quant_dsa, RVec<int> quant_gm, size_t best_muon) {
        if (quant_dsa.size() == 0) return -9999;
        return (best_muon > 3) ? quant_gm[best_muon-4] : quant_dsa[best_muon];
    };

    auto calcMatchedMuonMT = [&](float muon_pt, float muon_phi, float MET_pt, float MET_phi) {
        float dphi = DeltaPhi(muon_phi, MET_phi);
        return sqrt(2.0f * muon_pt * MET_pt * (1.0f - cos(dphi)));
    };

    auto calcMatchedMuonInvMass = [&](float mu1_pt, float mu1_eta, float mu1_phi, float mu2_pt, float mu2_eta, float mu2_phi) {
        return sqrt(2.0f * mu1_pt * mu2_pt * (cosh(mu1_eta - mu2_eta) - cos(mu1_phi - mu2_phi)));
    };

    TString jet_pt = "reco_PF_jet_corr_pt";
    TString jet_eta = "reco_PF_jet_corr_eta";
    TString jet_phi = "reco_PF_jet_corr_phi";
    TString MET_pt = "reco_PF_MET_corr_pt";
    TString MET_phi = "reco_PF_MET_corr_phi";
    if (macro_info_.find("jet_systematic") != macro_info_.end()) {
        std::string jet_syst = macro_info_["jet_systematic"].get<std::string>();
        jet_pt = Form("reco_PF_jet_corr_%s_pt", jet_syst.c_str());
        jet_eta = Form("reco_PF_jet_corr_%s_eta", jet_syst.c_str());
        jet_phi = Form("reco_PF_jet_corr_%s_phi", jet_syst.c_str());
        MET_pt = Form("reco_PF_MET_corr_%s_pt", jet_syst.c_str());
        MET_phi = Form("reco_PF_MET_corr_%s_phi", jet_syst.c_str());
    }

    auto df_wgts = df.
        Define("dsa_pass_ID", passMuonID, {"reco_dsa_trk_n_planes", "reco_dsa_trk_n_hits", "reco_dsa_trk_chi2", "reco_dsa_pt", "reco_dsa_eta", "reco_dsa_pt_err"}).
        Define("gm_pass_ID", passMuonID, {"reco_gm_trk_n_planes", "reco_gm_trk_n_hits", "reco_gm_trk_chi2", "reco_gm_pt", "reco_gm_eta", "reco_gm_pt_err"}).
        Define("n_good_dsa", "(int)Nonzero(dsa_pass_ID).size()").
        Define("n_good_gm", "(int)Nonzero(gm_pass_ID).size()").
        Define("good_vtx_dsadsa", passVtxID, {"dsa_pass_ID", "dsa_pass_ID", "reco_dsa_charge", "reco_dsa_charge", "reco_vtx_dsadsa_reduced_chi2"}).
        Define("good_vtx_gmgm", passVtxID, {"gm_pass_ID", "gm_pass_ID", "reco_gm_charge", "reco_gm_charge", "reco_vtx_gmgm_reduced_chi2"}).
        Define("good_vtx_dsagm", passVtxID, {"dsa_pass_ID", "gm_pass_ID", "reco_dsa_charge", "reco_gm_charge", "reco_vtx_dsagm_reduced_chi2"}).
        Define("best_vtx_dsadsa", pickBestVtx, {"reco_vtx_dsadsa_reduced_chi2", "good_vtx_dsadsa"}).
        Define("best_dsa_0", "best_vtx_dsadsa / 4").
        Define("best_dsa_1", "best_vtx_dsadsa % 4").
        Define("best_muon_0", findMuonMatch0, {"reco_gbmdsa_match", "reco_gbmdsa_dR", "good_vtx_dsagm", "best_dsa_0", "best_dsa_1"}).
        Define("best_muon_1", findMuonMatch1, {"reco_gbmdsa_match", "reco_gbmdsa_dR", "good_vtx_dsagm", "good_vtx_gmgm", "best_dsa_1", "best_muon_0"}).
        Define("n_matched", "(int)(best_muon_0>3) + (int)(best_muon_1>3)").
        Define("matched_muon_vtx_vxy", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_vxy", "reco_vtx_gmgm_vxy", "reco_vtx_dsagm_vxy", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_vz", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_vz", "reco_vtx_gmgm_vz", "reco_vtx_dsagm_vz", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_sigmavxy", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_sigmavxy", "reco_vtx_gmgm_sigmavxy", "reco_vtx_dsagm_sigmavxy", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_reduced_chi2", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_reduced_chi2", "reco_vtx_gmgm_reduced_chi2", "reco_vtx_dsagm_reduced_chi2", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_dR", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_dR", "reco_vtx_gmgm_dR", "reco_vtx_dsagm_dR", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_sign", calcVtxSignificance, {"matched_muon_vtx_vxy", "matched_muon_vtx_sigmavxy"}).
        Define("reco_sel_mu_pt0", takeMatchedMuonQuantity, {"reco_dsa_pt", "reco_gm_pt", "best_muon_0"}).
        Define("reco_sel_mu_pt1", takeMatchedMuonQuantity, {"reco_dsa_pt", "reco_gm_pt", "best_muon_1"}).
        Define("reco_sel_mu_eta0", takeMatchedMuonQuantity, {"reco_dsa_eta", "reco_gm_eta", "best_muon_0"}).
        Define("reco_sel_mu_eta1", takeMatchedMuonQuantity, {"reco_dsa_eta", "reco_gm_eta", "best_muon_1"}).
        Define("reco_sel_mu_phi0", takeMatchedMuonQuantity, {"reco_dsa_phi", "reco_gm_phi", "best_muon_0"}).
        Define("reco_sel_mu_phi1", takeMatchedMuonQuantity, {"reco_dsa_phi", "reco_gm_phi", "best_muon_1"}).
        Define("reco_sel_mu_dxy0", takeMatchedMuonQuantity, {"reco_dsa_dxy", "reco_gm_dxy", "best_muon_0"}).
        Define("reco_sel_mu_dxy1", takeMatchedMuonQuantity, {"reco_dsa_dxy", "reco_gm_dxy", "best_muon_1"}).
        Define("reco_sel_mu_dxy_err0", takeMatchedMuonQuantity, {"reco_dsa_dxy_err", "reco_gm_dxy_err", "best_muon_0"}).
        Define("reco_sel_mu_dxy_err1", takeMatchedMuonQuantity, {"reco_dsa_dxy_err", "reco_gm_dxy_err", "best_muon_1"}).
        Define("reco_sel_mu_dxy_sign0", "reco_sel_mu_dxy0/reco_sel_mu_dxy_err0").
        Define("reco_sel_mu_dxy_sign1", "reco_sel_mu_dxy1/reco_sel_mu_dxy_err1").
        Define("reco_sel_mu_dz0", takeMatchedMuonQuantity, {"reco_dsa_dz", "reco_gm_dz", "best_muon_0"}).
        Define("reco_sel_mu_dz1", takeMatchedMuonQuantity, {"reco_dsa_dz", "reco_gm_dz", "best_muon_1"}).
        Define("reco_sel_mu_q0", takeMatchedMuonQuantityInt, {"reco_dsa_charge", "reco_gm_charge", "best_muon_0"}).
        Define("reco_sel_mu_q1", takeMatchedMuonQuantityInt, {"reco_dsa_charge", "reco_gm_charge", "best_muon_1"}).
        Define("reco_dsa_pt_sign", "reco_dsa_pt/reco_dsa_pt_err").
        Define("reco_gm_pt_sign", "reco_gm_pt/reco_gm_pt_err").
        Define("reco_sel_mu_pt_sign0", takeMatchedMuonQuantity, {"reco_dsa_pt_sign", "reco_gm_pt_sign", "best_muon_0"}).
        Define("reco_sel_mu_pt_sign1", takeMatchedMuonQuantity, {"reco_dsa_pt_sign", "reco_gm_pt_sign", "best_muon_1"}).
        Define("matched_muon_MT", calcMatchedMuonMT, {"reco_sel_mu_pt0", "reco_sel_mu_phi0", MET_pt.Data(), MET_phi.Data()}).
        Define("matched_muon_Mmumu", calcMatchedMuonInvMass, {"reco_sel_mu_pt0", "reco_sel_mu_eta0", "reco_sel_mu_phi0", "reco_sel_mu_pt1", "reco_sel_mu_eta1", "reco_sel_mu_phi1"}).
        Define("reco_PF_jet_pt0", takeQuantity0, {jet_pt.Data()}).
        Define("reco_PF_jet_pt1", takeQuantity1, {jet_pt.Data()}).
        Define("reco_PF_jet_eta0", takeQuantity0, {jet_eta.Data()}).
        Define("reco_PF_jet_eta1", takeQuantity1, {jet_eta.Data()}).
        Define("reco_PF_jet_phi0", takeQuantity0, {jet_phi.Data()}).
        Define("reco_PF_jet_phi1", takeQuantity1, {jet_phi.Data()}).
        Define("reco_dsa0_trk_chi2", takeQuantity0, {"reco_dsa_trk_chi2"}).
        Define("reco_dsa1_trk_chi2", takeQuantity1, {"reco_dsa_trk_chi2"}).
        Define("MET_jet_dphi", calcMETJetDphi, {jet_phi.Data(), MET_phi.Data()}).
        Define("MET_jet_dphi0", takeQuantity0, {"MET_jet_dphi"}).
        Define("recoil_jet_phi_dphi", calcMETJetDphi, {jet_phi.Data(), "reco_PF_recoil_phi"}).
        Define("fake_MET_fraction", findFakeMETCut, {MET_pt.Data(), MET_phi.Data(), "reco_Calo_MET_pt", "reco_Calo_MET_phi", "reco_PF_recoil_pt"}).
        Define("reco_PF_MetNoMu_pt", findMetNoMu, {MET_pt.Data(), MET_phi.Data(), "reco_gm_pt", "reco_gm_phi"}).
        Define("hem_veto", calcHemVeto, {"reco_PF_HEM_flag"}).
        Define("reco_n_bTag_jets", calcNBTag, {"reco_PF_jet_corr_BTag"}).
        Define("reco_METmu_dphi_v2", calcRecoMETmuDphi, {"reco_sel_mu_pt0", "reco_sel_mu_phi0", "reco_sel_mu_pt1", "reco_sel_mu_phi1", MET_phi.Data()}).
        Define("CaloPFMETRatio", Form("abs(reco_Calo_MET_pt - %s)/reco_Calo_MET_pt", MET_pt.Data()));


    if (mode_ == common::DATA) 
        df_wgts = df_wgts.Define("wgt", "1.0");
    else {
        df_wgts = df_wgts.
            Define("gen_muon_vxy", takeGenMuVxy, {"gen_vxy", "gen_ID"}).
            Define("gen_METmu_dphi", calcGenMETmuDphi, {"gen_pt", "gen_phi", "gen_ID", "gen_MET_phi"}).
            Define("Zwgt", calcZsf, {"gen_ID", "gen_pt"}).
            Define("Wwgt", calcWsf, {"gen_ID", "gen_pt"}).
            Define("Twgt", calcTsf, {"gen_ID", "gen_pt"}).
            Define("PUwgt", calcPUsf, {"gen_pu_true"}).
            Define("trig_wgt", calcTrigsf, {"reco_PF_MetNoMu_pt"}).
            Define("wgt", calcTotalWgt, {"Zwgt", "Wwgt", "Twgt", "PUwgt", "trig_wgt", "gen_wgt"});
    }


    // Beginning of cuts

    vector<TString> cut_strings;

    all_histos_1D_.clear();
    all_histos_2D_.clear();
    cutflow_.clear();

    auto df_filters = df_wgts.Filter(cuts_info_[0].cut.Data(), Form("Cut0"));
    cutflow_.push_back(df_filters.Sum<double>("wgt"));
    for (size_t cut = 1; cut < cuts_info_.size(); cut++) {

        // Some cuts are not inclusive, so make copy to restore later in case
        auto temp_df = df_filters;

        df_filters = df_filters.Filter(cuts_info_[cut].cut.Data(), Form("Cut%d", cut));
        cutflow_.push_back(df_filters.Sum<double>("wgt"));

        // make all requested histograms for each cut, IFF book_plot is true for that cut
        if (cuts_info_[cut].book_plot == TString("no")) {
            // before continue restore df if it's one of the 3 SR definitions (or a otherwise non-inclusive cut)
            if (cuts_info_[cut].inclusive == TString("no")) 
                df_filters = temp_df;
            continue;
        }

        for (auto & [histo_name, histo_info] : histos_info_) {
            // if current sample's group is included in histo's list of groups to exclude, continue
            if (std::find(histo_info->groups_to_exclude.begin(), histo_info->groups_to_exclude.end(), group_) != histo_info->groups_to_exclude.end())
                continue;

            // split into 1D and 2D, and int and float cases

            if (histo_info->type == "float1D") {
                // std::map auto-initializes if no element found
                if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                    double bin_edges[histo_info->binEdgesX.size()];
                    std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<float,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data(), "wgt");
                }
                else { // just number of bins and low and high X
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<float,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
                }
            }
            else if (histo_info->type == "int1D") {
                // std::map auto-initializes if no element found
                if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                    double bin_edges[histo_info->binEdgesX.size()];
                    std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<int,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data(), "wgt");
                }
                else // just number of bins and low and high X
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<int,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
            }
            else if (histo_info->type == "float2D") 
                all_histos_2D_[histo_name][cut] = df_filters.Histo2D<float,float,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");
            else if (histo_info->type == "int2D")
                all_histos_2D_[histo_name][cut] = df_filters.Histo2D<int,int,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");
            else
                cout << "Hist type not recognized!" << endl;
        }

        // restore df if it's one of the 3 SR definitions (or a otherwise non-inclusive cut)
        if (cuts_info_[cut].inclusive == "no") 
            df_filters = temp_df;
    }

    if (mode_ != common::DATA) {
        auto df_sumgenwgts = df_wgts.Sum("gen_wgt");

        df_sumgenwgts.OnPartialResult(everyN, [&barWidth, &progressBar/*, &barMutex*/](double &) {
                //std::lock_guard<std::mutex> l(barMutex); // lock_guard locks the mutex at construction, releases it at destruction
                progressBar.push_back('#');
                // re-print the line with the progress bar
                std::cout << "\r[" << std::left << std::setw(barWidth) << progressBar << ']' << std::flush;
                });

        cout << "Triggering event loop..." << endl;
        auto value = *df_sumgenwgts;
        cout << endl << "RDF sum_gen_wgts: " << value << endl;
    }

    df_filters.Report()->Print();

    return kTRUE;
}

void RDFAnalysis::Terminate() {
    //delete sf_pu;
    delete pileup_2018;
    delete pileup_2017;
    delete pileup_2016;
    for (std::pair<TString, TH1F *> pileup : pileup_ZJets_2017)
        delete pileup.second;
    delete sf_z_qcd;
    delete sf_z_ewk;
    delete sf_w_qcd;
    delete sf_w_ewk;
    delete trig_hist_2018;
    delete trig_hist_2017;
    delete trig_hist_2016;
}
