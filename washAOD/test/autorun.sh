#!/bin/bash

cmsRun python/ConfFile_cfg.py inputFiles_load=data/sample_0p83cm.list outputFile=dst/histo_0p83cm.root
cmsRun python/ConfFile_cfg.py inputFiles_load=data/sample_8p3cm.list  outputFile=dst/histo_8p3cm.root
cmsRun python/ConfFile_cfg.py inputFiles_load=data/sample_83cm.list   outputFile=dst/histo_83cm.root

python macros/plotAcceptance.py dst/histo_0p83cm.root
python macros/plotAcceptance.py dst/histo_8p3cm.root
python macros/plotAcceptance.py dst/histo_83cm.root

date=`date +%Y%m%d`
#mkdir /publicweb/w/wsi/public/lpcdm/$date
#echo "Options +Indexes" > /publicweb/w/wsi/public/lpcdm/$date/.htaccess
mv *.pdf /publicweb/w/wsi/public/lpcdm/$date
#mv *.pdf /publicweb/w/wsi/public/lpcdm/

echo "http://home.fnal.gov/~wsi/public/lpcdm/$date" | mail -s "Check this" wsi@CERN.CH
