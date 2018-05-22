import FWCore.ParameterSet.Config as cms

from SimGeneral.HepPDTESSource.pythiapdt_cfi import *
muTrackCandidates = cms.EDProducer("ConcreteChargedCandidateProducer", # ConcreteStandAloneMuonCandidateProducer
                                   src=cms.InputTag("standAloneMuons"),
                                   particleType=cms.string('mu+')
                                   )

saMuTrackCands = muTrackCandidates.clone()
dsaMuTrackCands = muTrackCandidates.clone(
                    src = cms.InputTag("displacedStandAloneMuons")
                    )
rsaMuTrackCands = muTrackCandidates.clone(
                    src = cms.InputTag("refittedStandAloneMuons")
                    )
saMuUAVTrackCands = muTrackCandidates.clone(
                    src=cms.InputTag("standAloneMuons", "UpdatedAtVtx")
                    )
globalMuTrackCands = muTrackCandidates.clone(
                    src=cms.InputTag("globalMuons")
                    )
dgMuTrackCands = muTrackCandidates.clone(
                    src=cms.InputTag("displacedGlobalMuons")
                    )
cosmicMuTrackCands = muTrackCandidates.clone(
                    src=cms.InputTag("cosmicMuons")
                    )
cosmicMu1LegTrackCands = muTrackCandidates.clone(
                    src=cms.InputTag("cosmicMuons1Leg")
                    )

makeMuTrackCandidates = cms.Sequence(
    saMuTrackCands +
    dsaMuTrackCands +
    rsaMuTrackCands +
    saMuUAVTrackCands +
    globalMuTrackCands +
    dgMuTrackCands +
    cosmicMuTrackCands +
    cosmicMu1LegTrackCands
    )