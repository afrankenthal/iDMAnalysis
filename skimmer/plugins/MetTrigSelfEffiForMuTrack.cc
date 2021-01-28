/**
 * Trigger efficiencies itself
 * =======================================
 * Require:
 *   - >=2 dSA muons passing general selections
 * Check trigger firing condition
**/

#include <cmath>

#include <boost/format.hpp>
#include <boost/any.hpp>

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TTree.h"

class MetTrigSelfEffiForMuTrack : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit MetTrigSelfEffiForMuTrack(const edm::ParameterSet&);
        ~MetTrigSelfEffiForMuTrack();

        static void fillDescriptions(edm::ConfigurationDescriptions&);

    private:
        virtual void beginJob() override;
        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endJob() override;

        const edm::InputTag muTrackTag_;
	const edm::InputTag genEvtInfoTag_;
 

        const edm::InputTag genParticleTag_;
       const edm::InputTag genJetTag_;
        const edm::InputTag genMetTag_;

        const edm::InputTag caloMetTag_;
        const edm::InputTag recoMetTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
 const edm::InputTag HBHENoiseFilterResultProducerTag_;
        const edm::InputTag primaryVertexFilterTag_;
        const edm::InputTag globalSuperTightHalo2016FilterTag_;
        const edm::InputTag EcalDeadCellTriggerPrimitiveFilterTag_;
        const edm::InputTag ecalBadCalibFilterTag_;
        const edm::InputTag BadPFMuonFilterTag_;
        const edm::InputTag muonBadTrackFilterTag_;

        const std::string trigPathNoVer_;
        const std::string trigPathNoVer0_;
        const std::string processName_;
        const int nMuons_;
        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken_;
	const edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_;
        const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

        const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
        const edm::EDGetTokenT<reco::GenMETCollection> genMetToken_;

        const edm::EDGetTokenT<reco::CaloMETCollection> caloMetToken_;
        const edm::EDGetTokenT<reco::PFMETCollection> recoMetToken_;
        const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
        const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
        const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
	 const edm::EDGetTokenT<bool>HBHENoiseFilterResultProducerToken_;
        const edm::EDGetTokenT<int>primaryVertexFilterToken_;
        const edm::EDGetTokenT<bool>globalSuperTightHalo2016FilterToken_;
        const edm::EDGetTokenT<bool>EcalDeadCellTriggerPrimitiveFilterToken_;
        const edm::EDGetTokenT<bool>ecalBadCalibFilterToken_;
        const edm::EDGetTokenT<bool>BadPFMuonFilterToken_;
        const edm::EDGetTokenT<bool>muonBadTrackFilterToken_;


        edm::Service<TFileService> fs;
        edm::Handle<reco::TrackCollection> muTrackHandle_;
	edm::Handle<GenEventInfoProduct> genEvtInfoHandle_;
        
        edm::Handle<reco::GenParticleCollection> genParticleHandle_;
        edm::Handle<reco::GenJetCollection> genJetHandle_;
        edm::Handle<reco::GenMETCollection> genMetHandle_;
        
        edm::Handle<reco::CaloMETCollection> caloMetHandle_;
        edm::Handle<reco::PFMETCollection> recoMetHandle_;
        edm::Handle<reco::PFJetCollection> recoJetHandle_;
        edm::Handle<edm::TriggerResults> trigResultsHandle_;
        edm::Handle<trigger::TriggerEvent> trigEventHandle_;
edm::Handle<bool> HBHENoiseFilterResultProducerHandle_;
        edm::Handle<int> primaryVertexFilterHandle_;
        edm::Handle<bool> globalSuperTightHalo2016FilterHandle_;
        edm::Handle<bool> EcalDeadCellTriggerPrimitiveFilterHandle_;
        edm::Handle<bool> ecalBadCalibFilterHandle_;
        edm::Handle<bool> BadPFMuonFilterHandle_;
        edm::Handle<bool> muonBadTrackFilterHandle_;

        std::string trigPath_;
        std::string trigPath0_;
        HLTConfigProvider hltConfig_;

        bool fired_;
        bool fired0_;

        std::vector<float> pt_;
        std::vector<float> eta_;
        std::vector<float> phi_;

        float caloPFMetPt_;
        float caloPFMetEta_;
        float caloPFMetPhi_;
        float recoPFMetPt_;
        float recoPFMetEta_;
        float recoPFMetPhi_;
        float recoJetPt_;
        float recoJetEta_;
        float recoJetPhi_;
        float recoJetPt1_;
        float recoJetEta1_;
        float recoJetPhi1_;
        float recoJetPt2_;
        float recoJetEta2_;
        float recoJetPhi2_;
	float genwgt_;
	float chargedHadronEnergyFraction_;
	float neutralHadronEnergyFraction_;
	float recoPFMETJetDeltaPhi_;
	float recoPFrecoilJetDeltaPhi_;
	float recoil_;
	float metmufrac_;
	float metnomu_;
	float musum_;

        std::vector<float> genpt_;
        std::vector<float> geneta_;
        std::vector<float> genphi_;

        float genPFMetPt_;
        float genPFMetEta_;
        float genPFMetPhi_;
        float genJetPt_;
        float genJetEta_;
        float genJetPhi_;
        float genJetPt1_;
        float genJetEta1_;
        float genJetPhi1_;
        float genJetPt2_;
        float genJetEta2_;
        float genJetPhi2_;
	float genchargedHadronEnergyFraction_;
	float genneutralHadronEnergyFraction_;
	float genPFMETJetDeltaPhi_;
	float genPFrecoilJetDeltaPhi_;
	float genrecoil_;
        TTree *genTrackT_;

	bool runGen = false;
        TTree *muTrackT_;
        TTree *genweight_;
};


MetTrigSelfEffiForMuTrack::MetTrigSelfEffiForMuTrack(const edm::ParameterSet& ps) :
    muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
    genEvtInfoTag_(ps.getParameter<edm::InputTag>("genEvt")),

    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
    genMetTag_(ps.getParameter<edm::InputTag>("genMet")),

    caloMetTag_(ps.getParameter<edm::InputTag>("caloMet")),
    recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
    trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
    HBHENoiseFilterResultProducerTag_("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"),
    primaryVertexFilterTag_("myPrimaryVertexFilter"),
    globalSuperTightHalo2016FilterTag_("globalSuperTightHalo2016Filter"),
    EcalDeadCellTriggerPrimitiveFilterTag_("EcalDeadCellTriggerPrimitiveFilter"),
    ecalBadCalibFilterTag_("ecalBadCalibFilter"),
    BadPFMuonFilterTag_("BadPFMuonFilter"),
    muonBadTrackFilterTag_("muonBadTrackFilter"),
    trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
    trigPathNoVer0_(ps.getParameter<std::string>("trigPath0")),
    processName_(ps.getParameter<std::string>("processName")),


    nMuons_(ps.getParameter<int>("nMuons")),
    muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_)),

    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),

    caloMetToken_(consumes<reco::CaloMETCollection>(caloMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
HBHENoiseFilterResultProducerToken_(consumes<bool>(HBHENoiseFilterResultProducerTag_)),
    primaryVertexFilterToken_(consumes<int>(primaryVertexFilterTag_)),
    globalSuperTightHalo2016FilterToken_(consumes<bool>(globalSuperTightHalo2016FilterTag_)),
    EcalDeadCellTriggerPrimitiveFilterToken_(consumes<bool>(EcalDeadCellTriggerPrimitiveFilterTag_)),
    ecalBadCalibFilterToken_(consumes<bool>(ecalBadCalibFilterTag_)),
    BadPFMuonFilterToken_(consumes<bool>(BadPFMuonFilterTag_)),
    muonBadTrackFilterToken_(consumes<bool>(muonBadTrackFilterTag_))
{
    usesResource("TFileService");
}

MetTrigSelfEffiForMuTrack::~MetTrigSelfEffiForMuTrack() = default;

void MetTrigSelfEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));

    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));

    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
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
    muTrackT_->Branch("recoJetPt", &recoJetPt_, "recoJetPt/F");
    muTrackT_->Branch("recoJetEta", &recoJetEta_, "recoJetEta/F");
    muTrackT_->Branch("recoJetPhi", &recoJetPhi_, "recoJetPhi/F");
    muTrackT_->Branch("chargedHadronFraction", &chargedHadronEnergyFraction_, "chargedHadronEnergyFraction/F");
    muTrackT_->Branch("neutralHadronFraction", &neutralHadronEnergyFraction_, "neutralHadronEnergyFraction/F");
    muTrackT_->Branch("recoJetPt1", &recoJetPt1_, "recoJetPt1/F");
    muTrackT_->Branch("recoJetEta1", &recoJetEta1_, "recoJetEta1/F");
    muTrackT_->Branch("recoJetPhi1", &recoJetPhi1_, "recoJetPhi1/F");
    muTrackT_->Branch("recoJetPt2", &recoJetPt2_, "recoJetPt2/F");
    muTrackT_->Branch("recoJetEta2", &recoJetEta2_, "recoJetEta2/F");
    muTrackT_->Branch("recoJetPhi2", &recoJetPhi2_, "recoJetPhi2/F");
    muTrackT_->Branch("caloMetPt", &caloPFMetPt_, "caloPFMetPt/F");
    muTrackT_->Branch("caloMetEta", &caloPFMetEta_, "caloPFMetEta/F");
    muTrackT_->Branch("caloMetPhi", &caloPFMetPhi_, "caloPFMetPhi/F");
    muTrackT_->Branch("recoil", &recoil_, "recoil/F");
    muTrackT_->Branch("metmufrac", &metmufrac_, "metmufrac/F");
    muTrackT_->Branch("metnomu", &metnomu_, "metnomu/F");
    muTrackT_->Branch("musum", &musum_, "musum/F");
    muTrackT_->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
    muTrackT_->Branch("recoPFMetEta", &recoPFMetEta_, "recoPFMetEta/F");
    muTrackT_->Branch("recoPFMetPhi", &recoPFMetPhi_, "recoPFMetPhi/F");
    muTrackT_->Branch("recoPFMETJetDeltaPhi", &recoPFMETJetDeltaPhi_ , "recoPFMETJetDeltaPhi/F");
    muTrackT_->Branch("recoPFrecoilJetDeltaPhi_", &recoPFrecoilJetDeltaPhi_ , "recoPFrecoilJetDeltaPhi_/F");
    muTrackT_->Branch("genwgt", &genwgt_, "genwgt/F");
    genweight_ = fs->make<TTree>("GenWeight", "");
    genweight_->Branch("genwgt", &genwgt_, "genwgt/F");
if(runGen){
    genTrackT_ = fs->make<TTree>("gen_MetTrigSelfEffiForMuTrack", "");
    genTrackT_->Branch("fired", &fired_, "fired/O");
    genTrackT_->Branch("fired0", &fired0_, "fired0/O");
    genTrackT_->Branch("genpt",   &genpt_);
    genTrackT_->Branch("geneta",  &geneta_);
    genTrackT_->Branch("genphi",  &genphi_);
    genTrackT_->Branch("genJetPt", &genJetPt_, "recoJetPt/F");
    genTrackT_->Branch("genJetEta", &genJetEta_, "recoJetEta/F");
    genTrackT_->Branch("genJetPhi", &genJetPhi_, "recoJetPhi/F");
    genTrackT_->Branch("genchargedHadronFraction", &genchargedHadronEnergyFraction_, "chargedHadronEnergyFraction/F");
    genTrackT_->Branch("genneutralHadronFraction", &genneutralHadronEnergyFraction_, "neutralHadronEnergyFraction/F");
    genTrackT_->Branch("genJetPt1", &genJetPt1_, "recoJetPt1/F");
    genTrackT_->Branch("genJetEta1", &genJetEta1_, "recoJetEta1/F");
    genTrackT_->Branch("genJetPhi1", &genJetPhi1_, "recoJetPhi1/F");
    genTrackT_->Branch("genJetPt2", &genJetPt2_, "recoJetPt2/F");
    genTrackT_->Branch("genJetEta2", &genJetEta2_, "recoJetEta2/F");
    genTrackT_->Branch("genJetPhi2", &genJetPhi2_, "recoJetPhi2/F");
    genTrackT_->Branch("genrecoil", &genrecoil_, "recoil/F");
    genTrackT_->Branch("genPFMetPt", &genPFMetPt_, "recoPFMetPt/F");
    genTrackT_->Branch("genPFMetEta", &genPFMetEta_, "recoPFMetEta/F");
    genTrackT_->Branch("genPFMetPhi", &genPFMetPhi_, "recoPFMetPhi/F");
    genTrackT_->Branch("genPFMETJetDeltaPhi", &genPFMETJetDeltaPhi_ , "recoPFMETJetDeltaPhi/F");
    genTrackT_->Branch("genPFrecoilJetDeltaPhi_", &genPFrecoilJetDeltaPhi_ , "recoPFrecoilJetDeltaPhi_/F");
    genTrackT_->Branch("genwgt", &genwgt_, "genwgt/F");
}
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

char error_msg[] = "iDMAnalyzer::GetCollections: Error in getting product %s from Event!";

    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogError("MetTrigSelfEffiForMuTrack")
            << "MetTrigSelfEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
            << endl;
        return;
    }
if(runGen){
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
}
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
 iEvent.getByToken(HBHENoiseFilterResultProducerToken_, HBHENoiseFilterResultProducerHandle_);
    if (!HBHENoiseFilterResultProducerHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "HBHENoiseFilter");
        return;
    }
    iEvent.getByToken(primaryVertexFilterToken_, primaryVertexFilterHandle_);
    if (!primaryVertexFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "primaryVertexFilter");
        return;
    }
    iEvent.getByToken(globalSuperTightHalo2016FilterToken_, globalSuperTightHalo2016FilterHandle_);
    if (!globalSuperTightHalo2016FilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "globalSuperTightHalo2016");
        return;
    }
    iEvent.getByToken(EcalDeadCellTriggerPrimitiveFilterToken_, EcalDeadCellTriggerPrimitiveFilterHandle_);
    if (!EcalDeadCellTriggerPrimitiveFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "EcalDeadCellTriggerPrimitive");
        return;
    }
    iEvent.getByToken(ecalBadCalibFilterToken_, ecalBadCalibFilterHandle_);
    if (!ecalBadCalibFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "ecalBadCalibFilter");
        return;
    }
    iEvent.getByToken(BadPFMuonFilterToken_, BadPFMuonFilterHandle_);
    if (!BadPFMuonFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "BadPFMuonFilter");
        return;
 }
    iEvent.getByToken(muonBadTrackFilterToken_, muonBadTrackFilterHandle_);
    if (!muonBadTrackFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "muonBadTrackFilter");
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


    float genpx = 0.0;
    float genpy = 0.0;
if(runGen){
    genpt_.clear(); genpt_.reserve(2);
    geneta_.clear(); geneta_.reserve(2);
    genphi_.clear(); genphi_.reserve(2);

    for (size_t i = 0; i < genParticleHandle_->size(); i++) {
        reco::GenParticleRef genP(genParticleHandle_, i);
        if (std::abs(genP->pdgId()) != 13) continue;
	if(abs(genP->eta())>2.4){continue;}
        genpt_.push_back(genP->pt());
        geneta_.push_back(genP->eta());
        genphi_.push_back(genP->phi());
        genpx += genP->px();
        genpy += genP->py();
    }
}
    float px = 0.0;
    float py = 0.0;
    for (const auto& recoMu : muRefs) {
	if(abs(recoMu->eta())>2.4){continue;}
        pt_ .push_back(recoMu->pt());
        eta_.push_back(recoMu->eta());
        phi_.push_back(recoMu->phi());
	px += recoMu->px();
	py += recoMu->py();
    }

    if (pt_.empty()) {return;}

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


//Met filter
 int METFiltersFailBits_ = 0;
    if (!(*HBHENoiseFilterResultProducerHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 0);
    if (!(*primaryVertexFilterHandle_)) // primaryVertexFilter == 0 means event is bad (number of vertices)
        METFiltersFailBits_ |= (1 << 1);
    if (!(*globalSuperTightHalo2016FilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 2);
    if (!(*EcalDeadCellTriggerPrimitiveFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 3);
    if (!(*ecalBadCalibFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 4);
    if (!(*BadPFMuonFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 5);
    if (!(*muonBadTrackFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 6);

    if( METFiltersFailBits_ != 0) {return;}



if(runGen){
    double genlargestPt = 0.0;
    double genlargestEta = 0.0, genlargestPhi = 0.0;
    double genlargestPt1 = 0.0;
    double genlargestEta1 = 0.0, genlargestPhi1 = 0.0;
    double genlargestPt2 = 0.0;
    double genlargestEta2 = 0.0, genlargestPhi2 = 0.0;
    genchargedHadronEnergyFraction_ = 0.0;
    genneutralHadronEnergyFraction_ = 0.0;
    for (size_t i(0); i != genJetHandle_->size(); ++i) {
        reco::GenJetRef genjetr(genJetHandle_, i);
	if (abs(genjetr->eta()) > 2.4){continue;}
        if (genjetr->pt() > genlargestPt) {
	    genlargestPt2 = genlargestPt1;
	    genlargestEta2 = genlargestEta1;
	    genlargestPhi2 = genlargestPhi1;
	    genlargestPt1 = genlargestPt;
	    genlargestEta1 = genlargestEta;
	    genlargestPhi1 = genlargestPhi;
            genlargestPt = genjetr->pt();
            genlargestEta = genjetr->eta();
            genlargestPhi = genjetr->phi();
//	    genchargedHadronEnergyFraction_ = genjetr->chargedHadronEnergyFraction();
//	    genneutralHadronEnergyFraction_ = genjetr->neutralHadronEnergyFraction();
        }
        else if (genjetr->pt() > genlargestPt1) {
	    genlargestPt2 = genlargestPt1;
	    genlargestEta2 = genlargestEta1;
	    genlargestPhi2 = genlargestPhi1;
            genlargestPt1 = genjetr->pt();
            genlargestEta1 = genjetr->eta();
            genlargestPhi1 = genjetr->phi();
        }
        else if (genjetr->pt() > genlargestPt2) {
            genlargestPt2 = genjetr->pt();
            genlargestEta2 = genjetr->eta();
            genlargestPhi2 = genjetr->phi();
        }
    }
    genJetPt_ = genlargestPt;
    genJetEta_ = genlargestEta;
    genJetPhi_ = genlargestPhi;
    genJetPt1_ = genlargestPt1;
    genJetEta1_ = genlargestEta1;
    genJetPhi1_ = genlargestPhi1;
    genJetPt2_ = genlargestPt2;
    genJetEta2_ = genlargestEta2;
    genJetPhi2_ = genlargestPhi2;
}
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
	if ((abs(jetr->eta()) > 2.4) || (jetr->chargedHadronEnergyFraction() <0.1)|| (jetr->neutralHadronEnergyFraction() >0.8)){continue;}
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
    reco::CaloMETRef metr3c(caloMetHandle_, 0);
    caloPFMetPt_ = metr3c->pt();
    caloPFMetEta_ = metr3c->eta();
    caloPFMetPhi_ = metr3c->phi();
    reco::PFMETRef metr3(recoMetHandle_, 0);
    recoPFMetPt_ = metr3->pt();
    recoPFMetEta_ = metr3->eta();
    recoPFMetPhi_ = metr3->phi();
   float metPx = metr3->px();
   float metPy = metr3->py();
   recoil_ = sqrt((metPx+px)*(metPx+px) + (metPy+py)*(metPy+py));
   musum_ = sqrt(px*px + py*py);
   metmufrac_ = metr3->MuonEtFraction();
   metnomu_ = metmufrac_*recoPFMetPt_;


    recoPFMETJetDeltaPhi_ = reco::deltaPhi(recoJetPhi_, recoPFMetPhi_);
    recoPFrecoilJetDeltaPhi_ = reco::deltaPhi(recoJetPhi_, atan2(metPy-py,metPx-px));
if(runGen){    
    reco::GenMETRef gmetr(genMetHandle_, 0);
    genPFMetPt_ = gmetr->pt();
    genPFMetPhi_ = gmetr->phi();
    genPFMetEta_ = gmetr->eta();
   float gmetPx = gmetr->px();
   float gmetPy = gmetr->py();
   genrecoil_ = sqrt((gmetPx+genpx)*(gmetPx+genpx) + (gmetPy+genpy)*(gmetPy+genpy));


    genPFMETJetDeltaPhi_ = reco::deltaPhi(genJetPhi_, genPFMetPhi_);
    genPFrecoilJetDeltaPhi_ = reco::deltaPhi(genJetPhi_, atan2(gmetPy-genpy,gmetPx-genpx));

}
    //save if Enriched criteria is met
    bool enrich = false;
    if( (recoJetPt_ >100) && (abs(recoJetEta_)<2.4) && (fired0_==true) && (pt_.at(0)>40) && (abs(eta_.at(0))<2.4)){
	enrich = true;
	}
    if (enrich){
    muTrackT_->Fill();
	}
    if (runGen){
    genTrackT_->Fill();
	}
    return;
}

void MetTrigSelfEffiForMuTrack::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void MetTrigSelfEffiForMuTrack::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(MetTrigSelfEffiForMuTrack);