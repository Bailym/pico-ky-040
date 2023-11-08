cmake_minimum_required(VERSION 3.12) # Require CMake 3.12 or newer

# Directory variables
set(EXAMPLES_DIR examples/)
set(K4-040_LIBRARY_DIR ky-040-library/)

# Include the Pico SDK CMake configuration
include(pico_sdk_import.cmake)

# Set the name and board for the project
set(PICO_BOARD 
    pico_w)

project(PICO-KY-040)

# Initialize the Pico SDK
pico_sdk_init()

# Add executable target
add_executable(PICO-KY-040_EXECUTABLE 
    ${EXAMPLES_DIR}/BasicExample.c
    ${K4-040_LIBRARY_DIR}/ky-040.c)

# Directories to include in build
include_directories(${K4-040_LIBRARY_DIR})

# Pull in external dependencies and additional I2C hardware support
target_link_libraries(PICO-KY-040_EXECUTABLE  
    pico_stdlib)

# Enable USB output, disable UART output
pico_enable_stdio_usb(PICO-KY-040_EXECUTABLE 1)
pico_enable_stdio_uart(PICO-KY-040_EXECUTABLE 0)

# Create map/bin/hex file, etc.
pico_add_extra_outputs(PICO-KY-040_EXECUTABLE)

