#include "iDMSkimmer/washAOD/interface/iDMAnalyzer.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include <cmath>

iDMAnalyzer::iDMAnalyzer(const edm::ParameterSet& ps):
    muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
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
    
    muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_)),
    trigResultsToken_(consumes<edm::TriggerResults>(trigResultsTag_)),
    trigEventToken_(consumes<trigger::TriggerEvent>(trigEventTag_)),
    pileupInfosToken_(consumes<std::vector<PileupSummaryInfo> >(pileupInfosTag_)),
    genEvtInfoToken_(consumes<GenEventInfoProduct>(genEvtInfoTag_))
{
    usesResource("TFileService");
}

iDMAnalyzer::~iDMAnalyzer() = default;

void iDMAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
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
    for (int i = 0; i < numCuts_; i++)
        cutsVec[i] = 0;

    recoT = fs->make<TTree>("reco", "");

    recoT->Branch("trig_fired", &fired_);
    recoT->Branch("reco_mu_pt",  &recoPt_);
    recoT->Branch("reco_mu_eta", &recoEta_);
    recoT->Branch("reco_mu_phi", &recoPhi_);
    recoT->Branch("reco_mu_dxy", &recoDxy_);
    recoT->Branch("reco_mu_dz",  &recoDz_);
    recoT->Branch("reco_mu_trk_chi2", &recoTrkChi2_);
    recoT->Branch("reco_mu_trk_num_planes", &recoTrkNumPlanes_);
    recoT->Branch("reco_mu_trk_num_hits", &recoTrkNumHits_);
    recoT->Branch("reco_mu_M2",  &recoM2_);
    recoT->Branch("reco_num_mu", &recoNMu_);
    recoT->Branch("reco_num_good_mu", &recoNGoodMu_);
    recoT->Branch("reco_MET_mu_DeltaPhi", &recoDeltaPhiMetMu_);
    recoT->Branch("reco_vertex_vxy", &recoVxy_);
    recoT->Branch("reco_vertex_vz",  &recoVz_);
    recoT->Branch("reco_vertex_dR",  &recoDr_);
    recoT->Branch("reco_vertex_reducedChi2", &recoVtxReducedChi2_);
    recoT->Branch("reco_vertex_sigmavxy", &recoVtxSigmaVxy_);
    recoT->Branch("reco_PF_MET_pt", &recoPFMetPt_);
    recoT->Branch("reco_PF_MET_phi", &recoPFMetPhi_);
    recoT->Branch("reco_PF_jet_pt", &recoPFJetPt_);
    recoT->Branch("reco_PF_jet_eta", &recoPFJetEta_);
    recoT->Branch("reco_PF_jet_phi", &recoPFJetPhi_);
    recoT->Branch("reco_PF_num_jets", &recoPFNJet_);
    recoT->Branch("reco_PF_num_highPt_jets", &recoPFNHighPtJet_);
    recoT->Branch("reco_MHT_Pt", &MHTPt_);
    recoT->Branch("cutsVec", cutsVec, "cutsVec[16]/i");
    recoT->Branch("event_num", &event_);

    genT = fs->make<TTree>("gen", "");

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
    genT->Branch("event_num", &event_);
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
        
    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "muTrack");
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
    iEvent.getByToken(pileupInfosToken_, pileupInfosHandle_);
    if (!pileupInfosHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "pileupInfos");
        return false;
    }
    iEvent.getByToken(genEvtInfoToken_, genEvtInfoHandle_);
    if (!genEvtInfoHandle_.isValid()) {
        LogError("HandleError") << boost::str(boost::format(error_msg) % "genEventInfo");
        return false;
    }

    return true;
}


void iDMAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    getCollections(iEvent);

    const edm::EventAuxiliary& aux = iEvent.eventAuxiliary();
    event_ = aux.event();

    recoPt_ .clear();
    recoEta_.clear();
    recoPhi_.clear();
    recoDxy_.clear();
    recoDz_ .clear();
    recoTrkChi2_.clear();
    recoTrkNumPlanes_.clear();
    recoTrkNumHits_.clear();
    recoVxy_.clear();
    recoVtxSigmaVxy_.clear();
    recoVtxReducedChi2_.clear();
    recoVz_ .clear();
    recoDr_ .clear();
    recoVi_.clear();
    recoVj_.clear();
    recoPFJetPt_.clear();
    recoPFJetEta_.clear();
    recoPFJetPhi_.clear();

    fired_ = 0;
    recoPFMetPt_ = -9999;
    recoPFMetPhi_ = -9999;
    recoM2_ = -9999;
    recoDeltaPhiMetMu_ = -9999;
    MHTPt_ = -9999;


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

    // Get all jets info, sorted by pT
    //vector<reco::PFJetRef> PFJets{};
    //for (int i = 0; i < recoJetHandle_->size(); i++) {
    //    PFJets.emplace_back(recoJetHandle_, i);
    //}
    //sort(PFJets.begin(), PFJets.end(), [](const auto & l, const auto & r) { return l->pt() > r->pt(); } );
    //for (int i = 0; i < PFJets.size(); i++) {
    //    recoPFJetPt_.push_back(PFJets[i]->pt());
    //    recoPFJetEta_.push_back(PFJets[i]->eta());
    //    recoPFJetPhi_.push_back(PFJets[i]->phi());
    //}
    
    // Get all jets info, sorted by pT
    // Note that jet collection is already sorted by pT

    recoPFNJet_ = recoJetHandle_->size(); 
    recoPFNHighPtJet_ = 0;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        //if (i > 4) break;
        reco::PFJetRef jet_i(recoJetHandle_, i);
	if(jet_i->pt() > 30){recoPFNHighPtJet_++;}
	if(i<10){
        	recoPFJetPt_.push_back(jet_i->pt());
       		recoPFJetEta_.push_back(jet_i->eta());
       		recoPFJetPhi_.push_back(jet_i->phi());
	}
    }

    // Sort muons (note that muon collection is *not* sorted by pT at first)

    recoNMu_ = muTrackHandle_->size();

    vector<reco::TrackRef> muTracks{};
    for (size_t i = 0; i < muTrackHandle_->size(); i++) {
        muTracks.emplace_back(muTrackHandle_, i);
    }
    sort(muTracks.begin(), muTracks.end(), [](const auto & l, const auto & r){ return l->pt() > r->pt(); });

    // Create separate collection for good quality muons
    vector<int> muGoodTracksIdx{};
    for (size_t i = 0; i < muTracks.size(); i++) {
        if (muTracks[i]->hitPattern().muonStationsWithValidHits() < 2 ||
                muTracks[i]->hitPattern().numberOfValidMuonHits() < 12 ||
                muTracks[i]->normalizedChi2() > 10){
            continue;
	}
	reco::TrackRef mu_i(muTracks[i]);
	if(mu_i->pt() < 5){
	continue;
	}
        muGoodTracksIdx.push_back(i);
    }
    
    // Only add good muons info to ntuple
    for (size_t i = 0; i < muGoodTracksIdx.size(); i++) {
        reco::TrackRef mu_i(muTracks[muGoodTracksIdx[i]]);
        recoPt_.push_back(mu_i->pt());
        recoEta_.push_back(mu_i->eta());
        recoPhi_.push_back(mu_i->phi());
        recoDxy_.push_back(mu_i->dxy());
        recoDz_.push_back(mu_i->dz());
        recoTrkChi2_.push_back(mu_i->normalizedChi2());
        recoTrkNumPlanes_.push_back(mu_i->hitPattern().muonStationsWithValidHits());
        recoTrkNumHits_.push_back(mu_i->hitPattern().numberOfValidMuonHits());
    }

    recoNGoodMu_ = muGoodTracksIdx.size();
    
    // Calculate invariant mass of highest pT good quality muons
    if (muGoodTracksIdx.size() > 1) {
        reco::TrackRef leadingMuRef(muTrackHandle_, muGoodTracksIdx[0]);
        reco::TrackRef subleadingMuRef(muTrackHandle_, muGoodTracksIdx[1]);

        recoM2_ = 2 * leadingMuRef->pt() * subleadingMuRef->pt() *
            (cosh(leadingMuRef->eta() - subleadingMuRef->eta()) -
             cos(leadingMuRef->phi() - subleadingMuRef->phi()));
    }

    // get 2 largest pt reco (dSA) muons
    // add top 2 leading pt reco muons to vector, after minimum quality cut
    // Edit: reco muons that pass minimum quality cut
    // (to cut on weird 11 GeV peak muons due to cosmic seed in dSA algo)

    //double maxMuPt = 0, secondMaxMuPt = 0;
    //int leadingMuIndex = 0, subleadingMuIndex = 0;
    //int goodQualityTracks = 0;
    //for (size_t i = 0; i < muTrackHandle_->size(); ++i) {
    //    reco::TrackRef muTempRef(muTrackHandle_, i);
    //    if (muTempRef->hitPattern().muonStationsWithValidHits() < 2 ||
    //            muTempRef->hitPattern().numberOfValidMuonHits() < 12 ||
    //            muTempRef->normalizedChi2() > 10)
    //        continue;
    //    goodQualityTracks++;
    //    if (muTempRef->pt() > maxMuPt) {
    //        secondMaxMuPt = maxMuPt;
    //        maxMuPt = muTempRef->pt();
    //        subleadingMuIndex = leadingMuIndex;
    //        leadingMuIndex = i;
    //    }
    //    else if (muTempRef->pt() > secondMaxMuPt) {
    //        secondMaxMuPt = muTempRef->pt();
    //        subleadingMuIndex = i;
    //    }
    //}
    //reco::TrackRef leadingMuRef, subleadingMuRef;
    //if (goodQualityTracks > 0)
    //    leadingMuRef = reco::TrackRef(muTrackHandle_, leadingMuIndex);
    //if (goodQualityTracks > 1)
    //    subleadingMuRef = reco::TrackRef(muTrackHandle_, subleadingMuIndex);
    
    // Get vertex between pairs of muons
    // Get all pairs between 4 leading muons, if that many

    ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
    KalmanVertexFitter kvf(true);
    TransientVertex tv;

    for (size_t i = 0; i < min(muGoodTracksIdx.size()-1, (size_t)3); i++) {
        for (size_t j = i+1; j < min(muGoodTracksIdx.size(), (size_t)4); j++) {

            vector<reco::TransientTrack> trans_tracks{};
            trans_tracks.push_back(theB->build(muTracks[muGoodTracksIdx[i]]));
            trans_tracks.push_back(theB->build(muTracks[muGoodTracksIdx[j]]));
            tv = kvf.vertex(trans_tracks);
            if (!tv.isValid()) {
                LogWarning("VertexFittingFailed") << "Warning! TV is not valid!";
            }
            else {
                reco::Vertex vertex = reco::Vertex(tv);
                float vxy = sqrt( vertex.x()*vertex.x() + vertex.y()*vertex.y() );
                recoVxy_.push_back( vxy );
                float sigma_vxy = (1/vxy)*( vertex.x()*vertex.xError() + vertex.y()*vertex.yError() );
                recoVtxSigmaVxy_.push_back( sigma_vxy );
                recoVz_.push_back( vertex.z() );
                recoDr_.push_back( deltaR(*muTracks[muGoodTracksIdx[i]], *muTracks[muGoodTracksIdx[j]]) );
                recoVi_.push_back( muGoodTracksIdx[i] );
                recoVj_.push_back( muGoodTracksIdx[j] );
                recoVtxReducedChi2_.push_back( vertex.normalizedChi2() );
            }

        }
    }

    // Calculate delta phi between MET and leading good muons
    if (muGoodTracksIdx.size() > 1) {
        reco::TrackRef leadingMuRef(muTrackHandle_, muGoodTracksIdx[0]);
        reco::TrackRef subleadingMuRef(muTrackHandle_, muGoodTracksIdx[1]);
        double avgMuPhi = atan2( sin(leadingMuRef->phi()) + sin(subleadingMuRef->phi()), cos(leadingMuRef->phi()) + cos(subleadingMuRef->phi()) );
        double phiDiff = recoMetr->phi() - avgMuPhi;
        double reducedPhiDiff = phiDiff;
        if (abs(reducedPhiDiff) > 3.141592)
            reducedPhiDiff -= 2 * 3.141592 * reducedPhiDiff/abs(reducedPhiDiff);
        recoDeltaPhiMetMu_ = reducedPhiDiff;
    }

    //vector<reco::TransientTrack> t_trks{};
    //KalmanVertexFitter kvf(true);
    //TransientVertex tv;
    //float _vxy = 0, _vz = 0, dR = -10;
    //if (goodQualityTracks > 1) {
    //    t_trks.push_back(theB->build(&leadingMuRef));
    //    t_trks.push_back(theB->build(&subleadingMuRef));
    //    tv = kvf.vertex(t_trks);
    //    if (!tv.isValid()) {
    //        std::cout << "Warning! TV is not valid!" << std::endl;
    //    }
    //    else {
    //        reco::Vertex _vtx = reco::Vertex(tv);
    //        _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
    //        _vz  = _vtx.z();

    //        dR = deltaR(*(leadingMuRef), *(subleadingMuRef));
    //    }
    //}

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

    //if (goodQualityTracks > 0) {
    //    recoPt_ .push_back(leadingMuRef->pt());
    //    recoEta_.push_back(leadingMuRef->eta());
    //    recoPhi_.push_back(leadingMuRef->phi());
    //    recoDxy_.push_back(leadingMuRef->dxy());
    //    recoDz_ .push_back(leadingMuRef->dz());
    //    if (goodQualityTracks > 1) {
    //        recoPt_ .push_back(subleadingMuRef->pt());
    //        recoEta_.push_back(subleadingMuRef->eta());
    //        recoPhi_.push_back(subleadingMuRef->phi());
    //        recoDxy_.push_back(subleadingMuRef->dxy());
    //        recoDz_ .push_back(subleadingMuRef->dz());
    //        recoVxy_.push_back(_vxy);
    //        recoVz_ .push_back(_vz);
    //        recoDr_ .push_back(dR);
    //        //deltaR_ .push_back(dR);
    //    }
    //}


    // Trigger check firing bit (MET+MHT 120 GeV trigger)
    const vector<string>& pathNames = hltConfig_.triggerNames();
    const vector<string> matchedPaths(hltConfig_.restoreVersion(pathNames, trigPathNoVer_));
    if (matchedPaths.size() == 0) {
        LogError("TriggerError") << "Could not find matched full trigger path with -> " << trigPathNoVer_;
        return;
    }
    trigPath_ = matchedPaths[0];
    if (hltConfig_.triggerIndex(trigPath_) >= hltConfig_.size()) {
        LogError("TriggerError") << "Cannot find trigger path -> " << trigPath_;
        return;
    }
    fired_ = trigResultsHandle_->accept(hltConfig_.triggerIndex(trigPath_));


    /****** GEN INFO *******/

    // Pile-up and event genweight

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


    /******* BEGINNING OF CUTS ******/

    for (size_t i = 0; i < numCuts_; i++)
        cutsVec[i] = 0;

    // Fill everything before cuts 
    cutsVec[0] = 1;
    
    if (fired_)
        cutsVec[1] = 1;

    // One leading reco jet w/ pT > 120...

    if (recoPFJetPt_[0] > 120)
        cutsVec[2] = 1;
    
    // ...and only one extra jet w/ pT > 30 GeV
    
    int nHighPtJets = 0;
    for (size_t i = 1; i < recoPFJetPt_.size(); i++) {
        if (recoPFJetPt_[i] > 30)
            nHighPtJets++;
        if (nHighPtJets >= 2) break;
    }
    if (nHighPtJets < 2)
        cutsVec[3] = 1;

    // Muon cuts
    
    for (size_t i = 0; i < min(muGoodTracksIdx.size(), (size_t)2); i++) {
        int numMuCuts = 5;
        reco::TrackRef mu_tmp(muTracks[muGoodTracksIdx[i]]);

        if (mu_tmp->hitPattern().muonStationsWithValidHits() >= 2)
            cutsVec[4 + i*numMuCuts] = 1;
        if (mu_tmp->hitPattern().numberOfValidMuonHits() >= 12)
            cutsVec[5 + i*numMuCuts] = 1;
        if (mu_tmp->normalizedChi2() < 10)
            cutsVec[6 + i*numMuCuts] = 1;

        if (mu_tmp->pt() > 5)
            cutsVec[7 + i*numMuCuts] = 1;
        if (abs(mu_tmp->dxy()) > 0.1 && abs(mu_tmp->dxy()) < 700)
            cutsVec[8 + i*numMuCuts] = 1;
    }

    // Check vertex between highest pT good muons
    if (muGoodTracksIdx.size() > 1) 
        for (size_t k = 0; k < recoVi_.size(); k++) 
            if (recoVi_[k] == muGoodTracksIdx[0] && recoVj_[k] == muGoodTracksIdx[1])
                if (recoDr_[k] < 0.8)
                    cutsVec[14] = 1;
    
    // Check DeltaPhi between MET and leading muon pair
    if (muGoodTracksIdx.size() > 1)
        if (abs(recoDeltaPhiMetMu_) < 0.4)
            cutsVec[15] = 1;

    recoT->Fill();
    genT->Fill();

    return;
    
}

void iDMAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}

void iDMAnalyzer::endJob() {}
