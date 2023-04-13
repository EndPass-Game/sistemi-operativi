#include "syscall.h"

#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>        // STATUS_KUp
#include <umps3/umps/libumps.h>    // LDST

#include "def-syscall.h"  // sycall codes
#include "exceptions.h"
#include "namespace.h"
#include "nucleus.h"
#include "process.h"  // insertProcQ
#include "scheduler.h"
#include "semaphore.h"
#include "utils.h"

// static void beginIO(int devnum, pcb_t *process);
// static void endIO(int devnum);
static int getNumRegister(int *cmdAddr);

void syscallHandler() {
    if (g_old_state->status & STATUS_KUc) {
        // TODO: simulate program trap, syscall without privilege
        // should this be handled here or other place?
    }

    // see 3.5 of pandos.pdf2
    unsigned int syscall_code = g_old_state->reg_a0;
    unsigned int a1 = g_old_state->reg_a1;
    unsigned int a2 = g_old_state->reg_a2;
    unsigned int a3 = g_old_state->reg_a3;

    // TODO: decidere se per le syscall è meglio passare funzioni
    // (metodo più clean, ma più lento)
    // oppure direttamente prenderle dalla memoria (più veloce)

    unsigned int result = 0;

    switch (syscall_code) {
        case SYSCALL_CREATEPROCESS:
            result = sysCreateProcess((state_t *) a1, (support_t *) a2, (nsd_t *) a3);
            break;
        case SYSCALL_TERMPROCESS:
            sysTerminateProcess((memaddr) a1);
            break;
        case SYSCALL_PASSEREN:
            sysPasseren((int *) a1);
            break;
        case SYSCALL_VERHOGEN:
            sysVerhogen((int *) a1);
            break;
        case SYSCALL_DOIO:
            result = sysDoIO((int *) a1, (int *) a2);
            break;
        case SYSCALL_GETTIME:
            result = sysGetTime();
            break;
        case SYSCALL_CLOCKWAIT:
            sysClockWait();
            break;
        case SYSCALL_GETSUPPORTPTR:
            result = (unsigned int) sysGetSupportPtr();
            break;
        case SYSCALL_GETPROCESSID:
            result = sysGetProcessID((int) a1);
            break;
        case SYSCALL_GETCHILDREN:
            result = sysGetChildren((int *) a1, (int) a2);
            break;
        default:
            passUpOrDie(GENERALEXCEPT);
            break;
    }
    // Nelle system call bloccanti questa parte di codice non viene mai eseguita.

    // see 7.2.3 pops for return register
    g_old_state->reg_v0 = result;

    LDST(g_old_state);
}

memaddr sysCreateProcess(state_t *statep, support_t *supportp, nsd_t *ns) {
    pcb_t *pcb = allocPcb();
    if (pcb == NULL) {
        return -1;
    }
    g_process_count++;
    memcpy((void *) &pcb->p_s, (void *) statep, sizeof(state_t));
    pcb->p_supportStruct = supportp;  // if supportp is null, it's ok

    if (ns != NULL) {
        pcb->namespaces[ns->n_type] = ns;
    }

    insertProcQ(&g_ready_queue, pcb);
    insertChild(g_current_process, pcb);
    // p_time and p_semadd are null/0 initialized by allocPcb.
    return (memaddr) pcb;
}

void sysTerminateProcess(memaddr pid) {
    pcb_t *current = (pcb_t *) pid;
    terminateProcess(current);
    scheduler();
}

void sysPasseren(int *semaddr) {
    if (*semaddr <= 0) {
        memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
        updateProcessTime();
        insertBlocked(semaddr, g_current_process);
        g_current_process = NULL;
        scheduler();
    } else {
        *semaddr = *semaddr - 1;
    }
}

void sysVerhogen(int *semaddr) {
    pcb_t *removed_pcb = removeBlocked(semaddr);
    if (removed_pcb != NULL) {
        insertProcQ(&g_ready_queue, removed_pcb);
    } else {
        *semaddr = *semaddr + 1;
    }
}

int sysDoIO(int *cmdAddr, int *cmdValues) {
    int dev_num = 0;

    // TODO: mettere su epc, t9, e stack per il nuovo frame, dovrebbero bastare queste due
    g_current_process->cmd_addr = cmdAddr;
    g_current_process->cmd_values = cmdValues;

    sysPasseren(&g_sysiostates[dev_num].sem_mut);
    memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
    beginIO(dev_num, g_current_process);

    // questa non dovrebbe mai essere eseguita, si potrebbe mettere un PANIC();
    endIO(dev_num);
    return 0;
}

static int getNumRegister(int *cmdAddr) {
    if ((memaddr) cmdAddr >= DEVREG_START_ADDR && (memaddr) cmdAddr < DEVREG_END_ADDR) {
        return 4;
    } else if ((memaddr) cmdAddr >= TERMREG_START_ADDR && (memaddr) cmdAddr < TERMREG_END_ADDR) {
        return 2;
    } else
        return -1;
    // TODO: gestisci IO su dispositivo non installato, dovrebbe ritornare -1, leggi pg29
}

void endIO(int devnum) {
    pcb_t *process = g_sysiostates[devnum].waiting_process;
    int *cmdAddr = process->cmd_addr;
    int *cmdValues = process->cmd_values;

    for (int i = 0; i < getNumRegister(cmdAddr); i++) {
        cmdValues[i] = cmdAddr[i];
    }
    g_soft_block_count--;
    sysVerhogen(&g_sysiostates[devnum].sem_mut);
}

void beginIO(int devnum, pcb_t *process) {
    g_sysiostates[devnum].waiting_process = process;
    int *cmdAddr = process->cmd_addr;
    int *cmdValues = process->cmd_values;

    for (int i = 0; i < getNumRegister(cmdAddr); i++) {
        cmdAddr[i] = cmdValues[i];
    }
    // int dev_num = resolveDeviceAddress((memaddr) cmdAddr);
    g_soft_block_count++;
    updateProcessTime();

        
    //sysPasseren(&g_sysiostates[devnum].sem_sync);
}

int sysGetTime(void) {
    // TODO: verificare se il p_time è gestito dallo status (cosa che non dovrebbe essere)
    // 3.8 pandos dovrebbe avere la risposta su come fare.
    return g_current_process->p_time + getPassedTime();
}

void sysClockWait(void) {
    // TODO: the other part is handled in the interrupt
    //  the interrupt need to mange this semaphore lika a binary sempahore
    //  **not an ordinary semaphore**

    updateProcessTime();
    sysPasseren(&g_pseudo_clock);
}

support_t *sysGetSupportPtr(void) {
    return g_current_process->p_supportStruct;
}

int sysGetProcessID(int is_parent) {
    pcb_t *parent_pcb = g_current_process->p_parent;

    nsd_t *parent_namespace = NULL;
    if (parent_pcb != NULL) {
        parent_namespace = getNamespace(parent_pcb, PID_NS);
    }
    nsd_t *current_namespace = getNamespace(g_current_process, PID_NS);

    if (parent_namespace != current_namespace) {
        return 0;
    }

    if (!is_parent) {
        return (int) g_current_process;
    } else {
        return (int) parent_pcb;
    }

    return 0;
}
static int getChildsByNamespace(int *children, int *remaining_size, nsd_t *current_namespace, pcb_t *pcb);

int sysGetChildren(int *children, int size) {
    nsd_t *current_namespace = getNamespace(g_current_process, PID_NS);
    return getChildsByNamespace(children, &size, current_namespace, g_current_process);
}

static int getChildsByNamespace(int *children, int *remaining_size, nsd_t *current_namespace, pcb_t *pcb) {
    if (getNamespace(pcb, PID_NS) == current_namespace) return 0;
    if ((*remaining_size) > 0) {
        *remaining_size -= 1;
        children[*remaining_size] = (int) pcb;
    }
    int same_namespace_num = 0;
    struct list_head *pos = NULL;

    list_for_each(pos, &pcb->p_child) {
        pcb_t *curr_pcb = container_of(pos, pcb_t, p_sib);

        same_namespace_num += getChildsByNamespace(
                                  children,
                                  remaining_size,
                                  current_namespace,
                                  curr_pcb
                              ) +
                              1;  // +1 per contare anche child
    }

    return same_namespace_num;
}
