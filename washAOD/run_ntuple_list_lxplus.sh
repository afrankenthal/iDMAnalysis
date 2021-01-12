#! /bin/bash

if [ "$#" -ne 2 ]; then                                                                                                         
  echo "Usage: $0  mchi dmchi " >&2
    exit 1
fi

Mchi=$1
dMchi=$2
ctaus=(1 10 100 1000) 
#ctaus=(100 1000) 
for ctau in "${ctaus[@]}"; do

    files=data/signal/2018/Mchi-$Mchi\_dMchi-$dMchi\_ctau-$ctau.list
    echo "reading list: " $files
    cmsRun python/iDMAnalyzer_cfg.py inputFiles_load=$files
    mv output.root output_Mchi-$Mchi\_dMchi-$dMchi\_ctau-$ctau\_year-2018.root
done
