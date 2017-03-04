#!/bin/bash

host0=148.247.102.190
host1=148.247.102.195
host2=148.247.102.192
host3=148.247.102.193
host4=148.247.102.140

#rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_A.dat
#rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_B.dat
#rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_C.dat
#rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFileA1.dat
#rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFileB1.dat

declare -a delNodes=($host1) 
for j in "${delNodes[@]}"
do
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_A.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_B.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFile_C.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFileA1.dat
rsh uriel@$j rm /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/dataFileB1.dat
done
