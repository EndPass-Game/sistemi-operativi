cmake_minimum_required(VERSION 3.5) 

project (sistemi_operativi-1)
    
set(UMPSDIR "/usr/share/umps3")

set(CMAKE_C_COMPILER "mipsel-linux-gnu-gcc" CACHE STRING "Set C Compiler" FORCE)
set (CMAKE_C_FLAGS "-Wall -Werror -g -ffreestanding -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32" 
    CACHE STRING "Set C Compiler Flags" FORCE)

set (CMAKE_LINKER "mipsel-linux-gnu-ld" CACHE STRING "Set Linker" FORCE)
set (CMAKE_EXE_LINKER_FLAGS "-G 0 -nostdlib -T${UMPSDIR}/umpscore.ldscript -m elf32ltsmip"
    CACHE STRING "Set Linker Flags" FORCE)

# https://cmake.org/pipermail/cmake/2014-August/058268.html
# CMAKE uses gcc to link, but this gcc can´t link, so we need to overwrite this behaviour
set(CMAKE_C_LINK_EXECUTABLE
    "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

SET (EXEC_NAME "phase1.kernel.elf") 

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
    COMMAND ${CMAKE_C_COMPILER} -ffreestanding -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 ${UMPSDIR}/crtso.S -c -o ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
    COMMAND ${CMAKE_C_COMPILER} -ffreestanding -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 ${UMPSDIR}/libumps.S -c -o ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
)


file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${EXEC_NAME} ${SOURCES}
${CMAKE_CURRENT_BINARY_DIR}/crtso.o
${CMAKE_CURRENT_BINARY_DIR}/libumps.o
)

target_include_directories(${EXEC_NAME} 
PUBLIC "include"
PUBLIC "/usr/include/umps3"
)

# also clean custom commands and core and stab files
# TODO: phases1.kernel.elf is not deleted
set_directory_properties(ADDITIONAL_MAKE_CLEAN_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
    ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
    ${CMAKE_CURRENT_BINARY_DIR}/phase1.kernel.elf.core.umps  
    ${CMAKE_CURRENT_BINARY_DIR}/phase1.kernel.elf.stab.umps
)

