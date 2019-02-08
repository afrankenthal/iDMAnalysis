#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "Firefighter/washAOD/interface/genTuplizer.h"
#include "Firefighter/washAOD/interface/recoEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/trigSelfEffiForMuTrack.h"
#include "Firefighter/washAOD/interface/SignalRegionEffi.h"
#include "Firefighter/washAOD/interface/TrigRecoEffi.h"

DEFINE_FWK_MODULE(genTuplizer);
DEFINE_FWK_MODULE(recoEffiForMuTrack);
DEFINE_FWK_MODULE(trigEffiForMuTrack);
DEFINE_FWK_MODULE(trigSelfEffiForMuTrack);
DEFINE_FWK_MODULE(SignalRegionEffi);
DEFINE_FWK_MODULE(TrigRecoEffi);
