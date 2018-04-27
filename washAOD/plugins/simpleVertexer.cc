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
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void branchTTree(TTree*, theMuSys&);

  edm::Service<TFileService> fs;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  edm::EDGetTokenT<reco::TrackCollection> saMuons;

  std::map<std::string, theMuSys> _themusys;
  std::map<std::string, TTree*> _thevtxtree;
    
};

void
simpleVertexer::branchTTree(TTree* t, theMuSys& p) {
  t->Branch("vxy", &p._vxy, "vxy/D");
  t->Branch("vz", &p._vz, "vz/D");
}

simpleVertexer::simpleVertexer(const edm::ParameterSet& iC)
{
  usesResource("TFileService");
  genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
  saMuons = consumes<reco::TrackCollection>(edm::InputTag("standAloneMuons"));
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

  Handle<reco::TrackCollection> saMuonsH;
  iEvent.getByToken(saMuons, saMuonsH);

  if (!saMuonsH.isValid()) {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Exception during event number: " << iEvent.id() << "\n";
  } else {
    edm::LogInfo("Firefighter/simpleVertexer")
    << "Found: " << saMuonsH->size() << " standAloneMuons" << "\n";

    // Transform Track to TransientTrack
    using MuSystem = std::pair<Ptr<reco::Track>, Ptr<reco::Track>>;
    using MuSystemCollection = std::vector<MuSystem>;
    MuSystemCollection musystems{};
    for (size_t iMu(0); iMu!=saMuonsH->size(); ++iMu) {
      Ptr<reco::Track> muA(saMuonsH, iMu);
      for (size_t jMu(iMu+1); jMu!=saMuonsH->size(); ++jMu) {
        Ptr<reco::Track> muB(saMuonsH, jMu);
        if (muA->charge()*muB->charge() > 0) continue;
        if (deltaR(*muA.get(), *muB.get()) > 1) continue;
        musystems.push_back(std::make_pair(muA, muB));
      }
    }
    // get the builder
    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", theB);
    // do the conversion:
    std::vector<reco::TransientTrack> saMuon_tks{};
    if (musystems.size() > 0) {
      for (const auto& mupair : musystems) {
        reco::TransientTrack tt1(theB->build(mupair.first.get()));
        reco::TransientTrack tt2(theB->build(mupair.second.get()));
        saMuon_tks.push_back(tt1);
        saMuon_tks.push_back(tt2);

        KalmanVertexFitter kvf(true);
        TransientVertex tv = kvf.vertex(saMuon_tks);
        _themusys["saMuon"] = theMuSys(reco::Vertex::Point(tv.position()));
        // std::cout << "Position: " << vp << std::endl;
        _thevtxtree["saMuon"]->Fill();

        saMuon_tks.clear();
      }
    }
  }
}

void
simpleVertexer::beginJob()
{
  _thevtxtree["saMuon"] = fs->make<TTree>("standAloneMuon", "");
  branchTTree(_thevtxtree["saMuon"], _themusys["saMuon"]);
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