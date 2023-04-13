#ifndef PANDOS_TYPES_H_INCLUDED
#define PANDOS_TYPES_H_INCLUDED

/****************************************************************************
 *
 * This header file contains utility types definitions.
 *
 ****************************************************************************/

#include <umps3/umps/types.h>
#include "pandos_const.h"
#include <list.h>


/* PID namespace */
#define NS_PID 0
#define NS_TYPE_LAST NS_PID
#define NS_TYPE_MAX (NS_TYPE_LAST + 1)

typedef signed int   cpu_t;
typedef unsigned int memaddr;

typedef struct context_t {
    unsigned int stackPtr, status, pc;
} context_t;

typedef struct support_t {
    int sup_asid;
    state_t sup_exceptState[2];
    context_t sup_exceptContext[2];
} support_t;

typedef struct nsd_t {
    /* Namespace type */
    int n_type;

    /* Namespace list */
    struct list_head n_link;
} nsd_t, *nsd_PTR;

/* process table entry type */
typedef struct pcb_t {
    /* process queue  */
    struct list_head p_list;

    /* process tree fields */
    struct pcb_t    *p_parent; /* ptr to parent	*/
    struct list_head p_child;  /* children list */
    struct list_head p_sib;    /* sibling list  */

    /* process status information */
    state_t p_s;    /* processor state */
    cpu_t   p_time; /* cpu time used by proc */

    /* Pointer to the semaphore the process is currently blocked on */
    int *p_semAdd;

    /* Namespace list */
    nsd_t *namespaces[NS_TYPE_MAX];

    /* support layer information */
    support_t *p_supportStruct;

    int *cmd_addr;  // address of the device register to submit the command
    int *cmd_values;  // values of the device register, 2 for the terminal 4 for other

}   pcb_t, *pcb_PTR;



/* semaphore descriptor (SEMD) data structure */
typedef struct semd_t {
    /* Semaphore key */
    int *s_key;
    /* Queue of PCBs blocked on the semaphore */
    struct list_head s_procq;

    /* Semaphore hash table */
    struct hlist_node s_link;
    /* Free Semaphores list */
    struct list_head s_freelink;
} semd_t, *semd_PTR;

/* Custom types */
typedef unsigned int devreg;

typedef struct termdev {
    devreg status;
    devreg command;
} termdev_t;


typedef struct sysiostate {
    int sem_mut;  // semaphore for mutual exclusion
    int sem_sync;  // semaphore for synchronization
    pcb_t* waiting_process;
} sysiostate_t;


#endif
