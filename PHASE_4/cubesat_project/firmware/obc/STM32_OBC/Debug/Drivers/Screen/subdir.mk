################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Screen/screen.c 

OBJS += \
./Drivers/Screen/screen.o 

C_DEPS += \
./Drivers/Screen/screen.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Screen/%.o Drivers/Screen/%.su Drivers/Screen/%.cyclo: ../Drivers/Screen/%.c Drivers/Screen/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../app/Telemetry_Manager -I../app/Oled_Display -I../Drivers/Screen -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Screen

clean-Drivers-2f-Screen:
	-$(RM) ./Drivers/Screen/screen.cyclo ./Drivers/Screen/screen.d ./Drivers/Screen/screen.o ./Drivers/Screen/screen.su

.PHONY: clean-Drivers-2f-Screen

