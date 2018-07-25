#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "Firefighter/washAOD/interface/genTuplizer.h"
#include "Firefighter/washAOD/interface/recoEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"

DEFINE_FWK_MODULE(genTuplizer);
DEFINE_FWK_MODULE(recoEffiForMuTrack);
DEFINE_FWK_MODULE(trigEffiForMuTrack);
