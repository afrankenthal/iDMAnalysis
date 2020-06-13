#!/bin/bash

# Expected argument: name of crab work area and year

if [ $# -eq 0 ] || [ $# -eq 1 ]; then
    echo "Only zero or 1 argument supplied, need crab work area and year"
    exit
fi

totaljson="{"

function process_one_sample {
    # if file just return
    dir="$1"
    if [[ -f $dir ]]; then
        return
    fi

    # find full dir from crab.log 
    base=`grep "config.Data.outLFNDirBase" "$dir/crab.log" | cut -f 2 -d "'"`
    dataset=`grep "config.Data.inputDataset" "$dir/crab.log" | cut -f 2 -d "'" | cut -f 2 -d "/"`
    crabtask=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 2- -d "_"`
    samplename=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 6 -d ":" | cut -f 4- -d "_"`
    time=`grep "Task name:" "$dir/crab.log" -m1 | cut -f 5 -d ":" | cut -c 2-`

    fulldir="$base/$dataset/$crabtask/$time/0000"
    numdir="$base/$dataset/$crabtask/$time"
    check1=`eos root://cmseos.fnal.gov ls $numdir`  
    check=($check1)

    # find year
    if [[ ${base} == *"2017"* ]]
    then
       echo "year = 2017"
	year="2017"
    elif [[ ${base} == *"2016"* ]]
    then
       echo "year = 2016"
	year="2016"
    else
       echo "year = 2018"
	year="2018"
    fi

    if [ "$year" != "$requestedyear" ]; then
        return
    fi
	
    echo "check sample and number of dirs"
    echo ${#check[@]}
    dirfiles="\"root://cmseos.fnal.gov/$fulldir\" "
    #echo $dirfiles
    #test="${test}, \"root://cmseos.fnal.gov/$fulldir\" "
    #echo $test


    for i in $(seq 1 ${#check[@]})
    do
	j=$((i-1))
	if [ ${j} -ne 0 ]
	then
    	dirfiles="${dirfiles}, \"root://cmseos.fnal.gov/$numdir/000${j}\" " 
#	echo ${dirfiles}
	fi
    done

#    if [ ${#check[@]} -eq 1 ]
#    then
#        json="\"$samplename\": { \"dir\": [ \"root://cmseos.fnal.gov/$fulldir\" ], \"group\": \"\", \"mode\": \"\", \"limit_num_files\": -1, \"sum_gen_wgt\": 0.0, \"year\":$year, \"xsec\": -1},"
#    elif [ ${#check[@]} -eq 2 ]
#    then
#    echo "0001 exists"
#    fulldir1="$base/$dataset/$crabtask/$time/0001"
#    	json="\"$samplename\": { \"dir\": [ \"root://cmseos.fnal.gov/$fulldir\", \"root://cmseos.fnal.gov/$fulldir1\" ], \"group\": \"\", \"mode\": \"\", \"limit_num_files\": -1, \"sum_gen_wgt\": 0.0, \"year\":$year, \"xsec\": -1},"
#    else
#    echo "0002 exists"
#    fulldir1="$base/$dataset/$crabtask/$time/0001"
#    fulldir2="$base/$dataset/$crabtask/$time/0002"
#    	json="\"$samplename\": { \"dir\": [ \"root://cmseos.fnal.gov/$fulldir\", \"root://cmseos.fnal.gov/$fulldir1\", \"root://cmseos.fnal.gov/$fulldir2\" ], \"group\": \"\", \"mode\": \"\", \"limit_num_files\": -1, \"sum_gen_wgt\": 0.0, \"year\":$year, \"xsec\": -1},"
#
#    fi
#    #echo $json >> sample_test.json
        json="\"$samplename\": { \"dir\": [ ${dirfiles} ], \"group\": \"\", \"mode\": \"\", \"limit_num_files\": -1, \"sum_gen_wgt\": 0.0, \"year\":$year, \"xsec\": -1},"
    totaljson="$totaljson $json"
}

crabdir=$1
requestedyear=$2
echo "Requested year is $requestedyear" 

for dirarg in $crabdir/*; do
    echo "Processing sample $dirarg"
    process_one_sample $dirarg #$year
done

# remove last comma and add closing braces
totaljson=${totaljson%?};
totaljson="$totaljson }"

# use python tool to prettify json
echo $totaljson | python -m json.tool > sample.json
