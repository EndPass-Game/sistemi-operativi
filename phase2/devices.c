#include "devices.h"

#include "nucleus.h"
#include "scheduler.h"
#include "semaphore.h"
#include "syscall.h"

static devreg *findDevRegAddr(int device_type);
static void ackDevice(devreg *devreg_addr);
static inline bool isTermReg(memaddr addr);

int resolveDeviceAddress(memaddr memaddress) {
    // 0x1000054 è il base del device normale
    // 0x10000254 questo è il primo indirizzo di termdevice, da qui in poi ho bisogno di due semafori
    // invece che 1

    if (memaddress < DEVREG_START_ADDR)
        return -1;  // non c'è nessun device associato
    else if (memaddress >= DEVREG_START_ADDR && memaddress < TERMREG_START_ADDR)
        return (memaddress - DEVREG_START_ADDR) / DEVREGSIZE;
    else if (memaddress >= TERMREG_START_ADDR && memaddress < TERMREG_END_ADDR)
        return (TERMREG_START_ADDR - DEVREG_START_ADDR) / DEVREGSIZE +
               (memaddress - TERMREG_START_ADDR) / (DEVREGSIZE / 2);
    else
        return -1;  // nessun device oltre a quello
}

int resolveSemAddr(memaddr semaddr) {
    if ((memaddr) semaddr >= (memaddr) &g_sysiostates[0] && (memaddr) semaddr < (memaddr) &g_sysiostates[DEVICE_NUMBER])
        return (semaddr - (memaddr) &g_sysiostates[0]) / sizeof(sysiostate_t);
    else
        return -1;
}

void handleDeviceInt(int device_type) {
    devreg *devreg_addr = findDevRegAddr(device_type);
    if (devreg_addr == NULL) return;  // This should never happen

    int dev_num = resolveDeviceAddress((memaddr) devreg_addr);

    // see documentation, sync semafore section for -1 meaning
    // tl;dr: -1 process killed before IO
    if (g_sysiostates[dev_num].sem_sync != -1) {
        endIO(dev_num);
        g_sysiostates[dev_num].waiting_process->p_s.reg_v0 = 0;
    }

    ackDevice(devreg_addr);
    sysVerhogen(&g_sysiostates[dev_num].sem_sync);

    pcb_t *removed_pcb = removeBlocked(&g_sysiostates[dev_num].sem_mut);
    if (removed_pcb != NULL) {
        beginIO(dev_num, removed_pcb);  // passing the baton pattern
    } else {
        g_sysiostates[dev_num].waiting_process = NULL;
        g_sysiostates[dev_num].sem_mut += 1;
    }
}

int getNumRegister(int *cmdAddr) {
    if ((memaddr) cmdAddr >= DEVREG_START_ADDR && (memaddr) cmdAddr < DEVREG_END_ADDR) {
        return 4;
    } else if ((memaddr) cmdAddr >= TERMREG_START_ADDR && (memaddr) cmdAddr < TERMREG_END_ADDR) {
        return 2;
    } else
        return -1;
}

void beginIO(int devnum, pcb_t *process) {
    g_sysiostates[devnum].waiting_process = process;
    int *cmdAddr = process->cmd_addr;
    int *cmdValues = process->cmd_values;

    for (int i = 0; i < getNumRegister(cmdAddr); i++) {
        cmdAddr[i] = cmdValues[i];
    }
    g_soft_block_count++;
    sysPasseren(&g_sysiostates[devnum].sem_sync);
}

void endIO(int devnum) {
    pcb_t *process = g_sysiostates[devnum].waiting_process;
    int *cmdAddr = process->cmd_addr;
    int *cmdValues = process->cmd_values;

    for (int i = 0; i < getNumRegister(cmdAddr); i++) {
        cmdValues[i] = cmdAddr[i];
    }
    g_soft_block_count--;
}

static devreg *findDevRegAddr(int device_type) {
    // Sto andando a guardare una zona di memoria che è formata da 5 words, una word
    // per device, se il bit i di questa word è on, allora ho trovato il device-iesimo
    // che ha creato l'interrupt.
    int *int_dev_bitmap = (int *) INTDEV_BITMAP;
    devreg *devreg_addr = NULL;
    for (int i = 0; i < 8; i++) {
        if (int_dev_bitmap[device_type - 3] & (1 << i)) {
            devreg_addr = (devreg *) DEVADDR(device_type, i);
            break;
        }
    }

    if (isTermReg((memaddr) devreg_addr)) {
        // se è un terminale, allora devo andare a vedere se è il trasmettitore o il ricevitore
        // gestisco subito il caso in cui sia il trasmittitore, poi in una gestione dell'interrupt futura
        // se è attivo solo il ricevitore, allora gestisco quello
        termdev_t *transmitter = (termdev_t *) (devreg_addr + sizeof(termdev_t) / sizeof(int));
        if ((transmitter->status & DEVICESTATUSMASK) == TRANSMITTED) {
            devreg_addr += sizeof(termdev_t) / sizeof(int);
        }
    }

    return devreg_addr;
}

static void ackDevice(devreg *devreg_addr) {
    if ((memaddr) devreg_addr >= DEVREG_START_ADDR && (memaddr) devreg_addr < TERMREG_END_ADDR) {
        devreg *commandp = (devreg *) (devreg_addr + 1);
        *commandp = ACK;
    }
}

static inline bool isTermReg(memaddr addr) {
    return (memaddr) addr >= TERMREG_START_ADDR && (memaddr) addr < TERMREG_END_ADDR;
}