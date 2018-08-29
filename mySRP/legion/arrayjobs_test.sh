#!/bin/bash -l
# Batch script to run a serial array job with the upgraded
# software stack under SGE.
# 1. Force bash
#$ -S /bin/bash
# 2. Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=5:30:0
# 3. Request 1 gigabyte of RAM.
#$ -l mem=128M
# 5. Set up the job array.  In this instance we have requested 10000 tasks
# numbered 1 to 100.
#$ -t 1-181
# 6. Set the name of the job.
#$ -N mm_10_srp
# 7. Set the working directory to somewhere in your scratch space.  This is
# a necessary step with the upgraded software stack as compute nodes cannot
# write to $HOME.
# Replace "<your_UCL_id>" with your UCL user ID :)
#$ -wd /home/uceszl2/Scratch/project/myRayTracer/log/
# 8. Run the application.
cd /home/uceszl2/Scratch/project/myRayTracer
userfile=/home/uceszl2/Scratch/project/myRayTracer/GPSIIRNAP.txt
lat=30.0
lon=180.0
resolution=0.0001
outputpath=/home/uceszl2/Scratch/project/myRayTracer/GPSIIR_0.1mm/
#echo "$JOB_NAME $SGE_TASK_ID"
let lat=${SGE_TASK_ID}-91
/home/uceszl2/Scratch/project/myRayTracer/bin/mysrp_test $userfile $lon $lat $resolution $outputpath
