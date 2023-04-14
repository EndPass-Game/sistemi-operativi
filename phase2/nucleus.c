#include "nucleus.h"

#include <list.h>
#include <pandos_const.h>  // KERNELSTACK, RAMTOP
#include <pandos_types.h>
#include <umps3/umps/cp0.h>      // STATUS_*
#include <umps3/umps/libumps.h>  // setSTATUS, getStatus

#include "exceptions.h"
#include "namespace.h"
#include "process.h"
#include "scheduler.h"
#include "semaphore.h"
#include "umps/bios_defs.h"

/**
 *  @brief  We define the global variables here,
 * that we have previsly declarate,
 * and export it in nucleus.h
 */
int g_process_count;
int g_soft_block_count;
struct list_head g_ready_queue;
pcb_t *g_current_process;
sysiostate_t g_sysiostates[DEVICE_NUMBER];
int g_pseudo_clock;
int g_debug[20];
unsigned int g_tod;
state_t *const g_old_state = (state_t *) BIOS_DATA_PAGE_BASE;

extern void test();
extern void uTLB_RefillHandler();

static void initGlobalVariable();

static void launchInit();
static void initSysIOState(int dev_num);

int main() {
    initGlobalVariable();
    initPcbs();
    initASH();
    initNamespaces();

    // Load system interval timer to 100ms
    LDIT(PSECOND);
    launchInit();

    scheduler();

    return 0;
}

static void launchInit() {
    g_process_count++;
    pcb_t *pcb = allocPcb();

    pcb->p_s.status |= STATUS_IEp | STATUS_TE | STATUS_IM_MASK;
    pcb->p_s.pc_epc = (memaddr) test;
    pcb->p_s.reg_t9 = pcb->p_s.pc_epc;

    RAMTOP(pcb->p_s.reg_sp);

    insertProcQ(&g_ready_queue, pcb);
}

static void initGlobalVariable() {
    g_process_count = 0;
    g_soft_block_count = 0;
    mkEmptyProcQ(&g_ready_queue);
    g_current_process = NULL;

    for (int i = 0; i < DEVICE_NUMBER; i++) {
        initSysIOState(i);
    }
    g_pseudo_clock = 1;
    g_tod = 0;

    passupvector_t *passupvector = (passupvector_t *) PASSUPVECTOR;
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    passupvector->tlb_refill_stackPtr = (memaddr) KERNELSTACK;

    passupvector->exception_handler = (memaddr) exceptionHandler;
    passupvector->exception_stackPtr = (memaddr) KERNELSTACK;
}

static void initSysIOState(int dev_num) {
    g_sysiostates[dev_num].sem_mut = 1;
    g_sysiostates[dev_num].sem_sync = 0;
    g_sysiostates[dev_num].waiting_process = NULL;
    // g_sysiostates[dev_num].cmd_addr = NULL;
    // g_sysiostates[dev_num].cmd_values = NULL;
}
