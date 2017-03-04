################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: performBenchmark

# Tool invocations
performBenchmark: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L/home/uriel/mpiELFs/lib -Wl,-rpath=/home/uriel/mpiELFs/lib/XCLFrame/multiDeviceMgmt -Wl,-rpath,'/home/uriel/mpiELFs/lib/XCLFrame/binding/libtskMgmt.so' -Wl,-rpath,'/home/uriel/mpiELFs/lib/XCLFrame/binding/libdvMgmt.so' -o "performBenchmark" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) performBenchmark
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
