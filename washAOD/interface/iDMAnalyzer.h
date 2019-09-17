#ifndef washAOD_iDMAnalyzer_H
#define washAOD_iDMAnalyzer_H

#include <boost/format.hpp>
#include <boost/any.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/BTauReco/interface/JetTag.h"
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
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TTree.h"

class iDMAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
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

        edm::Service<TFileService> fs;

        bool isData;

        // Tags
        const edm::InputTag bTagProbbTag_;
        const edm::InputTag bTagProbbbTag_;
        const edm::InputTag muTrackTag1_;
        const edm::InputTag muTrackTag2_;
        const edm::InputTag genParticleTag_;
        const edm::InputTag genJetTag_;
        const edm::InputTag genMetTag_;
        const edm::InputTag recoMetTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
        const std::string trigPathNoVer_;
        const edm::InputTag pileupInfosTag_;
        const edm::InputTag genEvtInfoTag_;
        const std::string processName_;
        const edm::InputTag HBHENoiseFilterResultProducerTag_;
        const edm::InputTag primaryVertexFilterTag_;
        const edm::InputTag globalSuperTightHalo2016FilterTag_;
        const edm::InputTag EcalDeadCellTriggerPrimitiveFilterTag_;
        const edm::InputTag ecalBadCalibFilterTag_;
        const edm::InputTag BadPFMuonFilterTag_;
        const edm::InputTag muonBadTrackFilterTag_;

        // Tokens
        const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbToken_;
        const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbbToken_;
        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken1_;
        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken2_;
        const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
        const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
        const edm::EDGetTokenT<reco::GenMETCollection> genMetToken_;
        const edm::EDGetTokenT<reco::PFMETCollection> recoMetToken_;
        const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
        const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
        const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;
        const edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupInfosToken_;
        const edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken_;
        const edm::EDGetTokenT<bool>HBHENoiseFilterResultProducerToken_;
        const edm::EDGetTokenT<int>primaryVertexFilterToken_;
        const edm::EDGetTokenT<bool>globalSuperTightHalo2016FilterToken_;
        const edm::EDGetTokenT<bool>EcalDeadCellTriggerPrimitiveFilterToken_;
        const edm::EDGetTokenT<bool>ecalBadCalibFilterToken_;
        const edm::EDGetTokenT<bool>BadPFMuonFilterToken_;
        const edm::EDGetTokenT<bool>muonBadTrackFilterToken_;

        // Handles
        edm::Handle<reco::JetTagCollection> bTagProbbHandle_;
        edm::Handle<reco::JetTagCollection> bTagProbbbHandle_;
        edm::Handle<reco::TrackCollection> muTrackHandle1_;
        edm::Handle<reco::TrackCollection> muTrackHandle2_;
        edm::Handle<reco::GenParticleCollection> genParticleHandle_;
        edm::Handle<reco::GenJetCollection> genJetHandle_;
        edm::Handle<reco::GenMETCollection> genMetHandle_;
        edm::Handle<reco::PFMETCollection> recoMetHandle_;
        edm::Handle<reco::PFJetCollection> recoJetHandle_;
        edm::Handle<edm::TriggerResults> trigResultsHandle_;
        edm::Handle<trigger::TriggerEvent> trigEventHandle_;
        edm::Handle<std::vector<PileupSummaryInfo>> pileupInfosHandle_;
        edm::Handle<GenEventInfoProduct> genEvtInfoHandle_;
        edm::Handle<bool> HBHENoiseFilterResultProducerHandle_;
        edm::Handle<int> primaryVertexFilterHandle_;
        edm::Handle<bool> globalSuperTightHalo2016FilterHandle_;
        edm::Handle<bool> EcalDeadCellTriggerPrimitiveFilterHandle_;
        edm::Handle<bool> ecalBadCalibFilterHandle_;
        edm::Handle<bool> BadPFMuonFilterHandle_;
        edm::Handle<bool> muonBadTrackFilterHandle_;
        
        std::string trigPath_;
        HLTConfigProvider hltConfig_;

        // Reco and gen TTrees
        TTree * recoT;
        TTree * genT;

        // Trigger and event number branches
        unsigned int fired_;
        unsigned long long eventNum_;
        unsigned long long runNum_;
        unsigned long long lumiSec_;

        uint32_t METFiltersFailBits_;

        // Gen branches
        
        // Pileup and gen weight
        int genpuobs_;
        int genputrue_;
        float genwgt_;
        int npv_;
        // Gen muon
        std::vector<float> genPt_;
        std::vector<float> genEta_;
        std::vector<float> genPhi_;
        std::vector<float> genEn_;
        std::vector<float> genVxy_;
        std::vector<float> genVz_;
        std::vector<float> genDr_;
        // Chi1 DM
        std::vector<float> genChi1Pt_;
        std::vector<float> genChi1Eta_;
        std::vector<float> genChi1Phi_;
        std::vector<float> genChi1En_;
        std::vector<float> genChi1Vxy_;
        std::vector<float> genChi1Vz_;
        // Chi2 DM
        std::vector<float> genChi2Pt_;
        std::vector<float> genChi2Eta_;
        std::vector<float> genChi2Phi_;
        std::vector<float> genChi2En_;
        std::vector<float> genChi2Vxy_;
        std::vector<float> genChi2Vz_;
        // Gen jet
        std::vector<float> genJetPt_;
        std::vector<float> genJetEta_;
        std::vector<float> genJetPhi_;
        // Gen MET
        float genLeadMetPt_;
        float genLeadMetPhi_;

        // Reco dSA muon branches
        int recoNMu_;
        int recoNGoodDSAMu_;
        std::vector<float> recoPt_;
        std::vector<float> recoEta_;
        std::vector<float> recoPhi_;
        std::vector<float> recoDxy_;
        std::vector<float> recoDz_;
        std::vector<int> recoCharge_;
        std::vector<float> recoTrkChi2_;
        std::vector<float> recoTrkNumPlanes_;
        std::vector<float> recoTrkNumHits_;
        int recoDSAIdx0_;
        int recoDSAIdx1_;

        // Selected muon branches
        int nSelectedMuons_;
        int recoNMatchedGBMvDSA_;
        float recoGBMDSADr_;
        std::vector<float> selectedMuonsPt_;
        std::vector<float> selectedMuonsEta_;
        std::vector<float> selectedMuonsPhi_;
        std::vector<float> selectedMuonsDxy_;
        std::vector<float> selectedMuonsDz_;
        std::vector<int> selectedMuonsCharge_;

        // Vertex branches
        float recoVtxVxy_;
        float recoVtxVz_;
        float recoVtxSigmaVxy_;
        float recoVtxReducedChi2_;
        float recoVtxDr_;

        // MET reco branches
        float recoPFMetPt_;
        float recoPFMetPhi_;
        float recoMmumu_;
        float recoDeltaPhiMetMu_;

        // Jet reco branches
        int recoPFNJet_;
        int recoPFNPassIDJet_;
        int recoPFNHighPtJet_;
        std::vector<float> recoPFJetPt_;
        std::vector<float> recoPFJetEta_;
        std::vector<float> recoPFJetPhi_;
        std::vector<float> recoPFJetBTag_;
        bool recoPFHEMFlag_;

        float recoPFMETJetDeltaPhi_;

        // MHT reco branch
        float MHTPt_;

        // as long as fewer than 32 cuts don't need to specify number
        uint32_t cuts_;
        inline void setCutBit(int bit) { cuts_ |= (1 << bit); }
        inline void clearCutBit(int bit) { cuts_ &= ~(1 << bit); }
};


#endif
