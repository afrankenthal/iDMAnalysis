// -*- C++ -*-
//
// Package:    firefighter/washAOD
// Class:      DecayLengthAnalyzer
// 
/**\class washAOD DecayLengthAnalyzer.cc firefighter/washAOD/plugins/DecayLengthAnalyzer.cc

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

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

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

class DecayLengthAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit DecayLengthAnalyzer(const edm::ParameterSet&);
      ~DecayLengthAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
      
      struct theMu {
        theMu() {}
        template<typename T>
        theMu(const T& m) {
          _pt = m.pt();
          _eta = m.eta();
          _phi = m.phi();
          _dxy = std::sqrt(m.vx()*m.vx() + m.vy()*m.vy());
          _dz = m.vz();
          _dl = std::sqrt(_dxy*_dxy + _dz*_dz);
        }
        double _pt, _eta, _phi, _dxy, _dz, _dl;
      };

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      const edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
      const edm::EDGetTokenT<reco::TrackCollection> tracks;
      const edm::EDGetTokenT<reco::TrackCollection> globalMuons;
      const edm::EDGetTokenT<reco::TrackCollection> saMuons;
      const edm::EDGetTokenT<reco::TrackCollection> saMuonsUAV;
      const edm::EDGetTokenT<reco::TrackCollection> rsaMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dgMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dsaMuons;
      const edm::EDGetTokenT<reco::MuonCollection> muons;

      void branchTTree(TTree*, theMu&);
      edm::Service<TFileService> fs;
      std::map<std::string, TTree*> _basicMuTree;
      std::map<std::string, theMu> _basicMu;
      TTree *evt, *DiMuon;

      double _dimuondr;

};

void
DecayLengthAnalyzer::branchTTree(TTree* t, theMu& m) {
  t->Branch("pt",  &m._pt,  "pt/D");
  t->Branch("eta", &m._eta, "eta/D");
  t->Branch("phi", &m._phi, "phi/D");
  t->Branch("dxy", &m._dxy, "dxy/D");
  t->Branch("dz",  &m._dz,  "dz/D");
  t->Branch("dl",  &m._dl,  "dl/D");
}
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DecayLengthAnalyzer::DecayLengthAnalyzer(const edm::ParameterSet& iC)
:
genParticles(consumes<reco::GenParticleCollection>(iC.getParameter<edm::InputTag>("_genParticles"))),
tracks(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_tracks"))),
globalMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_globalMuons"))),
saMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_saMuons"))),
saMuonsUAV(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_saMuonsUAV"))),
rsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_rsaMuons"))),
dgMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_dgMuons"))),
dsaMuons(consumes<reco::TrackCollection>(iC.getParameter<edm::InputTag>("_dsaMuons"))),
muons(consumes<reco::MuonCollection>(iC.getParameter<edm::InputTag>("_muons")))
{
   //now do what ever initialization is needed
   usesResource("TFileService");

}


DecayLengthAnalyzer::~DecayLengthAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
DecayLengthAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  
  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);
  
  Handle<reco::TrackCollection> tracksH;
  iEvent.getByToken(tracks, tracksH);
  
  Handle<reco::TrackCollection> globalMuonsH, saMuonsH, saMuonsUAVH, rsaMuonsH, dgMuonsH, dsaMuonsH;
  iEvent.getByToken(globalMuons, globalMuonsH);
  iEvent.getByToken(saMuons, saMuonsH);
  iEvent.getByToken(saMuonsUAV, saMuonsUAVH);
  iEvent.getByToken(rsaMuons, rsaMuonsH);
  iEvent.getByToken(dgMuons, dgMuonsH);
  iEvent.getByToken(dsaMuons, dsaMuonsH);

  Handle<reco::MuonCollection> muonsH;
  iEvent.getByToken(muons, muonsH);
  LogDebug("DecayLengthAnalyzer") << "number of muons "<<muonsH->size()
                                  << "number of globalMuons "<<globalMuonsH->size() 
                                  << "number of tracks "<<tracksH->size();
  
  // reco::Muon dimuon
  for (size_t imu(0); imu!=muonsH->size(); ++imu) {
    Ptr<reco::Muon> MuA(muonsH, imu);
    for (size_t jmu(imu+1); jmu!=muonsH->size(); ++jmu) {
      Ptr<reco::Muon> MuB(muonsH, jmu);
      if (MuA->charge() * MuB->charge() > 0)  continue;
      _dimuondr = deltaR(*MuA.get(), *MuB.get());
      DiMuon->Fill();
    }
  }

  // gen muon
  for (const auto& mu : *genParticlesH) {
    if (abs(mu.pdgId()) != 13) continue;
    _basicMu["genMu"] = theMu(mu);
    _basicMuTree["genMu"]->Fill();
  }

  // reco::Muon
  for (const auto& mu : *muonsH) {
    _basicMu["recoMu"] = theMu(mu);
    _basicMuTree["recoMu"]->Fill();
  }

  // global muon
  for (const auto& mu : *globalMuonsH) {
    _basicMu["globalMu"] = theMu(mu);
    _basicMuTree["globalMu"]->Fill();
  }

  // standalone muon
  for (const auto& mu : *saMuonsH) {
    _basicMu["saMu"] = theMu(mu);
    _basicMuTree["saMu"]->Fill();
  }

  // standalone muon updatedAtVertex
  for (const auto& mu : *saMuonsUAVH) {
    _basicMu["saMuUAV"] = theMu(mu);
    _basicMuTree["saMuUAV"]->Fill();
  }

  // refitted standalone muon
  for (const auto& mu : *rsaMuonsH) {
    _basicMu["rsaMu"] = theMu(mu);
    _basicMuTree["rsaMu"]->Fill();
  }

  // displaced global muon
  for (const auto& mu : *dgMuonsH) {
    _basicMu["dgMu"] = theMu(mu);
    _basicMuTree["dgMu"]->Fill();
  }

  // displaced standalone muon
  for (const auto& mu : *dsaMuonsH) {
    _basicMu["dsaMu"] = theMu(mu);
    _basicMuTree["dsaMu"]->Fill();
  }
}


// ------------ method called once each job just before starting event loop  ------------
void 
DecayLengthAnalyzer::beginJob()
{
  evt = fs->make<TTree>("Event", "Event information");
 
  DiMuon = fs->make<TTree>("DiMuon", "Two muons");
  DiMuon->Branch("dimuondR", &_dimuondr, "dimuondR/D");

  _basicMuTree["genMu"] = fs->make<TTree>("GenMuon", "gen muon");
  branchTTree(_basicMuTree["genMu"], _basicMu["genMu"]);

  _basicMuTree["recoMu"] = fs->make<TTree>("RecoMuon", "reco::Muon");
  branchTTree(_basicMuTree["recoMu"], _basicMu["recoMu"]);

  _basicMuTree["globalMu"] = fs->make<TTree>("GlobalMuon", "globalMuon");
  branchTTree(_basicMuTree["globalMu"], _basicMu["globalMu"]);

  _basicMuTree["saMu"] = fs->make<TTree>("StandAloneMuon", "standalone muon");
  branchTTree(_basicMuTree["saMu"], _basicMu["saMu"]);

  _basicMuTree["saMuUAV"] = fs->make<TTree>("StandAloneMuonUpdatedAtVertex", "");
  branchTTree(_basicMuTree["saMuUAV"], _basicMu["saMuUAV"]);

  _basicMuTree["rsaMu"] = fs->make<TTree>("RefittedStandAloneMuon", "refitted standalone muon");
  branchTTree(_basicMuTree["rsaMu"], _basicMu["rsaMu"]);

  _basicMuTree["dgMu"] = fs->make<TTree>("DisplacedGlobalMuon", "displaced global muon");
  branchTTree(_basicMuTree["dgMu"], _basicMu["dgMu"]);

  _basicMuTree["dsaMu"] = fs->make<TTree>("DisplacedStandAloneMuon", "displaced standalone muon");
  branchTTree(_basicMuTree["dsaMu"], _basicMu["dsaMu"]);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
DecayLengthAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
DecayLengthAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  // desc.setUnknown();
  desc.add<edm::InputTag>("_genParticles",edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("_tracks",      edm::InputTag("generalTracks"));
  desc.add<edm::InputTag>("_globalMuons", edm::InputTag("globalMuons"));
  desc.add<edm::InputTag>("_saMuons",     edm::InputTag("standAloneMuons"));
  desc.add<edm::InputTag>("_saMuonsUAV",  edm::InputTag("standAloneMuons","UpdatedAtVtx"));
  desc.add<edm::InputTag>("_rsaMuons",    edm::InputTag("refittedStandAloneMuons"));
  desc.add<edm::InputTag>("_dgMuons",     edm::InputTag("displacedGlobalMuons"));
  desc.add<edm::InputTag>("_dsaMuons",    edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("_muons",       edm::InputTag("muons"));
  // descriptions.addDefault(desc);
  descriptions.add("decaylengthana", desc);
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DecayLengthAnalyzer);
