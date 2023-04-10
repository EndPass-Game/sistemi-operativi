#include <pandos_types.h>

/**
 * @brief : integer indicating the number of started, but not yet
 * terminated processes.
 */
int g_process_count;

/**
 * @brief A process can be either in the “ready,” “running,” or
 * “blocked” (also known as “waiting”) state. This integer is the number
 * of started, but not terminated processes that in are the “blocked” state
 * due to an I/O or timer request.
 */
int g_soft_block_count;

/**
 * @brief Construct a new list head objectTail pointer to a queue of pcbs that are in the “ready”
 * state.
 */
struct list_head g_ready_queue;

/**
 * @brief Pointer to the pcb that is in the “running” state, i.e.
 * the current executing process.
 */
pcb_t *g_curr_pcb;

/**
 * @brief device semaphores
 * TODO: how many device semaphores are there?
 */
int g_dev_sem[64];

void initKernelGlobals() {
    g_process_count = 0;
    g_soft_block_count = 0;
    INIT_LIST_HEAD(&g_ready_queue);
    g_curr_pcb = NULL;
    for (int i = 0; i < 64; i++) {
        g_dev_sem[i] = 1;
    }
}