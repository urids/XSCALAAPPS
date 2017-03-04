#!/bin/bash
source syncFiles.sh


declare numhosts=(1)
for i in "${numhosts[@]}"
do
time /home/uriel/mpiELFs/bin/mpirun -np $i --hostfile /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/local_hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/local_ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/Debug/DynamicTokenRing --dynamic=RR --device=ALL_DEVICES --
done



