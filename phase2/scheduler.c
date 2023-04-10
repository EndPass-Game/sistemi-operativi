#include <umps3/umps/libumps.h>
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <pandos_const.h>  // TIMESLICE
#include <list.h>

#include "process.h"
#include "globals.h"

int debug_register = 0;

int break_here();

void scheduler() {
    if (emptyProcQ(&g_ready_queue)) {
        if (g_process_count == 0) {
            HALT();
        } else if (g_process_count > 0 && g_soft_block_count > 0) {
            // TODO: this code is not tested, should not work.
            // set the status register to enable interrupts
            // disable the Process Local Timer
            setSTATUS((getSTATUS() | STATUS_IEc) & ~STATUS_TE);
            WAIT();
        } else if (g_process_count > 0 && g_soft_block_count == 0) {
            // deadlock found TODO: how to detech deadlocks?
            PANIC();
        }
    } 

    g_curr_pcb = removeProcQ(&g_ready_queue);

    if (g_curr_pcb == NULL) return;  // this should never happen

    setTIMER(TIMESLICE);
    LDST(&g_curr_pcb->p_s);
    // TODO: capisci come deallocare e decrementare quando
    // un processo finisce
}


int break_here() {
    return 0;
}