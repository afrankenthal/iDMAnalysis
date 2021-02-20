#!/bin/bash


dirs=( `cat ../macros/configs/samples/test2016/single_full.json | grep "root"| sed 's/,//g' | sed 's/root:\/\/cmseos.fnal.gov\///g'| sed 's/"//g'` )
#dirs=( `cat ../macros/configs/samples/yearstack/backgrounds_full.json | grep "root"| sed 's/,//g' | sed 's/root:\/\/cmseos.fnal.gov\///g'| sed 's/"//g'` )
for dir in ${dirs[@]};do
	for i in {2..1000..20}; do 
		#hadd out_merge_${i}.root `xrdfs root://cmseos.fnal.gov ls -u ${dir} | sed -n $i,$((i+20))p`;
#		xrdcp out_merge_${i}.root root://cmseos.fnal.gov/${dir}/cat/out_merge_${i}.root; 
#		rm out_merge_${i}.root;
		for file in  `xrdfs root://cmseos.fnal.gov ls -u ${dir} | sed -n $i,$((i+20))p` ; do
		echo ${file}
		file2=$(echo ${file} | sed 's/output/used\/output/g')
		echo ${file2}
		eos root://cmseos.fnal.gov mv ${file} ${file2}
		done
		#eos root://cmseos.fnal.gov mv `xrdfs root://cmseos.fnal.gov ls -u ${dir} | sed -n $i,$((i+20))p` ${dir}/used/
	done
done
