#! /bin/bash

#### THIS SCRIPT NEEDS TO BE SOURCED!
#### (due to use of eosls command)

#### Choose lpcuser and comment out the "input mode" 
#### you don't want to use

#lpcuser=as2872
basedir="2018/signal"

# Use single mass and ctau parameter
#mchi=6p0
#dmchi=2p0
#ctau=10
mchi=$1
dmchi=$2
ctau=$3
dirs="Mchi-${mchi}_dMchi-${dmchi}_ctau-${ctau}"

# OR use directories in EOS to get all samples (uncomment)
#dirs=$(eosls /store/user/$lpcuser/iDM/AOD_Samples/2018/GenFilter/)

#for dir in $dirs; do 
#    echo $dir
#    mchistring="$(cut -d'_' -f1 <<< $dir)"
#    mchi="$(cut -d'-' -f2 <<< $mchistring)"
#    echo $mchi
#done

for dir in $dirs; do
    echo "Processing $dir"
    #eos root://cmseos.fnal.gov ls /store/user/$lpcuser/iDM/AOD_Samples/$basedir/$dir > data/iDM/$basedir/${dir}.list
    eos root://cmseos.fnal.gov ls /store/group/lpcmetx/iDM/AOD/$basedir/$dir > ../data/iDM/$basedir/${dir}.list
    sed -i '/MINIAOD/d' ../data/iDM/$basedir/${dir}.list
    #sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/user/$lpcuser/iDM/AOD_Samples/$basedir/$dir/|" data/iDM/$basedir/${dir}.list
    sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/group/lpcmetx/iDM/AOD/$basedir/$dir/|" ../data/iDM/$basedir/${dir}.list
done

#basename="Mchi-${mchi}_dMchi-${dmchi}_ctau-${ctau}"
#
#eos root://cmseos.fnal.gov ls /store/user/as2872/iDM/AOD_Samples/$basedir/$basename > data/iDM/$basedir/${basename}.list
#
#sed -i '/MINIAOD/d' data/iDM/$basedir/${basename}.list
#
#sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/user/asterenb/iDM/AOD_Samples/$basedir/$basename/|" data/iDM/$basedir/${basename}.list
