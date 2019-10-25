#ifndef washAOD_MetTrigSelfEffiForMuTrack_H
#define washAOD_MetTrigSelfEffiForMuTrack_H

/**
 * Trigger efficiencies itself
 * =======================================
 * Require:
 *   - >=2 dSA muons passing general selections
 * Check trigger firing condition
 */

#include <boost/format.hpp>
#include <boost/any.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

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
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
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

	bool runGen = true;
        TTree *muTrackT_;
        TTree *genweight_;
};

#endif
