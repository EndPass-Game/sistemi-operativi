#include <umps3/umps/libumps.h>
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <pandos_const.h>  // TIMESLICE
#include <list.h>

#include "process.h"
#include "globals.h"

void scheduler() {
    if (emptyProcQ(&g_ready_queue)) {
        if (g_process_count == 0) {
            HALT();
        } else if (g_process_count > 0 && g_soft_block_count > 0) {
            // set the status register to enable interrupts
            setSTATUS(getSTATUS() | STATUS_IEc);
            // disable the Process Local Timer
            setSTATUS(getSTATUS() & ~STATUS_TE);
            WAIT();
        } else if (g_process_count > 0 && g_soft_block_count == 0) {
            // deadlock found TODO: how to detech deadlocks?
            PANIC();
        }
    } 

    pcb_t *pcb = removeProcQ(&g_ready_queue);
    setTIMER(TIMESLICE); // TODO: verificare se sono veramente 5 ms, e poi?
    LDST(&pcb->p_s);

    // TODO: capisci come deallocare e decrementare quando
    // un processo finisce
}
