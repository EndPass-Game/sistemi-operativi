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

// TODO: move this in appropriate section
static void interruptHandler();
static void handleDeviceInt(int int_line);
static void handleLocalTimer();
static void handleSysTimer();

void exceptionHandler() {
    // see 3.4 of pandos.pdf, page 19 of pops
    //
    // TODO: forse dovremmo mettere un interrupt block
    //
    unsigned int cause_bits = CAUSE_GET_EXCCODE(g_old_state->cause);

    switch (cause_bits) {
        case EXC_INT:
            updateProcessTime();  // don't charge interrupt to the current process!
            interruptHandler();
            // todo:
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

    if ((memaddr) devreg_addr >= DEVREG_START_ADDR && (memaddr) devreg_addr < DEVREG_END_ADDR) {
        devreg *commandp = (devreg *) (devreg_addr + 1);
        *commandp = ACK;
    } else if ((memaddr) devreg_addr >= TERMREG_START_ADDR && (memaddr) devreg_addr < TERMREG_END_ADDR) {
        // see pops 5.7 page 43, acknoledge both receiver and trasmitter if active
        termdev_t *receiver = (termdev_t *) devreg_addr;
        termdev_t *transmitter = (termdev_t *) (devreg_addr + sizeof(termdev_t) / sizeof(int));
        if ((receiver->status & DEVICESTATUSMASK) == RECEIVED) {
            receiver->command = ACK;
        }

        if ((transmitter->status & DEVICESTATUSMASK) == TRANSMITTED) {
            transmitter->command = ACK;
        }
    }

    // TODO: use g_device semaphore resolver
    int dev_num = 0;

    // questo semaforo è quasi inutile
    sysVerhogen(&g_sysiostates[dev_num].sem_sync);
    state_t *state = &g_sysiostates[dev_num].waiting_process->p_s;
    endIO(dev_num);
    g_soft_block_count--;

    pcb_t *removed_pcb = removeBlocked(&g_sysiostates[dev_num].sem_mut);
    if (removed_pcb != NULL) {
        beginIO(dev_num, removed_pcb);
    } else {
        g_sysiostates[dev_num].sem_mut += 1;
    }
    state->reg_v0 = 0;  // set return value
    LDST(state);
}

static void handleSysTimer() {
    LDIT(PSECOND / 10);

    // Altra implementazione possibile è chiamare V finché ho processi bloccati.
    pcb_t *outBlocked = NULL;
    while ((outBlocked = removeBlocked(&g_pseudo_clock)) != NULL) {
        insertProcQ(&g_ready_queue, outBlocked);
    }

    g_pseudo_clock = 0;
}

static void handleLocalTimer() {
    setTIMER(TIMESLICE);
    memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
    insertProcQ(&g_ready_queue, g_current_process);
    g_current_process = NULL;
    scheduler();
}

// TODO: the functions down there are utilities, should be moved

/**
 * @brief La seguente è una proposta di risoluzione degli address dei device in indici:
 */
int resolveDeviceAddress(memaddr memaddress) {
    // 0x1000054 è il base del device normale

    // 0x10000254 questo è il primo indirizzo di termdevice, da qui in poi ho bisogno di due semafori
    // invece che 1

    if (memaddress < DEVREG_START_ADDR)
        return -1;  // non c'è nessun device associato
    else if (memaddress < DEVREG_END_ADDR)
        return (memaddress - DEVREG_START_ADDR) / DEVREGSIZE;  // dividiamo per lunghezza del registro ossia 16
    else if (memaddress < TERMREG_END_ADDR)
        return (memaddress - TERMREG_START_ADDR) / (DEVREGSIZE / 2) + 32;  // 32 è il numero dei device non term
    else
        return -1;  // nessun device oltre a quello
}

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
