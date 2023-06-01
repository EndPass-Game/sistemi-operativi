#include <list.h>
#include <pandos_const.h>    // TIMESLICE
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <umps3/umps/libumps.h>

#include "nucleus.h"
#include "process.h"

unsigned int scheduler_old_status;
void scheduler() {
    scheduler_old_status = getSTATUS();

    while (emptyProcQ(&g_ready_queue)) {
        if (g_process_count == 0) {
            HALT();
        } else if (g_process_count > 0 && g_soft_block_count > 0) {
            // set the status register to enable all interrupts
            // disable the Process Local Timer
            setSTATUS((scheduler_old_status | STATUS_IEc | STATUS_IM_MASK) & ~STATUS_TE);
            // ci può essere un interrupt subito dopo setStatus che mi mette un processo
            // se succede non vogliamo aspettare, se succede appena dopo faccio
            // il check amen, si aspettera il clock timer interrupt
            if (!emptyProcQ(&g_ready_queue)) break;
            WAIT();
            setSTATUS(scheduler_old_status);
        } else if (g_process_count > 0 && g_soft_block_count == 0) {
            PANIC();
        }
    }
    setSTATUS(scheduler_old_status);

    g_current_process = removeProcQ(&g_ready_queue);

    setTIMER(TIMESLICE);
    LDST(&g_current_process->p_s);
}