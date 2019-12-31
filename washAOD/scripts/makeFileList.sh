#!/bin/bash

if [ "$#" -ne 4 ] && [ "$#" -ne 3 ]; then
  echo "Usage: $0 year mchi dmchi [ctau]" >&2
    exit 1
fi

#### Choose lpcuser and comment out the "input mode" 
#### you don't want to use

year=$1
mchi=$2
dmchi=$3
if [ "$#" -eq 4 ]; then
    ctaus=($4)
else
    ctaus=(1 10 100 1000)
fi

basedirEOS="$year/signal"
basedirLOCAL="signal/$year"

# OR use directories in EOS to get all samples (uncomment)
#$lpcuser=as2872
#dirs=$(eosls /store/user/$lpcuser/iDM/AOD_Samples/2018/GenFilter/)

#for dir in $dirs; do 
#    echo $dir
#    mchistring="$(cut -d'_' -f1 <<< $dir)"
#    mchi="$(cut -d'-' -f2 <<< $mchistring)"
#    echo $mchi
#done

for ctau in "${ctaus[@]}"; do
    dir="Mchi-${mchi}_dMchi-${dmchi}_ctau-${ctau}"
    echo "Processing $dir"
    xrdfs root://cmseos.fnal.gov ls -u /store/group/lpcmetx/iDM/AOD/$basedirEOS/$dir > ../data/$basedirLOCAL/${dir}.list

#    #eos root://cmseos.fnal.gov ls /store/user/$lpcuser/iDM/AOD_Samples/$basedir/$dir > data/iDM/$basedir/${dir}.list
#    eos root://cmseos.fnal.gov ls /store/group/lpcmetx/iDM/AOD/$basedir/$dir > ../data/iDM/$basedir/${dir}.list
#    sed -i '/MINIAOD/d' ../data/iDM/$basedir/${dir}.list
#    #sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/user/$lpcuser/iDM/AOD_Samples/$basedir/$dir/|" data/iDM/$basedir/${dir}.list
#    sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/group/lpcmetx/iDM/AOD/$basedir/$dir/|" ../data/iDM/$basedir/${dir}.list
done

#basename="Mchi-${mchi}_dMchi-${dmchi}_ctau-${ctau}"
#eos root://cmseos.fnal.gov ls /store/user/as2872/iDM/AOD_Samples/$basedir/$basename > data/iDM/$basedir/${basename}.list
#sed -i '/MINIAOD/d' data/iDM/$basedir/${basename}.list
#sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/user/asterenb/iDM/AOD_Samples/$basedir/$basename/|" data/iDM/$basedir/${basename}.list
