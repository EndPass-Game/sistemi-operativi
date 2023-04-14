#include "devices.h"

#include "nucleus.h"
#include "scheduler.h"
#include "semaphore.h"
#include "syscall.h"

static devreg *findDevRegAddr(int device_type);
static void ackDevice(devreg *devreg_addr);

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

void handleDeviceInt(int device_type) {
    devreg *devreg_addr = findDevRegAddr(device_type);
    if (devreg_addr == NULL) return;  // This should never happen

    // TODO: use g_device semaphore resolver
    int dev_num = 0;

    endIO(dev_num);
    ackDevice(devreg_addr);

    g_sysiostates[dev_num].waiting_process->p_s.reg_v0 = 0;
    sysVerhogen(&g_sysiostates[dev_num].sem_sync);

    pcb_t *removed_pcb = removeBlocked(&g_sysiostates[dev_num].sem_mut);
    if (removed_pcb != NULL) {
        beginIO(dev_num, removed_pcb);  // passing the baton pattern
    } else {
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
    // TODO: gestisci IO su dispositivo non installato, dovrebbe ritornare -1, leggi pg29
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
    return devreg_addr;
}

static void ackDevice(devreg *devreg_addr) {
    // TODO: check if device has error, if error the procedure should return -1.
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
}