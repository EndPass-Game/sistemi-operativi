
#include <umps3/umps/types.h>
#include <umps3/umps/bios_defs.h>  // BIOS_EXEC_HANDLERS_ADDRS
#include <list.h>
#include <pandos_types.h>
#include <pandos_const.h>  // KERNELSTACK

// TODO: decide whether if we should refactor these imports
// should we include phase1? phase2 directories?
#include "process.h"
#include "semaphore.h"
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

    mkEmptyProcQ(&g_ready_queue);
    // TODO: initialize device semaphores
    // TODO: load system wide interval timer

    // TODO: refactor this part, needs t9 register??
    pcb_t *pcb = allocReadyPcb();
    pcb->p_s.pc_epc = (memaddr) test;

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