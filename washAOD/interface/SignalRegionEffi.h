#ifndef washAOD_SignalRegionEffi_H
#define washAOD_SignalRegionEffi_H

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

#include "TTree.h"

class SignalRegionEffi :
    public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>
{
    public:
        explicit SignalRegionEffi(const edm::ParameterSet&);
        ~SignalRegionEffi();

        static void fillDescriptions(edm::ConfigurationDescriptions&);

    private:
        virtual void beginJob() override;
        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endJob() override;

        const edm::InputTag muTrackTag_;
        const edm::InputTag genParticleTag_;
        const edm::InputTag genJetTag_;
        const edm::InputTag genMetTag_;
        const edm::InputTag recoMetTag_;
        const edm::InputTag recoJetTag_;
        const edm::InputTag trigResultsTag_;
        const edm::InputTag trigEventTag_;
        const std::string trigPathNoVer_;
        const std::string processName_;

        const edm::EDGetTokenT<reco::TrackCollection> muTrackToken_;
        const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
        const edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
        const edm::EDGetTokenT<reco::GenMETCollection> genMetToken_;
        const edm::EDGetTokenT<reco::PFMETCollection> recoMetToken_;
        const edm::EDGetTokenT<reco::PFJetCollection> recoJetToken_;
        const edm::EDGetTokenT<edm::TriggerResults> trigResultsToken_;
        const edm::EDGetTokenT<trigger::TriggerEvent> trigEventToken_;

        edm::Service<TFileService> fs;
        edm::Handle<reco::TrackCollection> muTrackHandle_;
        edm::Handle<reco::GenParticleCollection> genParticleHandle_;
        edm::Handle<reco::GenJetCollection> genJetHandle_;
        edm::Handle<reco::GenMETCollection> genMetHandle_;
        edm::Handle<reco::PFMETCollection> recoMetHandle_;
        edm::Handle<reco::PFJetCollection> recoJetHandle_;
        edm::Handle<edm::TriggerResults> trigResultsHandle_;
        edm::Handle<trigger::TriggerEvent> trigEventHandle_;
        
        std::string trigPath_;
        HLTConfigProvider hltConfig_;

        unsigned int fired_;
        //unsigned int nMatched_;

        //std::vector<float> genPt_;
        //std::vector<float> genEta_;
        //std::vector<float> genPhi_;
        //std::vector<float> genVxy_;
        //std::vector<float> genVz_;
        //std::vector<float> genDr_;
        std::vector<float> recoPt_;
        std::vector<float> recoEta_;
        std::vector<float> recoPhi_;
        std::vector<float> recoDxy_;
        std::vector<float> recoDz_;
        std::vector<float> recoVxy_;
        std::vector<float> recoVz_;
        std::vector<float> recoDr_;
        //std::vector<float> deltaR_;
        //float genJetPt_;
        //float genLeadMetPt_;
        //float genSubLeadMetPt_;
        float recoPFMetPt_;
        float recoPFMetPhi_;
        std::vector<float> recoPFJetPt_;
        std::vector<float> recoPFJetEta_;
        std::vector<float> recoPFJetPhi_;
        float MHTPt_;

        std::vector<TTree*> cutsTree;
        int cutsVec[6];
};

#endif
