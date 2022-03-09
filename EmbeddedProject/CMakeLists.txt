set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(TOOLCHAIN_PATH "C:/softwares/RISC-V-Embedded-GCC/bin")
# specify cross compilers and tools
set(CMAKE_C_COMPILER    ${TOOLCHAIN_PATH}/riscv-none-embed-gcc.exe)
set(CMAKE_CXX_COMPILER  ${TOOLCHAIN_PATH}/riscv-none-embed-g++.exe)
set(CMAKE_ASM_COMPILER  ${TOOLCHAIN_PATH}/riscv-none-embed-gcc.exe)
set(CMAKE_AR            ${TOOLCHAIN_PATH}/riscv-none-embed-ar.exe)
set(CMAKE_OBJCOPY       ${TOOLCHAIN_PATH}/riscv-none-embed-objcopy.exe)
set(CMAKE_OBJDUMP       ${TOOLCHAIN_PATH}/riscv-none-embed-objdump.exe)
set(SIZE                ${TOOLCHAIN_PATH}/riscv-none-embed-size.exe)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(CH571Prj C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore  -mcmodel=medany)
add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common)
add_compile_options(-Wunused -Wuninitialized -O2)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

#if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
#    message(STATUS "Maximum optimization for speed")
#    add_compile_options(-Ofast)
#elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
#    message(STATUS "Maximum optimization for speed, debug info included")
#    add_compile_options(-Ofast -g)
#elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
#    message(STATUS "Maximum optimization for size")
#    add_compile_options(-Os)
#else ()
#    message(STATUS "Minimal optimization, debug info included")
#    add_compile_options(-Og -g)
#endif ()

include_directories(
        SRC/StdPeriphDriver/inc
        SRC/RVMSIS
        APP/src
)

file(GLOB_RECURSE SOURCES "SRC/Startup/*.S" "SRC/RVMSIS/*.c" "SRC/StdPeriphDriver/src/*.c" "APP/src/*.*" )

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/SRC/Ld/Link.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-march=rv32imac -mabi=ilp32  -mcmodel=medany -nostartfiles --specs=nano.specs --specs=nosys.specs)
add_link_options(-lgcc -lc -lm)
add_link_options(-T ${LINKER_SCRIPT})

link_libraries(${CMAKE_SOURCE_DIR}/SRC/StdPeriphDriver/libISP573.a)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
