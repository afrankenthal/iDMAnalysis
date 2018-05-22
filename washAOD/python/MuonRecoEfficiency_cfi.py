import FWCore.ParameterSet.Config as cms

muonrecoeffi = cms.EDAnalyzer('MuonRecoEfficiency',
    _genParticles = cms.InputTag('genParticles'),
    _muTrack = cms.InputTag('globalMuTrackCands'),
    _mergedMCMatch = cms.InputTag('mergedMCMatch'),
    _nmuons = cms.uint32(4)
)