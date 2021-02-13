#!/bin/bash

# Expected argument: name of crab work area and year

if [ $# -ne 2 ]; then
    echo "Need 2 arguments: crab work area and year"
    exit
fi

function process_one_sample {
    # if file just return
    mydir="$1"
    if [[ -f $mydir ]]; then
        return
    fi

    # find full dir from crab.log 
    base=`grep "config.Data.outLFNDirBase" "$mydir/crab.log" | cut -f 2 -d "'"`
    dataset=`grep "config.Data.inputDataset" "$mydir/crab.log" | cut -f 2 -d "'" | cut -f 2 -d "/"`
    crabtask=`grep "Task name:" "$mydir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 2- -d "_"`
    samplename=`grep "Task name:" "$mydir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 4- -d "_"`
    time=`grep "Task name:" "$mydir/crab.log" -m1 | cut -f 5 -d ":" | cut -c 2-`

    fulldir="$base/$dataset/$crabtask/$time/0000"
    numdir="$base/$dataset/$crabtask/$time"
    checkstr=`eos root://cmseos.fnal.gov ls $numdir`  
    checknum=($check1)

    # find year
    if [[ ${base} == *"2017"* ]]
    then
       year="2017"
    elif [[ ${base} == *"2016"* ]]
    then
       year="2016"
    else
       year="2018"
    fi
    echo "year = $year"

    if [ "$year" != "$requestedyear" ]; then
        return
    fi
	
    echo "Checking sample and number of dirs "
    echo ${#checknum[@]}
    dirfiles="\"root://cmseos.fnal.gov/$fulldir\" "
    #echo $dirfiles
    #test="${test}, \"root://cmseos.fnal.gov/$fulldir\" "
    #echo $test
    
    # Check if directory already exists
    eos root://cmseos.fnal.gov ls "$fulldir/merged"
    status=$?
    if [ $status -eq 0 ]; then
        echo "Merged directory already exists! Not overwriting."
        return
    fi

    eos root://cmseos.fnal.gov/ mkdir "$fulldir/merged"
    #echo "root://cmseos.fnal.gov/$fulldir/merged/merged.root `xrdfs root://cmseos.fnal.gov ls -u $fulldir | grep '\.root'`"
    #hadd -k -f -j 4 "root://cmseos.fnal.gov/$fulldir/merged/merged.root" `xrdfs root://cmsxrootd.fnal.gov ls -u $fulldir | grep '\.root'`
    hadd -d /uscmst1b_scratch/lpc1/3DayLifetime/as2872/partial_hadd -j 8 -f -k /uscmst1b_scratch/lpc1/3DayLifetime/as2872/merged_${year}.root `xrdfs root://cmseos.fnal.gov ls -u $fulldir | grep '\.root'`
    xrdcp -f /uscmst1b_scratch/lpc1/3DayLifetime/as2872/merged_${year}.root root://cmseos.fnal.gov/$fulldir/merged/merged.root

    for i in $(seq 1 ${#checknum[@]})
    do
	j=$((i-1))
	if [ ${j} -ne 0 ]
	then
    	dirfiles="${dirfiles}, \"root://cmseos.fnal.gov/$numdir/000${j}\" " 
#	echo ${dirfiles}
	fi
    done
}

crabdir=$1
requestedyear=$2
echo "Requested year is $requestedyear" 

for dirarg in $crabdir/*; do
    #echo $dirarg
    #if [ "$dirarg" != *"ZJets"* ]; then
    #    continue
    #fi
    echo "Processing sample $dirarg"
    process_one_sample $dirarg
done
