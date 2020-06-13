#!/bin/bash

dir=$1

# find full dir from crab.log
base=`grep "config.Data.outLFNDirBase" "$dir/crab.log" | cut -f 2 -d "'"`
dataset=`grep "config.Data.inputDataset" "$dir/crab.log" | cut -f 2 -d "'" | cut -f 2 -d "/"`    crabtask=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 2- -d "_"`
samplename=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 4- -d "_"`
time=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 5 -d ":" | cut -c 2-`

fulldir="root://cmseos.fnal.gov/$base/$dataset/$crabtask/$time/0000/merged/merged.root"

echo $fulldir
