################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dMutex.c \
../src/dtbLList.c \
../src/mcs-mutex.c \
../src/mutex_bench.c 

OBJS += \
./src/dMutex.o \
./src/dtbLList.o \
./src/mcs-mutex.o \
./src/mutex_bench.o 

C_DEPS += \
./src/dMutex.d \
./src/dtbLList.d \
./src/mcs-mutex.d \
./src/mutex_bench.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-7.5/bin/nvcc -I/home/uriel/mpiELFs/include -G -g -O0   -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-7.5/bin/nvcc -I/home/uriel/mpiELFs/include -G -g -O0 --compile  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


