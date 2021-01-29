/**
 * Trigger efficiencies itself
 * =======================================
 * Require:
 *   - >=2 dSA muons passing general selections
 * Check trigger firing condition
**/

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "TTree.h"

class trigSelfEffiForMuTrack : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit trigSelfEffiForMuTrack(const edm::ParameterSet&);
        ~trigSelfEffiForMuTrack();

        static void fillDescriptions(edm::ConfigurationDescriptions&);

    private:
        virtual void beginJob() override;
        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endJob() override;

        const edm::InputTag muTrackTag_;
        const edm::InputTag genParticleTag_;
        const edm::InputTag genJetTag_;
        const edm::InputTag genMetTag_;
        const edm::InputTag recoMetTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
        const std::string trigPathNoVer_;
        const std::string processName_;
        const int nMuons_;
        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken_;
        const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
        const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
        const edm::EDGetTokenT<reco::GenMETCollection> genMetToken_;
        const edm::EDGetTokenT<reco::PFMETCollection> recoMetToken_;
        const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
        const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
        const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;

        edm::Service<TFileService> fs;
        edm::Handle<reco::TrackCollection> muTrackHandle_;
        edm::Handle<reco::GenParticleCollection> genParticleHandle_;
        edm::Handle<reco::GenJetCollection> genJetHandle_;
        edm::Handle<reco::GenMETCollection> genMetHandle_;
        edm::Handle<reco::PFMETCollection> recoMetHandle_;
        edm::Handle<reco::PFJetCollection> recoJetHandle_;
        edm::Handle<edm::TriggerResults> trigResultsHandle_;
        edm::Handle<trigger::TriggerEvent> trigEventHandle_;

        std::string trigPath_;
        HLTConfigProvider hltConfig_;

        bool fired_;

        std::vector<float> pt_;
        std::vector<float> eta_;
        std::vector<float> phi_;

        std::vector<float> genPt_;
        std::vector<float> genEta_;
        std::vector<float> genPhi_;
        std::vector<float> genVxy_;
        std::vector<float> genVz_;

        float genJetPt_;
        float genLeadMetPt_;
        float genSubLeadMetPt_;
        float recoPFMetPt_;
        float recoJetPt_;
        float recoJetEta_;
        float recoJetPhi_;

        TTree *muTrackT_;
};

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

    genPt_.clear(); genPt_.reserve(2);
    genEta_.clear(); genEta_.reserve(2);
    genPhi_.clear(); genPhi_.reserve(2);
    genVxy_.clear(); genVxy_.reserve(2);
    genVz_.clear(); genVz_.reserve(2);

    for (size_t i = 0; i < genParticleHandle_->size(); i++) {
        reco::GenParticleRef genP(genParticleHandle_, i);
        if (std::abs(genP->pdgId()) != 13) continue;
        genPt_.push_back(genP->pt());
        genEta_.push_back(genP->eta());
        genPhi_.push_back(genP->phi());
        genVxy_.push_back(genP->vertex().rho());
        genVz_.push_back(genP->vz());
    }

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

// define this as a plug-in
DEFINE_FWK_MODULE(trigSelfEffiForMuTrack);