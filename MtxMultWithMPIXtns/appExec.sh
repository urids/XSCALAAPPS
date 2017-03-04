#!/bin/bash

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
#time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns $j $k
#time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns --taskfile=scheduleFile.cfg  --
#time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/local_hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/local_ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns  --static=HEFT  --


time /home/uriel/mpiELFs/bin/mpirun -np $i --hostfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/local_hostfile.cfg \
										   --rankfile /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/local_ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/Debug/MtxMultWithMPIXtns \
										   --taskfile=scheduleFile.cfg --device=ALL_DEVICES --
										   
										   

done
#sed -i -e '$a\' resultsMtxMult.txt #writes a new line character.
#done
#let count+=1
#done

#--autotune



