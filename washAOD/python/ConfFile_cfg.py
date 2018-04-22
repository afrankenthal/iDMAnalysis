import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("DecayLengthAnalyzer")
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#Passing_Command_Line_Arguments_T
options = VarParsing.VarParsing('analysis')

import platform
if 'cmslpc' in platform.node():
    options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_AOD.root'
elif 'lxplus' in platform.node():
    options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-1_12714105_AOD.root'
options.outputFile = 'histo.root'
options.maxEvents = -1

options.parseArguments()

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

from PhysicsTools.HepMCCandAlgos.goodStandAloneMuonTrackMCMatch_cfi import goodStandAloneMuonTrackMCMatch
process.stamumcmatch = goodStandAloneMuonTrackMCMatch.clone()
process.stamumcmatch.src = cms.InputTag('standAloneMuons')

from PhysicsTools.PatAlgos.mcMatchLayer0.muonMatch_cfi import muonMatch
process.muonmatch = muonMatch.clone()

process.decaylengthana = cms.EDAnalyzer('DecayLengthAnalyzer',
    _genParticles = cms.InputTag('genParticles'),
    _tracks = cms.InputTag("generalTracks"),
    _globalMuons = cms.InputTag("globalMuons"),
    _saMuons = cms.InputTag("standAloneMuons"),
    _saMuonsUAV = cms.InputTag("standAloneMuons","UpdatedAtVtx"),
    _rsaMuons = cms.InputTag("refittedStandAloneMuons"),
    _dgMuons = cms.InputTag("displacedGlobalMuons"),
    _dsaMuons = cms.InputTag("displacedStandAloneMuons"),
    _muons = cms.InputTag("muons")
)

process.trigeffiana = cms.EDAnalyzer('TrigEffiAnalyzer',
    _trigResults = cms.InputTag("TriggerResults","","HLT"),
    _trigEvent = cms.InputTag("hltTriggerSummaryAOD","","HLT"),
    _genParticles = cms.InputTag('genParticles'),
    _saMuons = cms.InputTag("standAloneMuons"),
    _rsaMuons = cms.InputTag("refittedStandAloneMuons"),
    _dsaMuons = cms.InputTag("displacedStandAloneMuons"),
    _muons = cms.InputTag("muons"),
    _trigPaths = cms.untracked.vstring(
        'DST_DoubleMu3_noVtx_CaloScouting_v4',
        'HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx_v4',
        'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10',
        'HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v11'
    ),
    _trigFilters = cms.untracked.vstring(
        'hltDoubleMu3L3FilteredNoVtx',
        'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered5NoVtx',
        'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered6NoVtx',
        'hltL3pfL1sDoubleMu155ORTripleMu444L1f0L2pf0TwoMuL3PreFiltered8NoVtx'
    )
)

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string(options.outputFile),
    closeFileFast = cms.untracked.bool(True)
)

process.p = cms.Path(process.decaylengthana + process.trigeffiana)
