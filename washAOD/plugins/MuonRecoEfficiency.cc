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

// #include "DataFormats/Common/interface/Ref.h"
// #include "DataFormats/Math/interface/LorentzVector.h"
// #include "DataFormats/Math/interface/deltaR.h"

#include "TTree.h"

class MuonRecoEfficiency : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit MuonRecoEfficiency(const edm::ParameterSet&);
  ~MuonRecoEfficiency();
    
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  struct theMu {
    theMu() {}

    template<typename C>
    theMu(const C& cand) {
      _pt = cand->pt();
      _eta = cand->eta();
      _phi = cand->phi();
      _dxy = cand->dxy();
      _dz = cand->dz();
    }

    double _pt, _eta, _phi, _dxy, _dz;
  };

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  void branchTTree(TTree*, theMu&);

  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  // edm::EDGetTokenT<reco::MuonCollection> muons;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> globalMuons;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> saMuons;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> saMuonsUAV;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> rsaMuons;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> dgMuons;
  // edm::EDGetTokenT<reco::RecoChargedCandidateCollection> dsaMuons;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> muTrack;
  edm::EDGetTokenT<reco::GenParticleMatch> allMuMatch;

  edm::Service<TFileService> fs;
  std::map<std::string, theMu> _basicMu;
  std::map<std::string, TTree*> _basicMuTree;
  unsigned int nmuons;
};

void
MuonRecoEfficiency::branchTTree(TTree* t, theMu& m) {
  t->Branch("pt",  &m._pt, "pt/D");
  t->Branch("eta", &m._eta, "eta/D");
  t->Branch("phi", &m._phi, "phi/D");
  t->Branch("dxy", &m._dxy, "dxy/D");
  t->Branch("dz",  &m._dz,  "dz/D");
}


MuonRecoEfficiency::MuonRecoEfficiency(const edm::ParameterSet& iC)
:
genParticles(consumes<reco::GenParticleCollection>(iC.getParameter<edm::InputTag>("_genParticles"))),
muTrack(consumes<reco::RecoChargedCandidateCollection>(iC.getParameter<edm::InputTag>("_muTrack"))),
allMuMatch(consumes<reco::GenParticleMatch>(iC.getParameter<edm::InputTag>("_mergedMCMatch"))),
nmuons(iC.getParameter<unsigned int>("_nmuons"))
{
  usesResource("TFileService");

  // genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
  // muons        = consumes<reco::MuonCollection>(edm::InputTag("muons"));
  // globalMuons  = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("globalMuTrackCands"));
  // saMuons      = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("saMuTrackCands"));
  // saMuonsUAV   = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("saMuUAVTrackCands"));
  // rsaMuons     = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("rsaMuTrackCands"));
  // dgMuons      = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("dgMuTrackCands"));
  // dsaMuons     = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("dsaMuTrackCands"));
  
  // allMuMatch   = consumes<reco::GenParticleMatch>(edm::InputTag("mergedMCMatch"));
}

MuonRecoEfficiency::~MuonRecoEfficiency()
{
}

void
MuonRecoEfficiency::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  Handle<reco::RecoChargedCandidateCollection> muTrackH;
  iEvent.getByToken(muTrack, muTrackH);

  Handle<reco::GenParticleMatch> allMuMatchH;
  iEvent.getByToken(allMuMatch, allMuMatchH);
  
  // gen geometry acceptance
  unsigned int acceptance = std::count_if(genParticlesH->begin(), genParticlesH->end(),
          [](const auto& mu) {return abs(mu.pdgId())==13 && abs(mu.eta())<2.4;});
  if (acceptance<4) return;


  /* denominator */
  for (size_t iMu(0); iMu!=muTrackH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(muTrackH, iMu);
    _basicMu["denom"] = theMu(muRef->track());
    _basicMuTree["denom"]->Fill();
  }

  unsigned int nRecoed(0);
  for (size_t iMu(0); iMu!=muTrackH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(muTrackH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];
    if (muGen.isNonnull()) {++nRecoed;}
  }

  if (nRecoed < nmuons) {return;}

  /* numerator */
  for (size_t iMu(0); iMu!=muTrackH->size(); ++iMu) {
    reco::RecoChargedCandidateRef muRef(muTrackH, iMu);
    reco::GenParticleRef muGen = (*allMuMatchH)[muRef];
    if (muGen.isNonnull()) {
      _basicMu["numer"] = theMu(muRef->track());
      _basicMuTree["numer"]->Fill();
    }
  }

}

void
MuonRecoEfficiency::beginJob()
{
  _basicMuTree["denom"] = fs->make<TTree>("denom","");
  branchTTree(_basicMuTree["denom"], _basicMu["denom"]);

  _basicMuTree["numer"] = fs->make<TTree>("numer","");
  branchTTree(_basicMuTree["numer"], _basicMu["numer"]);
}

void
MuonRecoEfficiency::endJob()
{
}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
MuonRecoEfficiency::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("_genParticles", edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("_mergedMCMatch", edm::InputTag("mergedMCMatch"));
  desc.add<edm::InputTag>("_muTrack", edm::InputTag("globalMuTrackCands"));
  desc.add<unsigned int>("_nmuons", 4);
  
  descriptions.add("muonrecoeffi", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MuonRecoEfficiency);