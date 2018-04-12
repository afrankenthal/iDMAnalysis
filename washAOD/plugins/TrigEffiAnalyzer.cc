// -*- C++ -*-
//
// Package:    firefighter/washAOD
// Class:      TrigEffiAnalyzer
// 
/**\class washAOD TrigEffiAnalyzer.cc firefighter/washAOD/plugins/TrigEffiAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Weinan Si
//         Created:  Sat, 07 Apr 2018 02:14:41 GMT
//
//

#define EDM_ML_DEBUG
// system include files
#include <memory>
#include <map>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TTree.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class TrigEffiAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit TrigEffiAnalyzer(const edm::ParameterSet&);
      ~TrigEffiAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      struct theMu
      {
        theMu() {};

        template<typename T>
        theMu(const T& m) {
          _pt = m.pt();
          _eta = m.eta();
          _phi = m.phi();
          _dxy = std::sqrt(m.vx()*m.vx() + m.vy()*m.vy());
        }

        double _pt, _eta, _phi, _dxy;
      };


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      const edm::EDGetTokenT<edm::TriggerResults> trigResults;
      const std::vector<std::string> trigPaths;
      
      const edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
      const edm::EDGetTokenT<reco::TrackCollection> saMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dsaMuons;
      const edm::EDGetTokenT<reco::TrackCollection> rsaMuons;
      const edm::EDGetTokenT<reco::MuonCollection> muons;


      edm::Service<TFileService> fs;
      TTree *evt, *genparticle, *genaccept;


      theMu _genMu, _genMuAc;
      std::map<std::string, std::map<std::string, theMu>>  _triggeredMu;
      std::map<std::string, std::map<std::string, TTree*>> _triggeredMuTree;

      void fillTTree(TTree* t, theMu& m);
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

void
TrigEffiAnalyzer::fillTTree(TTree* t, theMu& m) {
  t->Branch("pt",  &m._pt,  "pt/D");
  t->Branch("eta", &m._eta, "eta/D");
  t->Branch("phi", &m._phi, "phi/D");
  t->Branch("dxy", &m._dxy, "dxy/D");
}

//
// constructors and destructor
//
TrigEffiAnalyzer::TrigEffiAnalyzer(const edm::ParameterSet& iC)
:
trigResults(consumes<edm::TriggerResults>(iC.getParameter<edm::InputTag>("_trigResults"))),
trigPaths(iC.getUntrackedParameter<std::vector<std::string>>("_trigPaths")),
genParticles(consumes<reco::GenParticleCollection>(iC.getParameter<edm::InputTag>("_genParticles"))),
saMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_saMuons"))),
dsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_dsaMuons"))),
rsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_rsaMuons"))),
muons(consumes<reco::MuonCollection>(iC.getParameter<edm::InputTag>("_muons")))
{
   //now do what ever initialization is needed
   usesResource("TFileService");

}


TrigEffiAnalyzer::~TrigEffiAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TrigEffiAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  for (const auto& p : *genParticlesH) {
    if (abs(p.pdgId()) != 13) continue;
    _genMu = theMu(p);
    genparticle->Fill();
  }

  auto passEta = [](const auto& p){return abs(p.pdgId()) == 13 && abs(p.eta()) < 2.4;};
  bool acceptance = std::count_if(cbegin(*genParticlesH), cend(*genParticlesH), passEta) >= 4;
  if (!acceptance) return;
  
  for (const auto& p : *genParticlesH) {
    if(abs(p.pdgId() != 13)) continue;
    _genMuAc = theMu(p);
    genaccept->Fill();
  }

 

  Handle<reco::TrackCollection> saMuonsH, dsaMuonsH, rsaMuonsH;
  Handle<reco::MuonCollection> recoMuonsH;
  iEvent.getByToken(saMuons, saMuonsH);
  iEvent.getByToken(dsaMuons, dsaMuonsH);
  iEvent.getByToken(rsaMuons, rsaMuonsH);
  iEvent.getByToken(muons, recoMuonsH);

  Handle<edm::TriggerResults> trigResultsH;
  iEvent.getByToken(trigResults, trigResultsH);

  const edm::TriggerNames& trigNames = iEvent.triggerNames(*trigResultsH);
   

  for (const auto& tp : trigPaths) {
    if (!trigResultsH->accept(trigNames.triggerIndex(tp))) continue;
    std::cout<< "-------------pass "<<tp<<"-------------"<<std::endl;
    for (const auto& mu : *saMuonsH) {
      _triggeredMu[tp]["saMu"] = theMu(mu);
      _triggeredMuTree[tp]["saMu"]->Fill();
    }
    for (const auto& mu : *dsaMuonsH) {
      _triggeredMu[tp]["dsaMu"] = theMu(mu);
      _triggeredMuTree[tp]["dsaMu"]->Fill();
    }
    for (const auto& mu : *rsaMuonsH) {
      _triggeredMu[tp]["rsaMu"] = theMu(mu);
      _triggeredMuTree[tp]["rsaMu"]->Fill();
    }
    for (const auto& mu : *recoMuonsH) {
      _triggeredMu[tp]["recoMu"] = theMu(mu);
      _triggeredMuTree[tp]["recoMu"]->Fill();
    } 
  }


}


// ------------ method called once each job just before starting event loop  ------------
void 
TrigEffiAnalyzer::beginJob()
{
  evt = fs->make<TTree>("Event", "Event information");
  genparticle = fs->make<TTree>("genParticles", "gen muons");
  fillTTree(genparticle, _genMu);

  genaccept = fs->make<TTree>("genAccepted", "accpted gen muons");
  fillTTree(genaccept, _genMuAc);

  for (const auto& p : trigPaths) {
    TFileDirectory tpDir = fs->mkdir(p.c_str());
    
    _triggeredMuTree[p]["saMu"]  = tpDir.make<TTree>("standAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["saMu"], _triggeredMu[p]["saMu"]);
    
    _triggeredMuTree[p]["rsaMu"] = tpDir.make<TTree>("refittedStandAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["rsaMu"], _triggeredMu[p]["rsaMu"]);

    _triggeredMuTree[p]["dsaMu"] = tpDir.make<TTree>("displacedStandAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["dsaMu"], _triggeredMu[p]["dsaMu"]);

    _triggeredMuTree[p]["recoMu"]= tpDir.make<TTree>("recoMuons", "");
    fillTTree(_triggeredMuTree[p]["recoMu"], _triggeredMu[p]["recoMu"]);
  }
  
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrigEffiAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
TrigEffiAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  // desc.setUnknown();
  desc.add<edm::InputTag>("_trigResults", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("_genParticles",edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("_saMuons",     edm::InputTag("standAloneMuons"));
  desc.add<edm::InputTag>("_rsaMuons",    edm::InputTag("refittedStandAloneMuons"));
  desc.add<edm::InputTag>("_dsaMuons",    edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("_muons",       edm::InputTag("muons"));

  std::vector<std::string> trigpaths;
  desc.addUntracked<std::vector<std::string>>("_trigPaths", trigpaths);

  // descriptions.addDefault(desc);
  descriptions.add("trigeffiana", desc);
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TrigEffiAnalyzer);
