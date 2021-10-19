# Source: https://www.kuon.ch/post/2018-07-11-avr-cmake/
cmake_minimum_required(VERSION 3.11)
# Project name
project("assignment2")

# Product filename
set(PRODUCT_NAME assignment2)

## AVR Chip Configuration
# CPU, you can find the list here:
# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
set(MCU atmega324a)

# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)


# mmcu MUST be passed to bot the compiler and linker, this handle the linker
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU} # MCU
    -std=c11 # C11 standard
    -Os # optimize
    -Wall # enable warnings
    -Wextra
    -Wl,--relax,--gc-sections
    -mcall-prologues # inherited from 
)
file(GLOB SRC_FILES "src/*.c") # Load all files in src folder

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_FILES})

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)

# Upload the firmware with avrdude

# doesn't work - fucking cmake won't let us run the fucking pavr2cmd bash command (fuck you cmake, eat shit and die)
#add_custom_target(flash COMMAND avrdude -c stk500v2 -P $(pavr2cmd --prog-port) -p atmega324pa -F -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

# depends on flash.fish, shitty but it works (we could make it a little less shitty using argv in the fish script)
# TODO try this: https://stackoverflow.com/a/66662298/5007892
add_custom_target(flash WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND ./flash.fish DEPENDS hex)

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.lst")