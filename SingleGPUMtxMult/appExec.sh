#!/bin/bash

host0=192.168.1.100
host1=192.168.1.101
host2=192.168.1.103
host3=192.168.1.104


#source delMatx.sh

declare -a arr=($host1)

for i in "${arr[@]}"
do

#rsh uriel@$i mkdir /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult 
#rsh uriel@$i mkdir /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug
#rsh uriel@$i mkdir /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/src

#copy the executable
rsync -r /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/SingleGPUMtxMult uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/src/matrixMult.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/src/

#copy the local scheduling file
rsync /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/taskSched.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/

#copy the Data (ignore if data is self generated (is FASTER!!))
#rsync /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile.dat uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/dataFile.dat
done

#exec


declare -a szs=(256 512 1024 2048 4096 8192)
for j in "${szs[@]}"
do
time /home/uriel/mpiELFs/bin/mpirun -np 1  --hostfile /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/SingleGPUMtxMult $j
done
#time /home/uriel/mpiELFs/bin/mpirun -np 1 /home/uriel/Dev/mpiApps/extnsApps/SingleGPUMtxMult/Debug/SingleGPUMtxMult


