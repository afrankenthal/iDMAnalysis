from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

sample = 'Mchi-5p25_dMchi-0p5_ctau-10'

config.General.requestName = 'skimmer_analyzer_' + sample
config.General.workArea = 'crab'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'SigRegEff_cfg.py'
#config.JobType.maxMemoryMB = 4000
#config.JobType.numCores = 1

config.Data.outputPrimaryDataset = 'iDM_2018_MC'
config.Data.outputDatasetTag = sample
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018'
config.Data.publication = False
config.Data.userInputFiles = open('../data/iDM/2018/' + sample + '.list').readlines()

config.Site.ignoreGlobalBlacklist = True
#config.Site.blacklist = ['T3_RU_FIAN', 'T3_US_MIT', 'T3_US_UCD',
#'T3_CO_Uniandes', 'T3_US_NotreDame', 
#'T3_IN_PUHEP', 'T3_UK_ScotGrid_ECDF', 'T3_BY_NCPHEP',
#'T2_CH_CERN_HLT', 'T3_CH_CERN_HelixNebula', 'T3_IN_TIFRCloud',
#'T0_CH_CERN', 'T3_GR_IASA', 'T3_CN_PKU', 'T0_CH_CSCS_HPC',
#'T3_IR_IPM', 'T2_RU_ITEP', 'T3_US_JHU', 'T3_US_Kansas',
#'T3_US_FSU', 'T3_KR_UOS', 'T3_CH_PSI']
config.Site.whitelist = ["T3_US_FNALLPC"]
config.Site.storageSite = 'T3_US_FNALLPC'
