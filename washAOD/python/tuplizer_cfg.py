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
      options.inputFiles = 'root://cmseos.fnal.gov///store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2/180720-105132/SIDMmumu_Mps-200_MZp-1p2_ctau-1p2_10980067_AOD.root'
    elif 'lxplus' in platform.node():
        options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
    options.maxEvents = -1
    options.outputFile = 'test.root'
else:
    options.maxEvents = -1

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
     destinations   =  cms.untracked.vstring('messages', 'cerr'),
     debugModules   = cms.untracked.vstring('*'),
     categories     = cms.untracked.vstring('FwkReport'),
     messages       = cms.untracked.PSet(
                        extension = cms.untracked.string('.txt'),
                        threshold =  cms.untracked.string('ERROR')
                    ),
     cerr           = cms.untracked.PSet(
                        FwkReport = cms.untracked.PSet(
                            reportEvery = cms.untracked.int32(10000)
                        )
                    )
    )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfThreads = cms.untracked.uint32(8)
)
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

from Firefighter.washAOD.genTuplizer_cfi import genTuplizer
process.GEN = genTuplizer.clone()
from Firefighter.washAOD.recoEffiForMuTrack_cfi import recoEffiForMuTrack
process.RECO_muTrackEffi = recoEffiForMuTrack.clone()

process.p = cms.Path(process.GEN + process.RECO_muTrackEffi)
