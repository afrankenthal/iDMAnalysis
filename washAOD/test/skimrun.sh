#!/bin/sh

[ -d skimrun ] || mkdir skimrun
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_0p1cm.txt outputFile=skimrun/histo_0p1cm.root
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_1cm.txt   outputFile=skimrun/histo_1cm.root
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_10cm.txt  outputFile=skimrun/histo_10cm.root
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_50cm.txt  outputFile=skimrun/histo_50cm.root
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_100cm.txt outputFile=skimrun/histo_100cm.root
cmsRun ../python/skim_cfg.py inputFiles_load=../data/20180421/sample_300cm.txt outputFile=skimrun/histo_300cm.root
