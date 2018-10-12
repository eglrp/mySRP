#!/bin/bash
#datapath=/Users/lizhen/papers/fast_ray_tracing/fast_ray_tracing_experiment/figures/Sentinel_1_0.1mm/
datapath=$1
datafile=${datapath}"/LAT-64.00.txt"
outputfile=${datapath}/$2
#sed '1d' $datafile | awk '{print $1}'
#outputfile="LON180.00.txt"
for((i=-90;i<=90;++i))
do

	if [[ $i -lt 0 ]];then
		filename="LAT"${i}".00LON180.00.txt"
		#echo $filename
		if [[ $i -gt -10 ]]; then
			lat=$(printf "%03d" "$i")
			filename="LAT"${lat}".00LON180.00.txt"
			#echo $lat
		fi
	else
		lat=$(printf "%03d" "$i")
		filename="LAT"${lat}".00LON180.00.txt"
		#echo $filename
	fi
	datafile=${datapath}/${filename}
	#echo $datafile
	sed -n '2,2p' $datafile >> $outputfile

done
