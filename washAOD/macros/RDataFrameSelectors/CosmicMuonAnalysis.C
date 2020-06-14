#include <math.h>

#include <ROOT/RVec.hxx>
using namespace ROOT::VecOps;
#include <TFile.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>

#include "CosmicMuonAnalysis.h"

using std::map, std::vector, std::cout, std::endl;

void CosmicMuonAnalysis::Begin() {
}

void CosmicMuonAnalysis::SetMacroConfig(json macro_info) {
    macro_info_ = macro_info;
}

void CosmicMuonAnalysis::SetHistoConfig(map<TString, common::THInfo*> histos_info) {
    histos_info_ = histos_info;
}

void CosmicMuonAnalysis::SetCutConfig(vector<common::CutInfo> cuts_info) {
    cuts_info_ = cuts_info;
}

void CosmicMuonAnalysis::SetSampleConfig(common::SampleInfo sample_info) {
    sample_info_ = sample_info;
    name_ = sample_info_.label;
    mode_ = sample_info_.mode;
    group_ = sample_info_.group;
    sum_gen_wgt_ = sample_info_.sum_gen_wgt;
    xsec_ = sample_info_.xsec;
    year_ = sample_info_.year;
    custom_lumi_ = sample_info_.lumi;
}

Bool_t CosmicMuonAnalysis::Process(TChain * chain) {

    if (sum_gen_wgt_ < 1e-10 && mode_ != common::DATA) return kFALSE;

    chain_ = chain;

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df(*chain_);

    float COSALPHA = -0.94;
    if (macro_info_.find("COSALPHA") != macro_info_.end())
        COSALPHA = macro_info_["COSALPHA"].get<float>();
    float PHI_MAX = -0.8;
    if (macro_info_.find("PHI_MAX") != macro_info_.end())
        PHI_MAX = macro_info_["PHI_MAX"].get<float>();
    float PHI_MIN = -2.1;
    if (macro_info_.find("PHI_MIN") != macro_info_.end())
        PHI_MIN = macro_info_["PHI_MIN"].get<float>();
    float ETA_MAX = 0.7;
    if (macro_info_.find("ETA_MAX") != macro_info_.end())
        ETA_MAX = macro_info_["ETA_MAX"].get<float>();
    float PT_MIN = 25.0;
    if (macro_info_.find("PT_MIN") != macro_info_.end())
        PT_MIN = macro_info_["PT_MIN"].get<float>();
    int NPLANES_MIN = 2;
    if (macro_info_.find("NPLANES_MIN") != macro_info_.end())
        NPLANES_MIN = macro_info_["NPLANES_MIN"].get<int>();
    int NHITS_MIN = 18;
    if (macro_info_.find("NHITS_MIN") != macro_info_.end())
        NHITS_MIN = macro_info_["NHITS_MIN"].get<int>();
    float CHI2_MAX = 10.0;
    if (macro_info_.find("CHI2_MAX") != macro_info_.end())
        CHI2_MAX = macro_info_["CHI2_MAX"].get<float>();
    float PTRES_MAX = 1.0;
    if (macro_info_.find("PTRES_MAX") != macro_info_.end())
        PTRES_MAX = macro_info_["PTRES_MAX"].get<float>();

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

   auto passTagMuonID = [&](RVec<float> trk_n_planes, RVec<float> trk_n_hits, RVec<float> trk_chi2, RVec<float> pt, RVec<float> pt_err, RVec<float> eta, RVec<float> phi) {
       RVec<bool> pass = (phi > PHI_MIN) && (phi < PHI_MAX) && (abs(eta) < ETA_MAX) &&
                            (trk_n_planes >= (float)NPLANES_MIN) && (trk_n_hits >= (float)NHITS_MIN) && 
                            (trk_chi2 < CHI2_MAX) && (pt > PT_MIN) && (pt_err/pt < PTRES_MAX);
       return pass;
   };
   auto passProbeMuonID = [&](RVec<float> trk_n_planes, RVec<float> trk_n_hits, RVec<float> trk_chi2, RVec<float> pt, RVec<float> pt_err, RVec<float> eta, RVec<float> phi) {
       RVec<bool> pass =    (trk_n_planes >= 2) && (trk_n_hits >= 12) && 
                            (trk_chi2 < 10) && (pt > 5) && (pt_err/pt < 1);
       return pass;
   };

   auto passVtxID = [&](RVec<bool> muon_pass0, RVec<bool> muon_pass1, RVec<int> q0, RVec<int> q1, RVec<float> vtx_chi2) {
       // construct vertex id mask from each muon id mask
       // vertex index is 4 * muon0 + muon1
       RVec<bool> vtx_pass(17, 0);
       auto comb_idx = Combinations(4, 4);
       for (size_t i = 0; i < comb_idx[0].size(); i++)
           if (comb_idx[0][i] < muon_pass0.size() && comb_idx[1][i] < muon_pass1.size())
               vtx_pass[i] = (muon_pass0[comb_idx[0][i]] == 1) && (muon_pass1[comb_idx[1][i]] == 1); // && (q0[comb_idx[0][i]] + q1[comb_idx[1][i]] == 0) && (vtx_chi2[i] < 4);
       return vtx_pass;
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

   auto calcCosAlpha = [&](RVec<float> mu_pt, RVec<float> mu_eta, RVec<float> mu_phi, RVec<bool> muon_pass) {
       // Calculate cos_alpha for all combinations of muons in the event (up to 4).
       // The default impossible value is -10k, so e.g. diagonal elements in the 
       // 4x4 matrix (refer to the same muon) have that value, or if there are 
       // fewer muons than 4 in the dSA collection, the remaining elements also 
       // get -10k. Also require that the tag muon (the row in the matrix) passes
       // the tight (tag) ID
       RVec<float> vtx_cosalpha(16, -10000);
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

   auto passCosmicLeg = [&](RVec<float> vtx_cosalpha, RVec<bool> dsa_pass_probe_ID) {
       // Check if there are any combinations of muons 
       // back-to-back and both passing the probe (loose) ID
       // Note that probe ID *must* be looser than tag ID
       RVec<bool> dsa_pass_probe_ID_pair(16, 0);
       auto comb_idx = Combinations(4, 4);
       for (size_t i = 0; i < comb_idx[0].size(); i++) {
           size_t idx0 = comb_idx[0][i];
           size_t idx1 = comb_idx[1][i];
           if (idx1 >= dsa_pass_probe_ID.size()) continue;
           dsa_pass_probe_ID_pair[i] = dsa_pass_probe_ID[idx1];
       }
       RVec<bool> pass = Where(vtx_cosalpha < COSALPHA && vtx_cosalpha > -10000 &&
               dsa_pass_probe_ID_pair == 1, 1, 0);
       return pass;
   };

   auto findTagMuon = [&](RVec<bool> passCosmicLeg) {
       // If a pair exists s.t. passCosmicLeg is 1, then tag != probe
       // This is b/c if all elements in passCosmicLeg are 0, then the
       // ArgMax(passCosmicLeg) will be the first element, which has 
       // both tag and probe equal to 0 (the combination (4,4))
       size_t tag = ArgMax(passCosmicLeg)/4;
       return tag;
   };

   auto findProbeMuon = [&](RVec<bool> passCosmicLeg) {
       // If a pair exists s.t. passCosmicLeg is 1, then tag != probe
       // This is b/c if all elements in passCosmicLeg are 0, then the
       // ArgMax(passCosmicLeg) will be the first element, which has 
       // both tag and probe equal to 0 (the combination (4,4))
       size_t probe = ArgMax(passCosmicLeg) % 4;
       return probe;
   };

   TString jet_pt = "reco_PF_jet_corr_pt";
   TString jet_eta = "reco_PF_jet_corr_eta";
   TString jet_phi = "reco_PF_jet_corr_phi";
   TString MET_pt = "reco_PF_MET_corr_pt";
   TString MET_phi = "reco_PF_MET_corr_phi";

   auto df_wgts = df.
       Define("dsa_pass_tag_ID", passTagMuonID, {"reco_dsa_trk_n_planes", "reco_dsa_trk_n_hits", "reco_dsa_trk_chi2", "reco_dsa_pt", "reco_dsa_pt_err", "reco_dsa_eta", "reco_dsa_phi"}).
       Define("dsa_pass_probe_ID", passProbeMuonID, {"reco_dsa_trk_n_planes", "reco_dsa_trk_n_hits", "reco_dsa_trk_chi2", "reco_dsa_pt", "reco_dsa_pt_err", "reco_dsa_eta", "reco_dsa_phi"}).
       Define("n_good_dsa", "(int)Nonzero(dsa_pass_tag_ID).size()").
       Define("cosalpha", calcCosAlpha, {"reco_dsa_pt", "reco_dsa_eta", "reco_dsa_phi", "dsa_pass_tag_ID"}).
       Define("vtx_pass_cosmicleg", passCosmicLeg, {"cosalpha", "dsa_pass_probe_ID"}).
       Define("tag_muon", findTagMuon, {"vtx_pass_cosmicleg"}).
       Define("probe_muon", findProbeMuon, {"vtx_pass_cosmicleg"}).
       Define("tag_muon_pt", takeMatchedMuonQuantity, {"reco_dsa_pt", "reco_gm_pt", "tag_muon"}).
       Define("tag_muon_eta", takeMatchedMuonQuantity, {"reco_dsa_eta", "reco_gm_eta", "tag_muon"}).
       Define("tag_muon_phi", takeMatchedMuonQuantity, {"reco_dsa_phi", "reco_gm_phi", "tag_muon"}).
       Define("tag_muon_dxy", takeMatchedMuonQuantity, {"reco_dsa_dxy", "reco_gm_dxy", "tag_muon"}).
       Define("tag_muon_dz", takeMatchedMuonQuantity, {"reco_dsa_dz", "reco_gm_dz", "tag_muon"}).
       Define("tag_muon_nhits", takeMatchedMuonQuantity, {"reco_dsa_trk_n_hits", "reco_gm_trk_n_hits", "tag_muon"}).
       Define("tag_muon_nplanes", takeMatchedMuonQuantity, {"reco_dsa_trk_n_planes", "reco_gm_trk_n_planes", "tag_muon"}).
       Define("tag_muon_chi2", takeMatchedMuonQuantity, {"reco_dsa_trk_chi2", "reco_gm_trk_chi2", "tag_muon"}).
       Define("probe_muon_pt", takeMatchedMuonQuantity, {"reco_dsa_pt", "reco_gm_pt", "probe_muon"}).
       Define("probe_muon_eta", takeMatchedMuonQuantity, {"reco_dsa_eta", "reco_gm_eta", "probe_muon"}).
       Define("probe_muon_phi", takeMatchedMuonQuantity, {"reco_dsa_phi", "reco_gm_phi", "probe_muon"}).
       Define("probe_muon_dxy", takeMatchedMuonQuantity, {"reco_dsa_dxy", "reco_gm_dxy", "probe_muon"}).
       Define("probe_muon_dz", takeMatchedMuonQuantity, {"reco_dsa_dz", "reco_gm_dz", "probe_muon"}).
       Define("probe_muon_nhits", takeMatchedMuonQuantity, {"reco_dsa_trk_n_hits", "reco_gm_trk_n_hits", "probe_muon"}).
       Define("probe_muon_nplanes", takeMatchedMuonQuantity, {"reco_dsa_trk_n_planes", "reco_gm_trk_n_planes", "probe_muon"}).
       Define("probe_muon_chi2", takeMatchedMuonQuantity, {"reco_dsa_trk_chi2", "reco_gm_trk_chi2", "probe_muon"}).
       Define("reco_dsa_ptres", "reco_dsa_pt_err/reco_dsa_pt").
       Define("reco_gm_ptres", "reco_gm_pt_err/reco_gm_pt").
       Define("tag_muon_ptres", takeMatchedMuonQuantity, {"reco_dsa_ptres", "reco_gm_ptres", "tag_muon"}).
       Define("probe_muon_ptres", takeMatchedMuonQuantity, {"reco_dsa_ptres", "reco_gm_ptres", "probe_muon"}).
       Define("charge_product", "if (n_good_dsa > 0) return reco_dsa_charge[tag_muon]*reco_dsa_charge[probe_muon]; return -3;");

   df_wgts = df_wgts.Define("wgt", "1.0");

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
               if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                   double bin_edges[histo_info->binEdgesX.size()];
                   std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<float>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data());
               }
               else // just number of bins and low and high X
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<float>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data());
           }
           else if (histo_info->type == "vector_float1D") {
               if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                   double bin_edges[histo_info->binEdgesX.size()];
                   std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<vector<float>>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data());
               }
               else // just number of bins and low and high X
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<vector<float>>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data());
           }
           else if (histo_info->type == "int1D") {
               if (histo_info->binEdgesX[0] != -1) { // bin edges provided
                   double bin_edges[histo_info->binEdgesX.size()];
                   std::copy(histo_info->binEdgesX.begin(), histo_info->binEdgesX.end(), bin_edges);
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<int>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, bin_edges}, histo_info->quantity.Data());
               }
               else // just number of bins and low and high X
                   all_histos_1D_[histo_name][cut] = df_filters.Histo1D<int>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data());
           }
           else if (histo_info->type == "float2D")
               all_histos_2D_[histo_name][cut] = df_filters.Histo2D<float,float>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data());
           else if (histo_info->type == "int2D")
               all_histos_2D_[histo_name][cut] = df_filters.Histo2D<int,int>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX, histo_info->nbinsY, histo_info->lowY, histo_info->highY}, histo_info->quantity.Data(), histo_info->quantity2.Data());
           else
               cout << "Hist type not recognized!" << endl;
       }
       
       // restore df if it's one of the 3 SR definitions (or a otherwise non-inclusive cut)
       if (cuts_info_[cut].inclusive == "no") 
           df_filters = temp_df;
   }

   if (mode_ != common::DATA) {
       auto df_sumgenwgts = df_wgts.Sum("gen_wgt");
       cout << "RDF sum_gen_wgts: " << *df_sumgenwgts << endl;
   }

   df_filters.Report()->Print();

   return kTRUE;
}

void CosmicMuonAnalysis::Terminate() {
}
