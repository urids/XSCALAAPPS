#!/bin/bash
touch resultsStrassenV2.txt
count=1
##exec
declare -a szs=(4096)
declare numhosts=(1)

for j in "${szs[@]}"
do
for i in "${numhosts[@]}"
do
echo -n "$i " >> resultsStrassenV2.txt #writes the number of hosts

time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/StrassenV2/Debug/StrassenV2 $j 

sed -i -e '$a\' resultsStrassenV2.txt #writes a new line character.
done
let count+=1
done



