#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Common/interface/Ref.h"

class TestTrackRef : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit TestTrackRef(const edm::ParameterSet&);
  ~TestTrackRef();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<reco::TrackCollection> generalTks;
  edm::EDGetTokenT<reco::TrackCollection> globalMuons;
  edm::EDGetTokenT<reco::TrackCollection> saMuons;
  edm::EDGetTokenT<reco::MuonCollection> recoMuons;
  edm::EDGetTokenT<reco::PFCandidateCollection> pfCands;
};


TestTrackRef::TestTrackRef(const edm::ParameterSet& iC)
{
  generalTks = consumes<reco::TrackCollection>(edm::InputTag("generalTracks"));
  globalMuons = consumes<reco::TrackCollection>(edm::InputTag("globalMuons"));
  saMuons = consumes<reco::TrackCollection>(edm::InputTag("standAloneMuons", "UpdatedAtVtx"));
  recoMuons = consumes<reco::MuonCollection>(edm::InputTag("muons"));
  pfCands = consumes<reco::PFCandidateCollection>(edm::InputTag("particleFlow"));
}

TestTrackRef::~TestTrackRef() {}

void TestTrackRef::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  std::cout<<"++++ Event "<<iEvent.id()<<" ++++"<<std::endl;
  Handle<reco::TrackCollection> globalMuonsH, saMuonsH, generalTksH;
  iEvent.getByToken(generalTks, generalTksH);
  iEvent.getByToken(globalMuons, globalMuonsH);
  iEvent.getByToken(saMuons, saMuonsH);
  Handle<reco::MuonCollection> recoMuonsH;
  iEvent.getByToken(recoMuons, recoMuonsH);
  Handle<reco::PFCandidateCollection> pfCandsH;
  iEvent.getByToken(pfCands, pfCandsH);

  // for (size_t iGm(0); iGm!=globalMuonsH->size(); ++iGm) {
  //   reco::TrackRef gmRef(globalMuonsH, iGm);
  // }
  std::vector<reco::TrackRef> generalTksRefV{}, globalMuonsRefV{}, saMuonsRefV{};
  for (size_t igk(0); igk!=generalTksH->size(); ++igk) {
    generalTksRefV.emplace_back(generalTksH, igk);
  }
  for (size_t igm(0); igm!=globalMuonsH->size(); ++igm) {
    globalMuonsRefV.emplace_back(globalMuonsH, igm);
  }
  for (size_t isam(0); isam!=saMuonsH->size(); ++isam) {
    saMuonsRefV.emplace_back(saMuonsH, isam);
  }

  for (const auto& gm : globalMuonsRefV) {
    for (const auto& gt : generalTksRefV) {
      if (gm == gt) std::cout<<"Found a globalMuon in generalTracks: True\n";
    }
  }

  for (const auto& mu : *recoMuonsH) {
    for (const auto& gt : generalTksRefV) {
      if (mu.innerTrack() == gt)
        std::cout<<"Found a recoMu _inner_ track in generalTracks: True "
        <<int(&gt-&generalTksRefV[0])<<std::endl;
      if (mu.outerTrack() == gt)
        std::cout<<"Found a recoMu _outer_ track in generalTracks: True "
        <<int(&gt-&generalTksRefV[0])<<std::endl;
      if (mu.globalTrack()== gt)
        std::cout<<"Found a recoMu _global_ track in generalTracks: True "
        <<int(&gt-&generalTksRefV[0])<<std::endl;
    }
  }

  for (const auto& mu : *recoMuonsH) {
    for (const auto& gt : globalMuonsRefV) {
      if (mu.innerTrack() == gt)
        std::cout<<"Found a recoMu _inner_ track in globalMuons: True "
        <<int(&gt-&globalMuonsRefV[0])<<std::endl;
      if (mu.outerTrack() == gt)
        std::cout<<"Found a recoMu _outer_ track in globalMuons: True "
        <<int(&gt-&globalMuonsRefV[0])<<std::endl;
      if (mu.globalTrack()== gt)
        std::cout<<"Found a recoMu _global_ track in globalMuons: True "
        <<int(&gt-&globalMuonsRefV[0])<<std::endl;
    }
  }

  for (const auto& mu : *recoMuonsH) {
    for (const auto& sa : saMuonsRefV) {
      if (mu.innerTrack() == sa)
        std::cout<<"Found a recoMu _inner_ track in standAloneMuons: True "
        <<int(&sa-&saMuonsRefV[0])<<std::endl;
      if (mu.outerTrack() == sa)
        std::cout<<"Found a recoMu _outer_ track in standAloneMuons: True "
        <<int(&sa-&saMuonsRefV[0])<<std::endl;
      if (mu.globalTrack()== sa)
        std::cout<<"Found a recoMu _global_ track in standAloneMuons: True "
        <<int(&sa-&saMuonsRefV[0])<<std::endl;
    }
  }

  // for (const auto& pc : *pfCandsH) {
  //   if (pc.trackRef().isNull()) continue;
  //   for (const auto& gt : generalTksRefV) {
  //     if (pc.trackRef() == gt)
  //       std::cout<<"Found a pfCand's trackRef in generalTracks: True "
  //       <<int(&gt-&generalTksRefV[0])<<std::endl;
  //   }
  // }

  for (const auto& pc : *pfCandsH) {
    if (pc.muonRef().isNull()) continue;
    for (size_t iMu(0); iMu!=recoMuonsH->size(); ++iMu) {
      if (pc.muonRef() == reco::MuonRef(recoMuonsH, iMu))
        std::cout<<"Found a pfCand's muonRef in recoMuons: True "
        <<iMu<<std::endl;
    }
  }
}

void TestTrackRef::beginJob()
{
  std::cout<<"Begin job >>>\n";
}

void TestTrackRef::endJob()
{
  std::cout<<"End job <<<\n";
}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void TestTrackRef::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  descriptions.add("testtrackref", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TestTrackRef);