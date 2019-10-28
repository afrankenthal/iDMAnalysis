#define RDFAnalysis_cxx
// The class definition in RDFAnalysis.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("RDFAnalysis.C")
// root> T->Process("RDFAnalysis.C","some options")
// root> T->Process("RDFAnalysis.C+")
//

#include "RDFAnalysis.h"
#include <TH2.h>
#include <TStyle.h>

void RDFAnalysis::SetCuts(std::vector<common::CutInfo> cuts_info) {
    cuts_info_ = cuts_info;
}

void RDFAnalysis::SetParams(common::SampleInfo sample_info, Double_t lumi, TString region = "SR") {
    sample_info_ = sample_info;
    mode_ = sample_info_.mode;
    group_ = sample_info_.group;
    sum_gen_wgt_ = sample_info_.sum_gen_wgt;
    xsec_ = sample_info_.xsec;
    lumi_ = lumi;
    std::cout << "sum_gen_wgt: " << sum_gen_wgt_ << ", xsec: " << xsec_ << " [pb], lumi: " << lumi_ << " [1/pb] " << std::endl;

    region_ = region;

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

void RDFAnalysis::Begin() {
   cutflow_.clear();
   cutflow_.resize(30);

   all_histos_.clear();
//   for (auto & [name, hist] : histos_info_) {
//       if (std::find(hist->groups.begin(), hist->groups.end(), sample_info_.group) == hist->groups.end()) continue;
//       for (auto cut : hist->cuts) {
//           if (hist->nbinsY == -1) // 1D plot
//               cutflowHistos_[name][cut] = new TH1F(Form("%s_cut%d_%s", name.Data(), cut, sample_info_.group.Data()), common::group_plot_info[sample_info_.group].legend, hist->nbinsX, hist->lowX, hist->highX);
//           else // 2D plot
//               cutflowHistos_[name][cut] = new TH2F(Form("%s_cut%d_%s", name.Data(), cut, sample_info_.group.Data()), common::group_plot_info[sample_info_.group].legend, hist->nbinsX, hist->lowX, hist->highX, hist->nbinsY, hist->lowY, hist->highY);
//           cutflowHistos_[name][cut]->Sumw2();
//           if (common::group_plot_info[sample_info_.group].mode == common::BKG) 
//               cutflowHistos_[name][cut]->SetFillColor(common::group_plot_info[sample_info_.group].color);
//           else if (common::group_plot_info[sample_info_.group].mode == common::DATA) {
//               cutflowHistos_[name][cut]->SetMarkerColor(common::group_plot_info[sample_info_.group].color);
//               cutflowHistos_[name][cut]->SetMarkerStyle(common::group_plot_info[sample_info_.group].style);
//               cutflowHistos_[name][cut]->SetMarkerSize(0.9);
//           }
//           else if (common::group_plot_info[sample_info_.group].mode == common::SIGNAL) {
//               cutflowHistos_[name][cut]->SetLineColor(common::group_plot_info[sample_info_.group].color);
//               cutflowHistos_[name][cut]->SetLineStyle(common::group_plot_info[sample_info_.group].style);
//               cutflowHistos_[name][cut]->SetLineWidth(2);
//               cutflowHistos_[name][cut]->SetMarkerSize(0);
//           }
//       }
//   }
}

//void RDFAnalysis::doFills(int cut, double weight) {
//   // Map between the actual variables and their string representation
//   // For automating things and allowing flexibility via configs
//   std::map<TString, Double_t> mapVariables;
//   mapVariables["reco_PF_METjet_dphi"] = *reco_PF_METjet_dphi;
//   mapVariables["reco_vtx_dR"] = *reco_vtx_dR;
//   mapVariables["reco_vtx_vxy"] = *reco_vtx_vxy;
//   mapVariables["reco_METmu_dphi"] = *reco_METmu_dphi;
//   mapVariables["reco_PF_n_highPt_jets"] = (Double_t)(*reco_PF_n_highPt_jets);
//   mapVariables["reco_PF_MET_pt"] = *reco_PF_MET_pt;
//   mapVariables["reco_PF_jet_pt[0]"] = (reco_PF_jet_pt.GetSize() > 0 ? reco_PF_jet_pt[0] : -10);
//   mapVariables["reco_sel_mu_pt[0]"] = (reco_sel_mu_pt.GetSize() > 0 ? reco_sel_mu_pt[0] : -10);
//   mapVariables["reco_sel_mu_pt[1]"] = (reco_sel_mu_pt.GetSize() > 1 ? reco_sel_mu_pt[1] : -10);
//   mapVariables["reco_n_dsa"] = *reco_n_dsa;
//   mapVariables["reco_n_good_dsa"] = *reco_n_good_dsa;
//
//   cutflow_[cut] += weight;
//
//   // Only fill cuts that are present in the config
//   for (auto & [name, hists] : cutflowHistos_)
//       if (std::find(histos_info_[name]->cuts.begin(), histos_info_[name]->cuts.end(), cut) != histos_info_[name]->cuts.end()) {
//           if (histos_info_[name]->nbinsY == -1) // 1D plot
//               hists[cut]->Fill(mapVariables[histos_info_[name]->quantity], weight);
//           else // 2D plot
//               ((TH2F*)hists[cut])->Fill(mapVariables[histos_info_[name]->quantity], mapVariables[histos_info_[name]->quantity2], weight);
//       }
//
//}

Bool_t RDFAnalysis::Process(TChain * chain) {

    if (sum_gen_wgt_ < 0.1 && mode_ != common::DATA) return kFALSE;

   all_histos_.clear();
    chain_ = chain;

    ROOT::EnableImplicitMT();
    //chain->SetCacheSize(0);
    ROOT::RDataFrame df(*chain_);

    // First, define all the relevant weights
    auto calcZsf = [&](std::vector<int> gen_ID, std::vector<float> gen_pt) { 
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
    auto calcWsf = [&](std::vector<int> gen_ID, std::vector<float> gen_pt) { 
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
        if (binNum == sf_w_qcd->GetNbinsX())
            binNum = sf_w_qcd->GetNbinsX()-1;
        float sf = (float)sf_w_qcd->GetBinContent(binNum) * (float)sf_w_ewk->GetBinContent(binNum);
        return sf; 
    };
    auto calcTsf = [&](std::vector<int> gen_ID, std::vector<float> gen_pt) { 
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

   // Need these to not segfault in case collection is empty
   // Also, RDataFrame.Histo1D() doesn't accept indexed vectors, so need to rename
   // e.g.: reco_PF_jet_pt[0] --> reco_PF_jet_pt0
   auto takeFirstJetPt = [&](std::vector<float> jets_pt) { return jets_pt.size() > 0 ? jets_pt[0] : -1; };
   auto takeSecondJetPt = [&](std::vector<float> jets_pt) { return jets_pt.size() > 1 ? jets_pt[1] : -1; };
   auto takeFirstJetEta = [&](std::vector<float> jets_eta) { return jets_eta.size() > 0 ? jets_eta[0] : -1; };
   auto takeSecondJetEta = [&](std::vector<float> jets_eta) { return jets_eta.size() > 1 ? jets_eta[1] : -1; };
   auto takeFirstJetPhi = [&](std::vector<float> jets_phi) { return jets_phi.size() > 0 ? jets_phi[0] : -1; };
   auto takeSecondJetPhi = [&](std::vector<float> jets_phi) { return jets_phi.size() > 1 ? jets_phi[1] : -1; };
   auto takeFirstMuonPt = [&](std::vector<float> muons_pt) { return muons_pt.size() > 0 ? muons_pt[0] : -1; };
   auto takeSecondMuonPt = [&](std::vector<float> muons_pt) { return muons_pt.size() > 1 ? muons_pt[1] : -1; };
   auto takeFirstMuonEta = [&](std::vector<float> muons_eta) { return muons_eta.size() > 0 ? muons_eta[0] : -1; };
   auto takeSecondMuonEta = [&](std::vector<float> muons_eta) { return muons_eta.size() > 1 ? muons_eta[1] : -1; };
   auto takeFirstMuonPhi = [&](std::vector<float> muons_phi) { return muons_phi.size() > 0 ? muons_phi[0] : -1; };
   auto takeSecondMuonPhi = [&](std::vector<float> muons_phi) { return muons_phi.size() > 1 ? muons_phi[1] : -1; };
   auto takeMETJetDphi = [&](std::vector<float> jets_phi, float MET_phi) { if (jets_phi.size() == 0) return -5.0f; float dphi = abs(jets_phi[0] - MET_phi); if (dphi > 3.141592) dphi -= 2 * 3.141592; return abs(dphi); };



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
       Define("MET_jet_phi_dphi", takeMETJetDphi, {"reco_PF_jet_phi", "reco_PF_MET_phi"});

   if (mode_ == common::DATA) {
       df_wgts = df_wgts.Define("wgt", "1.0");
   }
   else {
       df_wgts = df_wgts.
       Define("Zwgt", calcZsf, {"gen_ID", "gen_pt"}).
       Define("Wwgt", calcWsf, {"gen_ID", "gen_pt"}).
       Define("Twgt", calcTsf, {"gen_ID", "gen_pt"}).
       Define("PUwgt", calcPUsf, {"gen_pu_true"}).
       Define("wgt", calcTotalWgt, {"Zwgt", "Wwgt", "Twgt", "PUwgt", "gen_wgt"});
   }


   // Beginning of cuts
   
   std::vector<TString> cut_strings;

   //cut_strings.push_back("1"); // cut 0 -- no cut
   //cut_strings.push_back("MET_filters_fail_bits == 0");
   //cut_strings.push_back("reco_PF_HEM_flag == 0");
   //cut_strings.push_back("trig_fired%2 == 1");
   //cut_strings.push_back("reco_PF_MET_pt > 200");
   //cut_strings.push_back("reco_PF_jet_pt.size() > 0");
   //cut_strings.push_back("for (int i = 0; i < reco_PF_n_highPt_jets; i++) if (abs(reco_PF_jet_eta[i]) > 2.5) return false; return true;");
   //cut_strings.push_back("reco_PF_jet_pt[0] > 200");
   //cut_strings.push_back("is_electron == 0");
   //cut_strings.push_back("is_photon == 0");
   //if (region_ == "CR_0muons")
   //    cut_strings.push_back("reco_n_good_dsa == 0");
   //else
   //    cut_strings.push_back("reco_n_good_dsa >= 0");
   //cut_strings.push_back("MET_jet_phi_dphi > 1.0");
   ////cut_strings.push_back("abs(reco_PF_jet_eta[0]) < 2.5");
   //if (region_ == "CR_nJets" || region_ == "CR_0muons")
   //    cut_strings.push_back("reco_PF_n_highPt_jets == 3");
   //else
   //    cut_strings.push_back("reco_PF_n_highPt_jets < 3");
   //if (region_ == "CR_bTag")
   //    cut_strings.push_back("for (int i = 0; i < reco_PF_n_highPt_jets; i++) if (reco_PF_jet_BTag[i] < 0.4184) return false; return true;");
   //else
   //    cut_strings.push_back("for (int i = 0; i < reco_PF_n_highPt_jets; i++) if (reco_PF_jet_BTag[i] > 0.4184) return false; return true;");
   //cut_strings.push_back("reco_n_good_dsa >= 1");
   //cut_strings.push_back("reco_dsa_idx0 > -9999 && reco_dsa_idx1 > -9999");
   //cut_strings.push_back("reco_dsa_trk_n_planes[reco_dsa_idx0] >= 2");
   //cut_strings.push_back("reco_dsa_trk_n_hits[reco_dsa_idx0] >= 12");
   //cut_strings.push_back("reco_dsa_trk_chi2[reco_dsa_idx0] < 10");
   //cut_strings.push_back("reco_dsa_pt[reco_dsa_idx0] > 5");
   //cut_strings.push_back("abs(reco_dsa_eta[reco_dsa_idx0]) < 2.4");
   //cut_strings.push_back("reco_dsa_trk_n_planes[reco_dsa_idx1] >= 2");
   //cut_strings.push_back("reco_dsa_trk_n_hits[reco_dsa_idx1] >= 12");
   //cut_strings.push_back("reco_dsa_trk_chi2[reco_dsa_idx1] < 10");
   //cut_strings.push_back("reco_dsa_pt[reco_dsa_idx1] > 5");
   //cut_strings.push_back("abs(reco_dsa_eta[reco_dsa_idx1]) < 2.4");
   //if (region_ == "CR_dR")
   //    cut_strings.push_back("reco_vtx_dR > 0.9");
   //else
   //    cut_strings.push_back("reco_vtx_dR < 0.9");
   //cut_strings.push_back("reco_sel_mu_charge[0]+reco_sel_mu_charge[1] == 0");
   //cut_strings.push_back("reco_n_gbmdsa_match == 0");
   //cut_strings.push_back("reco_n_gbmdsa_match == 1");
   //cut_strings.push_back("reco_n_gbmdsa_match == 2");


   all_histos_.clear();
   cutflow_.clear();
   auto df_filters = df_wgts.Filter(cuts_info_[0].cut.Data(), Form("Cut0"));
   cutflow_.push_back(df_filters.Sum<double>("wgt"));
   for (size_t cut = 1; cut < cuts_info_.size(); cut++) {
       // Last 3 cuts are not inclusive, so make copy to restore later
       auto temp_df = df_filters;

       df_filters = df_filters.Filter(cuts_info_[cut].cut.Data(), Form("Cut%d", cut));
       cutflow_.push_back(df_filters.Sum<double>("wgt"));
       // make all requested histograms for each cut
       for (auto & [histo_name, histo_info] : histos_info_) {
           if (std::find(histo_info->groups.begin(), histo_info->groups.end(), group_) == histo_info->groups.end()) continue;

           if (all_histos_.find(histo_name) == all_histos_.end())
               all_histos_[histo_name] = std::map<int, ROOT::RDF::RResultPtr<TH1D>>();
           if (histo_info->type == "float")
               all_histos_[histo_name][cut] = df_filters.Histo1D<float,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
           else if (histo_info->type == "int")
               all_histos_[histo_name][cut] = df_filters.Histo1D<int,double>({Form("%s_cut%d_%s", histo_name.Data(), cut, group_.Data()), common::group_plot_info[group_].legend, histo_info->nbinsX, histo_info->lowX, histo_info->highX}, histo_info->quantity.Data(), "wgt");
           else
               std::cout << "Hist type not recognized!" << std::endl;
       }
       // restore df if it's one of the 3 SR definitions
       if (cuts_info_[cut].inclusive == "no") //cut_strings[cut].Contains("reco_n_gbmdsa_match"))
           df_filters = temp_df;
   }

   if (mode_ != common::DATA) {
       auto df_sumgenwgts = df_wgts.Sum("gen_wgt");
       std::cout << "RDF sum_gen_wgts: " << *df_sumgenwgts << std::endl;
   }

   df_filters.Report()->Print();

   //TFile f(Form("deleteme/testDF_%s.root", sample_info_.label.Data()),"RECREATE");
   //for (auto hist : hists)
   //    hist->Write();
   //f.Close();


   return kTRUE;
}

void RDFAnalysis::Terminate() {
   //delete sf_pu;
}
