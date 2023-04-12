#include <list.h>
#include <pandos_const.h>    // TIMESLICE
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <umps3/umps/libumps.h>

#include "nucleus.h"
#include "process.h"

int debug[10];
void scheduler() {
    while (emptyProcQ(&g_ready_queue)) {
        if (g_process_count == 0) {
            HALT();
        } else if (g_process_count > 0 && g_soft_block_count > 0) {
            // set the status register to enable all interrupts
            // disable the Process Local Timer
            unsigned int old_status = getSTATUS();
            setSTATUS((old_status | STATUS_IEc | STATUS_IM_MASK) & ~STATUS_TE);
            WAIT();
            setSTATUS(old_status);
        } else if (g_process_count > 0 && g_soft_block_count == 0) {
            // deadlock found TODO: how to detech deadlocks?
            debug[3] = 0xFFFFFFFF;
            PANIC();
        }
    }

    g_current_process = removeProcQ(&g_ready_queue);
    debug[0] = g_current_process->p_s.reg_t9;

    setTIMER(TIMESLICE);
    LDST(&g_current_process->p_s);
    // TODO: capisci come deallocare e decrementare quando
    // un processo finisce
}