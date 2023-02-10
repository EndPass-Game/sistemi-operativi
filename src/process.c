#include "process.h"
#include "utils.h"

/**
 * @brief Lista dei PCB liberi o inutilizzati.
 */
LIST_HEAD(pcbFree_h);

/**
 * @brief PCB table che contiene tutti i PCB presenti nel programma
 * allocati staticamente.
 */
pcb_t pcbFree_table[MAX_PROC];

/**
 * @brief Resettiamo il Process Control Block(pcb), settando tutti i
 * campi a 0. (oppure a lista vuota (che punta sé stessi) se il campo è una lista)
 */
static void reset(pcb_t* pcb);

void initPcbs() {
    for (int i = 0; i < MAX_PROC; i++) {
        list_add(&pcbFree_table[i].p_list, &pcbFree_h);
    }
}

void freePcb(pcb_t *p) {
    // rimuovi dalle altre code se p_list ci è
    // ancora inserito
    if (!list_empty(&p->p_list)) {
        list_delete_safe(&p->p_list);
    }

    list_add(&p->p_list, &pcbFree_h);
}

pcb_t *allocPcb() {
    if (list_empty(&pcbFree_h)) {
        return NULL;
    }

    struct list_head *next = pcbFree_h.next;
    list_delete_safe(next);
    
    pcb_t *pcb = container_of(next, pcb_t, p_list);
    reset(pcb);
    return pcb;
}

void mkEmptyProcQ(struct list_head *head) {
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head) {
    return list_empty(head);
}

void insertProcQ(struct list_head *head, pcb_t *p) {
    // aggiungiamo in coda così da implementare una queue
    list_add_tail(&p->p_list, head);
}

pcb_t *headProcQ(struct list_head *head) {
    if (emptyProcQ(head)) return NULL;
    
    return container_of(head->next, pcb_t, p_list);
}

pcb_t *removeProcQ(struct list_head *head) {
    if (list_empty(head)) {
        return NULL;
    }
    
    pcb_t *next_elem = container_of(head->next, pcb_t, p_list);
    list_delete_safe(&next_elem->p_list);
    return next_elem;
}

pcb_t *outProcQ(struct list_head *head, pcb_t *process_block) {
    bool present = false;

    struct list_head *pos = NULL;
    list_for_each(pos, head) {
        if (&process_block->p_list == pos) {
            present = true;
            break;
        }
    }

    if (present) {
        list_delete_safe(&process_block->p_list);
        return process_block;
    }

    return NULL;
}

int emptyChild(pcb_t *p) {
    return list_empty(&p->p_child);
}

void insertChild(pcb_t *parent, pcb_t *child) {
    struct list_head *sib_head = &child->p_sib;
    child->p_parent = parent;
    list_add_tail(sib_head, &parent->p_child);
}

pcb_t *removeChild(pcb_t *parent) {
    if (list_empty(&parent->p_child)) {
        return NULL;
    }
    
    pcb_t *child_pcb = container_of(parent->p_child.next, pcb_t, p_sib);
    list_delete_safe(&child_pcb->p_sib);
    return child_pcb;
}

pcb_t *outChild(pcb_t *child) {
    if (child->p_parent == NULL) {
        return NULL;
    }

    pcb_t *parent = child->p_parent;
    struct list_head *child_head = &parent->p_child;

    struct list_head *pos = NULL;
    struct list_head *p_list = NULL;
    list_for_each(pos, child_head) {
        if (&child->p_sib == pos) {
            p_list = pos;
            break;
        }
    }

    if (p_list != NULL) {
        list_delete_safe(p_list);
        return child;
    }

    return NULL;
}

static void reset(pcb_t* pcb) {
    memset(pcb, 0, sizeof(pcb_t));

    INIT_LIST_HEAD(&pcb->p_list);
    INIT_LIST_HEAD(&pcb->p_child);
    INIT_LIST_HEAD(&pcb->p_sib);
}