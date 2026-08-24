################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/RTC/rtc.c 

OBJS += \
./Drivers/RTC/rtc.o 

C_DEPS += \
./Drivers/RTC/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/RTC/%.o Drivers/RTC/%.su Drivers/RTC/%.cyclo: ../Drivers/RTC/%.c Drivers/RTC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../app/Sensor_Manager -I../Drivers/I2C_Manager -I../Drivers/Temperature -I../Drivers/RTC -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-RTC

clean-Drivers-2f-RTC:
	-$(RM) ./Drivers/RTC/rtc.cyclo ./Drivers/RTC/rtc.d ./Drivers/RTC/rtc.o ./Drivers/RTC/rtc.su

.PHONY: clean-Drivers-2f-RTC

