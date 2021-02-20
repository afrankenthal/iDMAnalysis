#!/usr/bin/env python
"""
This is a small script that does the equivalent of multicrab.
"""
import os
from optparse import OptionParser

from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException


def getOptions():
    """
    Parse and return the arguments provided by the user.
    """
    usage = ("Usage: %prog --crabCmd CMD [--workArea WAD --crabCmdOpts OPTS]"
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

    # The submit command needs special treatment.
    if options.crabCmd == 'submit':

        #--------------------------------------------------------
        # This is the base config:
        #--------------------------------------------------------
        from CRABClient.UserUtilities import config, getUsernameFromSiteDB
        config = config()

        basedir = '/uscms/home/mreid/CRAB/signal_region_analysis/CMSSW_10_2_1/src/Firefighter/skimmer/data/iDM/2018/GenFilter/'

        config.General.workArea = 'crab'
        config.General.transferOutputs = True
        config.General.transferLogs = True

        config.JobType.pluginName = 'Analysis'
        config.JobType.psetName = 'TrigRecoEffi_cfg.py'
        #config.JobType.maxMemoryMB = 4000
        #config.JobType.numCores = 1

        config.Data.outputPrimaryDataset = 'TrigRecoEffStudies'
        config.Data.splitting = 'FileBased'
        config.Data.unitsPerJob = 1
        config.Data.outLFNDirBase = '/store/user/asterenb/iDM/NTuples'
        config.Data.publication = False

        config.Site.ignoreGlobalBlacklist = True
        #config.Site.blac = ['T3_RU_FIAN', 'T3_US_MIT', 'T3_US_UCD',
        #'T3_CO_Uniandes', 'T3_US_NotreDame', 
        #'T3_IN_PUHEP', 'T3_UK_ScotGrid_ECDF', 'T3_BY_NCPHEP',
        #'T2_CH_CERN_HLT', 'T3_CH_CERN_HelixNebula', 'T3_IN_TIFRCloud',
        #'T0_CH_CERN', 'T3_GR_IASA', 'T3_CN_PKU', 'T0_CH_CSCS_HPC',
        #'T3_IR_IPM', 'T2_RU_ITEP', 'T3_US_JHU', 'T3_US_Kansas',
        #'T3_US_FSU', 'T3_KR_UOS', 'T3_CH_PSI']
        config.Site.whitelist = ["T3_US_FNALLPC"]
        config.Site.storageSite = 'T3_US_FNALLPC'

        #--------------------------------------------------------

        # Will submit one task for each of these input datasets.
        samples = [
                 # 'Mchi-105p0_dMchi-10p0_ctau-1',
                 # 'Mchi-105p0_dMchi-10p0_ctau-10',
                 # 'Mchi-105p0_dMchi-10p0_ctau-100',
                 # 'Mchi-105p0_dMchi-10p0_ctau-1000',
                  #'Mchi-10p5_dMchi-1p0_ctau-1',
                 # 'Mchi-10p5_dMchi-1p0_ctau-10',
                 # 'Mchi-10p5_dMchi-1p0_ctau-100',
                 # 'Mchi-10p5_dMchi-1p0_ctau-1000',
                 # 'Mchi-120p0_dMchi-40p0_ctau-1',
                 # 'Mchi-120p0_dMchi-40p0_ctau-10',
                 # 'Mchi-120p0_dMchi-40p0_ctau-100',
                 # 'Mchi-120p0_dMchi-40p0_ctau-1000',
                 # 'Mchi-12p0_dMchi-4p0_ctau-1',
                 # 'Mchi-12p0_dMchi-4p0_ctau-10',
                 # 'Mchi-12p0_dMchi-4p0_ctau-100',
                 # 'Mchi-12p0_dMchi-4p0_ctau-1000',
                 'Mchi-1p2_dMchi-0p4_ctau-1',
                 'Mchi-1p2_dMchi-0p4_ctau-10',
                 'Mchi-1p2_dMchi-0p4_ctau-100',
                 'Mchi-1p2_dMchi-0p4_ctau-1000',
                 'Mchi-21p0_dMchi-2p0_ctau-1',
                 'Mchi-21p0_dMchi-2p0_ctau-10',
                 'Mchi-21p0_dMchi-2p0_ctau-100',
                 'Mchi-21p0_dMchi-2p0_ctau-1000',
                 'Mchi-24p0_dMchi-8p0_ctau-1',
                 'Mchi-24p0_dMchi-8p0_ctau-10',
                 'Mchi-24p0_dMchi-8p0_ctau-100',
                 'Mchi-24p0_dMchi-8p0_ctau-1000',
                 'Mchi-31p5_dMchi-3p0_ctau-1',
                 'Mchi-31p5_dMchi-3p0_ctau-10',
                 'Mchi-31p5_dMchi-3p0_ctau-100',
                 'Mchi-31p5_dMchi-3p0_ctau-1000',
                 'Mchi-36p0_dMchi-12p0_ctau-1',
                 'Mchi-36p0_dMchi-12p0_ctau-10',
                 'Mchi-36p0_dMchi-12p0_ctau-100',
                 'Mchi-36p0_dMchi-12p0_ctau-1000',
                 'Mchi-42p0_dMchi-4p0_ctau-1',
                 'Mchi-42p0_dMchi-4p0_ctau-10',
                 'Mchi-42p0_dMchi-4p0_ctau-100',
                 'Mchi-42p0_dMchi-4p0_ctau-1000',
                 'Mchi-48p0_dMchi-16p0_ctau-1',
                 'Mchi-48p0_dMchi-16p0_ctau-10',
                 'Mchi-48p0_dMchi-16p0_ctau-100',
                 'Mchi-48p0_dMchi-16p0_ctau-1000',
                 'Mchi-52p5_dMchi-5p0_ctau-1',
                 'Mchi-52p5_dMchi-5p0_ctau-10',
                 'Mchi-52p5_dMchi-5p0_ctau-100',
                 'Mchi-52p5_dMchi-5p0_ctau-1000',
                 'Mchi-63p0_dMchi-6p0_ctau-1',
                 'Mchi-63p0_dMchi-6p0_ctau-10',
                 'Mchi-63p0_dMchi-6p0_ctau-100',
                 'Mchi-63p0_dMchi-6p0_ctau-1000',
                 'Mchi-72p0_dMchi-24p0_ctau-1',
                 'Mchi-72p0_dMchi-24p0_ctau-10',
                 'Mchi-72p0_dMchi-24p0_ctau-100',
                 'Mchi-72p0_dMchi-24p0_ctau-1000',
                 'Mchi-84p0_dMchi-8p0_ctau-1',
                 'Mchi-84p0_dMchi-8p0_ctau-10',
                 'Mchi-84p0_dMchi-8p0_ctau-100',
                 'Mchi-84p0_dMchi-8p0_ctau-1000',
                 'Mchi-96p0_dMchi-32p0_ctau-1',
                 'Mchi-96p0_dMchi-32p0_ctau-10',
                 'Mchi-96p0_dMchi-32p0_ctau-100',
                 'Mchi-96p0_dMchi-32p0_ctau-1000',
                  ]

        for sample in samples:

            config.General.requestName = 'skimmer_TrigRecoEff_' + sample
            config.Data.outputDatasetTag = sample
            config.Data.userInputFiles = open(basedir + sample + '.list').readlines()

            # Submit.
            try:
                print "Submitting for input dataset %s" % (sample)
                crabCommand(options.crabCmd, config = config)#, *options.crabCmdOpts.split())
            except HTTPException as hte:
                print "Submission for input dataset %s failed: %s" % (sample, hte.headers)
            except ClientException as cle:
                print "Submission for input dataset %s failed: %s" % (sample, cle)

    # All other commands can be simply executed.
    elif options.workArea:

        for dir in osdir(options.workArea):
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
