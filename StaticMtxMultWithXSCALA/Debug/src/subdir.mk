################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/createData.c \
../src/main.c \
../src/printMatrix.c \
../src/refactorData.c 

OBJS += \
./src/createData.o \
./src/main.o \
./src/printMatrix.o \
./src/refactorData.o 

C_DEPS += \
./src/createData.d \
./src/main.d \
./src/printMatrix.d \
./src/refactorData.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/uriel/mpiELFs/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


