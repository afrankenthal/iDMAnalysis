import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("MuAnalyzer")
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#Passing_Command_Line_Arguments_T
options = VarParsing.VarParsing('analysis')

options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_MINIAOD.root'
options.outputFile = 'histo.root'
options.maxEvents = -1

options.parseArguments()

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('detailedInfo', 'critical', 'cerr'),
    categories   = cms.untracked.vstring('MuAnalyzer', 'FwkReport'),

    critical     = cms.untracked.PSet( threshold = cms.untracked.string('ERROR') ),
    detailedInfo = cms.untracked.PSet(
        threshold   = cms.untracked.string('DEBUG'),
        MuAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(10))
    ),
    cerr         = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO'),
        FwkReport = cms.untracked.PSet(reportEvery = cms.untracked.int32(100)),
        MuAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(100))
    ),
    debugModules = cms.untracked.vstring('muana')
)

process.options   = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
)

process.muana = cms.EDAnalyzer('MuAnalyzer',
	_MuTag = cms.InputTag('slimmedMuons')
)

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string(options.outputFile),
    closeFileFast = cms.untracked.bool(True)
)

process.p = cms.Path(process.muana)