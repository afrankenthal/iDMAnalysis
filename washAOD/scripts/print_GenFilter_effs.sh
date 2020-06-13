#! /bin/bash

### Need to adjust samples directory and directory of runGenFilterEfficiencyAnalyzer_cfg.py

samples=$(ls ../../data/iDM/2018/GenFilter/Mchi-{5,6,7,8}*ctau-1.list)

declare -a pidarray

count=0
for sample in $samples; do
    samplename=$( echo $sample | rev | cut -d'/' -f1 | rev )
    echo "Running on $samplename"
    ( cmsRun ../../../../GeneratorInterface/Core/test/runGenFilterEfficiencyAnalyzer_cfg.py inputFiles_load=$sample | tee "$(cut -d'.' -f1 <<< $samplename).out" ) &
    pidarray[$count]=$!
    count=$(( $count + 1))
done

for pid in ${pidarray[@]}; do
    wait $pid && echo "pid $pid exited normally" || echo "pid $pid exited abnormally with status $?"
done
