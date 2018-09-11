import FWCore.ParameterSet.Config as cms

recoEffiForMuTrack = cms.EDAnalyzer('recoEffiForMuTrack',
    muTrack = cms.InputTag("displacedStandAloneMuons"),
    genParticle = cms.InputTag("genParticles"),
		genJet = cms.InputTag("ak4GenJets"),
		genMet = cms.InputTag("genMetTrue"),
		recoMet = cms.InputTag("pfMet")
  )
