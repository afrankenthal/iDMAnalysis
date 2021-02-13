#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Need 4 arguments! macro, cuts, samplefile, and eosdir"
    exit 1
fi

macro=$1
cuts=$2
samplefile=$3
eosdir=$4

ntuple_version=`echo "$samplefile" | cut -d'/' -f3`
year=`echo "$samplefile" | cut -d'/' -f4`
sample=`echo "$samplefile" | cut -d'/' -f5`

echo "Extracting package..."

tar -xf package.tar.gz

echo "Sourcing setup.sh and LCG release..."

. setup.sh

echo "About to call macroRun..."

macrobasename=`basename ${macro} | cut -d'.' -f1`
cutsbasename=`basename ${cuts} | cut -d'.' -f1`
samplebasename=`basename ${sample} | cut -d'.' -f1`

set +x
macroRun -s $samplefile -m $macro -c $cuts -y $year -o ${macrobasename}_${cutsbasename}_${year}_${samplebasename}.root
set -x

echo "Finished macroRun execution!"

echo "Transferring output files to EOS..."

xrdfs root://cmseos.fnal.gov mkdir -p /store/group/lpcmetx/iDM/analysis/${ntuple_version}
xrdfs root://cmseos.fnal.gov mkdir -p /store/group/lpcmetx/iDM/analysis/${ntuple_version}/${eosdir}

xrdcp *.root root://cmseos.fnal.gov//store/group/lpcmetx/iDM/analysis/${ntuple_version}/${eosdir}
rm *.root
xrdcp *.csv root://cmseos.fnal.gov//store/group/lpcmetx/iDM/analysis/${ntuple_version}/${eosdir}
rm *.csv
xrdcp *.txt root://cmseos.fnal.gov//store/group/lpcmetx/iDM/analysis/${ntuple_version}/${eosdir}
rm *.txt

echo "Done! Exiting..."
