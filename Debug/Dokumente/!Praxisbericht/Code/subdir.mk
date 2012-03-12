################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dokumente/!Praxisbericht/Code/FindString.c \
../Dokumente/!Praxisbericht/Code/ISR_Endschalter.c \
../Dokumente/!Praxisbericht/Code/led.c \
../Dokumente/!Praxisbericht/Code/menu.c \
../Dokumente/!Praxisbericht/Code/rs232_empfang.c \
../Dokumente/!Praxisbericht/Code/rs232_empfang_stepper.c \
../Dokumente/!Praxisbericht/Code/switch_Motor.c \
../Dokumente/!Praxisbericht/Code/switch_stepper.c \
../Dokumente/!Praxisbericht/Code/uart_rx.c 

OBJS += \
./Dokumente/!Praxisbericht/Code/FindString.o \
./Dokumente/!Praxisbericht/Code/ISR_Endschalter.o \
./Dokumente/!Praxisbericht/Code/led.o \
./Dokumente/!Praxisbericht/Code/menu.o \
./Dokumente/!Praxisbericht/Code/rs232_empfang.o \
./Dokumente/!Praxisbericht/Code/rs232_empfang_stepper.o \
./Dokumente/!Praxisbericht/Code/switch_Motor.o \
./Dokumente/!Praxisbericht/Code/switch_stepper.o \
./Dokumente/!Praxisbericht/Code/uart_rx.o 

C_DEPS += \
./Dokumente/!Praxisbericht/Code/FindString.d \
./Dokumente/!Praxisbericht/Code/ISR_Endschalter.d \
./Dokumente/!Praxisbericht/Code/led.d \
./Dokumente/!Praxisbericht/Code/menu.d \
./Dokumente/!Praxisbericht/Code/rs232_empfang.d \
./Dokumente/!Praxisbericht/Code/rs232_empfang_stepper.d \
./Dokumente/!Praxisbericht/Code/switch_Motor.d \
./Dokumente/!Praxisbericht/Code/switch_stepper.d \
./Dokumente/!Praxisbericht/Code/uart_rx.d 


# Each subdirectory must supply rules for building sources it contributes
Dokumente/!Praxisbericht/Code/%.o: ../Dokumente/!Praxisbericht/Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega324a -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


