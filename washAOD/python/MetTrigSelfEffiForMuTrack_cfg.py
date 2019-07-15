import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')
options.register('test',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run for a test or not")
options.register('year',
                 2017,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "sample of the year")


options.parseArguments()

if options.test:
    import platform
    if 'cmslpc' in platform.node():
        if options.year == 2017:
            options.inputFiles = 'root://cmseos.fnal.gov///store/user/mreid/iDM/AOD_Samples/Mchi-60_dMchi-20/lifetime_10mm/iDM_Mchi-60_dMchi-20_mZD-150_Wchi2-1p00e-03_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_9906774_ctau-10_AOD.root'
        if options.year == 2018:
            options.inputFiles = 'root://cmseos.fnal.gov////store/user/as2872/iDM/AOD_Samples/2018/Mchi-60_dMchi-20_ctau-10/iDM_Mchi-60_dMchi-20_mZDinput-150_ctau-0_9985539_AOD_ctau-10.root'
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

if options.year == 2017:
    process.GlobalTag.globaltag = '94X_mc2017_realistic_v15'
if options.year == 2018:
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v15'

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
    wantSummary = cms.untracked.bool(True),
    # For CRAB need numberOfThreads = 1, otherwise set to 8
    numberOfThreads = cms.untracked.uint32(1)
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

#from iDMSkimmer.washAOD.genTuplizer_cfi import genTuplizer
#process.GEN = genTuplizer.clone()

## Trigger-reco efficiency
from iDMSkimmer.washAOD.MetTrigSelfEffiForMuTrack_cfi import MetTrigSelfEffiForMuTrack
process.RECO_dsa = MetTrigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'),trigPath0 = cms.string('HLT_Mu17'))
process.RECO_dsa50 = MetTrigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), trigPath0 = cms.string('HLT_Mu50'))
process.RECO_gbm = MetTrigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), trigPath0 = cms.string('HLT_Mu17'), muTrack = cms.InputTag("globalMuons"))
process.RECO_gbm50 = MetTrigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), trigPath0 = cms.string('HLT_Mu50'), muTrack = cms.InputTag("globalMuons"))

## constructing the path
if options.year == 2017:
    process.p = cms.Path(
                         process.RECO_dsa
                         + process.RECO_dsa50
                         + process.RECO_gbm
                         + process.RECO_gbm50
                         )

if options.year == 2018:
    process.p = cms.Path(
                         process.RECO_dsa
                         + process.RECO_dsa50
                         + process.RECO_gbm
                         + process.RECO_gbm50
                         )
