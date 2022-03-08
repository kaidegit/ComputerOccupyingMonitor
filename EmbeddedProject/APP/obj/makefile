################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
PWMX.hex \

SECONDARY_SIZE += \
PWMX.siz \


# All Target
all: PWMX.elf secondary-outputs

# Tool invocations
PWMX.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -T "C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\EmbeddedProject\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"C:\Users\yekai\Documents\GitHub\ComputerOccupyingMonitor\EmbeddedProject\SRC\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"PWMX.map" --specs=nano.specs --specs=nosys.specs -o "PWMX.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
PWMX.hex: PWMX.elf
	@	riscv-none-embed-objcopy -O ihex "PWMX.elf"  "PWMX.hex"
	@	@
PWMX.siz: PWMX.elf
	@	riscv-none-embed-size --format=berkeley "PWMX.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) PWMX.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
