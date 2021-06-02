#include <math.h>
#include <cstdlib>

#include <ROOT/RVec.hxx>
using namespace ROOT::VecOps;
#include <TFile.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>

#include "../../utils/rapidcsv.h"
#include "RDFAnalysis.h"

using std::map, std::vector, std::cout, std::endl;

Bool_t RDFAnalysis::Begin(int syst) {
    syst_ = (ScaleFactors::SYST)syst;
    scale_factors.buildScaleFactors(syst_);
    nEvents_ = 0;
    return true;
}

void RDFAnalysis::SetMacroConfig(json macro_info) {
    macro_info_ = macro_info;
    maxDrGMdSA_ = macro_info_["maxDrGMdSA"].get<float>();
    num_cores_ = 4; // default is now 3 or 4 cores to still fit within condor's 2 GB memory limit
    if (macro_info_.find("num_cores") != macro_info_.end())
        num_cores_ = macro_info_["num_cores"].get<int>();
    jet_syst_ = "";
    if (macro_info_.find("jet_systematic") != macro_info_.end())
        jet_syst_ = macro_info_["jet_systematic"].get<std::string>();
}

void RDFAnalysis::SetHistoConfig(map<TString, common::THInfo*> histos_info) {
    histos_info_ = histos_info;
}

void RDFAnalysis::SetCutConfig(vector<common::CutInfo> cuts_info) {
    cuts_info_ = cuts_info;
}

long long RDFAnalysis::GetNEvents() {
    return nEvents_;
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

    if (year_ == 2016)
        lumi_ = 35.92 * 1000;
    else if (year_ == 2017)
        lumi_ = 41.53 * 1000;
    else if (year_ == 2018)
        lumi_ = 59.74 * 1000;
    else
        cout << "ERROR! Year not one of 2016/2017/2018. Exiting..." << endl;
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

    cout << "Creating dataframe..." << endl;

    ROOT::EnableImplicitMT(num_cores_);
    ROOT::RDataFrame df(*chain_);

    const auto poolSize = ROOT::GetThreadPoolSize();
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
    nEvents_ = nEvents;

    // First, define all the relevant weights

    auto calcBTagWgt = [&](int n_btagged_jets, RVec<float> jets_pt, RVec<float> jets_eta, RVec<bool> jets_btag_pass) {
        return 1.0f;
        auto & sfs = scale_factors.btag_sfs[year_];
        if (n_btagged_jets > 2)
            return 1.0f;
        // for our purposes the sf does not change between eta -2.5 and 2.5
        float w = 1.0f;
        if (n_btagged_jets == 2) {
            float pt = jets_pt[0];
            auto it = std::lower_bound(sfs.begin(), sfs.end(), pt, ScaleFactors::compareBTagLowerEdge);
            // if pt is greater than the highest lower edge, use this last entry to compute sf
            if (it == sfs.end())
                it--;
            float sf1 = it->formula->Eval(pt);
            pt = jets_pt[1];
            it = std::lower_bound(sfs.begin(), sfs.end(), pt, ScaleFactors::compareBTagLowerEdge);
            if (it == sfs.end())
                it--;
            float sf2 = it->formula->Eval(pt);
            w = (1.0f - sf1) * (1.0f - sf2);
        }
        else if (n_btagged_jets == 1) {
            float pt = (jets_btag_pass[0] ? jets_pt[0] : jets_pt[1]);
            auto it = std::lower_bound(sfs.begin(), sfs.end(), pt, ScaleFactors::compareBTagLowerEdge);
            if (it == sfs.end())
                it--;
            float sf = it->formula->Eval(pt);
            w = (1.0f - sf);
        }
        else if (n_btagged_jets == 0) {
            w = 1.0f;
        }
        return w;
    };

    auto calcZWgt = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if ((group_ != "ZJets" && group_ != "DY") || gen_pt.size() != gen_ID.size()) return 1.0f;
        return 1.23f;
        // TURNED OFF FOR NOW
        // float Zpt = -1.0f;
        // sf_z_qcd = scale_factors.all_sfs["z_qcd"][-1];
        // sf_z_ewk = scale_factors.all_sfs["z_ewk"][-1]; 
        // Zpt = Min(gen_pt[abs(gen_ID) == 23]);
        // if (Zpt < 0) return 1.0f;
        // int binNum = sf_z_qcd->FindBin(Zpt);
        // if (binNum == 0) binNum++;
        // else if (binNum == sf_z_qcd->GetNbinsX()) binNum--;
        // float sf = (float)sf_z_qcd->GetBinContent(binNum) * (float)sf_z_ewk->GetBinContent(binNum);
        // return sf;
    };

    auto calcWWgt = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if (group_ != "WJets" || gen_pt.size() != gen_ID.size()) return 1.0f;
        return 1.21f;
        // TURNED OFF FOR NOW
        // float Wpt = -1.0f;
        // sf_w_qcd = scale_factors.all_sfs["w_qcd"][-1]
        // sf_w_ewk = scale_factors.all_sfs["w_ewk"][-1] 
        // Wpt = Min(gen_pt[abs(gen_ID) == 24]);
        // if (Wpt < 0) return 1.0f;
        // int binNum = sf_w_qcd->FindBin(Wpt);
        // if (binNum == 0) binNum++;
        // else if (binNum == sf_w_qcd->GetNbinsX()+1) binNum--;
        // float sf = (float)sf_w_qcd->GetBinContent(binNum) * (float)sf_w_ewk->GetBinContent(binNum);
        // // scale down WJets MC by 15% to match with data
        // // (currently not enabled, just multiply by 1)
        // sf *= 1.0;
        // return sf; 
    };

    auto calcTWgt = [&](RVec<int> gen_ID, RVec<float> gen_pt) { 
        if (group_ != "Top" || gen_pt.size() != gen_ID.size())
            return 1.0f;
        int n_top = (abs(gen_ID) == 6).size();
        if (n_top != 2)
            return 1.0f;
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

    auto calcTrigWgt = [&](float met) {
        if (met < 100.0 || met > 800.0)
            return 1.0f;
        auto & sf = scale_factors.all_sfs["trigger"][year_];
        return (float)sf->GetBinContent(sf->FindBin(met));
    };

    auto calcPUWgt = [&](int pileup) { 
        auto & sf = scale_factors.all_sfs["pileup"][year_];
        return (float)sf->GetBinContent(sf->FindBin((double)pileup)); 
    };

    auto calcDisplacedIDWgt = [&](RVec<float> pt, RVec<float> eta, RVec<float> dxy, size_t id) {
        if (pt.size() < 2)
            return 1.0f;
        auto & sfs_Z = scale_factors.all_sfs["dsa_ID_Z"][year_];
        auto & sfs_Z_stat = scale_factors.all_sfs["dsa_ID_Z_stat"][year_];
        auto & sfs_Z_syst = scale_factors.all_sfs["dsa_ID_Z_syst"][year_];
        auto & sfs_JPsi = scale_factors.all_sfs["dsa_ID_JPsi"][year_];
        auto & sfs_JPsi_stat = scale_factors.all_sfs["dsa_ID_JPsi_stat"][year_];
        auto & sfs_JPsi_syst = scale_factors.all_sfs["dsa_ID_JPsi_syst"][year_];
        float wgt = 1.0f;
        if (pt[id] > 20) {
            auto bin = sfs_Z->FindBin(abs(eta[id]), pt[id]);
            wgt = sfs_Z->GetBinContent(bin);
            if (syst_ == ScaleFactors::DSA_ID_DOWN)
                wgt -= sqrt(pow(sfs_Z_stat->GetBinContent(bin), 2) + pow(sfs_Z_syst->GetBinContent(bin), 2));
            else if (syst_ == ScaleFactors::DSA_ID_UP)
                wgt += sqrt(pow(sfs_Z_stat->GetBinContent(bin), 2) + pow(sfs_Z_syst->GetBinContent(bin), 2));
        }
        else if (pt[id] > 5) {
            auto bin = sfs_JPsi->FindBin(abs(eta[id]), pt[id]);
            wgt = sfs_JPsi->GetBinContent(bin);
            if (syst_ == ScaleFactors::DSA_ID_DOWN)
                wgt -= sqrt(pow(sfs_JPsi_stat->GetBinContent(bin), 2) + pow(sfs_JPsi_syst->GetBinContent(bin), 2));
            else if (syst_ == ScaleFactors::DSA_ID_UP)
                wgt += sqrt(pow(sfs_JPsi_stat->GetBinContent(bin), 2) + pow(sfs_JPsi_syst->GetBinContent(bin), 2));
        }
        // efficiency from displacement
        float slope = 0.00035; 
        float disp = 1.0 - slope * abs(dxy[id]);
        return wgt * disp;
    };

    auto calcDSARecoWgt = [&](RVec<float> pt, RVec<float> eta, RVec<float> dxy, size_t id) {
        if (pt.size() < 2)
            return 1.0f;
        auto & sfs_Z = scale_factors.all_sfs["dsa_reco_Z"][year_];
        auto & sfs_Z_stat = scale_factors.all_sfs["dsa_reco_Z_stat"][year_];
        auto & sfs_Z_syst = scale_factors.all_sfs["dsa_reco_Z_syst"][year_];
        auto & sfs_JPsi = scale_factors.all_sfs["dsa_reco_JPsi"][year_];
        auto & sfs_JPsi_stat = scale_factors.all_sfs["dsa_reco_JPsi_stat"][year_];
        auto & sfs_JPsi_syst = scale_factors.all_sfs["dsa_reco_JPsi_syst"][year_];
        float wgt = 1.0f;
        if (pt[id] > 20) {
            auto bin = sfs_Z->FindBin(abs(eta[id]), pt[id]);
            wgt = sfs_Z->GetBinContent(bin);
            if (syst_ == ScaleFactors::DSA_RECO_DOWN)
                wgt -= sqrt(pow(sfs_Z_stat->GetBinContent(bin), 2) + pow(sfs_Z_syst->GetBinContent(bin), 2));
            else if (syst_ == ScaleFactors::DSA_RECO_UP)
                wgt += sqrt(pow(sfs_Z_stat->GetBinContent(bin), 2) + pow(sfs_Z_syst->GetBinContent(bin), 2));
        }
        else if (pt[id] > 5) {
            auto bin = sfs_JPsi->FindBin(abs(eta[id]), pt[id]);
            wgt = sfs_JPsi->GetBinContent(bin);
            if (syst_ == ScaleFactors::DSA_RECO_DOWN)
                wgt -= sqrt(pow(sfs_JPsi_stat->GetBinContent(bin), 2) + pow(sfs_JPsi_syst->GetBinContent(bin), 2));
            else if (syst_ == ScaleFactors::DSA_RECO_UP)
                wgt += sqrt(pow(sfs_JPsi_stat->GetBinContent(bin), 2) + pow(sfs_JPsi_syst->GetBinContent(bin), 2));
        }
        return wgt;
    };

    auto calcGMWgt = [&](RVec<float> pt, RVec<float> eta, size_t id) {
        auto & sfs_highpt = scale_factors.all_sfs["gm_ID"][year_];
        auto & sfs_highpt_stat = scale_factors.all_sfs["gm_ID_stat"][year_];
        auto & sfs_highpt_syst = scale_factors.all_sfs["gm_ID_syst"][year_];
        auto & sfs_lowpt = scale_factors.gmlowpt_sfs[year_]; // different structure since hard-coded
        if (id < 4) // *not* a global muon
            return 1.0f;
        float wgt = 1.0f;
        if (pt[id-4] > 20) {
            auto bin = sfs_highpt->FindBin(pt[id-4], abs(eta[id-4]));
            wgt = sfs_highpt->GetBinContent(bin);
            if (syst_ == ScaleFactors::GM_ID_DOWN)
                wgt -= sqrt(pow(sfs_highpt_stat->GetBinContent(bin), 2) + pow(sfs_highpt_syst->GetBinContent(bin), 2));
            else if (syst_ == ScaleFactors::GM_ID_UP)
                wgt += sqrt(pow(sfs_highpt_stat->GetBinContent(bin), 2) + pow(sfs_highpt_syst->GetBinContent(bin), 2));
        }
        else if (pt[id-4] > 5) {
            auto it = std::lower_bound(sfs_lowpt.begin(), sfs_lowpt.end(), eta[id-4], ScaleFactors::compareGMLowerEdge);
            // if eta is greater than the highest lower edge, use last entry to compute sf
            if (it == sfs_lowpt.end())
                it--;
            wgt = it->central;
            if (syst_ == ScaleFactors::GM_ID_UP)
                wgt += it->up;
            else if (syst_ == ScaleFactors::GM_ID_DOWN)
                wgt -= it->down;
        }
        return wgt;
    };

    auto calcVetoWgt = [&](RVec<int> good_e, RVec<float> eta_e, RVec<float> pt_e, RVec<int> good_ph, RVec<float> eta_ph, RVec<float> pt_ph) { 
        float wgt_e = 1.0, wgt_ph = 1.0;
        auto & sfs_e = scale_factors.all_sfs["veto_e"][year_];
        auto & sfs_ph = scale_factors.all_sfs["veto_ph"][year_];
        for (size_t i = 0; i < eta_e.size(); i++) {
          if (!good_e[i] || pt_e[i] < 10 || abs(eta_e[i]) > 2.5) 
            continue;
          float sf_e = sfs_e->GetBinContent(sfs_e->FindBin(eta_e[i], pt_e[i]));
          float sf_eerr = sfs_e->GetBinError(sfs_e->FindBin(eta_e[i], pt_e[i]));
          wgt_e *= (1.0 - sf_e);
          //wgt_e *= (1.0 - (sf_e+sf_eerr));
        }
        for (size_t i = 0; i < eta_ph.size(); i++) {
          if (good_ph[i] != 1 || pt_ph[i] < 15 || abs(eta_ph[i]) > 2.5)
            continue;
          float sf_ph = sfs_ph->GetBinContent(sfs_ph->FindBin(eta_ph[i], pt_ph[i]));
          float sf_pherr = sfs_ph->GetBinError(sfs_ph->FindBin(eta_ph[i], pt_ph[i]));
          wgt_ph *= (1.0 - sf_ph);
          //wgt_p *= (1.0 - (sf_p+sf_perr));
        }
        return wgt_e * wgt_ph;
    };

    auto calcTotalWgt = [&](float veto_wgt, float dsa_reco_wgt, float dsa_ID_wgt, float gm_wgt, float Zwgt, 
                        float Wwgt, float Twgt, float PUwgt, float trig_wgt, float gen_wgt, float bTag_wgt) {
        double weight =  veto_wgt * dsa_reco_wgt * dsa_ID_wgt * gm_wgt * Zwgt * 
                        Wwgt * PUwgt * trig_wgt * xsec_ * lumi_ * gen_wgt * bTag_wgt / sum_gen_wgt_;
        return weight;
    };

    auto calcHemVeto = [&](bool hem_veto) { 
        return (year_ == 2018 ? !hem_veto : true);
    };

    auto calcBTagPass = [&](RVec<float> jets_btag_id) {
        // pfDeepCSVJetTags:probb+probbb medium working points for 18+17+16
        float bTag_wp = 1.0f;
        if (year_ == 2016)
            bTag_wp = 0.6324;
        else if (year_ == 2017)
            bTag_wp = 0.4941;
        else if (year_ == 2018)
            bTag_wp = 0.4184;
        RVec<bool> jets_btag_pass = jets_btag_id > bTag_wp;
        return jets_btag_pass;
    };

    auto calcNBTag = [&](RVec<float> jets_btag_id, RVec<bool> jets_btag_pass) {
        return (int)jets_btag_id[jets_btag_pass].size();
    };


    // Need these to not segfault in case collection is empty
    // Also, RDataFrame.Histo1D() doesn't accept indexed vectors, so need to rename
    // e.g.: reco_PF_jet_pt[0] --> reco_PF_jet_pt0
    auto takeQuantity0 = [&](RVec<float> quant_vec) { return quant_vec.size() > 0 ? quant_vec[0] : -9999.f; };
    auto takeQuantity1 = [&](RVec<float> quant_vec) { return quant_vec.size() > 1 ? quant_vec[1] : -9999.f; };

    auto calcMETJetDphi = [&](RVec<float> jets_phi, float MET_phi) { 
        if (jets_phi.size() < 1)
            return RVec<float>{};
        return abs(DeltaPhi(jets_phi, MET_phi));
    };
    // Calc dphi between MET and jets except for leading one
    auto calcMETJetDphiN = [&](RVec<float> jets_phi, float MET_phi) {
        if (jets_phi.size() < 2)
            return RVec<float>{};
        auto jets_N_phi = Take(jets_phi, -(jets_phi.size()-1)); // take all but first element
        return abs(DeltaPhi(jets_N_phi, MET_phi));
    };
    auto calcMuonJetdR = [&](float eta, float phi, RVec<float> jets_eta, RVec<float> jets_phi) {
        if (jets_phi.size() < 1)
            return RVec<float>{};
        auto muon_vec_eta = RVec(jets_eta.size(), eta);
        auto muon_vec_phi = RVec(jets_phi.size(), phi);
        return DeltaR(muon_vec_eta, jets_eta, muon_vec_phi, jets_phi);
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

    auto correct_EE_MET_pt = [&](float MET_pt,float MET_phi,float EE_px,float EE_py){
        float return_pt = MET_pt;
        if(year_==2017){
           float corr_px = MET_pt*cos(MET_phi) + EE_px;
           float corr_py = MET_pt*sin(MET_phi) + EE_py;
           return_pt = sqrt(corr_px*corr_px + corr_py*corr_py);
        }
        return return_pt;

    };

    auto correct_EE_MET_phi = [&](float MET_pt,float MET_phi,float EE_px,float EE_py){
        float return_phi = MET_phi;
        if(year_==2017){
           float corr_px = MET_pt*cos(MET_phi) + EE_px;
           float corr_py = MET_pt*sin(MET_phi) + EE_py;
           return_phi =  atan2(corr_py,corr_px);
        }
        return return_phi;
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
    auto calcVtxResolution = [&](float reco_vtx_vxy, float reco_vtx_sigmavxy) { return reco_vtx_sigmavxy/reco_vtx_vxy; };

    auto passDisplacedID = [&](RVec<float> trk_n_planes, RVec<float> trk_n_hits, RVec<float> trk_n_csc_hits, 
                                RVec<float> trk_chi2, RVec<float> pt, RVec<float> eta, RVec<float> pt_err) {
        RVec<bool> pass = (trk_n_planes > 1) && 
                          ((trk_n_hits > 12 && trk_n_csc_hits > 0) || (trk_n_hits > 18 && trk_n_csc_hits == 0)) && 
                          (trk_chi2 < 2.5) &&
                          (pt > 5) && 
                          (abs(eta) < 2.4) && 
                          (pt_err/pt < 1);
        return pass;
    };

    auto passLooseID = [&](RVec<bool> isPF, RVec<float> pt, RVec<float> eta) {
        RVec<bool> pass = (isPF == 1) && (pt > 5) && (abs(eta) < 2.4);
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
   
    auto goodQuantity = [&](RVec<float> quantity, RVec<bool> pass){
      return quantity[pass];
    };

    auto takegoodeta = [&](RVec<float> quantity, RVec<float> q2){
      bool good = true;
      for (size_t i=0; i < quantity.size() ; i++) {
        if (q2[i] < 30)
            continue;
        if (abs(quantity[i]) > 2.4)
            good = false;
      }
      return good;
    };

    // for each dsa muon in best vtx, look for *best* gm match in dr
    auto findMuonMatch0 = [&](RVec<int> dsagm_match, RVec<float> dsagm_dR, RVec<bool> vtx_pass_dsagm, size_t dsa_index_0, size_t dsa_index_1) {
        size_t best_index = dsa_index_0; // if no match, just return dsa index
        float min_dR = 999999.f;
        for (size_t j = 0; j < 4; j++) {
            // max dR threshold is now configurable (default 0.2), instead of checking (dsagm_match[4 * dsa_index_0 + j] == 1)
            if (abs(dsagm_dR[4 * dsa_index_0 + j]) < maxDrGMdSA_ && abs(dsagm_dR[4 * dsa_index_0 + j]) < min_dR && vtx_pass_dsagm[4 * dsa_index_1 + j] == 1) {
                min_dR = abs(dsagm_dR[4 * dsa_index_0 + j]);
                best_index = 4 + j; // make gm index offset by 4
            }
        }
        return best_index;
    };

    auto findMuonMatch1 = [&](RVec<int> dsagm_match, RVec<float> dsagm_dR, RVec<bool> vtx_pass_dsagm, RVec<bool> vtx_pass_gmgm, size_t dsa_index_1, size_t best_muon_0) {
        size_t best_index = dsa_index_1; // if no match, just return dsa index
        float min_dR = 999999.f;
        for (size_t j = 0; j < 4; j++) {
            if (best_muon_0 > 3) { // a match was found for first dsa, check gmgm collections, also make sure same gm is not selected again
                // max dR threshold is now configurable (default 0.2), instead of checking (dsagm_match[4 * dsa_index_1 + j] == 1)
                if (abs(dsagm_dR[4 * dsa_index_1 + j]) < maxDrGMdSA_ && abs(dsagm_dR[4 * dsa_index_1 + j]) < min_dR & vtx_pass_gmgm[4 * (best_muon_0-4) + j] == 1 && 4 + j != best_muon_0 ) {
                    min_dR = abs(dsagm_dR[4 * dsa_index_1 + j]);
                    best_index = 4 + j; // make gm index offset by 4
                }
            }
            else { // no match was found for first dsa, check dsagm collections
                // max dR threshold is now configurable (default 0.2), instead of checking (dsagm_match[4 * dsa_index_1 + j] == 1)
                if (abs(dsagm_dR[4 * dsa_index_1 + j]) < maxDrGMdSA_ && abs(dsagm_dR[4 * dsa_index_1 + j]) < min_dR && vtx_pass_dsagm[4 * best_muon_0 + j] == 1) {
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

    auto calcCosAlpha = [&](RVec<float> mu_pt, RVec<float> mu_eta, RVec<float> mu_phi, RVec<bool> muon_pass) {
        // Calculate cos_alpha for all combinations of muons in the event (up to 4).
        // The default impossible value is -10k, so e.g. diagonal elements in the
        // 4x4 matrix (refer to the same muon) have that value, or if there are
        // fewer muons than 4 in the dSA collection, the remaining elements also
        // get -10k. Also require that the tag muon (the row in the matrix) passes
        // the tight (tag) ID
        RVec<float> vtx_cosalpha(16, 10000);
        auto comb_idx = Combinations(4, 4);
        for (size_t i = 0; i < comb_idx[0].size(); i++) {
            size_t idx0 = comb_idx[0][i];
            size_t idx1 = comb_idx[1][i];
            if (idx0 == idx1) continue;
            if (idx0 >= muon_pass.size() || idx1 >= muon_pass.size())
                continue;
            if (!muon_pass[idx0]) continue;
            float mu1_pt = mu_pt[idx0];
            float mu1_eta = mu_eta[idx0];
            float mu1_phi = mu_phi[idx0];
            float mu2_pt = mu_pt[idx1];
            float mu2_eta = mu_eta[idx1];
            float mu2_phi = mu_phi[idx1];
            float mu1_px = mu1_pt * cos(mu1_phi);
            float mu1_py = mu1_pt * sin(mu1_phi);
            float mu1_pz = mu1_pt * sinh(mu1_eta);
            float mu2_px = mu2_pt * cos(mu2_phi);
            float mu2_py = mu2_pt * sin(mu2_phi);
            float mu2_pz = mu2_pt * sinh(mu2_eta);
            float dot_product = mu1_px*mu2_px + mu1_py*mu2_py + mu1_pz*mu2_pz;
            float mu1_p = sqrt(mu1_px*mu1_px + mu1_py*mu1_py + mu1_pz*mu1_pz);
            float mu2_p = sqrt(mu2_px*mu2_px + mu2_py*mu2_py + mu2_pz*mu2_pz);
            vtx_cosalpha[i] = dot_product / (mu1_p * mu2_p);
        }
        return vtx_cosalpha;
    };

    // RDF does not accept templated or overloaded functions...
    auto takeSelectedMuonQuantityFloat = [&](RVec<float> quant_dsa, RVec<float> quant_gm, size_t best_muon) {
        if (quant_dsa.size() == 0) return -9999.f;
        return (best_muon > 3) ? quant_gm[best_muon-4] : quant_dsa[best_muon];
    };
    auto takeSelectedMuonQuantityInt = [&](RVec<int> quant_dsa, RVec<int> quant_gm, size_t best_muon) {
        if (quant_dsa.size() == 0) return -9999;
        return (best_muon > 3) ? quant_gm[best_muon-4] : quant_dsa[best_muon];
    };
    
    auto takeUnmatchedDSAQuantityFloat = [&](RVec<float> quant_dsa, size_t dsa_index_0, size_t best_muon_0, size_t dsa_index_1, size_t best_muon_1) {
        if (quant_dsa.size() == 0) return quant_dsa;
        RVec<float> unmatched{};
        if (dsa_index_0 == best_muon_0)
            unmatched.push_back(quant_dsa[dsa_index_0]);
        if (dsa_index_1 == best_muon_1)
            unmatched.push_back(quant_dsa[dsa_index_1]);
        return unmatched;
    };

    auto takeMatchedDSAQuantityFloat = [&](RVec<float> quant_dsa, size_t dsa_index_0, size_t best_muon_0, size_t dsa_index_1, size_t best_muon_1) {
        if (quant_dsa.size() == 0) return quant_dsa;
        RVec<float> matched{};
        if (dsa_index_0 != best_muon_0)
            matched.push_back(quant_dsa[dsa_index_0]);
        if (dsa_index_1 != best_muon_1)
            matched.push_back(quant_dsa[dsa_index_1]);
        return matched;
    };
    auto takeMatchedGMQuantityFloat = [&](RVec<float> quant_gm, size_t dsa_index_0, size_t best_muon_0, size_t dsa_index_1, size_t best_muon_1) {
        if (quant_gm.size() == 0) return quant_gm;
        RVec<float> matched{};
        if (dsa_index_0 != best_muon_0 && best_muon_0 > 3)
            matched.push_back(quant_gm[best_muon_0-4]);
        if (dsa_index_1 != best_muon_1 && best_muon_1 > 3)
            matched.push_back(quant_gm[best_muon_1-4]);
        return matched;
    };
    auto FromScalarToRVec = [&](float quantity, int len) {
        return RVec<float>(len, quantity);
    };
    auto calcNUnmatched = [&](RVec<float> unmatched_dsa_dxy, int n_matched) {
        cout << "size of unmatched dsa dxy: " << unmatched_dsa_dxy.size() << " , n_matched: " << n_matched << endl;
        if (unmatched_dsa_dxy.size() < 2)
            return (int)unmatched_dsa_dxy.size() - n_matched;
        return 2 - n_matched;
    };

    auto selectMuonWithMinimumDxy = [&](float dxy0, float dxy1, size_t best_muon_0, size_t best_muon_1) {
        if (abs(dxy0) < abs(dxy1))
            return best_muon_0;
        return best_muon_1;
    };

    auto muonMinDxyPassLoosePFIso = [&](RVec<float> reco_gm_iso, size_t mindxy_muon) {
        if (mindxy_muon < 4)
            return -1.f;
        return (float)(reco_gm_iso[mindxy_muon-4] < 0.25);
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
    TString MET_pt_corr_noEE = "reco_PF_MET_corr_pt";
    TString MET_phi_corr_noEE = "reco_PF_MET_corr_phi";
    std::string jet_syst = "";
    switch (syst_) {
        case ScaleFactors::JES_UP:
            jet_syst = "JESUp";
            break;
        case ScaleFactors::JES_DOWN:
            jet_syst = "JESDown";
            break;
        case ScaleFactors::JER_UP:
            jet_syst = "JERUp";
            break;
        case ScaleFactors::JER_DOWN:
            jet_syst = "JERDown";
    }
    if (jet_syst != "") {
        jet_pt = Form("reco_PF_jet_corr_%s_pt", jet_syst.c_str());
        jet_eta = Form("reco_PF_jet_corr_%s_eta", jet_syst.c_str());
        jet_phi = Form("reco_PF_jet_corr_%s_phi", jet_syst.c_str());
        MET_pt_corr_noEE = Form("reco_PF_MET_corr_%s_pt", jet_syst.c_str());
        MET_phi_corr_noEE = Form("reco_PF_MET_corr_%s_phi", jet_syst.c_str());
    }

    auto df_wgts = df.
        Define("MET_pt", correct_EE_MET_pt,{MET_pt_corr_noEE.Data(),MET_phi_corr_noEE.Data(),"reco_PF_MET_EE_delta_px","reco_PF_MET_EE_delta_py"}).
        Define("MET_phi", correct_EE_MET_phi,{MET_pt_corr_noEE.Data(),MET_phi_corr_noEE.Data(),"reco_PF_MET_EE_delta_px","reco_PF_MET_EE_delta_py"}).
        Define("reco_dsa_pt_res", "reco_dsa_pt_err/reco_dsa_pt").
        Define("reco_gm_pt_res", "reco_gm_pt_err/reco_gm_pt").
        Define("dsa_pass_ID", passDisplacedID, {"reco_dsa_trk_n_planes", "reco_dsa_trk_n_hits", "reco_dsa_trk_n_CSC_hits", "reco_dsa_trk_chi2", "reco_dsa_pt", "reco_dsa_eta", "reco_dsa_pt_err"}).
        Define("gm_pass_ID", passLooseID, {"reco_gm_isPF", "reco_gm_pt", "reco_gm_eta"}).
        Define("reco_pass_dsa_pt", goodQuantity, {"reco_dsa_pt","dsa_pass_ID"}).
        Define("reco_pass_dsa_eta", goodQuantity, {"reco_dsa_eta","dsa_pass_ID"}).
        Define("reco_pass_dsa_phi", goodQuantity, {"reco_dsa_phi","dsa_pass_ID"}).
        Define("reco_pass_gm_pt", goodQuantity, {"reco_gm_pt","gm_pass_ID"}).
        Define("reco_pass_gm_eta", goodQuantity, {"reco_gm_eta","gm_pass_ID"}).
        Define("reco_pass_gm_phi", goodQuantity, {"reco_gm_phi","gm_pass_ID"}).
        Define("n_good_dsa", "(int)Nonzero(dsa_pass_ID).size()").
        Define("n_good_gm", "(int)Nonzero(gm_pass_ID).size()").
        Define("good_vtx_dsadsa", passVtxID, {"dsa_pass_ID", "dsa_pass_ID", "reco_dsa_charge", "reco_dsa_charge", "reco_vtx_dsadsa_reduced_chi2"}).
        Define("good_vtx_gmgm", passVtxID, {"gm_pass_ID", "gm_pass_ID", "reco_gm_charge", "reco_gm_charge", "reco_vtx_gmgm_reduced_chi2"}).
        Define("good_vtx_dsagm", passVtxID, {"dsa_pass_ID", "gm_pass_ID", "reco_dsa_charge", "reco_gm_charge", "reco_vtx_dsagm_reduced_chi2"}).
        Define("best_vtx_dsadsa", pickBestVtx, {"reco_vtx_dsadsa_reduced_chi2", "good_vtx_dsadsa"}).
        Define("best_dsa_0", "best_vtx_dsadsa / 4").
        Define("best_dsa_1", "best_vtx_dsadsa % 4").
        Define("cosalphas", calcCosAlpha, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_phi", "dsa_pass_ID"}).
        Define("min_cosalpha", "Min(cosalphas)").//[best_dsa_0*4 + best_dsa_1]").
        Define("reco_dsa_pt_res0", "reco_dsa_pt_res.size() > 0 ? reco_dsa_pt_res[best_dsa_0] : -9999.f").
        Define("reco_dsa_pt_res1", "reco_dsa_pt_res.size() > 1 ? reco_dsa_pt_res[best_dsa_1] : -9999.f").
        Define("best_muon_0", findMuonMatch0, {"reco_gbmdsa_match", "reco_gbmdsa_dR", "good_vtx_dsagm", "best_dsa_0", "best_dsa_1"}).
        Define("best_muon_1", findMuonMatch1, {"reco_gbmdsa_match", "reco_gbmdsa_dR", "good_vtx_dsagm", "good_vtx_gmgm", "best_dsa_1", "best_muon_0"}).
        Define("n_matched", "(int)(best_muon_0>3) + (int)(best_muon_1>3)").
        Define("matched_dsa_pt", takeMatchedDSAQuantityFloat, {"reco_dsa_pt", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_pt", takeMatchedGMQuantityFloat, {"reco_gm_pt", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_dsa_eta", takeMatchedDSAQuantityFloat, {"reco_dsa_eta", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_eta", takeMatchedGMQuantityFloat, {"reco_gm_eta", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_dsa_phi", takeMatchedDSAQuantityFloat, {"reco_dsa_phi", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_phi", takeMatchedGMQuantityFloat, {"reco_gm_phi", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_dsa_trk_chi2", takeMatchedDSAQuantityFloat, {"reco_dsa_trk_chi2", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_trk_chi2", takeMatchedGMQuantityFloat, {"reco_gm_trk_chi2", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_dsa_dxy", takeMatchedDSAQuantityFloat, {"reco_dsa_dxy", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_dxy", takeMatchedGMQuantityFloat, {"reco_gm_dxy", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("unmatched_dsa_dxy", takeUnmatchedDSAQuantityFloat, {"reco_dsa_dxy", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("n_unmatched", "(int)unmatched_dsa_dxy.size()"). //calcNUnmatched, {"unmatched_dsa_dxy", "n_matched"}).
        Define("matched_dsa_dxy_err", takeMatchedDSAQuantityFloat, {"reco_dsa_dxy_err", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_gm_dxy_err", takeMatchedGMQuantityFloat, {"reco_gm_dxy_err", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("unmatched_dsa_dxy_err", takeUnmatchedDSAQuantityFloat, {"reco_dsa_dxy_err", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_dsa_dxy_res", "matched_dsa_dxy_err/matched_dsa_dxy").
        Define("matched_gm_dxy_res", "matched_gm_dxy_err/matched_gm_dxy").
        Define("unmatched_dsa_dxy_res", "unmatched_dsa_dxy_err/unmatched_dsa_dxy").
        Define("matched_gm_PFIso", takeMatchedGMQuantityFloat, {"reco_gm_PFIso", "best_dsa_0", "best_muon_0", "best_dsa_1", "best_muon_1"}).
        Define("matched_muon_vtx_vxy", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_vxy", "reco_vtx_gmgm_vxy", "reco_vtx_dsagm_vxy", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_vxy_RVec_m", FromScalarToRVec, {"matched_muon_vtx_vxy", "n_matched"}). // workaround for RDF not supporting scalar vs RVec 2D plots
        Define("matched_muon_vtx_vxy_RVec_unm", FromScalarToRVec, {"matched_muon_vtx_vxy", "n_unmatched"}). // workaround for RDF not supporting scalar vs RVec 2D plots
        Define("matched_muon_vtx_vz", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_vz", "reco_vtx_gmgm_vz", "reco_vtx_dsagm_vz", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_sigmavxy", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_sigmavxy", "reco_vtx_gmgm_sigmavxy", "reco_vtx_dsagm_sigmavxy", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_reduced_chi2", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_reduced_chi2", "reco_vtx_gmgm_reduced_chi2", "reco_vtx_dsagm_reduced_chi2", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_dR", takeMatchedVtxQuantity, {"reco_vtx_dsadsa_dR", "reco_vtx_gmgm_dR", "reco_vtx_dsagm_dR", "best_muon_0", "best_muon_1"}).
        Define("matched_muon_vtx_sign", calcVtxSignificance, {"matched_muon_vtx_vxy", "matched_muon_vtx_sigmavxy"}).
        Define("matched_muon_vtx_res", calcVtxResolution, {"matched_muon_vtx_vxy", "matched_muon_vtx_sigmavxy"}).
        Define("reco_sel_mu_pt0", takeSelectedMuonQuantityFloat, {"reco_dsa_pt", "reco_gm_pt", "best_muon_0"}).
        Define("reco_sel_mu_pt1", takeSelectedMuonQuantityFloat, {"reco_dsa_pt", "reco_gm_pt", "best_muon_1"}).
        Define("reco_sel_mu_eta0", takeSelectedMuonQuantityFloat, {"reco_dsa_eta", "reco_gm_eta", "best_muon_0"}).
        Define("reco_sel_mu_eta1", takeSelectedMuonQuantityFloat, {"reco_dsa_eta", "reco_gm_eta", "best_muon_1"}).
        Define("reco_sel_mu_phi0", takeSelectedMuonQuantityFloat, {"reco_dsa_phi", "reco_gm_phi", "best_muon_0"}).
        Define("reco_sel_mu_phi1", takeSelectedMuonQuantityFloat, {"reco_dsa_phi", "reco_gm_phi", "best_muon_1"}).
        Define("reco_sel_pair_deta", "(float)abs(reco_sel_mu_eta1 - reco_sel_mu_eta0)").
        Define("reco_sel_pair_dphi", "(float)abs(reco_sel_mu_phi1 - reco_sel_mu_phi0)").
        Define("reco_sel_pair_reduced_dphi", "(float)abs(reco_sel_pair_dphi - 3.141592 * (1 + (reco_sel_pair_dphi - 3.141592) / abs(reco_sel_pair_dphi - 3.141592)))").
        Define("reco_sel_pair_cos_dEtaOdPhi", "(float)cos(reco_sel_pair_deta/reco_sel_pair_reduced_dphi)").
        Define("reco_sel_pair_log_dEtaOdPhi", "(float)log(reco_sel_pair_deta/reco_sel_pair_reduced_dphi)").
        Define("reco_sel_mu_dxy0", takeSelectedMuonQuantityFloat, {"reco_dsa_dxy", "reco_gm_dxy", "best_muon_0"}).
        Define("reco_sel_mu_dxy1", takeSelectedMuonQuantityFloat, {"reco_dsa_dxy", "reco_gm_dxy", "best_muon_1"}).
        Define("reco_sel_mu_mindxy_idx", selectMuonWithMinimumDxy, {"reco_sel_mu_dxy0", "reco_sel_mu_dxy1", "best_muon_0", "best_muon_1"}).
        Define("reco_sel_mu_mindxy_passPFIso", muonMinDxyPassLoosePFIso, {"reco_gm_PFIso", "reco_sel_mu_mindxy_idx"}).
        Define("reco_sel_mu_dxy_avg", "(float)(0.5*(abs(reco_sel_mu_dxy0) + abs(reco_sel_mu_dxy1)))").
        Define("reco_sel_mu_dxy_min", "(float)(min(abs(reco_sel_mu_dxy0), abs(reco_sel_mu_dxy1)))").
        Define("reco_sel_mu_dxy_max", "(float)(max(abs(reco_sel_mu_dxy0), abs(reco_sel_mu_dxy1)))").
        Define("reco_sel_mu_dxy_err0", takeSelectedMuonQuantityFloat, {"reco_dsa_dxy_err", "reco_gm_dxy_err", "best_muon_0"}).
        Define("reco_sel_mu_dxy_err1", takeSelectedMuonQuantityFloat, {"reco_dsa_dxy_err", "reco_gm_dxy_err", "best_muon_1"}).
        Define("reco_sel_mu_dxy_sign0", "reco_sel_mu_dxy0/reco_sel_mu_dxy_err0").
        Define("reco_sel_mu_dxy_sign1", "reco_sel_mu_dxy1/reco_sel_mu_dxy_err1").
        Define("reco_sel_mu_dz0", takeSelectedMuonQuantityFloat, {"reco_dsa_dz", "reco_gm_dz", "best_muon_0"}).
        Define("reco_sel_mu_dz1", takeSelectedMuonQuantityFloat, {"reco_dsa_dz", "reco_gm_dz", "best_muon_1"}).
        Define("reco_sel_mu_q0", takeSelectedMuonQuantityInt, {"reco_dsa_charge", "reco_gm_charge", "best_muon_0"}).
        Define("reco_sel_mu_q1", takeSelectedMuonQuantityInt, {"reco_dsa_charge", "reco_gm_charge", "best_muon_1"}).
        Define("reco_dsa_pt_sign", "reco_dsa_pt/reco_dsa_pt_err").
        Define("reco_gm_pt_sign", "reco_gm_pt/reco_gm_pt_err").
        Define("reco_sel_mu_pt_sign0", takeSelectedMuonQuantityFloat, {"reco_dsa_pt_sign", "reco_gm_pt_sign", "best_muon_0"}).
        Define("reco_sel_mu_pt_sign1", takeSelectedMuonQuantityFloat, {"reco_dsa_pt_sign", "reco_gm_pt_sign", "best_muon_1"}).
        Define("reco_sel_mu_pt_res0", takeSelectedMuonQuantityFloat, {"reco_dsa_pt_res", "reco_gm_pt_res", "best_muon_0"}).
        Define("reco_sel_mu_pt_res1", takeSelectedMuonQuantityFloat, {"reco_dsa_pt_res", "reco_gm_pt_res", "best_muon_1"}).
        Define("matched_muon_MT", calcMatchedMuonMT, {"reco_sel_mu_pt0", "reco_sel_mu_phi0", "MET_pt", "MET_phi"}).
        Define("matched_muon_Mmumu", calcMatchedMuonInvMass, {"reco_sel_mu_pt0", "reco_sel_mu_eta0", "reco_sel_mu_phi0", "reco_sel_mu_pt1", "reco_sel_mu_eta1", "reco_sel_mu_phi1"}).
        Define("n_highpt_corr_jets", Form("(int)%s.size()", jet_pt.Data())).
        Define("reco_PF_jet_pt0", takeQuantity0, {jet_pt.Data()}).
        Define("reco_PF_jet_pt1", takeQuantity1, {jet_pt.Data()}).
        Define("reco_PF_jet_eta0", takeQuantity0, {jet_eta.Data()}).
        Define("reco_PF_jet_eta1", takeQuantity1, {jet_eta.Data()}).
        Define("good_eta", takegoodeta, {jet_eta.Data(), jet_pt.Data()}).
        Define("reco_PF_jet_phi0", takeQuantity0, {jet_phi.Data()}).
        Define("reco_PF_jet_phi1", takeQuantity1, {jet_phi.Data()}).
        Define("reco_dsa0_trk_chi2", takeQuantity0, {"reco_dsa_trk_chi2"}).
        Define("reco_dsa1_trk_chi2", takeQuantity1, {"reco_dsa_trk_chi2"}).
        Define("reco_dsa_muon_pt0", takeQuantity0, {"reco_dsa_pt"}).
        Define("reco_dsa_muon_eta0", takeQuantity0, {"reco_dsa_eta"}).
        Define("reco_dsa_trk_n_hits0", takeQuantity0, {"reco_dsa_trk_n_hits"}).
        Define("reco_dsa_trk_n_planes0", takeQuantity0, {"reco_dsa_trk_n_planes"}).
        Define("reco_gm_muon_pt0", takeQuantity0, {"reco_pass_gm_pt"}).
        Define("reco_gm_muon_eta0", takeQuantity0, {"reco_pass_gm_eta"}).
        Define("MET_jet_dphi", calcMETJetDphi, {jet_phi.Data(), "MET_phi"}).
        Define("MET_jet_dphi0", takeQuantity0, {"MET_jet_dphi"}).
        Define("MET_jet_dphiN", calcMETJetDphiN, {jet_phi.Data(), "MET_phi"}).
        Define("recoil_jet_phi_dphi", calcMETJetDphi, {jet_phi.Data(), "reco_PF_recoil_phi"}).
        Define("muon0_jet_dR", calcMuonJetdR, {"reco_sel_mu_eta0", "reco_sel_mu_phi0", jet_eta.Data(), jet_phi.Data()}).
        Define("muon1_jet_dR", calcMuonJetdR, {"reco_sel_mu_eta1", "reco_sel_mu_phi1", jet_eta.Data(), jet_phi.Data()}).
        Define("muon0_jet0_dR", takeQuantity0, {"muon0_jet_dR"}).
        Define("muon1_jet0_dR", takeQuantity0, {"muon1_jet_dR"}).
        Define("fake_MET_fraction", findFakeMETCut, {"MET_pt", "MET_phi", "reco_Calo_MET_pt", "reco_Calo_MET_phi", "reco_Calo_MET_pt"}).
        Define("reco_PF_MetNoMu_pt", findMetNoMu, {"MET_pt", "MET_phi", "reco_pass_gm_pt", "reco_pass_gm_phi"}).
        Define("hem_veto", calcHemVeto, {"reco_PF_HEM_flag"}).
        Define("reco_bTag_pass", calcBTagPass, {"reco_PF_jet_corr_BTag"}).
        Define("reco_n_bTag_jets", calcNBTag, {"reco_PF_jet_corr_BTag", "reco_bTag_pass"}).
        Define("reco_METmu_dphi_v2", calcRecoMETmuDphi, {"reco_sel_mu_pt0", "reco_sel_mu_phi0", "reco_sel_mu_pt1", "reco_sel_mu_phi1", "MET_phi"}).
        Define("CaloPFMETRatio", Form("abs(reco_Calo_MET_pt - %s)/reco_Calo_MET_pt", "MET_pt"));


    if (mode_ == common::DATA) 
        df_wgts = df_wgts.Define("wgt", "1.0");
    else {
        df_wgts = df_wgts.
            Define("gen_muon_vxy", takeGenMuVxy, {"gen_vxy", "gen_ID"}).
            Define("gen_METmu_dphi", calcGenMETmuDphi, {"gen_pt", "gen_phi", "gen_ID", "gen_MET_phi"}).
            Define("Zwgt", calcZWgt, {"gen_ID", "gen_pt"}).
            Define("Wwgt", calcWWgt, {"gen_ID", "gen_pt"}).
            Define("Twgt", calcTWgt, {"gen_ID", "gen_pt"}).
            Define("PUwgt", calcPUWgt, {"gen_pu_true"}).
            Define("trig_wgt", calcTrigWgt, {"reco_PF_MetNoMu_pt"}).
            Define("veto_wgt", calcVetoWgt, {"reco_electron_id_result", "reco_electron_eta", "reco_electron_pt", 
                                            "reco_photon_id_result", "reco_photon_eta", "reco_photon_pt"}).
            Define("dsa_ID_wgt_0", calcDisplacedIDWgt, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_dxy", "best_dsa_0"}).
            Define("dsa_ID_wgt_1", calcDisplacedIDWgt, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_dxy", "best_dsa_1"}).
            Define("dsa_ID_wgt", "dsa_ID_wgt_0 * dsa_ID_wgt_1").
            Define("dsa_reco_wgt_0", calcDSARecoWgt, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_dxy", "best_dsa_0"}).
            Define("dsa_reco_wgt_1", calcDSARecoWgt, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_dxy", "best_dsa_1"}).
            Define("dsa_reco_wgt", "dsa_reco_wgt_0 * dsa_reco_wgt_1").
            Define("gm_wgt_0", calcGMWgt, {"reco_gm_pt", "reco_gm_eta", "best_muon_0"}).
            Define("gm_wgt_1", calcGMWgt, {"reco_gm_pt", "reco_gm_eta", "best_muon_1"}).
            Define("gm_wgt", "gm_wgt_0 * gm_wgt_1").
            Define("bTag_wgt", calcBTagWgt, {"reco_n_bTag_jets", jet_pt.Data(), jet_eta.Data(), "reco_bTag_pass"}).
            Define("wgt", calcTotalWgt, {"veto_wgt", "dsa_reco_wgt", "dsa_ID_wgt", "gm_wgt", "Zwgt", 
                                            "Wwgt", "Twgt", "PUwgt", "trig_wgt", "gen_wgt", "bTag_wgt"});
            //Define("wgt", "1.0");
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

        if (cuts_info_[cut].print_events == TString("yes")) {
            auto disp = df_filters.Display({"run_num", "lumi_sec", "event_num", "matched_muon_vtx_vxy"}, 20);
            disp->Print();
        }

        // make all requested histograms for each cut, IFF book_plot is true for that cut
        if (cuts_info_[cut].book_plot == TString("no")) {
            // before continue restore df if it's one of the 3 SR definitions (or a otherwise non-inclusive cut)
            if (cuts_info_[cut].inclusive == TString("no")) 
                df_filters = temp_df;
            continue;
        }
        // if (cut == 22)
            // df_filters.Snapshot("ntuples_gbm/recoT", Form("snapshot_Cut%d_allcols.root", cut));

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
            else if (histo_info->type == "vec_float1D") {
                // std::map auto-initializes if no element found
                if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                    double bin_edges[histo_info->binEdgesX.size()];
                    std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<RVec<float>,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data(), "wgt");
                }
                else { // just number of bins and low and high X
                    all_histos_1D_[histo_name][cut] = df_filters.Histo1D<RVec<float>,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
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
            else if (histo_info->type == "vec_float2D")
                all_histos_2D_[histo_name][cut] = df_filters.Histo2D<RVec<float>,RVec<float>,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");

            else if (histo_info->type == "int2D")
                all_histos_2D_[histo_name][cut] = df_filters.Histo2D<int,int,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");
            else
                cout << "Hist type not recognized!" << endl;
        }

        // restore df if it's one of the 3 SR definitions (or a otherwise non-inclusive cut)
        if (cuts_info_[cut].inclusive == "no") 
            df_filters = temp_df;
    }

    TString wgt = (mode_ != common::DATA) ? "gen_wgt" : "reco_PF_MET_corr_phi";

    auto df_sumgenwgts = df_wgts.Sum(wgt.Data());

    if (mode_ != common::SIGNAL)
        df_sumgenwgts.OnPartialResult(everyN, [&barWidth, &progressBar/*, &barMutex*/](double &) {
                //std::lock_guard<std::mutex> l(barMutex); // lock_guard locks the mutex at construction, releases it at destruction
                progressBar.push_back('#');
                // re-print the line with the progress bar
                std::cout << "\r[" << std::left << std::setw(barWidth) << progressBar << ']' << std::flush;
                });

    cout << "Triggering event loop..." << endl;
    auto value = *df_sumgenwgts;
    cout << endl << "RDF sum_gen_wgts (dummy MET phi if running on data): " << value << endl;

    df_filters.Report()->Print();

    return kTRUE;
}

Bool_t RDFAnalysis::Terminate() {    
    return true;
}