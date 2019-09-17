#include "iDMSkimmer/washAOD/interface/iDMAnalyzer.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "CommonTools/Utils/interface/InvariantMass.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include <cmath>
#include <algorithm>
#include <vector>

iDMAnalyzer::iDMAnalyzer(const edm::ParameterSet& ps):
    isData(ps.getUntrackedParameter<bool>("isData", false)),

    bTagProbbTag_(ps.getParameter<edm::InputTag>("bTagProbb")),
    bTagProbbbTag_(ps.getParameter<edm::InputTag>("bTagProbbb")),
    muTrackTag1_(ps.getParameter<edm::InputTag>("muTrack1")),
    muTrackTag2_(ps.getParameter<edm::InputTag>("muTrack2")),
    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
    genMetTag_(ps.getParameter<edm::InputTag>("genMet")),
    recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
    trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
    trigPathNoVer_(ps.getParameter<std::string>("trigPath")),
    pileupInfosTag_(ps.getParameter<edm::InputTag>("pileups")),
    genEvtInfoTag_(ps.getParameter<edm::InputTag>("genEvt")),
    processName_(ps.getParameter<std::string>("processName")),
    HBHENoiseFilterResultProducerTag_("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"),
    primaryVertexFilterTag_("myPrimaryVertexFilter"),
    globalSuperTightHalo2016FilterTag_("globalSuperTightHalo2016Filter"),
    EcalDeadCellTriggerPrimitiveFilterTag_("EcalDeadCellTriggerPrimitiveFilter"),
    ecalBadCalibFilterTag_("ecalBadCalibFilter"),
    BadPFMuonFilterTag_("BadPFMuonFilter"),
    muonBadTrackFilterTag_("muonBadTrackFilter"),
    
    bTagProbbToken_(consumes<reco::JetTagCollection>(bTagProbbTag_)),
    bTagProbbbToken_(consumes<reco::JetTagCollection>(bTagProbbbTag_)),
    muTrackToken1_(consumes<reco::TrackCollection>(muTrackTag1_)),
    muTrackToken2_(consumes<reco::TrackCollection>(muTrackTag2_)),
    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
    pileupInfosToken_(consumes<std::vector<PileupSummaryInfo>>(pileupInfosTag_)),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_)),
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

iDMAnalyzer::~iDMAnalyzer() = default;

void iDMAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.addUntracked<bool>("isData", 0);
    desc.add<edm::InputTag>("bTagProbb", edm::InputTag("pfDeepCSVJetTags", "probb", "RECO"));
    desc.add<edm::InputTag>("bTagProbbb", edm::InputTag("pfDeepCSVJetTags", "probbb", "RECO"));
    desc.add<edm::InputTag>("muTrack1", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("muTrack2", edm::InputTag("globalMuons"));
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("recoMet", edm::InputTag("pfMet"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
    desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults", "", "HLT"));
    desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD", "", "HLT"));
    desc.add<std::string>("trigPath", "Defaultshouldntbecalled");
    desc.add<edm::InputTag>("pileups", edm::InputTag("addPileupInfo"));
    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));
    desc.add<std::string>("processName", "HLT");
    descriptions.add("iDMAnalyzer", desc);
}

void iDMAnalyzer::beginJob()
{
    recoT = fs->make<TTree>("recoT", "");

    recoT->Branch("event_num", &eventNum_);
    recoT->Branch("lumi_sec", &lumiSec_);
    recoT->Branch("run_num", &runNum_);
    recoT->Branch("npv", &npv_);
    recoT->Branch("MET_filters_fail_bits", &METFiltersFailBits_);
    recoT->Branch("trig_fired", &fired_);
    recoT->Branch("reco_n_dsa", &recoNMu_);
    recoT->Branch("reco_n_good_dsa", &recoNGoodDSAMu_);
    recoT->Branch("reco_dsa_pt",  &recoPt_);
    recoT->Branch("reco_dsa_eta", &recoEta_);
    recoT->Branch("reco_dsa_phi", &recoPhi_);
    recoT->Branch("reco_dsa_dxy", &recoDxy_);
    recoT->Branch("reco_dsa_dz",  &recoDz_);
    recoT->Branch("reco_dsa_charge", &recoCharge_);
    recoT->Branch("reco_dsa_trk_chi2", &recoTrkChi2_);
    recoT->Branch("reco_dsa_trk_n_planes", &recoTrkNumPlanes_);
    recoT->Branch("reco_dsa_trk_n_hits", &recoTrkNumHits_);
    recoT->Branch("reco_dsa_idx0", &recoDSAIdx0_);
    recoT->Branch("reco_dsa_idx1", &recoDSAIdx1_);
    recoT->Branch("reco_n_gbmdsa_match", &recoNMatchedGBMvDSA_);
    recoT->Branch("reco_gbmdsa_dR", &recoGBMDSADr_);
    recoT->Branch("reco_sel_mu_pt", &selectedMuonsPt_);
    recoT->Branch("reco_sel_mu_eta", &selectedMuonsEta_);
    recoT->Branch("reco_sel_mu_phi", &selectedMuonsPhi_);
    recoT->Branch("reco_sel_mu_dxy", &selectedMuonsDxy_);
    recoT->Branch("reco_sel_mu_dz", &selectedMuonsDz_);
    recoT->Branch("reco_sel_mu_charge", &selectedMuonsCharge_);
    recoT->Branch("reco_Mmumu",  &recoMmumu_);
    recoT->Branch("reco_METmu_dphi", &recoDeltaPhiMetMu_);
    recoT->Branch("reco_vtx_vxy", &recoVtxVxy_);
    recoT->Branch("reco_vtx_vz",  &recoVtxVz_);
    recoT->Branch("reco_vtx_sigmavxy", &recoVtxSigmaVxy_);
    recoT->Branch("reco_vtx_reduced_chi2", &recoVtxReducedChi2_);
    recoT->Branch("reco_vtx_dR",  &recoVtxDr_);
    recoT->Branch("reco_PF_MET_pt", &recoPFMetPt_);
    recoT->Branch("reco_PF_MET_phi", &recoPFMetPhi_);
    recoT->Branch("reco_PF_n_jets", &recoPFNJet_);
    recoT->Branch("reco_PF_n_passID_jets", &recoPFNPassIDJet_);
    recoT->Branch("reco_PF_n_highPt_jets", &recoPFNHighPtJet_);
    recoT->Branch("reco_PF_jet_pt", &recoPFJetPt_);
    recoT->Branch("reco_PF_jet_eta", &recoPFJetEta_);
    recoT->Branch("reco_PF_jet_phi", &recoPFJetPhi_);
    recoT->Branch("reco_PF_jet_BTag", &recoPFJetBTag_);
    recoT->Branch("reco_PF_HEM_flag", &recoPFHEMFlag_);
    recoT->Branch("reco_PF_METjet_dphi", &recoPFMETJetDeltaPhi_);
    recoT->Branch("reco_MHT_Pt", &MHTPt_);
    recoT->Branch("cuts", &cuts_);

    if (!isData) {

        genT = fs->make<TTree>("genT", "");

        genT->Branch("event_num", &eventNum_);
        genT->Branch("gen_pu_obs", &genpuobs_);
        genT->Branch("gen_pu_true", &genputrue_);
        genT->Branch("gen_wgt", &genwgt_);
        genT->Branch("gen_mu_pt", &genPt_);
        genT->Branch("gen_mu_eta", &genEta_);
        genT->Branch("gen_mu_phi", &genPhi_);
        genT->Branch("gen_mu_energy", &genEn_);
        genT->Branch("gen_mu_dR", &genDr_);
        genT->Branch("gen_mu_vxy", &genVxy_);
        genT->Branch("gen_mu_vz", &genVz_);
        genT->Branch("gen_chi1_pt", &genChi1Pt_);
        genT->Branch("gen_chi1_eta", &genChi1Eta_);
        genT->Branch("gen_chi1_phi", &genChi1Phi_);
        genT->Branch("gen_chi1_energy", &genChi1En_);
        genT->Branch("gen_chi1_vxy", &genChi1Vxy_);
        genT->Branch("gen_chi1_vz", &genChi1Vz_);
        genT->Branch("gen_chi2_pt", &genChi2Pt_);
        genT->Branch("gen_chi2_eta", &genChi2Eta_);
        genT->Branch("gen_chi2_phi", &genChi2Phi_);
        genT->Branch("gen_chi2_energy", &genChi2En_);
        genT->Branch("gen_chi2_vxy", &genChi2Vxy_);
        genT->Branch("gen_chi2_vz", &genChi2Vz_);
        genT->Branch("gen_jet_pt", &genJetPt_);
        genT->Branch("gen_jet_eta", &genJetEta_);
        genT->Branch("gen_jet_phi", &genJetPhi_);
        genT->Branch("gen_MET_pt", &genLeadMetPt_);
        genT->Branch("gen_MET_phi", &genLeadMetPhi_);
    }
}


void iDMAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    using namespace edm;

    bool changed = true;
    if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
        if (changed) {
            LogInfo("HLTConfig") << "iDMAnalyzer::beginRun: " << "hltConfig init for Run" << iRun.run();
            hltConfig_.dump("ProcessName");
            hltConfig_.dump("GlobalTag");
            hltConfig_.dump("TableName");
        }
    } 
    else {
        LogError("HLTConfig") << "iDMAnalyzer::beginRun: config extraction failure with processName -> " << processName_;
    }
}


bool iDMAnalyzer::getCollections(const edm::Event& iEvent) {
    using namespace edm;

    char error_msg[] = "iDMAnalyzer::GetCollections: Error in getting product %s from Event!";
        
    iEvent.getByToken(bTagProbbToken_, bTagProbbHandle_);
    if (!bTagProbbHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "bTagProbb");
        return false;
    }
    iEvent.getByToken(bTagProbbbToken_, bTagProbbbHandle_);
    if (!bTagProbbbHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "bTagProbbb");
        return false;
    }
    iEvent.getByToken(muTrackToken1_, muTrackHandle1_);
    if (!muTrackHandle1_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "muTrack1");
        return false;
    }
    iEvent.getByToken(muTrackToken2_, muTrackHandle2_);
    if (!muTrackHandle2_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "muTrack2");
        return false;
    }
    iEvent.getByToken(recoMetToken_, recoMetHandle_);
    if (!recoMetHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoMET");
        return false;
    }
    iEvent.getByToken(recoJetToken_, recoJetHandle_);
    if (!recoJetHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoJet");
        return false;
    }
    iEvent.getByToken(trigResultsToken_, trigResultsHandle_);
    if (!trigResultsHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "trigResults");
        return false;
    }
    iEvent.getByToken(trigEventToken_, trigEventHandle_);
    if (!trigEventHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "trigEvent");
        return false;
    }
    const std::vector<std::string>& pathNames = hltConfig_.triggerNames();
    const std::vector<std::string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
    if (matchedPaths.size() == 0) {
        LogError("TriggerError") << "Could not find matched full trigger path with -> " << trigPathNoVer_;
        return false;
    }
    trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
        LogError("TriggerError") << "Cannot find trigger path -> " << trigPath_;
        return false;
    }
    iEvent.getByToken(HBHENoiseFilterResultProducerToken_, HBHENoiseFilterResultProducerHandle_);
    if (!HBHENoiseFilterResultProducerHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "HBHENoiseFilter");
        return false;
    }
    iEvent.getByToken(primaryVertexFilterToken_, primaryVertexFilterHandle_);
    if (!primaryVertexFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "primaryVertexFilter");
        return false;
    }
    iEvent.getByToken(globalSuperTightHalo2016FilterToken_, globalSuperTightHalo2016FilterHandle_);
    if (!globalSuperTightHalo2016FilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "globalSuperTightHalo2016");
        return false;
    }
    iEvent.getByToken(EcalDeadCellTriggerPrimitiveFilterToken_, EcalDeadCellTriggerPrimitiveFilterHandle_);
    if (!EcalDeadCellTriggerPrimitiveFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "EcalDeadCellTriggerPrimitive");
        return false;
    }
    iEvent.getByToken(ecalBadCalibFilterToken_, ecalBadCalibFilterHandle_);
    if (!ecalBadCalibFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "ecalBadCalibFilter");
        return false;
    }
    iEvent.getByToken(BadPFMuonFilterToken_, BadPFMuonFilterHandle_);
    if (!BadPFMuonFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "BadPFMuonFilter");
        return false;
    }
    iEvent.getByToken(muonBadTrackFilterToken_, muonBadTrackFilterHandle_);
    if (!muonBadTrackFilterHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "muonBadTrackFilter");
        return false;
    }

    if (!isData) {
        iEvent.getByToken(genEvtInfoToken_, genEvtInfoHandle_);
        if (!genEvtInfoHandle_.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % "genEventInfo");
            return false;
        }
        iEvent.getByToken(genParticleToken_, genParticleHandle_);
        if (!genParticleHandle_.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % "genParticle");
            return false;
        }
        iEvent.getByToken(genJetToken_, genJetHandle_);
        if (!genJetHandle_.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % "genJet");
            return false;
        }
        iEvent.getByToken(genMetToken_, genMetHandle_);
        if (!genMetHandle_.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % "genMET");
            return false;
        }
        iEvent.getByToken(pileupInfosToken_, pileupInfosHandle_);
        if (!pileupInfosHandle_.isValid()) {
            LogError("HandleError") << boost::str(boost::format(error_msg) % "pileupInfos");
            return false;
        }
    }

    return true;
}


void iDMAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using std::cout, std::vector, std::endl;

    if (!getCollections(iEvent))
        return;

    //const edm::EventAuxiliary& aux = iEvent.eventAuxiliary();
    //event_ = aux.event();

    // Event information
    eventNum_ = iEvent.id().event();
    lumiSec_ = iEvent.luminosityBlock();
    runNum_ = iEvent.id().run();
    npv_ = *primaryVertexFilterHandle_;

    // MET filters
    METFiltersFailBits_ = 0;
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
    


    recoPt_ .clear();
    recoEta_.clear();
    recoPhi_.clear();
    recoDxy_.clear();
    recoDz_ .clear();
    recoCharge_.clear();
    recoTrkChi2_.clear();
    recoTrkNumPlanes_.clear();
    recoTrkNumHits_.clear();
    recoDSAIdx0_ = -9999;
    recoDSAIdx1_ = -9999;
    recoPFJetPt_.clear();
    recoPFJetEta_.clear();
    recoPFJetPhi_.clear();
    recoPFJetBTag_.clear();
    recoPFHEMFlag_ = false;
    selectedMuonsPt_.clear();
    selectedMuonsEta_.clear();
    selectedMuonsPhi_.clear();
    selectedMuonsDxy_.clear();
    selectedMuonsDz_.clear();
    selectedMuonsCharge_.clear();

    fired_ = 0;
    recoPFMetPt_ = -9999;
    recoPFMetPhi_ = -9999;
    recoMmumu_ = -9999;
    recoDeltaPhiMetMu_ = -9999;
    MHTPt_ = -9999;
    recoNMatchedGBMvDSA_ = -1;

    // Trigger check firing bit (MET+MHT 120 GeV trigger)
    fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));

    // get MET
    // assumes 0-th element of recoMet collection is largest pt (and only?) element
    reco::PFMETRef recoMetr(recoMetHandle_, 0);
    recoPFMetPt_ = recoMetr->pt();
    recoPFMetPhi_ = recoMetr->phi();
    
    // calculate MHT
    math::XYZTLorentzVector MHT;
    for (auto & jet : *recoJetHandle_) {
        if (jet.pt() < 30) continue;
        MHT += jet.p4();
    }
    MHTPt_ = MHT.pt();

    // Sort dSA muons (note that muon collection is *not* sorted by pT at first)
    recoNMu_ = muTrackHandle1_->size();

    vector<reco::TrackRef> muTracks1{};
    for (size_t i = 0; i < muTrackHandle1_->size(); i++) {
        muTracks1.emplace_back(muTrackHandle1_, i);
    }
    sort(muTracks1.begin(), muTracks1.end(), [](const auto & l, const auto & r){ return l->pt() > r->pt(); });

    // Sort global muons (note that muon collection is *not* sorted by pT at first)
    vector<reco::TrackRef> muTracks2{};
    for (size_t i = 0; i < muTrackHandle2_->size(); i++) {
        muTracks2.emplace_back(muTrackHandle2_, i);
    }
    sort(muTracks2.begin(), muTracks2.end(), [](const auto & l, const auto & r){ return l->pt() > r->pt(); });

    // Create separate collection for good quality dSA muons
    vector<int> muGoodTracksIdx{};
    for (size_t i = 0; i < muTracks1.size(); i++) {
        if (muTracks1[i]->hitPattern().muonStationsWithValidHits() < 2 ||
            muTracks1[i]->hitPattern().numberOfValidMuonHits() < 12 ||
            muTracks1[i]->normalizedChi2() > 10 ||
	       	muTracks1[i]->pt() < 5 ||
            muTracks1[i]->eta() > 2.4) {
                continue;
        }
        muGoodTracksIdx.push_back(i);
    }
    recoNGoodDSAMu_ = muGoodTracksIdx.size();
    
    // Create separate collection for good quality global muons
    vector<int> muGoodTracksIdx2{};
    for (size_t i = 0; i < muTracks2.size(); i++) {
        if (muTracks2[i]->hitPattern().muonStationsWithValidHits() < 2 ||
            muTracks2[i]->hitPattern().numberOfValidMuonHits() < 12 ||
            muTracks2[i]->normalizedChi2() > 10 ||
	       	muTracks2[i]->pt() < 5 ||
            muTracks2[i]->eta() > 2.4) {
                continue;
        }
        muGoodTracksIdx2.push_back(i);
    }
    
    // Only add good muons' info to ntuple
    for (size_t i = 0; i < muGoodTracksIdx.size(); i++) {
        reco::TrackRef mu_i = muTracks1[muGoodTracksIdx[i]];
        recoPt_.push_back(mu_i->pt());
        recoEta_.push_back(mu_i->eta());
        recoPhi_.push_back(mu_i->phi());
        recoDxy_.push_back(mu_i->dxy());
        recoDz_.push_back(mu_i->dz());
        recoCharge_.push_back(mu_i->charge());
        recoTrkChi2_.push_back(mu_i->normalizedChi2());
        recoTrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        recoTrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
    }

    // Apply Jet loose ID to jet collection, tag passes/fails on a side vector
    // Additionally mitigate HEM issue on chambers 15 and 16
    vector<bool> jetIDResults;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        reco::PFJetRef jet_i(recoJetHandle_, i);
        bool jetIDResult = true;
        if (
                jet_i->neutralHadronEnergyFraction() > 0.99 || 
                jet_i->neutralEmEnergyFraction() > 0.99 ||
                jet_i->numberOfDaughters() <= 1
           )
            jetIDResult = false;
        if (jet_i->eta() < 2.4) {
            if (
                    jet_i->chargedHadronEnergyFraction() <= 0 ||
                    jet_i->chargedEmEnergyFraction() > 0.99 ||
                    jet_i->chargedMultiplicity() <= 0
               )
                jetIDResult = false;
        }
        jetIDResults.push_back(jetIDResult);

        // If passed jet is located in HEM region, veto the event
        // Has to happpen before jet ID, so don't check for jetIDResult
        double pt = jet_i->pt(), eta = jet_i->eta(), phi = jet_i->phi();
        if (pt > 30 && eta > -3.0 && eta < -1.4 && phi > -1.57 && phi < -0.87)
            recoPFHEMFlag_ = true;
    }
    
    // Perform cross-cleaning in jet collection with good-quality dSA muons
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        // Skip failed ID jets
        if (!jetIDResults[i]) continue;
        reco::PFJetRef jet_i(recoJetHandle_, i);
        for (size_t j = 0; j < muGoodTracksIdx.size(); ++j) {
            reco::TrackRef mu_j = muTracks1[muGoodTracksIdx[j]];
            double dR = reco::deltaR(*jet_i, *mu_j);
            // if muon and jet match in dR fail the ID jet
            // because the jet is probably a muon instead
            if (dR < 0.3)
                jetIDResults[i] = false;
        }
    }
    
    // Get 10 top leading jets info, sorted by pT
    // Note that jet collection is already sorted by pT
    // Only pick jets that have passed loose ID and cross-cleaning
    const reco::JetTagCollection & bTagsProbb = *(bTagProbbHandle_.product());
    const reco::JetTagCollection & bTagsProbbb = *(bTagProbbbHandle_.product());
    recoPFNJet_ = recoJetHandle_->size(); 
    recoPFNPassIDJet_ = 0;
    recoPFNHighPtJet_ = 0;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        // Exclude jets that didn't pass ID above
        if (!jetIDResults[i]) continue;
        recoPFNPassIDJet_++;

        reco::PFJetRef jet_i(recoJetHandle_, i);
        if (jet_i->pt() > 30) {
            recoPFNHighPtJet_++;
        }
        if (i < 10) {
            recoPFJetPt_.push_back(jet_i->pt());
       		recoPFJetEta_.push_back(jet_i->eta());
       		recoPFJetPhi_.push_back(jet_i->phi());
            if (bTagsProbb.size() > i && bTagsProbbb.size() > i)
                recoPFJetBTag_.push_back(bTagsProbb[i].second+bTagsProbbb[i].second);
            else
                recoPFJetBTag_.push_back(-10000);
        }
    }

    // Calculate angle between MET and leading jet
    recoPFMETJetDeltaPhi_ = reco::deltaPhi(recoPFJetPhi_[0], recoPFMetPhi_);

    // Pick pair of dSA muons with smallest vertex chi square fit
    bool fFoundValidVertex = false;
    int dSAIdx[2]; dSAIdx[0] = -1; dSAIdx[1] = -1;
    recoVtxVxy_ = -9999;
    recoVtxVz_ = -9999;
    recoVtxSigmaVxy_ = -9999;
    recoVtxReducedChi2_ = 9999999;
    recoVtxDr_ = -9999;

    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
    KalmanVertexFitter kvf(true);

    if (recoNGoodDSAMu_ >= 2) {
        for (size_t i = 0; i < muGoodTracksIdx.size()-1; i++) {
            for (size_t j = i+1; j < muGoodTracksIdx.size(); j++) {

                TransientVertex tv;
                vector<reco::TransientTrack> trans_tracks{};
                trans_tracks.push_back(theB->build(muTracks1[muGoodTracksIdx[i]]));
                trans_tracks.push_back(theB->build(muTracks1[muGoodTracksIdx[j]]));
                tv = kvf.vertex(trans_tracks);
                if (tv.isValid()) {
                    reco::Vertex vertex = reco::Vertex(tv);
                    float vxy = sqrt(vertex.x()*vertex.x() + vertex.y()*vertex.y());
                    float sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
                    float vtxChi2 = vertex.normalizedChi2();
                    if (vtxChi2 < recoVtxReducedChi2_) {
                        fFoundValidVertex = true;
                        recoVtxReducedChi2_ = vtxChi2;
                        recoVtxVxy_ = vxy;
                        recoVtxSigmaVxy_ = sigma_vxy;
                        recoVtxVz_ = vertex.z();
                        recoVtxDr_ = reco::deltaR(*muTracks1[muGoodTracksIdx[i]], *muTracks1[muGoodTracksIdx[j]]);
                        dSAIdx[0] = muGoodTracksIdx[i]; 
                        dSAIdx[1] = muGoodTracksIdx[j];
                        recoDSAIdx0_ = muGoodTracksIdx[i]; 
                        recoDSAIdx1_ = muGoodTracksIdx[j];
                    }
                }
            }
        }
        if (!fFoundValidVertex)
            edm::LogWarning("VertexFittingFailed") << "Warning! No valid vertices found!";
    }

    nSelectedMuons_ = -1;
    if (fFoundValidVertex)
        nSelectedMuons_ = 2;
    else if (recoNGoodDSAMu_ == 1)
        nSelectedMuons_ = 1;
    else if (recoNGoodDSAMu_ == 0)
        nSelectedMuons_ = 0;

    // Now try to match global muons with selected dSA muons
    float smallest_dR = 10000, second_smallest_dR = 10000;
    int smallest_i = -1, smallest_j = -1, second_smallest_i = -1;
    vector<vector<float>> deltaRs;
    deltaRs.resize(muGoodTracksIdx2.size(), vector<float>(2));
    if (fFoundValidVertex) {
        recoNMatchedGBMvDSA_ = 0;
        for (size_t i = 0; i < muGoodTracksIdx2.size(); i++) {
            reco::TrackRef gbm_i = muTracks2[muGoodTracksIdx2[i]];
            for (size_t j = 0; j < 2; j++) {
                reco::TrackRef dsa_j = muTracks1[dSAIdx[j]];
                float dR = reco::deltaR(gbm_i->outerEta(), gbm_i->outerPhi(),
                        dsa_j->outerEta(), dsa_j->outerPhi());

                deltaRs[i][j] = dR;
                if (dR < smallest_dR) {
                    smallest_dR = dR;
                    smallest_i = (int)i; smallest_j = (int)j;
                }
            }
        }
        recoGBMDSADr_ = smallest_dR;
        if (smallest_dR < 0.1) {
            recoNMatchedGBMvDSA_++;
            reco::TrackRef gbm_i = muTracks2[muGoodTracksIdx2[smallest_i]];
            selectedMuonsPt_.push_back(gbm_i->pt());
            selectedMuonsEta_.push_back(gbm_i->eta());
            selectedMuonsPhi_.push_back(gbm_i->phi());
            selectedMuonsDxy_.push_back(gbm_i->dxy());
            selectedMuonsDz_.push_back(gbm_i->dz());
            selectedMuonsCharge_.push_back(gbm_i->charge());
            for (size_t i = 0; i < muGoodTracksIdx2.size(); i++) {
                for (size_t j = 0; j < 2; j++) {
                    if ((int)i != smallest_i && (int)j != smallest_j) {
                        if (deltaRs[i][j] < second_smallest_dR) {
                            second_smallest_dR = deltaRs[i][j];
                            second_smallest_i = (int)i;
                        }
                    }
                }
            }
            if (second_smallest_dR < 0.1) {
                recoNMatchedGBMvDSA_++;
                reco::TrackRef gbm2_i = muTracks2[muGoodTracksIdx2[second_smallest_i]];
                selectedMuonsPt_.push_back(gbm2_i->pt());
                selectedMuonsEta_.push_back(gbm2_i->eta());
                selectedMuonsPhi_.push_back(gbm2_i->phi());
                selectedMuonsDxy_.push_back(gbm2_i->dxy());
                selectedMuonsDz_.push_back(gbm2_i->dz());
                selectedMuonsCharge_.push_back(gbm2_i->charge());
            }
            else {
                reco::TrackRef dsa2_j = muTracks1[dSAIdx[1-smallest_j]];
                selectedMuonsPt_.push_back(dsa2_j->pt());
                selectedMuonsEta_.push_back(dsa2_j->eta());
                selectedMuonsPhi_.push_back(dsa2_j->phi());
                selectedMuonsDxy_.push_back(dsa2_j->dxy());
                selectedMuonsDz_.push_back(dsa2_j->dz());
                selectedMuonsCharge_.push_back(dsa2_j->charge());
            }
        }
        else {
            reco::TrackRef dsa1_j = muTracks1[dSAIdx[0]];
            selectedMuonsPt_.push_back(dsa1_j->pt());
            selectedMuonsEta_.push_back(dsa1_j->eta());
            selectedMuonsPhi_.push_back(dsa1_j->phi());
            selectedMuonsDxy_.push_back(dsa1_j->dxy());
            selectedMuonsDz_.push_back(dsa1_j->dz());
            selectedMuonsCharge_.push_back(dsa1_j->charge());
            reco::TrackRef dsa2_j = muTracks1[dSAIdx[1]];
            selectedMuonsPt_.push_back(dsa2_j->pt());
            selectedMuonsEta_.push_back(dsa2_j->eta());
            selectedMuonsPhi_.push_back(dsa2_j->phi());
            selectedMuonsDxy_.push_back(dsa2_j->dxy());
            selectedMuonsDz_.push_back(dsa2_j->dz());
            selectedMuonsCharge_.push_back(dsa2_j->charge());
        }
    }
    
    // Re-fit vertex if one or more dsa muons got replaced
    reco::TrackRef muon1, muon2;
    if (smallest_dR < 0.1) {
        muon1 = muTracks2[muGoodTracksIdx2[smallest_i]];
        if (second_smallest_dR < 0.1)
            muon2 = muTracks2[muGoodTracksIdx2[second_smallest_i]];
        else if (recoNGoodDSAMu_ >= 2)
            muon2 = muTracks1[dSAIdx[1-smallest_j]];
    }
    else if (recoNGoodDSAMu_ >= 1) {
        if (fFoundValidVertex)
            muon1 = muTracks1[dSAIdx[0]];
        else
            muon1 = muTracks1[0];
        if (recoNGoodDSAMu_ >= 2) {
            if (fFoundValidVertex)
                muon2 = muTracks1[dSAIdx[1]];
            else
                muon2 = muTracks1[1];
        }
    }

    if (smallest_dR < 0.1) {
        TransientVertex tv;
        vector<reco::TransientTrack> trans_tracks{};
        trans_tracks.push_back(theB->build(muon1));
        trans_tracks.push_back(theB->build(muon2));
        tv = kvf.vertex(trans_tracks);
        if (!tv.isValid()) {
            edm::LogWarning("VertexFittingFailed") << "Warning! Vertex re-fitting failed!";
            fFoundValidVertex = false;
        }
        else {
            reco::Vertex vertex = reco::Vertex(tv);
            float vxy = sqrt(vertex.x()*vertex.x() + vertex.y()*vertex.y());
            float sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
            float vtxChi2 = vertex.normalizedChi2();
            recoVtxReducedChi2_ = vtxChi2;
            recoVtxVxy_ = vxy;
            recoVtxSigmaVxy_ = sigma_vxy;
            recoVtxVz_ = vertex.z();
            float dR = reco::deltaR(muon1->outerEta(), muon1->outerPhi(),
                    muon2->outerEta(), muon2->outerPhi());
            recoVtxDr_ = dR;
        }
    }
    
    // Calculate delta phi between MET and selected muons
    if (fFoundValidVertex) {
        double avgMuPhi = atan2( (1/2)*(sin(muon1->phi()) + sin(muon2->phi())), (1/2)*(cos(muon1->phi()) + cos(muon2->phi())) );
        double reducedPhiDiff = reco::deltaPhi(recoMetr->phi(), avgMuPhi);
        recoDeltaPhiMetMu_ = reducedPhiDiff;
    }

    // Calculate invariant mass of selected muons
    if (fFoundValidVertex)
        recoMmumu_ = std::sqrt(2 * muon1->pt() * muon2->pt() *
            (cosh(muon1->eta() - muon2->eta()) -
             cos(muon1->phi() - muon2->phi())));

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

        genPt_.clear();
        genEta_.clear();
        genPhi_.clear();
        genEn_.clear();
        genVxy_.clear();
        genVz_.clear();
        genChi1Pt_.clear();
        genChi1Eta_.clear();
        genChi1Phi_.clear();
        genChi1En_.clear();
        genChi1Vxy_.clear();
        genChi1Vz_.clear();
        genChi2Pt_.clear();
        genChi2Eta_.clear();
        genChi2Phi_.clear();
        genChi2En_.clear();
        genChi2Vxy_.clear();
        genChi2Vz_.clear();
        genDr_.clear();
        genJetPt_.clear();
        genJetEta_.clear();
        genJetPhi_.clear();


        // Pile-up and event genweight
        genpuobs_ = -9999;
        genputrue_ = -9999;
        genwgt_ = -9999;
        genLeadMetPt_ = -9999;
        genLeadMetPhi_ = -9999;

        // Gen weight
        genwgt_ = genEvtInfoHandle_->weight();

        // Pile-up
        for (const auto & pileupInfo : *pileupInfosHandle_) {
            if (pileupInfo.getBunchCrossing() == 0) {
                genpuobs_ = pileupInfo.getPU_NumInteractions();
                genputrue_ = pileupInfo.getTrueNumInteractions();
                break;
            }
        }

        // all hard-process gen muons
        for (size_t i = 0; i < genParticleHandle_->size(); i++) {
            reco::GenParticleRef genParticle(genParticleHandle_, i);
            if (genParticle->isHardProcess() && (std::abs(genParticle->pdgId()) == 13)) {
                genPt_.push_back(genParticle->pt());
                genEta_.push_back(genParticle->eta());
                genPhi_.push_back(genParticle->phi());
                genEn_.push_back(genParticle->energy());
                genVxy_.push_back(genParticle->vertex().rho());
                genVz_.push_back(genParticle->vz());
            }
        }

        // all hard-process gen DM_Chi1
        for (size_t i = 0; i < genParticleHandle_->size(); i++) {
            reco::GenParticleRef genParticle(genParticleHandle_, i);
            if (genParticle->isHardProcess() && (std::abs(genParticle->pdgId()) == 1000022)) {
                genChi1Pt_.push_back(genParticle->pt());
                genChi1Eta_.push_back(genParticle->eta());
                genChi1Phi_.push_back(genParticle->phi());
                genChi1En_.push_back(genParticle->energy());
                genChi1Vxy_.push_back(genParticle->vertex().rho());
                genChi1Vz_.push_back(genParticle->vz());
            }
        }

        // all hard-process gen DM_Chi2
        for (size_t i = 0; i < genParticleHandle_->size(); i++) {
            reco::GenParticleRef genParticle(genParticleHandle_, i);
            if (genParticle->isHardProcess() && (std::abs(genParticle->pdgId()) == 1000023)) {
                genChi2Pt_.push_back(genParticle->pt());
                genChi2Eta_.push_back(genParticle->eta());
                genChi2Phi_.push_back(genParticle->phi());
                genChi2En_.push_back(genParticle->energy());
                genChi2Vxy_.push_back(genParticle->vertex().rho());
                genChi2Vz_.push_back(genParticle->vz());
            }
        }

        // all gen jets
        for (size_t i = 0; i < genJetHandle_->size(); i++) {
            reco::GenJetRef jetRef(genJetHandle_, i);
            genJetPt_.push_back(jetRef->pt());
            genJetEta_.push_back(jetRef->eta());
            genJetPhi_.push_back(jetRef->phi());
        }

        // Lead gen MET
        genLeadMetPt_ = -9999;
        genLeadMetPhi_ = -9999;
        if (genMetHandle_->size() > 0) {
            reco::GenMETRef metRef(genMetHandle_, 0);
            genLeadMetPt_ = metRef->pt();
            genLeadMetPhi_ = metRef->phi();
        }

        genT->Fill();
    }


    /******* BEGINNING OF CUTS ******/
    // Pre-computation of some cuts, stored in bits

    cuts_ = 0;

    // Fill everything before cuts 
    setCutBit(0);

    // Trigger check
    if (fired_)
        setCutBit(1);

    // MET offline selection of 200 GeV
    if (recoPFMetPt_ > 200) 
        setCutBit(2);

    // One leading reco jet w/ pT > 120...
    if (recoPFJetPt_[0] > 120)
        setCutBit(3);
    
    // ...and only one extra jet w/ pT > 30 GeV
    
    recoPFNHighPtJet_ = 0;
    setCutBit(4);
    for (size_t i = 0; i < recoPFJetPt_.size(); i++) 
        if (recoPFJetPt_[i] > 30)
            recoPFNHighPtJet_++;
    if (recoPFNHighPtJet_ >= 3) clearCutBit(4);

    // None of the 1-or-2 high-pt jets is b-tagged
    setCutBit(5);
    if (recoPFNHighPtJet_ <= 2) 
        for (int i = 0; i < recoPFNHighPtJet_; i++) 
            if (recoPFJetBTag_[i] > 0.4184) // || recoPFJetBTag_[i] < 0) --> negative if no b-tagging info for some reason (but don't veto on negative)
                clearCutBit(5);

    // dsa muon cuts
    for (size_t i = 0; i < 2; i++) {
        int numMuCuts = 5;
        if (dSAIdx[i] == -1) continue;
        reco::TrackRef mu_i = muTracks1[dSAIdx[i]];

        if (mu_i->hitPattern().muonStationsWithValidHits() >= 2)
            setCutBit(6 + i*numMuCuts);
        if (mu_i->hitPattern().numberOfValidMuonHits() >= 12)
            setCutBit(7 + i*numMuCuts);
        if (mu_i->normalizedChi2() < 10)
            setCutBit(8 + i*numMuCuts);

        if (mu_i->pt() > 5)
            setCutBit(9 + i*numMuCuts);
        if (mu_i->eta() < 2.4)
            setCutBit(10 + i*numMuCuts);
        //if (abs(mu_tmp->dxy()) > 0.1 && abs(mu_tmp->dxy()) < 700)
         //   cutsVec[9 + i*numMuCuts] = 1;
    }

    // Check dR between selected muons (SR)
    if (recoNGoodDSAMu_ > 1 && fFoundValidVertex) 
        if (std::abs(recoVtxDr_) < 0.8)
            setCutBit(16);

    // Check invariant mass between muons (SR)
    if (std::abs(recoMmumu_) < 50)
        setCutBit(17);

    // Check OS muons
    if (recoNGoodDSAMu_ > 1 && fFoundValidVertex)
       if ((selectedMuonsCharge_[0]+selectedMuonsCharge_[1]) == 0)
            setCutBit(18);
    
    // Check DeltaPhi between MET and leading muon pair
    if (recoNGoodDSAMu_ > 1 && fFoundValidVertex)
        if (std::abs(recoDeltaPhiMetMu_) < 0.4)
            setCutBit(19);
    
    // Only have 1 good dSA muon (one-lepton CR)
    if (recoNGoodDSAMu_ == 1 && nSelectedMuons_ == 1)
        setCutBit(20);

    // Number of matched GBM-DSA muons
    setCutBit(21 + recoNMatchedGBMvDSA_);

    recoT->Fill();

    return;
}

void iDMAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void iDMAnalyzer::endJob() {}
