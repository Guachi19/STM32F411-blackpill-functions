cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(Plantilla C)

add_executable(firmware
    Src/main.c
    Src/startup.c
    Src/syscall.c
    Src/sysmem.c
)

set_target_properties(firmware PROPERTIES SUFFIX ".elf")

target_include_directories(firmware PRIVATE Inc)
target_compile_options(firmware PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-Og;-g3>
    $<$<CONFIG:Release>:-Os>
)

target_link_options(firmware PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    --specs=nosys.specs
    -T${CMAKE_SOURCE_DIR}/linker.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/firmware.map
    -Wl,--print-memory-usage
)

find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy REQUIRED)
add_custom_command(TARGET firmware POST_BUILD
    COMMAND ${ARM_NONE_EABI_OBJCOPY} -O ihex $<TARGET_FILE:firmware> ${CMAKE_CURRENT_BINARY_DIR}/firmware.hex
    COMMAND ${ARM_NONE_EABI_OBJCOPY} -O binary $<TARGET_FILE:firmware> ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/firmware.hex ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
    COMMENT "Generating Intel HEX and binary firmware images"
)

