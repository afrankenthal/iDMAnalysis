#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TTree.h"
#include "TLorentzVector.h"

class genTuplizer : 
  public edm::one::EDAnalyzer<edm::one::SharedResources>
{
  public:
    explicit genTuplizer(const edm::ParameterSet&);
    ~genTuplizer();

    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    const edm::InputTag genParticleTag_;
    const edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;

    edm::Service<TFileService> fs;
    edm::Handle<reco::GenParticleCollection> genParticleHandle_;

    unsigned int nGenP_;

    std::vector<int> pid_;
    std::vector<int> charge_;
    std::vector<double> pt_, pz_;
    std::vector<double> eta_, phi_;
    std::vector<double> vxy_, vz_;
    std::vector<double> mass_, energy_;

    std::vector<double> pairInvM_;
    std::vector<double> pairDeltaR_;
    std::vector<int>    pairPid_;
    unsigned long long event;
    TTree *genT_;
};

genTuplizer::genTuplizer(const edm::ParameterSet& ps) :
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_))
{
  usesResource("TFileService");
}

genTuplizer::~genTuplizer() = default;

void
genTuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
    descriptions.add("genTuplizer", desc);
}

void
genTuplizer::beginJob()
{
  genT_ = fs->make<TTree>("gen", "");

  genT_->Branch("nGen", &nGenP_, "nGen/i");
  genT_->Branch("pid", &pid_);
  genT_->Branch("charge", &charge_);
  genT_->Branch("pt", &pt_);
  genT_->Branch("pz", &pz_);
  genT_->Branch("eta", &eta_);
  genT_->Branch("phi", &phi_);
  genT_->Branch("mass", &mass_);
  genT_->Branch("energy", &energy_);
  genT_->Branch("gen_vxy", &vxy_);
  genT_->Branch("gen_vz", &vz_);
  genT_->Branch("pairInvM", &pairInvM_);
  genT_->Branch("pairDeltaR", &pairDeltaR_);
  genT_->Branch("pairPid", &pairPid_);
  genT_->Branch("event", &event);
}

void
genTuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogVerbatim("genTuplizer") << "genTuplizer::analyze: Error in getting genParticle product from Event!"
      << endl;
      return;
  }
    const edm::EventAuxiliary& aux = iEvent.eventAuxiliary();
    event = aux.event();
  //nGenP_ = (*genParticleHandle_).size();
  nGenP_ = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){return g.isHardProcess();});// and abs(g.pdgId())>8;});

  pid_.clear(); pid_.reserve(nGenP_);
  charge_.clear(); charge_.reserve(nGenP_);
  pt_.clear(); pt_.reserve(nGenP_);
  pz_.clear(); pz_.reserve(nGenP_);
  eta_.clear(); eta_.reserve(nGenP_);
  phi_.clear(); phi_.reserve(nGenP_);
  mass_.clear(); mass_.reserve(nGenP_);
  energy_.clear(); energy_.reserve(nGenP_);
  vxy_.clear(); vxy_.reserve(nGenP_);
  vz_.clear(); vz_.reserve(nGenP_);

  vector<pair<reco::GenParticleRef, reco::GenParticleRef>> genPairs{};
  for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
    reco::GenParticleRef genA(genParticleHandle_, ig);
    if (!genA->isHardProcess() or abs(genA->pdgId())<9) continue;
    for (size_t jg(ig+1); jg!=genParticleHandle_->size(); ++jg) {
      reco::GenParticleRef genB(genParticleHandle_, jg);
      if (!genB->isHardProcess() or abs(genB->pdgId())<9) continue;
      // conditions to be counted as a pair:
      // 1. opposite pdgId/charge
      // 2. same vertex.
      if (genA->pdgId() + genB->pdgId() != 0) {continue;}
      if (genA->vertex()!=genB->vertex()) {continue;}
      genPairs.emplace_back(genA, genB);
    }
  }
  pairInvM_.clear(); pairInvM_.reserve(genPairs.size());
  pairDeltaR_.clear(); pairDeltaR_.reserve(genPairs.size());
  pairPid_.clear(); pairPid_.reserve(genPairs.size());

  for (const reco::GenParticle& gp : *genParticleHandle_) {
    if (!gp.isHardProcess()) continue; // or abs(gp.pdgId())<9) continue;
    //cout<<gp.pdgId()<<":\t"<<gp.vx()<<"\t"<<gp.vy()<<"\t"<<gp.vz()<<"\t"<<gp.eta()<<"\t"<<gp.phi()<<"\t"<<gp.pt()<<endl;
    //cout<<gp.pdgId()<<":\t"<<gp.statusFlags().flags_<<endl;
    pid_.push_back(gp.pdgId());
    charge_.push_back(gp.charge());
    pt_.push_back(gp.pt());
    pz_.push_back(gp.pz());
    eta_.push_back(gp.eta());
    phi_.push_back(gp.phi());
    mass_.push_back(gp.mass());
    energy_.push_back(gp.energy());

    vxy_.push_back(gp.vertex().rho());
    vz_.push_back(gp.vz());
  }

  for (const auto& gp : genPairs) {
    double deta = gp.first->eta() - gp.second->eta();
    double dphi = gp.first->phi() - gp.second->phi();
    pairDeltaR_.push_back(sqrt(deta*deta + dphi*dphi));

    TLorentzVector vec_a(gp.first->px(), gp.first->py(), gp.first->pz(), gp.first->energy());
    TLorentzVector vec_b(gp.second->px(), gp.second->py(), gp.second->pz(), gp.second->energy());
    pairInvM_.push_back((vec_a+vec_b).M());
    pairPid_.push_back(abs(gp.first->pdgId()));
  }

  genT_->Fill();

  return;

}

void genTuplizer::endJob() {}

// define this as a plug-in
DEFINE_FWK_MODULE(genTuplizer);
