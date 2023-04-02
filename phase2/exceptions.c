#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // CAUSE_GET_EXCCODE, exeption codes EXC_*

#include "def-syscall.h"  // sycall codes

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

void syscallHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    if (!(old_state->status & STATUS_KUp)) {
        // TODO: simulate program trap, syscall without privilege
    }

    // see 3.5 of pandos.pdf
    unsigned int syscall_code = old_state->reg_a0;
    unsigned int a1 = old_state->reg_a1;
    unsigned int a2 = old_state->reg_a2;
    unsigned int a3 = old_state->reg_a3;

    // TODO: decidere se per le syscall è meglio passare funzioni
    // (metodo più clean, ma più lento)
    // oppure direttamente prenderle dalla memoria (più veloce)

    switch (syscall_code) {
    case SYSCALL_CREATEPROCESS:
        // TODO:
        break;
    case SYSCALL_TERMPROCESS:
        // TODO:
        break;
    case SYSCALL_PASSEREN:
        // TODO:
        break;
    case SYSCALL_VERHOGEN:
        // TODO:
        break;
    case SYSCALL_GETTIME:
        // TODO:
        break;
    case SYSCALL_CLOCKWAIT:
        // TODO:
        break;
    case SYSCALL_GETSUPPORTPTR:
        // TODO:
        break;
    case SYSCALL_GETPROCESSID:
        // TODO:
        break;
    case SYSCALL_GET_CHILDREN:
        // TODO:
        break;
    default:

        break;
    }
}