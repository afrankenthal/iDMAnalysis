#!/bin/sh

[ -d mupvtx ] || mkdir mupvtx
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_0p1cm.txt outputFile=mupvtx/histo_0p1cm.root
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_1cm.txt   outputFile=mupvtx/histo_1cm.root
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_10cm.txt  outputFile=mupvtx/histo_10cm.root
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_50cm.txt  outputFile=mupvtx/histo_50cm.root
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_100cm.txt outputFile=mupvtx/histo_100cm.root
cmsRun ../python/MuPairVertexer_cfg.py inputFiles_load=../data/20180421/sample_300cm.txt outputFile=mupvtx/histo_300cm.root
