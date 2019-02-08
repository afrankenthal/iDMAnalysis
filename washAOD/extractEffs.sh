#! /bin/bash

files=$(ls *.out)

if [ -a efficiencies.dat ]; then
    if [ "$1" = "1" ]; then
        rm efficiencies.dat
    else
        echo "Efficiencies.dat file already exists!"
        echo "Run with ./extractEffs.sh 1 to overwrite or delete the file prior"
        exit 1
    fi
fi

for file in $files; do
    echo $file | tee -a efficiencies.dat
    tail -n 2 $file | tee -a efficiencies.dat
done
