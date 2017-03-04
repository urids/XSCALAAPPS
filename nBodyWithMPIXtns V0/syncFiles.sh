#!/bin/bash

#host0=148.247.102.190
#host1=148.247.102.191
#host2=148.247.102.192
#host3=148.247.102.193
host4=148.247.102.140

declare -a dels=($host4)
for j in "${dels[@]}"
do
#rsh uriel@$j mkdir /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns 
#rsh uriel@$j mkdir /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug
#rsh uriel@$j mkdir /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/src
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/dataFile.dat
done

declare -a arr=($host4)
for i in "${arr[@]}"
do
#copy the executable
rsync -r /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/nBodyWithMPIXtns uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/src/NBodyExt.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/src/

#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/taskSched.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/taskSched.cfg
#copy the Data
#rsync /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/dataFile.dat uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/dataFile.dat
done
