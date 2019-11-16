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
        False,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.bool,
        "Run on data (1) or MC (0)")
options.register('Run2018D',
        False,
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

if options.year == 2016:
    if options.data:
         process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v7'
    else:
        process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'
if options.year == 2017:
    if options.data:
        process.GlobalTag.globaltag = '94X_dataRun2_v11'
    else:
        process.GlobalTag.globaltag = '94X_mc2017_realistic_v17'
if options.year == 2018:
    if options.data:
        if options.Run2018D:
            process.GlobalTag.globaltag = '102X_dataRun2_Prompt_v15'
        # else it's 2018 A, B, or C
        else: 
            process.GlobalTag.globaltag = '102X_dataRun2_v12'
    # else it's MC
    else:
        process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v20'

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

## Electron and photon VID
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
dataFormat = DataFormat.AOD
switchOnVIDElectronIdProducer(process, dataFormat)
switchOnVIDPhotonIdProducer(process, dataFormat)
# define which IDs we want to produce
if options.year == '2016': 
	id_e_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff']
	id_ph_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff']
	recoPhotonPath = 'egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-loose'
	recoElectronPath= 'egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose'
else:
	id_e_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff']
	id_ph_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V2_cff']
	recoPhotonPath= 'egmPhotonIDs:cutBasedPhotonID-Fall17-94X-V2-loose'
	recoElectronPath= 'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-loose'

#add them to the VID producer
for idmod in id_e_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
for idmod in id_ph_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)


## Main iDM analyzer
from iDMSkimmer.washAOD.iDMAnalyzer_cfi import iDMAnalyzer
process.ntuples_gbm = iDMAnalyzer.clone(corrLabel = corrLabel, muTrack2 = cms.InputTag('globalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'),photonPath =cms.string(recoPhotonPath), electronPath = cms.string(recoElectronPath), isData = cms.untracked.bool(options.data))
process.ntuples_dgm = iDMAnalyzer.clone(corrLabel = corrLabel, muTrack2 = cms.InputTag('displacedGlobalMuons'), trigPath = cms.string('HLT_PFMET120_PFMHT120_IDTight'), photonPath = cms.string(recoPhotonPath), electronPath = cms.string(recoElectronPath), isData = cms.untracked.bool(options.data))

if options.data:
    process.p = cms.Path(
        process.metFilters
        + process.egmGsfElectronIDSequence
        + process.egmPhotonIDSequence
        + process.ak4PFCHSL1FastL2L3ResidualCorrectorChain
        + process.correctionTermsPfMetType1Type2
        + process.correctionTermsPfMetType0RecoTrack
        + process.correctionTermsPfMetMult
        + process.pfMetT0rtT1Txy
        + process.ntuples_gbm
        + process.ntuples_dgm
    )
else:
    process.p = cms.Path(
        process.metFilters
        + process.egmGsfElectronIDSequence 
        + process.egmPhotonIDSequence
        + process.correctionTermsPfMetType1Type2
        + process.correctionTermsPfMetType0RecoTrack
        + process.correctionTermsPfMetMult
        + process.pfMetT0rtT1Txy
        + process.ak4PFCHSL1FastL2L3CorrectorChain
        + process.ntuples_gbm
        + process.ntuples_dgm
    )
