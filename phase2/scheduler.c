#include <list.h>
#include <pandos_const.h>    // TIMESLICE
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <umps3/umps/libumps.h>

#include "nucleus.h"
#include "process.h"

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
    } else {
        g_current_process = removeProcQ(&g_ready_queue);
        setTIMER(TIMESLICE);
        LDST(&g_current_process->p_s);
        // TODO: capisci come deallocare e decrementare quando
        // un processo finisce
    }
}