#include "namespace.h"

// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

/**
 * @brief Namespace table
 * diversi array, uno per tipo di namespace, di NSD
 */
nsd_t type_nsd[MAX_PROC];

/**
 * @brief Lista dei NSD di tipo type liberi o inutilizzati.
 */
struct list_head type_nsFree_h;

/**
 * @brief Lista dei namespace di tipo type attivi.
 */
struct list_head type_nsList_h;

void initNamespaces() {
    // TODO
}

nsd_t *getNamespace(pcb_t *p, int type) {
    // TODO
    return NULL;
}
int addNamespace(pcb_t *p, nsd_t *ns) {
    // TODO
    return 0;
}

nsd_t *allocNamespace(int type) {
    // TODO
    return NULL;
}

void freeNamespace(nsd_t *ns) {
    // TODO
}