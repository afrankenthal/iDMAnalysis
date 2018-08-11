#ifndef washAOD_recoEffiForMuTrack_H
#define washAOD_recoEffiForMuTrack_H

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

#include "TTree.h"

class recoEffiForMuTrack :
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit recoEffiForMuTrack(const edm::ParameterSet&);
    ~recoEffiForMuTrack();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    const edm::InputTag muTrackTag_;
    const edm::InputTag genParticleTag_;
    const edm::EDGetTokenT<reco::TrackCollection> muTrackToken_;
    const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::TrackCollection> muTrackHandle_;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    unsigned int nMatched_;

    std::vector<float> genPt_;
    std::vector<float> genEta_;
    std::vector<float> genPhi_;
    std::vector<float> genVxy_;
    std::vector<float> genVz_;
    std::vector<float> genDr_;
    std::vector<float> recoPt_;
    std::vector<float> recoEta_;
    std::vector<float> recoPhi_;
    std::vector<float> recoDxy_;
    std::vector<float> recoDz_;
    std::vector<float> recoVxy_;
    std::vector<float> recoVz_;
    std::vector<float> deltaR_;

    TTree *muTrackT_;
};

#endif
