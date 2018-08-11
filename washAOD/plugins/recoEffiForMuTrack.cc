#include "Firefighter/washAOD/interface/recoEffiForMuTrack.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

recoEffiForMuTrack::recoEffiForMuTrack(const edm::ParameterSet& ps) :
  muTrackTag_(ps.getParameter<edm::InputTag>("muTrack")),
  genParticleTag_(ps.getParameter<edm::InputTag>("genParticle")),
  muTrackToken_(consumes<reco::TrackCollection>(muTrackTag_)),
  genParticleToken_(consumes<reco::GenParticleCollection>(genParticleTag_))
{
  usesResource("TFileService");
}

recoEffiForMuTrack::~recoEffiForMuTrack() = default;

void
recoEffiForMuTrack::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("muTrack", edm::InputTag("displacedStandAloneMuons"));
  desc.add<edm::InputTag>("genParticle", edm::InputTag("genParticles"));
  descriptions.add("recoEffiForMuTrack", desc);
}

void
recoEffiForMuTrack::beginJob()
{
  muTrackT_ = fs->make<TTree>("recoEffiForMuTrack", "");

  muTrackT_->Branch("nMatched", &nMatched_, "nMatched/i");
  muTrackT_->Branch("genPt",   &genPt_);
  muTrackT_->Branch("genEta",  &genEta_);
  muTrackT_->Branch("genPhi",  &genPhi_);
  muTrackT_->Branch("genVxy",  &genVxy_);
  muTrackT_->Branch("genVz",   &genVz_);
  muTrackT_->Branch("genDr",   &genDr_);
  muTrackT_->Branch("recoPt",  &recoPt_);
  muTrackT_->Branch("recoEta", &recoEta_);
  muTrackT_->Branch("recoPhi", &recoPhi_);
  muTrackT_->Branch("recoDxy", &recoDxy_);
  muTrackT_->Branch("recoDz",  &recoDz_);
  muTrackT_->Branch("recoVxy", &recoVxy_);
  muTrackT_->Branch("recoVz",  &recoVz_);
  muTrackT_->Branch("deltaR",  &deltaR_);
}

void
recoEffiForMuTrack::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;

  iEvent.getByToken(muTrackToken_, muTrackHandle_);
  if (!muTrackHandle_.isValid()) {
    LogVerbatim("recoEffiForMuTrack") << "recoEffiForMuTrack::analyze: Error in getting muTrack product from Event!"
      << endl;
    return;
  }
  iEvent.getByToken(genParticleToken_, genParticleHandle_);
  if (!genParticleHandle_.isValid()) {
    LogVerbatim("recoEffiForMuTrack") << "recoEffiForMuTrack::analyze: Error in getting genParticle product from Event!"
      << endl;
    return;
  }

  int nAccpted = count_if((*genParticleHandle_).begin(), (*genParticleHandle_).end(),
      [](const reco::GenParticle& g){
        return abs(g.pdgId())==13
           and g.isHardProcess()
           and abs(g.eta())<2.4
           and abs(g.vertex().rho())<740  // decay inside CMS
           and abs(g.vz())<960;
        });
  if (nAccpted<4) return;

  genPt_  .clear(); genPt_  .reserve(4);
  genEta_ .clear(); genEta_ .reserve(4);
  genPhi_ .clear(); genPhi_ .reserve(4);
  genVxy_ .clear(); genVxy_ .reserve(4);
  genVz_  .clear(); genVz_  .reserve(4);
  genDr_  .clear(); genDr_  .reserve(4);
  recoPt_ .clear(); recoPt_ .reserve(4);
  recoEta_.clear(); recoEta_.reserve(4);
  recoPhi_.clear(); recoPhi_.reserve(4);
  recoDxy_.clear(); recoDxy_.reserve(4);
  recoDz_ .clear(); recoDz_ .reserve(4);
  recoVxy_.clear(); recoVxy_.reserve(4);
  recoVz_ .clear(); recoVz_ .reserve(4);
  deltaR_ .clear(); deltaR_ .reserve(4);

  // MC match
  vector<int> genMuIdx{};
  for (size_t ig(0); ig!=genParticleHandle_->size(); ++ig) {
    reco::GenParticleRef gp(genParticleHandle_, ig);
    if (abs(gp->pdgId())==13 and gp->isHardProcess() and abs(gp->eta())<2.4
        and abs(gp->vertex().rho())<740 and abs(gp->vz())<960) {
      genMuIdx.push_back(ig);
    }
  }

  // deltaR between mu pairs
  map<int, float> genMuIdDr{};
  for (size_t mu_i(0); mu_i!=genMuIdx.size(); ++mu_i) {
    reco::GenParticleRef ref_i(genParticleHandle_, genMuIdx[mu_i]);
    for (size_t mu_j(mu_i+1); mu_j!=genMuIdx.size(); ++mu_j) {
      reco::GenParticleRef ref_j(genParticleHandle_, genMuIdx[mu_j]);
      if (ref_i->vertex() != ref_j->vertex()) continue;
      float dR = deltaR(*(ref_i.get()), *(ref_j.get()));
      genMuIdDr.emplace(genMuIdx[mu_i], dR);
      genMuIdDr.emplace(genMuIdx[mu_j], dR);
    }
  }

  // vtxing between reco mu pairs
  ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

  map<int, pair<float, float>> recoMuIdVtx{};
  for (size_t mu_i(0); mu_i!=muTrackHandle_->size(); ++mu_i) {
    reco::TrackRef ref_i(muTrackHandle_, mu_i);
    for (size_t mu_j(mu_i+1); mu_j!=muTrackHandle_->size(); ++mu_j) {
      reco::TrackRef ref_j(muTrackHandle_, mu_j);
      if (deltaR(*(ref_i.get()), *(ref_j.get())) > 0.4) continue;
      if (ref_i->charge() == ref_j->charge()) continue;

      vector<reco::TransientTrack> t_tks{};
      t_tks.push_back(theB->build(&ref_i));
      t_tks.push_back(theB->build(&ref_j));
      KalmanVertexFitter kvf(true);
      TransientVertex tv = kvf.vertex(t_tks);
      if (!tv.isValid()) continue;

      reco::Vertex _vtx = reco::Vertex(tv);
      float _vxy = sqrt(_vtx.x()*_vtx.x() + _vtx.y()*_vtx.y());
      float _vz  = _vtx.z();
      recoMuIdVtx.emplace(int(mu_i), make_pair(_vxy, _vz));
      recoMuIdVtx.emplace(int(mu_j), make_pair(_vxy, _vz));
      break;

    }
  }
  for (size_t i(0); i!=muTrackHandle_->size(); ++i) {
    if (recoMuIdVtx.find(int(i)) != recoMuIdVtx.end()) continue;
    recoMuIdVtx.emplace(int(i), make_pair(0., 0.));
  }

  //cout<<endl;
  //for (size_t i(0); i!=muTrackHandle_->size(); ++i) {
  //  cout<<i<<"\t"<<recoMuIdVtx[i].first<<" "<<recoMuIdVtx[i].second<<endl;
  //}
  
  vector<int> matchedGenMuIdx{};
  int iTk = 0;
  for (const reco::Track& muTrack : *muTrackHandle_) {
    iTk = &muTrack - &(muTrackHandle_->at(0));
    for (const int genMuid : genMuIdx) {
      if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), genMuid) != matchedGenMuIdx.end()) continue;
      reco::GenParticleRef genMu(genParticleHandle_, genMuid);
      if (deltaR(muTrack, *(genMu.get()))>0.3) continue;
      if (muTrack.charge()!=genMu->charge()) continue;
      matchedGenMuIdx.push_back(genMuid);

      genPt_  .push_back(genMu->pt());
      genEta_ .push_back(genMu->eta());
      genPhi_ .push_back(genMu->phi());
      genVxy_ .push_back(genMu->vertex().rho());
      genVz_  .push_back(genMu->vz());
      genDr_  .push_back(genMuIdDr[genMuid]);
      recoPt_ .push_back(muTrack.pt());
      recoEta_.push_back(muTrack.eta());
      recoPhi_.push_back(muTrack.phi());
      recoDxy_.push_back(muTrack.dxy());
      recoDz_ .push_back(muTrack.dz());
      recoVxy_.push_back(recoMuIdVtx[iTk].first);
      recoVz_ .push_back(recoMuIdVtx[iTk].second);
      deltaR_ .push_back(deltaR(muTrack, *(genMu.get())));

      break;
    }
  }
  nMatched_ = matchedGenMuIdx.size();

  // Fill the rest GEN info
  for (const int muId : genMuIdx) {
    if (find(matchedGenMuIdx.begin(), matchedGenMuIdx.end(), muId) != matchedGenMuIdx.end()) continue;
    reco::GenParticleRef genMu(genParticleHandle_, muId);

    genPt_  .push_back(genMu->pt());
    genEta_ .push_back(genMu->eta());
    genPhi_ .push_back(genMu->phi());
    genVxy_ .push_back(genMu->vertex().rho());
    genVz_  .push_back(genMu->vz());
    genDr_  .push_back(genMuIdDr[muId]);
  }

  muTrackT_->Fill();

  return;

}

void
recoEffiForMuTrack::endJob() {}
