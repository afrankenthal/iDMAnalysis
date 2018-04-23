import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')

options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_AOD.root'
options.outputFile = 'skimtest.root'
options.maxEvents = 10

options.parseArguments()

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32(options.maxEvents)
            )
process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(options.inputFiles)
            )


from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch
process.muonmatch = muonMatch.clone()


trackMuMatch = cms.EDProducer("MCtkMatcher", # cut on deltaR, deltaPt/Pt; pick best by deltaR
    src     = cms.InputTag("standAloneMuons"), # RECO objects to match
    matched = cms.InputTag("genParticles"),   # mc-truth particle collection
    mcPdgId     = cms.vint32(13), # one or more PDG ID (13 = muon); absolute values (see below)
    checkCharge = cms.bool(True), # True = require RECO and MC objects to have the same charge
    mcStatus = cms.vint32(1),     # PYTHIA status code (1 = stable, 2 = shower, 3 = hard scattering)
    maxDeltaR = cms.double(0.5),  # Minimum deltaR for the match
    maxDPtRel = cms.double(0.5),  # Minimum deltaPt/Pt for the match
    resolveAmbiguities = cms.bool(True),     # Forbid two RECO objects to match to the same GEN object
    resolveByMatchQuality = cms.bool(False), # False = just match input in order; True = pick lowest deltaR pair first
)

process.samumatch = trackMuMatch
process.samuUAVmatch = process.samumatch.clone(
                    src = cms.InputTag("standAloneMuons", "UpdatedAtVtx")
                    )
process.dsamumatch = process.samumatch.clone(
                    src = cms.InputTag("displacedStandAloneMuons")
                    )
process.rsamumatch = process.samumatch.clone(
                    src = cms.InputTag("refittedStandAloneMuons")
                    )
process.globalmumatch = process.samumatch.clone(
                    src = cms.InputTag("globalMuons")
                    )
process.dgmumatch = process.samumatch.clone(
                    src = cms.InputTag("displacedGlobalMuons")
                    )

process.out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string(options.outputFile),
        outputCommands = cms.untracked.vstring(
                        'keep recoGenParticles_genParticles*_*_*',
                        'keep recoMuons_muons*_*_*',
                        'keep recoTracks_*_*_*',
                        'keep *_*_*_USER'
                        )
        )

process.p = cms.Path(
                process.muonmatch +
                process.samumatch +
                process.samuUAVmatch +
                process.dsamumatch +
                process.rsamumatch +
                process.globalmumatch +
                process.dgmumatch
                )
process.ep = cms.EndPath(process.out)