#include "namespace.h"

#include "process.h"  // ProcessBlockList
// TODO: queste variabili globali non possono essere definite in un file
// header, scoprire il motivo.

/**
 * @brief Namespace table
 * diversi array, uno per tipo di namespace, di NSD
 */
NamespaceList type_nsd[MAX_PROC];

/**
 * @brief Lista dei NSD di tipo type liberi o inutilizzati.
 */
LIST_HEAD(namespace_free_list);

/**
 * @brief Lista dei namespace di tipo type attivi.
 */
struct list_head active_type_nsd[MAX_TYPES];

/**
 * @brief set to default the given namespace
 */
static void reset(Namespace *namespace);

void initNamespaces() {
    for (int i = 0; i < MAX_PROC; i++) {
        reset(&type_nsd[i]);
        list_add(&type_nsd->list, &namespace_free_list);
    }

    for (int i = 0; i < MAX_TYPES; i++) {
        INIT_LIST_HEAD(active_type_nsd);
    }
}

nsd_t *getNamespace(pcb_t *p, int type) {
    struct list_head *curr = &active_type_nsd[type];

    struct list_head *pos = NULL;
    list_for_each(pos, curr) {
        ProcessBlockList *pcb_list = container_of(p, ProcessBlockList, list);
        if (pos == &pcb_list->list)
    }

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

static void reset(Namespace *namespace) {
    namespace->n_type = 0;
    INIT_LIST_HEAD(&namespace->n_link);
}