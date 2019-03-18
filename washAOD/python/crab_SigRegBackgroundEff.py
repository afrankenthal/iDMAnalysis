from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#sample = 'Mchi-5p25_dMchi-0p5_ctau-10'
#sample = 'ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia'
sample = 'WWTo2L2Nu_v3'
#sample = 'W2JetsToLNu'
#sample = 'DYJetsToTauTau'
#sample = 'TTJets_DiLept'
#sample = 'WWJJToLNuLNu'
#sample = 'WZTo3LNu'
#sample = 'ZJetsToNuNu'

config.General.requestName = 'washAOD_analyzer_' + sample
config.General.workArea = 'crab'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'SigRegEff_cfg.py'
#config.JobType.maxMemoryMB = 4000
#config.JobType.numCores = 1

#config.Data.outputPrimaryDataset = 'iDM_2018_MC'
config.Data.outputDatasetTag = sample
config.Data.splitting = 'Automatic'
#config.Data.unitsPerJob = 1
config.Data.publication = False
#config.Data.userInputFiles = open('../data/iDM/2018/' + sample + '.list').readlines()
#config.Data.inputDataset = '/store/mc/RunIIAutumn18DRPremix/ZZTo4L_TuneCP5_13TeV_powheg_pythia8/AODSIM/102X_upgrade2018_realistic_v15_ext1-v2/'


#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/ZZTo2L2Nu/'
#'config.Data.inputDataset = '/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'

#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/W2JetsToLNu/'
#config.Data.inputDataset = '/W2JetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM'

config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/WWTo2L2Nu_v3/'
#config.Data.inputDataset = '/WWTo2L2Nu_13TeV-powheg/RunIISummer16DR80Premix-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/AODSIM'
config.Data.inputDataset = '/WWTo2L2Nu_NNPDF31_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
#-------------------------------------#
#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/DYJetsToTauTauForcedMuDecay/'
#config.Data.inputDataset = '/DYJetsToTauTau_ForcedMuDecay_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM'

#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/TTJetsDiLept/'
#config.Data.inputDataset = '/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
#
#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/TTdilepton/'
#config.Data.inputDataset = '/TT_DiLept_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'

#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/WWJJToLNuLNu/'
#config.Data.inputDataset = '/WWJJToLNuLNu_QCD_noTop_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'
#
#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/WZTo3LNu/'
#config.Data.inputDataset = '/WZTo3LNu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'
#
#config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds/ZJetsToNuNu/'
#config.Data.inputDataset = '/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'


config.Site.ignoreGlobalBlacklist = True
#config.Site.blacklist = ['T3_RU_FIAN', 'T3_US_MIT', 'T3_US_UCD',
#'T3_CO_Uniandes', 'T3_US_NotreDame', 
#'T3_IN_PUHEP', 'T3_UK_ScotGrid_ECDF', 'T3_BY_NCPHEP',
#'T2_CH_CERN_HLT', 'T3_CH_CERN_HelixNebula', 'T3_IN_TIFRCloud',
#'T0_CH_CERN', 'T3_GR_IASA', 'T3_CN_PKU', 'T0_CH_CSCS_HPC',
#'T3_IR_IPM', 'T2_RU_ITEP', 'T3_US_JHU', 'T3_US_Kansas',
#'T3_US_FSU', 'T3_KR_UOS', 'T3_CH_PSI']
#config.Site.whitelist = ["T3_US_FNALLPC"]
config.Site.storageSite = 'T3_US_FNALLPC'
