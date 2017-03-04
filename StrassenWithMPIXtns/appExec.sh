#!/bin/bash

#source syncFiles.sh
#source delMatx.sh 

touch resultsStrassenMtxMult.txt
count=1
##exec
declare -a szs=(2048)
declare numhosts=(1)

for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> resultsStrassenMtxMult.txt #writes the number of hosts
#-x XSCALA_PROFILING_APP=1
time /home/uriel/mpiELFs/bin/mpirun -np $i -x XSCALA_PROFILING_APP=1 --hostfile /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/localhostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/localranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/Debug/StrassenWithMPIXtns $j 

sed -i -e '$a\' resultsStrassenMtxMult.txt #writes a new line character.
done
let count+=1
done


