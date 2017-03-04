#!/bin/bash
host0=148.247.102.190
host1=192.168.1.25

declare -a arr=($host0)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/tokenRing/
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/tokenRing/Debug/
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/tokenRing/src

#copy the executable
rsync /home/uriel/Dev/mpiApps/extnsApps/tokenRing/Debug/tokenRing uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/tokenRing/Debug/

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/tokenRing/src/proc.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/tokenRing/src/

#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/tokenRing/scheduleFile.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/tokenRing/

#copy AdjMtxfile 
rsync /home/uriel/Dev/mpiApps/extnsApps/tokenRing/adjMtx.csv uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/tokenRing/


done

