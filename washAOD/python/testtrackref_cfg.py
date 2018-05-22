import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("testtrackref")
options = VarParsing.VarParsing('analysis')
options.register('test',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run for a test or not")

options.parseArguments()

if options.test == 1:
    import platform
    if 'cmslpc' in platform.node():
        options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_AOD.root'
    elif 'lxplus' in platform.node():
        options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
    options.maxEvents = 100
    options.outputFile = 'testtrackref.root'
else:
    options.maxEvents = -1

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('detailedInfo', 'cerr'),
    categories   = cms.untracked.vstring('Firefighter/testtrackref', 'FwkReport'),
    
    detailedInfo = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG')
        ),
    cerr         = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO'),
        FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(100))
        )
    )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
    )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
    )

process.testtrackref = cms.EDAnalyzer('TestTrackRef'
                                      )
process.p = cms.Path(process.testtrackref)