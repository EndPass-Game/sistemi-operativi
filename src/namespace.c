#include "namespace.h"
#include "process.h"
#include "utils.h"  // list_delete_safe

/**
 * @brief Tabella dei namespace, sono diverse colonne ognuna per tipo di namespace (di NSD)
 */
nsd_t type_nsd[NS_TYPE_MAX][MAX_PROC];

/**
 * @brief Lista dei NSD di tipo type liberi o inutilizzati.
 */
struct list_head type_nsFree_h[NS_TYPE_MAX];

/**
 * @brief Lista dei namespace di tipo type attivi.
 */
struct list_head type_nsList_h[NS_TYPE_MAX];

void initNamespaces() {
    for (int i = 0; i < NS_TYPE_MAX; i++) {
        INIT_LIST_HEAD(&type_nsFree_h[i]);
        INIT_LIST_HEAD(&type_nsList_h[i]);

        for (int j = 0; j < MAX_PROC; j++) {
            type_nsd[i][j].n_type = i;
            list_add(&type_nsd[i][j].n_link, &type_nsFree_h[i]);
        }
    }
}

nsd_t *getNamespace(pcb_t *p, int type) {
    // un processo può avere
    // al massimo un namespace per tipo
    if (p != NULL && type >= 0 && type < NS_TYPE_MAX) {
        return p->namespaces[type];
    }

    return NULL;
}

int addNamespace(pcb_t *p, nsd_t *ns) {
    int type = ns->n_type;
    p->namespaces[type] = ns;

    struct list_head *pos = NULL;
    list_for_each(pos, &p->p_child) {
        pcb_t *pcb = container_of(pos, pcb_t, p_sib);
        addNamespace(pcb, ns);
    }

    return TRUE;
}

nsd_t *allocNamespace(int type) {
    if (type < 0 || type >= NS_TYPE_MAX) {
        return NULL;
    }
    
    if(list_empty(&type_nsFree_h[type])) {
        return NULL;
    }

    struct list_head *next = type_nsFree_h[type].next;

    list_del(next);
    list_add(next, &type_nsList_h[type]);
    return container_of(next, nsd_t, n_link);
}

void freeNamespace(nsd_t *ns) {
    list_del(&ns->n_link);
    list_add(&ns->n_link, &type_nsFree_h[ns->n_type]);
}