#include "iDMSkimmer/washAOD/interface/iDMAnalyzer.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "CommonTools/Utils/interface/InvariantMass.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "JetMETCorrections/Modules/interface/JetResolution.h"

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
#include <memory>

iDMAnalyzer::iDMAnalyzer(const edm::ParameterSet& ps):
    isData(ps.getUntrackedParameter<bool>("isData", false)),

    bTagProbbTag_(ps.getParameter<edm::InputTag>("bTagProbb")),
    bTagProbbbTag_(ps.getParameter<edm::InputTag>("bTagProbbb")),
    dsaRecoMuTag_(ps.getParameter<edm::InputTag>("dsaRecoMu")),
    dsaRecoMuTimingTag_(ps.getParameter<edm::InputTag>("dsaRecoMuTiming")),
    muTrackTag1_(ps.getParameter<edm::InputTag>("muTrack1")),
    muTrackTag2_(ps.getParameter<edm::InputTag>("muTrack2")),
    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
    genMETTag_(ps.getParameter<edm::InputTag>("genMET")),
    recoPFMETTag_(ps.getParameter<edm::InputTag>("PFMET")),
    recoCaloMETTag_(ps.getParameter<edm::InputTag>("caloMET")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    trigResultsTag_(ps.getParameter<edm::InputTag>("trigResult")),
    trigEventTag_(ps.getParameter<edm::InputTag>("trigEvent")),
    pileupInfosTag_(ps.getParameter<edm::InputTag>("pileups")),
    genEvtInfoTag_(ps.getParameter<edm::InputTag>("genEvt")),
    triggerProcessName_(ps.getParameter<std::string>("triggerProcessName")),
    HBHENoiseFilterResultProducerTag_("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"),
    HBHEIsoNoiseFilterResultProducerTag_("HBHENoiseFilterResultProducer","HBHEIsoNoiseFilterResult"),
    primaryVertexFilterTag_("myPrimaryVertexFilter"),
    globalSuperTightHalo2016FilterTag_("globalSuperTightHalo2016Filter"),
    EcalDeadCellTriggerPrimitiveFilterTag_("EcalDeadCellTriggerPrimitiveFilter"),
    ecalBadCalibFilterTag_("ecalBadCalibFilter"),
    BadPFMuonFilterTag_("BadPFMuonFilter"),
    muonBadTrackFilterTag_("muonBadTrackFilter"),
    mJetCorrectorTag_(ps.getParameter<edm::InputTag>("jetCorrector")),
    recoElectronTag_("gedGsfElectrons"),
    recoElectronIDTag_(ps.getParameter<edm::InputTag>("electronID")),
    recoPhotonTag_("gedPhotons"),
    recoPhotonIDTag_(ps.getParameter<edm::InputTag>("photonID")),
    rhoTag_(ps.getParameter<edm::InputTag>("rho")),
    
    bTagProbbToken_(consumes<reco::JetTagCollection>(bTagProbbTag_)),
    bTagProbbbToken_(consumes<reco::JetTagCollection>(bTagProbbbTag_)),
    dsaRecoMuToken_(consumes<reco::MuonCollection>(dsaRecoMuTag_)),
    dsaRecoMuTimingToken_(consumes<reco::MuonTimeExtraMap>(dsaRecoMuTimingTag_)),
    muTrackToken1_(consumes<reco::TrackCollection>(muTrackTag1_)),
    muTrackToken2_(consumes<reco::TrackCollection>(muTrackTag2_)),
    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
    genMETToken_(consumes<reco::GenMETCollection>(genMETTag_)),
    recoPFMETToken_(consumes<reco::PFMETCollection>(recoPFMETTag_)),
    recoCaloMETToken_(consumes<reco::CaloMETCollection>(recoCaloMETTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
    pileupInfosToken_(consumes<std::vector<PileupSummaryInfo>>(pileupInfosTag_)),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_)),
    HBHENoiseFilterResultProducerToken_(consumes<bool>(HBHENoiseFilterResultProducerTag_)),
    HBHEIsoNoiseFilterResultProducerToken_(consumes<bool>(HBHEIsoNoiseFilterResultProducerTag_)),
    primaryVertexFilterToken_(consumes<int>(primaryVertexFilterTag_)),
    globalSuperTightHalo2016FilterToken_(consumes<bool>(globalSuperTightHalo2016FilterTag_)),
    EcalDeadCellTriggerPrimitiveFilterToken_(consumes<bool>(EcalDeadCellTriggerPrimitiveFilterTag_)),
    ecalBadCalibFilterToken_(consumes<bool>(ecalBadCalibFilterTag_)),
    BadPFMuonFilterToken_(consumes<bool>(BadPFMuonFilterTag_)),
    muonBadTrackFilterToken_(consumes<bool>(muonBadTrackFilterTag_)),
    mJetCorrectorToken_(consumes<reco::JetCorrector>(mJetCorrectorTag_)),
    recoElectronToken_(consumes<reco::GsfElectronCollection>(recoElectronTag_)),
    recoElectronIDToken_(consumes<edm::ValueMap<float>>(recoElectronIDTag_)),
    recoPhotonToken_(consumes<reco::PhotonCollection>(recoPhotonTag_)),
    recoPhotonIDToken_(consumes<edm::ValueMap<bool>>(recoPhotonIDTag_)),
    rhoToken_(consumes<double>(rhoTag_))
{
    usesResource("TFileService");
    m_random_generator = std::mt19937(37428479);
}

iDMAnalyzer::~iDMAnalyzer() = default;

void iDMAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.addUntracked<bool>("isData", 0);
    desc.add<edm::InputTag>("bTagProbb", edm::InputTag("pfDeepCSVJetTags:probb")); 
    desc.add<edm::InputTag>("bTagProbbb", edm::InputTag("pfDeepCSVJetTags:probbb"));
    desc.add<edm::InputTag>("dsaRecoMu", edm::InputTag("muonsFromdSA"));
    desc.add<edm::InputTag>("dsaRecoMuTiming", edm::InputTag("muontimingFromdSA","combined"));
    desc.add<edm::InputTag>("muTrack1", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("muTrack2", edm::InputTag("globalMuons"));
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("genMET", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("PFMET", edm::InputTag("pfMetT0rtT1Txy"));
    desc.add<edm::InputTag>("caloMET", edm::InputTag("caloMet"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJetsCHS"));
    desc.add<edm::InputTag>("trigResult", edm::InputTag("TriggerResults", "", "HLT"));
    desc.add<edm::InputTag>("trigEvent", edm::InputTag("hltTriggerSummaryAOD", "", "HLT"));
    desc.add<edm::InputTag>("photonID", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("electronID", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("pileups", edm::InputTag("addPileupInfo"));
    desc.add<edm::InputTag>("genEvt", edm::InputTag("generator"));
    desc.add<std::string>("triggerProcessName", "HLT");
    desc.add<edm::InputTag>("jetCorrector", edm::InputTag("nodefault"));
    desc.add<edm::InputTag>("rho", edm::InputTag("fixedGridRhoFastjetAll"));
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
    recoT->Branch("reco_n_dsa", &recoNDSA_);
    recoT->Branch("reco_n_good_dsa", &recoNGoodDSA_);
    recoT->Branch("reco_dsa_pt",  &recoDSAPt_);
    recoT->Branch("reco_dsa_pt_err",  &recoDSAPtError_);
    recoT->Branch("reco_dsa_eta", &recoDSAEta_);
    recoT->Branch("reco_dsa_eta_err", &recoDSAEtaError_);
    recoT->Branch("reco_dsa_phi", &recoDSAPhi_);
    recoT->Branch("reco_dsa_phi_err", &recoDSAPhiError_);
    recoT->Branch("reco_dsa_dxy", &recoDSADxy_);
    recoT->Branch("reco_dsa_dxy_err", &recoDSADxyError_);
    recoT->Branch("reco_dsa_dz",  &recoDSADz_);
    recoT->Branch("reco_dsa_dz_err",  &recoDSADzError_);
    recoT->Branch("reco_dsa_charge", &recoDSACharge_);
    recoT->Branch("reco_dsa_trk_chi2", &recoDSATrkChi2_);
    recoT->Branch("reco_dsa_trk_n_planes", &recoDSATrkNumPlanes_);
    recoT->Branch("reco_dsa_trk_n_hits", &recoDSATrkNumHits_);
    recoT->Branch("reco_dsa_trk_n_DT_hits", &recoDSATrkNumDTHits_);
    recoT->Branch("reco_dsa_trk_n_CSC_hits", &recoDSATrkNumCSCHits_);
    recoT->Branch("reco_dsa_inv_beta", &recoDSAInvBeta_);
    recoT->Branch("reco_dsa_inv_beta_err", &recoDSAInvBetaErr_);
    recoT->Branch("reco_dsa_free_inv_beta", &recoDSAFreeInvBeta_);
    recoT->Branch("reco_dsa_free_inv_beta_err", &recoDSAFreeInvBetaErr_);
    recoT->Branch("reco_dsa_time_at_ip_in_out", &recoDSAtimeAtIpInOut_);
    recoT->Branch("reco_dsa_time_at_ip_in_out_err", &recoDSAtimeAtIpInOutErr_);
    recoT->Branch("reco_dsa_time_at_ip_out_in", &recoDSAtimeAtIpOutIn_);
    recoT->Branch("reco_dsa_time_at_ip_out_in_err", &recoDSAtimeAtIpOutInErr_);
    recoT->Branch("reco_dsa_time_ndof", &recoDSAtimingNdof_);
    recoT->Branch("reco_dsa_idx0", &recoDSAIdx0_);
    recoT->Branch("reco_dsa_idx1", &recoDSAIdx1_);
    recoT->Branch("reco_n_gm", &recoNGM_);
    recoT->Branch("reco_n_good_gm", &recoNGoodGM_);
    recoT->Branch("reco_gm_pt",  &recoGMPt_);
    recoT->Branch("reco_gm_pt_err",  &recoGMPtError_);
    recoT->Branch("reco_gm_eta", &recoGMEta_);
    recoT->Branch("reco_gm_eta_err", &recoGMEtaError_);
    recoT->Branch("reco_gm_phi", &recoGMPhi_);
    recoT->Branch("reco_gm_phi_err", &recoGMPhiError_);
    recoT->Branch("reco_gm_dxy", &recoGMDxy_);
    recoT->Branch("reco_gm_dxy_err", &recoGMDxyError_);
    recoT->Branch("reco_gm_dz",  &recoGMDz_);
    recoT->Branch("reco_gm_dz_err",  &recoGMDzError_);
    recoT->Branch("reco_gm_charge", &recoGMCharge_);
    recoT->Branch("reco_gm_trk_chi2", &recoGMTrkChi2_);
    recoT->Branch("reco_gm_trk_n_planes", &recoGMTrkNumPlanes_);
    recoT->Branch("reco_gm_trk_n_hits", &recoGMTrkNumHits_);
    recoT->Branch("reco_gm_trk_n_DT_hits", &recoGMTrkNumDTHits_);
    recoT->Branch("reco_gm_trk_n_CSC_hits", &recoGMTrkNumCSCHits_);
    recoT->Branch("reco_n_gbmdsa_match", &recoNMatchedGBMvDSA_);
    recoT->Branch("reco_gbmdsa_dR", &recoGMdSAdR_);
    recoT->Branch("reco_gbmdsa_match", &recoGMdSAmatch_);
    recoT->Branch("reco_sel_mu_pt", &selectedMuonsPt_);
    recoT->Branch("reco_sel_mu_pt_err", &selectedMuonsPtError_);
    recoT->Branch("reco_sel_mu_eta", &selectedMuonsEta_);
    recoT->Branch("reco_sel_mu_eta_err", &selectedMuonsEtaError_);
    recoT->Branch("reco_sel_mu_phi", &selectedMuonsPhi_);
    recoT->Branch("reco_sel_mu_phi_err", &selectedMuonsPhiError_);
    recoT->Branch("reco_sel_mu_dxy", &selectedMuonsDxy_);
    recoT->Branch("reco_sel_mu_dxy_error", &selectedMuonsDxyError_);
    recoT->Branch("reco_sel_mu_dz", &selectedMuonsDz_);
    recoT->Branch("reco_sel_mu_dz_error", &selectedMuonsDzError_);
    recoT->Branch("reco_sel_mu_charge", &selectedMuonsCharge_);
    recoT->Branch("reco_Mmumu",  &recoMmumu_);
    recoT->Branch("reco_METmu_dphi", &recoDeltaPhiMETMu_);
    recoT->Branch("reco_corr_METmu_dphi", &recoDeltaPhiCorrectedMETMu_);
    recoT->Branch("reco_vtx_dsadsa_vxy", &dsadsa_recoVtxVxy_);
    recoT->Branch("reco_vtx_dsadsa_vz",  &dsadsa_recoVtxVz_);
    recoT->Branch("reco_vtx_dsadsa_sigmavxy", &dsadsa_recoVtxSigmaVxy_);
    recoT->Branch("reco_vtx_dsadsa_reduced_chi2", &dsadsa_recoVtxReducedChi2_);
    recoT->Branch("reco_vtx_dsadsa_dR",  &dsadsa_recoVtxDr_);
    recoT->Branch("reco_vtx_gmgm_vxy", &gmgm_recoVtxVxy_);
    recoT->Branch("reco_vtx_gmgm_vz",  &gmgm_recoVtxVz_);
    recoT->Branch("reco_vtx_gmgm_sigmavxy", &gmgm_recoVtxSigmaVxy_);
    recoT->Branch("reco_vtx_gmgm_reduced_chi2", &gmgm_recoVtxReducedChi2_);
    recoT->Branch("reco_vtx_gmgm_dR",  &gmgm_recoVtxDr_);
    recoT->Branch("reco_vtx_dsagm_vxy", &dsagm_recoVtxVxy_);
    recoT->Branch("reco_vtx_dsagm_vz",  &dsagm_recoVtxVz_);
    recoT->Branch("reco_vtx_dsagm_sigmavxy", &dsagm_recoVtxSigmaVxy_);
    recoT->Branch("reco_vtx_dsagm_reduced_chi2", &dsagm_recoVtxReducedChi2_);
    recoT->Branch("reco_vtx_dsagm_dR",  &dsagm_recoVtxDr_);
    recoT->Branch("reco_n_electron", &recoNElectron_);
    recoT->Branch("reco_n_good_electron", &recoNGoodElectron_);
    recoT->Branch("reco_electron_pt",  &recoElectronPt_);
    recoT->Branch("reco_electron_eta", &recoElectronEta_);
    recoT->Branch("reco_electron_phi", &recoElectronPhi_);
    recoT->Branch("reco_electron_vxy", &recoElectronVxy_);
    recoT->Branch("reco_electron_vz",  &recoElectronVz_);
    recoT->Branch("reco_electron_charge", &recoElectronCharge_);
    recoT->Branch("reco_electron_id_result", &recoElectronIDResult_);
    recoT->Branch("reco_n_photon", &recoNPhoton_);
    recoT->Branch("reco_n_good_photon", &recoNGoodPhoton_);
    recoT->Branch("reco_photon_pt",  &recoPhotonPt_);
    recoT->Branch("reco_photon_eta", &recoPhotonEta_);
    recoT->Branch("reco_photon_phi", &recoPhotonPhi_);
    recoT->Branch("reco_photon_id_result", &recoPhotonIDResult_);
    recoT->Branch("reco_PF_MET_pt", &recoPFMETPt_);
    recoT->Branch("reco_PF_MET_phi", &recoPFMETPhi_);
    recoT->Branch("reco_PF_MET_smearing_pt", &recoPFMETSmearingOnlyPt_);
    recoT->Branch("reco_PF_MET_smearing_phi", &recoPFMETSmearingOnlyPhi_);
    recoT->Branch("reco_PF_MET_corr_pt", &recoPFMETCorrectedPt_);
    recoT->Branch("reco_PF_MET_corr_phi", &recoPFMETCorrectedPhi_);
    recoT->Branch("reco_PF_MET_corr_JESUp_pt", &recoPFMETJESUpPt_);
    recoT->Branch("reco_PF_MET_corr_JESUp_phi", &recoPFMETJESUpPhi_);
    recoT->Branch("reco_PF_MET_corr_JESDown_pt", &recoPFMETJESDownPt_);
    recoT->Branch("reco_PF_MET_corr_JESDown_phi", &recoPFMETJESDownPhi_);
    recoT->Branch("reco_PF_MET_corr_JERUp_pt", &recoPFMETJERUpPt_);
    recoT->Branch("reco_PF_MET_corr_JERUp_phi", &recoPFMETJERUpPhi_);
    recoT->Branch("reco_PF_MET_corr_JERDown_pt", &recoPFMETJERDownPt_);
    recoT->Branch("reco_PF_MET_corr_JERDown_phi", &recoPFMETJERDownPhi_);
    recoT->Branch("reco_Calo_MET_pt", &recoCaloMETPt_);
    recoT->Branch("reco_Calo_MET_phi", &recoCaloMETPhi_);
    recoT->Branch("reco_PF_recoil_pt", &recoPFRecoilPt_);
    recoT->Branch("reco_PF_recoil_phi", &recoPFRecoilPhi_);
    recoT->Branch("reco_PF_n_jets", &recoPFNJet_);
    recoT->Branch("reco_PF_n_passID_jets", &recoPFNPassIDJet_);
    recoT->Branch("reco_PF_n_highPt_jets", &recoPFNHighPtJet_);
    recoT->Branch("reco_PF_jet_pt", &recoPFJetPt_);
    recoT->Branch("reco_PF_jet_eta", &recoPFJetEta_);
    recoT->Branch("reco_PF_jet_phi", &recoPFJetPhi_);
    recoT->Branch("reco_PF_jet_corr_pt", &recoPFJetCorrectedPt_);
    recoT->Branch("reco_PF_jet_corr_eta", &recoPFJetCorrectedEta_);
    recoT->Branch("reco_PF_jet_corr_phi", &recoPFJetCorrectedPhi_);
    recoT->Branch("reco_PF_jet_corr_BTag", &recoPFJetCorrectedBTag_);
    recoT->Branch("reco_PF_jet_corr_CHEF", &recoPFJetCorrectedCHEF_);
    recoT->Branch("reco_PF_jet_corr_NHEF", &recoPFJetCorrectedNHEF_);
    recoT->Branch("reco_PF_jet_corr_CEEF", &recoPFJetCorrectedCEEF_);
    recoT->Branch("reco_PF_jet_corr_NEEF", &recoPFJetCorrectedNEEF_);
    recoT->Branch("reco_PF_jet_corr_NumDaughters", &recoPFJetCorrectedNumDaughters_);
    recoT->Branch("reco_PF_jet_corr_ChargedMultipl", &recoPFJetCorrectedChargedMultiplicity_);
    recoT->Branch("reco_PF_jet_corr_JESUp_pt", &recoPFJetCorrectedJESUpPt_);
    recoT->Branch("reco_PF_jet_corr_JESUp_eta", &recoPFJetCorrectedJESUpEta_);
    recoT->Branch("reco_PF_jet_corr_JESUp_phi", &recoPFJetCorrectedJESUpPhi_);
    recoT->Branch("reco_PF_jet_corr_JESDown_pt", &recoPFJetCorrectedJESDownPt_);
    recoT->Branch("reco_PF_jet_corr_JESDown_eta", &recoPFJetCorrectedJESDownEta_);
    recoT->Branch("reco_PF_jet_corr_JESDown_phi", &recoPFJetCorrectedJESDownPhi_);
    recoT->Branch("reco_PF_jet_corr_JERUp_pt", &recoPFJetCorrectedJERUpPt_);
    recoT->Branch("reco_PF_jet_corr_JERUp_eta", &recoPFJetCorrectedJERUpEta_);
    recoT->Branch("reco_PF_jet_corr_JERUp_phi", &recoPFJetCorrectedJERUpPhi_);
    recoT->Branch("reco_PF_jet_corr_JERDown_pt", &recoPFJetCorrectedJERDownPt_);
    recoT->Branch("reco_PF_jet_corr_JERDown_eta", &recoPFJetCorrectedJERDownEta_);
    recoT->Branch("reco_PF_jet_corr_JERDown_phi", &recoPFJetCorrectedJERDownPhi_);
    recoT->Branch("reco_PF_HEM_flag", &recoPFHEMFlag_);
    recoT->Branch("reco_MHT_Pt", &MHTPt_);

    if (!isData) {

        genT = fs->make<TTree>("genT", "");

        genT->Branch("event_num", &eventNum_);
        genT->Branch("gen_pu_obs", &genpuobs_);
        genT->Branch("gen_pu_true", &genputrue_);
        genT->Branch("gen_wgt", &genwgt_);
        genT->Branch("gen_ID", &genID_);
        //genT->Branch("gen_hard_process", &genHardProcess_);
        genT->Branch("gen_charge", &genCharge_);
        genT->Branch("gen_pt", &genPt_);
        genT->Branch("gen_eta", &genEta_);
        genT->Branch("gen_phi", &genPhi_);
        genT->Branch("gen_pz", &genPz_);
        genT->Branch("gen_energy", &genEn_);
        genT->Branch("gen_vxy", &genVxy_);
        genT->Branch("gen_vz", &genVz_);
        genT->Branch("gen_mass", &genMass_);
        genT->Branch("gen_jet_pt", &genJetPt_);
        genT->Branch("gen_jet_eta", &genJetEta_);
        genT->Branch("gen_jet_phi", &genJetPhi_);
        genT->Branch("gen_MET_pt", &genLeadMETPt_);
        genT->Branch("gen_MET_phi", &genLeadMETPhi_);
    }
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
    iEvent.getByToken(dsaRecoMuToken_, dsaRecoMuHandle_);
    if (!dsaRecoMuHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "dsaRecoMu");
        return false;
    }
    iEvent.getByToken(dsaRecoMuTimingToken_, dsaRecoMuTimingHandle_);
    if (!dsaRecoMuTimingHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "dsaRecoMuTiming");
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
    iEvent.getByToken(recoPFMETToken_, recoPFMETHandle_);
    if (!recoPFMETHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "PFMET");
        return false;
    }
    iEvent.getByToken(recoCaloMETToken_, recoCaloMETHandle_);
    if (!recoCaloMETHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "CaloMET");
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
    triggerPathsWithoutVersionNum_.clear();
    triggerPathsWithVersionNum_.clear();
    trigExist_.clear();

    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET120_PFMHT120_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET130_PFMHT130_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET140_PFMHT140_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu130_PFMHTNoMu130_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu140_PFMHTNoMu140_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMET120_PFMHT120_IDTight_PFHT60");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_IsoMu27");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DCA_PFMET50_PFMHT60");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET50_PFMHT60");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET70_PFMHT70");
    triggerPathsWithoutVersionNum_.emplace_back("HLT_DoubleMu3_DZ_PFMET90_PFMHT90");
    
    const std::vector<std::string>& pathNames = hltConfig_.triggerNames();
    for (auto trigPathNoVersion : triggerPathsWithoutVersionNum_) {
        auto matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVersion));
        if (matchedPaths.size() == 0) {
            //LogError("TriggerError") << "Could not find matched full trigger path with -> " << trigPathNoVersion;
            triggerPathsWithVersionNum_.push_back("None");
            trigExist_.push_back(false);
        }
        else {
            trigExist_.push_back(true);
            triggerPathsWithVersionNum_.push_back(matchedPaths[0]);
            if (hltConfig_.triggerIndex(matchedPaths[0]) >= hltConfig_.size()) {
                LogError("TriggerError") << "Cannot find trigger path -> " << matchedPaths[0];
                return false;
            }
        }
    }
    
    iEvent.getByToken(HBHENoiseFilterResultProducerToken_, HBHENoiseFilterResultProducerHandle_);
    if (!HBHENoiseFilterResultProducerHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "HBHENoiseFilter");
        return false;
    }
    iEvent.getByToken(HBHEIsoNoiseFilterResultProducerToken_, HBHEIsoNoiseFilterResultProducerHandle_);
    if (!HBHEIsoNoiseFilterResultProducerHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "HBHEIsoNoiseFilter");
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
    iEvent.getByToken(mJetCorrectorToken_, jetCorrectorHandle_);
    if (!jetCorrectorHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "mJetCorrector");
        return false;
    }
    iEvent.getByToken(recoElectronToken_, recoElectronHandle_);
    if (!recoElectronHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoElectron");
        return false;
    }
    iEvent.getByToken(recoElectronIDToken_, recoElectronIDHandle_);
    if (!recoElectronIDHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoElectronID");
        return false;
    }
    iEvent.getByToken(recoPhotonToken_, recoPhotonHandle_);
    if (!recoPhotonHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoPhoton");
        return false;
    }
    iEvent.getByToken(recoPhotonIDToken_, recoPhotonIDHandle_);
    if (!recoPhotonIDHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "recoPhotonID");
        return false;
    }
    iEvent.getByToken(rhoToken_, rhoHandle_);
    if (!rhoHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "rho");
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
        iEvent.getByToken(genMETToken_, genMETHandle_);
        if (!genMETHandle_.isValid()) {
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
    if (!(*HBHEIsoNoiseFilterResultProducerHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 1);
    if (!(*primaryVertexFilterHandle_)) // primaryVertexFilter == 0 means event is bad (number of vertices)
        METFiltersFailBits_ |= (1 << 2);
    if (!(*globalSuperTightHalo2016FilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 3);
    if (!(*EcalDeadCellTriggerPrimitiveFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 4);
    if (!(*ecalBadCalibFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 5);
    if (!(*BadPFMuonFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 6);
    if (!(*muonBadTrackFilterHandle_)) // false means event is bad
        METFiltersFailBits_ |= (1 << 7);

    recoDSAPt_.clear();
    recoDSAPtError_.clear();
    recoDSAEta_.clear();
    recoDSAEtaError_.clear();
    recoDSAPhi_.clear();
    recoDSAPhiError_.clear();
    recoDSADxy_.clear();
    recoDSADxyError_.clear();
    recoDSADz_.clear();
    recoDSADzError_.clear();
    recoDSACharge_.clear();
    recoDSATrkChi2_.clear();
    recoDSATrkNumPlanes_.clear();
    recoDSATrkNumHits_.clear();
    recoDSATrkNumDTHits_.clear();
    recoDSATrkNumCSCHits_.clear();
    recoDSAInvBeta_.clear();
    recoDSAInvBetaErr_.clear();
    recoDSAFreeInvBeta_.clear();
    recoDSAFreeInvBetaErr_.clear();
    recoDSAtimeAtIpInOut_.clear();
    recoDSAtimeAtIpInOutErr_.clear();
    recoDSAtimeAtIpOutIn_.clear();
    recoDSAtimeAtIpOutInErr_.clear();
    recoDSAtimingNdof_.clear();
    recoDSAIdx0_ = -9999;
    recoDSAIdx1_ = -9999;
    recoGMPt_.clear();
    recoGMPtError_.clear();
    recoGMEta_.clear();
    recoGMEtaError_.clear();
    recoGMPhi_.clear();
    recoGMPhiError_.clear();
    recoGMDxy_.clear();
    recoGMDxyError_.clear();
    recoGMDz_.clear();
    recoGMDzError_.clear();
    recoGMCharge_.clear();
    recoGMTrkChi2_.clear();
    recoGMTrkNumPlanes_.clear();
    recoGMTrkNumHits_.clear();
    recoGMTrkNumDTHits_.clear();
    recoGMTrkNumCSCHits_.clear();
    dsadsa_recoVtxVxy_.clear();
    dsadsa_recoVtxVz_.clear();
    dsadsa_recoVtxSigmaVxy_.clear();
    dsadsa_recoVtxReducedChi2_.clear();
    dsadsa_recoVtxDr_.clear();
    gmgm_recoVtxVxy_.clear();
    gmgm_recoVtxVz_.clear();
    gmgm_recoVtxSigmaVxy_.clear();
    gmgm_recoVtxReducedChi2_.clear();
    gmgm_recoVtxDr_.clear();
    dsagm_recoVtxVxy_.clear();
    dsagm_recoVtxVz_.clear();
    dsagm_recoVtxSigmaVxy_.clear();
    dsagm_recoVtxReducedChi2_.clear();
    dsagm_recoVtxDr_.clear();
    recoGMdSAdR_.clear();
    recoGMdSAmatch_.clear();
    recoElectronPt_.clear();
    recoElectronEta_.clear();
    recoElectronPhi_.clear();
    recoElectronVxy_.clear();
    recoElectronVz_.clear();
    recoElectronCharge_.clear();
    recoElectronIDResult_.clear();
    recoPhotonPt_.clear();
    recoPhotonEta_.clear();
    recoPhotonPhi_.clear();
    recoPhotonIDResult_.clear();
    recoPFJetPt_.clear();
    recoPFJetEta_.clear();
    recoPFJetPhi_.clear();
    recoPFJetCorrectedPt_.clear();
    recoPFJetCorrectedEta_.clear();
    recoPFJetCorrectedPhi_.clear();
    recoPFJetCorrectedBTag_.clear();
    recoPFJetCorrectedCHEF_.clear();
    recoPFJetCorrectedNHEF_.clear();
    recoPFJetCorrectedCEEF_.clear();
    recoPFJetCorrectedNEEF_.clear();
    recoPFJetCorrectedNumDaughters_.clear();
    recoPFJetCorrectedChargedMultiplicity_.clear();
    recoPFJetCorrectedJESUpPt_.clear();
    recoPFJetCorrectedJESUpEta_.clear();
    recoPFJetCorrectedJESUpPhi_.clear();
    recoPFJetCorrectedJESDownPt_.clear();
    recoPFJetCorrectedJESDownEta_.clear();
    recoPFJetCorrectedJESDownPhi_.clear();
    recoPFJetCorrectedJERUpPt_.clear();
    recoPFJetCorrectedJERUpEta_.clear();
    recoPFJetCorrectedJERUpPhi_.clear();
    recoPFJetCorrectedJERDownPt_.clear();
    recoPFJetCorrectedJERDownEta_.clear();
    recoPFJetCorrectedJERDownPhi_.clear();
    recoPFHEMFlag_ = false;
    selectedMuonsPt_.clear();
    selectedMuonsPtError_.clear();
    selectedMuonsEta_.clear();
    selectedMuonsEtaError_.clear();
    selectedMuonsPhi_.clear();
    selectedMuonsPhiError_.clear();
    selectedMuonsDxy_.clear();
    selectedMuonsDxyError_.clear();
    selectedMuonsDz_.clear();
    selectedMuonsDzError_.clear();
    selectedMuonsCharge_.clear();

    fired_ = 0;
    recoPFMETPt_ = -9999;
    recoPFMETPhi_ = -9999;
    recoPFMETSmearingOnlyPt_ = -9999;
    recoPFMETSmearingOnlyPhi_ = -9999;
    recoPFMETCorrectedPt_ = -9999;
    recoPFMETCorrectedPhi_ = -9999;
    recoPFMETJESUpPt_ = -9999;
    recoPFMETJESUpPhi_ = -9999;
    recoPFMETJESDownPt_ = -9999;
    recoPFMETJESDownPhi_ = -9999;
    recoPFMETJERUpPt_ = -9999;
    recoPFMETJERUpPhi_ = -9999;
    recoPFMETJERDownPt_ = -9999;
    recoPFMETJERDownPhi_ = -9999;
    recoPFMETMuonEtFraction_ = -9999;
    recoCaloMETPt_ = -9999;
    recoCaloMETPhi_ = -9999;
    recoPFRecoilPt_ = -9999;
    recoPFRecoilPhi_ = -9999;
    recoMmumu_ = -9999;
    recoDeltaPhiMETMu_ = -9999;
    MHTPt_ = -9999;
    recoNMatchedGBMvDSA_ = -1;

    // get MET
    // assumes 0-th element of PFMET collection is largest pt (and only?) element
    // in other words, why is recoPFMETHandle_ even a vector?
    reco::PFMETRef PFMETr(recoPFMETHandle_, 0);
    recoPFMETPt_ = PFMETr->pt();
    recoPFMETPhi_ = PFMETr->phi();
    recoPFMETMuonEtFraction_ = PFMETr->MuonEtFraction();
    reco::CaloMETRef CaloMETr(recoCaloMETHandle_, 0);
    recoCaloMETPt_ = CaloMETr->pt();
    recoCaloMETPhi_ = CaloMETr->phi();
    
    // calculate MHT
    math::XYZTLorentzVector MHT;
    for (auto & jet : *recoJetHandle_) {
        if (jet.pt() < 30) continue;
        MHT += jet.p4();
    }
    MHTPt_ = MHT.pt();
    
    // As we go along, calculate recoil with electrons, photons, and GM that passed ID
    double recoil_px = PFMETr->px();
    double recoil_py = PFMETr->py();

    // Add all electrons to ntuple, regardless of ID
    // for now "good" electron means only ID is passed
    // i.e. (IDmap % 2 == 1)
    recoNElectron_ = recoElectronHandle_->size();
    recoNGoodElectron_ = 0;
    const edm::ValueMap<float> & eIDmap = *recoElectronIDHandle_;
    for (size_t i = 0; i < recoElectronHandle_->size(); i++) {
        reco::GsfElectronRef electronRef(recoElectronHandle_, i);
        if ((int)eIDmap[electronRef] % 2 == 1 && electronRef->pt() > 10 && abs(electronRef->eta() < 2.5)) {
            recoNGoodElectron_++;
            recoil_px += electronRef->px();
            recoil_py += electronRef->py();
        }
        recoElectronPt_.push_back(electronRef->pt());
        recoElectronEta_.push_back(electronRef->eta());
        recoElectronPhi_.push_back(electronRef->phi());
        recoElectronVxy_.push_back(electronRef->trackPositionAtVtx().rho());
        recoElectronVz_.push_back(electronRef->trackPositionAtVtx().z());
        recoElectronCharge_.push_back(electronRef->charge());
        recoElectronIDResult_.push_back((int)eIDmap[electronRef]);
    }

    // Also add all photons to ntuple, regardless of ID
    // Photon ID only produces 1 or 0
    recoNPhoton_ = recoPhotonHandle_->size();
    recoNGoodPhoton_ = 0;
    const edm::ValueMap<bool> & phIDmap = *recoPhotonIDHandle_;
    for (size_t i = 0; i < recoPhotonHandle_->size(); i++) {
        reco::PhotonRef photonRef(recoPhotonHandle_, i);
        if (phIDmap[photonRef] && photonRef->pt() > 15 && abs(photonRef->eta()) < 2.5) {
            recoNGoodPhoton_++;
            recoil_px += photonRef->px();
            recoil_py += photonRef->py();
        }
        recoPhotonPt_.push_back(photonRef->pt());
        recoPhotonEta_.push_back(photonRef->eta());
        recoPhotonPhi_.push_back(photonRef->phi());
        recoPhotonIDResult_.push_back(phIDmap[photonRef]);
    }

    // Assign each trigger result to a different bit
    fired_ = 0;
    for (size_t i = 0; i < triggerPathsWithVersionNum_.size(); i++) {
        if (trigExist_.at(i)) {
            std::string trigPath = triggerPathsWithVersionNum_[i];
            fired_ |= (trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath)) << i);
        }
        else {
            fired_ |= (0 <<i);
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
    recoNDSA_ = muTrackHandle1_->size();

    sort(muObjs1.begin(), muObjs1.end(), [](const auto & l, const auto & r) {
            reco::TrackRef lt = l->standAloneMuon();
            reco::TrackRef rt = r->standAloneMuon();
            return lt->pt() > rt->pt();
            });

    sort(muTracks1.begin(), muTracks1.end(), [](const auto & l, const auto & r) {
            return l->pt() > r->pt();
            });

    // Sort global muons (note that muon collection is *not* sorted by pT at first)
    recoNGM_ = muTrackHandle2_->size();

    vector<reco::TrackRef> muTracks2{};
    for (size_t i = 0; i < muTrackHandle2_->size(); i++) 
        muTracks2.emplace_back(muTrackHandle2_, i);

    sort(muTracks2.begin(), muTracks2.end(), [](const auto & l, const auto & r) {
           return l->pt() > r->pt();
           });

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
    recoNGoodDSA_ = muGoodTracksIdx.size();

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
    recoNGoodGM_ = muGoodTracksIdx2.size();
    
    // Add leading 4 pt muons
    for (size_t i = 0; i < 4; i++) {
        if (i >= muTracks1.size()) break;
        reco::TrackRef mu_i = muTracks1[i];

        recoDSAPt_.push_back(mu_i->pt());
        recoDSAPtError_.push_back(mu_i->ptError());
        recoDSAEta_.push_back(mu_i->eta());
        recoDSAEtaError_.push_back(mu_i->etaError());
        recoDSAPhi_.push_back(mu_i->phi());
        recoDSAPhiError_.push_back(mu_i->phiError());
        recoDSADxy_.push_back(mu_i->dxy());
        recoDSADxyError_.push_back(mu_i->dxyError());
        recoDSADz_.push_back(mu_i->dz());
        recoDSADzError_.push_back(mu_i->dzError());
        recoDSACharge_.push_back(mu_i->charge());
        recoDSATrkChi2_.push_back(mu_i->normalizedChi2());
        recoDSATrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        recoDSATrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
        recoDSATrkNumDTHits_.push_back(mu_i->hitPattern().numberOfValidMuonDTHits());
        recoDSATrkNumCSCHits_.push_back(mu_i->hitPattern().numberOfValidMuonCSCHits());
        
        // add muon timing info from custom-built muon object
        reco::MuonRef muon_i = muObjs1[i];

        reco::MuonTimeExtra time_info = (*dsaRecoMuTimingHandle_)[muon_i];
        recoDSAInvBeta_.push_back(time_info.inverseBeta());
        recoDSAInvBetaErr_.push_back(time_info.inverseBeta());
        recoDSAFreeInvBeta_.push_back(time_info.freeInverseBeta());
        recoDSAFreeInvBetaErr_.push_back(time_info.freeInverseBetaErr());
        recoDSAtimeAtIpInOut_.push_back(time_info.timeAtIpInOut());
        recoDSAtimeAtIpInOutErr_.push_back(time_info.timeAtIpInOutErr());
        recoDSAtimeAtIpOutIn_.push_back(time_info.timeAtIpOutIn());
        recoDSAtimeAtIpOutInErr_.push_back(time_info.timeAtIpOutInErr());
        recoDSAtimingNdof_.push_back(time_info.nDof());
    }
    // Add leading 4 pt muons
    for (size_t i = 0; i < 4; i++) {
        if (i >= muTracks2.size()) break;
        reco::TrackRef mu_i = muTracks2[i];

        recoGMPt_.push_back(mu_i->pt());
        recoGMPtError_.push_back(mu_i->ptError());
        recoGMEta_.push_back(mu_i->eta());
        recoGMEtaError_.push_back(mu_i->etaError());
        recoGMPhi_.push_back(mu_i->phi());
        recoGMPhiError_.push_back(mu_i->phiError());
        recoGMDxy_.push_back(mu_i->dxy());
        recoGMDxyError_.push_back(mu_i->dxyError());
        recoGMDz_.push_back(mu_i->dz());
        recoGMDzError_.push_back(mu_i->dzError());
        recoGMCharge_.push_back(mu_i->charge());
        recoGMTrkChi2_.push_back(mu_i->normalizedChi2());
        recoGMTrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        recoGMTrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
        recoGMTrkNumDTHits_.push_back(mu_i->hitPattern().numberOfValidMuonDTHits());
        recoGMTrkNumCSCHits_.push_back(mu_i->hitPattern().numberOfValidMuonCSCHits());
        recoil_px += mu_i->px();
        recoil_py += mu_i->py();
    }

    // Calculate recoil pt after looking at all collections
    recoPFRecoilPt_ = sqrt(recoil_px*recoil_px + recoil_py*recoil_py);
    recoPFRecoilPhi_ = atan2(recoil_py, recoil_px);

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
            recoPFHEMFlag_ = true;
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
        double jec = jetCorrectorHandle_->correction(*jet_i);
        std::unique_ptr<reco::PFJet> corr_jet_i(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jes_up(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jes_down(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jer_up(jet_i->clone());
        std::unique_ptr<reco::PFJet> corr_jet_i_jer_down(jet_i->clone());
        // For MET corrections due to jet smearing
        std::unique_ptr<reco::PFJet> corr_jet_i_jer_only(jet_i->clone());

        corr_jet_i->scaleEnergy(jec);
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
                    if (dPt > 3 * corr_jet_i->pt() * jet_resolution)
                        continue;
                    min_dR = dR;
                    matched_genJet = &genJet;
                }
            }
            if (matched_genJet) {
                double dPt = corr_jet_i->pt() - matched_genJet->pt();
                smearFactor = 1 + (jer_sf - 1.) * dPt / corr_jet_i->pt();
                smearFactor_up = 1 + (jer_sf_up - 1.) * dPt / corr_jet_i->pt();
                smearFactor_down = 1 + (jer_sf_down - 1.) * dPt / corr_jet_i->pt();
            }
            else if (jer_sf > 1) { 
                double sigma = jet_resolution * std::sqrt(jer_sf * jer_sf - 1);
                std::normal_distribution<> d(0, sigma);
                smearFactor = 1. + d(m_random_generator);
            }
            //else
            //   std::cout << "ERROR! Impossible to smear this jet. jer_sf: " << jer_sf << std::endl;
            if (jer_sf_up >= 1.0) {
                double sigma_up = jet_resolution * std::sqrt(jer_sf_up * jer_sf_up - 1);
                std::normal_distribution<> d_up(0, sigma_up);
                smearFactor_up = 1. + d_up(m_random_generator);
            }
            if (jer_sf_down >= 1.0) {
                double sigma_down = jet_resolution * std::sqrt(jer_sf_down * jer_sf_down - 1);
                std::normal_distribution<> d_down(0, sigma_down);
                smearFactor_down = 1. + d_down(m_random_generator);
            }

            if (corr_jet_i->energy() * smearFactor < 0.01) {
                double newSmearFactor = 0.01 / corr_jet_i->energy();
                //std::cout << "The smearing factor (" << smearFactor << ") is either negative or too small. Fixing it to " << newSmearFactor << " to avoid change of direction." << std::endl;
                smearFactor = newSmearFactor;
            }
        } // end if checking for isData

        corr_jet_i->scaleEnergy(smearFactor);
        corr_jet_i_jes_up->scaleEnergy(smearFactor);
        corr_jet_i_jes_down->scaleEnergy(smearFactor);
        corr_jet_i_jer_up->scaleEnergy(smearFactor_up);
        corr_jet_i_jer_down->scaleEnergy(smearFactor_down);
        corr_jet_i_jer_only->scaleEnergy(smearFactor);

        // Before finishing, compute the MET correction due to JER alone (MET type-I already accounts for JEC)
        corr_METpx -= (corr_jet_i_jer_only->px() - jet_i->px());
        corr_METpy -= (corr_jet_i_jer_only->py() - jet_i->py());
        
        // Now temporarily "un-apply" smear factor to get JEC-corrected-only jet that was used in the original type-I calculation of MET
        corr_jet_i->scaleEnergy(1.0/smearFactor);
        corr_METpx_JESUp -= (corr_jet_i_jes_up->px() - corr_jet_i->px());
        corr_METpy_JESUp -= (corr_jet_i_jes_up->py() - corr_jet_i->py());
        corr_METpx_JESDown -= (corr_jet_i_jes_down->px() - corr_jet_i->px());
        corr_METpy_JESDown -= (corr_jet_i_jes_down->py() - corr_jet_i->py());
        corr_METpx_JERUp -= (corr_jet_i_jer_up->px() - corr_jet_i->px());
        corr_METpy_JERUp -= (corr_jet_i_jer_up->py() - corr_jet_i->py());
        corr_METpx_JERDown -= (corr_jet_i_jer_down->px() - corr_jet_i->px());
        corr_METpy_JERDown -= (corr_jet_i_jer_down->py() - corr_jet_i->py());
        corr_jet_i->scaleEnergy(smearFactor);

        correctedJets.push_back(std::make_pair(std::move(corr_jet_i), i));
        correctedJetsJESUp.push_back(std::make_pair(std::move(corr_jet_i_jes_up), i));
        correctedJetsJESDown.push_back(std::make_pair(std::move(corr_jet_i_jes_down), i));
        correctedJetsJERUp.push_back(std::make_pair(std::move(corr_jet_i_jer_up), i));
        correctedJetsJERDown.push_back(std::make_pair(std::move(corr_jet_i_jer_down), i));
    }

    // Before sorting the jet collections by pT, calculate MET corrections for each as well
    recoPFMETSmearingOnlyPt_ = std::sqrt(corr_METpx*corr_METpx + corr_METpy*corr_METpy);
    recoPFMETSmearingOnlyPhi_ = atan2(corr_METpy, corr_METpx);
    recoPFMETCorrectedPt_ = recoPFMETSmearingOnlyPt_;
    recoPFMETCorrectedPhi_ = recoPFMETSmearingOnlyPhi_;
    recoPFMETJESUpPt_ = std::sqrt(corr_METpx_JESUp*corr_METpx_JESUp + corr_METpy_JESUp*corr_METpy_JESUp);
    recoPFMETJESUpPhi_ = atan2(corr_METpy_JESUp, corr_METpx_JESUp);
    recoPFMETJESDownPt_ = std::sqrt(corr_METpx_JESDown*corr_METpx_JESDown + corr_METpy_JESDown*corr_METpy_JESDown);
    recoPFMETJESDownPhi_ = atan2(corr_METpy_JESDown, corr_METpx_JESDown);
    recoPFMETJERUpPt_ = std::sqrt(corr_METpx_JERUp*corr_METpx_JERUp + corr_METpy_JERUp*corr_METpy_JERUp);
    recoPFMETJERUpPhi_ = atan2(corr_METpy_JERUp, corr_METpx_JERUp);
    recoPFMETJERDownPt_ = std::sqrt(corr_METpx_JERDown*corr_METpx_JERDown + corr_METpy_JERDown*corr_METpy_JERDown);
    recoPFMETJERDownPhi_ = atan2(corr_METpy_JERDown, corr_METpx_JERDown);

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
    recoPFNJet_ = recoJetHandle_->size(); 
    recoPFNPassIDJet_ = 0;
    recoPFNHighPtJet_ = 0;
    
    for (size_t i = 0; i < correctedJets.size(); i++) {
        size_t index = correctedJets[i].second;
        reco::PFJet & jet_i = *(correctedJets[i].first);

        // Exclude jets that didn't pass ID above (ID checked against uncorrected jets)
        if (!jetIDResults[index]) continue;
        recoPFNPassIDJet_++;

        // Use JES+JER-corrected quantities
        if (jet_i.pt() > 30) 
            recoPFNHighPtJet_++;

        if (recoPFJetCorrectedPt_.size() < 10 && jet_i.pt() > 30) {
            recoPFJetCorrectedPt_.push_back(jet_i.pt());
            recoPFJetCorrectedEta_.push_back(jet_i.eta());
            recoPFJetCorrectedPhi_.push_back(jet_i.phi());
            // TODO: figure out how BtagProbb(b) collections actually behave
            // FIXME this might be problematic with the jet corrections, keep in mind
            if (bTagProbbHandle_->size() > i && bTagProbbbHandle_->size() > i)
                recoPFJetCorrectedBTag_.push_back((*bTagProbbHandle_)[index].second + (*bTagProbbbHandle_)[index].second);
            else 
                recoPFJetCorrectedBTag_.push_back(-9999);
            recoPFJetCorrectedCHEF_.push_back(jet_i.chargedHadronEnergyFraction());
            recoPFJetCorrectedNHEF_.push_back(jet_i.neutralHadronEnergyFraction());
            recoPFJetCorrectedCEEF_.push_back(jet_i.chargedEmEnergyFraction());
            recoPFJetCorrectedNEEF_.push_back(jet_i.neutralEmEnergyFraction());
            recoPFJetCorrectedNumDaughters_.push_back(jet_i.numberOfDaughters());
            recoPFJetCorrectedChargedMultiplicity_.push_back(jet_i.chargedMultiplicity());
        }
        // Add pt, eta, phi information for syst. uncert. collections
        jet_i = *(correctedJetsJESUp[i].first);
        if (recoPFJetCorrectedJESUpPt_.size() < 10 && jet_i.pt() > 30) {
            recoPFJetCorrectedJESUpPt_.push_back(jet_i.pt());
            recoPFJetCorrectedJESUpEta_.push_back(jet_i.eta());
            recoPFJetCorrectedJESUpPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJESDown[i].first);
        if (recoPFJetCorrectedJESDownPt_.size() < 10 && jet_i.pt() > 30) {
            recoPFJetCorrectedJESDownPt_.push_back(jet_i.pt());
            recoPFJetCorrectedJESDownEta_.push_back(jet_i.eta());
            recoPFJetCorrectedJESDownPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJERUp[i].first);
        if (recoPFJetCorrectedJERUpPt_.size() < 10 && jet_i.pt() > 30) {
            recoPFJetCorrectedJERUpPt_.push_back(jet_i.pt());
            recoPFJetCorrectedJERUpEta_.push_back(jet_i.eta());
            recoPFJetCorrectedJERUpPhi_.push_back(jet_i.phi());
        }
        jet_i = *(correctedJetsJERDown[i].first);
        if (recoPFJetCorrectedJERDownPt_.size() < 10 && jet_i.pt() > 30) {
            recoPFJetCorrectedJERDownPt_.push_back(jet_i.pt());
            recoPFJetCorrectedJERDownEta_.push_back(jet_i.eta());
            recoPFJetCorrectedJERDownPhi_.push_back(jet_i.phi());
        }
        // Also add uncorrected jet info for completeness
        reco::PFJetRef jet_ii(recoJetHandle_, i);
        if (recoPFJetPt_.size() < 10 && jet_ii->pt() > 30) {
            recoPFJetPt_.push_back(jet_ii->pt());
            recoPFJetEta_.push_back(jet_ii->eta());
            recoPFJetPhi_.push_back(jet_ii->phi());
        }
    }

    // Pick pair of muons with smallest vertex chi square fit for all collection combos
    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
    KalmanVertexFitter kvf(true);

    // dSA-dSA
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            reco::TrackRef muon_i, muon_j;
            if (i < muTracks1.size())
                muon_i = muTracks1[i];
            if (j < muTracks1.size())
                muon_j = muTracks1[j];

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
                sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
                vtx_chi2 = vertex.normalizedChi2();
                vz = vertex.z();
                dr = reco::deltaR(*muon_i, *muon_j);
            }

            dsadsa_recoVtxReducedChi2_.push_back(vtx_chi2);
            dsadsa_recoVtxVxy_.push_back(vxy);
            dsadsa_recoVtxSigmaVxy_.push_back(sigma_vxy);
            dsadsa_recoVtxVz_.push_back(vz);
            dsadsa_recoVtxDr_.push_back(dr);
        }
    }
    // GM-GM
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            reco::TrackRef muon_i, muon_j;
            if (i < muTracks2.size())
                muon_i = muTracks2[i];
            if (j < muTracks2.size())
                muon_j = muTracks2[j];

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
                sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
                vtx_chi2 = vertex.normalizedChi2();
                vz = vertex.z();
                dr = reco::deltaR(*muon_i, *muon_j);
            }

            gmgm_recoVtxReducedChi2_.push_back(vtx_chi2);
            gmgm_recoVtxVxy_.push_back(vxy);
            gmgm_recoVtxSigmaVxy_.push_back(sigma_vxy);
            gmgm_recoVtxVz_.push_back(vz);
            gmgm_recoVtxDr_.push_back(dr);
        }
    }
    // dSA-GM
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            reco::TrackRef muon_i, muon_j;
            if (i < muTracks1.size())
                muon_i = muTracks1[i];
            if (j < muTracks2.size())
                muon_j = muTracks2[j];

            TransientVertex tv;
            if (muon_i.isNonnull() && muon_j.isNonnull()) {
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
                sigma_vxy = (1/vxy)*(vertex.x()*vertex.xError() + vertex.y()*vertex.yError());
                vtx_chi2 = vertex.normalizedChi2();
                vz = vertex.z();
                dr = reco::deltaR(*muon_i, *muon_j);
            }

            dsagm_recoVtxReducedChi2_.push_back(vtx_chi2);
            dsagm_recoVtxVxy_.push_back(vxy);
            dsagm_recoVtxSigmaVxy_.push_back(sigma_vxy);
            dsagm_recoVtxVz_.push_back(vz);
            dsagm_recoVtxDr_.push_back(dr);
        }
    }

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

            recoGMdSAdR_.push_back(dr);
            if (dr < -9998)
                recoGMdSAmatch_.push_back(-1);
            else if (dr < 0.1)
                recoGMdSAmatch_.push_back(1);
            else
                recoGMdSAmatch_.push_back(0);
        }
    }

    recoPFMETCorrectedPt_ = std::sqrt(corr_METpx*corr_METpx + corr_METpy*corr_METpy);
    recoPFMETCorrectedPhi_ = atan2(corr_METpy, corr_METpx);
    recoPFMETJESUpPt_ = std::sqrt(corr_METpx_JESUp*corr_METpx_JESUp + corr_METpy_JESUp*corr_METpy_JESUp);
    recoPFMETJESUpPhi_ = atan2(corr_METpy_JESUp, corr_METpx_JESUp);
    recoPFMETJESDownPt_ = std::sqrt(corr_METpx_JESDown*corr_METpx_JESDown + corr_METpy_JESDown*corr_METpy_JESDown);
    recoPFMETJESDownPhi_ = atan2(corr_METpy_JESDown, corr_METpx_JESDown);
    recoPFMETJERUpPt_ = std::sqrt(corr_METpx_JERUp*corr_METpx_JERUp + corr_METpy_JERUp*corr_METpy_JERUp);
    recoPFMETJERUpPhi_ = atan2(corr_METpy_JERUp, corr_METpx_JERUp);
    recoPFMETJERDownPt_ = std::sqrt(corr_METpx_JERDown*corr_METpx_JERDown + corr_METpy_JERDown*corr_METpy_JERDown);
    recoPFMETJERDownPhi_ = atan2(corr_METpy_JERDown, corr_METpx_JERDown);

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

        nGen_ = (int)genParticleHandle_->size();
        genID_.clear();
        //genHardProcess_.clear();
        genCharge_.clear();
        genPt_.clear();
        genEta_.clear();
        genPhi_.clear();
        genPz_.clear();
        genEn_.clear();
        genVxy_.clear();
        genVz_.clear();
        genMass_.clear();
        genJetPt_.clear();
        genJetEta_.clear();
        genJetPhi_.clear();

        // Pile-up and event genweight
        genpuobs_ = -9999;
        genputrue_ = -9999;
        genwgt_ = -9999;
        genLeadMETPt_ = -9999;
        genLeadMETPhi_ = -9999;

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

        for (size_t i = 0; i < genParticleHandle_->size(); i++) {
            reco::GenParticleRef genParticle(genParticleHandle_, i);
            if (!genParticle->isHardProcess()) continue;
            genID_.push_back(genParticle->pdgId());
            //genHardProcess_.push_back(genParticle->isHardProcess());
            genCharge_.push_back(genParticle->charge());
            genPt_.push_back(genParticle->pt());
            genEta_.push_back(genParticle->eta());
            genPhi_.push_back(genParticle->phi());
            genPz_.push_back(genParticle->pz());
            genEn_.push_back(genParticle->energy());
            genVxy_.push_back(genParticle->vertex().rho());
            genVz_.push_back(genParticle->vz());
            genMass_.push_back(genParticle->mass());
        }

        // all gen jets
        for (size_t i = 0; i < genJetHandle_->size(); i++) {
            reco::GenJetRef jetRef(genJetHandle_, i);
            genJetPt_.push_back(jetRef->pt());
            genJetEta_.push_back(jetRef->eta());
            genJetPhi_.push_back(jetRef->phi());
        }

        // Lead gen MET
        genLeadMETPt_ = -9999;
        genLeadMETPhi_ = -9999;
        if (genMETHandle_->size() > 0) {
            reco::GenMETRef metRef(genMETHandle_, 0);
            genLeadMETPt_ = metRef->pt();
            genLeadMETPhi_ = metRef->phi();
        }

        genT->Fill();
    }

    //
    recoT->Fill();

    return;
}

void iDMAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void iDMAnalyzer::endJob() {}
