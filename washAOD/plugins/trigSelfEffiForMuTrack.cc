#include "Firefighter/washAOD/interface/trigSelfEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

trigSelfEffiForMuTrack::trigSelfEffiForMuTrack(const edm::ParameterSet& ps) :
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
    nMuons_(ps.getParameter<int>("nMuons")),
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

trigSelfEffiForMuTrack::~trigSelfEffiForMuTrack() = default;

void trigSelfEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
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
    desc.add<std::string>("trigPath", "Defaultshouldntbecalled!");
    desc.add<std::string>("processName", "HLT");
    desc.add<int>("nMuons", 2);
    descriptions.add("trigSelfEffiForMuTrack", desc);
}

void trigSelfEffiForMuTrack::beginJob()
{
    muTrackT_ = fs->make<TTree>("trigSelfEffiForMuTrack", "");

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

void trigSelfEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
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

void trigSelfEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
        LogError("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting recoMet product from Event!" << endl;
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
    //auto generalSelection = [](const auto& t){
    //    bool pass = t->pt() > 5
    //        && abs(t->eta()) < 2
    //        && t->hitPattern().numberOfValidMuonHits() > 16
    //        && t->hitPattern().muonStationsWithValidHits() > 1
    //        && t->normalizedChi2() < 10;
    //    return !pass;
    //};
    //muRefs.erase(remove_if(muRefs.begin(), muRefs.end(), generalSelection), muRefs.end());
    //if (int(muRefs.size())<nMuons_) return;

    /* sort mu by pT */
    sort(muRefs.begin(), muRefs.end(), [](const auto& lhs, const auto& rhs){ return lhs->pt() > rhs->pt(); });

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

void trigSelfEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void trigSelfEffiForMuTrack::endJob() {}
