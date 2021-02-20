//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 26 23:49:46 2019 by ROOT version 6.12/07
// from TTree recoT/
// found on file: root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_thirdrun/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/crab_iDMAnalysis_QCD_bEnriched_HT700to1000/190722_042845/0000/merged_39e87f58-7dd3-4764-9ef7-df13c05061c3_06.root
//////////////////////////////////////////////////////////

#ifndef mainAnalysisSelector_h
#define mainAnalysisSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
#include <vector>
#include <map>
#include <memory>

#include "../utils/common.h"


class mainAnalysisSelector : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<Int_t> npv = {fReader, "npv"};
   TTreeReaderValue<UInt_t> trig_fired = {fReader, "trig_fired"};
   TTreeReaderValue<Int_t> reco_n_dsa = {fReader, "reco_n_dsa"};
   TTreeReaderValue<Int_t> reco_n_good_dsa = {fReader, "reco_n_good_dsa"};
   TTreeReaderArray<float> reco_dsa_pt = {fReader, "reco_dsa_pt"};
   TTreeReaderArray<float> reco_dsa_eta = {fReader, "reco_dsa_eta"};
   TTreeReaderArray<float> reco_dsa_phi = {fReader, "reco_dsa_phi"};
   TTreeReaderArray<float> reco_dsa_dxy = {fReader, "reco_dsa_dxy"};
   TTreeReaderArray<float> reco_dsa_dz = {fReader, "reco_dsa_dz"};
   TTreeReaderArray<int> reco_dsa_charge = {fReader, "reco_dsa_charge"};
   TTreeReaderArray<float> reco_dsa_trk_chi2 = {fReader, "reco_dsa_trk_chi2"};
   TTreeReaderArray<float> reco_dsa_trk_n_planes = {fReader, "reco_dsa_trk_n_planes"};
   TTreeReaderArray<float> reco_dsa_trk_n_hits = {fReader, "reco_dsa_trk_n_hits"};
   TTreeReaderValue<Int_t> reco_dsa_idx0 = {fReader, "reco_dsa_idx0"};
   TTreeReaderValue<Int_t> reco_dsa_idx1 = {fReader, "reco_dsa_idx1"};
   TTreeReaderValue<Int_t> reco_n_gbmdsa_match = {fReader, "reco_n_gbmdsa_match"};
   TTreeReaderValue<Float_t> reco_gbmdsa_dR = {fReader, "reco_gbmdsa_dR"};
   TTreeReaderArray<float> reco_sel_mu_pt = {fReader, "reco_sel_mu_pt"};
   TTreeReaderArray<float> reco_sel_mu_eta = {fReader, "reco_sel_mu_eta"};
   TTreeReaderArray<float> reco_sel_mu_phi = {fReader, "reco_sel_mu_phi"};
   TTreeReaderArray<float> reco_sel_mu_dxy = {fReader, "reco_sel_mu_dxy"};
   TTreeReaderArray<float> reco_sel_mu_dz = {fReader, "reco_sel_mu_dz"};
   TTreeReaderArray<int> reco_sel_mu_charge = {fReader, "reco_sel_mu_charge"};
   TTreeReaderValue<Float_t> reco_Mmumu = {fReader, "reco_Mmumu"};
   TTreeReaderValue<Float_t> reco_METmu_dphi = {fReader, "reco_METmu_dphi"};
   TTreeReaderValue<Float_t> reco_vtx_vxy = {fReader, "reco_vtx_vxy"};
   TTreeReaderValue<Float_t> reco_vtx_vz = {fReader, "reco_vtx_vz"};
   TTreeReaderValue<Float_t> reco_vtx_sigmavxy = {fReader, "reco_vtx_sigmavxy"};
   TTreeReaderValue<Float_t> reco_vtx_reduced_chi2 = {fReader, "reco_vtx_reduced_chi2"};
   TTreeReaderValue<Float_t> reco_vtx_dR = {fReader, "reco_vtx_dR"};
   TTreeReaderValue<Float_t> reco_PF_MET_pt = {fReader, "reco_PF_MET_pt"};
   TTreeReaderValue<Float_t> reco_PF_MET_phi = {fReader, "reco_PF_MET_phi"};
   TTreeReaderValue<Int_t> reco_PF_n_jets = {fReader, "reco_PF_n_jets"};
   TTreeReaderValue<Int_t> reco_PF_n_highPt_jets = {fReader, "reco_PF_n_highPt_jets"};
   TTreeReaderArray<float> reco_PF_jet_pt = {fReader, "reco_PF_jet_pt"};
   TTreeReaderArray<float> reco_PF_jet_eta = {fReader, "reco_PF_jet_eta"};
   TTreeReaderArray<float> reco_PF_jet_phi = {fReader, "reco_PF_jet_phi"};
   TTreeReaderArray<float> reco_PF_jet_BTag = {fReader, "reco_PF_jet_BTag"};
   TTreeReaderValue<Float_t> reco_PF_METjet_dphi = {fReader, "reco_PF_METjet_dphi"};
   TTreeReaderValue<Float_t> reco_MHT_Pt = {fReader, "reco_MHT_Pt"};
   TTreeReaderValue<UInt_t> cuts = {fReader, "cuts"};
   // Branches that are only present in either data or MC (need dynamic allocation)
   // DATA
   std::unique_ptr<TTreeReaderValue<UInt_t>> MET_filters_fail_bits;
   //TTreeReaderValue<ULong64_t> num_event = {fReader, "num_event"};
   // MC
   std::unique_ptr<TTreeReaderValue<Float_t>> gen_wgt; // = {fReader, "gen_wgt"};
   std::unique_ptr<TTreeReaderValue<Int_t>> gen_pu_true;
   //TTreeReaderValue<ULong64_t> event_n = {fReader, "event_n"};
   std::unique_ptr<TTreeReaderArray<Float_t>> gen_pt;
   std::unique_ptr<TTreeReaderArray<Int_t>> gen_id;


   mainAnalysisSelector(TTree * /*tree*/ =0) : cutflow_(30, 0.0) { }
   virtual ~mainAnalysisSelector() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   void doFills(int cut, double weight);
   void SetMode(common::MODE mode);
   void SetParams(common::SampleInfo sample_info, Double_t lumi, TString region);
   void SetHistos(std::map<TString, common::THInfo*> histos_info) { histos_info_ = histos_info; }
   std::vector<double> GetCutflow() { return cutflow_; }
   std::map<TString, std::map<int, TH1*>> GetHistograms() { return cutflowHistos_; }

   ClassDef(mainAnalysisSelector,2);

   std::vector<double> cutflow_;
   std::map<TString, common::THInfo*> histos_info_;
   std::map<TString, std::map<int, TH1*>> cutflowHistos_;

   common::SampleInfo sample_info_;
   Double_t sum_gen_wgt_;
   Double_t xsec_;
   Double_t lumi_;
   common::MODE mode_;
   TString region_;

   TH1F * sf_z_qcd, * sf_z_ewk;
   TH1F * sf_w_qcd, * sf_w_ewk;
   TH1F * sf_pu;

};

#endif

#ifdef mainAnalysisSelector_cxx
void mainAnalysisSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t mainAnalysisSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef mainAnalysisSelector_cxx
