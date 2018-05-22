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
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
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
      const edm::EDGetTokenT<trigger::TriggerEvent> trigEvent;
      const std::vector<std::string> trigFilters;
      
      const edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
      const edm::EDGetTokenT<reco::TrackCollection> saMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dsaMuons;
      const edm::EDGetTokenT<reco::TrackCollection> rsaMuons;
      const edm::EDGetTokenT<reco::TrackCollection> gMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dgMuons;
      const edm::EDGetTokenT<reco::MuonCollection> muons;


      edm::Service<TFileService> fs;

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
trigEvent(consumes<trigger::TriggerEvent>(iC.getParameter<edm::InputTag>("_trigEvent"))),
trigFilters(iC.getUntrackedParameter<std::vector<std::string>>("_trigFilters")),
genParticles(consumes<reco::GenParticleCollection>(iC.getParameter<edm::InputTag>("_genParticles"))),
saMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_saMuons"))),
dsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_dsaMuons"))),
rsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_rsaMuons"))),
gMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_globalMuons"))),
dgMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_dgMuons"))),
muons(consumes<reco::MuonCollection>(iC.getParameter<edm::InputTag>("_muons")))
{
   //now do what ever initialization is needed
   usesResource("TFileService");
   // Trigger paths and filters must have one-to-one correspondence.
   assert(trigPaths.size() == trigFilters.size());
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

  Handle<reco::TrackCollection> saMuonsH, dsaMuonsH, rsaMuonsH, gMuonsH, dgMuonsH;
  Handle<reco::MuonCollection> recoMuonsH;
  iEvent.getByToken(saMuons, saMuonsH);
  iEvent.getByToken(dsaMuons, dsaMuonsH);
  iEvent.getByToken(rsaMuons, rsaMuonsH);
  iEvent.getByToken(gMuons, gMuonsH);
  iEvent.getByToken(dgMuons, dgMuonsH);
  iEvent.getByToken(muons, recoMuonsH);

  for (const auto& mu : *saMuonsH) {
    _triggeredMu["Total"]["saMu"] = theMu(mu);
    _triggeredMuTree["Total"]["saMu"]->Fill();
  }
  for (const auto& mu : *dsaMuonsH) {
    _triggeredMu["Total"]["dsaMu"] = theMu(mu);
    _triggeredMuTree["Total"]["dsaMu"]->Fill();
  }
  for (const auto& mu : *rsaMuonsH) {
    _triggeredMu["Total"]["rsaMu"] = theMu(mu);
    _triggeredMuTree["Total"]["rsaMu"]->Fill();
  }
  for (const auto& mu : *gMuonsH) {
    _triggeredMu["Total"]["gMu"] = theMu(mu);
    _triggeredMuTree["Total"]["gMu"]->Fill();
  }
  for (const auto& mu : *dgMuonsH) {
    _triggeredMu["Total"]["dgMu"] = theMu(mu);
    _triggeredMuTree["Total"]["dgMu"]->Fill();
  }
  for (const auto& mu : *recoMuonsH) {
    _triggeredMu["Total"]["recoMu"] = theMu(mu);
    _triggeredMuTree["Total"]["recoMu"]->Fill();
  }

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  auto passEtaPtgt6 = [](const auto& p){return abs(p.pdgId()) == 13 && abs(p.eta()) < 2.4 && p.pt() > 6;};
  auto passEtaPtgt16 = [](const auto& p){return abs(p.pdgId()) == 13 && abs(p.eta()) < 2.4 && p.pt() > 16;};
  bool acceptance = std::count_if(cbegin(*genParticlesH), cend(*genParticlesH), passEtaPtgt6) >= 3
                  &&std::count_if(cbegin(*genParticlesH), cend(*genParticlesH), passEtaPtgt16) >=1;
  if (!acceptance) return;


  for (const auto& mu : *saMuonsH) {
    _triggeredMu["Accepted"]["saMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["saMu"]->Fill();
  }
  for (const auto& mu : *dsaMuonsH) {
    _triggeredMu["Accepted"]["dsaMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["dsaMu"]->Fill();
  }
  for (const auto& mu : *rsaMuonsH) {
    _triggeredMu["Accepted"]["rsaMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["rsaMu"]->Fill();
  }
  for (const auto& mu : *gMuonsH) {
    _triggeredMu["Accepted"]["gMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["gMu"]->Fill();
  }
  for (const auto& mu : *dgMuonsH) {
    _triggeredMu["Accepted"]["dgMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["dgMu"]->Fill();
  }
  for (const auto& mu : *recoMuonsH) {
    _triggeredMu["Accepted"]["recoMu"] = theMu(mu);
    _triggeredMuTree["Accepted"]["recoMu"]->Fill();
  }

 

  Handle<edm::TriggerResults> trigResultsH;
  iEvent.getByToken(trigResults, trigResultsH);
  Handle<trigger::TriggerEvent> trigEventH;
  iEvent.getByToken(trigEvent, trigEventH);

  // for (trigger::size_type i(0); i!=trigEventH->sizeFilters(); ++i) {
  //   std::cout<< trigEventH->filterLabel(i) << std::endl;
  // }
  // std::cout<< "------------- This is a trigger event-------------"<<std::endl;

  const edm::TriggerNames& trigNames = iEvent.triggerNames(*trigResultsH);

  for (const auto& tp : trigPaths) {
    if (!trigResultsH->accept(trigNames.triggerIndex(tp))) continue;
    // std::cout<< "-------------pass "<<tp<<"-------------"<<std::endl;
    
    auto index = &tp - &trigPaths[0];
    std::string filterName(trigFilters[index]);
    trigger::size_type filterIndex = trigEventH->filterIndex(
      edm::InputTag(filterName,"",trigEventH->usedProcessName()));
    if (filterIndex < trigEventH->sizeFilters()) {
      const trigger::Keys& trigKeys = trigEventH->filterKeys(filterIndex);
      const trigger::TriggerObjectCollection& trigObjColl(trigEventH->getObjects());
      
      //customize part: look at scouting trigger with tri-trk muon pt cut.
      if (tp.find("DST") != std::string::npos) {
        int ptgt6(0), ptgt16(0);
        for (trigger::Keys::const_iterator keyIt=trigKeys.begin(); keyIt!=trigKeys.end(); ++keyIt) {
          if (trigObjColl[*keyIt].pt() >= 6) {ptgt6++;}
          if (trigObjColl[*keyIt].pt() >= 16) {ptgt16++;}
        }
        if (!(ptgt16>0 && ptgt6>1)) continue;
      }

      for (trigger::Keys::const_iterator keyIt=trigKeys.begin(); keyIt!=trigKeys.end(); ++keyIt) {
        const trigger::TriggerObject& obj = trigObjColl[*keyIt];
        _triggeredMu[tp]["trigObj"] = theMu(obj.particle());
        _triggeredMuTree[tp]["trigObj"]->Fill();
      }
    }


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
    for (const auto& mu : *gMuonsH) {
      _triggeredMu[tp]["gMu"] = theMu(mu);
      _triggeredMuTree[tp]["gMu"]->Fill();
    }
    for (const auto& mu : *dgMuonsH) {
      _triggeredMu[tp]["dgMu"] = theMu(mu);
      _triggeredMuTree[tp]["dgMu"]->Fill();
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

  std::vector<std::string> dirnames{"Total", "Accepted"};
  dirnames.insert(dirnames.end(), trigPaths.begin(), trigPaths.end());
 
  for (const auto& p : dirnames) {
    TFileDirectory tpDir = fs->mkdir(p.c_str());
    
    // trigger objects only exist for trigger paths
    if (std::find(trigPaths.begin(), trigPaths.end(), p) != trigPaths.end()) {
      _triggeredMuTree[p]["trigObj"] = tpDir.make<TTree>("triggerObjects", "");
      fillTTree(_triggeredMuTree[p]["trigObj"], _triggeredMu[p]["trigObj"]);
    }

    _triggeredMuTree[p]["saMu"]  = tpDir.make<TTree>("standAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["saMu"], _triggeredMu[p]["saMu"]);
    
    _triggeredMuTree[p]["rsaMu"] = tpDir.make<TTree>("refittedStandAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["rsaMu"], _triggeredMu[p]["rsaMu"]);

    _triggeredMuTree[p]["dsaMu"] = tpDir.make<TTree>("displacedStandAloneMuons", "");
    fillTTree(_triggeredMuTree[p]["dsaMu"], _triggeredMu[p]["dsaMu"]);

    _triggeredMuTree[p]["gMu"] = tpDir.make<TTree>("globalMuons", "");
    fillTTree(_triggeredMuTree[p]["gMu"], _triggeredMu[p]["gMu"]);

    _triggeredMuTree[p]["dgMu"] = tpDir.make<TTree>("displacedGlobalMuons", "");
    fillTTree(_triggeredMuTree[p]["dgMu"], _triggeredMu[p]["dgMu"]);

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
  desc.add<edm::InputTag>("_trigEvent",   edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<edm::InputTag>("_genParticles",edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("_saMuons",     edm::InputTag("standAloneMuons"));
  desc.add<edm::InputTag>("_rsaMuons",    edm::InputTag("refittedStandAloneMuons"));
  desc.add<edm::InputTag>("_dsaMuons",    edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("_globalMuons", edm::InputTag("globalMuons"));
  desc.add<edm::InputTag>("_dgMuons",     edm::InputTag("displacedGlobalMuons"));
  desc.add<edm::InputTag>("_muons",       edm::InputTag("muons"));

  std::vector<std::string> trigpaths, trigfilters;
  desc.addUntracked<std::vector<std::string>>("_trigPaths", trigpaths);
  desc.addUntracked<std::vector<std::string>>("_trigFilters", trigfilters);

  // descriptions.addDefault(desc);
  descriptions.add("trigeffiana", desc);
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TrigEffiAnalyzer);
