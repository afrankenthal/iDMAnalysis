import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')

options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_AOD.root'
options.outputFile = 'skimtest.root'
options.maxEvents = -1

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

process.allMuPtResolution = cms.EDAnalyzer("muMCAnalyzer")

process.TFileService = cms.Service("TFileService",
        fileName = cms.string(options.outputFile),
        closeFileFast = cms.untracked.bool(True)
        )

process.out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string('skimmed_' + options.outputFile),
        outputCommands = cms.untracked.vstring(
                        'keep recoGenParticles_genParticles*_*_*',
                        'keep recoMuons_muons*_*_*',
                        'keep recoTracks_*_*_*',
                        'keep *_*_*_USER'
                        )
        )
process.load("Firefighter.washAOD.mcMatchSeq_cff")
process.load("Firefighter.washAOD.produceCandidateFromTrack_cff")
process.p = cms.Path(process.makeMuTrackCandidates * process.mcMatch * process.allMuPtResolution)
# process.ep = cms.EndPath(process.out)