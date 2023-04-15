#include "syscall.h"

#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>        // STATUS_KUp
#include <umps3/umps/libumps.h>    // LDST

#include "def-syscall.h"  // sycall codes
#include "devices.h"
#include "exceptions.h"
#include "namespace.h"
#include "nucleus.h"
#include "process.h"  // insertProcQ
#include "scheduler.h"
#include "semaphore.h"
#include "utils.h"

/**
 * @brief Termina tutti i PCB figli e il corrente mettendoli nella free list
 *
 * @param pcb
 */
static void terminateProcess(pcb_t *pcb);

/**
 * @brief Controlla se l'indirizzo di semaddr è un semaforo mut o sync di un device
 */
static bool isDeviceSemaphore(memaddr semaddr);

void syscallHandler() {
    if (g_old_state->status & STATUS_KUc) {
        // TODO: capire cosa succede se provo a fare una syscall da user mode
        // a qualcosa accessibile solo da kernel mode
    }
    // see 3.5 of pandos.pdf2
    unsigned int syscall_code = g_old_state->reg_a0;
    unsigned int a1 = g_old_state->reg_a1;
    unsigned int a2 = g_old_state->reg_a2;
    unsigned int a3 = g_old_state->reg_a3;

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
    // NOTA: nelle system call bloccanti (3, 4, 7) questa parte di codice non viene mai eseguita.

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
        addNamespace(pcb, ns);
    } else {
        // copy parent namespaces to curr
        for (int i = 0; i < NS_TYPE_MAX; i++) {
            pcb->namespaces[i] = g_current_process->namespaces[i];
        }
    }

    insertProcQ(&g_ready_queue, pcb);
    insertChild(g_current_process, pcb);
    // p_time and p_semadd are null/0 initialized by allocPcb.
    return (memaddr) pcb;
}

void sysTerminateProcess(memaddr pid) {
    pcb_t *current = (pcb_t *) pid;
    if (current == NULL) {
        current = g_current_process;
    }
    terminateProcess(current);
    scheduler();
}

void sysPasseren(int *semaddr) {
    if (*semaddr == 1) {
        pcb_t *removed_pcb = removeBlocked(semaddr);
        if (removed_pcb != NULL) {
            insertProcQ(&g_ready_queue, removed_pcb);
        } else {
            *semaddr = *semaddr - 1;
        }
        return;
    }
    memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
    updateProcessTime();
    insertBlocked(semaddr, g_current_process);
    g_current_process = NULL;
    scheduler();
}

void sysVerhogen(int *semaddr) {
    if (*semaddr == 0) {
        pcb_t *removed_pcb = removeBlocked(semaddr);
        if (removed_pcb != NULL) {
            insertProcQ(&g_ready_queue, removed_pcb);
        } else {
            *semaddr = *semaddr + 1;
        }
        return;
    }
    memcpy((void *) &g_current_process->p_s, (void *) g_old_state, sizeof(state_t));
    updateProcessTime();
    insertBlocked(semaddr, g_current_process);
    g_current_process = NULL;
    scheduler();
}

int sysDoIO(int *cmdAddr, int *cmdValues) {
    int dev_num = 0;

    g_current_process->cmd_addr = cmdAddr;
    g_current_process->cmd_values = cmdValues;

    sysPasseren(&g_sysiostates[dev_num].sem_mut);
    beginIO(dev_num, g_current_process);
    endIO(dev_num);
    return 0;
}

int sysGetTime(void) {
    // 3.8 pandos dovrebbe avere la risposta su come fare.
    return g_current_process->p_time + getPassedTime();
}

void sysClockWait(void) {
    g_soft_block_count++;
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
static int getChildsByNamespace(int *children, const int total_size, int *used_size, const nsd_t *current_namespace, pcb_t *pcb);

int sysGetChildren(int *children, int size) {
    nsd_t *current_namespace = getNamespace(g_current_process, PID_NS);

    int used_size = 0;
    return getChildsByNamespace(children, size, &used_size, current_namespace, g_current_process) - 1;
}

static int getChildsByNamespace(int *children, const int total_size, int *used_size, const nsd_t *current_namespace, pcb_t *pcb) {
    if (getNamespace(pcb, PID_NS) != current_namespace) return 0;
    if (*used_size < total_size && pcb != g_current_process) {
        children[*used_size] = (int) pcb;
        *used_size += 1;
    }
    int same_namespace_num = 0;
    struct list_head *pos = NULL;

    list_for_each(pos, &pcb->p_child) {
        pcb_t *curr_pcb = container_of(pos, pcb_t, p_sib);
        same_namespace_num += getChildsByNamespace(
            children,
            total_size,
            used_size,
            current_namespace,
            curr_pcb
        );
    }

    return same_namespace_num + 1;
}

static void terminateProcess(pcb_t *pcb) {
    outChild(pcb);

    // Trovare semaforo in cui sono bloccato, e toglierlo.
    // controllare se sono bloccato
    int *blocked_sem = pcb->p_semAdd;
    if (blocked_sem != NULL) {
        pcb_t *removed_pcb = outBlocked(pcb);

        // TODO: trova l'offset corretto per g_sysiostates, a seconda di blocked_sem
        if (removed_pcb != NULL && isDeviceSemaphore((memaddr) blocked_sem) &&
            g_sysiostates[0].waiting_process == pcb) {
            g_soft_block_count--;
            g_sysiostates[0].sem_sync = -1;
        }
    }

    // TODO: esiste una implementazione migliore di terminate process
    // utilizza removeChild, invece di outchild, così non devo storarmi
    // i figli in un array, chiedere a Angelo per dettagli.

    struct list_head *pos = NULL;
    pcb_t *to_eliminate[MAX_PROC];
    int to_eliminate_size = 0;
    list_for_each(pos, &pcb->p_child) {
        pcb_t *child_proc = container_of(pos, pcb_t, p_sib);
        to_eliminate[to_eliminate_size] = child_proc;
        to_eliminate_size++;
    }

    // non mettere terminate process dentro al list for each,
    // non possiamo modificare la lista mentre la iteriamo.
    for (int i = 0; i < to_eliminate_size; i++) {
        terminateProcess(to_eliminate[i]);
    }

    g_process_count--;
    freePcb(pcb);
}

static bool isDeviceSemaphore(memaddr semaddr) {
    memaddr offsetted = (semaddr - (memaddr) &g_sysiostates[0]) % sizeof(sysiostate_t);

    // Attualmente questa implemententazione è basata fortemente sulla struttura di sysiostate
    // che ha nelle prime 2 word i due semafori.
    return offsetted <= 2 * sizeof(int) &&
           (memaddr) semaddr >= (memaddr) &g_sysiostates[0] &&
           (memaddr) semaddr < (memaddr) &g_sysiostates[DEVICE_NUMBER];
}