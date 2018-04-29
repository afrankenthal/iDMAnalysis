#include <memory>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "TTree.h"

class MuPairVertexer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit MuPairVertexer(const edm::ParameterSet&);
  ~MuPairVertexer();

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

  // MuSystemCollection
  // makeMuSystemFromHandle(edm::Handle<reco::TrackCollection>&);

  MuSystemCollection
  makeMuSystemFromHandle(edm::Handle<pat::MuonCollection>&, std::string);

  std::vector<TransientVertex>
  makeTransientVerticesFromMuSystems(edm::ESHandle<MagneticField>&, MuSystemCollection&);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void branchTTree(TTree*, theMuSys&);

  edm::Service<TFileService> fs;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  edm::EDGetTokenT<pat::MuonCollection>  patMuons;


  std::map<std::string, theMuSys> _themusys;
  std::map<std::string, TTree*> _thevtxtree;
    
};

void
MuPairVertexer::branchTTree(TTree* t, theMuSys& p) {
  t->Branch("vxy", &p._vxy, "vxy/D");
  t->Branch("vz", &p._vz, "vz/D");
}


MuPairVertexer::MuSystemCollection
MuPairVertexer::makeMuSystemFromHandle(edm::Handle<pat::MuonCollection>& h, std::string tkType) {
  MuSystemCollection musystems{};

  for (size_t iMu(0); iMu!=h->size(); ++iMu) {
      pat::MuonRef muA(h, iMu);
      for (size_t jMu(iMu+1); jMu!=h->size(); ++jMu) {
        pat::MuonRef muB(h, jMu);
        if (muA->charge()*muB->charge() > 0) continue;
        if (deltaR(*muA.get(), *muB.get()) > 1) continue;
        reco::TrackRef muA_tk, muB_tk;
        if (tkType == "global") {
          muA_tk = muA->globalTrack();
          muB_tk = muB->globalTrack();
        } else if (tkType == "inner") {
          muA_tk = muA->innerTrack();
          muB_tk = muB->innerTrack();
        } else if (tkType == "outer") {
          muA_tk = muA->outerTrack();
          muB_tk = muB->outerTrack();
        } else {
          throw cms::Exception("argumentNotAllowed")
          << "tkType \""<< tkType <<"\" not allowed.\n"
          << "Options: global, inner, outer\n";
        }
        
        if (muA_tk.isNull() || muB_tk.isNull()) continue;
        musystems.push_back(std::make_pair(muA_tk, muB_tk));
      }
  }

  return musystems;
}

std::vector<TransientVertex>
MuPairVertexer::makeTransientVerticesFromMuSystems(edm::ESHandle<MagneticField>& mf,
                                                   MuPairVertexer::MuSystemCollection& mus) {
  

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

MuPairVertexer::MuPairVertexer(const edm::ParameterSet& iC)
{
  usesResource("TFileService");
  genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("prunedGenParticles"));
  patMuons = consumes<pat::MuonCollection>(edm::InputTag("slimmedMuons"));
}

MuPairVertexer::~MuPairVertexer()
{

}

void
MuPairVertexer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  auto passEta = [](const auto& p){return abs(p.pdgId()) == 13 && abs(p.eta()) <= 2.4;};
  bool acceptance = std::count_if(cbegin(*genParticlesH), cend(*genParticlesH), passEta) >= 4;
  if (!acceptance) return;

  Handle<pat::MuonCollection> patMuonsH;

  edm::ESHandle<MagneticField> bFieldHandle;
  iSetup.get<IdealMagneticFieldRecord>().get(bFieldHandle);

  /// patMuons global|inner|outer
  iEvent.getByToken(patMuons, patMuonsH);
  if (!patMuonsH.isValid()) {
    edm::LogInfo("Firefighter/MuPairVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    MuSystemCollection patMuonSystems = makeMuSystemFromHandle(patMuonsH, "global");
    std::vector<TransientVertex> patMuSysVertices
      = makeTransientVerticesFromMuSystems(bFieldHandle, patMuonSystems);

    if (patMuSysVertices.size() > 0) {
      for (const auto& v : patMuSysVertices) {
        _themusys["patMu_g"] = theMuSys(v.position());
        _thevtxtree["patMu_g"]->Fill();
      }
    }


    patMuonSystems.clear(); patMuSysVertices.clear();
    patMuonSystems = makeMuSystemFromHandle(patMuonsH, "inner");
    patMuSysVertices = makeTransientVerticesFromMuSystems(bFieldHandle, patMuonSystems);

    if (patMuSysVertices.size() > 0) {
      for (const auto& v : patMuSysVertices) {
        _themusys["patMu_i"] = theMuSys(v.position());
        _thevtxtree["patMu_i"]->Fill();
      }
    }


    patMuonSystems.clear(); patMuSysVertices.clear();
    patMuonSystems = makeMuSystemFromHandle(patMuonsH, "outer");
    patMuSysVertices = makeTransientVerticesFromMuSystems(bFieldHandle, patMuonSystems);

    if (patMuSysVertices.size() > 0) {
      for (const auto& v : patMuSysVertices) {
        _themusys["patMu_o"] = theMuSys(v.position());
        _thevtxtree["patMu_o"]->Fill();
      }
    }
  }

}

void
MuPairVertexer::beginJob()
{
  _thevtxtree["patMu_g"] = fs->make<TTree>("patMuons_globalTrack", "");
  branchTTree(_thevtxtree["patMu_g"], _themusys["patMu_g"]);

  _thevtxtree["patMu_i"] = fs->make<TTree>("patMuons_innerTrack", "");
  branchTTree(_thevtxtree["patMu_i"], _themusys["patMu_i"]);

  _thevtxtree["patMu_o"] = fs->make<TTree>("patMuons_outerTrack", "");
  branchTTree(_thevtxtree["patMu_o"], _themusys["patMu_o"]);
}

void
MuPairVertexer::endJob()
{

}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
MuPairVertexer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.add("mupvtx", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MuPairVertexer);