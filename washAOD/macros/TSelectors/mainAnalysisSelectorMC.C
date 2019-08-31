#define mainAnalysisSelectorMC_cxx
// The class definition in mainAnalysisSelectorMC.h has been generated automatically
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
// root> T->Process("mainAnalysisSelectorMC.C")
// root> T->Process("mainAnalysisSelectorMC.C","some options")
// root> T->Process("mainAnalysisSelectorMC.C+")
//


#include "mainAnalysisSelectorMC.h"
#include <TH2.h>
#include <TStyle.h>

void mainAnalysisSelectorMC::SetParams(Double_t sum_gen_wgt, Double_t xsec, Double_t lumi, TString region = "SR") {
    sum_gen_wgt_ = sum_gen_wgt;
    xsec_ = xsec;
    lumi_ = lumi;
    std::cout << "sum_gen_wgt: " << sum_gen_wgt_ << ", xsec: " << xsec_ << " [pb], lumi: " << lumi_ << " [1/pb]" << std::endl;

    region_ = region;
}

void mainAnalysisSelectorMC::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void mainAnalysisSelectorMC::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   cutflow_.clear();
   cutflow_.resize(30);

   cutflowHistos_.clear();
   for (auto & [name, hist] : histos_info_) {
       cutflowHistos_[name][0] = new TH1F(Form("%s_%d", name.Data(), 0), hist->title, hist->nbins, hist->lowX, hist->highX);
       cutflowHistos_[name][5] = new TH1F(Form("%s_%d", name.Data(), 5), hist->title, hist->nbins, hist->lowX, hist->highX);
       cutflowHistos_[name][10] = new TH1F(Form("%s_%d", name.Data(), 10), hist->title, hist->nbins, hist->lowX, hist->highX);
       cutflowHistos_[name][15] = new TH1F(Form("%s_%d", name.Data(), 15), hist->title, hist->nbins, hist->lowX, hist->highX);
   }

}

Bool_t mainAnalysisSelectorMC::Process(Long64_t entry)
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

   std::map<TString, Double_t> mapVariables;
   mapVariables["reco_PF_METjet_dphi"] = *reco_PF_METjet_dphi;
   mapVariables["reco_vtx_dR"] = *reco_vtx_dR;
   mapVariables["reco_vtx_vxy"] = *reco_vtx_vxy;
   mapVariables["reco_METmu_dphi"] = *reco_METmu_dphi;

   //if (entry > 1) return kFALSE;
   
   double weight = (*gen_wgt) * xsec_ * lumi_ / sum_gen_wgt_;

   int cut = 0;

   for (auto & [name, hists] : cutflowHistos_)
       hists[0]->Fill(mapVariables[histos_info_[name]->quantity], weight);

   cutflow_[cut++] += weight;

   // Trigger check
   if (!(*trig_fired)) return false;
   cutflow_[cut++] += weight;

   // MET offline selection (200 GeV) TODO might change
   if (*reco_PF_MET_pt < 200) return false;
   cutflow_[cut++] += weight;

   // One leading reco PF jet w/ pT > 120...
   if (reco_PF_jet_pt[0] < 120) return false;
   cutflow_[cut++] += weight;

   // ...and only one extra jet w/ pT > 30 GeV
   if (region_ == "CR_nJets") {
       if (*reco_PF_n_highPt_jets < 3) return false;
   }
   else {
       if (*reco_PF_n_highPt_jets >= 3) return false;
   }
   cutflow_[cut++] += weight;

   for (auto & [name, hists] : cutflowHistos_)
       hists[5]->Fill(mapVariables[histos_info_[name]->quantity], weight);

   // None of the 1-or-2 high-pT jets is b-tagged
   if (region_ == "CR_QCD") {
       for (int i = 0; i < *reco_PF_n_highPt_jets; i++)
           if (reco_PF_jet_BTag[i] < 0.4184) return false; // Medium operating point
   }
   else {
       for (int i = 0; i < *reco_PF_n_highPt_jets; i++)
           if (reco_PF_jet_BTag[i] > 0.4184) return false; // Medium operating point
           //if (reco_PF_jet_BTag[i] < 0 || reco_PF_jet_BTag[i] > 0.4184) return false;
   }
   cutflow_[cut++] += weight;

   // dSA muon cuts
   if (*reco_n_good_dsa < 2) return false;
   // having reco_dsa_idx{0,1} == -9999 means no valid vertex found between dSA muons
   if (*reco_dsa_idx0 == -9999 || *reco_dsa_idx1 == -9999) return false;
   cutflow_[cut++] += weight;

   if (reco_dsa_trk_n_planes[*reco_dsa_idx0] < 2) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_trk_n_hits[*reco_dsa_idx0] < 12) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_trk_chi2[*reco_dsa_idx0] > 10.0) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_pt[*reco_dsa_idx0] < 5) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_eta[*reco_dsa_idx0] > 2.4) return false;
   cutflow_[cut++] += weight;

   for (auto & [name, hists] : cutflowHistos_)
       hists[10]->Fill(mapVariables[histos_info_[name]->quantity], weight);

   if (reco_dsa_trk_n_planes[*reco_dsa_idx1] < 2) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_trk_n_hits[*reco_dsa_idx1] < 12) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_trk_chi2[*reco_dsa_idx1] > 10.0) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_pt[*reco_dsa_idx1] < 5) return false;
   cutflow_[cut++] += weight;
   if (reco_dsa_eta[*reco_dsa_idx1] > 2.4) return false;
   cutflow_[cut++] += weight;

   // dR between selected muons (SR) !!! IMPORTANT: blinding
   if (region_ == "CR_dR") {
       if (abs(*reco_vtx_dR) < 0.9) return false;
   }
   else {
       if (abs(*reco_vtx_dR) > 0.9) return false;
   }
   cutflow_[cut++] += weight;

   for (auto & [name, hists] : cutflowHistos_)
       hists[15]->Fill(mapVariables[histos_info_[name]->quantity], weight);

   // OSSF muons
   if (reco_sel_mu_charge[0] + reco_sel_mu_charge[1] != 0) return false;
   cutflow_[cut++] += weight;

   // Divide into 0, 1, or 2 GM-dSA matches
   cutflow_[cut + *reco_n_gbmdsa_match] += weight;

   return kTRUE;
}

void mainAnalysisSelectorMC::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

   TString outfilename = GetOption();

   TFile * file = new TFile("test.root", "RECREATE");
   for (auto & [quantity, hists] : cutflowHistos_) {
       hists.at(0)->Write();
       delete hists.at(0);
       hists.at(5)->Write();
       delete hists.at(5);
       hists.at(10)->Write();
       delete hists.at(10);
       hists.at(15)->Write();
       delete hists.at(15);
   }
   file->Close();

}

void mainAnalysisSelectorMC::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}
