# create the dirs
rsh uriel@192.168.1.103 mkdir /home/uriel/Dev/mpiApps/extnsApps
rsh uriel@192.168.1.103 mkdir /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/
rsh uriel@192.168.1.103 mkdir /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/Debug/
rsh uriel@192.168.1.103 mkdir /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/src

#copy the executable
rsync -r /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/Debug/vectorAddwitMPIXtns uriel@192.168.1.103:/home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/Debug  

#copy the kernel
rsync /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/src/vecAdd.cl uriel@192.168.1.103:/home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/src/

#copy the local scheduling file
rsync /home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/taskSched.cfg uriel@192.168.1.103:/home/uriel/Dev/mpiApps/extnsApps/vectorAddwitMPIXtns/

