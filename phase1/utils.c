#include "utils.h"

void memset(void *ptr, char byte, u32 size) {
  char b[] = {byte, byte, byte, byte};
  u32 i;
  for (i = 0; i < (size) / 4; i++) {
    *(((int *)ptr) + i) = *((int *)b);
  }
  for (u32 j = 0; j < (size) % 4; j++) {
    *((char *)ptr + j + i * 4) = byte;
  }
}


void memcpy(void *ptr, void *ptr2, u32 size) {
  u32 i;
  for (i = 0; i < (size) / 4; i++) {
    *(((int *)ptr) + i) = *(((int *)ptr2) + i);
  }
  for (u32 j = 0; j < (size) % 4; j++) {
    *((char *)ptr + j + i * 4) = *((char *)ptr2 + j + i * 4);
  }
}