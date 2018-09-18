import FWCore.ParameterSet.Config as cms

trigEffiForMuTrack = cms.EDAnalyzer('trigEffiForMuTrack',
    muTrack = cms.InputTag("displacedStandAloneMuons"),
    genParticle = cms.InputTag("genParticles"),
    trigResult = cms.InputTag("TriggerResults","","HLT"),
    trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    #trigPath = cms.string('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx'),
    #trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'),
    trigPath = cms.string('HLT_DoubleMu3_DCA_PFMET50_PFMHT60'),
    processName = cms.string('HLT')
  )
