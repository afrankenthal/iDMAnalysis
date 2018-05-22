#!/bin/bash

[ -d trigefficiencyrun ] || mkdir trigefficiencyrun
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_0p1cm.txt outputFile=trigefficiencyrun/histo_0p1cm.root
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_1cm.txt   outputFile=trigefficiencyrun/histo_1cm.root
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_10cm.txt  outputFile=trigefficiencyrun/histo_10cm.root
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_50cm.txt  outputFile=trigefficiencyrun/histo_50cm.root
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_100cm.txt outputFile=trigefficiencyrun/histo_100cm.root
cmsRun ../python/ConfFile_cfg.py inputFiles_load=../data/20180421/sample_300cm.txt outputFile=trigefficiencyrun/histo_300cm.root