################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/Sensor_Queue/sensor_queue.c 

OBJS += \
./app/Sensor_Queue/sensor_queue.o 

C_DEPS += \
./app/Sensor_Queue/sensor_queue.d 


# Each subdirectory must supply rules for building sources it contributes
app/Sensor_Queue/%.o app/Sensor_Queue/%.su app/Sensor_Queue/%.cyclo: ../app/Sensor_Queue/%.c app/Sensor_Queue/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Sensor_Manager -I../Drivers/I2C_Manager -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-Sensor_Queue

clean-app-2f-Sensor_Queue:
	-$(RM) ./app/Sensor_Queue/sensor_queue.cyclo ./app/Sensor_Queue/sensor_queue.d ./app/Sensor_Queue/sensor_queue.o ./app/Sensor_Queue/sensor_queue.su

.PHONY: clean-app-2f-Sensor_Queue

