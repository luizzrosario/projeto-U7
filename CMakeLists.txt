# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(projeto-U7 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(projeto-U7 projeto-U7.c src/ssd1306.c)

pico_set_program_name(projeto-U7 "projeto-U7")
pico_set_program_version(projeto-U7 "0.1")

# Generate PIO header
pico_generate_pio_header(projeto-U7 ${CMAKE_CURRENT_LIST_DIR}/utils/ws2812.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(projeto-U7 1)
pico_enable_stdio_usb(projeto-U7 1)

# Add the standard library to the build
target_link_libraries(projeto-U7
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(projeto-U7 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # Inclui a pasta include
    ${CMAKE_CURRENT_SOURCE_DIR}/src      # Inclui a pasta src (opcional)
)

# Add any user requested libraries
target_link_libraries(projeto-U7 
        hardware_spi
        hardware_i2c
        hardware_pio
        hardware_pwm
        hardware_adc
        )

pico_add_extra_outputs(projeto-U7)

