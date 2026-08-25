################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Light/cds_sensor.c 

OBJS += \
./Drivers/Light/cds_sensor.o 

C_DEPS += \
./Drivers/Light/cds_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Light/%.o Drivers/Light/%.su Drivers/Light/%.cyclo: ../Drivers/Light/%.c Drivers/Light/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Sensor_Manager -I../Drivers/Manager -I../Drivers/Light -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Light

clean-Drivers-2f-Light:
	-$(RM) ./Drivers/Light/cds_sensor.cyclo ./Drivers/Light/cds_sensor.d ./Drivers/Light/cds_sensor.o ./Drivers/Light/cds_sensor.su

.PHONY: clean-Drivers-2f-Light

