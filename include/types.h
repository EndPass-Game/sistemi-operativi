#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned int u32;
typedef unsigned int size_t;

/* Device register type for terminals */
typedef struct {
    unsigned int recv_status;
    unsigned int recv_command;
    unsigned int transm_status;
    unsigned int transm_command;
} termreg_t;

#endif  // _TYPES_H
