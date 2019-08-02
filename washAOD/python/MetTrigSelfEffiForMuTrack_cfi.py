import FWCore.ParameterSet.Config as cms

MetTrigSelfEffiForMuTrack = cms.EDAnalyzer('MetTrigSelfEffiForMuTrack',
  muTrack = cms.InputTag('displacedStandAloneMuons'),
  genEvt = cms.InputTag('generator'),
  recoJet = cms.InputTag('ak4PFJets'),
  recoMet = cms.InputTag('pfMet'),
  trigResult = cms.InputTag('TriggerResults', '', 'HLT'),
  trigEvent = cms.InputTag('hltTriggerSummaryAOD', '', 'HLT'),
  trigPath = cms.string('Defaultshouldntbecalled!'),
  trigPath0 = cms.string('Defaultshouldntbecalled!'),
  processName = cms.string('HLT'),
  nMuons = cms.int32(2)
)
