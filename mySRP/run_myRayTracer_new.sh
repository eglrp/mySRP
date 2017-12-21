#!/bin/sh
##compile first

bin_srp=bin/srp
bin_grid=bin/generateGrid
resolution=0.01
depth=1
currentDir=$PWD
echo ${currentDir}


outputpath=${currentDir}/GalileoIOV/
userfile=${outputpath}/GalileoIOV.txt

## just run it
## for GNSS attitude control, the longitude can only be -5 -4 -3 -2 -1 0 1 2 3 4 5 and 175 176 177 178 179 180 181 182 183 184 185
#longitude=( 178 179 180 -179 -178 )
longitude=( 180 179 )
len_lon=${#longitude[@]}

latitude=(45 30)
len_lat=${#latitude[@]}
#echo $len_lon
#echo $len_lat
for((i=0;i<${len_lat};i++))
do
	for((j=0;j < ${len_lon};j++))
 	do 
 		lon=${longitude[j]}
 		lat=${latitude[i]}
 		#echo $lon
 		#let lon= ${longitude[2]}
 	  $bin_srp $userfile $depth $resolution $lat $lon  $outputpath
	done
done


##generate the grids
#$bin_grid $outputpath