#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

typedef struct context_t {
    unsigned int stackPtr, status, pc;
} context_t;

typedef struct support_t {
    int sup_asid;
    state_t sup_exceptState[2];
    context_t sup_exceptContext[2];
} support_t;


void uTLB_RefillHandler();

void exceptionHandler();

#endif /* _EXCEPTIONS_H */