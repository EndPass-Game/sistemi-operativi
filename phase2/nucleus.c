
#include <umps3/umps/types.h>
#include <umps3/umps/bios_defs.h>  // BIOS_EXEC_HANDLERS_ADDRS
#include <umps3/umps/cp0.h>    // STATUS_IEp, STATUS_KUp
#include <umps3/umps/const.h>  // LDIT

#include <list.h>
#include <pandos_types.h>
#include <pandos_const.h>  // KERNELSTACK, RAMTOP

// TODO: decide whether if we should refactor these imports
// should we include phase1? phase2 directories?
#include "process.h"
#include "semaphore.h"
#include "namespace.h"
#include "scheduler.h"
#include "globals.h"
#include "exceptions.h"

extern void test();


// TODO: Device semaphores

/**
 * @brief Allocates a ready pcb. adds 1 to the process count
 * and appends the new process to ready queue
 * The process is ZERO/NULL initialized
 * 
 * @return pcb_t* 
 */
static pcb_t *allocReadyPcb();

/**
 * @brief initializes the pass up vector
 * as described in 3.1 of the project description
 */
static void initPassUpVector();

int main() {
    initPassUpVector();
    initPcbs();
    initASH();
    initNS();

    mkEmptyProcQ(&g_ready_queue);
    // TODO: initialize device semaphores

    // load interval timer with 100 ms
    // TODO: capire cosa intende con "non può essere una semplice constante" nelle
    LDIT(100);

    pcb_t *pcb = allocReadyPcb();
    pcb->p_s.pc_epc = (memaddr) test;
    pcb->p_s.reg_t9 = (memaddr) test;  // modify pc -> modify t9, 10.2-pops
    pcb->p_s.status |= STATUS_IEp | STATUS_KUp | STATUS_TE;
    RAMTOP(pcb->p_s.reg_sp);

    scheduler();
}

static pcb_t *allocReadyPcb() {
    pcb_t *pcb = allocPcb();
    if (pcb == NULL) {
        return NULL;
    }

    g_process_count++;
    insertProcQ(&g_ready_queue, pcb);
    return pcb;
}


static void initPassUpVector() {
    passupvector_t *passupvector = (passupvector_t *) (BIOS_EXEC_HANDLERS_ADDRS);
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    passupvector->tlb_refill_stackPtr = (memaddr) (KERNELSTACK);
    passupvector-> exception_handler = (memaddr) exceptionHandler;
    passupvector->exception_stackPtr = (memaddr) (KERNELSTACK);
}