################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/main.c \
../Core/Src/nsprs_app_srvc.c \
../Core/Src/nsprs_comm.c \
../Core/Src/nsprs_common.c \
../Core/Src/nsprs_drivers.c \
../Core/Src/nsprs_params.c \
../Core/Src/nsprs_platform_nucleo_g070rb.c \
../Core/Src/nsprs_sys_srvc.c \
../Core/Src/stm32_trg_driver.c \
../Core/Src/stm32g0xx_hal_msp.c \
../Core/Src/stm32g0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g0xx.c \
../Core/Src/test_dyn_sch.c 

OBJS += \
./Core/Src/main.o \
./Core/Src/nsprs_app_srvc.o \
./Core/Src/nsprs_comm.o \
./Core/Src/nsprs_common.o \
./Core/Src/nsprs_drivers.o \
./Core/Src/nsprs_params.o \
./Core/Src/nsprs_platform_nucleo_g070rb.o \
./Core/Src/nsprs_sys_srvc.o \
./Core/Src/stm32_trg_driver.o \
./Core/Src/stm32g0xx_hal_msp.o \
./Core/Src/stm32g0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g0xx.o \
./Core/Src/test_dyn_sch.o 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/nsprs_app_srvc.d \
./Core/Src/nsprs_comm.d \
./Core/Src/nsprs_common.d \
./Core/Src/nsprs_drivers.d \
./Core/Src/nsprs_params.d \
./Core/Src/nsprs_platform_nucleo_g070rb.d \
./Core/Src/nsprs_sys_srvc.d \
./Core/Src/stm32_trg_driver.d \
./Core/Src/stm32g0xx_hal_msp.d \
./Core/Src/stm32g0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g0xx.d \
./Core/Src/test_dyn_sch.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G070xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/nsprs_app_srvc.d ./Core/Src/nsprs_app_srvc.o ./Core/Src/nsprs_comm.d ./Core/Src/nsprs_comm.o ./Core/Src/nsprs_common.d ./Core/Src/nsprs_common.o ./Core/Src/nsprs_drivers.d ./Core/Src/nsprs_drivers.o ./Core/Src/nsprs_params.d ./Core/Src/nsprs_params.o ./Core/Src/nsprs_platform_nucleo_g070rb.d ./Core/Src/nsprs_platform_nucleo_g070rb.o ./Core/Src/nsprs_sys_srvc.d ./Core/Src/nsprs_sys_srvc.o ./Core/Src/stm32_trg_driver.d ./Core/Src/stm32_trg_driver.o ./Core/Src/stm32g0xx_hal_msp.d ./Core/Src/stm32g0xx_hal_msp.o ./Core/Src/stm32g0xx_it.d ./Core/Src/stm32g0xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32g0xx.d ./Core/Src/system_stm32g0xx.o ./Core/Src/test_dyn_sch.d ./Core/Src/test_dyn_sch.o

.PHONY: clean-Core-2f-Src

