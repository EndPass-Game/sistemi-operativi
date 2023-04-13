#include <list.h>
#include <pandos_const.h>    // TIMESLICE
#include <umps3/umps/cp0.h>  // STATUS_IEc, STATUS_TE
#include <umps3/umps/libumps.h>

#include "nucleus.h"
#include "process.h"

int what2 = 0xFF;
void scheduler() {
    // Disabilitiamo gli interrupt perché altrimenti potrei avere
    // un interrupt subito dopo il check ad g_soft_block_count > 0
    // e l'interrupt mi setta dopo questo check che il soft block count è 0
    unsigned int old_status = getSTATUS();
    what2 = emptyProcQ(&g_ready_queue);
    while (emptyProcQ(&g_ready_queue)) {
        setSTATUS(old_status & ~STATUS_IEc);
        if (g_process_count == 0) {
            HALT();
        } else if (g_process_count > 0 && g_soft_block_count > 0) {
            // set the status register to enable all interrupts
            // disable the Process Local Timer
            setSTATUS((old_status | STATUS_IEc | STATUS_IM_MASK) & ~STATUS_TE);
            WAIT();
        } else if (g_process_count > 0 && g_soft_block_count == 0) {
            // deadlock found TODO: how to detech deadlocks?
            PANIC();
        }
    }
    setSTATUS(old_status);

    g_current_process = removeProcQ(&g_ready_queue);

    setTIMER(TIMESLICE);
    LDST(&g_current_process->p_s);
    // TODO: capisci come deallocare e decrementare quando
    // un processo finisce
}