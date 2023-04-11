#include <umps3/umps/bios_defs.h>  // BIOS_DATA_PAGE_BASE
#include <umps3/umps/cp0.h>    // STATUS_KUp
#include <umps3/umps/libumps.h>  // LDST

#include "def-syscall.h"  // sycall codes
#include "syscall.h"
#include "semaphore.h"
#include "nucleus.h"
#include "scheduler.h"
#include "process.h"  // insertProcQ
#include "utils.h"
#include "namespace.h"
#include "exceptions.h"

void syscallHandler(state_t *old_state) {

    if (old_state->status & STATUS_KUc) {
        // TODO: simulate program trap, syscall without privilege
        // should this be handled here or other place?
    }

    // see 3.5 of pandos.pdf2
    unsigned int syscall_code = old_state->reg_a0;
    unsigned int a1 = old_state->reg_a1;
    unsigned int a2 = old_state->reg_a2;
    unsigned int a3 = old_state->reg_a3;

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

    // see 7.2.3 pops for return register
    old_state->reg_v0 = result;

    // prevent infinite loop, see 3.5.10 pandos.pdf
    old_state->pc_epc += WORDLEN;

    LDST(old_state);
}

memaddr sysCreateProcess(state_t *statep, support_t *supportp, nsd_t *ns) {
    pcb_t *pcb = allocPcb();
    if (pcb == NULL) {
        return -1;
    }
    g_process_count++;
    memcpy((void *) &pcb->p_s, (void *)statep, sizeof(state_t));
    pcb->p_supportStruct = supportp;  // if supportp is null, it's ok
    pcb->namespaces[ns->n_type] = ns;
    
    insertProcQ(&g_ready_queue, pcb);
    insertChild(g_current_process, pcb);

    // p_time and p_semadd are null/0 initialized by allocPcb.
    return (memaddr) pcb;
}

void sysTerminateProcess(memaddr pid) {
    pcb_t *current = (pcb_t *) pid;
    terminateProcess(current);
}

void sysPasseren(int *semaddr) {
    if (*semaddr <= 0) {
        // TODO: should we handle softblock here or other part? NO, NO and NO
        insertBlocked(semaddr, g_current_process);
        g_current_process = NULL;
        scheduler();
    } else {
        *semaddr = *semaddr - 1;
    }
}

pcb_t *sysVerhogen(int *semaddr) {
    pcb_t *removed_pcb = removeBlocked(semaddr);
    if (removed_pcb != NULL) {
        insertProcQ(&g_ready_queue, removed_pcb);
    } else {
        *semaddr = *semaddr + 1;
    }

    return removed_pcb;
}

int sysDoIO(int *cmdAddr, int *cmdValues) {
    // TODO: FIX: how to distinguish between different devices, terminal
    // devices should have 2 semaphores, one for each terminal (write or read)
    // but i don't know how to allocate them

    // calculate the address of the status and command registers of the device
    // see 5.1 pops, pg28. (or see p1test.c)
    // devreg *statusp = (devreg *) DEVSTATUS(cmdAddr));
    devreg *commandp = (devreg *) DEVCOMMAND(cmdAddr[0]);

    int device_number = DEVNUM(cmdAddr[0]);
    int *semaddr = &g_device_semaphores[device_number];

    g_soft_block_count++;
    sysPasseren(semaddr);
    
    *commandp = (devreg) cmdValues[0];  // TODO: write cmdValues[1] to the read end.

    if (g_current_process != NULL) {  // stop the process 
        insertBlocked(semaddr, g_current_process);
        g_current_process = NULL;
        scheduler();
    }

    return 0;
}

int sysGetTime(void) {
    // TODO: verificare se il p_time è gestito dallo status (cosa che non dovrebbe essere)
    // 3.8 pandos dovrebbe avere la risposta su come fare.
    return g_current_process->p_time;
}

void sysClockWait(void) {
    //TODO: the other part is handled in the interrupt
    // the interrupt need to mange this semaphore lika a binary sempahore
    // **not an ordinary semaphore**
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
    }else{
        return (int) parent_pcb;        
    }

    return 0;
}
static int getChildsByNamespace(int *children, int *remaining_size, nsd_t* current_namespace, pcb_t *pcb);

int sysGetChildren(int *children, int size) {
    nsd_t *current_namespace = getNamespace(g_current_process, PID_NS);
    return getChildsByNamespace(children, &size, current_namespace, g_current_process);
}

static int getChildsByNamespace(int *children, int *remaining_size, nsd_t* current_namespace, pcb_t *pcb){
    if(getNamespace(pcb, PID_NS) == current_namespace) return 0;
    if((*remaining_size) > 0){
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
        ) + 1;  // +1 per contare anche child
    }
    
    return same_namespace_num;
}