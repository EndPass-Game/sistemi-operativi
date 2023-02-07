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
pcb_list_t pcbFree_table[MAX_PROC];

/**
 * @brief Resettiamo il Process Control Block(pcb), settando tutti i
 * campi a 0.
 */
static void reset(pcb_list_t* pcb);

void initPcbs() {
    for (int i = 0; i < MAX_PROC; i++) {
        list_add(&pcbFree_table[i].list, &pcbFree_h);
    }
}

void freePcb(pcb_t *p) {
    // NOTA: il reset è fatto in fase di allocazione
    pcb_list_t *pcb_list = container_of_pcb_data(p);
    list_add(&pcb_list->list, &pcbFree_h);
}

pcb_t *allocPcb() {
    if (list_empty(&pcbFree_h))
        return NULL;

    struct list_head *next = pcbFree_h.next;
    list_del(next);

    pcb_list_t *p_list = container_of_pcb(next);
    reset(p_list);
    return &p_list->pcb;
}

void mkEmptyProcQ(struct list_head *head) {
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head) {
    return list_empty(head);
}

void insertProcQ(struct list_head *head, pcb_t *p) {
    pcb_list_t *list = container_of_pcb_data(p);
    // aggiungiamo in coda così da implementare una queue
    list_add_tail(&list->list, head);
}

pcb_t *headProcQ(struct list_head *head) {
    if (emptyProcQ(head)) return NULL;
    
    return &container_of_pcb(head->next)->pcb;
}

pcb_t *removeProcQ(struct list_head *head) {
    if(list_empty(head)) {
        return NULL;
    }
    
    pcb_list_t *next_elem = container_of_pcb(head->next);
    list_del(&next_elem->list);
    return &next_elem->pcb;
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
    bool present = false;

    struct list_head *pos = NULL;
    list_for_each(pos, head) {
        if (p == &container_of_pcb(pos)->pcb) {
            present = true;
            break;
        }
    }

    if (present) {
        pcb_list_t *p_list = container_of_pcb_data(p);
        list_del(&p_list->list);
        return &p_list->pcb;
    }

    return NULL;
}

int emptyChild(pcb_t *p) {
    return list_empty(&p->p_child);
}

void insertChild(pcb_t *parent, pcb_t *child) {
    pcb_list_t *to_add_list = container_of_pcb_data(child);
    child->p_parent = parent;
    list_add_tail(&to_add_list->list, &parent->p_child);
}

pcb_t *removeChild(pcb_t *parent) {
    if (list_empty(&parent->p_child)) {
        return NULL;
    }
    
    pcb_list_t *child_list = container_of_pcb(parent->p_child.next);
    list_del(&child_list->list);
    return &child_list->pcb;
}

pcb_t *outChild(pcb_t *child) {
    if (child->p_parent == NULL) {
        return NULL;
    }

    pcb_list_t *parent = container_of_pcb_data(child->p_parent);
    struct list_head *child_head = &parent->pcb.p_child;

    struct list_head *pos = NULL;
    struct list_head *p_list = NULL;
    list_for_each(pos, child_head) {
        if (child == &container_of_pcb(pos)->pcb) {
            p_list = pos;
            break;
        }
    }

    if (p_list != NULL) {
        list_del(p_list);
        return child;
    }

    return NULL;
}

static void reset(pcb_list_t* pcb_list) {
    memset(pcb_list, 0, sizeof(pcb_list_t));
    INIT_LIST_HEAD(&pcb_list->list);

    pcb_t *pcb = &pcb_list->pcb;
    
    INIT_LIST_HEAD(&pcb->p_child);
    INIT_LIST_HEAD(&pcb->p_list);
    INIT_LIST_HEAD(&pcb->p_sib);
}