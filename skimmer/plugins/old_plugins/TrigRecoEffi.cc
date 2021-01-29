#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
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

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "TTree.h"

class TrigRecoEffi :
    public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit TrigRecoEffi(const edm::ParameterSet&);
        ~TrigRecoEffi();

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

        unsigned int nMatched_;
        bool fired_;

        std::vector<float> genPt_;
        std::vector<float> genEta_;
        std::vector<float> genPhi_;
        std::vector<float> genVxy_;
        std::vector<float> genVz_;
        std::vector<float> genDr_;
        std::vector<float> recoPt_;
        std::vector<float> recoEta_;
        std::vector<float> recoPhi_;
        std::vector<float> recoDxy_;
        std::vector<float> recoDz_;
        std::vector<float> recoVxy_;
        std::vector<float> recoVz_;
        std::vector<float> recoDr_;
        std::vector<float> deltaR_;
        std::vector<float> recoLeadSubleadMuPt_;
        std::vector<float> recoLeadSubleadMuEta_;
        std::vector<float> recoLeadSubleadMuPhi_;
        std::vector<float> recoLeadSubleadMuDxy_;
        std::vector<float> recoLeadSubleadMuDz_;
        float genJetPt_;
        float genJetEta_;
        float genJetPhi_;
        float genLeadMetPt_;
        float genSubLeadMetPt_;
        float recoPFMetPt_;
        float recoJetPt_;
        float recoJetEta_;
        float recoJetPhi_;

        TTree *muTrackT_;
};



TrigRecoEffi::TrigRecoEffi(const edm::ParameterSet& ps) :
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

TrigRecoEffi::~TrigRecoEffi() = default;

void TrigRecoEffi::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
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
    descriptions.add("TrigRecoEffi", desc);
}

void TrigRecoEffi::beginJob()
{
    muTrackT_ = fs->make<TTree>("TrigRecoEffi", "");

    muTrackT_->Branch("nMatched", &nMatched_, "nMatched/i");
    muTrackT_->Branch("fired", &fired_, "fired/O");
    muTrackT_->Branch("genPt",   &genPt_);
    muTrackT_->Branch("genEta",  &genEta_);
    muTrackT_->Branch("genPhi",  &genPhi_);
    muTrackT_->Branch("genVxy",  &genVxy_);
    muTrackT_->Branch("genVz",   &genVz_);
    muTrackT_->Branch("genDr",   &genDr_);
    muTrackT_->Branch("recoLeadSubleadMuPt",  &recoLeadSubleadMuPt_);
    muTrackT_->Branch("recoLeadSubleadMuEta", &recoLeadSubleadMuEta_);
    muTrackT_->Branch("recoLeadSubleadMuPhi", &recoLeadSubleadMuPhi_);
    muTrackT_->Branch("recoLeadSubleadMuDxy", &recoLeadSubleadMuDxy_);
    muTrackT_->Branch("recoLeadSubleadMuDz",  &recoLeadSubleadMuDz_);
    muTrackT_->Branch("recoAllMuPt",  &recoPt_);
    muTrackT_->Branch("recoAllMuEta", &recoEta_);
    muTrackT_->Branch("recoAllMuPhi", &recoPhi_);
    muTrackT_->Branch("recoAllMuDxy", &recoDxy_);
    muTrackT_->Branch("recoAllMuDz",  &recoDz_);
    muTrackT_->Branch("recoVxy", &recoVxy_);
    muTrackT_->Branch("recoVz",  &recoVz_);
    muTrackT_->Branch("recoDr",  &recoDr_);
    muTrackT_->Branch("deltaR",  &deltaR_);
    muTrackT_->Branch("genJetPt", &genJetPt_, "genJetPt/F");
    muTrackT_->Branch("genJetEta", &genJetEta_, "genJetEta/F");
    muTrackT_->Branch("genJetPhi", &genJetPhi_, "genJetPhi/F");
    muTrackT_->Branch("recoJetPt", &recoJetPt_, "recoJetPt/F");
    muTrackT_->Branch("recoJetEta", &recoJetEta_, "recoJetEta/F");
    muTrackT_->Branch("recoJetPhi", &recoJetPhi_, "recoJetPhi/F");
    muTrackT_->Branch("genLeadMetPt", &genLeadMetPt_, "genLeadMetPt/F");
    muTrackT_->Branch("genSubLeadMetPt", &genSubLeadMetPt_, "genSubLeadMetPt/F");
    muTrackT_->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
}

void TrigRecoEffi::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    using namespace std;
    using namespace edm;

    bool changed(true);
    if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
        if (changed) {
            LogInfo("TrigRecoEffi")<<"TrigRecoEffi::beginRun: "<<"hltConfig init for Run"<<iRun.run();
            hltConfig_.dump("ProcessName");
            hltConfig_.dump("GlobalTag");
            hltConfig_.dump("TableName");
        }
    } else {
        LogError("TrigRecoEffi")<<"TrigRecoEffi::beginRun: config extraction failure with processName -> "
            <<processName_;
    }
}

void TrigRecoEffi::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogVerbatim("TrigRecoEffi") << "TrigRecoEffi::analyze: Error in getting muTrack product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(genParticleToken_, genParticleHandle_);
    if (!genParticleHandle_.isValid()) {
        LogVerbatim("TrigRecoEffi") << "TrigRecoEffi::analyze: Error in getting genParticle product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(genJetToken_, genJetHandle_);
    if (!genJetHandle_.isValid()) {
        LogVerbatim("TrigRecoEffi") << "TrigRecoEffi::analyze: Error in getting genJet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(genMetToken_, genMetHandle_);
    if (!genMetHandle_.isValid()) {
        LogVerbatim("TrigRecoEffi") << "TrigRecoEffi::analyze: Error in getting genMet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoMetToken_, recoMetHandle_);
    if (!recoMetHandle_.isValid()) {
        LogVerbatim("TrigRecoEffi") << "TrigRecoEffi::analyze: Error in getting recoMet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoJetToken_, recoJetHandle_);
    if (!recoJetHandle_.isValid()) {
        LogError("TrigRecoEffi")
            << "TrigRecoEffi::analyze: Error in getting recoJet product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
    if (!trigResultsHandle_.isValid()) {
        LogError("TrigRecoEffi")
            << "TrigRecoEffi::analyze: Error in getting triggerResults product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(trigEventToken_, trigEventHandle_);
    if (!trigEventHandle_.isValid()) {
        LogError("TrigRecoEffi")
            << "TrigRecoEffi::analyze: Error in getting triggerEvent product from Event!"
            << endl;
        return;
    }

    // Gen-level mu selection 
    int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
            [](const reco::GenParticle& g){
            return abs(g.pdgId())==13
            and g.isHardProcess()
            and abs(g.eta())<2.4
            and abs(g.vertex().rho())<740  // decay inside CMS
            and abs(g.vz())<960;
            });
    if (nAccpted<2) return;

    // Additionally, check if event has 1 or more gen jets with leading pt > 30 GeV 
    if (genJetHandle_->size() < 1) return;
    bool accept = 0;
    for (size_t i(0); i != genJetHandle_->size(); ++i) {
        reco::GenJetRef jetr(genJetHandle_, i);
        if (jetr->pt() > 30.0) {
            accept = 1;
            break;
        }
    }
    if (!accept) return;

    // No reco general selections -- Only gen-level selections

    genPt_  .clear(); genPt_  .reserve(2);
    genEta_ .clear(); genEta_ .reserve(2);
    genPhi_ .clear(); genPhi_ .reserve(2);
    genVxy_ .clear(); genVxy_ .reserve(2);
    genVz_  .clear(); genVz_  .reserve(2);
    genDr_  .clear(); genDr_  .reserve(2);
    recoPt_ .clear(); recoPt_ .reserve(2);
    recoEta_.clear(); recoEta_.reserve(2);
    recoPhi_.clear(); recoPhi_.reserve(2);
    recoDxy_.clear(); recoDxy_.reserve(2);
    recoDz_ .clear(); recoDz_ .reserve(2);
    recoVxy_.clear(); recoVxy_.reserve(1);
    recoVz_ .clear(); recoVz_ .reserve(1);
    recoDr_ .clear(); recoDr_ .reserve(1);
    deltaR_ .clear(); deltaR_ .reserve(2);
    recoLeadSubleadMuPt_.clear(); recoLeadSubleadMuPt_.reserve(2);
    recoLeadSubleadMuEta_.clear(); recoLeadSubleadMuEta_.reserve(2);
    recoLeadSubleadMuPhi_.clear(); recoLeadSubleadMuPhi_.reserve(2);
    recoLeadSubleadMuDxy_.clear(); recoLeadSubleadMuDxy_.reserve(2);
    recoLeadSubleadMuDz_.clear(); recoLeadSubleadMuDz_.reserve(2);

    // MC match
    vector<int> genMuIdx{};
    for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
        reco::GenParticleRef gp(genParticleHandle_, ig);
        if (abs(gp->pdgId())==13 and gp->isHardProcess() and abs(gp->eta())<2.4
                and abs(gp->vertex().rho())<740 and abs(gp->vz())<960) {
            genMuIdx.push_back(ig);
        }
    }

    // deltaR between mu pairs
    // Should ONLY have 2 gen muons for iDM
    map<int, float> genMuIdDr{};
    for (size_t mu_i(0); mu_i!=genMuIdx.size(); ++mu_i) {
        reco::GenParticleRef ref_i(genParticleHandle_, genMuIdx[mu_i]);
        for (size_t mu_j(mu_i+1); mu_j!=genMuIdx.size(); ++mu_j) {
            reco::GenParticleRef ref_j(genParticleHandle_, genMuIdx[mu_j]);
            if (ref_i->vertex() != ref_j->vertex()) continue;
            float dR = deltaR(*(ref_i.get()), *(ref_j.get()));
            genMuIdDr.emplace(genMuIdx[mu_i], dR);
            genMuIdDr.emplace(genMuIdx[mu_j], dR);
        }
    }

    // vtxing between reco mu pairs
    //ESHandle<TransientTrackBuilder> theB;
    //iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

    //map<int, pair<float, float>> recoMuIdVtx{};
    //for (size_t mu_i(0); mu_i!=muTrackHandle_->size(); ++mu_i) {
        //reco::TrackRef ref_i(muTrackHandle_, mu_i);
        //for (size_t mu_j(mu_i+1); mu_j!=muTrackHandle_->size(); ++mu_j) {
            //reco::TrackRef ref_j(muTrackHandle_, mu_j);
            //if (deltaR(*(ref_i.get()), *(ref_j.get())) > 0.4) continue;
            //if (ref_i->charge() == ref_j->charge()) continue;

            //vector<reco::TransientTrack> t_tks{};
            //t_tks.push_back(theB->build(&ref_i));
            //t_tks.push_back(theB->build(&ref_j));
            //KalmanVertexFitter kvf(true);
            //TransientVertex tv = kvf.vertex(t_tks);
            //if (!tv.isValid()) continue;

            //reco::Vertex _vtx = reco::Vertex(tv);
            //float _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
            //float _vz  = _vtx.z();
            //recoMuIdVtx.emplace(int(mu_i), make_pair(_vxy, _vz));
            //recoMuIdVtx.emplace(int(mu_j), make_pair(_vxy, _vz));
            //break;

        //}
    //}
    //for (size_t i(0); i!=muTrackHandle_->size(); ++i) {
        //if (recoMuIdVtx.find(int(i)) != recoMuIdVtx.end()) continue;
        //recoMuIdVtx.emplace(int(i), make_pair(0., 0.));
    //}

    //cout<<endl;
    //for (size_t i(0); i!=muTrackHandle_->size(); ++i) {
    //  cout<<i<<"\t"<<recoMuIdVtx[i].first<<" "<<recoMuIdVtx[i].second<<endl;
    //}
    
    // Sort mu by pT
    vector<reco::TrackRef> muRefs{};
    for (size_t i = 0; i < muTrackHandle_->size(); i++) {
        muRefs.emplace_back(muTrackHandle_, i);
    }

    sort(muRefs.begin(), muRefs.end(), [](const auto & lhs, const auto & rhs) { return lhs->pt() > rhs->pt(); } );

    vector<int> matchedGenMuIdx{};
    vector<int> matchedRecoMuIdx{};
    //int iTk = 0;
    for (size_t iTk = 0; iTk < muRefs.size(); iTk++) {
        const reco::Track & muTrack = *muRefs[iTk];
        // quality cut
        if (muTrack.hitPattern().numberOfValidMuonHits() < 12 ||
                muTrack.hitPattern().muonStationsWithValidHits() < 2 ||
                muTrack.normalizedChi2() > 10)
            continue;

        // add to all mu collection
        recoPt_ .push_back(muTrack.pt());
        recoEta_.push_back(muTrack.eta());
        recoPhi_.push_back(muTrack.phi());
        recoDxy_.push_back(muTrack.dxy());
        recoDz_ .push_back(muTrack.dz());

    //for (const reco::Track& muTrack : *muTrackHandle_) {
        //iTk = &muTrack - &(muTrackHandle_->at(0));
        for (const int genMuid : genMuIdx) {

            if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), genMuid) != matchedGenMuIdx.end()) // gen mu already matched
                continue;
            reco::GenParticleRef genMu(genParticleHandle_, genMuid);

            if (deltaR(muTrack, *(genMu.get())) > 0.3 || 
                    muTrack.charge() != genMu->charge()) { // doesn't match
                continue;
            }

            // Matched gen with reco muon

            deltaR_ .push_back(deltaR(muTrack, *(genMu.get())));

            genPt_  .push_back(genMu->pt());
            genEta_ .push_back(genMu->eta());
            genPhi_ .push_back(genMu->phi());
            genVxy_ .push_back(genMu->vertex().rho());
            genVz_  .push_back(genMu->vz());
            genDr_  .push_back(genMuIdDr[genMuid]);

            recoLeadSubleadMuPt_.push_back(muTrack.pt());
            recoLeadSubleadMuEta_.push_back(muTrack.eta());
            recoLeadSubleadMuPhi_.push_back(muTrack.phi());
            recoLeadSubleadMuDxy_.push_back(muTrack.dxy());
            recoLeadSubleadMuDz_.push_back(muTrack.dz());

            matchedGenMuIdx.push_back(genMuid);
            matchedRecoMuIdx.push_back(iTk);

            break;
        }
    }
    nMatched_ = matchedGenMuIdx.size();
    
    // Only add vtx info to event if 2 muons matched
    if (nMatched_ == 2) {
        // vtxing between reco mu pairs
        ESHandle<TransientTrackBuilder> theB;
        iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

        // there should ONLY be 2 reco muons matched
        reco::TrackRef ref_i = muRefs[matchedRecoMuIdx[0]];
        reco::TrackRef ref_j = muRefs[matchedRecoMuIdx[1]];
        vector<reco::TransientTrack> t_tks{};
        t_tks.push_back(theB->build(&ref_i));
        t_tks.push_back(theB->build(&ref_j));
        KalmanVertexFitter kvf(true);
        TransientVertex tv = kvf.vertex(t_tks);
        if (tv.isValid()) {

            reco::Vertex _vtx = reco::Vertex(tv);
            float _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
            float _vz  = _vtx.z();

            recoVxy_.push_back(_vxy);
            recoVz_.push_back(_vz);
            recoDr_.push_back(deltaR(*ref_i, *ref_j));
        }
    }
    
    // trigger firing condition
    const vector<string>& pathNames = hltConfig_.triggerNames();
    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
    if (matchedPaths.size() == 0) {
        LogError("TrigRecoEffi")<<"Could not find matched full trigger path with -> "<<trigPathNoVer_<<endl;
        return;
    }
    trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
        LogError("TrigRecoEffi")<<"Cannot find trigger path -> "<<trigPath_<<endl;
        return;
    }
    fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

    // Fill the rest GEN info
    for (const int muId : genMuIdx) {
        if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), muId) != matchedGenMuIdx.end()) continue;
        reco::GenParticleRef genMu(genParticleHandle_, muId);

        genPt_  .push_back(genMu->pt());
        genEta_ .push_back(genMu->eta());
        genPhi_ .push_back(genMu->phi());
        genVxy_ .push_back(genMu->vertex().rho());
        genVz_  .push_back(genMu->vz());
        genDr_  .push_back(genMuIdDr[muId]);
    }

    // Fill j1pT gen branch

    double largestPt = 0.0, largestEta = 0.0, largestPhi = 0.0;
    for (size_t i(0); i != genJetHandle_->size(); ++i) {
        reco::GenJetRef jetr(genJetHandle_, i);
        if (jetr->pt() > largestPt) {
            largestPt = jetr->pt();
            largestEta = jetr->eta();
            largestPhi = jetr->phi();
        }
    }
    genJetPt_ = largestPt;
    genJetEta_ = largestEta;
    genJetPhi_ = largestPhi;

    // fill reco jet w/ largest pt
    largestPt = 0.0; largestEta = 0.0; largestPhi = 0.0;
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

    // Fill MET gen branch

    reco::GenMETRef metr(genMetHandle_, 0);
    genLeadMetPt_ = metr->pt();
    reco::GenMETRef metr2(genMetHandle_, 1);
    genSubLeadMetPt_ = metr2->pt();
    reco::PFMETRef metr3(recoMetHandle_, 0);
    recoPFMetPt_ = metr3->pt();

    muTrackT_->Fill();

    return;

}

void TrigRecoEffi::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void TrigRecoEffi::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(TrigRecoEffi);