#include "utils.h"

void memset(void *ptr, char byte, u32 size) {
    for (u32 i = 0; i < size; i++) {
        *((char *) ptr + i) = byte;
    }
}

void memcpy(void *ptr, void* ptr2, u32 size) {
    for (u32 i = 0; i < size; i++) {
        *((char *) ptr + i) = *((char *) ptr2 + i);
    }
}

