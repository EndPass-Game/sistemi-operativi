#ifndef _PHASE2_GLOBALS_H
#define _PHASE2_GLOBALS_H

#include <list.h>
#include <pandos_types.h>

/**
 * @brief : integer indicating the number of started, but not yet
 * terminated processes.
 */
extern int g_process_count;

/**
 * @brief A process can be either in the “ready,” “running,” or
 * “blocked” (also known as “waiting”) state. This integer is the number
 * of started, but not terminated processes that in are the “blocked” state
 * due to an I/O or timer request.
 */
extern int g_soft_block_count;

/**
 * @brief Construct a new list head objectTail pointer to a queue of pcbs that are in the “ready”
 * state.
 */
extern struct list_head g_ready_queue;

/**
 * @brief Pointer to the pcb that is in the “running” state, i.e.
 * the current executing process.
 */
extern pcb_t *g_curr_pbc;

#endif  // _PHASE2_GLOBALS_H