#include "namespace.h"
#include "process.h"

/**
 * @brief Namespace table
 * diversi array, uno per tipo di namespace, di NSD
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
        for (int j = 0; j < MAX_PROC; j++) {
            type_nsd[i][j].n_type = i;
            list_add(&type_nsd[i][j].n_link, &type_nsFree_h[i]);
        }
    }
    // for (int i = 0; i < MAX_PROC; i++) {
    //     list_add(&type_nsd[i].n_link, &type_nsFree_h);
    // }
}

nsd_t *getNamespace(pcb_t *p, int type) {
    for (int i = 0; i < NS_TYPE_MAX; i++) {
        if (p->namespaces[i] != NULL && p->namespaces[i]->n_type == type) {
            return p->namespaces[i];
        }
    }
    return NULL;
}

// assioma, un figlio non può avere tra i child un suo padre
int addNamespace(pcb_t *p, nsd_t *ns) {
    int type = ns->n_type;
    p->namespaces[type] = ns;

    struct list_head *pos = NULL;
    list_for_each(pos, &p->p_child) {
        pcb_list_t *pcb = container_of_pcb(pos);
        addNamespace(&pcb->pcb, ns);
    }

    return TRUE;
}

nsd_t *allocNamespace(int type) {
    if (isEmpty(type_nsFree_h)) {
        return NULL;
    }

    struct list_head *next = &type_nsFree_h.next;

    list_del(next);
    list_add(next, &type_nsList_h);
    return container_of(next, nsd_t, n_link);
}

void freeNamespace(nsd_t *ns) {
    list_del(ns);
}