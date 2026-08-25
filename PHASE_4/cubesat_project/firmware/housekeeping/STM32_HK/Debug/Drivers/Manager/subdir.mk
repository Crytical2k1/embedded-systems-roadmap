################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Manager/adc_manager.c \
../Drivers/Manager/i2c_manager.c 

OBJS += \
./Drivers/Manager/adc_manager.o \
./Drivers/Manager/i2c_manager.o 

C_DEPS += \
./Drivers/Manager/adc_manager.d \
./Drivers/Manager/i2c_manager.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Manager/%.o Drivers/Manager/%.su Drivers/Manager/%.cyclo: ../Drivers/Manager/%.c Drivers/Manager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Sensor_Manager -I../Drivers/Manager -I../Drivers/Light -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Manager

clean-Drivers-2f-Manager:
	-$(RM) ./Drivers/Manager/adc_manager.cyclo ./Drivers/Manager/adc_manager.d ./Drivers/Manager/adc_manager.o ./Drivers/Manager/adc_manager.su ./Drivers/Manager/i2c_manager.cyclo ./Drivers/Manager/i2c_manager.d ./Drivers/Manager/i2c_manager.o ./Drivers/Manager/i2c_manager.su

.PHONY: clean-Drivers-2f-Manager

