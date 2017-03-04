#!/bin/bash
host0=148.247.102.190

declare -a arr=($host0)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/Debug
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/src

#copy the executable
rsync /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/Debug/DynamicTokenRing uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/Debug

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/src/proc.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/src/

#copy scheduling configuration file
#rsync /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/scheduleFile.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing

#copy AdjMtxfile 
#rsync /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/adjMtx.csv uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing


done

