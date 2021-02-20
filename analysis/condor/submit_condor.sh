#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Need 4 arguments! macro config, cuts config, sample config (in format '../configs/samples/\$ntuple_version/\$year/\$sample.json') and eosdir "
    exit 1
fi

cp condor_job_template.jdl condor_job.jdl

sed -i "s|XXmacroXX|$1|g" condor_job.jdl
sed -i "s|XXcutsXX|$2|g" condor_job.jdl
sed -i "s|XXsamplefileXX|$3|g" condor_job.jdl
sed -i "s|XXeosdirXX|$4|g" condor_job.jdl

condor_submit condor_job.jdl