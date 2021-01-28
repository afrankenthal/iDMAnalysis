#!/bin/bash

#dir=/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMAnalysis/skimmer
dir=/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/skimmer_push/iDMAnalysis/skimmer
#for d in $(find ${dir}/macros/plots/ -type d)
for d in $(find ${dir}/scripts/plots_dsaeff/ -type d)
do
  #Do something, the directory is accessible with $d:
  echo $d
  python $dir/scripts/make_html_listing.py $d
done
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/moresamples/
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/alltrigs/
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/tanh_fit/
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/erf_fit/
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/trigstudy/
#cp -r $dir/macros/plots/* /publicweb/m/mreid/iDM_AN_Plots/dsaID_n2_plots/
newdir=/publicweb/m/mreid/iDM_AN_Plots/dsaID_py_plots/
rm -r $newdir
mkdir $newdir
cp -r $dir/scripts/plots_dsaeff/* $newdir 

