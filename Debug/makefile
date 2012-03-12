################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include tinymenu/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
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
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Translator.lss \

FLASH_IMAGE += \

EEPROM_IMAGE += \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \


# All Target
all: Translator.elf secondary-outputs

# Tool invocations
Translator.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Translator.map -mmcu=atmega324a -o"Translator.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

Translator.lss: Translator.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Translator.elf  >"Translator.lss"
	@echo 'Finished building: $@'
	@echo ' '

Translator.hex: Translator.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Translator.elf  "Translator.hex"
	@echo 'Finished building: $@'
	@echo ' '

Translator.eep: Translator.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Translator.elf  "Translator.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Translator.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega324a Translator.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: Translator.elf
	@echo 'Invoking: AVRDude'
	C:\WinAVR-20100110\bin\avrdude -pm324a -cavrisp2 -Pusb -u -Uflash:w:Translator.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(AVRDUDEDUMMY)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) Translator.elf
	-@echo ' '

post-build:
	-avr-objcopy -j .text -j .data -O ihex Translator.elf Translator.hex
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
