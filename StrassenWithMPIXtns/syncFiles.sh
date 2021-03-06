#!/bin/bash
host0=148.247.102.190
host1=148.247.102.195
host2=148.247.102.192
host3=148.247.102.193
host4=148.247.102.140


declare -a arr=($host0 $host2 $host3)
for i in "${arr[@]}"
do

rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns 
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/Debug
rsh uriel@$i mkdir -p /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src

#copy the executable
rsync  /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/Debug/StrassenWithMPIXtns uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/Debug/

#copy the src code
#rsync -r /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src/ uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src/

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src/procedures.cl uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src/procedures.cl

#copy scheduling configuration file
rsync /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/scheduleFile.cfg uriel@$i:/home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/scheduleFile.cfg

done
