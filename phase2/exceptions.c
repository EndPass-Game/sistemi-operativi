#include "exceptions.h"

#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>        // CAUSE_GET_EXCCODE, exeption codes EXC_*
#include <umps3/umps/libumps.h>
#include <umps3/umps/types.h>

#include "nucleus.h"
#include "syscall.h"  // syscallHandler
#include "semaphore.h"  // removeBlocked1
#include "utils.h"    // memcpy

// TODO: move this in appropriate section
static void interruptHandler();
static void handleDeviceInt(int int_line);
static void handleLocalTimer();
static void handleSysTimer();


void exceptionHandler() {
    state_t *old_state = (state_t *) BIOS_DATA_PAGE_BASE;

    // see 3.4 of pandos.pdf, page 19 of pops
    unsigned int cause_bits = CAUSE_GET_EXCCODE(old_state->cause);

    switch (cause_bits) {
        case EXC_INT:
            interruptHandler(old_state);
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
            syscallHandler(old_state);
            break;
        default:
            passUpOrDie(GENERALEXCEPT);
            // TODO: invent a default behaviour, could also be nothing
            break;
    }

    return;
}

void passUpOrDie(int passupType) {
    if (g_current_process == NULL || g_current_process->p_supportStruct == NULL) {
        sysTerminateProcess((memaddr) g_current_process);
    } else {
        state_t *target_state = &g_current_process->p_supportStruct->sup_exceptState[passupType];
        memcpy((void *) BIOS_DATA_PAGE_BASE, (void *) target_state, sizeof(state_t));
        LDCXT(target_state->reg_sp, target_state->status, target_state->pc_epc);
    }
}

static void interruptHandler(state_t *old_state) {

    int interrupt_mask = DISKINTERRUPT |
                         FLASHINTERRUPT |
                         PRINTINTERRUPT |
                         TERMINTERRUPT |
                         LOCALTIMERINT |
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
        handleLocalTimer();
    } else if (old_state->cause & TIMERINTERRUPT) {
        old_state->cause &= ~TIMERINTERRUPT;
        handleSysTimer();
    } else if (old_state->cause & DISKINTERRUPT) {
        old_state->cause &= ~DISKINTERRUPT;
        handleDeviceInt(DISK_INTLINE);
    } else if (old_state->cause & FLASHINTERRUPT) {
        old_state->cause &= ~FLASHINTERRUPT;
        handleDeviceInt(FLASH_INTLINE);
    } else if (old_state->cause & PRINTINTERRUPT) {
        old_state->cause &= ~PRINTINTERRUPT;
        handleDeviceInt(PRINTER_INTLINE);
    } else if (old_state->cause & TERMINTERRUPT) {
        old_state->cause &= ~TERMINTERRUPT;
        handleDeviceInt(TERM_INTLINE);
    }

    if (old_state->cause & interrupt_mask) {
        interruptHandler(old_state);
    }
}

int debug_arr[2];
static void handleDeviceInt(int device_type) {
    // Sto andando a guardare una zona di memoria che è formata da 5 words, una word
    // per device, se il bit i di questa word è on, allora ho trovato il device-iesimo
    // che ha creato l'interrupt.
    int *int_dev_bitmap = (int *) INTDEV_BITMAP;
    int *devreg_addr = NULL;
    for (int i = 0; i < 8; i++) {
        if (int_dev_bitmap[device_type - 3] & (1 << i)) {
            devreg_addr = (int *) DEVADDR(device_type, i);
            break;
        }
    }

    if (devreg_addr == NULL) return;  // This should never happen

    int status_code = 0;
    if ((memaddr) devreg_addr >= DEVREG_START_ADDR && (memaddr) devreg_addr < DEVREG_END_ADDR) {
        devreg *statusp = (devreg *) devreg_addr;
        devreg *commandp = (devreg *) (devreg_addr + 1);
        status_code = *statusp;
        *commandp = ACK;
    } else if ((memaddr) devreg_addr >= TERMREG_START_ADDR && (memaddr) devreg_addr < TERMREG_END_ADDR) {
        // see pops 5.7 page 43, acknoledge both receiver and trasmitter if active
        termdev_t *receiver = (termdev_t *) devreg_addr;
        termdev_t *transmitter = (termdev_t *) (devreg_addr + sizeof(termdev_t) / sizeof(int));
        if ((receiver->status & DEVICESTATUSMASK) == RECEIVED) {
            status_code = receiver->status;
            receiver->command = ACK;
        }
        
        if ((transmitter->status & DEVICESTATUSMASK) == TRANSMITTED) {
            status_code = transmitter->status;  // NOTE: the status code is overwritten (but should be the same)
            transmitter->command = ACK;
        }
    }

    // TODO: use g_device semaphore resolver
    int *semaddr = &g_device_semaphores[0];
    g_soft_block_count--;
    pcb_t *unblocked = sysVerhogen(semaddr);
    if (unblocked == NULL) {
        return;  // This should never happen
    }

    debug_arr[0] = emptyProcQ(&g_ready_queue);
    debug_arr[1] = headBlocked(semaddr) == NULL;

    unblocked->p_s.reg_v0 = status_code;

    LDST((int *) BIOS_DATA_PAGE_BASE);
}

static void handleSysTimer() {
    LDIT(PSECOND / 10);   

    // Altra implementazione possibile è chiamare V finché ho processi bloccati.
    pcb_t *outBlocked = NULL;
    while ((outBlocked = removeBlocked(&g_pseudo_clock)) != NULL) {
        insertProcQ(&g_ready_queue, outBlocked);
    }

    g_pseudo_clock = 0;

    LDST((int *) BIOS_DATA_PAGE_BASE);
}

static void handleLocalTimer() {
    // Acknowledge the PLT interrupt by loading the timer with a new value. [Section 4.1.4-pops]
    // • Copy the processor state at the time of the exception (located at the start
    // of the BIOS Data Page [Section 3.2.2-pops]) into the Current Process’s pcb
    // (p s).
    // • Place the Current Process on the Ready Queue; transitioning the Current
    // Process from the “running” state to the “ready” state.
    // • Call the Scheduler

    // TODO:
}


/**
 * @brief 

La seguente è una proposta di risoluzione degli address dei device in indici:

int addressResolver(int memaddress) {
    0x1000054 è il base del device normale

    0x10000254 questo è il primo indirizzo di termdevice, da qui in poi ho bisogno di due semafori
    invece che 1
    
    if (memaddress < 0x10000054) return -1; // non c'è nessun device associato
    else if (memaddress < 0x10000254) return (memaddress - 0x10000054) / DEVREGSIZE;  // dividiamo per lunghezza del registro ossia 16
    else if (memaddress < 0x10000254 + 0x80) return (memaddress - 0x10000254) / (DEVREGSIZE / 2) + 32;
    else return -1; // nessun device oltre a quello
}

 */