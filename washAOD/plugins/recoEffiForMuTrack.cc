#include "Firefighter/washAOD/interface/recoEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

recoEffiForMuTrack::recoEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_))
{
  usesResource("TFileService");
}

recoEffiForMuTrack::~recoEffiForMuTrack() = default;

void
recoEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  descriptions.add("recoEffiForMuTrack", desc);
}

void
recoEffiForMuTrack::beginJob()
{
  muTrackT_ = fs->make<TTree>("recoEffiForMuTrack", "");

  muTrackT_->Branch("nMatched", &nMatched_, "nMatched/i");
  muTrackT_->Branch("genPt",   &genPt_);
  muTrackT_->Branch("genEta",  &genEta_);
  muTrackT_->Branch("genPhi",  &genPhi_);
  muTrackT_->Branch("recoPt",  &recoPt_);
  muTrackT_->Branch("recoEta", &recoEta_);
  muTrackT_->Branch("recoPhi", &recoPhi_);
  muTrackT_->Branch("deltaR",  &deltaR_);
}

void
recoEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muTrackToken_, muTrackHandle_);
  if (!muTrackHandle_.isValid()) {
    LogVerbatim("recoEffiForMuTrack") << "recoEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogVerbatim("recoEffiForMuTrack") << "recoEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }

  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){return abs(g.pdgId())==13 and g.isHardProcess() and abs(g.eta())<2.4;});
  if (nAccpted<4) return;

  genPt_  .clear(); genPt_  .reserve(4);
  genEta_ .clear(); genEta_ .reserve(4);
  genPhi_ .clear(); genPhi_ .reserve(4);
  recoPt_ .clear(); recoPt_ .reserve(4);
  recoEta_.clear(); recoEta_.reserve(4);
  recoPhi_.clear(); recoPhi_.reserve(4);
  deltaR_ .clear(); deltaR_ .reserve(4);

  // MC match
  vector<int> genMuIdx{};
  for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
    reco::GenParticleRef gp(genParticleHandle_, ig);
    if (abs(gp->pdgId())==13 and gp->isHardProcess() and abs(gp->eta())<2.4) {
      genMuIdx.push_back(ig);
    }
  }

  vector<int> matchedGenMuIdx{};
  for (const reco::Track& muTrack : *muTrackHandle_) {
    for (const int genMuid : genMuIdx) {
      if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), genMuid) != matchedGenMuIdx.end()) continue;
      reco::GenParticleRef genMu(genParticleHandle_, genMuid);
      if (deltaR(muTrack, *(genMu.get()))>0.3) continue;
      if (muTrack.charge()!=genMu->charge()) continue;
      matchedGenMuIdx.push_back(genMuid);

      genPt_  .push_back(genMu->pt());
      genEta_ .push_back(genMu->eta());
      genPhi_ .push_back(genMu->phi());
      recoPt_ .push_back(muTrack.pt());
      recoEta_.push_back(muTrack.eta());
      recoPhi_.push_back(muTrack.phi());
      deltaR_ .push_back(deltaR(muTrack, *(genMu.get())));
    }
  }
  nMatched_ = matchedGenMuIdx.size();

  muTrackT_->Fill();

  return;

}

void
recoEffiForMuTrack::endJob() {}
