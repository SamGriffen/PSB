# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/samgriffen/raspberry-pi-pico/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Include the RF24 library
include(lib/RF24/CMakeLists.txt)

project(AudioTrans C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(AudioTrans 
        AudioTrans.cpp
        NRF24.cpp
)

pico_set_program_name(AudioTrans "AudioTrans")
pico_set_program_version(AudioTrans "0.1")

pico_enable_stdio_uart(AudioTrans 1)
pico_enable_stdio_usb(AudioTrans 0)

# Add the standard library to the build
target_link_libraries(AudioTrans pico_stdlib)

# Add any user requested libraries
target_link_libraries(AudioTrans
        pico_stdio_uart
        hardware_spi
        hardware_adc
)

pico_add_extra_outputs(AudioTrans)

