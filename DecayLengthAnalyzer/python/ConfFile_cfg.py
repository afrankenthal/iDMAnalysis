import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("DecayLengthAnalyzer")
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#Passing_Command_Line_Arguments_T
options = VarParsing.VarParsing('analysis')

options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-0p1_18858911_AOD.root'
options.outputFile = 'histo.root'
options.maxEvents = -1

options.parseArguments()

# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMessageLogger
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMessageLoggerParameters
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('detailedInfo', 'critical', 'cerr'),
    categories   = cms.untracked.vstring('DecayLengthAnalyzer'),

    critical     = cms.untracked.PSet( threshold = cms.untracked.string('ERROR') ),
    detailedInfo = cms.untracked.PSet(
        threshold   = cms.untracked.string('DEBUG'),
        DecayLengthAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(10))
    ),
    cerr         = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO'),
        DecayLengthAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(100))
    ),
    debugModules = cms.untracked.vstring('decaylengthana')
)

process.options   = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
)

process.decaylengthana = cms.EDAnalyzer('DecayLengthAnalyzer',
    _genParticles = cms.InputTag('genParticles'),
    _tracks = cms.InputTag("generalTracks"),
    _globalMuons = cms.InputTag("globalMuons"),
    _saMuons = cms.InputTag("standAloneMuons"),
    _rsaMuons = cms.InputTag("refittedStandAloneMuons"),
    _dgMuons = cms.InputTag("displacedGlobalMuons"),
    _dsaMuons = cms.InputTag("displacedStandAloneMuons"),
    _muons = cms.InputTag("muons")
)

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string(options.outputFile),
    closeFileFast = cms.untracked.bool(True)
)

process.p = cms.Path(process.decaylengthana)
