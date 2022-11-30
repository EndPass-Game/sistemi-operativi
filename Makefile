CC=mipsel-linux-gnu-gcc
CFLAGS=-Wall -Werror -ffreestanding -nostdlib -nostdinc -mips1 -mabi=32 -mfp32 -mno-gpopt -fno-pic -G O -mno-abicalls
INCLUDES=-I/usr/umps3 -I/usr/include

LDFLAGS= -G O -nostdlib -T/usr/umps3/umpscore.ldscript

TARGET=src/namespace.c 
all: $(TARGET)