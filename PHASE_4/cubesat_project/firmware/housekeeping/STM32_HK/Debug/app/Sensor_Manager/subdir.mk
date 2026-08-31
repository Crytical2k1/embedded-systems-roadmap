################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/Sensor_Manager/sensor_manager.c 

OBJS += \
./app/Sensor_Manager/sensor_manager.o 

C_DEPS += \
./app/Sensor_Manager/sensor_manager.d 


# Each subdirectory must supply rules for building sources it contributes
app/Sensor_Manager/%.o app/Sensor_Manager/%.su app/Sensor_Manager/%.cyclo: ../app/Sensor_Manager/%.c app/Sensor_Manager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Telemetry_Manager -I../app/Sensor_Manager -I../Drivers/Manager -I../Drivers/Light -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-Sensor_Manager

clean-app-2f-Sensor_Manager:
	-$(RM) ./app/Sensor_Manager/sensor_manager.cyclo ./app/Sensor_Manager/sensor_manager.d ./app/Sensor_Manager/sensor_manager.o ./app/Sensor_Manager/sensor_manager.su

.PHONY: clean-app-2f-Sensor_Manager

