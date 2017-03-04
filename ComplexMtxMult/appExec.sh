#!/bin/bash

touch resultsCplx.txt
count=1
#exec
declare -a szs=(2000)
declare numhosts=(1)
declare devType=(0) ##2->CPU, 1->GPU 0->ALL
for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> resultsCplx.txt 
for k in "${devType[@]}"
do
time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/Debug/ComplexMtxMult $j $k
done
sed -i -e '$a\' resultsCplx.txt #writes a new line character.
done
let count+=1
done

#time /home/uriel/mpiELFs/bin/mpirun -np 1 /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/Debug/vectorAddwitMPIXtns
