#include "CommonTools/UtilAlgos/interface/PhysObjectMatcher.h"
#include "CommonTools/UtilAlgos/interface/MCMatchSelector.h"
#include "CommonTools/UtilAlgos/interface/MatchByDRDPt.h"
#include "CommonTools/UtilAlgos/interface/MatchLessByDPt.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

typedef reco::PhysObjectMatcher<
  reco::TrackCollection,
  reco::GenParticleCollection,
  reco::MCMatchSelector<reco::TrackCollection::value_type,
            reco::GenParticleCollection::value_type>,
  reco::MatchByDRDPt<reco::TrackCollection::value_type,
             reco::GenParticleCollection::value_type>
> MCtkMatcher;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MCtkMatcher);