#define mainAnalysisSelector_cxx
// The class definition in mainAnalysisSelector.h has been generated automatically
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
// root> T->Process("mainAnalysisSelector.C")
// root> T->Process("mainAnalysisSelector.C","some options")
// root> T->Process("mainAnalysisSelector.C+")
//


#include "mainAnalysisSelector.h"
#include <TH2.h>
#include <TStyle.h>

void mainAnalysisSelector::SetMode(common::MODE mode) { 
    mode_ = mode; 
    if (mode_ == common::DATA)
        MET_filters_fail_bits = std::make_unique<TTreeReaderValue<UInt_t>>(fReader, "MET_filters_fail_bits");
    else {
        gen_wgt = std::make_unique<TTreeReaderValue<Float_t>>(fReader, "gen_wgt");
        gen_pu_true = std::make_unique<TTreeReaderValue<Int_t>>(fReader, "gen_pu_true");
        gen_pt = std::make_unique<TTreeReaderArray<Float_t>>(fReader, "gen_pt"); // FIXME
        gen_id = std::make_unique<TTreeReaderArray<Int_t>>(fReader, "gen_ID"); // FIXME rightlabels
    }
}

void mainAnalysisSelector::SetParams(common::SampleInfo sample_info, Double_t lumi, TString region = "SR") {
    sample_info_ = sample_info;
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
    kfactors->Close();
    
    // Set up pileup corrections
    TFile * pileup = new TFile("../data/puWeights_10x_56ifb.root");
    TH1F * h_pu = (TH1F*)pileup->Get("puWeights");
    sf_pu = (TH1F*)h_pu->Clone();
    sf_pu->SetDirectory(0);
    pileup->Close();

}

void mainAnalysisSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void mainAnalysisSelector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   cutflow_.clear();
   cutflow_.resize(30);

   cutflowHistos_.clear();
   for (auto & [name, hist] : histos_info_) {
       if (std::find(hist->groups.begin(), hist->groups.end(), sample_info_.group) == hist->groups.end()) continue;
       for (auto cut : hist->cuts) {
           if (hist->nbinsY == -1) // 1D plot
               cutflowHistos_[name][cut] = new TH1F(Form("%s_cut%d_%s", name.Data(), cut, sample_info_.group.Data()), common::group_plot_info[sample_info_.group].legend, hist->nbinsX, hist->lowX, hist->highX);
           else // 2D plot
               cutflowHistos_[name][cut] = new TH2F(Form("%s_cut%d_%s", name.Data(), cut, sample_info_.group.Data()), common::group_plot_info[sample_info_.group].legend, hist->nbinsX, hist->lowX, hist->highX, hist->nbinsY, hist->lowY, hist->highY);
           cutflowHistos_[name][cut]->Sumw2();
           GetOutputList()->Add(cutflowHistos_[name][cut]);
           if (common::group_plot_info[sample_info_.group].mode == common::BKG) 
               cutflowHistos_[name][cut]->SetFillColor(common::group_plot_info[sample_info_.group].color);
           else if (common::group_plot_info[sample_info_.group].mode == common::DATA) {
               cutflowHistos_[name][cut]->SetMarkerColor(common::group_plot_info[sample_info_.group].color);
               cutflowHistos_[name][cut]->SetMarkerStyle(common::group_plot_info[sample_info_.group].style);
               cutflowHistos_[name][cut]->SetMarkerSize(0.9);
           }
           else if (common::group_plot_info[sample_info_.group].mode == common::SIGNAL) {
               cutflowHistos_[name][cut]->SetLineColor(common::group_plot_info[sample_info_.group].color);
               cutflowHistos_[name][cut]->SetLineStyle(common::group_plot_info[sample_info_.group].style);
               cutflowHistos_[name][cut]->SetLineWidth(2);
               cutflowHistos_[name][cut]->SetMarkerSize(0);
           }
       }
   }
}

void mainAnalysisSelector::doFills(int cut, double weight) {
   // Map between the actual variables and their string representation
   // For automating things and allowing flexibility via configs
   std::map<TString, Double_t> mapVariables;
   mapVariables["reco_PF_METjet_dphi"] = *reco_PF_METjet_dphi;
   mapVariables["reco_vtx_dR"] = *reco_vtx_dR;
   mapVariables["reco_vtx_vxy"] = *reco_vtx_vxy;
   mapVariables["reco_METmu_dphi"] = *reco_METmu_dphi;
   mapVariables["reco_PF_n_highPt_jets"] = (Double_t)(*reco_PF_n_highPt_jets);
   mapVariables["reco_PF_MET_pt"] = *reco_PF_MET_pt;
   mapVariables["reco_PF_jet_pt[0]"] = (reco_PF_jet_pt.GetSize() > 0 ? reco_PF_jet_pt[0] : -10);
   mapVariables["reco_sel_mu_pt[0]"] = (reco_sel_mu_pt.GetSize() > 0 ? reco_sel_mu_pt[0] : -10);
   mapVariables["reco_sel_mu_pt[1]"] = (reco_sel_mu_pt.GetSize() > 1 ? reco_sel_mu_pt[1] : -10);
   mapVariables["reco_n_dsa"] = *reco_n_dsa;
   mapVariables["reco_n_good_dsa"] = *reco_n_good_dsa;

   cutflow_[cut] += weight;

   // Only fill cuts that are present in the config
   for (auto & [name, hists] : cutflowHistos_)
       if (std::find(histos_info_[name]->cuts.begin(), histos_info_[name]->cuts.end(), cut) != histos_info_[name]->cuts.end()) {
           if (histos_info_[name]->nbinsY == -1) // 1D plot
               hists[cut]->Fill(mapVariables[histos_info_[name]->quantity], weight);
           else // 2D plot
               ((TH2F*)hists[cut])->Fill(mapVariables[histos_info_[name]->quantity], mapVariables[histos_info_[name]->quantity2], weight);
       }

}

Bool_t mainAnalysisSelector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   fReader.SetLocalEntry(entry);

   // data weight is always 1
   double weight = 1;

   // MC gen weight, lumi, and cross-section
   if (mode_ != common::DATA)
       weight = (**gen_wgt) * xsec_ * lumi_ / sum_gen_wgt_;

   // Top corrections
   if (sample_info_.group == "Top") {
       double sf_top = 1;
       int n_top = 0;
       for (int i = 0; i < (*gen_id).GetSize(); i++) {
           if ((*gen_id)[i] == 6) {
               sf_top *= (*gen_pt)[i];
               n_top++;
           }
       }
       if (n_top == 0)
           std::cout << "ERROR! Sample group is 'Top' but no gen-level top quarks found." << std::endl;
       else
           sf_top = pow(sf_top, 1/n_top);
       weight *= sf_top;
   }

   // ZJets+WJets NLO (QCD) and EWK corrections
   if (sample_info_.group == "WJets") {
       for (int i = 0; i < (*gen_id).GetSize(); i++) {
           if ((*gen_id)[i] == 24) {
               double w_pt = (*gen_pt)[i];
               weight *= sf_w_qcd->GetBinContent(sf_w_qcd->FindBin(w_pt));
               weight *= sf_w_ewk->GetBinContent(sf_w_ewk->FindBin(w_pt));
           }
       }
   }
   else if (sample_info_.group == "ZJets" || sample_info_.group == "DY") {
       for (int i = 0; i < (*gen_id).GetSize(); i++) {
           if ((*gen_id)[i] == 23) {
               double z_pt = (*gen_pt)[i];
               weight *= sf_z_qcd->GetBinContent(sf_z_qcd->FindBin(z_pt));
               weight *= sf_z_ewk->GetBinContent(sf_z_ewk->FindBin(z_pt));
           }
       }
   }
   
   /// Pileup corrections
   if (mode_ != common::DATA) {
       int pu_true = **gen_pu_true;
       weight *= sf_pu->GetBinContent(sf_pu->FindBin((double)pu_true));
   }

   // Beginning of cuts
   
   int cut = 0;

   doFills(cut++, weight);

   // TODO MC doesn't have MET filters yet, just make the cut pass
   if (mode_ == common::DATA) {
       if ((**MET_filters_fail_bits)) return false;
       doFills(cut++, weight);
   }
   else {
       doFills(cut++, weight);
   }

   // Trigger check
   if (!(*trig_fired)) return false;
   doFills(cut++, weight);

   // MET offline selection (200 GeV) TODO might change
   if (*reco_PF_MET_pt < 200) return false;
   doFills(cut++, weight);

   // One leading reco PF jet w/ pT > 120...
   if (reco_PF_jet_pt.GetSize() == 0) return false;
   if (reco_PF_jet_pt[0] < 120) return false;
   doFills(cut++, weight);

   // ...and only one extra jet w/ pT > 30 GeV
   if (region_ == "CR_nJets") {
       if (*reco_PF_n_highPt_jets < 3) return false;
   }
   else {
       if (*reco_PF_n_highPt_jets >= 3) return false;
   }
   doFills(cut++, weight);

   // None of the 1-or-2 high-pT jets is b-tagged
   if (region_ == "CR_bTag") {
       for (int i = 0; i < *reco_PF_n_highPt_jets; i++)
           if (reco_PF_jet_BTag[i] < 0.4184) return false; // Medium operating point
   }
   else {
       for (int i = 0; i < *reco_PF_n_highPt_jets; i++)
           if (reco_PF_jet_BTag[i] > 0.4184) return false; // Medium operating point
           //if (reco_PF_jet_BTag[i] < 0 || reco_PF_jet_BTag[i] > 0.4184) return false;
   }
   doFills(cut++, weight);

   // dSA muon cuts
   if (*reco_n_good_dsa < 2) return false;
   // having reco_dsa_idx{0,1} == -9999 means no valid vertex found between dSA muons
   if (*reco_dsa_idx0 == -9999 || *reco_dsa_idx1 == -9999) return false;
   doFills(cut++, weight);

   if (reco_dsa_trk_n_planes[*reco_dsa_idx0] < 2) return false;
   doFills(cut++, weight);
   if (reco_dsa_trk_n_hits[*reco_dsa_idx0] < 12) return false;
   doFills(cut++, weight);
   if (reco_dsa_trk_chi2[*reco_dsa_idx0] > 10.0) return false;
   doFills(cut++, weight);
   if (reco_dsa_pt[*reco_dsa_idx0] < 5) return false;
   doFills(cut++, weight);
   if (reco_dsa_eta[*reco_dsa_idx0] > 2.4) return false;
   doFills(cut++, weight);

   if (reco_dsa_trk_n_planes[*reco_dsa_idx1] < 2) return false;
   doFills(cut++, weight);
   if (reco_dsa_trk_n_hits[*reco_dsa_idx1] < 12) return false;
   doFills(cut++, weight);
   if (reco_dsa_trk_chi2[*reco_dsa_idx1] > 10.0) return false;
   doFills(cut++, weight);
   if (reco_dsa_pt[*reco_dsa_idx1] < 5) return false;
   doFills(cut++, weight);
   if (reco_dsa_eta[*reco_dsa_idx1] > 2.4) return false;
   doFills(cut++, weight);

   // dR between selected muons (SR) !!! IMPORTANT: blinding
   if (region_ == "CR_dR") {
       if (abs(*reco_vtx_dR) < 0.9) return false;
   }
   else {
       if (abs(*reco_vtx_dR) > 0.9) return false;
   }
   doFills(cut++, weight);

   // OSSF muons
   if (reco_sel_mu_charge[0] + reco_sel_mu_charge[1] != 0) return false;
   doFills(cut++, weight);

   // Divide into 0, 1, or 2 GM-dSA matches
   doFills(cut + *reco_n_gbmdsa_match, weight);
   //cutflow_[cut + *reco_n_gbmdsa_match] += weight;

   return kTRUE;
}

void mainAnalysisSelector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

   TString option = GetOption();

}

void mainAnalysisSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   //delete sf_pu;
}
