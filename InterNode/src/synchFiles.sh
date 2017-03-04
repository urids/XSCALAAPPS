#!/bin/bash

host0=148.247.102.190
appPath=/home/uriel/Dev/mpiApps/extnsApps/InterNode/
declare -a arr=($host0)
for i in "${arr[@]}"
do
rsh uriel@$i mkdir -p $appPath/Debug/
#copy the executable
rsync $appPath/Debug/InterNode uriel@$i:$appPath/Debug
done
