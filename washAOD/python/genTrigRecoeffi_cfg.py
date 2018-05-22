import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')
options.register('test',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run for a test or not")

options.parseArguments()

if options.test:
    import platform
    if 'cmslpc' in platform.node():
        options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_AOD.root'
    elif 'lxplus' in platform.node():
        options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
    options.maxEvents = 100
    options.outputFile = 'skim.root'
else:
    options.maxEvents = -1


process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))
process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32(options.maxEvents)
            )
process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(options.inputFiles)
            )

process.TFileService = cms.Service("TFileService",
        fileName = cms.string(options.outputFile),
        closeFileFast = cms.untracked.bool(True)
        )

process.load("Firefighter.washAOD.produceCandidateFromTrack_cff")
process.load("Firefighter.washAOD.mcMatchSeq_cff")
process.load("Firefighter.washAOD.MuonRecoEfficiency_cff")

from Firefighter.washAOD.GenMuonAnalyzer_cfi import genmuana
process.genmuana = genmuana.clone()

from Firefighter.washAOD.TrigEffiAnalyzer_cfi import trigeffiana
process.trigeffiana = trigeffiana.clone()

process.recoeffi = cms.Sequence(process.makeMuTrackCandidates
                     * process.mcMatchFromTrackCands
                     * process.makeMuTrackEfficiencyFromMatched)

process.p = cms.Path(process.genmuana
                    +process.trigeffiana
                    + process.recoeffi)