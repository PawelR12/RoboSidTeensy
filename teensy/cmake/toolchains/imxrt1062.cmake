set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1.0)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(COMPILER_PATH /home/user/.arduino15/packages/teensy/tools/teensy-compile/15.2.1/arm/bin)
set(TOOLCHAIN_PREFIX ${COMPILER_PATH}/arm-none-eabi-)
set(LINKER_SCRIPT "/home/user/RoboSid/RoboSidTeensy/teensy/cmake/toolchains/imxrt1062.ld")
set(CMAKE_C_COMPILER    ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER  ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER        ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE          ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Teensy specific
# Use these lines for Teensy 4.0
set(MCU IMXRT1062)
set(MCU_DEF ARDUINO_TEENSY40)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb")
set(TARGET_FLAGS "${TARGET_FLAGS} -Wall -g -O2 -MMD -ffunction-sections -fdata-sections")
# verify if needed: set(CMAKE_OPTIONS "-DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE")
# verify if needed: OPTIONS += -D__$(MCU)__ -DARDUINO=10813 -DTEENSYDUINO=159 -D$(MCU_DEF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -felide-constructors -fno-exceptions -fpermissive -fno-rtti -Wno-error=narrowing")
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS} -Os -Wl,--gc-sections -T${LINKER_SCRIPT}")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

# add_link_options(LINKER:--ram_model)
# add_link_options(LINKER:--warn_sections)
# add_link_options(${LINKER_COMMANDS})