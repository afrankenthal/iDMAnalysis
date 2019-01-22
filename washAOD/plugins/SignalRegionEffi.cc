#include "Firefighter/washAOD/interface/SignalRegionEffi.h"

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

SignalRegionEffi::SignalRegionEffi(const edm::ParameterSet& ps) :
    muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
    genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
    genJetTag_(ps.getParameter<edm::InputTag>("genJet")),
    genMetTag_(ps.getParameter<edm::InputTag>("genMet")),
    recoMetTag_(ps.getParameter<edm::InputTag>("recoMet")),
    recoJetTag_(ps.getParameter<edm::InputTag>("recoJet")),
    
    muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
    genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_)),
    genJetToken_(consumes<reco::GenJetCollection>(genJetTag_)),
    genMetToken_(consumes<reco::GenMETCollection>(genMetTag_)),
    recoMetToken_(consumes<reco::PFMETCollection>(recoMetTag_)),
    recoJetToken_(consumes<reco::PFJetCollection>(recoJetTag_))
{
    usesResource("TFileService");
}

SignalRegionEffi::~SignalRegionEffi() = default;

void SignalRegionEffi::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    desc.add<edm::InputTag>("genJet", edm::InputTag("ak4GenJets"));
    desc.add<edm::InputTag>("genMet", edm::InputTag("genMetTrue"));
    desc.add<edm::InputTag>("recoMet", edm::InputTag("pfMet"));
    desc.add<edm::InputTag>("recoJet", edm::InputTag("ak4PFJets"));
    descriptions.add("SignalRegionEffi", desc);
}

void SignalRegionEffi::beginJob()
{
    for (int i = 0; i < 7; i++) {
        std::stringstream cutlabel; cutlabel << "cut" << i;
        auto temp = cutlabel.str();
        cutsTree.push_back(fs->make<TTree>(temp.c_str(), ""));
    }

    for (int i = 0; i < 7; i++) {
        cutsTree[i]->Branch("nMatched", &nMatched_, "nMatched/i");
        cutsTree[i]->Branch("recoPt",  &recoPt_);
        cutsTree[i]->Branch("recoEta", &recoEta_);
        cutsTree[i]->Branch("recoPhi", &recoPhi_);
        cutsTree[i]->Branch("recoDxy", &recoDxy_);
        cutsTree[i]->Branch("recoDz",  &recoDz_);
        cutsTree[i]->Branch("recoVxy", &recoVxy_);
        cutsTree[i]->Branch("recoVz",  &recoVz_);
        cutsTree[i]->Branch("deltaR",  &deltaR_);
        cutsTree[i]->Branch("recoPFMetPt", &recoPFMetPt_, "recoPFMetPt/F");
        cutsTree[i]->Branch("recoPFJetPt", &recoPFJetPt_, "recoPFJetPt/F");
        cutsTree[i]->Branch("MHTPt", &MHTPt_, "MHTPt/F");
    }
}

void SignalRegionEffi::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;

    iEvent.getByToken(muTrackToken_, muTrackHandle_);
    if (!muTrackHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting muTrack product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(genParticleToken_, genParticleHandle_);
    if (!genParticleHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting genParticle product from Event!"
            << endl;
        return;
    }
    iEvent.getByToken(genJetToken_, genJetHandle_);
    if (!genJetHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting genJet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(genMetToken_, genMetHandle_);
    if (!genMetHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting genMet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoMetToken_, recoMetHandle_);
    if (!recoMetHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting recoMet product from Event!" << endl;
        return;
    }
    iEvent.getByToken(recoJetToken_, recoJetHandle_);
    if (!recoJetHandle_.isValid()) {
        LogVerbatim("SignalRegionEffi") << "SignalRegionEffi::analyze: Error in getting recoJet product from Event!" << endl;
        return;
    }

    recoPt_ .clear();
    recoEta_.clear();
    recoPhi_.clear();
    recoDxy_.clear();
    recoDz_ .clear();
    recoVxy_.clear();
    recoVz_ .clear();
    deltaR_ .clear();

    // get MET
    // assumes 0-th element of recoMet collection is largest pt (and only?) element

    reco::PFMETRef recoMetr(recoMetHandle_, 0);
    recoPFMetPt_ = recoMetr->pt();
    
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
    int jetMaxPtIndex = 0;
    for (size_t i = 0; i < recoJetHandle_->size(); ++i) {
        reco::PFJetRef jetTempRef(recoJetHandle_, i);
        if (jetTempRef->pt() > maxJetPt) {
            secondMaxJetPt = maxJetPt;
            maxJetPt = jetTempRef->pt();
            jetMaxPtIndex = i;
        }
        else if (jetTempRef->pt() > secondMaxJetPt) {
            secondMaxJetPt = jetTempRef->pt();
        }
        else if (jetTempRef->pt() > 30) {
            flagHighPtJets = true;
        }
    }
    if (recoJetHandle_->size() > 0) {
        reco::PFJetRef jetMaxPtRef(recoJetHandle_, jetMaxPtIndex);
        recoPFJetPt_ = jetMaxPtRef->pt();
    }

    // get 2 largest pt reco (dSA) muons

    double maxMuPt = 0, secondMaxMuPt = 0;
    int leadingMuIndex = 0, subleadingMuIndex = 0;
    for (size_t i = 0; i < muTrackHandle_->size(); ++i) {
        reco::TrackRef muTempRef(muTrackHandle_, i);
        if (muTempRef->pt() > maxMuPt) {
            secondMaxMuPt = maxMuPt;
            maxMuPt = muTempRef->pt();
            subleadingMuIndex = leadingMuIndex;
            leadingMuIndex = i;
        }
        else if (muTempRef->pt() > secondMaxMuPt) {
            secondMaxMuPt = muTempRef->pt();
            subleadingMuIndex = i;
        }
    }
    reco::TrackRef leadingMuRef(muTrackHandle_, leadingMuIndex);
    reco::TrackRef subleadingMuRef(muTrackHandle_, subleadingMuIndex);
    
    // vtxing between reco mu pair
    ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

    vector<reco::TransientTrack> t_trks{};
    KalmanVertexFitter kvf(true);
    TransientVertex tv;
    float _vxy = 0, _vz = 0, dR = 0;
    if (muTrackHandle_->size() > 1) {
        t_trks.push_back(theB->build(&leadingMuRef));
        t_trks.push_back(theB->build(&subleadingMuRef));
        tv = kvf.vertex(t_trks);
        if (!tv.isValid()) {
            std::cout << "Warning! TV is not valid!" << std::endl;
        }
        else {
            reco::Vertex _vtx = reco::Vertex(tv);
            _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
            _vz  = _vtx.z();

            dR = deltaR(*(leadingMuRef.get()), *(subleadingMuRef.get()));
        }
    }

    if (muTrackHandle_->size() > 1) {
        recoPt_ .push_back(leadingMuRef->pt());
        recoEta_.push_back(leadingMuRef->eta());
        recoPhi_.push_back(leadingMuRef->phi());
        recoDxy_.push_back(leadingMuRef->dxy());
        recoDz_ .push_back(leadingMuRef->dz());
        recoPt_ .push_back(subleadingMuRef->pt());
        recoEta_.push_back(subleadingMuRef->eta());
        recoPhi_.push_back(subleadingMuRef->phi());
        recoDxy_.push_back(subleadingMuRef->dxy());
        recoDz_ .push_back(subleadingMuRef->dz());
        recoVxy_.push_back(_vxy);
        recoVz_ .push_back(_vz);
        deltaR_ .push_back(dR);
    }

    // Fill everything before cuts (cutsTree[0])
    cutsTree[0]->Fill();

    // Trigger emulation: reco MET > 120 GeV
    if (recoPFMetPt_ > 120) {
        cutsTree[1]->Fill();
        //std::cout << "passed cut 1" << std::endl;
    }
    else
        return;

    // Trigger emulation: reco MHT > 120 GeV
    if (MHT.pt() > 120) {
        cutsTree[2]->Fill();
        //std::cout << "passed cut 2" << std::endl;
    }
    else
        return;

    // One leading reco jet w/ pT > 120 and only one extra jet w/ pT > 30 GeV.
    // If the conditions in the previous line are satisfied, does it follow
    // that MHT and leading jet are roughly back-to-back? I think so.
    // TODO: quantify this statement.
    if (maxJetPt > 120 && flagHighPtJets == false) {
        cutsTree[3]->Fill();
        //std::cout << "passed cut 3" << std::endl;
    }
    else
        return;

    // First muon has vertex between 1 mm and 30 cm and pt > 5 GeV
    if (muTrackHandle_->size() > 0) {
        if (leadingMuRef->pt() > 5 && std::sqrt(_vxy*_vxy + _vz*_vz) > 1
                && std::sqrt(_vxy*_vxy + _vz*_vz) < 300) {
            cutsTree[4]->Fill();
            //std::cout << "passed cut 4" << std::endl;
        }
        else {
            //std::cout << "Didn't pass cut 4, pt of leading mu: " << leadingMuRef->pt() << ", d0: " << std::sqrt(_vxy*_vxy + _vz*_vz) << std::endl;
            //std::cout << "muTrackHandle_->size(): " << muTrackHandle_->size() << std::endl;
            return;
        }
    }
    else
        return;

    // Second muon, same criterion plus check if vertex is valid
    if (muTrackHandle_->size() > 1) {
        if (subleadingMuRef->pt() > 5 && std::sqrt(_vxy*_vxy + _vz*_vz) > 1
                && std::sqrt(_vxy*_vxy + _vz*_vz) < 300 && tv.isValid()) {
            cutsTree[5]->Fill();
            //std::cout << "passed cut 5" << std::endl;
        }
        else
            return;
    }
    else
        return;

    // delta phi between MET and muon jet < 0.4, if it got to here
    // then there are at least 2 muons

    if (std::abs(recoMetr->phi() - (leadingMuRef->phi() + subleadingMuRef->phi())/2)
            < 0.4) {
        cutsTree[6]->Fill();
        //std::cout << "passed cut 6" << std::endl;
    }
    else return;

    return;
}

void SignalRegionEffi::endJob() {}
