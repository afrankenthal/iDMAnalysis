#include "iDMSkimmer/washAOD/interface/MetTrigSelfEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

MetTrigSelfEffiForMuTrack::MetTrigSelfEffiForMuTrack(const edm::ParameterSet& ps) :
    muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
    genEvtInfoTag_(ps.getParameter<edm::InputTag>("genEvt")),
//    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
//    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
//    genMetTag_(ps.getParameter<edm::InputTag>("genMet")),
    caloMetTag_(ps.getParameter<edm::InputTag>("caloMet")),
    recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
    trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
    trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
    trigPathNoVer0_(ps.getParameter<std::string>("trigPath0")),
    processName_(ps.getParameter<std::string>("processName")),
    nMuons_(ps.getParameter<int>("nMuons")),
    muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_)),
//    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
//    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
//    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
    caloMetToken_(consumes<reco::CaloMETCollection>(caloMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_))
{
    usesResource("TFileService");
}

MetTrigSelfEffiForMuTrack::~MetTrigSelfEffiForMuTrack() = default;

void MetTrigSelfEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));
//    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
//    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
//    desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("caloMet", edm::InputTag("caloMet"));
    desc.add<edm::InputTag>("recoMet", edm::InputTag("pfMet"));
    desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults","","HLT"));
    desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD","","HLT"));
    desc.add<std::string>("trigPath", "Defaultshouldntbecalled!");
    desc.add<std::string>("trigPath0", "Defaultshouldntbecalled!");
    desc.add<std::string>("processName", "HLT");
    desc.add<int>("nMuons", 2);
    descriptions.add("MetTrigSelfEffiForMuTrack", desc);
}

void MetTrigSelfEffiForMuTrack::beginJob()
{
    muTrackT_ = fs->make<TTree>("MetTrigSelfEffiForMuTrack", "");

    muTrackT_->Branch("fired", &fired_, "fired/O");
    muTrackT_->Branch("fired0", &fired0_, "fired0/O");
    muTrackT_->Branch("pt",   &pt_);
    muTrackT_->Branch("eta",  &eta_);
    muTrackT_->Branch("phi",  &phi_);
//    muTrackT_->Branch("genJetPt", &genJetPt_, "genJetPt/F");
    muTrackT_->Branch("recoJetPt", &recoJetPt_, "recoJetPt/F");
    muTrackT_->Branch("recoJetEta", &recoJetEta_, "recoJetEta/F");
    muTrackT_->Branch("recoJetPhi", &recoJetPhi_, "recoJetPhi/F");
    muTrackT_->Branch("chargeHadronFraction", &chargedHadronEnergyFraction_, "chargedHadronEnergyFraction/F");
    muTrackT_->Branch("neutralHadronFraction", &neutralHadronEnergyFraction_, "neutralHadronEnergyFraction/F");
    muTrackT_->Branch("recoJetPt1", &recoJetPt1_, "recoJetPt1/F");
    muTrackT_->Branch("recoJetEta1", &recoJetEta1_, "recoJetEta1/F");
    muTrackT_->Branch("recoJetPhi1", &recoJetPhi1_, "recoJetPhi1/F");
    muTrackT_->Branch("recoJetPt2", &recoJetPt2_, "recoJetPt2/F");
    muTrackT_->Branch("recoJetEta2", &recoJetEta2_, "recoJetEta2/F");
    muTrackT_->Branch("recoJetPhi2", &recoJetPhi2_, "recoJetPhi2/F");
//    muTrackT_->Branch("genLeadMetPt", &genLeadMetPt_, "genLeadMetPt/F");
//    muTrackT_->Branch("genSubLeadMetPt", &genSubLeadMetPt_, "genSubLeadMetPt/F");
    muTrackT_->Branch("caloPFMetPt", &caloPFMetPt_, "caloPFMetPt/F");
    muTrackT_->Branch("caloPFMetEta", &caloPFMetEta_, "caloPFMetEta/F");
    muTrackT_->Branch("caloPFMetPhi", &caloPFMetPhi_, "caloPFMetPhi/F");
    muTrackT_->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
    muTrackT_->Branch("recoPFMetEta", &recoPFMetEta_, "recoPFMetEta/F");
    muTrackT_->Branch("recoPFMetPhi", &recoPFMetPhi_, "recoPFMetPhi/F");
    muTrackT_->Branch("recoPFMETJetDeltaPhi", &recoPFMETJetDeltaPhi_ , "recoPFMETJetDeltaPhi/F");
    muTrackT_->Branch("genwgt", &genwgt_, "genwgt/F");
    genweight_ = fs->make<TTree>("GenWeight", "");
    genweight_->Branch("genwgt", &genwgt_, "genwgt/F");
}

void MetTrigSelfEffiForMuTrack::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    using namespace std;
    using namespace edm;

    bool changed(true);
    if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
        if (changed) {
            LogInfo("MetTrigSelfEffiForMuTrack")<<"MetTrigSelfEffiForMuTrack::beginRun: "<<"hltConfig init for Run"<<iRun.run();
            hltConfig_.dump("ProcessName");
            hltConfig_.dump("GlobalTag");
            hltConfig_.dump("TableName");
        }
    } else {
        LogError("MetTrigSelfEffiForMuTrack")<<"MetTrigSelfEffiForMuTrack::beginRun: config extraction failure with processName -> "
            <<processName_;
    }

}

void MetTrigSelfEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogError("MetTrigSelfEffiForMuTrack")
            << "MetTrigSelfEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
            << endl;
        return;
    }
//    iEvent.getByToken(genParticleToken_, genParticleHandle_);
//    if (!genParticleHandle_.isValid()) {
//        LogError("trigEffiForMuTrack")
//            << "trigEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
//            << endl;
//        return;
//    }
//    iEvent.getByToken(genJetToken_, genJetHandle_);
//    if (!genJetHandle_.isValid()) {
//        LogError("trigEffiForMuTrack")
//            << "trigEffiForMuTrack::analyze: Error in getting genJet product from Event!"
//            << endl;
//        return;
//    }
//    iEvent.getByToken(genMetToken_, genMetHandle_);
//    if (!genMetHandle_.isValid()) {
//        LogError("trigEffiForMuTrack")
//            << "trigEffiForMuTrack::analyze: Error in getting genMet product from Event!"
//            << endl;
//        return;
//    }
    iEvent.getByToken(caloMetToken_, caloMetHandle_);
    if (!caloMetHandle_.isValid()) {
        LogError("trigEffiForMuTrack") << "trigEffiForMuTrack::analyze: Error in getting caloMet product from Event!" << endl;
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
        LogError("MetTrigSelfEffiForMuTrack")
            << "MetTrigSelfEffiForMuTrack::analyze: Error in getting triggerResults product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(trigEventToken_, trigEventHandle_);
    if (!trigEventHandle_.isValid()) {
        LogError("MetTrigSelfEffiForMuTrack")
            << "MetTrigSelfEffiForMuTrack::analyze: Error in getting triggerEvent product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(genEvtInfoToken_, genEvtInfoHandle_);
    genwgt_=-9999;
    if (!genEvtInfoHandle_.isValid()) {
       // LogError("HandleError") << boost::str(boost::format(error_msg) % "genEventInfo");
        genwgt_=1;
    }
    else{
	// Gen weight
    	 genwgt_ = genEvtInfoHandle_->weight();		
	}
    genweight_->Fill();

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

//    genPt_.clear(); genPt_.reserve(2);
//    genEta_.clear(); genEta_.reserve(2);
//    genPhi_.clear(); genPhi_.reserve(2);
//    genVxy_.clear(); genVxy_.reserve(2);
//    genVz_.clear(); genVz_.reserve(2);
//
//    for (size_t i = 0; i < genParticleHandle_->size(); i++) {
//        reco::GenParticleRef genP(genParticleHandle_, i);
//        if (std::abs(genP->pdgId()) != 13) continue;
//        genPt_.push_back(genP->pt());
//        genEta_.push_back(genP->eta());
//        genPhi_.push_back(genP->phi());
//        genVxy_.push_back(genP->vertex().rho());
//        genVz_.push_back(genP->vz());
//    }

    for (const auto& recoMu : muRefs) {
        pt_ .push_back(recoMu->pt());
        eta_.push_back(recoMu->eta());
        phi_.push_back(recoMu->phi());
    }

    // trigger firing condition
    const vector<string>& pathNames0 = hltConfig_.triggerNames();
    const vector<string> matchedPaths0(hltConfig_.restoreVersion(pathNames0, trigPathNoVer0_));
    if (matchedPaths0.size() == 0) {
        LogError("MetTrigSelfEffiForMuTrack")<<"Could not find matched full trigger path with -> "<<trigPathNoVer0_<<endl;
        return;
    }
    trigPath0_ = matchedPaths0[0];
    if (hltConfig_.triggerIndex(trigPath0_) >= hltConfig_.size()) {
        LogError("MetTrigSelfEffiForMuTrack")<<"Cannot find trigger path -> "<<trigPath0_<<endl;
        return;
    }
    fired0_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath0_));

    // trigger firing condition
    const vector<string>& pathNames = hltConfig_.triggerNames();
    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
    if (matchedPaths.size() == 0) {
        LogError("MetTrigSelfEffiForMuTrack")<<"Could not find matched full trigger path with -> "<<trigPathNoVer_<<endl;
        return;
    }
    trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
        LogError("MetTrigSelfEffiForMuTrack")<<"Cannot find trigger path -> "<<trigPath_<<endl;
        return;
    }
    fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

 //   double largestPt = 0.0;
//    for (size_t i(0); i != genJetHandle_->size(); ++i) {
//        reco::GenJetRef jetr(genJetHandle_, i);
//        if (jetr->pt() > largestPt) {
//            largestPt = jetr->pt();
//        }
//    }
//    genJetPt_ = largestPt;

    double largestPt = 0.0;
    double largestEta = 0.0, largestPhi = 0.0;
    double largestPt1 = 0.0;
    double largestEta1 = 0.0, largestPhi1 = 0.0;
    double largestPt2 = 0.0;
    double largestEta2 = 0.0, largestPhi2 = 0.0;
    chargedHadronEnergyFraction_ = 0.0;
    neutralHadronEnergyFraction_ = 0.0;
    for (size_t i(0); i != recoJetHandle_->size(); ++i) {
        reco::PFJetRef jetr(recoJetHandle_, i);
        if (jetr->pt() > largestPt) {
	    largestPt2 = largestPt1;
	    largestEta2 = largestEta1;
	    largestPhi2 = largestPhi1;
	    largestPt1 = largestPt;
	    largestEta1 = largestEta;
	    largestPhi1 = largestPhi;
            largestPt = jetr->pt();
            largestEta = jetr->eta();
            largestPhi = jetr->phi();
	    chargedHadronEnergyFraction_ = jetr->chargedHadronEnergyFraction();
	    neutralHadronEnergyFraction_ = jetr->neutralHadronEnergyFraction();
        }
        else if (jetr->pt() > largestPt1) {
	    largestPt2 = largestPt1;
	    largestEta2 = largestEta1;
	    largestPhi2 = largestPhi1;
            largestPt1 = jetr->pt();
            largestEta1 = jetr->eta();
            largestPhi1 = jetr->phi();
        }
        else if (jetr->pt() > largestPt2) {
            largestPt2 = jetr->pt();
            largestEta2 = jetr->eta();
            largestPhi2 = jetr->phi();
        }
    }
    recoJetPt_ = largestPt;
    recoJetEta_ = largestEta;
    recoJetPhi_ = largestPhi;
    recoJetPt1_ = largestPt1;
    recoJetEta1_ = largestEta1;
    recoJetPhi1_ = largestPhi1;
    recoJetPt2_ = largestPt2;
    recoJetEta2_ = largestEta2;
    recoJetPhi2_ = largestPhi2;

    // Fill MET branches
//    reco::GenMETRef metr(genMetHandle_, 0);
//    genLeadMetPt_ = metr->pt();
//    reco::GenMETRef metr2(genMetHandle_, 1);
//    genSubLeadMetPt_ = metr2->pt();
    reco::CaloMETRef metr3c(caloMetHandle_, 0);
    caloPFMetPt_ = metr3c->pt();
    caloPFMetEta_ = metr3c->eta();
    caloPFMetPhi_ = metr3c->phi();
    reco::PFMETRef metr3(recoMetHandle_, 0);
    recoPFMetPt_ = metr3->pt();
    recoPFMetEta_ = metr3->eta();
    recoPFMetPhi_ = metr3->phi();
    


    recoPFMETJetDeltaPhi_ = reco::deltaPhi(recoJetPhi_, recoPFMetPhi_);
    
    //save if Enriched criteria is met
    bool enrich = false;
    if( (recoJetPt_ >100) && (recoJetEta_<2.4) && (fired0_==true) ){//&& (pt_.at(0)>25) && (eta_.at(0)<2.5)){
	enrich = true;
	}
    if (enrich){
    muTrackT_->Fill();
	}
    return;
}

void MetTrigSelfEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void MetTrigSelfEffiForMuTrack::endJob() {}
