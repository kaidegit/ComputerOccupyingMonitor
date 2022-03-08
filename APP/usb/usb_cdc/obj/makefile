################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include shell/subdir.mk
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
usb_cdc_uart.hex \

SECONDARY_SIZE += \
usb_cdc_uart.siz \


# 所有目标
all: usb_cdc_uart.elf secondary-outputs

# 工具调用
usb_cdc_uart.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "C:\Users\23714\Desktop\CH573_CDC_OK\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -Xlinker --print-memory-usage -Wl,-Map,"usb_cdc_uart.map" --specs=nano.specs --specs=nosys.specs -o "usb_cdc_uart.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
usb_cdc_uart.hex: usb_cdc_uart.elf
	@	riscv-none-embed-objcopy -O ihex "usb_cdc_uart.elf"  "usb_cdc_uart.hex"
	@	@
usb_cdc_uart.siz: usb_cdc_uart.elf
	@	riscv-none-embed-size --format=berkeley "usb_cdc_uart.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) usb_cdc_uart.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
