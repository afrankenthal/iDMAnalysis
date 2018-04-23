#include <memory>
#include <map>

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
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"

#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TTree.h"

class muMCAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit muMCAnalyzer(const edm::ParameterSet&);
  ~muMCAnalyzer();
    
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  struct theMu {
    theMu() {}
    template<typename C, typename G>
    theMu(const C& cand, const G& gen) {
      _pt = cand->pt();
      _ptGen = gen->pt();
      _res = abs(_pt-_ptGen)/_ptGen;
    }

    template<typename C>
    theMu(const C& cand) {
      _pt = cand->pt();
      _ptGen = -1.;
      _res = -1.;
    }

    double _pt, _ptGen, _res;
  };

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  void branchTTree(TTree*, theMu&);

  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  edm::EDGetTokenT<reco::MuonCollection> muons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> globalMuons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> saMuons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> saMuonsUAV;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> rsaMuons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> dgMuons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> dsaMuons;
  edm::EDGetTokenT<reco::GenParticleMatch> allMuMatch;

  edm::Service<TFileService> fs;
  std::map<std::string, theMu> _basicMu;
  std::map<std::string, TTree*> _basicMuTree;
};

void
muMCAnalyzer::branchTTree(TTree* t, theMu& m) {
  t->Branch("pt",    &m._pt, "pt/D");
  t->Branch("ptGen", &m._ptGen, "ptGen/D");
  t->Branch("res",   &m._res, "res/D");
}


muMCAnalyzer::muMCAnalyzer(const edm::ParameterSet& iC)
{
  usesResource("TFileService");

  genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
  muons        = consumes<reco::MuonCollection>(edm::InputTag("muons"));
  globalMuons  = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("globalMuTrackCands"));
  saMuons      = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("saMuTrackCands"));
  saMuonsUAV   = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("saMuUAVTrackCands"));
  rsaMuons     = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("rsaMuTrackCands"));
  dgMuons      = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("dgMuTrackCands"));
  dsaMuons     = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("dsaMuTrackCands"));
  
  allMuMatch   = consumes<reco::GenParticleMatch>(edm::InputTag("mergedMCMatch"));
}

muMCAnalyzer::~muMCAnalyzer()
{
}

void
muMCAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  Handle<reco::RecoChargedCandidateCollection> globalMuonsH, saMuonsUAVH, rsaMuonsH, dgMuonsH, dsaMuonsH, saMuonsH;
  iEvent.getByToken(globalMuons, globalMuonsH);
  iEvent.getByToken(saMuons, saMuonsH);
  iEvent.getByToken(saMuonsUAV, saMuonsUAVH);
  iEvent.getByToken(rsaMuons, rsaMuonsH);
  iEvent.getByToken(dgMuons, dgMuonsH);
  iEvent.getByToken(dsaMuons, dsaMuonsH);

  Handle<reco::MuonCollection> muonsH;
  iEvent.getByToken(muons, muonsH);

  Handle<reco::GenParticleMatch> allMuMatchH;
  iEvent.getByToken(allMuMatch, allMuMatchH);
  
  for (size_t iMu(0); iMu!=muonsH->size(); ++iMu) {
    Ref<reco::MuonCollection> muRef(muonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["recoMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["recoMu"] = theMu(muRef);
    }

    _basicMuTree["recoMu"]->Fill();
  }

  for (size_t iMu(0); iMu!=saMuonsH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(saMuonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["saMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["saMu"] = theMu(muRef);
    }

    _basicMuTree["saMu"]->Fill();
  }

  for (size_t iMu(0); iMu!=dsaMuonsH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(dsaMuonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["dsaMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["dsaMu"] = theMu(muRef);
    }

    _basicMuTree["dsaMu"]->Fill();
  }

  for (size_t iMu(0); iMu!=rsaMuonsH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(rsaMuonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["rsaMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["rsaMu"] = theMu(muRef);
    }

    _basicMuTree["rsaMu"]->Fill();
  }

  for (size_t iMu(0); iMu!=saMuonsUAVH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(saMuonsUAVH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["saMuUAV"] = theMu(muRef, muGen);
    } else {
      _basicMu["saMuUAV"] = theMu(muRef);
    }

    _basicMuTree["saMuUAV"]->Fill();
  }

  for (size_t iMu(0); iMu!=globalMuonsH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(globalMuonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["globalMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["globalMu"] = theMu(muRef);
    }

    _basicMuTree["globalMu"]->Fill();
  }

  for (size_t iMu(0); iMu!=dgMuonsH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(dgMuonsH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];

    if (muGen.isNonnull()) {
      _basicMu["dgMu"] = theMu(muRef, muGen);
    } else {
      _basicMu["dgMu"] = theMu(muRef);
    }

    _basicMuTree["dgMu"]->Fill();
  }
}

void
muMCAnalyzer::beginJob()
{
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

void
muMCAnalyzer::endJob()
{
}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
muMCAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.add("mumcana", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(muMCAnalyzer);