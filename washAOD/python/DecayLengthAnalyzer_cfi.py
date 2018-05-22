import FWCore.ParameterSet.Config as cms

decaylengthana = cms.EDAnalyzer('DecayLengthAnalyzer',
    _genParticles = cms.InputTag('genParticles'),
    _tracks = cms.InputTag("generalTracks"),
    _globalMuons = cms.InputTag("globalMuons"),
    _saMuons = cms.InputTag("standAloneMuons"),
    _saMuonsUAV = cms.InputTag("standAloneMuons","UpdatedAtVtx"),
    _rsaMuons = cms.InputTag("refittedStandAloneMuons"),
    _dgMuons = cms.InputTag("displacedGlobalMuons"),
    _dsaMuons = cms.InputTag("displacedStandAloneMuons"),
    _cosmicMu1Leg = cms.InputTag("cosmicMuons1Leg"),
    _muons = cms.InputTag("muons")
)
