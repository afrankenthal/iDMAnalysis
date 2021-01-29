/**
 * Trigger efficiencies in terms of events
 * =======================================
 * Require:
 *   - 2 gen muons in |eta|<2.4
 *   - 2 dSA muons matched with gen muons (dR<0.3)
 *   - >=2 dSA muons passing general selections
 *   - >= 30 GeV Jet pt
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

class trigEffiForMuTrack : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit trigEffiForMuTrack(const edm::ParameterSet&);
        ~trigEffiForMuTrack();

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

        int numEventsTotal;

        std::vector<float> pt_;
        std::vector<float> eta_;
        std::vector<float> phi_;

        float genJetPt_;
        float genLeadMetPt_;
        float genSubLeadMetPt_;
        float recoPFMetPt_;
        float recoJetPt_;
        float recoJetEta_;
        float recoJetPhi_;

        TTree *muTrackT_;
        TTree *overallInfoT_;
};

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

void trigEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
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
    desc.add<std::string>("trigPath", "DefaultSHouldntbecalled!");
    //desc.add<std::string>("trigPath", "HLT_PFMET120_PFMHT120_IDTight");
    desc.add<std::string>("processName", "HLT");
    descriptions.add("trigEffiForMuTrack", desc);
}

void trigEffiForMuTrack::beginJob()
{
    overallInfoT_ = fs->make<TTree>("overallInfo", "");
    overallInfoT_->Branch("numEventsTotal", &numEventsTotal, "numEventsTotal/I");

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

void trigEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
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

void trigEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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

    numEventsTotal = 1;
    overallInfoT_->Fill();

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
            && abs(t->eta()) < 2.4
            && t->hitPattern().numberOfValidMuonHits() > 12
            && t->hitPattern().muonStationsWithValidHits() > 1
            && t->normalizedChi2() < 10;
        return pass;
    };

    int tracksPassedGS = count_if(muTrackIdx.begin(), muTrackIdx.end(), generalSelection);
    if (tracksPassedGS<2) return;

    // Additionally, check if event has 1 or more jets with leading pt > 120 GeV
    if (recoJetHandle_->size() < 1) return;
    bool accept = 0;
    for (size_t i(0); i != recoJetHandle_->size(); ++i) {
        reco::PFJetRef jetr(recoJetHandle_, i);
        if (jetr->pt() > 120.0) {
            accept = 1;
            break;
        }
    }
    if (!accept) return;

    pt_  .clear(); pt_  .reserve(2);
    eta_ .clear(); eta_ .reserve(2);
    phi_ .clear(); phi_ .reserve(2);

    for (const int muid : muTrackIdx) {
        if (!generalSelection(muid)) continue;
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

void trigEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void trigEffiForMuTrack::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(trigEffiForMuTrack);