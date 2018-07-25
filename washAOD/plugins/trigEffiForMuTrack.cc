#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

trigEffiForMuTrack::trigEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_))
{
  usesResource("TFileService");
}

trigEffiForMuTrack::~trigEffiForMuTrack() = default;

void
trigEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::string>("trigPath", "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v");
  desc.add<std::string>("processName", "HLT");
  descriptions.add("trigEffiForMuTrack", desc);
}

void
trigEffiForMuTrack::beginJob()
{
  muTrackT_ = fs->make<TTree>("trigEffiForMuTrack", "");

  muTrackT_->Branch("fired", &fired_, "fired/O");
  muTrackT_->Branch("pt",   &pt_);
  muTrackT_->Branch("eta",  &eta_);
  muTrackT_->Branch("phi",  &phi_);
}

void
trigEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("trigEffiForMuTrack")<<"trigEffiForMuTrack::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("trigEffiForMuTrack")<<"trigEffiForMuTrack::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
trigEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muTrackToken_, muTrackHandle_);
  if (!muTrackHandle_.isValid()) {
    LogVerbatim("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogVerbatim("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogVerbatim("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogVerbatim("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting triggerEvent product from Event!"
      << endl;
    return;
  }

  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){return abs(g.pdgId())==13 and g.isHardProcess() and abs(g.eta())<2.4;});
  if (nAccpted<4) return;
  if (muTrackHandle_->size()<4) return;

  // sort mu key by pT
  vector<int> muTrackIdx{};
  for (size_t i(0); i!=muTrackHandle_->size(); ++i) muTrackIdx.push_back(i);
  sort(muTrackIdx.begin(), muTrackIdx.end(),
      [&](int l, int r){
        reco::TrackRef lhs(muTrackHandle_, l);
        reco::TrackRef rhs(muTrackHandle_, r);
        return lhs->pt() > rhs->pt();
      });

  // MC match
  vector<int> matchedGenMuIdx{};
  for (const int muid : muTrackIdx) {
    reco::TrackRef recoMu(muTrackHandle_, muid);
    for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
      if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), ig) != matchedGenMuIdx.end()) continue;
      reco::GenParticleRef genMu(genParticleHandle_, ig);
      if (deltaR(*(recoMu.get()), *(genMu.get())) > 0.3) continue;
      if (recoMu->charge() != genMu->charge()) continue;
      matchedGenMuIdx.push_back(ig);
    }
  }
  if (matchedGenMuIdx.size()<4) return;

  pt_  .clear(); pt_  .reserve(4);
  eta_ .clear(); eta_ .reserve(4);
  phi_ .clear(); phi_ .reserve(4);

  for (const int muid : muTrackIdx) {
    reco::TrackRef recoMu(muTrackHandle_, muid);
    pt_ .push_back(recoMu->pt());
    eta_.push_back(recoMu->eta());
    phi_.push_back(recoMu->phi());
  }

  // trigger firing condition
  const vector<string>& pathNames = hltConfig_.triggerNames();
  const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
  if (matchedPaths.size() == 0) {
    LogError("trigEffiForMuTrack")<<"Could not find matched full trigger path with -> "<<trigPathNoVer_<<endl;
    return;
  }
  trigPath_ = matchedPaths[0];
  if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
    LogError("trigEffiForMuTrack")<<"Cannot find trigger path -> "<<trigPath_<<endl;
    return;
  }
  fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

  muTrackT_->Fill();

  return;
}

void
trigEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigEffiForMuTrack::endJob() {}
