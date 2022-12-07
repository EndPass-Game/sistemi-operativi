#include "process.h"

#include <container_of.h>

#include "macros.h"

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

LIST_HEAD(pcbFree_h);
ProcessBlockList pcbFree_table[MAX_PROC];

// TODO: decidere se tutte queste funzioni hanno bisogno di critical section o meno.

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

/**
 * @brief Get the Process Block List object corrisponente
 * al pcb_t puntato da p,
 * @return NULL se non esiste tale puntatore, altrimenti
 * il puntatore alla ProcessBlockList corrispondente
 */
static ProcessBlockList *getProcessBlockList(pcb_t *p);

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
    struct list_head *p_list = NULL;

    // find the pcb in the pcbFree_table
    for (int i = 0; i < MAX_PROC; i++) {
        if (&pcbFree_table[i].pcb == p) {
            p_list = &pcbFree_table[i].list;
            break;
        }
    }

    if (list_empty(p_list)) {
        // TODO: panic message
    }

    list_add(p_list, &pcbFree_h);
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
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head) {
    return list_empty(head);
}

void insertProcQ(struct list_head *head, ProcessCtrlBlock *p) {
    ProcessBlockList *processBlockList = getProcessBlockList(p);
    if (processBlockList != NULL) {
        list_add_tail(&processBlockList->list, head);
    }
}

pcb_t *headProcQ(struct list_head *head) {
    if (head == NULL || emptyProcQ(head)) {
        return NULL;
    } else {
        struct list_head *next = head->next;
        ProcessBlockList *processBlockList = container_of(next, ProcessBlockList, list);
        return &processBlockList->pcb;
    }
}

pcb_t *removeProcQ(struct list_head *head) {
    if (emptyProcQ(head))
        return NULL;

    struct list_head *next = head->next;
    list_del(next);

    return &container_of(next, ProcessBlockList, list)->pcb;
}

pcb_t *outProcQ(struct list_head *head, ProcessCtrlBlock *p) {
    ProcessBlockList *processBlockList = getProcessBlockList(p);

    struct list_head *pos = NULL;
    list_for_each(pos, head) {
        if (pos == &processBlockList->list) {
            list_del(pos);
            return p;
        }
    }

    return NULL;
}

int emptyChild(ProcessCtrlBlock *p) {
    if (p == NULL) {
        return 1;  // un puntatore invalido non ha figli
    }

    return list_empty(&p->p_child);
}

void insertChild(ProcessCtrlBlock *parent, ProcessCtrlBlock *p) {
    ProcessBlockList *parent_list = getProcessBlockList(parent);
    ProcessBlockList *p_list = getProcessBlockList(p);

    if (parent_list == NULL || p_list == NULL) return;
    p->p_parent = parent;
    list_add(&p_list->list, &parent_list->pcb.p_child);
}

pcb_t *removeChild(ProcessCtrlBlock *p) {
    if (emptyChild(p)) return NULL;

    struct list_head *first_children_list = p->p_child.next;
    ProcessBlockList *first_child = container_of(first_children_list, ProcessBlockList, list);
    list_del(first_children_list);
    return &first_child->pcb;
}

pcb_t *outChild(ProcessCtrlBlock *child_pcb) {
    ProcessCtrlBlock *parent = child_pcb->p_parent;
    if (parent == NULL) return NULL;

    // assumiamo che child_list sia correttamente linkata nella lista del
    // genitore
    ProcessBlockList *child_list = getProcessBlockList(child_pcb);
    list_del(&child_list->list);

    return child_pcb;
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

static ProcessBlockList *getProcessBlockList(pcb_t *p) {
    for (int i = 0; i < MAX_PROC; i++) {
        if (&pcbFree_table[i].pcb == p) {
            return &pcbFree_table[i];
        }
    }

    return NULL;
}
