#! /bin/bash

files=$(ls *_ctau-1.out)

if [ -a efficiencies.dat ]; then
    if [ "$1" = "1" ]; then
        rm -f efficiencies.dat
        rm -f summary.dat
        rm -f samples.dat
    else
        echo "Efficiencies.dat file already exists!"
        echo "Run with ./extract_effs.sh 1 to overwrite, or delete existing file prior to running"
        exit 1
    fi
fi

for file in $files; do
    samplename=$( echo $file | rev | cut -d'/' -f1 | rev )
    echo $file | tee -a summary.dat
    tail -n 2 $file | tee -a summary.dat
    effstring="$(tail -n 1 $file)"
    set $effstring
    echo $4 | tee -a efficiencies.dat
    echo $samplename $4 >> samples.dat
done
