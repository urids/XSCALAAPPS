#!/bin/bash

host0=148.247.102.190
host1=148.247.102.192
#host1=148.247.102.191
#host2=148.247.102.192
#host3=148.247.102.193
#host4=148.247.102.140


#source delMatx.sh

declare -a arr=($host0 $host1)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/Debug
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/src

#copy the executable
rsync -r /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/Debug/StaticMtxMultWithXSCALA uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/Debug

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/src/matrixMult.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/src/

#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/scheduleFile.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/

#copy AdjMtxfile 
rsync /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/adjMtx.csv uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA

done


touch resultsMtxMult.txt
count=1
#exec
declare -a szs=(1024)
declare numhosts=(1)
declare devType=(0)
#for j in "${szs[@]}"
#do
for i in "${numhosts[@]}"
do
#echo -n "$i " >> resultsMtxMult.txt #writes the number of hosts
#for k in "${devType[@]}"
#do

time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/Debug/StaticMtxMultWithXSCALA --autotune --static=ISH  --

#time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/StaticMtxMultWithXSCALA/Debug/StaticMtxMultWithXSCALA --taskfile=scheduleFile.cfg  --

done
#sed -i -e '$a\' resultsMtxMult.txt #writes a new line character.
#done
#let count+=1
#done

#exec
#time /home/uriel/mpiELFs/bin/mpirun -np 1  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns 4096

#declare -a szs=(256 512 1024 2048 4096 8192)
#for j in "${szs[@]}"
#do
#time /home/uriel/mpiELFs/bin/mpirun -np 4  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns $j
#done

#time /home/uriel/mpiELFs/bin/mpirun -np 4  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns 8192
 

#time /home/uriel/mpiELFs/bin/mpirun -np 1 /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns 4096


