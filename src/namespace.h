#pragma once

#include "list.h"
#include "macros.h"
#include "process.h"

/* namespace descriptor data structure */
typedef struct nsd_t {
    int n_type;
    struct list_head n_link;
} nsd_t;

/**
 * @brief Namespace table
 * diversi array, uno per tipo di namespace, di NSD
 */
nsd_t type_nsd[MAX_PROC]; 

/**
 * @brief Lista dei NSD di tipo type liberi o inutilizzati.
 */
list_head type_nsFree_h; 

/**
 * @brief Lista dei namespace di tipo type attivi.
 */
list_head type_nsList_h;



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

// Alloca un namespace di tipo type dalla lista corretta.

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