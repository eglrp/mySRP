#!/bin/bash -l
# Batch script to run a serial array job with the upgraded
# software stack under SGE.
# 1. Force bash
#$ -S /bin/bash
# 2. Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=2:30:0
# 3. Request 1 gigabyte of RAM.
#$ -l mem=256M
# 5. Set up the job array.  In this instance we have requested 10000 tasks
# numbered 1 to 100.
#$ -t 1-5
# 6. Set the name of the job.
#$ -N GPSIIR_srp
# 7. Set the working directory to somewhere in your scratch space.  This is
# a necessary step with the upgraded software stack as compute nodes cannot
# write to $HOME.
# Replace "<your_UCL_id>" with your UCL user ID :)
#$ -wd /home/uceszl2/Scratch/project/fast_SRP/GPSIIR_NONAP/log/
# 8. Run the application.
#cd /home/uceszl2/Scratch/project/fast_SRP/GPSIIR_NONAP/log
userfile=/home/uceszl2/Scratch/project/fast_SRP/GPSIIR_NONAP/GPSIIR_NONAP.txt
lat="."
lon="180"
resolution=0.001
##echo "$JOB_NAME $SGE_TASK_ID"
##let lat=${SGE_TASK_ID}
let depth=${SGE_TASK_ID}
outputpath=/home/uceszl2/Scratch/project/fast_SRP/GPSIIR_NONAP/${depth}/
/home/uceszl2/Scratch/project/fast_SRP/bin/srp ${userfile} ${depth} ${resolution} ${outputpath} ${lon} ${lat}
