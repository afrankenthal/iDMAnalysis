//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 26 23:49:46 2019 by ROOT version 6.12/07
// from TTree recoT/
// found on file: root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_thirdrun/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/crab_iDMAnalysis_QCD_bEnriched_HT700to1000/190722_042845/0000/merged_39e87f58-7dd3-4764-9ef7-df13c05061c3_06.root
//////////////////////////////////////////////////////////

#ifndef CosmicMuonAnalysis_h
#define CosmicMuonAnalysis_h

#include <map>
#include <vector>

#include <TH1D.h>
#include <TH2D.h>
#include <ROOT/RDF/InterfaceUtils.hxx>
#include <ROOT/RDataFrame.hxx>
template<typename T>
using RDFSumReturnType_t = ROOT::RDF::RDFDetail::SumReturnType_t<T>;
//typedef ROOT::RDF::RDFDetail::SumReturnType_t<double> RDFSumReturnType_t;
template<typename T>
using RDFResultPtrSumType = ROOT::RDF::RResultPtr<RDFSumReturnType_t<T>>;
using RDFResultPtrSumType_d = RDFResultPtrSumType<double>;
//typedef ROOT::RDF::RResultPtr<RDFSumReturnType_t> RDFResultPtrSumType;
template<typename T>
using RDFResultPtr = ROOT::RDF::RResultPtr<T>;
using RDFResultPtr1D = RDFResultPtr<TH1D>;
using RDFResultPtr2D = RDFResultPtr<TH2D>;
//typedef ROOT::RDF::RResultPtr<TH1D> RDFResultPtr1D;
//typedef ROOT::RDF::RResultPtr<TH2D> RDFResultPtr2D;
#include <TChain.h>
#include <TH1F.h>

#include "../utils/common.h"
#include "../utils/json.hpp"
using json = nlohmann::json;


class CosmicMuonAnalysis {
public :
   CosmicMuonAnalysis() { }
   ~CosmicMuonAnalysis() { }

   void    Begin();
   Bool_t  Process(TChain * chain);
   void    Terminate();

   void SetMacroConfig(json macro_info);
   void SetHistoConfig(std::map<TString, common::THInfo*> histos_info);
   void SetCutConfig(std::vector<common::CutInfo> cuts_info);
   void SetSampleConfig(common::SampleInfo sample_info);

   std::vector<RDFResultPtrSumType_d> GetCutflow() { return cutflow_; }
   std::map<TString, std::map<int, RDFResultPtr1D>> GetHistograms1D() { return all_histos_1D_; }
   std::map<TString, std::map<int, RDFResultPtr2D>> GetHistograms2D() { return all_histos_2D_; }

   TChain * chain_;

   std::map<TString, std::map<int, RDFResultPtr1D>> all_histos_1D_;
   std::map<TString, std::map<int, RDFResultPtr2D>> all_histos_2D_;
   std::vector<RDFResultPtrSumType_d> cutflow_;

   json macro_info_;
   std::map<TString, common::THInfo*> histos_info_;
   std::vector<common::CutInfo> cuts_info_;
   common::SampleInfo sample_info_;

   TString name_;
   Double_t sum_gen_wgt_;
   Double_t xsec_;
   Double_t lumi_;
   Double_t custom_lumi_;
   int year_;
   //float trig_wgt;
   common::MODE mode_;
   TString group_;
};

#endif
