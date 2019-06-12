#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "iDMSkimmer/washAOD/interface/genTuplizer.h"
#include "iDMSkimmer/washAOD/interface/recoEffiForMuTrack.h"
#include "iDMSkimmer/washAOD/interface/trigEffiForMuTrack.h"
#include "iDMSkimmer/washAOD/interface/trigSelfEffiForMuTrack.h"
#include "iDMSkimmer/washAOD/interface/SignalRegionEffi.h"
#include "iDMSkimmer/washAOD/interface/TrigRecoEffi.h"
#include "iDMSkimmer/washAOD/interface/iDMAnalyzer.h"

DEFINE_FWK_MODULE(genTuplizer);
DEFINE_FWK_MODULE(recoEffiForMuTrack);
DEFINE_FWK_MODULE(trigEffiForMuTrack);
DEFINE_FWK_MODULE(trigSelfEffiForMuTrack);
DEFINE_FWK_MODULE(SignalRegionEffi);
DEFINE_FWK_MODULE(TrigRecoEffi);
DEFINE_FWK_MODULE(iDMAnalyzer);
