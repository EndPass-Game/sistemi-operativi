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
LIST_HEAD(active_type_ns);

/**
 * @brief set to default the given namespace
 */
static void reset(Namespace *namespace);

void initNamespaces() {
    for (int i = 0; i < MAX_PROC; i++) {
        reset(&type_nsd[i]);
        list_add(&type_nsd->list, &namespace_free_list);
    }

}

nsd_t *getNamespace(pcb_t *p, int type) {
    Namespace *namespace = NULL;
    for (int i = 0; i < NS_TYPE_MAX; i++) {
        for (int j = 0; j < MAX_PROC; j++) {
            if (p->namespaces[i] == &type_nsd[j].list && 
            type == type_nsd[j].namespace->n_type) {
                namespace = type_nsd[j].namespace;
                break;
            }
        }

        if (namespace != NULL) break;
    }

    return namespace;
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