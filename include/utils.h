#ifndef _UTILS_H
#define _UTILS_H

typedef unsigned int size_t;

/**
 * @brief Copies n bytes from src to dest, own implementation of memcpy
 * 
 * @param dest destination address to write to
 * @param src pointer of the source address to copy from
 * @param n number of bytes to copy
 * @return void* the pointer to dest
 */
void *memcpy(void *dest, const void *src, size_t n);

#endif // _UTILS_H
