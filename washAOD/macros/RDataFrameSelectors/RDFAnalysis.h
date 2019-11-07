//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 26 23:49:46 2019 by ROOT version 6.12/07
// from TTree recoT/
// found on file: root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_thirdrun/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/crab_iDMAnalysis_QCD_bEnriched_HT700to1000/190722_042845/0000/merged_39e87f58-7dd3-4764-9ef7-df13c05061c3_06.root
//////////////////////////////////////////////////////////

#ifndef RDFAnalysis_h
#define RDFAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/InterfaceUtils.hxx>

// Headers needed by this particular selector
#include <vector>
#include <map>

#include "../utils/common.h"


class RDFAnalysis {
public :
   //RDFAnalysis() : cutflow_(30, 0.0) { }
   RDFAnalysis() { }
   ~RDFAnalysis() { }
   void    Begin();
   Bool_t  Process(TChain * chain);
   void    Terminate();

   //void doFills(int cut, double weight);
   void SetParams(common::SampleInfo sample_info, Double_t lumi);
   void SetCuts(std::vector<common::CutInfo> cuts_info);
   void SetHistos(std::map<TString, common::THInfo*> histos_info) { histos_info_ = histos_info; }
   //std::vector<double> GetCutflow() { return cutflow_; }
   std::vector<ROOT::RDF::RResultPtr<ROOT::RDF::RDFDetail::SumReturnType_t<double>>> GetCutflow() { return cutflow_; }
   //std::map<TString, std::map<int, TH1*>> GetHistograms() { return all_histos_; }
   std::map<TString, std::map<int, ROOT::RDF::RResultPtr<TH1D>>> GetHistograms1D() { return all_histos_1D_; }
   std::map<TString, std::map<int, ROOT::RDF::RResultPtr<TH2D>>> GetHistograms2D() { return all_histos_2D_; }

   TChain * chain_;

   //std::vector<double> cutflow_;
   std::map<TString, common::THInfo*> histos_info_;
   //std::map<TString, std::map<int, TH1*>> cutflowHistos_;
   std::map<TString, std::map<int, ROOT::RDF::RResultPtr<TH1D>>> all_histos_1D_;
   std::map<TString, std::map<int, ROOT::RDF::RResultPtr<TH2D>>> all_histos_2D_;
   std::vector<ROOT::RDF::RResultPtr<ROOT::RDF::RDFDetail::SumReturnType_t<double>>> cutflow_;

   std::vector<common::CutInfo> cuts_info_;

   common::SampleInfo sample_info_;
   Double_t sum_gen_wgt_;
   Double_t xsec_;
   Double_t lumi_;
   common::MODE mode_;
   TString group_;

   TH1F * sf_z_qcd, * sf_z_ewk;
   TH1F * sf_w_qcd, * sf_w_ewk;
   TH1F * sf_pu;

};

#endif

