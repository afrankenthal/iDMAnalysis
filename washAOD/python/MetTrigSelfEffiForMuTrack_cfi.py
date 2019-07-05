import FWCore.ParameterSet.Config as cms

MetTrigSelfEffiForMuTrack = cms.EDAnalyzer('MetTrigSelfEffiForMuTrack',
    muTrack = cms.InputTag("displacedStandAloneMuons"),
    trigResult = cms.InputTag("TriggerResults","","HLT"),
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    trigPath = cms.string('HLT_DoubleL2Mu23NoVtx_2Cha'),
    trigPath0 = cms.string('HLT_DoubleL2Mu23NoVtx_2Cha'),
    processName = cms.string('HLT'),
    nMuons = cms.int32(2)
  )
