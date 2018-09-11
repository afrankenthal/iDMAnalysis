#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

trigEffiForMuTrack::trigEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
	genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
	genMetTag_(ps.getParameter<edm::InputTag>("genMet")),
	recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
	recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
  trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
  trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
  trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
  processName_(ps.getParameter<std::string>("processName")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
	genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
	genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
	recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
	recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
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
	desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
	desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
	desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));
	desc.add<edm::InputTag>("recoMet", edm::InputTag("pfMet"));
  desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
  desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
  desc.add<std::string>("trigPath", "HLT_DoubleMu3_DCA_PFMET50_PFMHT60");
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
	muTrackT_->Branch("genJetPt", &genJetPt_, "genJetPt/F");
	muTrackT_->Branch("recoJetPt", &recoJetPt_, "recoJetPt/F");
	muTrackT_->Branch("recoJetEta", &recoJetEta_, "recoJetEta/F");
	muTrackT_->Branch("recoJetPhi", &recoJetPhi_, "recoJetPhi/F");
	muTrackT_->Branch("genLeadMetPt", &genLeadMetPt_, "genLeadMetPt/F");
	muTrackT_->Branch("genSubLeadMetPt", &genSubLeadMetPt_, "genSubLeadMetPt/F");
	muTrackT_->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
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
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }
	iEvent.getByToken(genJetToken_, genJetHandle_);
	if (!genJetHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting genJet product from Event!"
      << endl;
    return;
	}
	iEvent.getByToken(genMetToken_, genMetHandle_);
	if (!genMetHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting genMet product from Event!"
      << endl;
    return;
	}
	iEvent.getByToken(recoMetToken_, recoMetHandle_);
	if (!recoMetHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting recoMet product from Event!"
      << endl;
    return;
	}
	iEvent.getByToken(recoJetToken_, recoJetHandle_);
	if (!recoJetHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting recoJet product from Event!"
      << endl;
    return;
	}
  iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
  if (!trigResultsHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting triggerResults product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(trigEventToken_, trigEventHandle_);
  if (!trigEventHandle_.isValid()) {
    LogError("trigEffiForMuTrack")
      << "trigEffiForMuTrack::analyze: Error in getting triggerEvent product from Event!"
      << endl;
    return;
  }

  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){return abs(g.pdgId())==13 and g.isHardProcess() and abs(g.eta())<2.4;});
  if (nAccpted<2) return;
  if (muTrackHandle_->size()<2) return;

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
  if (matchedGenMuIdx.size()<2) return;

  /* general selection */
  auto generalSelection = [&](const auto tid){
    reco::TrackRef t(muTrackHandle_, tid);
    bool pass = t->pt() > 5
             && abs(t->eta()) < 2
             && t->hitPattern().numberOfValidMuonHits() > 16
             && t->hitPattern().muonStationsWithValidHits() > 1
             && t->normalizedChi2() < 10;
    return pass;
  };

  int tracksPassedGS = count_if(muTrackIdx.begin(), muTrackIdx.end(), generalSelection);
  if (tracksPassedGS<2) return;

	// Additionally, check if event has 1 or more jets with leading pt >
	// 30 GeV
	if (recoJetHandle_->size() < 1) return;
	bool accept = 0;
	for (size_t i(0); i != recoJetHandle_->size(); ++i) {
					reco::PFJetRef jetr(recoJetHandle_, i);
					if (jetr->pt() > 30.0) {
									accept = 1;
									break;
					}
	}
	if (!accept) return;

  pt_  .clear(); pt_  .reserve(2);
  eta_ .clear(); eta_ .reserve(2);
  phi_ .clear(); phi_ .reserve(2);

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

	// Fill j1pT branches TODO: does this need to be std::abs?
	double largestPt = 0.0;
	for (size_t i(0); i != genJetHandle_->size(); ++i) {
					reco::GenJetRef jetr(genJetHandle_, i);
					if (jetr->pt() > largestPt) {
									largestPt = jetr->pt();
					}
	}
	genJetPt_ = largestPt;

	largestPt = 0.0;
	double largestEta = 0.0, largestPhi = 0.0;
	for (size_t i(0); i != recoJetHandle_->size(); ++i) {
					reco::PFJetRef jetr(recoJetHandle_, i);
					if (jetr->pt() > largestPt) {
									largestPt = jetr->pt();
									largestEta = jetr->eta();
									largestPhi = jetr->phi();
					}
	}
	recoJetPt_ = largestPt;
	recoJetEta_ = largestEta;
	recoJetPhi_ = largestPhi;

	// Fill MET branches
	reco::GenMETRef metr(genMetHandle_, 0);
	genLeadMetPt_ = metr->pt();
	reco::GenMETRef metr2(genMetHandle_, 1);
	genSubLeadMetPt_ = metr2->pt();
	reco::PFMETRef metr3(recoMetHandle_, 0);
	recoPFMetPt_ = metr3->pt();

  muTrackT_->Fill();

  return;
}

void
trigEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void
trigEffiForMuTrack::endJob() {}
