#include "nMinus1Selector.h"
#include <TH2.h>
#include <TStyle.h>

using std::vector;

void nMinus1Selector::SetCuts(vector<common::CutInfo> cuts_info) {
    cuts_info_ = cuts_info;
}

void nMinus1Selector::SetParams(common::SampleInfo sample_info, Double_t lumi) {
    sample_info_ = sample_info;
    mode_ = sample_info_.mode;
    group_ = sample_info_.group;
    sum_gen_wgt_ = sample_info_.sum_gen_wgt;
    xsec_ = sample_info_.xsec;
    lumi_ = lumi;
    std::cout << "sum_gen_wgt: " << sum_gen_wgt_ << ", xsec: " << xsec_ << " [pb], lumi: " << lumi_ << " [1/pb] " << std::endl;

    // Set up QCD and EWK corrections
    TFile * kfactors = new TFile("../data/kfactors.root");
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
    
    // Set up pileup corrections
    TFile * pileup = new TFile("../data/puWeights_10x_56ifb.root");
    TH1F * h_pu = (TH1F*)pileup->Get("puWeights");
    sf_pu = (TH1F*)h_pu->Clone();
    sf_pu->SetDirectory(0);
    pileup->Close();

}

void nMinus1Selector::Begin() {
   all_histos_1D_.clear();
   all_histos_2D_.clear();
}

Bool_t nMinus1Selector::Process(TChain * chain) {

    if (sum_gen_wgt_ < 0.1 && mode_ != common::DATA) return kFALSE;

    all_histos_1D_.clear();
    all_histos_2D_.clear();
    chain_ = chain;

    ROOT::EnableImplicitMT();
    //chain->SetCacheSize(0);
    ROOT::RDataFrame df(*chain_);

    // First, define all the relevant weights
    auto calcZsf = [&](vector<int> gen_ID, vector<float> gen_pt) { 
        if (group_ != "ZJets")
            return 1.0f;
        if (gen_pt.size() != gen_ID.size())
            return 1.0f;
        float Zpt = -1.0f; 
        for (int i = 0; i < gen_ID.size(); i++) { 
            if (abs(gen_ID[i]) == 23) 
                Zpt = gen_pt[i]; 
        } 
        if (Zpt < 0)
            return 1.0f;
        int binNum = sf_z_qcd->FindBin(Zpt);
        if (binNum == 0)
            binNum = 1;
        if (binNum == sf_z_qcd->GetNbinsX())
            binNum = sf_z_qcd->GetNbinsX()-1;
        float sf = (float)sf_z_qcd->GetBinContent(binNum) * (float)sf_z_ewk->GetBinContent(binNum);
        return 1.0f; 
    };
    auto calcWsf = [&](vector<int> gen_ID, vector<float> gen_pt) { 
        if (group_ != "WJets")
            return 1.0f;
        if (gen_pt.size() != gen_ID.size())
            return 1.0f;
        float Wpt = -1.0f; 
        for (int i = 0; i < gen_ID.size(); i++) { 
            if (abs(gen_ID[i]) == 24) 
                Wpt = gen_pt[i]; 
        } 
        if (Wpt < 0.0)
            return 1.0f;
        int binNum = sf_w_qcd->FindBin(Wpt);
        if (binNum == 0)
            binNum = 1;
        else if (binNum == sf_w_qcd->GetNbinsX())
            binNum = sf_w_qcd->GetNbinsX()-1;
        float sf = (float)sf_w_qcd->GetBinContent(binNum) * (float)sf_w_ewk->GetBinContent(binNum);
        // scale down WJets MC by 15% to match with data
        sf *= 1.0;
        return sf; 
    };
    auto calcTsf = [&](vector<int> gen_ID, vector<float> gen_pt) { 
        if (group_ != "Top")
            return 1.0f;
        if (gen_pt.size() != gen_ID.size())
            return 1.0f;
        float sf_top = 1.0f;
        int n_top = 0;
        for (int i = 0; i < gen_ID.size(); i++) {
            if (abs(gen_ID[i]) == 6) {
                if (gen_pt[i] > 0 && gen_pt[i] < 800)
                    sf_top *= exp(0.0615 - 0.0005 * gen_pt[i]);
                else if (gen_pt[i] > 0)
                    sf_top *= exp(0.0615 - 0.0005 * 800);
                else
                    sf_top *= exp(0.0615 - 0.0005 * 0);
                //sf_top *= gen_pt[i];
                n_top++;
            }
        }
        if (n_top == 2)
            sf_top = powf(sf_top, 1/n_top);
        else
            sf_top = 1.0f;
        return sf_top;
   };
   auto calcPUsf = [&](int pileup) { 
       return (float)sf_pu->GetBinContent(sf_pu->FindBin((double)pileup)); 
   };
   auto calcTotalWgt = [&](float Zwgt, float Wwgt, float Twgt, float PUwgt, float genwgt) {
       return Zwgt * Wwgt * PUwgt * genwgt * xsec_ * lumi_ / sum_gen_wgt_;
       //return genwgt * xsec_ * lumi_ / sum_gen_wgt_;
   };
   auto calcHemVeto = [&](bool hem_veto) { 
       return !hem_veto;
   };
   auto calcBTagVeto = [&](int high_pt_jets, vector<float> jets_btag) {
       float bTag_wp = 0.4184; //pfDeepCSVJetTags:probb+probbb medium working point for 2018 & 2017
       for (int i = 0; i < high_pt_jets; i++)
           if (jets_btag[i] > bTag_wp)
               return false;
       return true;
   }; 

   // Need these to not segfault in case collection is empty
   // Also, RDataFrame.Histo1D() doesn't accept indexed vectors, so need to rename
   // e.g.: reco_PF_jet_pt[0] --> reco_PF_jet_pt0
   auto takeFirstJetPt = [&](vector<float> jets_pt) { return jets_pt.size() > 0 ? jets_pt[0] : -1; };
   auto takeSecondJetPt = [&](vector<float> jets_pt) { return jets_pt.size() > 1 ? jets_pt[1] : -1; };
   auto takeFirstJetEta = [&](vector<float> jets_eta) { return jets_eta.size() > 0 ? jets_eta[0] : -1; };
   auto takeSecondJetEta = [&](vector<float> jets_eta) { return jets_eta.size() > 1 ? jets_eta[1] : -1; };
   auto takeFirstJetPhi = [&](vector<float> jets_phi) { return jets_phi.size() > 0 ? jets_phi[0] : -1; };
   auto takeSecondJetPhi = [&](vector<float> jets_phi) { return jets_phi.size() > 1 ? jets_phi[1] : -1; };
   auto takeFirstMuonPt = [&](vector<float> muons_pt) { return muons_pt.size() > 0 ? muons_pt[0] : -1; };
   auto takeSecondMuonPt = [&](vector<float> muons_pt) { return muons_pt.size() > 1 ? muons_pt[1] : -1; };
   auto takeFirstMuonEta = [&](vector<float> muons_eta) { return muons_eta.size() > 0 ? muons_eta[0] : -1; };
   auto takeSecondMuonEta = [&](vector<float> muons_eta) { return muons_eta.size() > 1 ? muons_eta[1] : -1; };
   auto takeFirstMuonPhi = [&](vector<float> muons_phi) { return muons_phi.size() > 0 ? muons_phi[0] : -1; };
   auto takeSecondMuonPhi = [&](vector<float> muons_phi) { return muons_phi.size() > 1 ? muons_phi[1] : -1; };
   auto takeFirstMuonDxy = [&](vector<float> muons_dxy) { return muons_dxy.size() > 0 ? abs(muons_dxy[0]) : -1; };
   auto takeFirstMuonDz = [&](vector<float> muons_dz) { return muons_dz.size() > 0 ? muons_dz[0] : -1; };
   auto takeSecondMuonDxy = [&](vector<float> muons_dxy) { return muons_dxy.size() > 1 ? abs(muons_dxy[1]) : -1; };
   auto takeSecondMuonDz = [&](vector<float> muons_dz) { return muons_dz.size() > 1 ? muons_dz[1] : -1; };
   auto takeDSA0TrkChi2 = [&](vector<float> muons_chi2) { return muons_chi2.size() > 0 ? muons_chi2[0] : -1; };
   auto takeDSA1TrkChi2 = [&](vector<float> muons_chi2) { return muons_chi2.size() > 1 ? muons_chi2[1] : -1; };
   auto takeMETJetDphi = [&](vector<float> jets_phi, float MET_phi) { if (jets_phi.size() == 0) return -5.0f; float dphi = abs(jets_phi[0] - MET_phi); if (dphi > 3.141592) dphi -= 2 * 3.141592; return abs(dphi); };
   auto findFakeMETCut = [&](float MET_pt, float MET_phi, float calomet_pt, float calomet_phi, float recoil) { return sqrt( ((MET_pt*cos(MET_phi)-calomet_pt*cos(calomet_phi))*(MET_pt*cos(MET_phi)-calomet_pt*cos(calomet_phi))) + ((MET_pt*sin(MET_phi)-calomet_pt*sin(calomet_phi))*(MET_pt*sin(MET_phi)-calomet_pt*sin(calomet_phi))))/recoil; };
   auto calcMT = [&](vector<float> muons_pt, vector<float> muons_phi, float MET_pt, float MET_phi) { if (!muons_pt.size()) return -9999.9f; float dphi = abs(MET_phi - muons_phi[0]); if (dphi > 3.141592) dphi -= 2 * 3.141592; float MT2 = 2.0 * muons_pt[0] * MET_pt * (1.0 - cos(dphi)); return sqrt(MT2); };
   auto takeGenMuVxy = [&](vector<float> gen_vxy, vector<int> gen_ID) { for (size_t i = 0; i < gen_vxy.size(); i++) if (abs(gen_ID[i]) == 13) return gen_vxy[i]; return -9999.99f; }; 
   auto takeGenDphiMETmu = [&](vector<float> gen_pt, vector<float> gen_phi, vector<int> gen_ID, float MET_phi) {
       float px = 0.0, py = 0.0;
       for (size_t i = 0; i < gen_phi.size(); i++) {
           if (abs(gen_ID[i]) == 13) {
               px += gen_pt[i] * cos(gen_phi[i]);
               py += gen_pt[i] * sin(gen_phi[i]);
           }
       }
       float mu_phi = atan2(py, px);
       float dphi = abs(MET_phi - mu_phi);
       if (dphi > 3.141592)
           dphi -= 2 * 3.141592;
       return abs(dphi);
   };
   auto takeRecoDphiMETmu = [&](vector<float> reco_sel_mu_pt, vector<float> reco_sel_mu_phi, float reco_MET_phi) {
       float px = 0.0, py = 0.0;
       if (reco_sel_mu_pt.size() < 2) return -9999.99f;
       for (size_t i = 0; i < reco_sel_mu_pt.size(); i++) {
               px += reco_sel_mu_pt[i] * cos(reco_sel_mu_phi[i]);
               py += reco_sel_mu_pt[i] * sin(reco_sel_mu_phi[i]);
       }
       float mu_phi = atan2(py, px);
       float dphi = abs(reco_MET_phi - mu_phi);
       if (dphi > 3.141592)
           dphi -= 2 * 3.141592;
       return abs(dphi);
   };
   auto takeCaloPFMETRatio = [&](float reco_PF_MET_pt, float reco_Calo_MET_pt) { return abs(reco_Calo_MET_pt - reco_PF_MET_pt)/reco_Calo_MET_pt; };
   auto takeVtxSignificance = [&](float reco_vtx_vxy, float reco_vtx_sigmavxy) { return reco_vtx_vxy/reco_vtx_sigmavxy; };
   auto takeSigmaPtbyPt = [&](vector<float> reco_pt, vector<float> reco_sigpt, int index) { if (index > -1 && index < reco_pt.size()) return reco_sigpt[index]/reco_pt[index]; return -9999.9f; };

   auto df_wgts = df.
       Define("reco_PF_jet_pt0", takeFirstJetPt, {"reco_PF_jet_pt"}).
       Define("reco_PF_jet_pt1", takeSecondJetPt, {"reco_PF_jet_pt"}).
       Define("reco_PF_jet_eta0", takeFirstJetEta, {"reco_PF_jet_eta"}).
       Define("reco_PF_jet_eta1", takeSecondJetEta, {"reco_PF_jet_eta"}).
       Define("reco_PF_jet_phi0", takeFirstJetPhi, {"reco_PF_jet_phi"}).
       Define("reco_PF_jet_phi1", takeSecondJetPhi, {"reco_PF_jet_phi"}).
       Define("reco_sel_mu_pt0", takeFirstMuonPt, {"reco_sel_mu_pt"}).
       Define("reco_sel_mu_pt1", takeSecondMuonPt, {"reco_sel_mu_pt"}).
       Define("reco_sel_mu_eta0", takeFirstMuonEta, {"reco_sel_mu_eta"}).
       Define("reco_sel_mu_eta1", takeSecondMuonEta, {"reco_sel_mu_eta"}).
       Define("reco_sel_mu_phi0", takeFirstMuonPhi, {"reco_sel_mu_phi"}).
       Define("reco_sel_mu_phi1", takeSecondMuonPhi, {"reco_sel_mu_phi"}).
       Define("reco_sel_mu_dxy0", takeFirstMuonDxy, {"reco_sel_mu_dxy"}).
       Define("reco_sel_mu_dz0", takeFirstMuonDz, {"reco_sel_mu_dz"}).
       Define("reco_sel_mu_dxy1", takeSecondMuonDxy, {"reco_sel_mu_dxy"}).
       Define("reco_sel_mu_dz1", takeSecondMuonDz, {"reco_sel_mu_dz"}).
       Define("reco_dsa0_trk_chi2", takeDSA0TrkChi2, {"reco_dsa_trk_chi2"}).
       Define("reco_dsa1_trk_chi2", takeDSA1TrkChi2, {"reco_dsa_trk_chi2"}).
       Define("MET_jet_phi_dphi", takeMETJetDphi, {"reco_PF_jet_phi", "reco_PF_MET_phi"}).
       Define("recoil_jet_phi_dphi", takeMETJetDphi, {"reco_PF_jet_phi", "reco_PF_recoil_phi"}).
       Define("fake_MET_fraction", findFakeMETCut, {"reco_PF_MET_pt", "reco_PF_MET_phi","reco_Calo_MET_pt","reco_Calo_MET_phi","reco_PF_recoil_pt"}).
       Define("hem_veto", calcHemVeto ,{"reco_PF_HEM_flag"}).
       Define("bTag_veto", calcBTagVeto ,{"reco_PF_n_highPt_jets","reco_PF_jet_BTag"}).
       Define("reco_MT", calcMT, {"reco_dsa_pt", "reco_dsa_phi", "reco_PF_MET_pt", "reco_PF_MET_phi"}).
       Define("reco_METmu_dphi_v2", takeRecoDphiMETmu, {"reco_sel_mu_pt", "reco_sel_mu_phi", "reco_PF_MET_phi"}).
       Define("CaloPFMETRatio", takeCaloPFMETRatio, {"reco_PF_MET_pt", "reco_Calo_MET_pt"}).
       Define("reco_vtx_significance", takeVtxSignificance, {"reco_vtx_vxy", "reco_vtx_sigmavxy"}).
       Define("reco_dsa_idx0_sigpt_by_pt", takeSigmaPtbyPt, {"reco_dsa_pt", "reco_dsa_pt_error", "reco_dsa_idx0"}).
       Define("reco_dsa_idx1_sigpt_by_pt", takeSigmaPtbyPt, {"reco_dsa_pt", "reco_dsa_pt_error", "reco_dsa_idx1"});

   if (mode_ == common::DATA) {
       df_wgts = df_wgts.Define("wgt", "1.0");
   }
   else {
       df_wgts = df_wgts.
       Define("gen_muon_vxy", takeGenMuVxy, {"gen_vxy", "gen_ID"}).
       Define("gen_METmu_dphi", takeGenDphiMETmu, {"gen_pt", "gen_phi", "gen_ID", "gen_MET_phi"}).
       Define("Zwgt", calcZsf, {"gen_ID", "gen_pt"}).
       Define("Wwgt", calcWsf, {"gen_ID", "gen_pt"}).
       Define("Twgt", calcTsf, {"gen_ID", "gen_pt"}).
       Define("PUwgt", calcPUsf, {"gen_pu_true"}).
       Define("wgt", calcTotalWgt, {"Zwgt", "Wwgt", "Twgt", "PUwgt", "gen_wgt"});
   }


   // Beginning of cuts
   
   vector<TString> cut_strings;

   all_histos_1D_.clear();
   all_histos_2D_.clear();

   // For N-1 plots, apply all cuts except the one under study
   // We are only interested in the final SR plots, not intermediate ones
   // Each histo in macro config is one of the N-1 variables
   for (auto & [histo_name, histo_info] : histos_info_) {

       auto df_filters = df_wgts.Filter(cuts_info_[0].cut.Data(), Form("Cut0"));

       for (size_t cut = 1; cut < cuts_info_.size(); cut++) {

           auto temp_df = df_filters;

           // If this cut is the one for this N-1 hist, don't apply it
           if (histo_info->nMinus1CutDescription != cuts_info_[cut].description)
               df_filters = df_filters.Filter(cuts_info_[cut].cut.Data(), Form("Cut%d", cut));

           // if we are at last cut, make N-1 plot
           if (cut == cuts_info_.size()-1) {

               // if current sample's group is included in histo's list of groups to exclude, continue
               if (std::find(histo_info->groups_to_exclude.begin(), histo_info->groups_to_exclude.end(), group_) != histo_info->groups_to_exclude.end())
                   continue;

               // split into 1D and 2D, and int and float cases
           
               if (histo_info->type == "float1D") {
                   all_histos_1D_[histo_name] = df_filters.Histo1D<float,double>({Form("%s_%s", histo_name.Data(), group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
               }
               else if (histo_info->type == "int1D") {
                   all_histos_1D_[histo_name] = df_filters.Histo1D<int,double>({Form("%s_%s", histo_name.Data(), group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
               }
               else if (histo_info->type == "float2D") {
                   all_histos_2D_[histo_name] = df_filters.Histo2D<float,float,double>({Form("%s_%s", histo_name.Data(), group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");
               }
               else if (histo_info->type == "int2D") {
                   all_histos_2D_[histo_name] = df_filters.Histo2D<int,int,double>({Form("%s_%s", histo_name.Data(), group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data(), "wgt");
               }
               else
                   std::cout << "Hist type not recognized!" << std::endl;
           }

       }

       // force evaluation right now (maybe related to memory issue)
       df_filters.Report()->Print();

   }

   if (mode_ != common::DATA) {
       auto df_sumgenwgts = df_wgts.Sum("gen_wgt");
       std::cout << "RDF sum_gen_wgts: " << *df_sumgenwgts << std::endl;
   }

   return kTRUE;
}

void nMinus1Selector::Terminate() {
   //delete sf_pu;
}
