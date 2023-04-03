#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // CAUSE_GET_EXCCODE, exeption codes EXC_*

#include "exceptions.h"
#include "syscall.h"  // syscallHandler

// TODO: capire cosa serva
void uTLB_RefillHandler() {
    setENTRYHI(0x80000000);
    setENTRYLO(0x80000000);
    TLBWR();
    LDST((state_t *) BIOS_DATA_PAGE_BASE);
}

// TODO: move this in appropriate section
static void passUpOrDie();
static void interruptHanlder();

void exceptionHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    // see 3.4 of pandos.pdf, page 19 of pops
    unsigned int cause_bits = CAUSE_GET_EXCCODE(old_state->cause);

    switch (cause_bits) {
    case EXC_INT:
        // TODO: pass to interrupt handler
        interruptHandler();
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

static void passUpOrDie() {
    if (/*check support vecto presence*/ false) {
        // TODO;
    } else {
        support_t *support = NULL; // TODO: assign that of the state, how to get the pcb?

        // Ok for this part we should wait the files, but it's the same for TLB and
        // interrupts!
    }
}

static void interruptHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    int non_timer_mask = DISKINTERRUPT | FLASHINTERRUPT | PRINTINTERRUPT | TERMINTERRUPT;
    int timer_mask =  LOCALTIMERINT | TIMERINTERRUPT;
    // TODO: gestisci casi in cui ci sono piu interrupt aperti


    if (old_state->cause & timer_mask) {
        
    } else if (old_state->cause & non_timer_mask) {
        
    }

}