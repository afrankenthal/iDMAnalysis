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
                 2018,
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
           options.inputFiles = 'file:/eos/uscms/store/user/wsi/MCSIDM/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12/180726-222103/SIDMmumu_Mps-200_MZp-1p2_ctau-0p12_10268877_AOD.root'
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

from iDMAnalysis.skimmer.genTuplizer_cfi import genTuplizer
process.GEN = genTuplizer.clone()

## reco efficiency
#from iDMAnalysis.skimmer.recoEffiForMuTrack_cfi import recoEffiForMuTrack
#process.RECO_dsa = recoEffiForMuTrack.clone()
#process.RECO_dgm = recoEffiForMuTrack.clone(muTrack = cms.InputTag("displacedGlobalMuons"))
#process.RECO_rsa = recoEffiForMuTrack.clone(muTrack = cms.InputTag("refittedStandAloneMuons"))
#process.RECO_gbm = recoEffiForMuTrack.clone(muTrack = cms.InputTag("globalMuons"))

## trigger efficiency in terms of events
from iDMAnalysis.skimmer.trigEffiForMuTrack_cfi import trigEffiForMuTrack
### 2017 & 2018
process.TRIG_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_DoubleMu3_DCA_PFMET50_PFMHT60'))
process.TRIG_dsa_HLT_PFMET110_PFMHT110 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET110_PFMHT110_IDTight'))
process.TRIG_dsa_HLT_PFMET120_PFMHT120 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
process.TRIG_gbm_HLT_PFMET120_PFMHT120 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), muTrack=cms.InputTag('globalMuons'))
process.TRIG_dsa_HLT_PFMET130_PFMHT130 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET130_PFMHT130_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight'))
process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100 = trigEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight'))

## trigger efficiency itself
from iDMAnalysis.skimmer.trigSelfEffiForMuTrack_cfi import trigSelfEffiForMuTrack
### 2017 & 2018
process.TRIGself_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_DoubleMu3_DCA_PFMET50_PFMHT60'), nMuons=cms.int32(2))
process.TRIGself_dsa_HLT_PFMET110_PFMHT110 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET110_PFMHT110_IDTight'), nMuons=cms.int32(2))
process.TRIGself_dsa_HLT_PFMET120_PFMHT120 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), nMuons=cms.int32(2))
process.TRIGself_gbm_HLT_PFMET120_PFMHT120 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), nMuons=cms.int32(2), muTrack=cms.InputTag('globalMuons'))
process.TRIGself_dsa_HLT_PFMET130_PFMHT130 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_PFMET130_PFMHT130_IDTight'), nMuons=cms.int32(2))
process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70_IDTight'))
process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight'))
process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight'))
process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100 = trigSelfEffiForMuTrack.clone(trigPath = cms.string('HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight'))


## constructing the path
if options.year == 2017:
    process.p = cms.Path(process.GEN
                         + process.RECO_dsa
                         + process.RECO_dgm
                         + process.RECO_rsa
                         + process.RECO_gbm
                         + process.TRIG_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 
                         + process.TRIG_dsa_HLT_PFMET110_PFMHT110 
                         + process.TRIG_dsa_HLT_PFMET120_PFMHT120 
                         + process.TRIG_dsa_HLT_PFMET130_PFMHT130 
                         + process.TRIGself_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 
                         + process.TRIGself_dsa_HLT_PFMET110_PFMHT110 
                         + process.TRIGself_dsa_HLT_PFMET120_PFMHT120 
                         + process.TRIGself_dsa_HLT_PFMET130_PFMHT130 
                         )

if options.year == 2018:
    process.p = cms.Path(process.GEN
                         #+ process.RECO_dsa
                         # + process.RECO_dgm
                         # + process.RECO_rsa
                         # + process.RECO_gbm
                         # + process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70
                         # + process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80 
                         # + process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90 
                         # + process.TRIG_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100
                         # + process.TRIG_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 
                         # + process.TRIG_dsa_HLT_PFMET110_PFMHT110 
                          + process.TRIG_dsa_HLT_PFMET120_PFMHT120 
                          + process.TRIG_gbm_HLT_PFMET120_PFMHT120
                         # + process.TRIG_dsa_HLT_PFMET130_PFMHT130 
                         # + process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET70_PFMHT70
                         # + process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80 
                         # + process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90 
                         # + process.TRIGself_dsa_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100
                         # + process.TRIGself_dsa_HLT_DoubleMu3_DCA_PFMET50_PFMHT60 
                         # + process.TRIGself_dsa_HLT_PFMET110_PFMHT110 
                          + process.TRIGself_dsa_HLT_PFMET120_PFMHT120 
                          + process.TRIGself_gbm_HLT_PFMET120_PFMHT120
                         # + process.TRIGself_dsa_HLT_PFMET130_PFMHT130 
                         )
