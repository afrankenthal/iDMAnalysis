#!/bin/bash

TAG=$1
DATALIST=${TAG}.list
OUTPUT=${TAG}.root

cmsRun ../python/tuplizer_cfg.py inputFiles_load=../data/${DATALIST} outputFile=${OUTPUT} 2>&1
