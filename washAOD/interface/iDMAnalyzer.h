#ifndef washAOD_iDMAnalyzer_H
#define washAOD_iDMAnalyzer_H

#include <boost/format.hpp>
#include <boost/any.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
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
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TTree.h"

#include <random>

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
        std::mt19937 m_random_generator; 

        edm::ESHandle<JetCorrectorParametersCollection> JetCorParCollHandle_;
        JetCorrectionUncertainty * jecUnc;

        bool isData;

        // Tags
        const edm::InputTag bTagProbbTag_;
        const edm::InputTag bTagProbbbTag_;
        const edm::InputTag bTagCombineTag_;
        const edm::InputTag dsaRecoMuTag_;
        const edm::InputTag dsaRecoMuTimingTag_;
        const edm::InputTag muTrackTag1_;
        const edm::InputTag muTrackTag2_;
        const edm::InputTag genParticleTag_;
        const edm::InputTag genJetTag_;
        const edm::InputTag genMETTag_;
        const edm::InputTag recoPFMETTag_;
        const edm::InputTag recoCaloMETTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
        const edm::InputTag pileupInfosTag_;
        const edm::InputTag genEvtInfoTag_;
        const std::string processName_;
        const edm::InputTag HBHENoiseFilterResultProducerTag_;
        const edm::InputTag HBHEIsoNoiseFilterResultProducerTag_;
        const edm::InputTag primaryVertexFilterTag_;
        const edm::InputTag globalSuperTightHalo2016FilterTag_;
        const edm::InputTag EcalDeadCellTriggerPrimitiveFilterTag_;
        const edm::InputTag ecalBadCalibFilterTag_;
        const edm::InputTag BadPFMuonFilterTag_;
        const edm::InputTag muonBadTrackFilterTag_;
        const edm::InputTag mJetCorrectorTag_;
        const edm::InputTag recoElectronTag_;
        const edm::InputTag recoElectronIDTag_;
        const edm::InputTag recoPhotonTag_;
        const edm::InputTag recoPhotonIDTag_;
        const edm::InputTag rhoTag_;

        // Tokens
        const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbToken_;
        const edm::EDGetTokenT<reco::JetTagCollection> bTagProbbbToken_;
        const edm::EDGetTokenT<reco::JetTagCollection> bTagCombineToken_;
        const edm::EDGetTokenT<reco::MuonCollection> dsaRecoMuToken_;
        const edm::EDGetTokenT<reco::MuonTimeExtraMap> dsaRecoMuTimingToken_;
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
        const edm::EDGetTokenT<bool>globalSuperTightHalo2016FilterToken_;
        const edm::EDGetTokenT<bool>EcalDeadCellTriggerPrimitiveFilterToken_;
        const edm::EDGetTokenT<bool>ecalBadCalibFilterToken_;
        const edm::EDGetTokenT<bool>BadPFMuonFilterToken_;
        const edm::EDGetTokenT<bool>muonBadTrackFilterToken_;
        const edm::EDGetTokenT<reco::JetCorrector> mJetCorrectorToken_;
        const edm::EDGetTokenT<reco::GsfElectronCollection> recoElectronToken_;
        const edm::EDGetTokenT<edm::ValueMap<float>> recoElectronIDToken_;
        const edm::EDGetTokenT<reco::PhotonCollection> recoPhotonToken_;
        const edm::EDGetTokenT<edm::ValueMap<bool>> recoPhotonIDToken_;
        const edm::EDGetTokenT<double> rhoToken_;

        // Handles
        edm::Handle<reco::JetTagCollection> bTagProbbHandle_;
        edm::Handle<reco::JetTagCollection> bTagProbbbHandle_;
        edm::Handle<reco::JetTagCollection> bTagCombineHandle_;
        edm::Handle<reco::MuonCollection> dsaRecoMuHandle_;
        edm::Handle<reco::MuonTimeExtraMap> dsaRecoMuTimingHandle_;
        edm::Handle<reco::TrackCollection> muTrackHandle1_;
        edm::Handle<reco::TrackCollection> muTrackHandle2_;
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
        // Gen particles
        int nGen_;
        std::vector<int> genID_;
        // Only save hard-process gen particles
        //std::vector<bool> genHardProcess_;
        std::vector<int> genCharge_;
        std::vector<float> genPt_;
        std::vector<float> genEta_;
        std::vector<float> genPhi_;
        std::vector<float> genPz_;
        std::vector<float> genEn_;
        std::vector<float> genVxy_;
        std::vector<float> genVz_;
        std::vector<float> genMass_;
        
        // Gen muon
        //std::vector<float> genPt_;
        //std::vector<float> genEta_;
        //std::vector<float> genPhi_;
        //std::vector<float> genEn_;
        //std::vector<float> genVxy_;
        //std::vector<float> genVz_;
        //std::vector<float> genDr_;
        //// Chi1 DM
        //std::vector<float> genChi1Pt_;
        //std::vector<float> genChi1Eta_;
        //std::vector<float> genChi1Phi_;
        //std::vector<float> genChi1En_;
        //std::vector<float> genChi1Vxy_;
        //std::vector<float> genChi1Vz_;
        //// Chi2 DM
        //std::vector<float> genChi2Pt_;
        //std::vector<float> genChi2Eta_;
        //std::vector<float> genChi2Phi_;
        //std::vector<float> genChi2En_;
        //std::vector<float> genChi2Vxy_;
        //std::vector<float> genChi2Vz_;
        
        // Gen jet
        std::vector<float> genJetPt_;
        std::vector<float> genJetEta_;
        std::vector<float> genJetPhi_;
        // Gen MET
        float genLeadMETPt_;
        float genLeadMETPhi_;

        // Reco dSA muon branches
        int recoNDSA_;
        int recoNGoodDSA_;
        std::vector<float> recoDSAPt_;
        std::vector<float> recoDSAPtError_;
        std::vector<float> recoDSAEta_;
        std::vector<float> recoDSAEtaError_;
        std::vector<float> recoDSAPhi_;
        std::vector<float> recoDSAPhiError_;
        std::vector<float> recoDSADxy_;
        std::vector<float> recoDSADxyError_;
        std::vector<float> recoDSADz_;
        std::vector<float> recoDSADzError_;
        std::vector<int> recoDSACharge_;
        std::vector<float> recoDSATrkChi2_;
        std::vector<float> recoDSATrkNumPlanes_;
        std::vector<float> recoDSATrkNumHits_;
        std::vector<float> recoDSATrkNumDTHits_;
        std::vector<float> recoDSATrkNumCSCHits_;
        std::vector<float> recoDSAInvBeta_;
        std::vector<float> recoDSAInvBetaErr_;
        std::vector<float> recoDSAFreeInvBeta_;
        std::vector<float> recoDSAFreeInvBetaErr_;
        std::vector<float> recoDSAtimeAtIpInOut_;
        std::vector<float> recoDSAtimeAtIpInOutErr_;
        std::vector<float> recoDSAtimeAtIpOutIn_;
        std::vector<float> recoDSAtimeAtIpOutInErr_;
        std::vector<float> recoDSAtimingNdof_;
        int recoDSAIdx0_;
        int recoDSAIdx1_;
        
        // Reco GM branches
        int recoNGM_;
        int recoNGoodGM_;
        std::vector<float> recoGMPt_;
        std::vector<float> recoGMPtError_;
        std::vector<float> recoGMEta_;
        std::vector<float> recoGMEtaError_;
        std::vector<float> recoGMPhi_;
        std::vector<float> recoGMPhiError_;
        std::vector<float> recoGMDxy_;
        std::vector<float> recoGMDxyError_;
        std::vector<float> recoGMDz_;
        std::vector<float> recoGMDzError_;
        std::vector<int> recoGMCharge_;
        std::vector<float> recoGMTrkChi2_;
        std::vector<float> recoGMTrkNumPlanes_;
        std::vector<float> recoGMTrkNumHits_;
        std::vector<float> recoGMTrkNumDTHits_;
        std::vector<float> recoGMTrkNumCSCHits_;

        // Selected muon branches
        int nSelectedMuons_;
        int recoNMatchedGBMvDSA_;
        float recoGBMDSADr_;
        std::vector<float> selectedMuonsPt_;
        std::vector<float> selectedMuonsPtError_;
        std::vector<float> selectedMuonsEta_;
        std::vector<float> selectedMuonsEtaError_;
        std::vector<float> selectedMuonsPhi_;
        std::vector<float> selectedMuonsPhiError_;
        std::vector<float> selectedMuonsDxy_;
        std::vector<float> selectedMuonsDxyError_;
        std::vector<float> selectedMuonsDz_;
        std::vector<float> selectedMuonsDzError_;
        std::vector<int> selectedMuonsCharge_;
        
        // Reco electron branches
        int recoNElectron_;
        int recoNGoodElectron_;
        std::vector<float> recoElectronPt_;
        std::vector<float> recoElectronEta_;
        std::vector<float> recoElectronPhi_;
        std::vector<float> recoElectronVxy_;
        std::vector<float> recoElectronVz_;
        std::vector<int> recoElectronCharge_;
        std::vector<int> recoElectronIDResult_;
        
        // Reco photon branches
        int recoNPhoton_;
        int recoNGoodPhoton_;
        std::vector<float> recoPhotonPt_;
        std::vector<float> recoPhotonEta_;
        std::vector<float> recoPhotonPhi_;
        std::vector<int> recoPhotonIDResult_;

        // Vertex branches
        float recoVtxVxy_;
        float recoVtxVz_;
        float recoVtxSigmaVxy_;
        float recoVtxReducedChi2_;
        float recoVtxDr_;

        float recoMmumu_;

        // MET reco branches
        float recoPFMETPt_;
        float recoPFMETPhi_;
        float recoPFMETSmearingOnlyPt_;
        float recoPFMETSmearingOnlyPhi_;
        float recoPFMETCorrectedPt_;
        float recoPFMETCorrectedPhi_;
        float recoPFMETJESUpPt_;
        float recoPFMETJESUpPhi_;
        float recoPFMETJESDownPt_;
        float recoPFMETJESDownPhi_;
        float recoPFMETJERUpPt_;
        float recoPFMETJERUpPhi_;
        float recoPFMETJERDownPt_;
        float recoPFMETJERDownPhi_;
        float recoPFMETMuonEtFraction_;
        float recoCaloMETPt_;
        float recoCaloMETPhi_;
        float recoPFRecoilPt_;
        float recoPFRecoilPhi_;
        float recoDeltaPhiMETMu_;
        float recoDeltaPhiCorrectedMETMu_;

        // Jet reco branches
        int recoPFNJet_;
        int recoPFNPassIDJet_;
        int recoPFNHighPtJet_;
        std::vector<float> recoPFJetPt_;
        std::vector<float> recoPFJetEta_;
        std::vector<float> recoPFJetPhi_;
        std::vector<float> recoPFJetCorrectedPt_;
        std::vector<float> recoPFJetCorrectedEta_;
        std::vector<float> recoPFJetCorrectedPhi_;
        std::vector<float> recoPFJetCorrectedBTag_;
        std::vector<float> recoPFJetCorrectedCHEF_;
        std::vector<float> recoPFJetCorrectedNHEF_;
        std::vector<float> recoPFJetCorrectedCEEF_;
        std::vector<float> recoPFJetCorrectedNEEF_;
        std::vector<float> recoPFJetCorrectedNumDaughters_;
        std::vector<float> recoPFJetCorrectedChargedMultiplicity_;
        std::vector<float> recoPFJetCorrectedJESUpPt_;
        std::vector<float> recoPFJetCorrectedJESUpEta_;
        std::vector<float> recoPFJetCorrectedJESUpPhi_;
        std::vector<float> recoPFJetCorrectedJESDownPt_;
        std::vector<float> recoPFJetCorrectedJESDownEta_;
        std::vector<float> recoPFJetCorrectedJESDownPhi_;
        std::vector<float> recoPFJetCorrectedJERUpPt_;
        std::vector<float> recoPFJetCorrectedJERUpEta_;
        std::vector<float> recoPFJetCorrectedJERUpPhi_;
        std::vector<float> recoPFJetCorrectedJERDownPt_;
        std::vector<float> recoPFJetCorrectedJERDownEta_;
        std::vector<float> recoPFJetCorrectedJERDownPhi_;
        bool recoPFHEMFlag_;
	bool bTagCombineBool;

        float recoPFMETJetDeltaPhi_;

        // MHT reco branch
        float MHTPt_;

        // as long as fewer than 32 cuts don't need to specify number
        uint32_t cuts_;
        inline void setCutBit(int bit) { cuts_ |= (1 << bit); }
        inline void clearCutBit(int bit) { cuts_ &= ~(1 << bit); }
};


#endif
