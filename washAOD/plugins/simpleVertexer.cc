#include <memory>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/Ptr.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "TTree.h"

class simpleVertexer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit simpleVertexer(const edm::ParameterSet&);
  ~simpleVertexer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  struct theMuSys
  {
    theMuSys(){};
    theMuSys(reco::Vertex::Point&& p) {
      _vxy = std::sqrt(p.X()*p.X() + p.Y()*p.Y());
      _vz = p.Z();
    }
    double _vxy, _vz;
    
  };

  using MuSystem = std::pair<edm::Ptr<reco::Track>, edm::Ptr<reco::Track>>;
  using MuSystemCollection = std::vector<MuSystem>;

  MuSystemCollection
  makeMuSystemFromHandle(edm::Handle<reco::TrackCollection>&);

  std::vector<TransientVertex>
  makeTransientVerticesFromMuSystems(edm::ESHandle<TransientTrackBuilder>&, MuSystemCollection&);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void branchTTree(TTree*, theMuSys&);

  edm::Service<TFileService> fs;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  edm::EDGetTokenT<reco::TrackCollection> globalMuons;
  edm::EDGetTokenT<reco::TrackCollection> saMuons;
  edm::EDGetTokenT<reco::TrackCollection> saMuonsUAV;
  edm::EDGetTokenT<reco::TrackCollection> rsaMuons;
  edm::EDGetTokenT<reco::TrackCollection> dgMuons;
  edm::EDGetTokenT<reco::TrackCollection> dsaMuons;

  std::map<std::string, theMuSys> _themusys;
  std::map<std::string, TTree*> _thevtxtree;
    
};

void
simpleVertexer::branchTTree(TTree* t, theMuSys& p) {
  t->Branch("vxy", &p._vxy, "vxy/D");
  t->Branch("vz", &p._vz, "vz/D");
}

simpleVertexer::MuSystemCollection
simpleVertexer::makeMuSystemFromHandle(edm::Handle<reco::TrackCollection>& h) {
  MuSystemCollection musystems{};

  for (size_t iMu(0); iMu!=h->size(); ++iMu) {
      edm::Ptr<reco::Track> muA(h, iMu);
      for (size_t jMu(iMu+1); jMu!=h->size(); ++jMu) {
        edm::Ptr<reco::Track> muB(h, jMu);
        if (muA->charge()*muB->charge() > 0) continue;
        if (deltaR(*muA.get(), *muB.get()) > 1) continue;
        musystems.push_back(std::make_pair(muA, muB));
      }
  }

  return musystems;
}

std::vector<TransientVertex>
simpleVertexer::makeTransientVerticesFromMuSystems(edm::ESHandle<TransientTrackBuilder>& theB,
                                                   simpleVertexer::MuSystemCollection& mus) {
  

  std::vector<TransientVertex> _tvs{};
  std::vector<reco::TransientTrack> _tks{};
  for (const auto& mupair : mus) {
    reco::TransientTrack tt1(theB->build(mupair.first.get()));
    reco::TransientTrack tt2(theB->build(mupair.second.get()));
    _tks.push_back(tt1);
    _tks.push_back(tt2);
    
    KalmanVertexFitter kvf(true);
    _tvs.push_back( kvf.vertex(_tks) );
    _tks.clear();
  }

  return _tvs;
}

simpleVertexer::simpleVertexer(const edm::ParameterSet& iC)
{
  usesResource("TFileService");
  genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
  globalMuons = consumes<reco::TrackCollection>(edm::InputTag("globalMuons"));
  saMuons = consumes<reco::TrackCollection>(edm::InputTag("standAloneMuons"));
  saMuonsUAV = consumes<reco::TrackCollection>(edm::InputTag("standAloneMuons","UpdatedAtVtx"));
  rsaMuons = consumes<reco::TrackCollection>(edm::InputTag("refittedStandAloneMuons"));
  dgMuons = consumes<reco::TrackCollection>(edm::InputTag("displacedGlobalMuons"));
  dsaMuons = consumes<reco::TrackCollection>(edm::InputTag("displacedStandAloneMuons"));
}

simpleVertexer::~simpleVertexer()
{

}

void
simpleVertexer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  auto passEta = [](const auto& p){return abs(p.pdgId()) == 13 && abs(p.eta()) <= 2.4;};
  bool acceptance = std::count_if(cbegin(*genParticlesH), cend(*genParticlesH), passEta) >= 4;
  if (!acceptance) return;  

  Handle<reco::TrackCollection> globalMuonsH, saMuonsH, saMuonsUAVH, rsaMuonsH, dgMuonsH, dsaMuonsH;
  edm::ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
  
  iEvent.getByToken(saMuons, saMuonsH);
  if (!saMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    // edm::LogInfo("Firefighter/simpleVertexer")
    // << "Found: " << saMuonsH->size() << " standAloneMuons" << "\n";
    
    MuSystemCollection saMuonSystems = makeMuSystemFromHandle(saMuonsH);
    std::vector<TransientVertex> saMuSysVertices
      = makeTransientVerticesFromMuSystems(theB, saMuonSystems);

    if (saMuSysVertices.size() > 0) {
      for (const auto& v : saMuSysVertices) {
        _themusys["saMuon"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["saMuon"]->Fill();
      }
    }
  }


  iEvent.getByToken(globalMuons, globalMuonsH);
  if (!globalMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection globalMuonSystems = makeMuSystemFromHandle(globalMuonsH);
    std::vector<TransientVertex> globalMuSysVertices
      = makeTransientVerticesFromMuSystems(theB, globalMuonSystems);

    if (globalMuSysVertices.size() > 0) {
      for (const auto& v : globalMuSysVertices) {
        _themusys["globalMu"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["globalMu"]->Fill();
      }
    }
  }


  iEvent.getByToken(saMuonsUAV, saMuonsUAVH);
  if (!saMuonsUAVH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection saMuonUAVSystems = makeMuSystemFromHandle(saMuonsUAVH);
    std::vector<TransientVertex> saMuUAVSysVertices
      = makeTransientVerticesFromMuSystems(theB, saMuonUAVSystems);

    if (saMuUAVSysVertices.size() > 0) {
      for (const auto& v : saMuUAVSysVertices) {
        _themusys["saMuUAV"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["saMuUAV"]->Fill();
      }
    }
  }


  iEvent.getByToken(rsaMuons, rsaMuonsH);
  if (!rsaMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection rsaMuonSystems = makeMuSystemFromHandle(rsaMuonsH);
    std::vector<TransientVertex> rsaMuSysVertices
      = makeTransientVerticesFromMuSystems(theB, rsaMuonSystems);

    if (rsaMuSysVertices.size() > 0) {
      for (const auto& v : rsaMuSysVertices) {
        _themusys["rsaMuon"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["rsaMuon"]->Fill();
      }
    }
  }


  iEvent.getByToken(dsaMuons, dsaMuonsH);
  if (!dsaMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection dsaMuonSystems = makeMuSystemFromHandle(dsaMuonsH);
    std::vector<TransientVertex> dsaMuSysVertices
      = makeTransientVerticesFromMuSystems(theB, dsaMuonSystems);

    if (dsaMuSysVertices.size() > 0) {
      for (const auto& v : dsaMuSysVertices) {
        _themusys["dsaMuon"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["dsaMuon"]->Fill();
      }
    }
  }


  iEvent.getByToken(dgMuons, dgMuonsH);
  if (!dgMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection dgMuonSystems = makeMuSystemFromHandle(dgMuonsH);
    std::vector<TransientVertex> dgMuSysVertices
      = makeTransientVerticesFromMuSystems(theB, dgMuonSystems);

    if (dgMuSysVertices.size() > 0) {
      for (const auto& v : dgMuSysVertices) {
        _themusys["dgMuon"] = theMuSys(reco::Vertex::Point(v.position()));
        _thevtxtree["dgMuon"]->Fill();
      }
    }
  }

}

void
simpleVertexer::beginJob()
{
  _thevtxtree["saMuon"] = fs->make<TTree>("standAloneMuon", "");
  branchTTree(_thevtxtree["saMuon"], _themusys["saMuon"]);

  _thevtxtree["globalMu"] = fs->make<TTree>("globalMuons", "");
  branchTTree(_thevtxtree["globalMu"], _themusys["globalMu"]);

  _thevtxtree["saMuUAV"] = fs->make<TTree>("standAloneMuonUpdatedAtVertex", "");
  branchTTree(_thevtxtree["saMuUAV"], _themusys["saMuUAV"]);

  _thevtxtree["rsaMuon"] = fs->make<TTree>("refittedStandAloneMuon", "");
  branchTTree(_thevtxtree["rsaMuon"], _themusys["rsaMuon"]);

  _thevtxtree["dsaMuon"] = fs->make<TTree>("displacedStandAloneMuon", "");
  branchTTree(_thevtxtree["dsaMuon"], _themusys["dsaMuon"]);

  _thevtxtree["dgMuon"] = fs->make<TTree>("displacedGlobalMuon", "");
  branchTTree(_thevtxtree["dgMuon"], _themusys["dgMuon"]);
}

void
simpleVertexer::endJob()
{

}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
simpleVertexer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.add("simpvtx", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(simpleVertexer);