cmake_minimum_required(VERSION 3.10.0)

# Setup the toolchain
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arm-gcc-toolchain.cmake)

project(stm32l4s5xx)
enable_language(C CXX ASM)

option(SEMIHOSTING "Option to enable or disable semihosting" OFF)

set(STM32CUBEMX_GENERATED_FILES
    
    Core/Src/main.c
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
    startup_stm32l4s5xx.s
)

add_executable(${PROJECT_NAME} ${STM32CUBEMX_GENERATED_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        -DUSE_HAL_DRIVER -DSTM32L4S5xx
)

target_include_directories(${PROJECT_NAME} PRIVATE 
        Core/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc
        Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32L4xx/Include
        Drivers/CMSIS/Include
)

target_compile_options(${PROJECT_NAME} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall

        $<$<CONFIG:Debug>:-Og -gdwarf-2>
)

if(${SEMIHOSTING} AND ON)
    message("Semihosting is enabled.")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE -DSEMIHOSTING
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            -T${CMAKE_SOURCE_DIR}/STM32L4S5VITx_FLASH.ld
            -mcpu=cortex-m4
            -mthumb
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
            -specs=rdimon.specs
            -lc
            -lrdimon
            -lm
            -lnosys
            -Wl,-Map=${PROJECT_NAME}.map,--cref
            -Wl,--gc-sections
    )
else()
    message("Semihosting is disabled.")
    target_link_options(${PROJECT_NAME} PRIVATE
        -T${CMAKE_SOURCE_DIR}/STM32L4S5VITx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        # -specs=nosys.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
    )
endif()

# Add custom post   build commands
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.bin
)


# Clear from cmake cache
unset(SEMIHOSTING CACHE)