#include <algorithm>
#include <cmath> 
#include <memory>
#include <random>
#include <vector>
#include <boost/format.hpp>
#include <boost/any.hpp>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/InvariantMass.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtra.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETFwd.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
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

#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

#include "TTree.h"

#include "NtupleContainer.hh"

class iDMAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources> {

public:
    explicit iDMAnalyzer(const edm::ParameterSet&);
    ~iDMAnalyzer();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
    bool getCollections(const edm::Event&);

    virtual void beginJob() override;
    virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void endJob() override;

    TTree *recoT, *genT;
    NtupleContainer nt;
    edm::Service<TFileService> fs;

    std::mt19937 m_random_generator; 

    edm::ESHandle<JetCorrectorParametersCollection> JetCorParCollHandle_;
    JetCorrectionUncertainty * jecUnc;

    bool isData;
    const std::string triggerProcessName_;

    // Tokens
    const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbToken_;
    const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbbToken_;
    const edm::EDGetTokenT<reco::MuonCollection> dsaRecoMuToken_;
    const edm::EDGetTokenT<reco::MuonTimeExtraMap> dsaRecoMuTimingToken_;
    const edm::EDGetTokenT<reco::MuonCollection> pfRecoMuToken_;
    const edm::EDGetTokenT<reco::TrackCollection> muTrackToken1_;
    const edm::EDGetTokenT<reco::TrackCollection> muTrackToken2_;
    const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
    const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
    const edm::EDGetTokenT<reco::GenMETCollection> genMETToken_;
    const edm::EDGetTokenT<reco::PFMETCollection> recoPFMETToken_;
    const edm::EDGetTokenT<reco::CaloMETCollection> recoCaloMETToken_;
    const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
    const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
    const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
    const edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupInfosToken_;
    const edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_;
    const edm::EDGetTokenT<bool>HBHENoiseFilterResultProducerToken_;
    const edm::EDGetTokenT<bool>HBHEIsoNoiseFilterResultProducerToken_;
    const edm::EDGetTokenT<int>primaryVertexFilterToken_;
    const edm::EDGetTokenT<reco::VertexCollection>primaryVertexToken_;
    const edm::EDGetTokenT<bool>globalSuperTightHalo2016FilterToken_;
    const edm::EDGetTokenT<bool>EcalDeadCellTriggerPrimitiveFilterToken_;
    const edm::EDGetTokenT<bool>ecalBadCalibFilterToken_;
    const edm::EDGetTokenT<bool>BadPFMuonFilterToken_;
    const edm::EDGetTokenT<bool>muonBadTrackFilterToken_;
    const edm::EDGetTokenT<reco::JetCorrector> jetCorrectorToken_;
    const edm::EDGetTokenT<reco::GsfElectronCollection> recoElectronToken_;
    const edm::EDGetTokenT<edm::ValueMap<float>> recoElectronIDToken_;
    const edm::EDGetTokenT<reco::PhotonCollection> recoPhotonToken_;
    const edm::EDGetTokenT<edm::ValueMap<bool>> recoPhotonIDToken_;
    const edm::EDGetTokenT<double> rhoToken_;

    // Handles
    edm::Handle<reco::JetTagCollection> bTagProbbHandle_;
    edm::Handle<reco::JetTagCollection> bTagProbbbHandle_;
    edm::Handle<reco::MuonCollection> dsaRecoMuHandle_;
    edm::Handle<reco::MuonTimeExtraMap> dsaRecoMuTimingHandle_;
    edm::Handle<reco::MuonCollection> pfRecoMuHandle_;
    edm::Handle<reco::TrackCollection> muTrackHandle1_;
    edm::Handle<reco::TrackCollection> muTrackHandle2_;
    edm::Handle<reco::VertexCollection> primaryVertexHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;
    edm::Handle<reco::GenJetCollection> genJetHandle_;
    edm::Handle<reco::GenMETCollection> genMETHandle_;
    edm::Handle<reco::PFMETCollection> recoPFMETHandle_;
    edm::Handle<reco::CaloMETCollection> recoCaloMETHandle_;
    edm::Handle<reco::PFJetCollection> recoJetHandle_;
    edm::Handle<edm::TriggerResults> trigResultsHandle_;
    edm::Handle<trigger::TriggerEvent> trigEventHandle_;
    edm::Handle<std::vector<PileupSummaryInfo>> pileupInfosHandle_;
    edm::Handle<GenEventInfoProduct> genEvtInfoHandle_;
    edm::Handle<bool> HBHENoiseFilterResultProducerHandle_;
    edm::Handle<bool> HBHEIsoNoiseFilterResultProducerHandle_;
    edm::Handle<int> primaryVertexFilterHandle_;
    edm::Handle<bool> globalSuperTightHalo2016FilterHandle_;
    edm::Handle<bool> EcalDeadCellTriggerPrimitiveFilterHandle_;
    edm::Handle<bool> ecalBadCalibFilterHandle_;
    edm::Handle<bool> BadPFMuonFilterHandle_;
    edm::Handle<bool> muonBadTrackFilterHandle_;
    edm::Handle<reco::JetCorrector> jetCorrectorHandle_;
    edm::Handle<reco::GsfElectronCollection> recoElectronHandle_;
    edm::Handle<edm::ValueMap<float>> recoElectronIDHandle_;
    edm::Handle<reco::PhotonCollection> recoPhotonHandle_;
    edm::Handle<edm::ValueMap<bool>> recoPhotonIDHandle_;
    edm::Handle<double> rhoHandle_;
    
    std::vector<std::string> triggerPathsWithoutVersionNum_;
    std::vector<std::string> triggerPathsWithVersionNum_;
    std::vector<bool> trigExist_;
    HLTConfigProvider hltConfig_;

};


iDMAnalyzer::iDMAnalyzer(const edm::ParameterSet& ps):
    isData(ps.getParameter<bool>("isData")),
    triggerProcessName_(ps.getParameter<std::string>("triggerProcessName")),
    
    bTagProbbToken_(consumes<reco::JetTagCollection>(ps.getParameter<edm::InputTag>("bTagProbb"))),
    bTagProbbbToken_(consumes<reco::JetTagCollection>(ps.getParameter<edm::InputTag>("bTagProbbb"))),
    dsaRecoMuToken_(consumes<reco::MuonCollection>(ps.getParameter<edm::InputTag>("dsaRecoMu"))),
    dsaRecoMuTimingToken_(consumes<reco::MuonTimeExtraMap>(ps.getParameter<edm::InputTag>("dsaRecoMuTiming"))),
    pfRecoMuToken_(consumes<reco::MuonCollection>(ps.getParameter<edm::InputTag>("pfRecoMu"))),
    muTrackToken1_(consumes<reco::TrackCollection>(ps.getParameter<edm::InputTag>("muTrack1"))),
    muTrackToken2_(consumes<reco::TrackCollection>(ps.getParameter<edm::InputTag>("muTrack2"))),
    genParticleToken_(consumes<reco::GenParticleCollection>(ps.getParameter<edm::InputTag>("genParticle"))),
    genJetToken_(consumes<reco::GenJetCollection>(ps.getParameter<edm::InputTag>("genJet"))),
    genMETToken_(consumes<reco::GenMETCollection>(ps.getParameter<edm::InputTag>("genMET"))),
    recoPFMETToken_(consumes<reco::PFMETCollection>(ps.getParameter<edm::InputTag>("PFMET"))),
    recoCaloMETToken_(consumes<reco::CaloMETCollection>(ps.getParameter<edm::InputTag>("caloMET"))),
    recoJetToken_(consumes<reco::PFJetCollection>(ps.getParameter<edm::InputTag>("recoJet"))),
    trigResultsToken_(consumes<edm::TriggerResults>(ps.getParameter<edm::InputTag>("trigResult"))),
    trigEventToken_(consumes<trigger::TriggerEvent>(ps.getParameter<edm::InputTag>("trigEvent"))),
    pileupInfosToken_(consumes<std::vector<PileupSummaryInfo>>(ps.getParameter<edm::InputTag>("pileups"))),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(ps.getParameter<edm::InputTag>("genEvt"))),
    HBHENoiseFilterResultProducerToken_(consumes<bool>(ps.getParameter<edm::InputTag>("HBHENoiseFilter"))),
    HBHEIsoNoiseFilterResultProducerToken_(consumes<bool>(ps.getParameter<edm::InputTag>("HBHEIsoNoiseFilter"))),
    primaryVertexFilterToken_(consumes<int>(ps.getParameter<edm::InputTag>("primaryVertexFilter"))),
    primaryVertexToken_(consumes<reco::VertexCollection>(ps.getParameter<edm::InputTag>("primaryVertex"))),
    globalSuperTightHalo2016FilterToken_(consumes<bool>(ps.getParameter<edm::InputTag>("globalSuperTightHaloFilter"))),
    EcalDeadCellTriggerPrimitiveFilterToken_(consumes<bool>(ps.getParameter<edm::InputTag>("EcalDeadCellTrgPrimitFilter"))),
    ecalBadCalibFilterToken_(consumes<bool>(ps.getParameter<edm::InputTag>("ecalBadCalibFilter"))),
    BadPFMuonFilterToken_(consumes<bool>(ps.getParameter<edm::InputTag>("BadPFMuonFilter"))),
    muonBadTrackFilterToken_(consumes<bool>(ps.getParameter<edm::InputTag>("muonBadTrackFilter"))),
    jetCorrectorToken_(consumes<reco::JetCorrector>(ps.getParameter<edm::InputTag>("jetCorrector"))),
    recoElectronToken_(consumes<reco::GsfElectronCollection>(ps.getParameter<edm::InputTag>("recoElectron"))),
    recoElectronIDToken_(consumes<edm::ValueMap<float>>(ps.getParameter<edm::InputTag>("electronID"))),
    recoPhotonToken_(consumes<reco::PhotonCollection>(ps.getParameter<edm::InputTag>("recoPhoton"))),
    recoPhotonIDToken_(consumes<edm::ValueMap<bool>>(ps.getParameter<edm::InputTag>("photonID"))),
    rhoToken_(consumes<double>(ps.getParameter<edm::InputTag>("rho")))
{
    usesResource("TFileService");
    m_random_generator = std::mt19937(37428479);
}

iDMAnalyzer::~iDMAnalyzer() = default;

void iDMAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    // Only specify tags with reasonable defaults -- check out cfg for others

    edm::ParameterSetDescription desc;
    desc.add<bool>("isData", 0);
    desc.add<std::string>("triggerProcessName", "HLT");

    desc.add<edm::InputTag>("bTagProbb", edm::InputTag("pfDeepCSVJetTags:probb")); 
    desc.add<edm::InputTag>("bTagProbbb", edm::InputTag("pfDeepCSVJetTags:probbb"));
    desc.add<edm::InputTag>("dsaRecoMu", edm::InputTag("muonsFromdSA"));
    desc.add<edm::InputTag>("dsaRecoMuTiming", edm::InputTag("muontimingFromdSA","combined"));
    desc.add<edm::InputTag>("pfRecoMu", edm::InputTag("muons"));
    desc.add<edm::InputTag>("muTrack1", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("muTrack2", edm::InputTag("globalMuons"));
    desc.add<edm::InputTag>("primaryVertexFilter", edm::InputTag("myPrimaryVertexFilter"));
    desc.add<edm::InputTag>("primaryVertex", edm::InputTag("offlinePrimaryVertices"));
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("genMET", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("PFMET", edm::InputTag("pfMetT0rtT1Txy"));
    desc.add<edm::InputTag>("caloMET", edm::InputTag("caloMet"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJetsCHS"));
    desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults", "", "HLT"));
    desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD", "", "HLT"));
    desc.add<edm::InputTag>("recoElectron", edm::InputTag("gedGsfElectrons"));
    desc.add<edm::InputTag>("electronID", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("recoPhoton", edm::InputTag("gedPhotons"));
    desc.add<edm::InputTag>("photonID", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("pileups", edm::InputTag("addPileupInfo"));
    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));
    desc.add<edm::InputTag>("HBHENoiseFilter", edm::InputTag("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"));
    desc.add<edm::InputTag>("HBHEIsoNoiseFilter", edm::InputTag("HBHENoiseFilterResultProducer","HBHEIsoNoiseFilterResult"));
    desc.add<edm::InputTag>("globalSuperTightHaloFilter", edm::InputTag("globalSuperTightHalo2016Filter"));
    desc.add<edm::InputTag>("EcalDeadCellTrgPrimitFilter", edm::InputTag("EcalDeadCellTriggerPrimitiveFilter"));
    desc.add<edm::InputTag>("ecalBadCalibFilter", edm::InputTag("ecalBadCalibFilter"));
    desc.add<edm::InputTag>("BadPFMuonFilter", edm::InputTag("BadPFMuonFilter"));
    desc.add<edm::InputTag>("muonBadTrackFilter", edm::InputTag("muonBadTrackFilter"));
    desc.add<edm::InputTag>("jetCorrector", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("rho", edm::InputTag("fixedGridRhoFastjetAll"));
    
    descriptions.add("iDMAnalyzer", desc);
}

void iDMAnalyzer::beginJob()
{
    recoT = fs->make<TTree>("recoT", "recoT");
    nt.SetRecoTree(recoT);
    if (!isData) {
        genT = fs->make<TTree>("genT", "genT");
        nt.SetGenTree(genT);
    }
    nt.CreateTreeBranches();
}


void iDMAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    using namespace edm;

    bool changed = true;
    if (hltConfig_.init(iRun, iSetup, triggerProcessName_, changed)) {
        if (changed) {
            LogInfo("HLTConfig") << "iDMAnalyzer::beginRun: " << "hltConfig init for Run" << iRun.run();
            hltConfig_.dump("ProcessName");
            hltConfig_.dump("GlobalTag");
            hltConfig_.dump("TableName");
        }
    } 
    else {
        LogError("HLTConfig") << "iDMAnalyzer::beginRun: config extraction failure with triggerProcessName -> " << triggerProcessName_;
        return;
    }

    // Add trigger paths if they exist
    triggerPathsWithoutVersionNum_.clear();
    triggerPathsWithVersionNum_.clear();
    trigExist_.clear();

    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET120_PFMHT120_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET130_PFMHT130_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET140_PFMHT140_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu130_PFMHTNoMu130_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu140_PFMHTNoMu140_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET120_PFMHT120_IDTight_PFHT60"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET200_HBHECleaned"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET200_HBHE_BeamHaloCleaned"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETTypeOne200_HBHE_BeamHaloCleaned"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFHT500_PFMET100_PFMHT100_IDTight"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFHT700_PFMET85_PFMHT85_IDTight"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFHT800_PFMET75_PFMHT75_IDTight"); // 2017+2018
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET170_HBHECleaned"); // 2016
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET300"); // 2016
    triggerPathsWithoutVersionNum_.emplace_back("HLT_MET200"); // 2016
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFHT300_PFMET110"); // 2016
    triggerPathsWithoutVersionNum_.emplace_back("HLT_IsoMu27"); // For MET trigger eff. studies in data
    triggerPathsWithoutVersionNum_.emplace_back("HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight"); // Alternative triggers
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DCA_PFMET50_PFMHT60"); // Alternative triggers
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET50_PFMHT60");  // Alternative triggers
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET70_PFMHT70");  // Alternative triggers
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET90_PFMHT90");  // Alternative triggers
    triggerPathsWithoutVersionNum_.emplace_back("HLT_L2Mu10_NoVertex_NoBPTX");    // For dSA eff. studies in data
    triggerPathsWithoutVersionNum_.emplace_back("HLT_L2Mu10_NoVertex_NoBPTX3BX"); // For dSA eff. studies in data
    
    const std::vector<std::string>& pathNames = hltConfig_.triggerNames();
    for (auto trigPathNoVersion : triggerPathsWithoutVersionNum_) {
        auto matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVersion));
        if (matchedPaths.size() == 0) {
            LogWarning("TriggerNotFound") << "Could not find matched full trigger path with --> " << trigPathNoVersion;
            triggerPathsWithVersionNum_.push_back("None");
            trigExist_.push_back(false);
        }
        else {
            trigExist_.push_back(true);
            triggerPathsWithVersionNum_.push_back(matchedPaths[0]);
            if (hltConfig_.triggerIndex(matchedPaths[0]) >= hltConfig_.size()) {
                LogError("TriggerError") << "Cannot find trigger path --> " << matchedPaths[0];
                return;
            }
        }
    }

    // JEC Uncertainty object
    iSetup.get<JetCorrectionsRecord>().get("AK4PFchs", JetCorParCollHandle_); 
    JetCorrectorParameters const & JetCorPar = (*JetCorParCollHandle_)["Uncertainty"];
    jecUnc = new JetCorrectionUncertainty(JetCorPar);
    if (!jecUnc) {
        LogError("JECUncertainty") << "iDMAnalyzer::beginRun: failed to get jecUnc object!";
    }
}


bool iDMAnalyzer::getCollections(const edm::Event& iEvent) {
    using namespace edm;

    char error_msg[] = "iDMAnalyzer::GetCollections: Error in getting product %s from Event!";

    bool ret = true;
    auto getHandle = [&]<typename T>(const EDGetTokenT<T> &token, Handle<T> &handle, std::string name) {
        iEvent.getByToken(token, handle);
        if (!handle.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % name);
            ret = false;
        }
    };

    getHandle(bTagProbbToken_, bTagProbbHandle_, "bTagProbb");
    getHandle(bTagProbbbToken_, bTagProbbbHandle_, "bTagProbbb");
    getHandle(dsaRecoMuToken_, dsaRecoMuHandle_, "dsaRecoMu");
    getHandle(dsaRecoMuTimingToken_, dsaRecoMuTimingHandle_, "dsaRecoMuTiming");
    getHandle(pfRecoMuToken_, pfRecoMuHandle_, "pfRecoMu");
    getHandle(muTrackToken1_, muTrackHandle1_, "muTrack1");
    getHandle(muTrackToken2_, muTrackHandle2_, "muTrack2");
    getHandle(primaryVertexToken_, primaryVertexHandle_, "primaryVertex");
    getHandle(recoPFMETToken_, recoPFMETHandle_, "PFMET");
    getHandle(recoCaloMETToken_, recoCaloMETHandle_, "CaloMET");
    getHandle(recoJetToken_, recoJetHandle_, "recoJet");
    getHandle(trigResultsToken_, trigResultsHandle_, "trigResults");
    getHandle(trigEventToken_, trigEventHandle_, "trigEvent");
    getHandle(HBHENoiseFilterResultProducerToken_, HBHENoiseFilterResultProducerHandle_, "HBHENoiseFilter");
    getHandle(HBHEIsoNoiseFilterResultProducerToken_, HBHEIsoNoiseFilterResultProducerHandle_, "HBHEIsoNoiseFilter");
    getHandle(primaryVertexFilterToken_, primaryVertexFilterHandle_, "primaryVertexFilter");
    getHandle(globalSuperTightHalo2016FilterToken_, globalSuperTightHalo2016FilterHandle_, "globalSuperTightHalo2016");
    getHandle(EcalDeadCellTriggerPrimitiveFilterToken_, EcalDeadCellTriggerPrimitiveFilterHandle_, "EcalDeadCellTriggerPrimitive");
    getHandle(ecalBadCalibFilterToken_, ecalBadCalibFilterHandle_, "ecalBadCalibFilter");
    getHandle(BadPFMuonFilterToken_, BadPFMuonFilterHandle_, "BadPFMuonFilter");
    getHandle(muonBadTrackFilterToken_, muonBadTrackFilterHandle_, "muonBadTrackFilter");
    getHandle(jetCorrectorToken_, jetCorrectorHandle_, "jetCorrector");
    getHandle(recoElectronToken_, recoElectronHandle_, "recoElectron");
    getHandle(recoElectronIDToken_, recoElectronIDHandle_, "recoElectronID");
    getHandle(recoPhotonToken_, recoPhotonHandle_, "recoPhoton");
    getHandle(recoPhotonIDToken_, recoPhotonIDHandle_, "recoPhotonID");
    getHandle(rhoToken_, rhoHandle_, "rho");
    if (!isData) {
        getHandle(genEvtInfoToken_, genEvtInfoHandle_, "genEventInfo");
        getHandle(genParticleToken_, genParticleHandle_, "genParticle");
        getHandle(genJetToken_, genJetHandle_, "genJet");
        getHandle(genMETToken_, genMETHandle_, "genMET");
        getHandle(pileupInfosToken_, pileupInfosHandle_, "pileupInfos");
    }
    
    return ret;
}


void iDMAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using std::cout, std::vector, std::endl;

    if (!getCollections(iEvent))
        return;

    // Clear branches before filling
    nt.ClearTreeBranches();

    // Start filling

    // Event information
    nt.eventNum_ = iEvent.id().event();
    nt.lumiSec_ = iEvent.luminosityBlock();
    nt.runNum_ = iEvent.id().run();
    nt.npv_ = *primaryVertexFilterHandle_;

    // Primary vertex
    reco::Vertex pv = *primaryVertexHandle_->begin();
    nt.pvx_ = pv.x();
    nt.pvy_ = pv.y();
    nt.pvz_ = pv.z();

    // MET filters
    if (!(*HBHENoiseFilterResultProducerHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 0);
    if (!(*HBHEIsoNoiseFilterResultProducerHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 1);
    if (!(*primaryVertexFilterHandle_)) // primaryVertexFilter == 0 means event is bad (number of vertices)
        nt.METFiltersFailBits_ |= (1 << 2);
    if (!(*globalSuperTightHalo2016FilterHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 3);
    if (!(*EcalDeadCellTriggerPrimitiveFilterHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 4);
    if (!(*ecalBadCalibFilterHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 5);
    if (!(*BadPFMuonFilterHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 6);
    if (!(*muonBadTrackFilterHandle_)) // false means event is bad
        nt.METFiltersFailBits_ |= (1 << 7);

    // get MET
    // assumes 0-th element of PFMET collection is largest pt (and only?) element
    // in other words, why is recoPFMETHandle_ even a vector?
    reco::PFMETRef PFMETr(recoPFMETHandle_, 0);
    nt.recoPFMETPt_ = PFMETr->pt();
    nt.recoPFMETPhi_ = PFMETr->phi();
    nt.recoPFMETMuonEtFraction_ = PFMETr->MuonEtFraction();
    reco::CaloMETRef CaloMETr(recoCaloMETHandle_, 0);
    nt.recoCaloMETPt_ = CaloMETr->pt();
    nt.recoCaloMETPhi_ = CaloMETr->phi();
    
    // calculate MHT
    math::XYZTLorentzVector MHT;
    for (auto & jet : *recoJetHandle_) {
        if (jet.pt() < 30) continue;
        MHT += jet.p4();
    }
    nt.MHTPt_ = MHT.pt();
    
    // As we go along, calculate recoil with electrons, photons, and GM that passed ID
    double recoil_px = PFMETr->px();
    double recoil_py = PFMETr->py();

    // Add all electrons to ntuple, regardless of ID
    // for now "good" electron means only ID is passed
    // i.e. (IDmap % 2 == 1)
    nt.recoNElectron_ = recoElectronHandle_->size();
    nt.recoNGoodElectron_ = 0;
    const edm::ValueMap<float> & eIDmap = *recoElectronIDHandle_;
    for (size_t i = 0; i < recoElectronHandle_->size(); i++) {
        reco::GsfElectronRef electronRef(recoElectronHandle_, i);
        if ((int)eIDmap[electronRef] % 2 == 1 && electronRef->pt() > 10 && abs(electronRef->eta() < 2.5)) {
            nt.recoNGoodElectron_++;
            recoil_px += electronRef->px();
            recoil_py += electronRef->py();
        }
        nt.recoElectronPt_.push_back(electronRef->pt());
        nt.recoElectronEta_.push_back(electronRef->eta());
        nt.recoElectronPhi_.push_back(electronRef->phi());
        nt.recoElectronVxy_.push_back(electronRef->trackPositionAtVtx().rho());
        nt.recoElectronVz_.push_back(electronRef->trackPositionAtVtx().z());
        nt.recoElectronCharge_.push_back(electronRef->charge());
        nt.recoElectronIDResult_.push_back((int)eIDmap[electronRef]);
    }

    // Also add all photons to ntuple, regardless of ID
    // Photon ID only produces 1 or 0
    nt.recoNPhoton_ = recoPhotonHandle_->size();
    nt.recoNGoodPhoton_ = 0;
    const edm::ValueMap<bool> & phIDmap = *recoPhotonIDHandle_;
    for (size_t i = 0; i < recoPhotonHandle_->size(); i++) {
        reco::PhotonRef photonRef(recoPhotonHandle_, i);
        if (phIDmap[photonRef] && photonRef->pt() > 15 && abs(photonRef->eta()) < 2.5) {
            nt.recoNGoodPhoton_++;
            recoil_px += photonRef->px();
            recoil_py += photonRef->py();
        }
        nt.recoPhotonPt_.push_back(photonRef->pt());
        nt.recoPhotonEta_.push_back(photonRef->eta());
        nt.recoPhotonPhi_.push_back(photonRef->phi());
        nt.recoPhotonIDResult_.push_back(phIDmap[photonRef]);
    }

    // Assign each trigger result to a different bit
    nt.fired_ = 0;
    for (size_t i = 0; i < triggerPathsWithVersionNum_.size(); i++) {
        if (trigExist_.at(i)) {
            std::string trigPath = triggerPathsWithVersionNum_[i];
            nt.fired_ |= (trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath)) << i);
        }
        else {
            nt.fired_ |= (0 <<i);
        }
    }
    
    vector<reco::TrackRef> muTracks1{};
    vector<reco::MuonRef> muObjs1{};
    for (size_t i = 0; i < dsaRecoMuHandle_->size(); i++) {
        reco::MuonRef muon_i(dsaRecoMuHandle_, i);
        reco::TrackRef track_i = muon_i->standAloneMuon();
        if (!track_i.isNonnull())
            cout << "Track " << i << " from muon reco is not valid! " << endl;
        else {
            muTracks1.emplace_back(track_i);
            muObjs1.emplace_back(muon_i);
        }
    }

    // Sort dSA muons (note that muon collection is *not* sorted by pT at first)
    nt.recoNDSA_ = muTrackHandle1_->size();

    sort(muObjs1.begin(), muObjs1.end(), [](const auto & l, const auto & r) {
            reco::TrackRef lt = l->standAloneMuon();
            reco::TrackRef rt = r->standAloneMuon();
            return lt->pt() > rt->pt();
            });

    sort(muTracks1.begin(), muTracks1.end(), [](const auto & l, const auto & r) {
            return l->pt() > r->pt();
            });

    // Sort global muons (note that muon collection is *not* sorted by pT at first)
    nt.recoNGM_ = muTrackHandle2_->size();

    vector<reco::TrackRef> muTracks2{};
    vector<reco::MuonRef> muObjs2{};
    for (size_t i = 0; i < pfRecoMuHandle_->size(); i++) {
        reco::MuonRef muon_i(pfRecoMuHandle_, i);
        reco::TrackRef track_i = muon_i->combinedMuon();
        if (track_i.isNonnull()) {
            muTracks2.emplace_back(track_i);
            muObjs2.emplace_back(muon_i);
        }
    }

    //vector<reco::TrackRef> muTracks2{};
    //for (size_t i = 0; i < muTrackHandle2_->size(); i++) 
    //    muTracks2.emplace_back(muTrackHandle2_, i);

    sort(muObjs2.begin(), muObjs2.end(), [](const auto & l, const auto & r) {
            reco::TrackRef lt = l->combinedMuon();
            reco::TrackRef rt = r->combinedMuon();
            return lt->pt() > rt->pt();
            });

    sort(muTracks2.begin(), muTracks2.end(), [](const auto & l, const auto & r) {
           return l->pt() > r->pt();
           });

    // Quick check if collection of global tracks inside Muon object is the same as global muon tracks collection
    //for (auto global_track : muTracks2_check) {
    //    bool match = 0;
    //    for (auto check : muTracks2) {
    //        if (global_track == check)
    //            match = 1;
    //    }
    //    if (!match)
    //        cout << "Global muon track in reco::Muon object not inside global track collection!" << endl;
    //    else
    //        cout << "Found a match for muon!" << endl;
    //}

    // Create separate collection for good quality dSA muons
    // DEPRECATED
    // Now store 4 leading muons in each collection for later processing in macros
    vector<int> muGoodTracksIdx{};
    for (size_t i = 0; i < muTracks1.size(); i++) {
        if (muTracks1[i]->hitPattern().muonStationsWithValidHits() < 2 ||
            muTracks1[i]->hitPattern().numberOfValidMuonHits() < 12 ||
            muTracks1[i]->normalizedChi2() > 10 ||
	       	muTracks1[i]->pt() < 5 ||
            abs(muTracks1[i]->eta()) > 2.4 ||
            muTracks1[i]->ptError()/muTracks1[i]->pt() > 1) {
                continue;
        }
        muGoodTracksIdx.push_back(i);
    }
    nt.recoNGoodDSA_ = muGoodTracksIdx.size();

    // Create separate collection for good quality global muons
    // TODO: this needs to change, GMs have tracker info as well
    // Cuts on muon chamber variables can be looser, while also
    // implementing cuts on tracker variables (the whole point
    // of having GMs as well)
    // DEPRECATED (only used for jet cross-cleaning)
    vector<int> muGoodTracksIdx2{};
    for (size_t i = 0; i < muTracks2.size(); i++) {
        if (muTracks2[i]->hitPattern().muonStationsWithValidHits() < 2 ||
            muTracks2[i]->hitPattern().numberOfValidMuonHits() < 12 ||
            muTracks2[i]->normalizedChi2() > 10 ||
	       	muTracks2[i]->pt() < 5 ||
            abs(muTracks2[i]->eta()) > 2.4 ||
            muTracks2[i]->ptError()/muTracks2[i]->pt() > 1) {
                continue;
        }
        muGoodTracksIdx2.push_back(i);
    }
    nt.recoNGoodGM_ = muGoodTracksIdx2.size();
    
    // Add leading 4 pt muons
    for (size_t i = 0; i < 4; i++) {
        if (i >= muTracks1.size()) break;
        reco::TrackRef mu_i = muTracks1[i];

        nt.recoDSAPt_.push_back(mu_i->pt());
        nt.recoDSAPtError_.push_back(mu_i->ptError());
        nt.recoDSAEta_.push_back(mu_i->eta());
        nt.recoDSAEtaError_.push_back(mu_i->etaError());
        nt.recoDSAPhi_.push_back(mu_i->phi());
        nt.recoDSAPhiError_.push_back(mu_i->phiError());
        nt.recoDSADxy_.push_back(mu_i->dxy(pv.position()));
        nt.recoDSADxyError_.push_back(mu_i->dxyError());
        nt.recoDSADz_.push_back(mu_i->dz(pv.position()));
        nt.recoDSADzError_.push_back(mu_i->dzError());
        nt.recoDSACharge_.push_back(mu_i->charge());
        nt.recoDSATrkChi2_.push_back(mu_i->normalizedChi2());
        nt.recoDSATrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        nt.recoDSATrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
        nt.recoDSATrkNumDTHits_.push_back(mu_i->hitPattern().numberOfValidMuonDTHits());
        nt.recoDSATrkNumCSCHits_.push_back(mu_i->hitPattern().numberOfValidMuonCSCHits());
        
        // add muon timing info from custom-built muon object
        reco::MuonRef muon_i = muObjs1[i];

        reco::MuonTimeExtra time_info = (*dsaRecoMuTimingHandle_)[muon_i];
        nt.recoDSAInvBeta_.push_back(time_info.inverseBeta());
        nt.recoDSAInvBetaErr_.push_back(time_info.inverseBeta());
        nt.recoDSAFreeInvBeta_.push_back(time_info.freeInverseBeta());
        nt.recoDSAFreeInvBetaErr_.push_back(time_info.freeInverseBetaErr());
        nt.recoDSAtimeAtIpInOut_.push_back(time_info.timeAtIpInOut());
        nt.recoDSAtimeAtIpInOutErr_.push_back(time_info.timeAtIpInOutErr());
        nt.recoDSAtimeAtIpOutIn_.push_back(time_info.timeAtIpOutIn());
        nt.recoDSAtimeAtIpOutInErr_.push_back(time_info.timeAtIpOutInErr());
        nt.recoDSAtimingNdof_.push_back(time_info.nDof());
    }
    // Add leading 4 pt muons
    for (size_t i = 0; i < 4; i++) {
        if (i >= muTracks2.size()) break;
        reco::TrackRef mu_i = muTracks2[i];

        nt.recoGMPt_.push_back(mu_i->pt());
        nt.recoGMPtError_.push_back(mu_i->ptError());
        nt.recoGMEta_.push_back(mu_i->eta());
        nt.recoGMEtaError_.push_back(mu_i->etaError());
        nt.recoGMPhi_.push_back(mu_i->phi());
        nt.recoGMPhiError_.push_back(mu_i->phiError());
        nt.recoGMDxy_.push_back(mu_i->dxy(pv.position()));
        nt.recoGMDxyError_.push_back(mu_i->dxyError());
        nt.recoGMDz_.push_back(mu_i->dz(pv.position()));
        nt.recoGMDzError_.push_back(mu_i->dzError());
        nt.recoGMCharge_.push_back(mu_i->charge());
        nt.recoGMTrkChi2_.push_back(mu_i->normalizedChi2());
        nt.recoGMTrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        nt.recoGMTrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
        nt.recoGMTrkNumDTHits_.push_back(mu_i->hitPattern().numberOfValidMuonDTHits());
        nt.recoGMTrkNumCSCHits_.push_back(mu_i->hitPattern().numberOfValidMuonCSCHits());

        reco::MuonRef muon_i = muObjs2[i];
        nt.recoGMIsPF_.push_back(muon_i->isPFMuon());
        nt.recoGMPFIso_.push_back((muon_i->pfIsolationR04().sumChargedHadronPt + std::max(0., 
                        muon_i->pfIsolationR04().sumNeutralHadronEt + muon_i->pfIsolationR04().sumPhotonEt
                        - 0.5*muon_i->pfIsolationR04().sumPUPt))/muon_i->pt());
        nt.recoGMTrkIso_.push_back(muon_i->isolationR03().sumPt/muon_i->pt());
        nt.recoGMTrkNumPixelHit_.push_back(muon_i->innerTrack()->hitPattern().numberOfValidPixelHits());
        nt.recoGMTrkNumTrkLayers_.push_back(muon_i->innerTrack()->hitPattern().trackerLayersWithMeasurement());

        recoil_px += mu_i->px();
        recoil_py += mu_i->py();
    }

    // Calculate recoil pt after looking at all collections
    nt.recoPFRecoilPt_ = sqrt(recoil_px*recoil_px + recoil_py*recoil_py);
    nt.recoPFRecoilPhi_ = atan2(recoil_py, recoil_px);

    // Apply Jet loose ID to jet collection, tag passes/fails on a side vector
    // Additionally mitigate HEM issue on chambers 15 and 16
    vector<bool> jetIDResults;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        reco::PFJetRef jet_i(recoJetHandle_, i);
        bool jetIDResult = true;

        if (    jet_i->neutralHadronEnergyFraction() > 0.99 || 
                jet_i->neutralEmEnergyFraction() > 0.99 ||
                jet_i->numberOfDaughters() <= 1)
            jetIDResult = false;
        
        if (abs(jet_i->eta()) < 2.4)
            if (    jet_i->chargedHadronEnergyFraction() <= 0 ||
                    jet_i->chargedEmEnergyFraction() > 0.99 ||
                    jet_i->chargedMultiplicity() <= 0)
                jetIDResult = false;

        // as per monojet, also check extra cuts for leading jet
        if (i == 0) 
            if (    jet_i->chargedHadronEnergyFraction() < 0.1 ||
                    jet_i->neutralHadronEnergyFraction() > 0.8)
                jetIDResult = false;

        jetIDResults.push_back(jetIDResult);

        // If passed jet is located in HEM region, veto the event
        // Has to happpen before jet ID, so don't check for jetIDResult
        double pt = jet_i->pt(), eta = jet_i->eta(), phi = jet_i->phi();
        if (pt > 30 && eta > -3.0 && eta < -1.4 && phi > -1.57 && phi < -0.87)
            nt.recoPFHEMFlag_ = true;
    }
    
    // Perform cross-cleaning in jet collection with good-quality GM muons
    // TODO: other option is that muon is coming out of jet, in that case
    // want a way to reject entire event since it's probably J/Psi
    // Nevermind: event selection ensures jets not collimated with MET+muons
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        // Skip failed ID jets
        if (!jetIDResults[i]) continue;
        reco::PFJetRef jet_i(recoJetHandle_, i);
        for (size_t j = 0; j < muGoodTracksIdx2.size(); ++j) {
            reco::TrackRef mu_j = muTracks2[muGoodTracksIdx2[j]];
            double dR = reco::deltaR(*jet_i, *mu_j);
            // if muon and jet match in dR fail the ID jet
            // because the jet is probably a muon instead
            if (dR < 0.4)
                jetIDResults[i] = false;
        }
    }
    // TODO: dSA muons dont contribute to PFJets so no need to cross-clean
    // However, possible muons from J/Psi in jets --> how to mitigate?
    // Nevermind: event selection ensures jets not collimated with MET+muons

    // Apply JEC+JER to jets that pass ID
    // Need to re-order jets by pT after this
    // vector key: index that will be changed after re-ordering
    // vector value1: corrected pT, value2: original key to refer back to
    
    JME::JetResolution resolution = JME::JetResolution::get(iSetup, "AK4PFchs_pt");
    JME::JetResolutionScaleFactor resolution_sf = JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");

    vector<std::pair<std::unique_ptr<reco::PFJet>, size_t>> correctedJets;
    vector<std::pair<std::unique_ptr<reco::PFJet>, size_t>> correctedJetsJESUp;
    vector<std::pair<std::unique_ptr<reco::PFJet>, size_t>> correctedJetsJESDown;
    vector<std::pair<std::unique_ptr<reco::PFJet>, size_t>> correctedJetsJERUp;
    vector<std::pair<std::unique_ptr<reco::PFJet>, size_t>> correctedJetsJERDown;

    // Initialize MET corrections due to JES/JER
    float corr_METpx = PFMETr->px(), corr_METpy = PFMETr->py();
    float corr_METpx_JESUp = PFMETr->px(), corr_METpy_JESUp = PFMETr->py();
    float corr_METpx_JESDown = PFMETr->px(), corr_METpy_JESDown = PFMETr->py();
    float corr_METpx_JERUp = PFMETr->px(), corr_METpy_JERUp = PFMETr->py();
    float corr_METpx_JERDown = PFMETr->px(), corr_METpy_JERDown = PFMETr->py();

    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        reco::PFJetRef jet_i(recoJetHandle_, i);

        // before JEC, check for EE noise and see if jet is in
        // critical region
        if (jet_i->pt() < 50 && abs(jet_i->eta()) > 2.65 && abs(jet_i->eta()) < 3.139) {
            nt.recoPFMETEEDeltaPx_ += jet_i->px();
            nt.recoPFMETEEDeltaPy_ += jet_i->py();
        }

        double jec = jetCorrectorHandle_->correction(*jet_i);
        std::unique_ptr<reco::PFJet> corr_jet_i(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jes_up(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jes_down(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jer_up(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jer_down(jet_i->clone());
        // For MET corrections due to jet smearing
        std::unique_ptr<reco::PFJet> corr_jet_i_jes_only(jet_i->clone());

        corr_jet_i->scaleEnergy(jec);
        corr_jet_i_jes_only->scaleEnergy(jec);
        corr_jet_i_jes_up->scaleEnergy(jec);
        corr_jet_i_jes_down->scaleEnergy(jec);
        corr_jet_i_jer_up->scaleEnergy(jec);
        corr_jet_i_jer_down->scaleEnergy(jec);

        jecUnc->setJetEta(corr_jet_i->eta());
        jecUnc->setJetPt(corr_jet_i->pt());
        double uncUp = jecUnc->getUncertainty(true); // true: Up direction
        jecUnc->setJetEta(corr_jet_i->eta());
        jecUnc->setJetPt(corr_jet_i->pt());
        double uncDown = jecUnc->getUncertainty(false); // false: Down direction

        double jesUp = 1 + uncUp;
        double jesDown = 1 - uncDown;

        corr_jet_i_jes_up->scaleEnergy(jesUp);
        corr_jet_i_jes_down->scaleEnergy(jesDown);

        // Only do jet smearing (JES) if not working on data!
        double smearFactor = 1., smearFactor_up = 1., smearFactor_down = 1.;
        if (!isData) {
        
            double jet_resolution = resolution.getResolution({{JME::Binning::JetPt, corr_jet_i->pt()}, {JME::Binning::JetEta, corr_jet_i->eta()}, {JME::Binning::Rho, *rhoHandle_}});
            double jer_sf = resolution_sf.getScaleFactor({{JME::Binning::JetPt, corr_jet_i->pt()}, {JME::Binning::JetEta, corr_jet_i->eta()}, {JME::Binning::Rho, *rhoHandle_}}, Variation::NOMINAL);
            double jer_sf_up = resolution_sf.getScaleFactor({{JME::Binning::JetPt, corr_jet_i->pt()}, {JME::Binning::JetEta, corr_jet_i->eta()}, {JME::Binning::Rho, *rhoHandle_}}, Variation::UP);
            double jer_sf_down = resolution_sf.getScaleFactor({{JME::Binning::JetPt, corr_jet_i->pt()}, {JME::Binning::JetEta, corr_jet_i->eta()}, {JME::Binning::Rho, *rhoHandle_}}, Variation::DOWN);

            // Try matching with gen jet
            double min_dR = std::numeric_limits<double>::infinity();
            const reco::GenJet* matched_genJet = nullptr;
            for (const auto& genJet: *genJetHandle_) {
                double dR = deltaR(genJet, *corr_jet_i);
                if (dR > min_dR)
                    continue;
                if (dR < 0.2) {
                    double dPt = std::abs(genJet.pt() - corr_jet_i->pt());
                    matched_genJet = &genJet;
                }
            }
            if (matched_genJet) {
                double dPt = corr_jet_i->pt() - matched_genJet->pt();
                smearFactor = 1 + (jer_sf - 1.) * dPt / corr_jet_i->pt();
                smearFactor_up = 1 + (jer_sf_up - 1.) * dPt / corr_jet_i->pt();
                smearFactor_down = 1 + (jer_sf_down - 1.) * dPt / corr_jet_i->pt();
            }
            if (!matched_genJet && jer_sf > 1) { 
                double sigma = jet_resolution * std::sqrt(jer_sf * jer_sf - 1);
                std::normal_distribution<> d(0, sigma);
                smearFactor = 1. + d(m_random_generator);
            }
            if (!matched_genJet && jer_sf_up > 1) {
                double sigma_up = jet_resolution * std::sqrt(jer_sf_up * jer_sf_up - 1);
                std::normal_distribution<> d_up(0, sigma_up);
                smearFactor_up = 1. + d_up(m_random_generator);
            }
            if (!matched_genJet && jer_sf_down > 1) {
                double sigma_down = jet_resolution * std::sqrt(jer_sf_down * jer_sf_down - 1);
                std::normal_distribution<> d_down(0, sigma_down);
                smearFactor_down = 1. + d_down(m_random_generator);
            }
            //else
            //   std::cout << "ERROR! Impossible to smear this jet. jer_sf: " << jer_sf << std::endl;

            if (corr_jet_i->energy() * smearFactor < 0.01) {
                double newSmearFactor = 0.01 / corr_jet_i->energy();
                //std::cout << "The smearing factor (" << smearFactor << ") is either negative or too small. Fixing it to " << newSmearFactor << " to avoid change of direction." << std::endl;
                smearFactor = newSmearFactor;
            }
            if (corr_jet_i->energy() * smearFactor_up < 0.01) {
                double newSmearFactor = 0.01 / corr_jet_i->energy();
                //std::cout << "The smearing factor (" << smearFactor << ") is either negative or too small. Fixing it to " << newSmearFactor << " to avoid change of direction." << std::endl;
                smearFactor_up = newSmearFactor;
            }
            if (corr_jet_i->energy() * smearFactor_down < 0.01) {
                double newSmearFactor = 0.01 / corr_jet_i->energy();
                //std::cout << "The smearing factor (" << smearFactor << ") is either negative or too small. Fixing it to " << newSmearFactor << " to avoid change of direction." << std::endl;
                smearFactor_down = newSmearFactor;
            }
        } // end if checking for isData

        corr_jet_i->scaleEnergy(smearFactor);
        corr_jet_i_jes_up->scaleEnergy(smearFactor);
        corr_jet_i_jes_down->scaleEnergy(smearFactor);
        corr_jet_i_jer_up->scaleEnergy(smearFactor_up);
        corr_jet_i_jer_down->scaleEnergy(smearFactor_down);
        //corr_jet_i_jes_only->scaleEnergy(smearFactor);

        // Before finishing, compute the MET correction due to JER alone (MET type-I already accounts for JEC)
        //corr_METpx -= (corr_jet_i_jer_only->px() - jet_i->px());
        //corr_METpy -= (corr_jet_i_jer_only->py() - jet_i->py());
        corr_METpx -= (corr_jet_i->px() - corr_jet_i_jes_only->px());
        corr_METpy -= (corr_jet_i->py() - corr_jet_i_jes_only->py());
        
        // OBSOLETE --> temporarily "un-apply" smear factor to get JEC-corrected-only jet that was used in the original type-I calculation of MET
        //corr_jet_i->scaleEnergy(1.0/smearFactor);
        
        corr_METpx_JESUp -= (corr_jet_i_jes_up->px() - corr_jet_i_jes_only->px());
        corr_METpy_JESUp -= (corr_jet_i_jes_up->py() - corr_jet_i_jes_only->py());
        corr_METpx_JESDown -= (corr_jet_i_jes_down->px() - corr_jet_i_jes_only->px());
        corr_METpy_JESDown -= (corr_jet_i_jes_down->py() - corr_jet_i_jes_only->py());
        corr_METpx_JERUp -= (corr_jet_i_jer_up->px() - corr_jet_i_jes_only->px());
        corr_METpy_JERUp -= (corr_jet_i_jer_up->py() - corr_jet_i_jes_only->py());
        corr_METpx_JERDown -= (corr_jet_i_jer_down->px() - corr_jet_i_jes_only->px());
        corr_METpy_JERDown -= (corr_jet_i_jer_down->py() - corr_jet_i_jes_only->py());
        
        //corr_jet_i->scaleEnergy(smearFactor);

        correctedJets.push_back(std::make_pair(std::move(corr_jet_i), i));
        correctedJetsJESUp.push_back(std::make_pair(std::move(corr_jet_i_jes_up), i));
        correctedJetsJESDown.push_back(std::make_pair(std::move(corr_jet_i_jes_down), i));
        correctedJetsJERUp.push_back(std::make_pair(std::move(corr_jet_i_jer_up), i));
        correctedJetsJERDown.push_back(std::make_pair(std::move(corr_jet_i_jer_down), i));
    }

    // Before sorting the jet collections by pT, calculate MET corrections for each as well
    nt.recoPFMETSmearingOnlyPt_ = std::sqrt(corr_METpx*corr_METpx + corr_METpy*corr_METpy);
    nt.recoPFMETSmearingOnlyPhi_ = atan2(corr_METpy, corr_METpx);
    nt.recoPFMETCorrectedPt_ = nt.recoPFMETSmearingOnlyPt_;
    nt.recoPFMETCorrectedPhi_ = nt.recoPFMETSmearingOnlyPhi_;
    nt.recoPFMETJESUpPt_ = std::sqrt(corr_METpx_JESUp*corr_METpx_JESUp + corr_METpy_JESUp*corr_METpy_JESUp);
    nt.recoPFMETJESUpPhi_ = atan2(corr_METpy_JESUp, corr_METpx_JESUp);
    nt.recoPFMETJESDownPt_ = std::sqrt(corr_METpx_JESDown*corr_METpx_JESDown + corr_METpy_JESDown*corr_METpy_JESDown);
    nt.recoPFMETJESDownPhi_ = atan2(corr_METpy_JESDown, corr_METpx_JESDown);
    nt.recoPFMETJERUpPt_ = std::sqrt(corr_METpx_JERUp*corr_METpx_JERUp + corr_METpy_JERUp*corr_METpy_JERUp);
    nt.recoPFMETJERUpPhi_ = atan2(corr_METpy_JERUp, corr_METpx_JERUp);
    nt.recoPFMETJERDownPt_ = std::sqrt(corr_METpx_JERDown*corr_METpx_JERDown + corr_METpy_JERDown*corr_METpy_JERDown);
    nt.recoPFMETJERDownPhi_ = atan2(corr_METpy_JERDown, corr_METpx_JERDown);

    auto reverseSortJets = [](const std::pair<std::unique_ptr<reco::PFJet>, size_t> &a, const std::pair<std::unique_ptr<reco::PFJet>, size_t> &b) {
        return (a.first->pt() > b.first->pt());
    };
    sort(correctedJets.begin(), correctedJets.end(), reverseSortJets);
    sort(correctedJetsJESUp.begin(), correctedJetsJESUp.end(), reverseSortJets);
    sort(correctedJetsJESDown.begin(), correctedJetsJESDown.end(), reverseSortJets);
    sort(correctedJetsJERUp.begin(), correctedJetsJERUp.end(), reverseSortJets);
    sort(correctedJetsJERDown.begin(), correctedJetsJERDown.end(), reverseSortJets);

    // Get 10 top leading jets info, sorted by corrected pT
    // Only pick jets that have passed loose ID and cross-cleaning
    nt.recoPFNJet_ = recoJetHandle_->size(); 
    nt.recoPFNPassIDJet_ = 0;
    nt.recoPFNHighPtJet_ = 0;
    
    for (size_t i = 0; i < correctedJets.size(); i++) {
        size_t index = correctedJets[i].second;
        reco::PFJet & jet_i = *(correctedJets[i].first);

        // Exclude jets that didn't pass ID above (ID checked against uncorrected jets)
        if (!jetIDResults[index]) continue;
        nt.recoPFNPassIDJet_++;

        // Use JES+JER-corrected quantities
        if (jet_i.pt() > 30) 
            nt.recoPFNHighPtJet_++;

        if (nt.recoPFJetCorrectedPt_.size() < 10 && jet_i.pt() > 30) {
            nt.recoPFJetCorrectedPt_.push_back(jet_i.pt());
            nt.recoPFJetCorrectedEta_.push_back(jet_i.eta());
            nt.recoPFJetCorrectedPhi_.push_back(jet_i.phi());
            // TODO: figure out how BtagProbb(b) collections actually behave
            // FIXME this might be problematic with the jet corrections, keep in mind
            if (bTagProbbHandle_->size() > i && bTagProbbbHandle_->size() > i)
                nt.recoPFJetCorrectedBTag_.push_back((*bTagProbbHandle_)[index].second + (*bTagProbbbHandle_)[index].second);
            else 
                nt.recoPFJetCorrectedBTag_.push_back(-9999);
            nt.recoPFJetCorrectedCHEF_.push_back(jet_i.chargedHadronEnergyFraction());
            nt.recoPFJetCorrectedNHEF_.push_back(jet_i.neutralHadronEnergyFraction());
            nt.recoPFJetCorrectedCEEF_.push_back(jet_i.chargedEmEnergyFraction());
            nt.recoPFJetCorrectedNEEF_.push_back(jet_i.neutralEmEnergyFraction());
            nt.recoPFJetCorrectedNumDaughters_.push_back(jet_i.numberOfDaughters());
            nt.recoPFJetCorrectedChargedMultiplicity_.push_back(jet_i.chargedMultiplicity());
        }
        // Add pt, eta, phi information for syst. uncert. collections
        jet_i = *(correctedJetsJESUp[i].first);
        if (nt.recoPFJetCorrectedJESUpPt_.size() < 10 && jet_i.pt() > 30) {
            nt.recoPFJetCorrectedJESUpPt_.push_back(jet_i.pt());
            nt.recoPFJetCorrectedJESUpEta_.push_back(jet_i.eta());
            nt.recoPFJetCorrectedJESUpPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJESDown[i].first);
        if (nt.recoPFJetCorrectedJESDownPt_.size() < 10 && jet_i.pt() > 30) {
            nt.recoPFJetCorrectedJESDownPt_.push_back(jet_i.pt());
            nt.recoPFJetCorrectedJESDownEta_.push_back(jet_i.eta());
            nt.recoPFJetCorrectedJESDownPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJERUp[i].first);
        if (nt.recoPFJetCorrectedJERUpPt_.size() < 10 && jet_i.pt() > 30) {
            nt.recoPFJetCorrectedJERUpPt_.push_back(jet_i.pt());
            nt.recoPFJetCorrectedJERUpEta_.push_back(jet_i.eta());
            nt.recoPFJetCorrectedJERUpPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJERDown[i].first);
        if (nt.recoPFJetCorrectedJERDownPt_.size() < 10 && jet_i.pt() > 30) {
            nt.recoPFJetCorrectedJERDownPt_.push_back(jet_i.pt());
            nt.recoPFJetCorrectedJERDownEta_.push_back(jet_i.eta());
            nt.recoPFJetCorrectedJERDownPhi_.push_back(jet_i.phi());
        }
        // Also add uncorrected jet info for completeness
        reco::PFJetRef jet_ii(recoJetHandle_, i);
        if (nt.recoPFJetPt_.size() < 10 && jet_ii->pt() > 30) {
            nt.recoPFJetPt_.push_back(jet_ii->pt());
            nt.recoPFJetEta_.push_back(jet_ii->eta());
            nt.recoPFJetPhi_.push_back(jet_ii->phi());
        }
    }

    // Pick pair of muons with smallest vertex chi square fit for all collection combos
    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
    KalmanVertexFitter kvf(true);

    auto computeVertices = [&](vector<reco::TrackRef> coll_1, vector<reco::TrackRef> coll_2, std::string type) {
        for (size_t i = 0; i < 4; i++) {
            for (size_t j = 0; j < 4; j++) {
                reco::TrackRef muon_i, muon_j;
                if (i < coll_1.size())
                    muon_i = coll_1[i];
                if (j < coll_2.size())
                    muon_j = coll_2[j];

                TransientVertex tv;
                if (muon_i.isNonnull() && muon_j.isNonnull() && i != j) {
                    vector<reco::TransientTrack> transient_tracks{};
                    transient_tracks.push_back(theB->build(muon_i));
                    transient_tracks.push_back(theB->build(muon_j));
                    tv = kvf.vertex(transient_tracks);
                }

                float vxy = -9999;
                float sigma_vxy = -9999;
                float vtx_chi2 = 999999;
                float vz = -9999;
                float dr = -9999;

                if (tv.isValid()) {
                    reco::Vertex vertex = reco::Vertex(tv);
                    vxy = sqrt(vertex.x()*vertex.x() + vertex.y()*vertex.y());
                    sigma_vxy = (1/vxy)*sqrt(vertex.x()*vertex.x()*vertex.xError()*vertex.xError() +
                            vertex.y()*vertex.y()*vertex.yError()*vertex.yError());
                    //sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
                    vtx_chi2 = vertex.normalizedChi2();
                    vz = vertex.z();
                    dr = reco::deltaR(*muon_i, *muon_j);
                }

                if (type == "dsadsa") {
                    nt.dsadsa_recoVtxReducedChi2_.push_back(vtx_chi2);
                    nt.dsadsa_recoVtxVxy_.push_back(vxy);
                    nt.dsadsa_recoVtxSigmaVxy_.push_back(sigma_vxy);
                    nt.dsadsa_recoVtxVz_.push_back(vz);
                    nt.dsadsa_recoVtxDr_.push_back(dr);
                }
                else if (type == "gmgm") {
                    nt.gmgm_recoVtxReducedChi2_.push_back(vtx_chi2);
                    nt.gmgm_recoVtxVxy_.push_back(vxy);
                    nt.gmgm_recoVtxSigmaVxy_.push_back(sigma_vxy);
                    nt.gmgm_recoVtxVz_.push_back(vz);
                    nt.gmgm_recoVtxDr_.push_back(dr);
                }
                else if (type == "dsagm") {
                    nt.dsagm_recoVtxReducedChi2_.push_back(vtx_chi2);
                    nt.dsagm_recoVtxVxy_.push_back(vxy);
                    nt.dsagm_recoVtxSigmaVxy_.push_back(sigma_vxy);
                    nt.dsagm_recoVtxVz_.push_back(vz);
                    nt.dsagm_recoVtxDr_.push_back(dr);
                }
            }
        }
    };
    // dSA-dSA
    computeVertices(muTracks1, muTracks1, "dsadsa");
    // GM-GM
    computeVertices(muTracks2, muTracks2, "gmgm");
    // dSA-GM
    computeVertices(muTracks1, muTracks2, "dsagm");

    // Do muon matching in dR (0.1) between dSA and GM collections
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            reco::TrackRef dsa_i, gm_j;

            if (i < muTracks1.size())
                dsa_i = muTracks1[i];
            if (j < muTracks2.size())
                gm_j = muTracks2[j];

            float dr = -9999;
            if (dsa_i.isNonnull() && gm_j.isNonnull())
                dr = reco::deltaR(dsa_i->outerEta(), dsa_i->outerPhi(),
                        gm_j->outerEta(), gm_j->outerPhi());

            nt.recoGMdSAdR_.push_back(dr);
            if (dr < -9998)
                nt.recoGMdSAmatch_.push_back(-1);
            else if (dr < 0.1)
                nt.recoGMdSAmatch_.push_back(1);
            else
                nt.recoGMdSAmatch_.push_back(0);
        }
    }

    nt.recoPFMETCorrectedPt_ = std::sqrt(corr_METpx*corr_METpx + corr_METpy*corr_METpy);
    nt.recoPFMETCorrectedPhi_ = atan2(corr_METpy, corr_METpx);
    nt.recoPFMETJESUpPt_ = std::sqrt(corr_METpx_JESUp*corr_METpx_JESUp + corr_METpy_JESUp*corr_METpy_JESUp);
    nt.recoPFMETJESUpPhi_ = atan2(corr_METpy_JESUp, corr_METpx_JESUp);
    nt.recoPFMETJESDownPt_ = std::sqrt(corr_METpx_JESDown*corr_METpx_JESDown + corr_METpy_JESDown*corr_METpy_JESDown);
    nt.recoPFMETJESDownPhi_ = atan2(corr_METpy_JESDown, corr_METpx_JESDown);
    nt.recoPFMETJERUpPt_ = std::sqrt(corr_METpx_JERUp*corr_METpx_JERUp + corr_METpy_JERUp*corr_METpy_JERUp);
    nt.recoPFMETJERUpPhi_ = atan2(corr_METpy_JERUp, corr_METpx_JERUp);
    nt.recoPFMETJERDownPt_ = std::sqrt(corr_METpx_JERDown*corr_METpx_JERDown + corr_METpy_JERDown*corr_METpy_JERDown);
    nt.recoPFMETJERDownPhi_ = atan2(corr_METpy_JERDown, corr_METpx_JERDown);

    //recoNMatchedGBMvDSA_ = -1;
    //int nDoubleMatched = 0;
    //if (fFoundValidVertex) {
    //    recoNMatchedGBMvDSA_ = 0;
    //    for (size_t i = 0; i < muGoodTracksIdx.size(); i++) {
    //        if ((int)i != dSA1Idx && (int)i != dSA2Idx) continue;
    //        bool alreadyMatched = false;
    //        auto & track1 = muTracks[muGoodTracksIdx[i]];
    //        const reco::HitPattern &p1 = track1->hitPattern();
    //        //std::cout << "p1 number of valid muon hits: " << p1.numberOfValidMuonHits() << std::endl;
    //        //std::cout << "p1 number of stations: " << p1.muonStationsWithValidHits() << std::endl;
    //        //std::cout << "p1 pattern: "; p1.print(reco::HitPattern::TRACK_HITS);
    //        std::vector<int> stations1;
    //        // loop over the hits of track1 
    //        for (int k = 0; k < p1.numberOfAllHits(reco::HitPattern::TRACK_HITS); k++) {
    //            uint32_t hit1 = p1.getHitPattern(reco::HitPattern::TRACK_HITS, k);
    //            if (!p1.validHitFilter(hit1)) continue;
    //            stations1.push_back(hit1);
    //        }
    //        stations1.erase(std::unique(stations1.begin(), stations1.end()), stations1.end());
    //        std::sort(stations1.begin(), stations1.end());
    //        for (size_t j = 0; j < muGoodTracksIdx2.size(); j++) {
    //            size_t nCommonChambers = 0;
    //            auto & track2 = muTracks2[muGoodTracksIdx2[j]];
    //            const reco::HitPattern &p2 = track2->hitPattern();
    //            //std::cout << "p2 number of valid muon hits: " << p2.numberOfValidMuonHits() << std::endl;
    //            //std::cout << "p2 number of stations: " << p2.muonStationsWithValidHits() << std::endl;
    //            //std::cout << "p2 pattern: "; p2.print(reco::HitPattern::TRACK_HITS);
    //            std::vector<int> stations2;
    //            // loop over the hits of track2
    //            for (int l = 0; l < p2.numberOfAllHits(reco::HitPattern::TRACK_HITS); l++) {
    //                uint32_t hit2 = p2.getHitPattern(reco::HitPattern::TRACK_HITS, l);
    //                if (!p2.validHitFilter(hit2)) continue;
    //                stations2.push_back(hit2);
    //            }
    //            stations2.erase(std::unique(stations2.begin(), stations2.end()), stations2.end());
    //            std::sort(stations2.begin(), stations2.end());
    //            std::vector<int> intersect;
    //            std::set_intersection(stations1.begin(),stations1.end(),stations2.begin(),stations2.end(), std::back_inserter(intersect));
    //            nCommonChambers = intersect.size();
    //            nCommonChambers_.push_back(min(stations1.size(),stations2.size()) - nCommonChambers);
    //            //std::cout << "nCommonChambers: " << nCommonChambers << std::endl;
    //            int distanceMetric = -1;
    //            // number of chambers not equal
    //            if (std::abs((int)stations1.size() - (int)stations2.size()) >= 1) {
    //                // if all chambers of smaller set match, not ok
    //                // (to make it ok, change distanceMetric back to 1 here)
    //                if (nCommonChambers == min(stations1.size(), stations2.size())) 
    //                    distanceMetric = 1;
    //                // one chamber of smaller set deosnt match, ok/not ok:
    //                else if (nCommonChambers == min(stations1.size(),stations2.size())-1)
    //                    distanceMetric = 1;
    //                // at least two chambers of smaller set doesnt match, not ok
    //                else 
    //                    distanceMetric = 2;
    //            }
    //            // number of chambers equal
    //            else {
    //                // at most one chamber different, ok
    //                if ((stations1.size() - nCommonChambers) <= 1)
    //                    distanceMetric = 1;
    //                // more than one chamber different, not ok
    //                else 
    //                    distanceMetric = 2;
    //            }
    //            if (distanceMetric == 1) {
    //            //if (nCommonStations >=  min(p1.muonStationsWithValidHits(), p2.muonStationsWithValidHits())) {
    //                recoNMatchedGBMvDSA_++;
    //                if (alreadyMatched)
    //                    nDoubleMatched++;
    //                else
    //                    alreadyMatched = true;
    //            }
    //        }
    //    }
    //}
    //std::cout << "[AF] Number of matched muons: " << recoNMatchedGBMvDSA_ << std::endl;
    //std::cout << "[AF] Number of double-matchings: " << nDoubleMatched << std::endl;
    

    /****** GEN INFO *******/

    if (!isData) {

        nt.nGen_ = (int)genParticleHandle_->size();
        
        // Gen weight
        nt.genwgt_ = genEvtInfoHandle_->weight();

        // Pile-up
        for (const auto & pileupInfo : *pileupInfosHandle_) {
            if (pileupInfo.getBunchCrossing() == 0) {
                nt.genpuobs_ = pileupInfo.getPU_NumInteractions();
                nt.genputrue_ = pileupInfo.getTrueNumInteractions();
                break;
            }
        }

        for (size_t i = 0; i < genParticleHandle_->size(); i++) {
            reco::GenParticleRef genParticle(genParticleHandle_, i);
            if (!genParticle->isHardProcess()) continue;
            nt.genID_.push_back(genParticle->pdgId());
            //genHardProcess_.push_back(genParticle->isHardProcess());
            nt.genCharge_.push_back(genParticle->charge());
            nt.genPt_.push_back(genParticle->pt());
            nt.genEta_.push_back(genParticle->eta());
            nt.genPhi_.push_back(genParticle->phi());
            nt.genPz_.push_back(genParticle->pz());
            nt.genEn_.push_back(genParticle->energy());
            nt.genVxy_.push_back(genParticle->vertex().rho());
            nt.genVz_.push_back(genParticle->vz());
            nt.genMass_.push_back(genParticle->mass());
        }

        // all gen jets
        for (size_t i = 0; i < genJetHandle_->size(); i++) {
            reco::GenJetRef jetRef(genJetHandle_, i);
            nt.genJetPt_.push_back(jetRef->pt());
            nt.genJetEta_.push_back(jetRef->eta());
            nt.genJetPhi_.push_back(jetRef->phi());
        }

        // Lead gen MET
        if (genMETHandle_->size() > 0) {
            reco::GenMETRef metRef(genMETHandle_, 0);
            nt.genLeadMETPt_ = metRef->pt();
            nt.genLeadMETPhi_ = metRef->phi();
        }

        genT->Fill();
    }

    recoT->Fill();

    return;
}

void iDMAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void iDMAnalyzer::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(iDMAnalyzer);