#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>


// TODO: capire cosa serva
void uTLB_RefillHandler() {
    setENTRYHI(0x80000000);
    setENTRYLO(0x80000000);
    TLBWR();
    LDST((state_t *) 0x0FFFF000);
}

void exceptionHandler() {
    // TODO: implementare
    return;
}