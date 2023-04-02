#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // CAUSE_GET_EXCCODE, exeption codes EXC_*


// TODO: capire cosa serva
void uTLB_RefillHandler() {
    setENTRYHI(0x80000000);
    setENTRYLO(0x80000000);
    TLBWR();
    LDST((state_t *) BIOS_DATA_PAGE_BASE);
}

void exceptionHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    // see 3.4 of pandos.pdf, page 19 of pops
    unsigned int cause_bits = CAUSE_GET_EXCCODE(old_state->cause);

    switch (cause_bits) {
    case EXC_INT:
        // TODO: pass to interrupt handler
        break;
    case EXC_MOD:
    case EXC_TLBL:
    case EXC_TLBS:
        // TODO: pass to TLB handler
        break;
    case EXC_ADEL:
    case EXC_ADES:
    case EXC_IBE:
    case EXC_DBE:
    case EXC_BP:
    case EXC_RI:
    case EXC_CPU:
    case EXC_OV:
        // TODO: pass to program trap handler
        break;

    case EXC_SYS:
        syscallHandler();
        break;
    default:
        // TODO: invent a default behaviour, could also be nothing
        break;
    }

    return;
}
