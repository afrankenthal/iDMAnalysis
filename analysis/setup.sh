#!/bin/bash

ABSOLUTE_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
BASE_DIR=`dirname $ABSOLUTE_PATH`

source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc8-opt/setup.sh
export PATH=$BASE_DIR/bin:$BASE_DIR/scripts:$PATH
export LD_LIBRARY_PATH=$BASE_DIR/lib:$LD_LIBRARY_PATH
export ANALYSIS_DATA_PATH=$BASE_DIR/data
