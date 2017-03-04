#!/bin/bash

host0=148.247.102.190
host1=148.247.102.191
host2=148.247.102.192
host3=148.247.102.193
#host4=148.247.102.140


declare -a dels=($host0 $host1 $host2 $host3)
#declare -a dataFiles=(dataFile_A.dat dataFile_B.dat dataFile_C.dat dataFile_D.dat dataFile_A1.dat dataFile_B1.dat)

for j in "${dels[@]}"
do
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile_A.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile_B.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile_C.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile_D.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFileA1.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFileB1.dat
done
