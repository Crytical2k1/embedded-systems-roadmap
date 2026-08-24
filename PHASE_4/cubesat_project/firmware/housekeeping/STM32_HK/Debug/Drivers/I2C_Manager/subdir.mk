################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/I2C_Manager/i2c_manager.c 

OBJS += \
./Drivers/I2C_Manager/i2c_manager.o 

C_DEPS += \
./Drivers/I2C_Manager/i2c_manager.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/I2C_Manager/%.o Drivers/I2C_Manager/%.su Drivers/I2C_Manager/%.cyclo: ../Drivers/I2C_Manager/%.c Drivers/I2C_Manager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Sensor_Manager -I../Drivers/I2C_Manager -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-I2C_Manager

clean-Drivers-2f-I2C_Manager:
	-$(RM) ./Drivers/I2C_Manager/i2c_manager.cyclo ./Drivers/I2C_Manager/i2c_manager.d ./Drivers/I2C_Manager/i2c_manager.o ./Drivers/I2C_Manager/i2c_manager.su

.PHONY: clean-Drivers-2f-I2C_Manager

