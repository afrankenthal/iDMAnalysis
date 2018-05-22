#!/bin/sh

[ -d gentrigrecoeffirun ] || mkdir gentrigrecoeffirun
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_0p1cm.txt outputFile=gentrigrecoeffirun/histo_0p1cm.root
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_1cm.txt   outputFile=gentrigrecoeffirun/histo_1cm.root
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_10cm.txt  outputFile=gentrigrecoeffirun/histo_10cm.root
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_50cm.txt  outputFile=gentrigrecoeffirun/histo_50cm.root
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_100cm.txt outputFile=gentrigrecoeffirun/histo_100cm.root
cmsRun ../python/genTrigRecoeffi_cfg.py inputFiles_load=../data/20180421/sample_300cm.txt outputFile=gentrigrecoeffirun/histo_300cm.root
