cmake_minimum_required(VERSION 3.5) 

project (sistemi_operativi-1)
    
set(CMAKE_C_COMPILER "mipsel-linux-gnu-gcc" CACHE STRING "Set C Compiler" FORCE)

set (CMAKE_C_FLAGS "-Wall -Werror -g -ffreestanding -nostdlib -nostdinc -mips1 -mabi=32 -mfp32 -mno-gpopt -fno-pic -G 0 -mno-abicalls" 
    CACHE STRING "Set C Compiler Flags" FORCE)

set (CMAKE_LINKER "mipsel-linux-gnu-ld" CACHE STRING "Set Linker" FORCE)
set (CMAKE_EXE_LINKER_FLAGS "-G 0 -nostdlib -T/usr/share/umps3/umpscore.ldscript"
    CACHE STRING "Set Linker Flags" FORCE)

SET (EXEC_NAME "phase1.umps3")

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${EXEC_NAME} ${SOURCES})

target_include_directories(${EXEC_NAME} PUBLIC "include")

