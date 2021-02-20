from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#sample = 'WZTo3LNu'
#sample = 'WWJJToLNuLNu'
#sample = 'ZZTo2L2Nu'
#sample = 'WWTo2L2Nu'

#sample = 'DYJetsToTauTau'
#sample = 'DYJetsToLL'

#sample = 'WJetsToLNu'
#sample = 'WJetsToLNu_HT-70To100'
#sample = 'WJetsToLNu_HT-100To200'
#sample = 'WJetsToLNu_HT-200To400'
#sample = 'WJetsToLNu_HT-400To600'

#sample = 'ZJetsToNuNu_HT-100To200'
#sample = 'ZJetsToNuNu_HT-200To400'
sample = 'ZJetsToNuNu_HT-400To600'

#sample = 'TTTo2L2Nu'
#sample = 'TT_diLept'
#sample = 'TTJets'

#sample = 'QCD_bEnriched_HT100to200'
#sample = 'QCD_bEnriched_HT200to300'
#sample = 'QCD_bEnriched_HT300to500'
#sample = 'QCD_bEnriched_HT500to700'
#sample = 'QCD_bEnriched_HT700to1000'
#sample = 'QCD_bEnriched_HT1000to1500'
#sample = 'QCD_bEnriched_HT1500to2000'
#sample = 'QCD_bEnriched_HT2000toInf'

#sample = 'WMuNu'
input_data = {

'WZTo3LNu':'/WZTo3LNu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
'WWJJToLNuLNu':'/WWJJToLNuLNu_QCD_noTop_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
'ZZTo2L2Nu':'/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
'WWTo2L2Nu':'/WWTo2L2Nu_NNPDF31_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',

'DYJetsToTauTau':'/DYJetsToTauTau_ForcedMuDecay_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
'DYJetsToLL':'/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',

'WJetsToLNu':'/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'WJetsToLNu_HT-70To100':'/WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'WJetsToLNu_HT-100To200':'/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'WJetsToLNu_HT-200To400':'/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'WJetsToLNu_HT-400To600':'/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',

'ZJetsToNuNu_HT-100To200':'/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'ZJetsToNuNu_HT-200To400':'/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'ZJetsToNuNu_HT-400To600':'/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'ZJetsToNuNu_HT-600To800':'/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'ZJetsToNuNu_HT-800To1200':'/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'ZJetsToNuNu_HT-1200To2500':'/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'ZJetsToNuNu_HT-2500ToInf':'/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',

'TTTo2L2Nu':'/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'TT_diLept':'/TT_DiLept_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
'TTJets':'/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',

'QCD_bEnriched_HT100to200':'/QCD_bEnriched_HT100to200_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
'QCD_bEnriched_HT200to300':'/QCD_bEnriched_HT200to300_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT300to500':'/QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT500to700':'/QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT700to1000':'/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT1000to1500':'/QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT1500to2000':'/QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
'QCD_bEnriched_HT2000toInf':'/QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',

'WMuNu': '/WToMuNu_M-200_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
}

config.Data.inputDataset = input_data[sample] 
config.General.requestName = 'skimmer_analyzer_' + sample
config.General.workArea = 'crab'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'SigRegEff_cfg.py'
#config.JobType.psetName = 'MetTrigSelfEffiForMuTrack_cfg.py'
config.JobType.psetName = 'iDMAnalyzer_cfg.py'
#config.JobType.maxMemoryMB = 4000
#config.JobType.numCores = 1

#config.Data.outputPrimaryDataset = 'iDM_2018_MC'
config.Data.outputDatasetTag = 'crab_' + sample
config.Data.splitting = 'Automatic'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 100
#config.Data.totalUnits = 100 
config.Data.publication = False
config.Data.publishWithGroupName = True
#config.Data.userInputFiles = open('../data/iDM/2018/' + sample + '.list').readlines()
#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/'
config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_thirdrun'


#config.Site.ignoreGlobalBlacklist = True
#config.Site.blacklist = ['T3_RU_FIAN', 'T3_US_MIT', 'T3_US_UCD',
#'T3_CO_Uniandes', 'T3_US_NotreDame', 
#'T3_IN_PUHEP', 'T3_UK_ScotGrid_ECDF', 'T3_BY_NCPHEP',
#'T2_CH_CERN_HLT', 'T3_CH_CERN_HelixNebula', 'T3_IN_TIFRCloud',
#'T0_CH_CERN', 'T3_GR_IASA', 'T3_CN_PKU', 'T0_CH_CSCS_HPC',
#'T3_IR_IPM', 'T2_RU_ITEP', 'T3_US_JHU', 'T3_US_Kansas',
#'T3_US_FSU', 'T3_KR_UOS', 'T3_CH_PSI']
#config.Site.whitelist = ["T3_US_FNALLPC"]
config.Site.storageSite = 'T3_US_FNALLPC'
