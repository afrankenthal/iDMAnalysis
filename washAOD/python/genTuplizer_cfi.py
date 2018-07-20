import FWCore.ParameterSet.Config as cms

genTuplizer = cms.EDAnalyzer('genTuplizer',
    genParticle = cms.InputTag("genParticles")
)
