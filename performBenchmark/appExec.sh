#!/bin/bash
host0=148.247.102.190
host1=148.247.102.195
host2=148.247.102.192
host3=148.247.102.193
host4=148.247.102.140



declare -a arr=($host4)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/performBenchmark 
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/Debug
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/src

#copy the executable
rsync  /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/Debug/performBenchmark uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/performBenchmark/Debug/

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/src/gemm.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/performBenchmark/src/gemm.cl

#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/taskSched.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/performBenchmark/taskSched.cfg

done


touch perforBenchmark.txt
count=1
##exec
declare -a szs=(512)
declare numhosts=(1)

for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> perforBenchmark.txt #writes the number of hosts

time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/performBenchmark/Debug/performBenchmark 

sed -i -e '$a\' perforBenchmark.txt #writes a new line character.
done
let count+=1
done



