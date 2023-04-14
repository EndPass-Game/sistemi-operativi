#include "exceptions.h"

#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>        // CAUSE_GET_EXCCODE, exeption codes EXC_*
#include <umps3/umps/libumps.h>
#include <umps3/umps/types.h>

#include "nucleus.h"
#include "scheduler.h"
#include "semaphore.h"  // removeBlocked1
#include "syscall.h"    // syscallHandler
#include "utils.h"      // memcpy
#include "devices.h"    // interruptHandler

static void interruptHandler();
static void handleLocalTimer();
static void handleSysTimer();

void exceptionHandler() {
    // see 3.4 of pandos.pdf, page 19 of pops
    unsigned int cause_bits = CAUSE_GET_EXCCODE(g_old_state->cause);

    switch (cause_bits) {
        case EXC_INT:
            updateProcessTime();  // don't charge interrupt to the current process!
            interruptHandler();
            break;
        case EXC_MOD:
        case EXC_TLBL:
        case EXC_TLBS:
            passUpOrDie(PGFAULTEXCEPT);
            break;
        case EXC_ADEL:
        case EXC_ADES:
        case EXC_IBE:
        case EXC_DBE:
        case EXC_BP:
        case EXC_RI:
        case EXC_CPU:
        case EXC_OV:
            passUpOrDie(GENERALEXCEPT);
            break;
        case EXC_SYS:
            // prevent infinite loop, see 3.5.10 pandos.pdf
            g_old_state->pc_epc += WORDLEN;
            syscallHandler();
            break;
        default:
            passUpOrDie(GENERALEXCEPT);
            break;
    }

    return;
}

void passUpOrDie(int passupType) {
    if (g_current_process == NULL || g_current_process->p_supportStruct == NULL) {
        sysTerminateProcess((memaddr) g_current_process);
    } else {
        state_t *target_state = &g_current_process->p_supportStruct->sup_exceptState[passupType];
        memcpy((void *) target_state, (void *) BIOS_DATA_PAGE_BASE, sizeof(state_t));
        LDCXT(target_state->reg_sp, target_state->status, target_state->pc_epc);
    }
}

static void interruptHandler() {
    int interrupt_mask = DISKINTERRUPT |
                         FLASHINTERRUPT |
                         PRINTINTERRUPT |
                         TERMINTERRUPT |
                         LOCALTIMERINT |
                         TIMERINTERRUPT;

    // NOTA: non cambiare l'ordine, è importante per la precedenza
    if (g_old_state->cause & LOCALTIMERINT) {
        g_old_state->cause &= ~LOCALTIMERINT;
        handleLocalTimer();
    } else if (g_old_state->cause & TIMERINTERRUPT) {
        g_old_state->cause &= ~TIMERINTERRUPT;
        handleSysTimer();
    } else if (g_old_state->cause & DISKINTERRUPT) {
        g_old_state->cause &= ~DISKINTERRUPT;
        handleDeviceInt(DISK_INTLINE);
    } else if (g_old_state->cause & FLASHINTERRUPT) {
        g_old_state->cause &= ~FLASHINTERRUPT;
        handleDeviceInt(FLASH_INTLINE);
    } else if (g_old_state->cause & PRINTINTERRUPT) {
        g_old_state->cause &= ~PRINTINTERRUPT;
        handleDeviceInt(PRINTER_INTLINE);
    } else if (g_old_state->cause & TERMINTERRUPT) {
        g_old_state->cause &= ~TERMINTERRUPT;
        handleDeviceInt(TERM_INTLINE);
    }

    if (g_old_state->cause & interrupt_mask) {
        interruptHandler();
    }

    STCK(g_tod);
    LDST((int *) BIOS_DATA_PAGE_BASE);
}

static void handleSysTimer() {
    // Altra implementazione possibile è chiamare V finché ho processi bloccati.
    pcb_t *outBlocked = NULL;
    while ((outBlocked = removeBlocked(&g_pseudo_clock)) != NULL) {
        insertProcQ(&g_ready_queue, outBlocked);
    }

    g_pseudo_clock = 0;
    LDIT(PSECOND / 10);
}

static void handleLocalTimer() {
    setTIMER(TIMESLICE);
    memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
    insertProcQ(&g_ready_queue, g_current_process);
    g_current_process = NULL;
    scheduler();
}

/**
 * @brief La seguente è una proposta di risoluzione degli address dei device in indici:
 */
unsigned int getPassedTime() {
    unsigned int tod;
    STCK(tod);
    return tod - g_tod;
}

void updateProcessTime() {
    if (g_current_process != NULL) {
        g_current_process->p_time += getPassedTime();
    }
    STCK(g_tod);
}
