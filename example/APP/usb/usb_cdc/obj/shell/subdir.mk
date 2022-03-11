################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shell/shell.c \
../shell/shell_cmd.c \
../shell/shell_ext.c 

OBJS += \
./shell/shell.o \
./shell/shell_cmd.o \
./shell/shell_ext.o 

C_DEPS += \
./shell/shell.d \
./shell/shell_cmd.d \
./shell/shell_ext.d 


# Each subdirectory must supply rules for building sources it contributes
shell/%.o: ../shell/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=1 -I"C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\example\SRC\StdPeriphDriver\inc" -I"C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\example\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

