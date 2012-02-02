################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dokumente/!Praxisbericht/Code/led_lauflicht_sample.c 

OBJS += \
./Dokumente/!Praxisbericht/Code/led_lauflicht_sample.o 

C_DEPS += \
./Dokumente/!Praxisbericht/Code/led_lauflicht_sample.d 


# Each subdirectory must supply rules for building sources it contributes
Dokumente/!Praxisbericht/Code/%.o: ../Dokumente/!Praxisbericht/Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega324a -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


