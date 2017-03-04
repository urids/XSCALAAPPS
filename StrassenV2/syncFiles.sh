#!/bin/bash
host0=148.247.102.190
host1=148.247.102.195
host2=148.247.102.192
host3=148.247.102.193
host4=148.247.102.140

declare -a arr=($host2 $host4)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/Debug
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/src

#copy the executable
rsync  /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/Debug/StrassenV2 uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenV2/Debug/
#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/src/kernels.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenV2/src/kernels.cl
#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/taskSched.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenV2/taskSched.cfg
done
