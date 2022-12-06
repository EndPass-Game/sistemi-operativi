#include "process.h"

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

struct list_head pcbFree_h;
pcb_t pcbFree_table[MAX_PROC];

void initPcbs() {
    // TODO
}

void freePcb(pcb_t *p) {
    // TODO
}

pcb_t *allocPcb() {
    // TODO
    return NULL;
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