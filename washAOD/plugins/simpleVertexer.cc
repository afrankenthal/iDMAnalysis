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
    theMuSys(GlobalPoint p) {
      _vxy = std::sqrt(p.x()*p.x() + p.y()*p.y());
      _vz = p.z();
    }
    double _vxy, _vz;
    
  };

  using MuSystem = std::pair<reco::TrackRef, reco::TrackRef>;
  using MuSystemCollection = std::vector<MuSystem>;

  MuSystemCollection
  makeMuSystemFromHandle(edm::Handle<reco::TrackCollection>&);

  std::vector<TransientVertex>
  makeTransientVerticesFromMuSystems(edm::ESHandle<MagneticField>&, MuSystemCollection&);

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
      reco::TrackRef muA(h, iMu);
      for (size_t jMu(iMu+1); jMu!=h->size(); ++jMu) {
        reco::TrackRef muB(h, jMu);
        if (muA->charge()*muB->charge() > 0) continue;
        if (deltaR(*muA.get(), *muB.get()) > 1) continue;
        musystems.push_back(std::make_pair(muA, muB));
      }
  }

  return musystems;
}

std::vector<TransientVertex>
simpleVertexer::makeTransientVerticesFromMuSystems(edm::ESHandle<MagneticField>& mf,
                                                   simpleVertexer::MuSystemCollection& mus) {
  

  std::vector<TransientVertex> _tvs{};
  std::vector<reco::TransientTrack> _tks{};
  for (const auto& mupair : mus) {
    _tks.emplace_back(mupair.first,  &(*mf));
    _tks.emplace_back(mupair.second, &(*mf));
    
    KalmanVertexFitter kvf(true);
    TransientVertex tv = kvf.vertex(_tks);
    if ( tv.isValid() ) { //<- must check this! otherwise uninitialized proxyBase exception
      _tvs.push_back(tv);
    }
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
  // edm::ESHandle<TransientTrackBuilder> theB;
  // iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);

  edm::ESHandle<MagneticField> bFieldHandle;
  iSetup.get<IdealMagneticFieldRecord>().get(bFieldHandle);
  
  /// standAloneMuons
  iEvent.getByToken(saMuons, saMuonsH);
  if (!saMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    // edm::LogInfo("Firefighter/simpleVertexer")
    // << "Found: " << saMuonsH->size() << " standAloneMuons" << "\n";
    
    MuSystemCollection saMuonSystems = makeMuSystemFromHandle(saMuonsH);
    std::vector<TransientVertex> saMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, saMuonSystems);

    if (saMuSysVertices.size() > 0) {
      for (const auto& v : saMuSysVertices) {
        _themusys["saMuon"] = theMuSys(v.position());
        _thevtxtree["saMuon"]->Fill();
      }
    }
  }

  /// globalMuons
  iEvent.getByToken(globalMuons, globalMuonsH);
  if (!globalMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection globalMuonSystems = makeMuSystemFromHandle(globalMuonsH);
    std::vector<TransientVertex> globalMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, globalMuonSystems);

    if (globalMuSysVertices.size() > 0) {
      for (const auto& v : globalMuSysVertices) {
        _themusys["globalMu"] = theMuSys(v.position());
        _thevtxtree["globalMu"]->Fill();
      }
    }
  }

  /// standAloneMuons UpdatedAtVertex
  iEvent.getByToken(saMuonsUAV, saMuonsUAVH);
  if (!saMuonsUAVH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection saMuonUAVSystems = makeMuSystemFromHandle(saMuonsUAVH);
    std::vector<TransientVertex> saMuUAVSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, saMuonUAVSystems);

    if (saMuUAVSysVertices.size() > 0) {
      for (const auto& v : saMuUAVSysVertices) {
        _themusys["saMuUAV"] = theMuSys(v.position());
        _thevtxtree["saMuUAV"]->Fill();
      }
    }
  }

  /// refittedStandAloneMuons
  iEvent.getByToken(rsaMuons, rsaMuonsH);
  if (!rsaMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection rsaMuonSystems = makeMuSystemFromHandle(rsaMuonsH);
    std::vector<TransientVertex> rsaMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, rsaMuonSystems);

    if (rsaMuSysVertices.size() > 0) {
      for (const auto& v : rsaMuSysVertices) {
        _themusys["rsaMuon"] = theMuSys(v.position());
        _thevtxtree["rsaMuon"]->Fill();
      }
    }
  }

  /// displacedStandAloneMuons
  iEvent.getByToken(dsaMuons, dsaMuonsH);
  if (!dsaMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection dsaMuonSystems = makeMuSystemFromHandle(dsaMuonsH);
    std::vector<TransientVertex> dsaMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, dsaMuonSystems);

    if (dsaMuSysVertices.size() > 0) {
      for (const auto& v : dsaMuSysVertices) {
        _themusys["dsaMuon"] = theMuSys(v.position());
        _thevtxtree["dsaMuon"]->Fill();
      }
    }
  }

  /// displacedGlobalMuons
  iEvent.getByToken(dgMuons, dgMuonsH);
  if (!dgMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection dgMuonSystems = makeMuSystemFromHandle(dgMuonsH);
    std::vector<TransientVertex> dgMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, dgMuonSystems);

    if (dgMuSysVertices.size() > 0) {
      for (const auto& v : dgMuSysVertices) {
        _themusys["dgMuon"] = theMuSys(v.position());
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