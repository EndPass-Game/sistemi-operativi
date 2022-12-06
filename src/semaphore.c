#include "semaphore.h"

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

semd_t semd_table[MAX_PROC];

/**
 * @brief Lista dei SEMD liberi o inutilizzati.
 */

struct list_head semdFree_h;

/**
 * @brief hash dei semafori attivi
 * (Active Semaphore Hash – ASH)
 */
struct list_head semd_h;

int insertBlocked(int *semAdd, pcb_t *p) {
    // TODO
    return 0;
}

pcb_t *removeBlocked(int *semAdd) {
    // TODO
    return NULL;
}

pcb_t *outBlocked(pcb_t *p) {
    // TODO
    return NULL;
}

pcb_t *headBlocked(int *semAdd) {
    // TODO
    return NULL;
}

void initASH() {
    // TODO
}