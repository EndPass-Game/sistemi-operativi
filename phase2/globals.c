#include <pandos_types.h>

/**
 * @brief : integer indicating the number of started, but not yet
 * terminated processes.
 */
int g_process_count = 0;

/**
 * @brief A process can be either in the “ready,” “running,” or
 * “blocked” (also known as “waiting”) state. This integer is the number
 * of started, but not terminated processes that in are the “blocked” state
 * due to an I/O or timer request.
 */
int g_soft_block_count = 0;

/**
 * @brief Construct a new list head objectTail pointer to a queue of pcbs that are in the “ready”
 * state.
 */
LIST_HEAD(g_ready_queue);

/**
 * @brief Pointer to the pcb that is in the “running” state, i.e.
 * the current executing process.
 */
pcb_t *g_curr_pbc = NULL;