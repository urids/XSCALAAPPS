#!/bin/bash

time /home/uriel/mpiELFs/bin/mpirun -np 1  --hostfile /home/uriel/Dev/mpiApps/extnsApps/XSCALA_HelloWorld/Debug/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/XSCALA_HelloWorld/Debug/ranksfile.cfg  /home/uriel/Dev/mpiApps/extnsApps/XSCALA_HelloWorld/Debug/XSCALA_HelloWorld
