#!/bin/bash

if [ $# -eq 0 ]; then
    echo "No arguments supplied"
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

    # create sample json file
    json="\"$samplename\": { \"dir\": [ \"root://cmseos.fnal.gov/$fulldir\" ], \"group\": \"\", \"mode\": \"\", \"limit_num_files\": -1, \"sum_gen_wgt\": 0.0, \"xsec\": -1},"

    #echo $json >> sample_test.json
    totaljson="$totaljson $json"
}

crabdir=$1
for dirarg in $crabdir/*; do
    echo "Processing sample $dirarg"
    process_one_sample $dirarg
done

# remove last comma and add closing braces
totaljson=${totaljson%?};
totaljson="$totaljson }"

# use python tool to prettify json
echo $totaljson | python -m json.tool > sample.json
