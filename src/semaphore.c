#include "semaphore.h"

#include "process.h"  // ProcessCtrlBlock

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

Semaphore semd_table[MAX_PROC];

/**
 * @brief Lista dei SEMD liberi o inutilizzati.
 */
struct list_head semdFree_h;

/**
 * @brief hash dei semafori attivi
 * (Active Semaphore Hash – ASH)
 */
struct list_head semd_h;

int insertBlocked(int *semAdd, ProcessCtrlBlock *p) {
    // TODO
    return 0;
}

ProcessCtrlBlock *removeBlocked(int *semAdd) {
    // TODO
    return NULL;
}

ProcessCtrlBlock *outBlocked(ProcessCtrlBlock *p) {
    // TODO
    return NULL;
}

ProcessCtrlBlock *headBlocked(int *semAdd) {
    // TODO
    return NULL;
}

void initASH() {
    // TODO
}