#ifndef _NUCLEUS_H_
#define _NUCLEUS_H_
#include <list.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include <process.h>

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
extern pcb_t *g_current_process;

/**
 * @brief device semaphores
 *
 * TODO: how many device semaphores are there?
 */
extern int g_device_semaphores[DEVICE_NUMBER];  // TODO: vedere quanti device semaphores ci sono

/**
 * @brief Pseudo clock semaphore for interval timer
 */
extern int g_pseudo_clock;

/**
 * @brief global that stores the global pseudo clock at the start
 * of a process.
 *
 */
extern unsigned int g_tod;

/**
 * @brief alias for casting BIOS_DATA_PAGE_BASE
 *
 */
extern state_t *g_old_state;

extern int g_debug[20];
#endif  // _NUCLEUS_H_
