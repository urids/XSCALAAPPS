#!/bin/bash

#exec
#time /home/uriel/mpiELFs/bin/mpirun -np 2  --hostfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/hostfile.info --rankfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/ranksfile.info  /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/nBodyWithMPIXtns



touch resultsnBody.txt
count=1
#exec
#declare -a szs=(8192 16384 32768 65536 131072 262144 524288 1048576)
declare -a szs=(8192)
declare numhosts=(1)
declare devType=(0) # 1 GPU EXEC
for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> resultsnBody.txt #adds the number of hosts
for k in "${devType[@]}"
do

#time /home/uriel/mpiELFs/bin/mpirun -np $i --hostfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/local_hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/local_ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/nBodyWithMPIXtns $j $k
time /home/uriel/mpiELFs/bin/mpirun -np $i --hostfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/local_hostfile.cfg \
										   --rankfile /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/local_ranksfile.cfg \
										   /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/nBodyWithMPIXtns $j --device=GPU_ONLY --taskfile=scheduleFile.cfg  --

done
sed -i -e '$a\' resultsnBody.txt #writes a new line character.
done
let count+=1
done
#time optirun /home/uriel/mpiELFs/bin/mpirun -mca btl tcp,self -np 1 /home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/Debug/nBodyWithMPIXtns
