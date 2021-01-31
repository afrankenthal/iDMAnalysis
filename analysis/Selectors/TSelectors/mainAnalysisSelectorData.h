//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 26 23:05:33 2019 by ROOT version 6.12/07
// from TTree recoT/
// found on file: root://cmsxrootd.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/data/MET/crab_iDMAnalysis_MET_Run2018A_v2/190826_054750/0000/output_95.root
//////////////////////////////////////////////////////////

#ifndef mainAnalysisSelectorData_h
#define mainAnalysisSelectorData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
#include <vector>



class mainAnalysisSelectorData : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<ULong64_t> event_num = {fReader, "event_num"};
   TTreeReaderValue<ULong64_t> lumi_sec = {fReader, "lumi_sec"};
   TTreeReaderValue<ULong64_t> run_num = {fReader, "run_num"};
   TTreeReaderValue<Int_t> npv = {fReader, "npv"};
   TTreeReaderValue<UInt_t> MET_filters_fail_bits = {fReader, "MET_filters_fail_bits"};
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


   mainAnalysisSelectorData(TTree * /*tree*/ =0) { }
   virtual ~mainAnalysisSelectorData() { }
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

   ClassDef(mainAnalysisSelectorData,0);

};

#endif

#ifdef mainAnalysisSelectorData_cxx
void mainAnalysisSelectorData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t mainAnalysisSelectorData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef mainAnalysisSelectorData_cxx
