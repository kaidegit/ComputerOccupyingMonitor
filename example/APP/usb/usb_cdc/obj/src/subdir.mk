################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Main.c \
../src/my_uart.c \
../src/ringbuffer.c \
../src/usb_cdc.c \
../src/usb_descript.c 

OBJS += \
./src/Main.o \
./src/my_uart.o \
./src/ringbuffer.o \
./src/usb_cdc.o \
./src/usb_descript.o 

C_DEPS += \
./src/Main.d \
./src/my_uart.d \
./src/ringbuffer.d \
./src/usb_cdc.d \
./src/usb_descript.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=1 -I"C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\example\SRC\StdPeriphDriver\inc" -I"C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\example\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

