#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // CAUSE_GET_EXCCODE, exeption codes EXC_*

#include "exceptions.h"
#include "syscall.h"  // syscallHandler

// TODO: move this in appropriate section
static void passUpOrDie();
static void interruptHandler();
static void nonTimerInterruptHandler(int int_line);
static void timerInterruptHandler();

void exceptionHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    // see 3.4 of pandos.pdf, page 19 of pops
    unsigned int cause_bits = CAUSE_GET_EXCCODE(old_state->cause);

    switch (cause_bits) {
    case EXC_INT:
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
        passUpOrDie();
        // TODO: invent a default behaviour, could also be nothing
        break;
    }

    return;
}

static void passUpOrDie() {
    if (/*check support vecto presence*/ false) {
        // TODO;
    } else {
        // support_t *support = NULL; // TODO: assign that of the state, how to get the pcb?

        // Ok for this part we should wait the files, but it's the same for TLB and
        // interrupts!
    }
}



static void interruptHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    int interrupt_mask = DISKINTERRUPT | 
                        FLASHINTERRUPT | 
                        PRINTINTERRUPT | 
                        TERMINTERRUPT  | 
                        LOCALTIMERINT  | 
                        TIMERINTERRUPT;

    // TODO: gestire casi in cui ho più interrupt sullo stesso filo
    // non sono sicuro se sia corretto il settaggio di ~intbit

    // TODO: gestire le interrupt mask singole per ogni linea

    // TODO: these aliases could be useful
    // #define LOCALTIMERN 1
    // #define TIMERN 2
    // #define DISKN 3
    // #define FLASHN 4
    // #define PRINTN 5
    // #define TERMN 6

    // NOTA: non cambiare l'ordine, è importante per la precedenza
    if (old_state->cause & LOCALTIMERINT) { 
        old_state->cause &= ~LOCALTIMERINT;
        timerInterruptHandler();
    } else if (old_state->cause & TIMERINTERRUPT) {
        old_state->cause &= ~TIMERINTERRUPT;
        timerInterruptHandler();
    } else if (old_state->cause & DISKINTERRUPT) {
        old_state->cause &= ~DISKINTERRUPT;
        nonTimerInterruptHandler(3);
    } else if (old_state->cause & FLASHINTERRUPT) {
        old_state->cause &= ~FLASHINTERRUPT;
        nonTimerInterruptHandler(4);
    } else if (old_state->cause & PRINTINTERRUPT) {
        old_state->cause &= ~PRINTINTERRUPT;
        nonTimerInterruptHandler(5);
    } else if (old_state->cause & TERMINTERRUPT) {
        old_state->cause &= ~TERMINTERRUPT;
        nonTimerInterruptHandler(6);
    }

    if (old_state->cause & interrupt_mask) {
        interruptHandler();
    }
}

static void nonTimerInterruptHandler(int int_line) {
    // 1. Calculate the address for this device’s device register. [Section 5.1-pops]
    // 2. Save off the status code from the device’s device register.
    // 3. Acknowledge the outstanding interrupt. This is accomplished by writing
    // the acknowledge command code in the interrupting device’s device register.
    // Alternatively, writing a new command in the interrupting device’s device
    // register will also acknowledge the interrupt.
    // 4. Perform a V operation on the Nucleus maintained semaphore associated
    // with this (sub)device. This operation should unblock the process (pcb)
    // which initiated this I/O operation and then requested to wait for its completion via a SYS5 operation.
    // 5. Place the stored off status code in the newly unblocked pcb’s v0 register.
    // 6. Insert the newly unblocked pcb on the Ready Queue, transitioning this process from the “blocked” state to the “ready” state.
    // 7. Return control to the Current Process: Perform a LDST on the saved exception state (located at the start of the BIOS Data Page [Section 3.4]).

    // TODO: capire come calcolare l'indirizzo del device register
}

static void timerInterruptHandler() {
    // Acknowledge the PLT interrupt by loading the timer with a new value. [Section 4.1.4-pops]
    // • Copy the processor state at the time of the exception (located at the start
    // of the BIOS Data Page [Section 3.2.2-pops]) into the Current Process’s pcb
    // (p s).
    // • Place the Current Process on the Ready Queue; transitioning the Current
    // Process from the “running” state to the “ready” state.
    // • Call the Scheduler

    // TODO:
}