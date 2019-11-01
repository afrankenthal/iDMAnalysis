#!/bin/bash

dir=/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD
cp -r $dir/macros/bin/plots/* /publicweb/m/mreid/iDM_AN_Plots/macrorun/

for d in $(find /publicweb/m/mreid/iDM_AN_Plots/macrorun/ -type d)
do
  #Do something, the directory is accessible with $d:
  echo $d
  python $dir/scripts/make_html_listing.py $d
done
