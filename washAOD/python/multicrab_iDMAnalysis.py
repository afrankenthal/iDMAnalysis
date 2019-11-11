#!/usr/bin/env python
"""
This is a small script that does the equivalent of multicrab.
"""
import os
import sys
from optparse import OptionParser

from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

def merge_dicts(*dict_args):
    """
    Given any number of dicts, shallow copy and merge into a new dict,
    precedence goes to key value pairs in latter dicts.
    """
    result = {}
    for dictionary in dict_args:
        result.update(dictionary)
    return result

def getOptions():
    """
    Parse and return the arguments provided by the user.
    """
    usage = ("Usage: %prog --crabCmd CMD [--workArea WAD --crabCmdOpts OPTS --sampleType STYP]"
             "\nThe multicrab command executes 'crab CMD OPTS' for each project directory contained in WAD"
             "\nUse multicrab -h for help")

    parser = OptionParser(usage=usage)

    parser.add_option('-c', '--crabCmd',
                      dest = 'crabCmd',
                      default = '',
                      help = "crab command",
                      metavar = 'CMD')

    parser.add_option('-w', '--workArea',
                      dest = 'workArea',
                      default = '',
                      help = "work area directory (only if CMD != 'submit')",
                      metavar = 'WAD')

    parser.add_option('-o', '--crabCmdOpts',
                      dest = 'crabCmdOpts',
                      default = '',
                      help = "options for crab command CMD",
                      metavar = 'OPTS')

    parser.add_option('-s', '--sampleType',
                      dest = 'sampleType',
                      default = 'custom',
                      help = "Which kind of sample to process ('MC' (default), 'data' or 'custom')",
                      metavar = 'STYP')

    parser.add_option('-y', '--year',
                      dest = 'year',
                      default = '2018',
                      help = "Which year would you like to process ('2018' (default), '2017' or 'custom')",
                      metavar = 'YEAR')

    (options, arguments) = parser.parse_args()

    if arguments:
        parser.error("Found positional argument(s): %s." % (arguments))
    if not options.crabCmd:
        parser.error("(-c CMD, --crabCmd=CMD) option not provided.")
    if options.crabCmd != 'submit':
        if not options.workArea:
            parser.error("(-w WAR, --workArea=WAR) option not provided.")
        if not os.path.isdir(options.workArea):
            parser.error("'%s' is not a valid directory." % (options.workArea))

    return options


def main():

    options = getOptions()

    isData = False
    if options.sampleType == 'data':
        isData = True

    # The submit command needs special treatment.
    if options.crabCmd == 'submit':

        #--------------------------------------------------------
        # This is the base config:
        #--------------------------------------------------------
        from CRABClient.UserUtilities import config, getUsernameFromSiteDB
        config = config()

        if not options.workArea:
            config.General.workArea = 'crab'
        else:
            config.General.workArea = options.workArea
        config.General.transferOutputs = True
        config.General.transferLogs = True

        config.JobType.pluginName = 'Analysis'
        config.JobType.psetName = 'iDMAnalyzer_cfg.py'
        #config.JobType.psetName = 'python/iDMAnalyzer_cfg.py'
        #config.JobType.maxMemoryMB = 4000
        #config.JobType.numCores = 1

        config.Data.splitting = 'Automatic'
        #config.Data.splitting = 'LumiBased'
        #config.Data.unitsPerJob = 100
        if isData == True:
            config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/data_sixthrun'
        else:
            config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/2018/backgrounds_sixthrun'

        config.Data.publication = False
        #config.Data.ignoreLocality = True

        #config.Site.ignoreGlobalBlacklist = True
        #config.Site.whitelist = ['T2_RU_ITEP']
        #config.Site.whitelist = ['T2_DE_*', 'T2_EE_*', 'T2_ES_*', 'T2_FR_*', 'T2_GR_*', 'T2_HU_*', 'T2_IT_*', 'T2_RU_*', 'T2_UK_*']
        config.Site.blacklist = ['T2_TW_NCHC','T2_BE_IIHE']
        #config.Site.blac = ['T3_RU_FIAN', 'T3_US_MIT', 'T3_US_UCD',
        #'T3_CO_Uniandes', 'T3_US_NotreDame', 
        #'T3_IN_PUHEP', 'T3_UK_ScotGrid_ECDF', 'T3_BY_NCPHEP',
        #'T2_CH_CERN_HLT', 'T3_CH_CERN_HelixNebula', 'T3_IN_TIFRCloud',
        #'T0_CH_CERN', 'T3_GR_IASA', 'T3_CN_PKU', 'T0_CH_CSCS_HPC',
        #'T3_IR_IPM', 'T2_RU_ITEP', 'T3_US_JHU', 'T3_US_Kansas',
        #'T3_US_FSU', 'T3_KR_UOS', 'T3_CH_PSI']
        #config.Site.whitelist = ["T3_US_FNALLPC"]
        config.Site.storageSite = 'T3_US_FNALLPC'

        #--------------------------------------------------------

        # Will submit one task for each of these input datasets.
        #------ MC backgrounds -----#

        QCD = {
            #'QCD_HT100to200':'/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'QCD_HT200to300':'/QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT300to500':'/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT500to700':'/QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT700to1000':'/QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT1000to1500':'/QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT1500to2000':'/QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'QCD_HT2000toInf':'/QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        QCD_bEnriched = {
            'QCD_bEnriched_HT100to200':'/QCD_bEnriched_HT100to200_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'QCD_bEnriched_HT200to300':'/QCD_bEnriched_HT200to300_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT300to500':'/QCD_bEnriched_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT500to700':'/QCD_bEnriched_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT700to1000':'/QCD_bEnriched_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT1000to1500':'/QCD_bEnriched_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT1500to2000':'/QCD_bEnriched_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'QCD_bEnriched_HT2000toInf':'/QCD_bEnriched_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM'
        }
        TTbar = {
            #'TTTo2L2Nu':'/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'TTSemiLeptonic':'/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'TTToHadronic':'/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
            #'TT_diLept':'/TT_DiLept_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            'TTJets_NLO': '/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'
            #'TTJets':'/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        SingleTop = {
            'ST_s-channel': '/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v4/AODSIM',
            #'ST_t-channel_top': '/ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ST_t-channel_top_5f': '/ST_t-channel_top_5f_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            #'ST_t-channel_antitop': '/ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ST_t-channel_antitop_5f': '/ST_t-channel_antitop_5f_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'tW_top':'/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v1/AODSIM',
            'tW_antitop':'/ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v1/AODSIM'
        }
        ZJets  = {
            'ZJetsToNuNu_HT-100To200':'/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ZJetsToNuNu_HT-200To400':'/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ZJetsToNuNu_HT-400To600':'/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'ZJetsToNuNu_HT-600To800':'/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ZJetsToNuNu_HT-800To1200':'/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ZJetsToNuNu_HT-1200To2500':'/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'ZJetsToNuNu_HT-2500ToInf':'/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        WJets = {
            #'WJetsToLNu':'/WJetsToLNu_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'WJetsToLNu_HT-70To100':'/WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-100To200':'/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-200To400':'/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-400To600':'/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-600To800':'/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-800To1200':'/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-1200To2500':'/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'WJetsToLNu_HT-2500ToInf':'/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        GJets = {
            'GJets_HT-40to100':'/GJets_HT-40To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'GJets_HT-100to200':'/GJets_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-4cores5k_102X_upgrade2018_realistic_v15-v1/AODSIM',
            'GJets_HT-200to400':'/GJets_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'GJets_HT-400to600':'/GJets_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM',
            'GJets_HT-600toInf':'/GJets_HT-600ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'
        }
        DY = {
            #'DYJetsToTauTau':'/DYJetsToTauTau_ForcedMuDecay_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
            'DYJetsToLL_M-5to50':'/DYJetsToLL_M-5to50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'DYJetsToLL_M-50toInf':'/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        Diboson = {
            'WW': '/WW_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            'WZ': '/WZ_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v3/AODSIM',
            'ZZ': '/ZZ_TuneCP5_13TeV-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v2/AODSIM',
            #'WZTo3LNu':'/WZTo3LNu_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            #'WWJJToLNuLNu':'/WWJJToLNuLNu_QCD_noTop_13TeV-madgraph-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            #'ZZTo2L2Nu':'/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            #'ZZTo2L2Nu_ext2':'/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext2-v2/AODSIM',
            #'WWTo2L2Nu':'/WWTo2L2Nu_NNPDF31_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15-v1/AODSIM'
        }
        Triboson = {
            'WWZ':'/WWZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            'WZZ':'/WZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            'ZZZ':'/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM',
            'WWW':'/WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext1-v2/AODSIM'
        }

        full_MC_samples = merge_dicts(QCD, TTbar, SingleTop, ZJets, WJets, GJets, DY, Diboson, Triboson)

        #------ data -----#

        data_ABC = {
            'MET_Run2018A':'/MET/Run2018A-17Sep2018-v1/AOD',
            'MET_Run2018B':'/MET/Run2018B-17Sep2018-v1/AOD',
            'MET_Run2018C':'/MET/Run2018C-17Sep2018-v1/AOD'
        }
        data_D = {
            'MET_Run2018D':'/MET/Run2018D-PromptReco-v2/AOD'
        }
        single_data = {
        'singleMu_RunA_Sep':'/SingleMuon/Run2018A-17Sep2018-v2/AOD',
        'singleMu_RunB_Sep':'/SingleMuon/Run2018B-17Sep2018-v1/AOD',
        'singleMu_RunC_Sep':'/SingleMuon/Run2018C-17Sep2018-v1/AOD',
        'singleMu_RunD_Jan':'/SingleMuon/Run2018D-22Jan2019-v2/AOD',
        }

        full_data_samples = merge_dicts(data_ABC, data_D)

	#----- MC 2017 -----#
	Diboson_2017 = {
        'WZ': '/WZ_TuneCP5_13TeV-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
        'WW': '/WW_TuneCP5_13TeV-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
        'ZZ':'/ZZ_TuneCP5_13TeV-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM'
        }

        Triboson_2017 = {
        'WWZ':'/WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
        'WZZ':'/WZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
        'ZZZ':'/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
        'WWW':'/WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM'
	}

        DY_2017 = {
                'DYJetsToLL_M-50toInf':'/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11_ext1-v1/AODSIM'
        }
        WJets_2017 = {
                'WJetsToLNu_HT-70To100':'/WJetsToLNu_HT-70To100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-100To200':'/WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
                'WJetsToLNu_HT-200To400':'/WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-400To600':'/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-600To800':'/WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-800To1200':'/WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-1200To2500':'/WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'WJetsToLNu_HT-2500ToInf':'/WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v3/AODSIM'
        }

        TTbar_2017 = {
                'TTJets' : '/TTJets_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM'
        }


        SingleTop_2017 = {
                'ST_s-channel':'/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'ST_t-channel_top':'/ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'ST_t-channel_antitop':'/ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8/RunIIFall17DRPremix-PU2017_new_pmx_94X_mc2017_realistic_v11-v2/AODSIM',
                'tW_top':'/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'tW_anittop':'/ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM'
        }

 	QCD_2017 = {
                'QCD_HT50To100'   :'/QCD_HT50to100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'QCD_HT100To200'  :'/QCD_HT100to200_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
                'QCD_HT200To300'  :'/QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'QCD_HT300To500'  :'/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'QCD_HT500To700'  :'/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
                'QCD_HT700To1000' :'/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v1/AODSIM',
                'QCD_HT1000To1500':'/QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_new_pmx_94X_mc2017_realistic_v11-v1/AODSIM',
                'QCD_HT1500To2000':'/QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM',
                'QCD_HT2000ToInf' :'/QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17DRPremix-PU2017_94X_mc2017_realistic_v11-v2/AODSIM'
        }

        full_MC_samples_2017 = merge_dicts(QCD_2017, TTbar_2017, SingleTop_2017, WJets_2017, DY_2017, Diboson_2017, Triboson_2017)
	#------Data 2017----#
	single_data_2017= {
                'singleMu_RunB':'/SingleMuon/Run2017B-17Nov2017-v1/AOD',
                'singleMu_RunC':'/SingleMuon/Run2017C-17Nov2017-v1/AOD',
                'singleMu_RunD':'/SingleMuon/Run2017D-17Nov2017-v1/AOD',
                'singleMu_RunE':'/SingleMuon/Run2017E-17Nov2017-v1/AOD',
                'singleMu_RunF':'/SingleMuon/Run2017F-17Nov2017-v1/AOD',
                'singleMu_RunG':'/SingleMuon/Run2017G-17Nov2017-v1/AOD',
                'singleMu_RunH':'/SingleMuon/Run2017H-17Nov2017-v2/AOD'
        }


        #------ Extra -----#

        trig_data = {
            #'singleMu_RunA_Jun':'/SingleMuon/Run2018A-06Jun2018-v1/AOD',
            #'singleMu_RunA_Sep':'/SingleMuon/Run2018A-17Sep2018-v2/AOD',
            #'singleMu_RunA_May':'/SingleMuon/Run2018A-22May2018-v1/AOD',
            #'singleMu_RunB_Sep':'/SingleMuon/Run2018B-17Sep2018-v1/AOD',
            #'singleMu_RunC_Sep':'/SingleMuon/Run2018C-17Sep2018-v1/AOD',
            'singleMu_RunD_Jan':'/SingleMuon/Run2018D-22Jan2019-v2/AOD',
            #'singleMu_RunD_Apr':'/SingleMuon/Run2018D-30Apr2019-v1/AOD'
        }
        extra_samples = {
            'ZZTo2L2Nu_ext2':'/ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8/RunIIAutumn18DRPremix-102X_upgrade2018_realistic_v15_ext2-v2/AODSIM'
        }

        if options.sampleType == 'data':
            if options.year == '2017':
                samples = single_data_2017
            else:
                #samples = single_data
                samples = full_data_samples
        elif options.sampleType == 'MC':
            if options.year =='2017':
                samples = full_MC_samples_2017
            else:
                samples = full_MC_samples
        elif options.sampleType == 'custom':
            samples = merge_dicts(QCD) # -------> Pick your custom samples here!
            if (len(samples) == 0):
                print "Error! No samples selected to be processed."
                sys.exit()
        else:
            print "Error! SampleType option {} not recognized.".format(options.sampleType)
            sys.exit()

        for sample, dataset in samples.items():

            isRun2018D = False
            if sample == 'MET_Run2018D':
                isRun2018D = True
            config.JobType.pyCfgParams = ['data={}'.format(isData),'Run2018D={}'.format(isRun2018D)]

            config.Data.inputDataset = dataset
            config.General.requestName = 'iDMAnalysis_' + sample 
            #config.Data.outputDatasetTag = sample

            #basedir = '/uscms/home/mreid/CRAB/signal_region_analysis/CMSSW_10_2_1/src/Firefighter/washAOD/data/iDM/2018/GenFilter/'
            #config.Data.userInputFiles = open(basedir + sample + '.list').readlines()

            # Submit.
            try:
                print "Submitting for input dataset %s with options %s" % (sample, options.crabCmdOpts)
                crabCommand(options.crabCmd, config = config, *options.crabCmdOpts.split())
            except HTTPException as hte:
                print "Submission for input dataset %s failed: %s" % (sample, hte.headers)
            except ClientException as cle:
                print "Submission for input dataset %s failed: %s" % (sample, cle)

    # All other commands can be simply executed.
    elif options.workArea:

        for dir in os.listdir(options.workArea):
            projDir = os.path.join(options.workArea, dir)
            if not os.path.isdir(projDir):
                continue
            # Execute the crab command.
            msg = "Executing (the equivalent of): crab %s --dir %s %s" % (options.crabCmd, projDir, options.crabCmdOpts)
            print "-"*len(msg)
            print msg
            print "-"*len(msg)
            try:
                crabCommand(options.crabCmd, dir = projDir, *options.crabCmdOpts.split())
            except HTTPException as hte:
                print "Failed executing command %s for task %s: %s" % (options.crabCmd, projDir, hte.headers)
            except ClientException as cle:
                print "Failed executing command %s for task %s: %s" % (options.crabCmd, projDir, cle)


if __name__ == '__main__':
    main()
