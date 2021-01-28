#include <cmath>

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/METReco/interface/BeamHaloSummary.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/LorentzVector.h"

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

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "TTree.h"

class TrackQuality :
    public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit TrackQuality(const edm::ParameterSet&);
        ~TrackQuality();

        static void fillDescriptions(edm::ConfigurationDescriptions&);

    private:
        virtual void beginJob() override;
        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endJob() override;

        const edm::InputTag muTrackTag_;
//        const edm::InputTag genParticleTag_;
//        const edm::InputTag genJetTag_;
//        const edm::InputTag genMetTag_;
        const edm::InputTag recoMetTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
        const std::string trigPathNoVer_;
//        const edm::InputTag pileupInfosTag_;
//        const edm::InputTag genEvtInfoTag_;
        const std::string processName_;
	const edm::InputTag beamHaloSummaryTag_;

        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken_;
//        const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
//        const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
//        const edm::EDGetTokenT<reco::GenMETCollection> genMetToken_;
        const edm::EDGetTokenT<reco::PFMETCollection> recoMetToken_;
        const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
        const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
        const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
//        const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupInfosToken_;
//        const edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_;
	const edm::EDGetTokenT<reco::BeamHaloSummary> beamHaloSummaryToken_;

        edm::Service<TFileService> fs;
        edm::Handle<reco::TrackCollection> muTrackHandle_;
//        edm::Handle<reco::GenParticleCollection> genParticleHandle_;
//        edm::Handle<reco::GenJetCollection> genJetHandle_;
//        edm::Handle<reco::GenMETCollection> genMetHandle_;
        edm::Handle<reco::PFMETCollection> recoMetHandle_;
        edm::Handle<reco::PFJetCollection> recoJetHandle_;
        edm::Handle<edm::TriggerResults> trigResultsHandle_;
        edm::Handle<trigger::TriggerEvent> trigEventHandle_;
//        edm::Handle<std::vector<PileupSummaryInfo> > pileupInfosHandle_;
//        edm::Handle<GenEventInfoProduct> genEvtInfoHandle_;
        edm::Handle<reco::BeamHaloSummary> beamHaloSummaryHandle_;
        
        std::string trigPath_;
        HLTConfigProvider hltConfig_;

        unsigned int fired_;
        //unsigned int nMatched_;

//        std::vector<float> genPt_;
//        std::vector<float> genEta_;
//        std::vector<float> genPhi_;
//        std::vector<float> genVxy_;
//        std::vector<float> genVz_;
        //std::vector<float> genDr_;
        std::vector<float> recoPt_;
        std::vector<float> recoEta_;
        std::vector<float> recoPhi_;
        std::vector<float> recoDxy_;
        std::vector<float> recoDz_;
        std::vector<float> recoVxy_;
        std::vector<float> recoVz_;
        std::vector<float> recoDr_;
        //std::vector<float> deltaR_;
        //float genJetPt_;
        //float genLeadMetPt_;
        //float genSubLeadMetPt_;
        float recoPFMetPt_;
        float recoPFMetPhi_;
        std::vector<float> recoPFJetPt_;
        std::vector<float> recoPFJetEta_;
        std::vector<float> recoPFJetPhi_;
        float MHTPt_;
//        int genpuobs;
//        int genputrue;
//        float genwgt;
	float diMuonPhi;


	float delPhiMetMu;// delta phi between met and dimuon phi
        std::vector<float> trackPlanes; // track quality info planes =2
        std::vector<float> trackHits; // track quality info hits =12
        std::vector<float> trackChi2; // track quality info chi2 =10
        std::vector<bool> qualityTrack;// is the track good quality (above criteria)
//        std::vector<bool> isGenMatched; // if the muon is gen matched dR<0.3
//        std::vector<float> GenDR; // if the muon is gen matched dR<0.3
        int event;

	bool beamHaloHcal;
	bool beamHaloEcal;
	bool beamHaloCSC;
	std::vector<char> beamHaloHcalReport;
	std::vector<char> beamHaloEcalReport;
	std::vector<char> beamHaloCSCReport;
	bool beamHaloGlobaltight2016;
	bool beamHaloGlobalsupertight2016;
	bool beamHaloCSCtight2015;

        TTree* cutsTree;
        int cutsVec[6];
};

TrackQuality::TrackQuality(const edm::ParameterSet& ps) :
    muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
//    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
//    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
//    genMetTag_(ps.getParameter<edm::InputTag>("genMet")),
    recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
    trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
    trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
//    pileupInfosTag_(ps.getParameter<edm::InputTag>("pileups")),
//    genEvtInfoTag_(ps.getParameter<edm::InputTag>("genEvt")),
    processName_(ps.getParameter<std::string>("processName")),
    beamHaloSummaryTag_(ps.getParameter<edm::InputTag>("beamHaloSummary")),
    
    muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
//    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
//    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
//    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
//    pileupInfosToken_(consumes<std::vector<PileupSummaryInfo> >(pileupInfosTag_)),
//    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_)),
    beamHaloSummaryToken_(consumes<reco::BeamHaloSummary>(beamHaloSummaryTag_))
{
    usesResource("TFileService");
}

TrackQuality::~TrackQuality() = default;

void TrackQuality::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
//    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
//    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
//    desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("recoMet", edm::InputTag("pfMet"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
    desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults", "", "HLT"));
    desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD", "", "HLT"));
    desc.add<std::string>("trigPath", "Defaultshouldntbecalled");
//    desc.add<edm::InputTag>("pileups", edm::InputTag("addPileupInfo"));
//    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));
    desc.add<std::string>("processName", "HLT");
    desc.add<edm::InputTag>("beamHaloSummary", edm::InputTag("BeamHaloSummary"));
    descriptions.add("TrackQuality", desc);
}

void TrackQuality::beginJob()
{
    for (int i = 0; i < 6; i++)
        cutsVec[i] = 0;

    //for (int i = 0; i < 1; i++) {
        //std::stringstream cutlabel; cutlabel << "cut" << i;
        //auto temp = cutlabel.str();
    cutsTree = fs->make<TTree>("cutsTree", "");
    //}

    for (int i = 0; i < 1; i++) {
        cutsTree->Branch("fired", &fired_, "fired/i");
//        cutsTree->Branch("genPt",  &genPt_);
//        cutsTree->Branch("genEta", &genEta_);
//        cutsTree->Branch("genPhi", &genPhi_);
//        cutsTree->Branch("genVxy", &genVxy_);
//        cutsTree->Branch("genVz",  &genVz_);
        cutsTree->Branch("recoPt",  &recoPt_);
        cutsTree->Branch("recoEta", &recoEta_);
        cutsTree->Branch("recoPhi", &recoPhi_);
        cutsTree->Branch("recoDxy", &recoDxy_);
        cutsTree->Branch("recoDz",  &recoDz_);
        cutsTree->Branch("recoVxy", &recoVxy_);
        cutsTree->Branch("recoVz",  &recoVz_);
        cutsTree->Branch("recoDr",  &recoDr_);
        //cutsTree[i]->Branch("deltaR",  &deltaR_);
        cutsTree->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
        cutsTree->Branch("recoPFMetPhi", &recoPFMetPhi_, "recoPFMetPhi/F");
        cutsTree->Branch("recoPFJetPt", &recoPFJetPt_);//, "recoPFJetPt/F");
        cutsTree->Branch("recoPFJetEta", &recoPFJetEta_);//, "recoPFJetEta/F");
        cutsTree->Branch("recoPFJetPhi", &recoPFJetPhi_);//, "recoPFJetPhi/F");
        cutsTree->Branch("MHTPt", &MHTPt_, "MHTPt/F");
        cutsTree->Branch("cutsVec", cutsVec, "cutsVec[6]/i");
//        cutsTree->Branch("genpuobs", &genpuobs);
//        cutsTree->Branch("genputrue", &genputrue);
//        cutsTree->Branch("genwgt", &genwgt);
	// extra produced by Tres
        cutsTree->Branch("beamHaloHcal", &beamHaloHcal);
        cutsTree->Branch("beamHaloEcal", &beamHaloEcal);
        cutsTree->Branch("beamHaloCSC", &beamHaloCSC);
        cutsTree->Branch("beamHaloGlobal", &beamHaloGlobaltight2016);
        cutsTree->Branch("beamHaloGlobalsuper", &beamHaloGlobalsupertight2016);
        cutsTree->Branch("beamHaloCSCtight", &beamHaloCSCtight2015);
        cutsTree->Branch("beamHaloHcalReport", &beamHaloHcalReport);
        cutsTree->Branch("beamHaloEcalReport", &beamHaloEcalReport);
        cutsTree->Branch("beamHaloCSCReport", &beamHaloCSCReport);
	cutsTree->Branch("diMuonPhi",&diMuonPhi); //average phi between the two muons
	cutsTree->Branch("delPhiMetMu",&delPhiMetMu);// delta phi between met and dimuon phi
	cutsTree->Branch("trackPlanes",&trackPlanes); // track quality info planes =2
	cutsTree->Branch("trackHits",&trackHits); // track quality info hits =12
	cutsTree->Branch("trackChi2",&trackChi2); // track quality info chi2 =10
	cutsTree->Branch("qualityTrack",&qualityTrack);// is the track good quality (above criteria)
//	cutsTree->Branch("isGenMatched",&isGenMatched); // if the muon is gen matched dR<0.3
//	cutsTree->Branch("GenDR",&GenDR); // if the muon is gen matched dR<0.3
	cutsTree->Branch("event",&event); //event number

        //for (int j = 0; j < 6; j++) {
            //std::stringstream cutlabel; cutlabel << "cut" << j;
            //auto temp = cutlabel.str();
            //cutlabel << "/i";
            //auto temp2 = cutlabel.str();
            //cutsTree[i]->Branch(temp.c_str(), cutsVec[j], temp2.c_str());
        //}
    }
}

void TrackQuality::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    using namespace std;
    using namespace edm;

    bool changed(true);
    if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
        if (changed) {
            LogInfo("TrackQuality") << "TrackQuality::beginRun: " << "hltConfig init for Run" << iRun.run();
            hltConfig_.dump("ProcessName");
            hltConfig_.dump("GlobalTag");
            hltConfig_.dump("TableName");
        }
    } else {
        LogError("TrackQuality") << "TrackQuality::beginRun: config extraction failure with processName -> " << processName_;
    }
}

void TrackQuality::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting muTrack product from Event!"
            << endl;
        return;
    }
//    iEvent.getByToken(genParticleToken_, genParticleHandle_);
//    if (!genParticleHandle_.isValid()) {
//        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting genParticle product from Event!"
//            << endl;
//        return;
//    }
//    iEvent.getByToken(genJetToken_, genJetHandle_);
//    if (!genJetHandle_.isValid()) {
//        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting genJet product from Event!" << endl;
//        return;
//    }
//    iEvent.getByToken(genMetToken_, genMetHandle_);
//    if (!genMetHandle_.isValid()) {
//        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting genMet product from Event!" << endl;
//        return;
//    }
    iEvent.getByToken(beamHaloSummaryToken_, beamHaloSummaryHandle_);
    if (!beamHaloSummaryHandle_.isValid()) {
        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting beamHalo product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoMetToken_, recoMetHandle_);
    if (!recoMetHandle_.isValid()) {
        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting recoMet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoJetToken_, recoJetHandle_);
    if (!recoJetHandle_.isValid()) {
        LogVerbatim("TrackQuality") << "TrackQuality::analyze: Error in getting recoJet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
    if (!trigResultsHandle_.isValid()) {
        LogError("TrackQuality")
            << "TrackQuality::analyze: Error in getting triggerResults product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(trigEventToken_, trigEventHandle_);
    if (!trigEventHandle_.isValid()) {
        LogError("TrackQuality")
            << "TrackQuality::analyze: Error in getting triggerEvent product from Event!"
            << endl;
        return;
    }
//    iEvent.getByToken(pileupInfosToken_, pileupInfosHandle_);
//    if (!pileupInfosHandle_.isValid()) {
//        LogError("TrackQuality")
//            << "TrackQuality::analyze: Error in getting pileupInfos product from Event!"
//            << endl;
//        return;
//    }
//    iEvent.getByToken(genEvtInfoToken_, genEvtInfoHandle_);
//    if (!genEvtInfoHandle_.isValid()) {
//        LogError("TrackQuality")
//            << "TrackQuality::analyze: Error in getting genEvtInfo product from Event!"
//            << endl;
//        return;
//    }


//    genPt_ .clear();
//    genEta_.clear();
//    genPhi_.clear();
//    genVxy_.clear();
//    genVz_ .clear();
    recoPt_ .clear();
    recoEta_.clear();
    recoPhi_.clear();
    recoDxy_.clear();
    recoDz_ .clear();
    recoVxy_.clear();
    recoVz_ .clear();
    recoDr_ .clear();
    //deltaR_ .clear();
    recoPFJetPt_.clear();
    recoPFJetEta_.clear();
    recoPFJetPhi_.clear();
    trackPlanes.clear();
    trackHits.clear();
    trackChi2.clear();
    qualityTrack.clear();
//    isGenMatched.clear();
//    GenDR.clear();

    for (int i = 0; i < 6; i++)
        cutsVec[i] = 0;

//    genwgt = genEvtInfoHandle_->weight();
//
//    genpuobs = -999;
//    genputrue = -999;

//    for (const auto & pileupInfo : *pileupInfosHandle_) {
//        if (pileupInfo.getBunchCrossing() == 0) {
//            genpuobs = pileupInfo.getPU_NumInteractions();
//            genputrue  = pileupInfo.getTrueNumInteractions();
//            break;
//        }
//    }

    // get MET
    // assumes 0-th element of recoMet collection is largest pt (and only?) element

    reco::PFMETRef recoMetr(recoMetHandle_, 0);
    recoPFMetPt_ = recoMetr->pt();
    recoPFMetPhi_ = recoMetr->phi();
    
    const reco::BeamHaloSummary& beamHalo = *beamHaloSummaryHandle_;
    //reco::BeamHaloSummary beamHalo;//(beamHaloSummaryHandle_, 0);
    beamHaloHcal = beamHalo.HcalLooseHaloId();
    beamHaloEcal = beamHalo.EcalLooseHaloId();
    beamHaloCSC = beamHalo.CSCLooseHaloId();
    beamHaloGlobalsupertight2016 = beamHalo.GlobalSuperTightHaloId2016();
    beamHaloGlobaltight2016 = beamHalo.GlobalTightHaloId2016();
    beamHaloCSCtight2015 = beamHalo.CSCTightHaloId2015();
    beamHaloHcalReport = beamHalo.GetHcalHaloReport();
    beamHaloEcalReport = beamHalo.GetEcalHaloReport();
    beamHaloCSCReport = beamHalo.GetCSCHaloReport();
    
    // calculate MHT
    math::XYZTLorentzVector MHT;
    for (auto & jet : *recoJetHandle_) {
        if (jet.pt() < 30) continue;
        MHT += jet.p4();
    }
    MHTPt_ = MHT.pt();

    // get largest pt jet (and its pt) and second largest jet pt
    bool flagHighPtJets = false;
    double maxJetPt = 0, secondMaxJetPt = 0;
    //int jetMaxPtIndex = 0;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        reco::PFJetRef jetTempRef(recoJetHandle_, i);
        if (jetTempRef->pt() > maxJetPt) {
            secondMaxJetPt = maxJetPt;
            maxJetPt = jetTempRef->pt();
            //jetMaxPtIndex = i;
        }
        else if (jetTempRef->pt() > secondMaxJetPt) {
            secondMaxJetPt = jetTempRef->pt();
        }
        else if (jetTempRef->pt() > 30) {
            flagHighPtJets = true;
        }
    }
    /*if (recoJetHandle_->size() > 0) {
        reco::PFJetRef jetMaxPtRef(recoJetHandle_, jetMaxPtIndex);
        recoPFJetPt_ = jetMaxPtRef->pt();
        recoPFJetEta_ = jetMaxPtRef->eta();
        recoPFJetPhi_ = jetMaxPtRef->phi();
    }*/
    
    // Add leading 5 jets to event
    // Note that jet collection is already sorted by pT
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        if (i > 4) break;
        reco::PFJetRef jet_i(recoJetHandle_, i);
        recoPFJetPt_.push_back(jet_i->pt());
        recoPFJetEta_.push_back(jet_i->eta());
        recoPFJetPhi_.push_back(jet_i->phi());
    }

    // get 2 largest pt reco (dSA) muons
    // add top 2 leading pt reco muons to vector, after minimum quality cut
    // Edit: reco muons that pass minimum quality cut
    // (to cut on weird 11 GeV peak muons due to cosmic seed in dSA algo)

    double maxMuPt = 0, secondMaxMuPt = 0;
    double thirdMaxMuPt = 0, fourthMaxMuPt = 0;
    int leadingMuIndex = 0, subleadingMuIndex = 0;
    int subleadingMuIndex2 = 0, subleadingMuIndex3 = 0;
    int goodQualityTracks = 0;
    for (size_t i = 0; i < muTrackHandle_->size(); ++i) {
        reco::TrackRef muTempRef(muTrackHandle_, i);
      //  if (muTempRef->hitPattern().muonStationsWithValidHits() >= 2 ||
       //         muTempRef->hitPattern().numberOfValidMuonHits() >= 12 ||
       //         muTempRef->normalizedChi2() <= 10){
       //     continue;
       // goodQualityTracks++;}
        if (muTempRef->pt() > maxMuPt) {
            fourthMaxMuPt = thirdMaxMuPt;
            thirdMaxMuPt = secondMaxMuPt;
            secondMaxMuPt = maxMuPt;
            maxMuPt = muTempRef->pt();
            subleadingMuIndex3 = subleadingMuIndex2;
            subleadingMuIndex2 = subleadingMuIndex;
            subleadingMuIndex = leadingMuIndex;
            leadingMuIndex = i;
        }
        else if (muTempRef->pt() > secondMaxMuPt) {
            fourthMaxMuPt = thirdMaxMuPt;
            thirdMaxMuPt = secondMaxMuPt;
            secondMaxMuPt = muTempRef->pt();
            subleadingMuIndex3 = subleadingMuIndex2;
            subleadingMuIndex2 = subleadingMuIndex;
            subleadingMuIndex = i;
        }
        else if (muTempRef->pt() > thirdMaxMuPt) {
            fourthMaxMuPt = thirdMaxMuPt;
            thirdMaxMuPt = muTempRef->pt();
            subleadingMuIndex3 = subleadingMuIndex2;
            subleadingMuIndex2 = i;
        }
        else if (muTempRef->pt() > fourthMaxMuPt) {
            fourthMaxMuPt = muTempRef->pt();
            subleadingMuIndex3 = i;
        }
    }
    bool push_0 = (muTrackHandle_->size() > 0);
    bool push_1 = (muTrackHandle_->size() > 1);
    bool push_2 = (muTrackHandle_->size() > 2);
    bool push_3 = (muTrackHandle_->size() > 3);

	
    reco::TrackRef leadingMuRef, subleadingMuRef, subleadingMuRef2, subleadingMuRef3;
    reco::TrackRef leadingMuRefQ;// = NULL;
    reco::TrackRef subleadingMuRefQ;// = NULL; 
    //if (goodQualityTracks > 0)
        if(push_0){leadingMuRef = reco::TrackRef(muTrackHandle_, leadingMuIndex);}
    //if (goodQualityTracks > 1)
        if(push_1){subleadingMuRef = reco::TrackRef(muTrackHandle_, subleadingMuIndex);}
        if(push_2){subleadingMuRef2 = reco::TrackRef(muTrackHandle_, subleadingMuIndex2);}
        if(push_3){subleadingMuRef3 = reco::TrackRef(muTrackHandle_, subleadingMuIndex3);}
    
   
//	float dR_matchl = 10000;
//	float dR_matchs = 10000;
//	float dR_matchs2 = 10000;
//	float dR_matchs3 = 10000;
//	int reverse =0;
//	float gen_pt=0;
//    for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
//        reco::GenParticleRef gp(genParticleHandle_, ig); 
//	if(abs(gp->pdgId())==13 && gp->isHardProcess()){
//		if (gp->pt() > gen_pt){reverse++; gen_pt=gp->pt();}
//		if(push_0){
//		float dphi1 = gp->phi() - leadingMuRef->phi();
//        	if (std::abs(dphi1) > 3.1415)
//            		dphi1 -= 2 * 3.1415 * dphi1/std::abs(dphi1);	
//		float deta1 = gp->eta() - leadingMuRef->eta();
//		float dR_match_temp1 = sqrt(dphi1*dphi1 + deta1*deta1);
//		if (dR_match_temp1 < dR_matchl){ dR_matchl = dR_match_temp1;}
//		}
//		if(push_1){
//		float dphi2 = gp->phi() - subleadingMuRef->phi();
//        	if (std::abs(dphi2) > 3.1415)
//            		dphi2 -= 2 * 3.1415 * dphi2/std::abs(dphi2);	
//		float deta2 = gp->eta() - subleadingMuRef->eta();
//		float dR_match_temp2 = sqrt(dphi2*dphi2 + deta2*deta2);
//		if (dR_match_temp2 < dR_matchs){ dR_matchs = dR_match_temp2;}
//		}
//		if(push_2){
//		float dphi3 = gp->phi() - subleadingMuRef2->phi();
//        	if (std::abs(dphi3) > 3.1415)
//            		dphi3 -= 2 * 3.1415 * dphi3/std::abs(dphi3);	
//		float deta3 = gp->eta() - subleadingMuRef2->eta();
//		float dR_match_temp3 = sqrt(dphi3*dphi3 + deta3*deta3);
//		if (dR_match_temp3 < dR_matchs2){ dR_matchs2 = dR_match_temp3;}
//		}
//		if(push_3){
//		float dphi4 = gp->phi() - subleadingMuRef3->phi();
//        	if (std::abs(dphi4) > 3.1415)
//            		dphi4 -= 2 * 3.1415 * dphi4/std::abs(dphi4);	
//		float deta4 = gp->eta() - subleadingMuRef3->eta();
//		float dR_match_temp4 = sqrt(dphi4*dphi4 + deta4*deta4);
//		if (dR_match_temp4 < dR_matchs3){ dR_matchs3 = dR_match_temp4;}
//		}
//		
//        	genPt_ .push_back(gp->pt());
// 	        genEta_.push_back(gp->eta());
//       	 	genPhi_.push_back(gp->phi());
//       	 	genVxy_.push_back(sqrt(gp->vx()*gp->vx() + gp->vy()*gp->vy()));
//       		genVz_ .push_back(gp->vz());
//		}	
//	}
//	if (reverse>1){
//
//        	std::reverse(genPt_.begin(), genPt_.end()); 
// 	        std::reverse(genEta_.begin(),genEta_.end());
//       	 	std::reverse(genPhi_.begin(),genPhi_.end());
//       	 	std::reverse(genVxy_.begin(),genVxy_.end());
//       		std::reverse(genVz_ .begin(),genVz_.end());
//	}
//	if(push_0){
//	isGenMatched.push_back(dR_matchl<0.3);
//	GenDR.push_back(dR_matchl);
//	}
//	if(push_1){
//	isGenMatched.push_back(dR_matchs<0.3);
//	GenDR.push_back(dR_matchs);
//	}
//	if(push_2){
//		isGenMatched.push_back(dR_matchs2<0.3);
//		GenDR.push_back(dR_matchs2);}
//	if(push_3){
//	isGenMatched.push_back(dR_matchs3<0.3);
//	GenDR.push_back(dR_matchs3);}
//	float dR_matchs = 10000;
//    for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
//        reco::GenParticleRef gp(genParticleHandle_, ig); 
//	if(abs(gp->pdgId())==13){
//		float dphi = gp->phi() - leadingMuRef->phi();
//        	if (std::abs(dphi) > 3.1415)
//            		dphi -= 2 * 3.1415 * dphi/std::abs(dphi);	
//		float deta = gp->eta() - leadingMuRef->eta();
//		float dR_matchs_temp = sqrt(dphi*dphi + deta*deta);
//		if (dR_matchs_temp < dR_matchs){ dR_matchs = dR_matchs_temp;}
//		}	
//	}
//	isGenMatched.push_back(dR_matchs<0.3);
//	GenDR.push_back(dR_matchs);
    // add top 2 leading pt reco muons to vector, after minimum quality cut
    // Edit: reco muons that pass minimum quality cut
    // (to cut on weird 11 GeV peak muons due to cosmic seed in dSA algo)
    /*for (size_t i = 0; i < muTrackHandle_->size(); ++i) { 
        reco::TrackRef muTrack(muTrackHandle_, i);
        if (muTrack->hitPattern().muonStationsWithValidHits() < 2 ||
                muTrack->hitPattern().numberOfValidMuonHits() < 12 ||
                muTrack->normalizedChi2() > 10)
            continue;
        recoPt_.push_back(muTrack->pt());
        recoEta_.push_back(muTrack->eta());
        recoPhi_.push_back(muTrack->phi());
        recoDxy_.push_back(muTrack->dxy());
        recoDz_.push_back(muTrack->dz());
    }*/

    // Same with vertices
    // Make all possible vertices with pairs of muons
    // Ordering will be e.g. for 3 muons:
    // recoVxy[0] -> {mu0, mu1}
    // recoVxy[1] -> {mu0, mu2}
    // recoVxy[2] -> {mu1, mu2}
    // If vertex is not valid set to 0
    // Same ordering for deltaR vector
    /*ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

    for (size_t i = 0; i < muTrackHandle_->size(); ++i) {
        reco::TrackRef ref_i(muTrackHandle_, i);
        for (size_t j = i+1; j < muTrackHandle_->size(); ++j) {
            reco::TrackRef ref_j(muTrackHandle_, j);

            deltaR_.push_back(deltaR(*(ref_i.get()), *(ref_j.get())));

            vector<reco::TransientTrack> t_tks{};
            t_tks.push_back(theB->build(&ref_i));
            t_tks.push_back(theB->build(&ref_j));
            KalmanVertexFitter kvf(true);
            TransientVertex tv = kvf.vertex(t_tks);

            float _vxy, _vz;
            if (tv.isValid()) {
                reco::Vertex _vtx = reco::Vertex(tv);
                _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
                _vz  = _vtx.z();
            }
            else {
                _vxy = 0;
                _vz = 0;
            }
            recoVxy_.push_back(_vxy);
            recoVz_.push_back(_vz);
        }
    }*/

 //  if (goodQualityTracks > 0) {
   int passQ =0;
	if(push_0){
        recoPt_ .push_back(leadingMuRef->pt());
        recoEta_.push_back(leadingMuRef->eta());
        recoPhi_.push_back(leadingMuRef->phi());
        recoDxy_.push_back(leadingMuRef->dxy());
        recoDz_ .push_back(leadingMuRef->dz());
        trackPlanes.push_back(leadingMuRef ->hitPattern().muonStationsWithValidHits());
        trackHits.push_back(leadingMuRef ->hitPattern().numberOfValidMuonHits());
        trackChi2.push_back(leadingMuRef ->normalizedChi2());
        if (leadingMuRef->hitPattern().muonStationsWithValidHits() < 2 ||
                leadingMuRef->hitPattern().numberOfValidMuonHits() < 12 ||
                leadingMuRef->normalizedChi2() > 10){ qualityTrack.push_back(false);passQ++;}
	else{ goodQualityTracks++; qualityTrack.push_back(true);leadingMuRefQ = leadingMuRef;}
}
	if(push_1){
//        if (goodQualityTracks > 1) {
            recoPt_ .push_back(subleadingMuRef->pt());
            recoEta_.push_back(subleadingMuRef->eta());
            recoPhi_.push_back(subleadingMuRef->phi());
            recoDxy_.push_back(subleadingMuRef->dxy());
            recoDz_ .push_back(subleadingMuRef->dz());
            trackPlanes.push_back(subleadingMuRef ->hitPattern().muonStationsWithValidHits());
            trackHits.push_back(subleadingMuRef ->hitPattern().numberOfValidMuonHits());
            trackChi2.push_back(subleadingMuRef ->normalizedChi2());
            if (subleadingMuRef->hitPattern().muonStationsWithValidHits() < 2 ||
                subleadingMuRef->hitPattern().numberOfValidMuonHits() < 12 ||
                subleadingMuRef->normalizedChi2() > 10){ 
		qualityTrack.push_back(false);	
		passQ++;}
  	    else{ goodQualityTracks++; qualityTrack.push_back(true);
		if(passQ==0){ subleadingMuRefQ= subleadingMuRef;}
	//	if(leadingMuRefQ){ leadingMuRefQ = subleadingMuRef;}
	//	else{ subleadingMuRefQ = subleadingMuRef;}
		else{ leadingMuRefQ= subleadingMuRef;}
		}
            //deltaR_ .push_back(dR);
      //  }
   // }
		}

	    if(push_2){
            recoPt_ .push_back(subleadingMuRef2->pt());
            recoEta_.push_back(subleadingMuRef2->eta());
            recoPhi_.push_back(subleadingMuRef2->phi());
            recoDxy_.push_back(subleadingMuRef2->dxy());
            recoDz_ .push_back(subleadingMuRef2->dz());
            trackPlanes.push_back(subleadingMuRef2 ->hitPattern().muonStationsWithValidHits());
            trackHits.push_back(subleadingMuRef2 ->hitPattern().numberOfValidMuonHits());
            trackChi2.push_back(subleadingMuRef2 ->normalizedChi2());
            if (subleadingMuRef2->hitPattern().muonStationsWithValidHits() < 2 ||
                subleadingMuRef2->hitPattern().numberOfValidMuonHits() < 12 ||
                subleadingMuRef2->normalizedChi2() > 10){ qualityTrack.push_back(false); passQ++;}
  	    else{ goodQualityTracks++; qualityTrack.push_back(true);
		if(passQ==1){subleadingMuRefQ= subleadingMuRef2;}
		else if(passQ==2){leadingMuRefQ= subleadingMuRef2;}
		//if(subleadingMuRefQ){
		//if(leadingMuRefQ ){
		//leadingMuRefQ = subleadingMuRef2;}
		//else{
		//subleadingMuRefQ = subleadingMuRef2;}
		//}
		}
	}
	    if(push_3){
            recoPt_ .push_back(subleadingMuRef3->pt());
            recoEta_.push_back(subleadingMuRef3->eta());
            recoPhi_.push_back(subleadingMuRef3->phi());
            recoDxy_.push_back(subleadingMuRef3->dxy());
            recoDz_ .push_back(subleadingMuRef3->dz());
            trackPlanes.push_back(subleadingMuRef3 ->hitPattern().muonStationsWithValidHits());
            trackHits.push_back(subleadingMuRef3 ->hitPattern().numberOfValidMuonHits());
            trackChi2.push_back(subleadingMuRef3 ->normalizedChi2());
            if (subleadingMuRef3->hitPattern().muonStationsWithValidHits() < 2 ||
                subleadingMuRef3->hitPattern().numberOfValidMuonHits() < 12 ||
                subleadingMuRef3->normalizedChi2() > 10){ qualityTrack.push_back(false);}
  	    else{ goodQualityTracks++; qualityTrack.push_back(true);
		if(passQ==2){subleadingMuRefQ= subleadingMuRef3;}
		if(passQ>=3){goodQualityTracks=0;}
		//if(subleadingMuRefQ){
		//if(leadingMuRefQ){
		//leadingMuRefQ = subleadingMuRef3;}
		//else{
		////subleadingMuRefQ = subleadingMuRef3;}
		//goodQualityTracks=0;}
		//}
		}
	}

    // vtxing between reco mu pair
    ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

    vector<reco::TransientTrack> t_trks{};
    KalmanVertexFitter kvf(true);
    TransientVertex tv;
    float _vxy = 0, _vz = 0, dR = -10;
//	bool include = false;
	//bool full = false;
    if (goodQualityTracks > 1) {
	t_trks.push_back(theB->build(&leadingMuRefQ));
        t_trks.push_back(theB->build(&subleadingMuRefQ));
       	tv = kvf.vertex(t_trks);
        if (!tv.isValid()) {
            std::cout << "Warning! TV is not valid!" << std::endl;
        }
        else {
            reco::Vertex _vtx = reco::Vertex(tv);
            _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
            _vz  = _vtx.z();

            dR = deltaR(*(leadingMuRefQ), *(subleadingMuRefQ));

        }
       }
	    recoVxy_.push_back(_vxy);
            recoVz_ .push_back(_vz);
            recoDr_ .push_back(dR);

    // Trigger check firing bit (MET+MHT 120 GeV trigger)
//    const vector<string>& pathNames = hltConfig_.triggerNames();
//    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
//    if (matchedPaths.size() == 0) {
//        LogError("TrackQuality") << "Could not find matched full trigger path with -> " << trigPathNoVer_ << endl;
//        return;
//    }
//    trigPath_ = matchedPaths[0];
//    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
//        LogError("TrackQuality") << "Cannot find trigger path -> " << trigPath_ << endl;
//        return;
//    }
    bool fired = recoPFMetPt_>120? true: false; //trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));
    fired_ = fired;

    /******* BEGINNING OF CUTS ******/

    // Fill everything before cuts (cutsTree[0])
    //cutsTree[0]->Fill();
    cutsVec[0] = 1;
    
    if (fired) {
        //cutsTree[1]->Fill();
        cutsVec[1] = 1;
        //std::cout << "passed cut 1" << std::endl;
    }
    //else
        //return;

    //// Trigger emulation: reco MET > 120 GeV
    //if (recoPFMetPt_ > 120) {
        //cutsTree[1]->Fill();
        ////std::cout << "passed cut 1" << std::endl;
    //}
    //else
        //return;

    //// Trigger emulation: reco MHT > 120 GeV
    //if (MHT.pt() > 120) {
        //cutsTree[2]->Fill();
        ////std::cout << "passed cut 2" << std::endl;
    //}
    //else
        //return;

    // One leading reco jet w/ pT > 120 and only one extra jet w/ pT > 30 GeV.
    // If the conditions in the previous line are satisfied, does it follow
    // that MHT and leading jet are roughly back-to-back? I think so.
    // TODO: quantify this statement.
    if (maxJetPt > 120 && flagHighPtJets == false) {
        //cutsTree[2]->Fill();
        cutsVec[2] = 1;
        //std::cout << "passed cut 2" << std::endl;
    }
    //else
        //return;

    // First muon has impact parameter between 1 mm and 700 /*30*/ cm and pt > 5 GeV
    if (goodQualityTracks > 0) {
        if (leadingMuRefQ->pt() > 5 && std::abs(leadingMuRefQ->dxy()) > 0.1
                && std::abs(leadingMuRefQ->dxy()) < 700/*30*/) {
            //cutsTree[3]->Fill();
            cutsVec[3] = 1;
            //std::cout << "passed cut 3" << std::endl;
        }
        //else {
            ////std::cout << "Didn't pass cut 3, pt of leading mu: " << leadingMuRef->pt() << ", d0: " << std::sqrt(_vxy*_vxy + _vz*_vz) << std::endl;
            ////std::cout << "muTrackHandle_->size(): " << muTrackHandle_->size() << std::endl;
            //return;
        //}
    }
    //else
        //return;

    // Second muon, same criterion plus check if vertex is valid and dR < 0.4
    if (goodQualityTracks > 1) {
        if (subleadingMuRefQ->pt() > 5 && std::abs(subleadingMuRefQ->dxy()) > 0.1
                && std::abs(subleadingMuRefQ->dxy()) < 700 /*30*/ && tv.isValid() /*&& recoDr_[0] < 0.4*/) {
            //cutsTree[4]->Fill();
            cutsVec[4] = 1;
            //std::cout << "passed cut 4" << std::endl;
        }
        //else
            //return;
    }
    //else
        //return;
        
    // delta phi between MET and muon pair < 0.4
    delPhiMetMu = -10;
    diMuonPhi = -10;
    if (goodQualityTracks > 1) {
        diMuonPhi = atan2( sin(leadingMuRefQ->phi()) + sin(subleadingMuRefQ->phi()), cos(leadingMuRefQ->phi()) + cos(subleadingMuRefQ->phi()) );
        double phiDiff = recoMetr->phi() - diMuonPhi;
        double reducedPhiDiff = phiDiff;
        if (std::abs(reducedPhiDiff) > 3.1415)
            reducedPhiDiff -= 2 * 3.1415 * reducedPhiDiff/std::abs(reducedPhiDiff);

        //if (std::abs(recoMetr->phi() - (leadingMuRef->phi() + subleadingMuRef->phi())/2)
         //       < 0.4) {
        
        delPhiMetMu = reducedPhiDiff;
        if (std::abs(reducedPhiDiff) < 0.4) {
            //cutsTree[5]->Fill();
            cutsVec[5] = 1;
            //std::cout << "passed cut 5" << std::endl;
        }
    }
    //else return;
    cutsTree->Fill();

    return;
    
}

void TrackQuality::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void TrackQuality::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(TrackQuality);