#!/bin/bash
#source synchFiles.sh

touch resultsToken.txt
count=1

declare numhosts=(1)
declare devType=(0) #2->CPU, 1->GPU 0->ALL



for i in "${numhosts[@]}"
do
#echo -n "$i " >> resultsToken.txt 
for k in "${devType[@]}"
do
time /home/uriel/mpiELFs/bin/mpirun -np $i  --hostfile /home/uriel/Dev/mpiApps/extnsApps/tokenRing/local_hostfile.cfg \
											--rankfile /home/uriel/Dev/mpiApps/extnsApps/tokenRing/local_ranksfile.cfg \
											/home/uriel/Dev/mpiApps/extnsApps/tokenRing/Debug/tokenRing  --taskfile=taskSched.cfg --
done
#sed -i -e '$a\' resultsToken.txt #writes a new line character.
done
#let count+=1


