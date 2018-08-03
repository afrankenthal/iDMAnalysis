#include "Firefighter/washAOD/interface/trigSelfEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

trigSelfEffiForMuTrack::trigSelfEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  nMuons_(ps.getParameter<int>("nMuons")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
  trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_))
{
  usesResource("TFileService");
}

trigSelfEffiForMuTrack::~trigSelfEffiForMuTrack() = default;

void
trigSelfEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::string>("trigPath", "HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx");
  desc.add<std::string>("processName", "HLT");
  desc.add<int>("nMuons", 3);
  descriptions.add("trigSelfEffiForMuTrack", desc);
}

void
trigSelfEffiForMuTrack::beginJob()
{
  muTrackT_ = fs->make<TTree>("trigSelfEffiForMuTrack", "");

  muTrackT_->Branch("fired", &fired_, "fired/O");
  muTrackT_->Branch("pt",   &pt_);
  muTrackT_->Branch("eta",  &eta_);
  muTrackT_->Branch("phi",  &phi_);
}

void
trigSelfEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  using namespace std;
  using namespace edm;

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      LogInfo("trigSelfEffiForMuTrack")<<"trigSelfEffiForMuTrack::beginRun: "<<"hltConfig init for Run"<<iRun.run();
      hltConfig_.dump("ProcessName");
      hltConfig_.dump("GlobalTag");
      hltConfig_.dump("TableName");
    }
  } else {
    LogError("trigSelfEffiForMuTrack")<<"trigSelfEffiForMuTrack::beginRun: config extraction failure with processName -> "
      <<processName_;
  }

}

void
trigSelfEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muTrackToken_, muTrackHandle_);
  if (!muTrackHandle_.isValid()) {
    LogError("trigSelfEffiForMuTrack")
      << "trigSelfEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogError("trigSelfEffiForMuTrack")
      << "trigSelfEffiForMuTrack::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogError("trigSelfEffiForMuTrack")
      << "trigSelfEffiForMuTrack::analyze: Error in getting triggerEvent product from Event!"
      << endl;
    return;
  }

  vector<reco::TrackRef> muRefs{};
  for (size_t i(0); i!=muTrackHandle_->size(); ++i) {
    muRefs.emplace_back(muTrackHandle_, i);
  }

  /* general selection */
  auto generalSelection = [](const auto& t){
    bool pass = t->pt() > 5
             && abs(t->eta()) < 2
             && t->hitPattern().numberOfValidMuonHits() > 16
             && t->hitPattern().muonStationsWithValidHits() > 1
             && t->normalizedChi2() < 10;
    return !pass;
  };
  muRefs.erase(remove_if(muRefs.begin(), muRefs.end(), generalSelection), muRefs.end());
  if (int(muRefs.size())<nMuons_) return;

  /* sort mu by pT */
  sort(muRefs.begin(), muRefs.end(),
      [](const auto& lhs, const auto& rhs){ return lhs->pt() > rhs->pt(); });

  pt_  .clear(); pt_  .reserve(muRefs.size());
  eta_ .clear(); eta_ .reserve(muRefs.size());
  phi_ .clear(); phi_ .reserve(muRefs.size());

  for (const auto& recoMu : muRefs) {
    pt_ .push_back(recoMu->pt());
    eta_.push_back(recoMu->eta());
    phi_.push_back(recoMu->phi());
  }

  // trigger firing condition
  const vector<string>& pathNames = hltConfig_.triggerNames();
  const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
  if (matchedPaths.size() == 0) {
    LogError("trigSelfEffiForMuTrack")<<"Could not find matched full trigger path with -> "<<trigPathNoVer_<<endl;
    return;
  }
  trigPath_ = matchedPaths[0];
  if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
    LogError("trigSelfEffiForMuTrack")<<"Cannot find trigger path -> "<<trigPath_<<endl;
    return;
  }
  fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

  muTrackT_->Fill();

  return;
}

void
trigSelfEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigSelfEffiForMuTrack::endJob() {}
