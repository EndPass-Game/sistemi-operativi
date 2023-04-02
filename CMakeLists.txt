cmake_minimum_required(VERSION 3.5) 

project (sistemi_operativi)
    
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
    "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
    COMMAND ${CMAKE_C_COMPILER} -ffreestanding -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 ${UMPSDIR}/crtso.S -c -o ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
    COMMAND ${CMAKE_C_COMPILER} -ffreestanding -mips1 -mabi=32 -mno-gpopt -EL -G 0 -mno-abicalls -fno-pic -mfp32 -I/usr/include/umps3 ${UMPSDIR}/libumps.S -c -o ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
)


# find all phase 1 files
file(GLOB_RECURSE SOURCES_PHASE1 "phase1/*.c")
file(GLOB_RECURSE SOURCES_PHASE2 "phase2/*.c")

# use flag for phase1 or phase2
option(PHASE1 "Build ONLY phase1 OR both" OFF)

if(PHASE1)
    message(STATUS "Building for ONLY phase1")
    SET (EXEC_NAME "phase1.kernel.elf") 

    add_definitions(-DPHASE1_TEST)
    add_executable(${EXEC_NAME}
        ${SOURCES_PHASE1}
        ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
        ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
    )

else()
    message(STATUS "Building phase1 AND phase2")
    SET (EXEC_NAME "phase2.kernel.elf") 

    add_definitions(-DPHASE2)
    add_executable(${EXEC_NAME}
        ${SOURCES_PHASE1}
        ${SOURCES_PHASE2}
        ${CMAKE_CURRENT_BINARY_DIR}/crtso.o
        ${CMAKE_CURRENT_BINARY_DIR}/libumps.o
    )
endif()
# END PHASE DECISION



target_include_directories(${EXEC_NAME} 
    PRIVATE "include/phase1"
    PRIVATE "include/phase2"
    PRIVATE "include/utils"
    PRIVATE "lib"
    PRIVATE "/usr/include/umps3"
)

set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/phase1.kernel.elf.core.umps  
    ${CMAKE_CURRENT_BINARY_DIR}/phase1.kernel.elf.stab.umps
)
