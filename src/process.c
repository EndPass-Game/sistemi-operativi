#include "process.h"

#include <container_of.h>

#include "macros.h"

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

LIST_HEAD(pcbFree_h);
ProcessBlockList pcbFree_table[MAX_PROC];

// NOTE: these two functions could be done by a memset,
// se we could only implement memset and use it here
/**
 * @brief zero set a pcb
 */
static void reset(ProcessCtrlBlock *p);

/**
 * @brief zero set a state_t
 */
static void resetProcessorState(state_t *s);

void initPcbs(void) {
    for (int i = 0; i < MAX_PROC; i++) {
        reset(&pcbFree_table[i].pcb);
        list_add(&pcbFree_table[i].list, &pcbFree_h);
    }
}

void freePcb(pcb_t *p) {
    // NOTA: si dovrebbe fare un check per vedere se p è già nella lista?
    // probabilmente sarebbe meglio, TODO: decidere se è necessario

    reset(p);
    LIST_HEAD(p_list);

    // find the pcb in the pcbFree_table
    for (int i = 0; i < MAX_PROC; i++) {
        if (&pcbFree_table[i].pcb == p) {
            p_list = pcbFree_table[i].list;
            break;
        }
    }

    if (list_empty(&p_list)) {
        // TODO: panic message
    }

    list_add(&p_list, &pcbFree_h);
}

pcb_t *allocPcb() {
    if (list_empty(&pcbFree_h)) {
        return NULL;
    } else {
        struct list_head *next = pcbFree_h.next;
        ProcessBlockList *processBlocklist = container_of(next, ProcessBlockList, list);
        list_del(next);

        return &processBlocklist->pcb;
    }
}

void mkEmptyProcQ(struct list_head *head) {
    // TODO
}

int emptyProcQ(struct list_head *head) {
    // TODO
    return 0;
}

void insertProcQ(struct list_head *head, pcb_t *p) {
    // TODO
}

pcb_t *headProcQ(struct list_head *head) {
    // TODO
    return NULL;
}

pcb_t *removeProcQ(struct list_head *head) {
    // TODO
    return NULL;
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
    // TODO
    return NULL;
}

int emptyChild(pcb_t *p) {
    // TODO
    return 0;
}

void insertChild(pcb_t *prnt, pcb_t *p) {
    // TODO
}

pcb_t *removeChild(pcb_t *p) {
    // TODO
    return NULL;
}

pcb_t *outChild(pcb_t *p) {
    // TODO
    return NULL;
}

static void reset(ProcessCtrlBlock *p) {
    INIT_LIST_HEAD(&p->p_list);

    p->p_parent = NULL;

    INIT_LIST_HEAD(&p->p_child);
    INIT_LIST_HEAD(&p->p_sib);

    resetProcessorState(&p->p_s);
    p->p_time = 0;
    p->p_semAdd = NULL;

    for (int i = 0; i < MAX_TYPES; i++) {
        p->namespaces[i] = NULL;
    }
}

static void resetProcessorState(state_t *s) {
    s->entry_hi = 0;
    s->cause = 0;
    s->status = 0;
    s->pc_epc = 0;
    s->hi = 0;
    s->lo = 0;
    for (int i = 0; i < STATE_GPR_LEN; i++) {
        s->gpr[i] = 0;
    }
}