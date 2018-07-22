import FWCore.ParameterSet.Config as cms

recoEffiForMuTrack = cms.EDAnalyzer('recoEffiForMuTrack',
    muTrack = cms.InputTag("displacedStandAloneMuons"),
    genParticle = cms.InputTag("genParticles")
  )
