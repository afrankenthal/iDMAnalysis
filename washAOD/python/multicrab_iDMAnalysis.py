#!/usr/bin/env python
"""
This is a small script that does the equivalent of multicrab.
"""
import os
import sys
from optparse import OptionParser
import json
import copy
from multiprocessing import Process

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
                      help = "Which kind of sample to process ('MC' (default), 'data', 'trig' or 'custom')",
                      metavar = 'STYP')

    parser.add_option('-y', '--year',
                      dest = 'year',
                      default = '2018',
                      help = "Which year to process ('2018'(default)/'2017'/'2016')",
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

    year = options.year

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

        if isData == True:
            #config.Data.splitting = 'LumiBased'
            #config.Data.unitsPerJob = 100
            if year == '2018':
                config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt'
            elif year == '2017':
                config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/Final/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt'
            elif year == '2016':
                config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/Final/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt'

        config.Data.splitting = 'Automatic'

        if isData == True:
            config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/%s/data_eighthrun' % year
        else:
            config.Data.outLFNDirBase = '/store/group/lpcmetx/iDM/Ntuples/%s/backgrounds_eighthrun' % year

        config.Data.publication = False
        config.Data.ignoreLocality = True

        #config.Site.ignoreGlobalBlacklist = True
        #config.Site.whitelist = ['T2_RU_ITEP']
        config.Site.whitelist = ['T2_US_*', 'T2_DE_*', 'T2_EE_*', 'T2_ES_*', 'T2_GR_*', 'T2_HU_*', 'T2_IT_*', 'T2_RU_*', 'T2_UK_*']
        config.Site.blacklist = ['T2_TW_NCHC','T2_BE_IIHE', 'T2_FR_GRIF_LLR']
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

        total = {}

        with open('../data/dataset_db_%s.json' % year, 'r') as db:
            data = json.load(db)

            QCD = data['QCD_' + year]

            TTbar = data['TTbar_' + year]
            for it in ['TTTo2L2Nu', 'TTSemiLeptonic', 'TTToHadronic', 'TT_diLept']:
                if it in TTbar:
                    del TTbar[it]

            SingleTop = data['SingleTop_' + year]
            for it in ['ST_t-channel_top_5f', 'ST_t-channel_antitop_5f']:
                if it in SingleTop:
                    del SingleTop[it]

            WJets = data['WJets_' + year]
            for it in ['WJetsToLnu', 'WJetsToLNu_HT-70To100']:
                if it in WJets:
                    del WJets[it]

            ZJets = data['ZJets_' + year]

            DY = data['DY_' + year] 
            for it in ['DYJetsToTauTau', 'DYJetsToLL_M-5to50']:
                if it in DY:
                    del DY[it]

            Diboson = data['Diboson_' + year]
            for it in ['WZTo3LNu', 'ZZTo2L2Nu', 'ZZTo2L2Nu_ext2', 'WWTo2L2Nu', 'WWJJToLNuLNu']:
                if it in Diboson:
                    del Diboson[it]

            Triboson = data['Triboson_' + year]

            total_MC = merge_dicts(QCD, TTbar, SingleTop, ZJets, WJets, DY, Diboson, Triboson)

            for key, val in total_MC.items():
                total_MC[key + '_' + year] = val
                del total_MC[key]

            total_Data = data['Data_MET_' + year]

            total_Trig = data['Data_SingleMu_' + year]

            if options.sampleType == 'data':
                total = merge_dicts(total, total_Data)
            elif options.sampleType == 'trig':
                total = merge_dicts(total, total_Trig)
            elif options.sampleType == 'MC':
                total = merge_dicts(total, total_MC)
            elif options.sampleType == 'all':
                total = merge_dicts(total, total_MC, total_Data, total_Trig)
            elif options.sampleType == 'custom':
                total = merge_dicts() # -------------------------------> put here the custom samples you want!!!
            else:
                print "ERROR! SampleType option %s not recoginzed." % options.sampleType
                sys.exit()

        if len(total) == 0:
            print "ERROR! No samples selected to be processed."
            sys.exit()

        for sample, dataset in total.items():

            isRun2018D = False
            if sample == 'MET_Run2018D':
                isRun2018D = True

            config.JobType.pyCfgParams = ['data={}'.format(isData),'Run2018D={}'.format(isRun2018D), 'numThreads={}'.format(1)]
            config.JobType.numCores = 1

            config.Data.inputDataset = dataset
            config.General.requestName = 'iDMAnalysis_' + sample 
            #config.Data.outputDatasetTag = sample

            # If we need to pull input files from a list file instead of CRAB:
            #config.Data.userInputFiles = open(basedir + sample + '.list').readlines()

            # Submit.
            try:
                print "Submitting for input dataset %s with options %s" % (sample, options.crabCmdOpts)
                crabCommand(options.crabCmd, config = config, *options.crabCmdOpts.split())
                #p = Process(target=crabCommand, args=(options.crabCmd, config, options.crabCmdOpts.split(),))
                #p.start()
                #p.join()
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
