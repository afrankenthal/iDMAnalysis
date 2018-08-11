#!/bin/bash

TAG=$1
DATALIST=${TAG}.list
OUTPUT=${TAG}.root
YEAR=2018

cmsRun ../python/tuplizer_cfg.py year=${YEAR} inputFiles_load=../data/${YEAR}/${DATALIST} outputFile=${YEAR}/${OUTPUT} 2>&1
