import FWCore.ParameterSet.Config as cms

SignalRegionEffi = cms.EDAnalyzer('SignalRegionEffi',
        muTrack = cms.InputTag("displacedStandAloneMuons"),
        genParticle = cms.InputTag("genParticles"),
        genJet = cms.InputTag("ak4GenJets"),
        genMet = cms.InputTag("genMetTrue"),
        recoMet = cms.InputTag("pfMet"),
        recoJet = cms.InputTag("ak4PFJets"),
        pileups = cms.InputTag("addPileupInfo"),
        genEvt = cms.InputTag("generator")
        )
