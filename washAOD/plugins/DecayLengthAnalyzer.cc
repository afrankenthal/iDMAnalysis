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


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      const edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
      const edm::EDGetTokenT<reco::TrackCollection> tracks;
      const edm::EDGetTokenT<reco::TrackCollection> globalMuons;
      const edm::EDGetTokenT<reco::TrackCollection> saMuons;
      const edm::EDGetTokenT<reco::TrackCollection> rsaMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dgMuons;
      const edm::EDGetTokenT<reco::TrackCollection> dsaMuons;
      const edm::EDGetTokenT<reco::MuonCollection> muons;

      edm::Service<TFileService> fs;
      TTree *evt, *GenMuon, *DiMuon, *RecoMuon, *GlobalMuon, *SAMuon, *RSAMuon, *DGMuon, *DSAMuon;

      struct vtx {
        vtx() {};
        vtx(double dxy, double dz) {
          _dxy = dxy;
          _dz = _dz;
          _dl = std::sqrt(_dxy*_dxy + _dz*_dz);
        };
        vtx(double vx, double vy, double vz) {
          _dxy = std::sqrt(vx*vx + vy*vy);
          _dz = vz;
          _dl = std::sqrt(_dxy*_dxy + _dz*_dz);
        }
        double _dxy;
        double _dz;
        double _dl;
      };
      double _dimuondr;

      vtx _genMuVtx, _recoMuVtx, _globalMuVtx, _saMuVtx, _rsaMuVtx, _dgMuVtx, _dsaMuVtx;
};

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
  
  Handle<reco::TrackCollection> globalMuonsH, saMuonsH, rsaMuonsH, dgMuonsH, dsaMuonsH;
  iEvent.getByToken(globalMuons, globalMuonsH);
  iEvent.getByToken(saMuons, saMuonsH);
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
    _genMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    GenMuon->Fill();
  }

  // reco::Muon
  for (const auto& mu : *muonsH) {
    _recoMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    RecoMuon->Fill();
  }

  // global muon
  for (const auto& mu : *globalMuonsH) {
    _globalMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    // LogInfo("DecayLengthAnalyzer")<<_globalMuVtx._dxy<<" dz:"<<_globalMuVtx._dz;
    GlobalMuon->Fill();
  }

  // standalone muon
  for (const auto& mu : *saMuonsH) {
    _saMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    SAMuon->Fill();
  }

  // refitted standalone muon
  for (const auto& mu : *rsaMuonsH) {
    _rsaMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    RSAMuon->Fill();
  }

  // displaced global muon
  for (const auto& mu : *dgMuonsH) {
    _dgMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    DGMuon->Fill();
  }

  // displaced standalone muon
  for (const auto& mu : *dsaMuonsH) {
    _dsaMuVtx = vtx(mu.vx(), mu.vy(), mu.vz());
    DSAMuon->Fill();
  }
}


// ------------ method called once each job just before starting event loop  ------------
void 
DecayLengthAnalyzer::beginJob()
{
  evt = fs->make<TTree>("Event", "Event information");

  
  DiMuon = fs->make<TTree>("DiMuon", "Two muons");
  DiMuon->Branch("dimuondR", &_dimuondr, "dimuondR/D");

  GenMuon = fs->make<TTree>("GenMuon", "gen muon");
  GenMuon->Branch("dxy", &_genMuVtx._dxy, "dxy/D");
  GenMuon->Branch("dz",  &_genMuVtx._dz,  "dz/D");
  GenMuon->Branch("dl",  &_genMuVtx._dl,  "dl/D");

  RecoMuon = fs->make<TTree>("RecoMuon", "reco::Muon");
  RecoMuon->Branch("dxy", &_recoMuVtx._dxy, "dxy/D");
  RecoMuon->Branch("dz",  &_recoMuVtx._dz,  "dz/D");
  RecoMuon->Branch("dl",  &_recoMuVtx._dl,  "dl/D");

  GlobalMuon = fs->make<TTree>("GlobalMuon", "globalMuon");
  GlobalMuon->Branch("dxy", &_globalMuVtx._dxy, "dxy/D");
  GlobalMuon->Branch("dz",  &_globalMuVtx._dz,  "dz/D");
  GlobalMuon->Branch("dl",  &_globalMuVtx._dl,  "dl/D");

  SAMuon = fs->make<TTree>("StandAloneMuon", "standalone muon");
  SAMuon->Branch("dxy", &_saMuVtx._dxy, "dxy/D");
  SAMuon->Branch("dz" , &_saMuVtx._dz , "dz/D");
  SAMuon->Branch("dl" , &_saMuVtx._dl , "dl/D");

  RSAMuon = fs->make<TTree>("RefittedStandAloneMuon", "refitted standalone muon");
  RSAMuon->Branch("dxy", &_rsaMuVtx._dxy, "dxy/D");
  RSAMuon->Branch("dz" , &_rsaMuVtx._dz , "dz/D");
  RSAMuon->Branch("dl" , &_rsaMuVtx._dl , "dl/D");

  DGMuon = fs->make<TTree>("DisplacedGlobalMuon", "displaced global muon");
  DGMuon->Branch("dxy", &_dgMuVtx._dxy, "dxy/D");
  DGMuon->Branch("dz",  &_dgMuVtx._dz,  "dz/D");
  DGMuon->Branch("dl",  &_dgMuVtx._dl,  "dl/D");

  DSAMuon = fs->make<TTree>("DisplacedStandAloneMuon", "displaced standalone muon");
  DSAMuon->Branch("dxy", &_dsaMuVtx._dxy, "dxy/D");
  DSAMuon->Branch("dz" , &_dsaMuVtx._dz , "dz/D");
  DSAMuon->Branch("dl" , &_dsaMuVtx._dl , "dl/D");
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
