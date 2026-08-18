################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/IMU/imu.c \
../Drivers/IMU/imu_calibration.c \
../Drivers/IMU/imu_processing.c 

OBJS += \
./Drivers/IMU/imu.o \
./Drivers/IMU/imu_calibration.o \
./Drivers/IMU/imu_processing.o 

C_DEPS += \
./Drivers/IMU/imu.d \
./Drivers/IMU/imu_calibration.d \
./Drivers/IMU/imu_processing.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/IMU/%.o Drivers/IMU/%.su Drivers/IMU/%.cyclo: ../Drivers/IMU/%.c Drivers/IMU/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/IMU -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-IMU

clean-Drivers-2f-IMU:
	-$(RM) ./Drivers/IMU/imu.cyclo ./Drivers/IMU/imu.d ./Drivers/IMU/imu.o ./Drivers/IMU/imu.su ./Drivers/IMU/imu_calibration.cyclo ./Drivers/IMU/imu_calibration.d ./Drivers/IMU/imu_calibration.o ./Drivers/IMU/imu_calibration.su ./Drivers/IMU/imu_processing.cyclo ./Drivers/IMU/imu_processing.d ./Drivers/IMU/imu_processing.o ./Drivers/IMU/imu_processing.su

.PHONY: clean-Drivers-2f-IMU

