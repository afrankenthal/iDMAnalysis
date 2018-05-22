#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "TTree.h"

class GenMuonAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
  explicit GenMuonAnalyzer(const edm::ParameterSet&);
  ~GenMuonAnalyzer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  struct theMu
  {
    theMu(){}

    template<typename T>
    theMu(const T& m)
    {
      _pt = m.pt();
      _eta = m.eta();
      _phi = m.phi();
      _vxy = m.vertex().rho();
      _vz = m.vertex().Z();
    }

    double _pt, _eta, _phi, _vxy, _vz;
  };
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  void branchTTree(TTree*, theMu&);

  edm::EDGetTokenT<reco::GenParticleCollection> genParticles;
  edm::Service<TFileService> fs;

  TTree* _basicMuTree;
  theMu _basicMu;
};

void
GenMuonAnalyzer::branchTTree(TTree* t, theMu& m) {
  t->Branch("pt", &m._pt, "pt/D");
  t->Branch("eta", &m._eta, "eta/D");
  t->Branch("phi", &m._phi, "phi/D");
  t->Branch("vxy", &m._vxy, "vxy/D");
  t->Branch("vz", &m._vz,   "vz/D");
}


GenMuonAnalyzer::GenMuonAnalyzer(const edm::ParameterSet&)
{
  usesResource("TFileService");
  genParticles = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
}

GenMuonAnalyzer::~GenMuonAnalyzer(){}

void
GenMuonAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  Handle<reco::GenParticleCollection> genParticlesH;
  iEvent.getByToken(genParticles, genParticlesH);

  unsigned int acceptance = std::count_if(genParticlesH->begin(), genParticlesH->end(),
          [](const auto& mu) {return abs(mu.pdgId())==13 && abs(mu.eta())<2.4;});
  if (acceptance<4) return;

  for (const auto& mu : *genParticlesH) {
    if (abs(mu.pdgId()) != 13) continue;
    _basicMu = theMu(mu);
    _basicMuTree->Fill();
  }
}

void
GenMuonAnalyzer::beginJob()
{
  _basicMuTree = fs->make<TTree>("genMu","");
  branchTTree(_basicMuTree, _basicMu);
}

void
GenMuonAnalyzer::endJob(){}

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
void
GenMuonAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.add("genmuonana", desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenMuonAnalyzer);