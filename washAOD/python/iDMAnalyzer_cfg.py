import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("USER")
options = VarParsing.VarParsing('analysis')
options.register('test',
        0,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.int,
        "Run for a test (1) or not (0)"
        )
options.register('year',
        2018,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.int,
        "sample year (2018/2017/2016)"
        )
options.register('data',
        False,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.bool,
        "Run on data (1) or MC (0)"
        )
options.register('Run2018D',
        False,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.bool,
        "Is this sample Run2018D (different GT), yes (1) or no (0)")
options.register('numThreads',
        8,
        VarParsing.VarParsing.multiplicity.singleton,
        VarParsing.VarParsing.varType.int,
        "Number of threads (for CRAB vs non-CRAB execution)")

options.parseArguments()

if options.test:
    import platform
    if 'cmslpc' in platform.node():
        if options.year == 2017:
            options.inputFiles = 'root://cmseos.fnal.gov///store/user/mreid/iDM/AOD_Samples/Mchi-60_dMchi-20/lifetime_10mm/iDM_Mchi-60_dMchi-20_mZD-150_Wchi2-1p00e-03_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_9906774_ctau-10_AOD.root'
        elif options.year == 2018:
            #options.inputFiles = 'root://cmseos.fnal.gov////store/group/lpcmetx/iDM/AOD/2018/GenFilter_1or2jets_icckw1_drjj0_xptj80_xqcut20_qcut20/Mchi-60p0_dMchi-20p0_ctau-10/iDM_Mchi-60p0_dMchi-20p0_mZDinput-150p0_ctau-0_1or2jets_icckw1_drjj0_xptj80_xqcut20_9576064_AOD_ctau-10.root'
            if options.data:
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

globaltag = ''
if options.year == 2016:
    globaltag = '80X_dataRun2_2016SeptRepro_v7' if options.data else '80X_mcRun2_asymptotic_2016_TrancheIV_v8'
elif options.year == 2017:
    globaltag = '94X_dataRun2_v11' if options.data else '94X_mc2017_realistic_v17'
elif options.year == 2018:
    # Check if it's 2018 D data
    if options.data and options.Run2018D:
        globaltag = '102X_dataRun2_Prompt_v15'
    # or 2018 A, B, or C data
    elif options.data:
        globaltag = '102X_dataRun2_v12'
    # else it's MC
    else:
        globaltag = '102X_upgrade2018_realistic_v20'

process.GlobalTag.globaltag = globaltag

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
    numberOfThreads = cms.untracked.uint32(options.numThreads)
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
process.Timing = cms.Service("Timing",
    summaryOnly = cms.untracked.bool(True),
    useJobReport = cms.untracked.bool(True)
    )

## Import custom MET filters
process.load('iDMSkimmer.washAOD.myMETFilters_cff')

## Jet correctors to use
process.load('JetMETCorrections.Configuration.JetCorrectors_cff')
if options.data:
    jetCorrector = cms.InputTag('ak4PFCHSL1FastL2L3ResidualCorrector')
else:
    jetCorrector = cms.InputTag('ak4PFCHSL1FastL2L3Corrector')

## MET correction
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetType0RecoTrack_cff")
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetType1Type2_cff")
process.load("JetMETCorrections.Type1MET.correctionTermsPfMetMult_cff")
process.load("JetMETCorrections.Type1MET.correctedMet_cff")

## Add DeepCSV b-tagging for 2016
process.load("RecoBTag.ImpactParameter.impactParameter_cff")
process.load("RecoBTag.SecondaryVertex.secondaryVertex_cff")
process.load("RecoBTag.SoftLepton.softLepton_cff")
process.load("RecoBTag.Combined.combinedMVA_cff")
process.load("RecoBTag.CTagging.cTagging_cff")
process.load("RecoBTag.Combined.deepFlavour_cff")
if options.year == 2016:
    process.btagging = cms.Sequence()
    from iDMSkimmer.washAOD.myBTagging_cff import addBTaggingAK4CHS
    addBTaggingAK4CHS(process)

## Now can define the same tags for all years
bTagProbb = cms.InputTag('pfDeepCSVJetTags:probb')
bTagProbbb = cms.InputTag('pfDeepCSVJetTags:probbb')

## Electron and photon VID
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
dataFormat = DataFormat.AOD
switchOnVIDElectronIdProducer(process, dataFormat)
switchOnVIDPhotonIdProducer(process, dataFormat)

## Define which IDs we want to produce
if options.year == 2016: 
	id_e_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff']
	id_ph_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff']
	photonID = cms.InputTag('egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-loose')
	electronID = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto')
else:
	id_e_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff']
	id_ph_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V2_cff']
	photonID = cms.InputTag('egmPhotonIDs:cutBasedPhotonID-Fall17-94X-V2-loose')
	electronID = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-veto')

## add them to the VID producer
for idmod in id_e_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
for idmod in id_ph_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)

## Build muon candidate from dSA
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff")
from iDMSkimmer.washAOD.muonsFromdSA_cff import * 
process.muonsFromdSA = muonsFromdSA.clone()
## Now build extra muon timing variables
from RecoMuon.MuonIdentification.muonTiming_cfi import *
process.muontimingFromdSA = muontiming.clone(MuonCollection = cms.InputTag("muonsFromdSA"))

## Main iDM analyzer
from iDMSkimmer.washAOD.iDMAnalyzer_cfi import iDMAnalyzer
process.ntuples_gbm = iDMAnalyzer.clone(
    jetCorrector = jetCorrector,
    muTrack2 = cms.InputTag('globalMuons'),
    photonID = photonID,
    electronID = electronID,
    isData = cms.untracked.bool(options.data),
    bTagProbb = bTagProbb,
    bTagProbbb = bTagProbbb,
    rho = cms.InputTag("fixedGridRhoFastjetAll")
)
#process.ntuples_dgm = iDMAnalyzer.clone(
#    jetCorrector = jetCorrector,
#    muTrack2 = cms.InputTag('displacedGlobalMuons'),
#    photonPath = cms.string(recoPhotonPath),
#    electronPath = cms.string(recoElectronPath),
#    isData = cms.untracked.bool(options.data),
#    bTagProbb=cms.string(bTagProbb),
#    bTagProbbb=cms.string(bTagProbbb),
#    bTagCombine=cms.string(bTagCombined),
#    rho = cms.InputTag("fixedGridRhoFastjetAll")
#)

process.commonSequence = cms.Sequence(
    process.muonsFromdSA
    + process.muontimingFromdSA
    + process.egmGsfElectronIDSequence 
    + process.egmPhotonIDSequence
    + process.correctionTermsPfMetType1Type2
    + process.correctionTermsPfMetType0RecoTrack
    + process.correctionTermsPfMetMult
    + process.pfMetT0rtT1Txy
    + process.ntuples_gbm
    #+ process.ntuples_dgm
    )

if options.data:
    if options.year == 2016:
        process.p = cms.Path(
            process.metFilters
            + process.btagging
            + process.ak4PFCHSL1FastL2L3ResidualCorrectorChain
            + process.commonSequence
        )
    else:
        process.p = cms.Path(
            process.metFilters
            + process.ak4PFCHSL1FastL2L3ResidualCorrectorChain
            + process.commonSequence
        )
else:
    if options.year == 2016:
        process.p = cms.Path(
            process.metFilters
            + process.btagging
            + process.ak4PFCHSL1FastL2L3CorrectorChain
            + process.commonSequence
        )
    else:
        process.p = cms.Path(
            process.metFilters
            + process.ak4PFCHSL1FastL2L3CorrectorChain
            + process.commonSequence
        )
