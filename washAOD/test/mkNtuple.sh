#!/bin/bash

TAG=$1
DATALIST=${TAG}.list
OUTPUT=${TAG}.root

cmsRun ../python/ntuplizer_cfg.py inputFiles_load=../data/${DATALIST} outputFile=${OUTPUT}.root 2>&1
