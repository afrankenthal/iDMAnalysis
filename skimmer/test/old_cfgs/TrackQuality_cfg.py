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
            #options.inputFiles = 'root://cmseos.fnal.gov////store/user/as2872/iDM/AOD_Samples/2018/Mchi-60_dMchi-20_ctau-10/iDM_Mchi-60_dMchi-20_mZDinput-150_ctau-0_9985539_AOD_ctau-10.root'
            options.inputFiles = 'root://cmseos.fnal.gov////store/group/lpcmetx/iDM/AOD/2018/GenFilter_1or2jets_icckw1_drjj0_xptj80_xqcut20_qcut20/Mchi-60p0_dMchi-20p0_ctau-10/iDM_Mchi-60p0_dMchi-20p0_mZDinput-150p0_ctau-0_1or2jets_icckw1_drjj0_xptj80_xqcut20_9576064_AOD_ctau-10.root'
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
        # numberOfThreads = cms.untracked.uint32(8)
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

#from iDM_analysis_skimming.skimmer.genTuplizer_cfi import genTuplizer
#process.GEN = genTuplizer.clone()

## Signal Region efficiency
from iDMAnalysis.skimmer.TrackQuality_cfi import TrackQuality
process.SREffi_dsa = TrackQuality.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
process.SREffi_gbm = TrackQuality.clone(muTrack = cms.InputTag('globalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
process.SREffi_rsa = TrackQuality.clone(muTrack = cms.InputTag('refittedStandAloneMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
process.SREffi_dgm = TrackQuality.clone(muTrack = cms.InputTag('displacedGlobalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
process.SREffi_sam = TrackQuality.clone(muTrack = cms.InputTag('standAloneMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))

## constructing the path
if options.year == 2017:
    process.p = cms.Path(#process.GEN
             process.SREffi_dsa
            + process.SREffi_gbm
            + process.SREffi_rsa
            + process.SREffi_dgm
            + process.SREffi_sam
            )

if options.year == 2018:
    process.p = cms.Path(#process.GEN
             process.SREffi_dsa
            + process.SREffi_gbm
            + process.SREffi_rsa
            + process.SREffi_dgm
            + process.SREffi_sam
            )
