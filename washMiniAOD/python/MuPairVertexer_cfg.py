import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("mupairvertexer")
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#Passing_Command_Line_Arguments_T
options = VarParsing.VarParsing('analysis')
options.register('test',
                 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run for a test or not")

options.parseArguments()

if options.test == 1:
    import platform
    if 'cmslpc' in platform.node():
        options.inputFiles = 'file:/eos/uscms/store/user/wsi/standaloneComp/SIDMmumu_Mps-200_MZp-1p2_ctau-1_41695249_MINIAOD.root'
    elif 'lxplus' in platform.node():
        options.inputFiles = 'file:/eos/user/w/wsi/prelimSamples/SIDMmumu_Mps-200_MZp-1p2_ctau-0p1_24660204_MINIAOD.root'
    options.maxEvents = 100
    options.outputFile = 'mupairvertexer.root'
else:
    options.maxEvents = -1

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('detailedInfo', 'cerr'),
    categories   = cms.untracked.vstring('Firefighter/MuPairVertexer', 'FwkReport'),
    
    detailedInfo = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG')
        ),
    cerr         = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO'),
        FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(100))
        )
    )

# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTransientTracks
# process.load("Configuration.Geometry.GeometryExtended2017Plan1Reco_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_mc2017_realistic_v10', '')
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")


process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    SkipEvent = cms.untracked.vstring( "Error: uninitialized ProxyBase used" )
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

process.mupvtx = cms.EDAnalyzer('MuPairVertexer'
)
process.p = cms.Path(process.mupvtx)
