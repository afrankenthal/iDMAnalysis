#!/bin/sh

[ -d simplevtx ] || mkdir simplevtx
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_0p1cm.txt outputFile=simplevtx/histo_0p1cm.root
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_1cm.txt   outputFile=simplevtx/histo_1cm.root
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_10cm.txt  outputFile=simplevtx/histo_10cm.root
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_50cm.txt  outputFile=simplevtx/histo_50cm.root
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_100cm.txt outputFile=simplevtx/histo_100cm.root
cmsRun ../python/simplevertexer_cfg.py inputFiles_load=../data/20180421/sample_300cm.txt outputFile=simplevtx/histo_300cm.root
