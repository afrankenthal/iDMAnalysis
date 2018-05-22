import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#Passing_Command_Line_Arguments_T
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
    options.outputFile = 'histo.root'
else:
    options.maxEvents = -1


# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMessageLogger
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMessageLoggerParameters
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('detailedInfo', 'critical', 'cerr'),
    categories   = cms.untracked.vstring('DecayLengthAnalyzer', 'FwkReport'),

    critical     = cms.untracked.PSet( threshold = cms.untracked.string('ERROR') ),
    detailedInfo = cms.untracked.PSet(
        threshold   = cms.untracked.string('DEBUG'),
        DecayLengthAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(10))
    ),
    cerr         = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO'),
        FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(100) ),
        DecayLengthAnalyzer = cms.untracked.PSet(reportEvery = cms.untracked.int32(100))
    ),
    debugModules = cms.untracked.vstring('decaylengthana')
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

# from PhysicsTools.HepMCCandAlgos.goodStandAloneMuonTrackMCMatch_cfi import goodStandAloneMuonTrackMCMatch
# process.stamumcmatch = goodStandAloneMuonTrackMCMatch.clone()
# process.stamumcmatch.src = cms.InputTag('standAloneMuons')

# from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch
# process.muonmatch = muonMatch.clone()

from Firefighter.washAOD.DecayLengthAnalyzer_cfi import decaylengthana
process.decaylengthana = decaylengthana.clone()

from Firefighter.washAOD.TrigEffiAnalyzer_cfi import trigeffiana
process.trigeffiana = trigeffiana.clone()

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string(options.outputFile),
    closeFileFast = cms.untracked.bool(True)
)

process.p = cms.Path(process.decaylengthana + process.trigeffiana)
