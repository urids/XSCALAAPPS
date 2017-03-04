#!/bin/bash

/home/uriel/mpiELFs/bin/mpirun -np 2 --hostfile /home/uriel/Dev/mpiApps/extnsApps/InterNode/hostfile.cfg --rankfile /home/uriel/Dev/mpiApps/extnsApps/InterNode/ranksfile.cfg /home/uriel/Dev/mpiApps/extnsApps/InterNode/Debug/InterNode
