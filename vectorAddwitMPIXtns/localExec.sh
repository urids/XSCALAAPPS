#!/bin/bash

host0=148.247.102.195
touch results2.txt
count=1

declare -a szs=(16777216)
declare -a numhosts=(1)
declare -a devType=(0)
for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> results2.txt 
for k in "${devType[@]}"
do
							
time /home/uriel/mpiELFs/bin/mpirun -np $i --hostfile /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/local_hostfile.cfg --rankfile  /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/local_ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/Debug/vectorAddwitMPIXtns $j --taskfile=scheduleFile.cfg --device=$k --

done
sed -i -e '$a\' results2.txt #writes a new line character.
done
sed -i -e "$count s/^/$i /" results2.txt
let count+=1
done

