#ifndef _NAMESPACE_H
#define _NAMESPACE_H

#include <pandos_types.h>
#include <list.h>

#include "macros.h"

/* namespace descriptor data structure */
typedef nsd_t Namespace;

/**
 * @brief Inizializza tutte le liste dei namespace liberi. Questo metodo viene invocato
 * una volta sola durante l’inizializzazione della struttura dati.
 */
void initNamespaces();

/**
 * @brief Get the Namespace object
 *
 * @param p
 * @param type
 * @return nsd_t* il namespace di tipo type associato al processo p (o NULL).
 */
nsd_t *getNamespace(pcb_t *p, int type);

/**
 * @brief Associa al processo p e a tutti I suoi figli il namespace ns.
 *
 * @param p
 * @param ns
 * @return int Ritorna FALSE in caso di errore, TRUE altrimenti.
 */
int addNamespace(pcb_t *p, nsd_t *ns);

/**
 * @brief Alloca un namespace di tipo type dalla lista corretta.
 *
 * @param type
 * @return nsd_t* il namespace di tipo type allocato (o NULL).
 */
nsd_t *allocNamespace(int type);

/**
 * @brief Libera il namespace ns ri-inserendolo nella lista di namespace corretta.
 *
 * @param ns
 */
void freeNamespace(nsd_t *ns);

#endif  // _NAMESPACE_H