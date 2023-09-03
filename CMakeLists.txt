cmake_minimum_required(VERSION 3.10.0)
#####################################################################
# Setup the toolchain                                               #
#####################################################################
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arm-gcc-toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL ON FORCE)

project(stm32l4s5xx)
enable_language(C CXX ASM)
#####################################################################
# Build freeRTOS kernel as a static library                         #
#####################################################################
add_library(freertos_config INTERFACE)
set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")

target_include_directories(freertos_config SYSTEM
    INTERFACE Core/Inc)
target_compile_options(freertos_config INTERFACE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fdata-sections
    -ffunction-sections
    -Wall
    $<$<CONFIG:Debug>:-Og -gdwarf-2>)
add_subdirectory(FreeRTOS-Kernel)
#####################################################################
# Build the firmware                                                #
#####################################################################
set(STM32CUBEMX_GENERATED_FILES
    
    Core/Src/main.cpp
    Core/Src/clock.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    
    Core/Src/gpio.c
    Core/Src/adc.c
    Core/Src/dfsdm.c
    Core/Src/i2c.c
    Core/Src/octospi.c
    Core/Src/spi.c
    Core/Src/usart.c
    Core/Src/usb_otg.c
    Core/Src/stm32l4xx_it.c
    Core/Src/stm32l4xx_hal_msp.c
    Core/Src/stm32l4xx_hal_timebase_tim.c
    Core/Src/system_stm32l4xx.c

    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ospi.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
    Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
    
    # startup code in assembly
    startup_stm32l4s5xx.s )

add_executable(${PROJECT_NAME} ${STM32CUBEMX_GENERATED_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        -DUSE_HAL_DRIVER 
        -DSTM32L4S5xx)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fdata-sections
    -ffunction-sections
    -Wall
    $<$<CONFIG:Debug>:-Og -gdwarf-2>)

target_include_directories(${PROJECT_NAME} PRIVATE 
        Core/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
        Drivers/CMSIS/Include
        FreeRTOS-Kernel/include
        FreeRTOS-Kernel/portable/GCC/ARM_CM4F)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE freertos_kernel)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32L4S5VITx_FLASH.ld
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nano.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections)

#####################################################################
# Add custom post-build commands                                    #
#####################################################################
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.bin)