import FWCore.ParameterSet.Config as cms

TrigRecoEffi = cms.EDAnalyzer('TrigRecoEffi',
        muTrack = cms.InputTag("displacedStandAloneMuons"),
        genParticle = cms.InputTag("genParticles"),
        genJet = cms.InputTag("ak4GenJets"),
        genMet = cms.InputTag("genMetTrue"),
        recoMet = cms.InputTag("pfMet"),
        trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
        trigPath = cms.string('HLT_DoubleMu3_DCA_PFMET50_PFMHT60'),
        processName = cms.string('HLT')
        )
