#!/bin/bash

# Argument is crab directory to process

basedir="TrigRecoEffStudies"
sample=$(echo $1 | cut -d'_' -f4,5,6)

datestamp=`python ./mergeCRABOutput.py $1 2>&1`
ret=$?

echo $datestamp

#if [ $ret -ne 0 ]; then
    #echo "Latest task is not complete yet! Wait for completion before merging the ROOT files."
#else
    echo "Running hadd on outuput NTuple ROOT files..."
    hadd $basedir/Production/"$sample".root `xrdfsls -u /store/user/as2872/iDM/NTuples/$basedir/$sample/$datestamp/0000/ | grep '\.root'`
#fi
