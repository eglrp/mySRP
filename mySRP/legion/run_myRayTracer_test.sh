#!/bin/sh
##compile first
g++ -std=c++14 -O3  src/*.cpp -o bin/mysrp
lat=30.0
resolution=0.001
userfile=GalileoFOC.txt
currentDir=$PWD
echo ${currentDir}
outputpath=${currentDir}/galileoFOC/

## just run it
for((i=0;i <= 180;i++))
 do 
 		let lat=$i-90
	echo $lat	
# bin/mysrp $userfile $lat $resolution $outputpath
done
