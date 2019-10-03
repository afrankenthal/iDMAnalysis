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
options.register('data',
        0,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.bool,
        "Run on data (1) or MC (0)")
options.register('Run2018D',
        0,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.bool,
        "Is this sample Run2018D (different GT), yes (1) or no (0)")


options.parseArguments()

if options.test:
    import platform
    if 'cmslpc' in platform.node():
        if options.year == 2017:
            options.inputFiles = 'root://cmseos.fnal.gov///store/user/mreid/iDM/AOD_Samples/Mchi-60_dMchi-20/lifetime_10mm/iDM_Mchi-60_dMchi-20_mZD-150_Wchi2-1p00e-03_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_9906774_ctau-10_AOD.root'
        if options.year == 2018:
            #options.inputFiles = 'root://cmseos.fnal.gov////store/group/lpcmetx/iDM/AOD/2018/GenFilter_1or2jets_icckw1_drjj0_xptj80_xqcut20_qcut20/Mchi-60p0_dMchi-20p0_ctau-10/iDM_Mchi-60p0_dMchi-20p0_mZDinput-150p0_ctau-0_1or2jets_icckw1_drjj0_xptj80_xqcut20_9576064_AOD_ctau-10.root'
            if options.data:
                #options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018C/MET/AOD/17Sep2018-v1/60000/FF66CD3E-969F-3644-8A5F-542FD5B56676.root'
                #options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018C/MET/AOD/17Sep2018-v1/100000/15CAEDFA-86F6-924D-B216-C81ACFE0DCEA.root'
                #options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018A/MET/AOD/17Sep2018-v1/60001/ABDA7B68-11D2-3E4E-B0A7-5F28E1FAB9ED.root'
                #options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018B/MET/AOD/17Sep2018-v1/120000/F099DF9A-6FF6-594A-9865-EB7125104EDF.root'
                #options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018B/MET/AOD/17Sep2018-v1/120000/6ECBC797-E226-FA47-BBAE-C79150CCBA41.root'
                # data AOD test
                options.inputFiles = 'root://cmsxrootd.fnal.gov////store/data/Run2018B/MET/AOD/17Sep2018-v1/120000/572AAC76-E629-DC44-A27A-0F327B99FA7A.root'
            else:
                # background AOD test
                options.inputFiles = 'root://cmsxrootd.fnal.gov////store/mc/RunIIAutumn18DRPremix/ZJetsToNuNu_HT-400To600_13TeV-madgraph/AODSIM/102X_upgrade2018_realistic_v15-v2/260000/FFF1E16F-A07D-ED45-9970-E077E64935DD.root'
                # signal AOD test
                #options.inputFiles = 'root://cmseos.fnal.gov////store/group/lpcmetx/iDM/AOD/2018/signal/Mchi-60p0_dMchi-20p0_ctau-100/iDM_Mchi-60p0_dMchi-20p0_mZDinput-150p0_ctau-0_1or2jets_icckw1_drjj0_xptj80_xqcut20_1547134_AOD_ctau-100.root'
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
    if options.data:
        if options.Run2018D:
            process.GlobalTag.globaltag = '102X_dataRun2_Prompt_v14'
        # else it's 2018 A, B, or C
        else: 
            #process.GlobalTag.globaltag = '102X_dataRun2_Sep2018ABC_v2'
            process.GlobalTag.globaltag = '102X_dataRun2_v11'
    # else it's MC
    else:
        #process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v15'
        process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v18'

process.MessageLogger = cms.Service("MessageLogger",
        destinations   =  cms.untracked.vstring('messages', 'cerr'),
        statistics     =  cms.untracked.vstring('messages', 'cerr'),
        debugModules   = cms.untracked.vstring('*'),
        categories     = cms.untracked.vstring('FwkReport'),
        messages       = cms.untracked.PSet(
            extension = cms.untracked.string('.txt'),
            threshold =  cms.untracked.string('WARNING')
            ),
        cerr           = cms.untracked.PSet(
            threshold = cms.untracked.string('WARNING'),
            WARNING = cms.untracked.PSet(
                reportEvery = cms.untracked.int32(10)
                ),
            INFO = cms.untracked.PSet(
                reportEvery = cms.untracked.int32(10)
                ),
            FwkReport = cms.untracked.PSet(
                reportEvery = cms.untracked.int32(10000)
                )
            )
        )

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(True),
        #numberOfThreads = cms.untracked.uint32(8)
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

process.load('iDMSkimmer.washAOD.myMETFilters_cff')
process.load('JetMETCorrections.Configuration.JetCorrectors_cff')

## Gen-level event analyzer
from iDMSkimmer.washAOD.genTuplizer_cfi import genTuplizer
process.GEN = genTuplizer.clone()

## Jet corrector to use
if options.data:
    corrLabel = cms.InputTag('ak4PFCHSL1FastL2L3ResidualCorrector')
else:
    corrLabel = cms.InputTag('ak4PFCHSL1FastL2L3Corrector')

## MET correction
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetType0RecoTrack_cff")
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetType1Type2_cff")
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetMult_cff")
process.load("JetMETCorrections.Type1MET.correctedMet_cff")

## Main iDM analyzer
from iDMSkimmer.washAOD.iDMAnalyzer_cfi import iDMAnalyzer
process.ntuples_gbm = iDMAnalyzer.clone(corrLabel = corrLabel, muTrack2 = cms.InputTag('globalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), isData = cms.untracked.bool(options.data))
process.ntuples_dgm = iDMAnalyzer.clone(corrLabel = corrLabel, muTrack2 = cms.InputTag('displacedGlobalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), isData = cms.untracked.bool(options.data))
#process.SREffi_rsa = iDMAnalyzer.clone(muTrack2 = cms.InputTag('refittedStandAloneMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
#process.SREffi_dsa = iDMAnalyzer.clone(trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))
#process.SREffi_sam = iDMAnalyzer.clone(muTrack2 = cms.InputTag('standAloneMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'))

if options.data:
    process.p = cms.Path(
        process.metFilters
        + process.ak4PFCHSL1FastL2L3ResidualCorrectorChain
        + process.correctionTermsPfMetType1Type2
        + process.correctionTermsPfMetType0RecoTrack
        + process.correctionTermsPfMetMult
        + process.pfMetT0rtT1Txy
        + process.ntuples_gbm
        + process.ntuples_dgm
        #+ process.SREffi_rsa
        #+ process.SREffi_dsa
        #+ process.SREffi_sam
    )
else:
    process.p = cms.Path(
        process.GEN
        + process.metFilters
        + process.correctionTermsPfMetType1Type2
        + process.correctionTermsPfMetType0RecoTrack
        + process.correctionTermsPfMetMult
        + process.pfMetT0rtT1Txy
        + process.ak4PFCHSL1FastL2L3CorrectorChain
        + process.ntuples_gbm
        + process.ntuples_dgm
        #+ process.SREffi_rsa
        #+ process.SREffi_dsa
        #+ process.SREffi_sam
    )
