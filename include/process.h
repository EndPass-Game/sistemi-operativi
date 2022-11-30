#ifndef _PROCESS_H
#define _PROCESS_H

#include "list.h"
#include "macros.h"

typedef struct nsd_t nsd_t;  // forward declaration, solves circular dependency
// #include "namespace.h"

// TODO: sostituire questi seguenti typedef con le cose adeguate
typedef int state_t;
typedef int cpu_t;

/**
 * @brief Struttura dati che rappresenta un processo.
 * PCB = Process Control Block
 */
typedef struct pcb_t {
    /* process queue */
    struct list_head p_next;

    /* process tree fields */
    struct pcb_t *p_parent;    // pointer to parent process
    struct list_head p_child;  // list of children
    struct list_head p_sib;    // sibling list

    /* process status information */
    state_t p_s;                     // processor state
    cpu_t p_time;                    // cpu time used by proc
    int *p_semAdd;                   // ptr to semaphore on which proc is blocked
    nsd_t *p_namespaces[MAX_TYPES];  // ACTIVE namespace for each type
} pcb_t, *pcb_PTR;

list_head pcbFree_h;
pcb_t pcbFree_table[MAX_PROC];

/**
 * @brief Inizializza la lista pcbFree in modo da contenere tutti gli elementi della
 * pcbFree_table. Questo metodo deve  essere chiamato UNA SOLA sola in fase di
 * inizializzazione della struttura dati.
 */
void initPcbs(void);

/**
 * @brief Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)
 *
 * @param p
 */
void freePcb(pcb_t *p);

/**
 * @brief Rimuove un elemento dalla pcbFree, inizializza tutti i campi
 * (NULL/0) e restituisce l’elemento rimosso.
 *
 * @return pcb_t* NULL se la pcbFree_h è vuota. altrimenti l'elemento rimosso.
 */
pcb_t *allocPcb();

// LISTA DEI PCB

/**
 * @brief Crea una lista di PCB, inizializzandola come lista vuota
 *
 * @param head
 */
void mkEmptyProcQ(struct list_head *head);

/**
 * @param head
 * @return int Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti
 */
int emptyProcQ(struct list_head *head);

/**
 * @brief Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
 *
 * @param head
 * @param p
 */
void insertProcQ(struct list_head *head, pcb_t *p);

/**
 * @brief Peek alla testa della lista, SENZA MODIFICARLA
 *
 * @param head
 * @return pcb_t la testa della coda dei processi da head, NULL se la coda non ha elementi.
 */
pcb_t headProcQ(struct list_head *head);

/**
 * @brief Rimuove il primo elemento dalla coda dei processi puntata da head. e lo restituisce.
 *
 * @param head
 * @return pcb_t* NULL se la coda è vuota. Altrimenti ritorna il puntatore all’elemento rimosso dalla lista.
 */
pcb_t *removeProcQ(struct list_head *head);

/**
 * @brief Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non è presente
 *
 * @param head
 * @param p una posizione arbitraria all'interno della coda
 * @return pcb_t* NULL se p non è presente, altrimenti il puntatore a p
 */
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/**
 * @brief funzione utilizzata per fare un check sull'esistenza dei figli
 *
 * @param p
 * @return int Restituisce TRUE se il PCB puntato da p non ha figli, FALSE altrimenti.
 */
int emptyChild(pcb_t *p);

/**
 * @brief Inserisce il PCB puntato da p come figlio del PCB puntato da prnt.
 *
 * @param prnt
 * @param p
 */
void insertChild(pcb_t *prnt, pcb_t *p);

/**
 * @brief Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL.
 *
 * @param p
 * @return pcb_t* NULL se p non ha figli, altrimenti il puntatore al primo figlio di p
 */
pcb_t *removeChild(pcb_t *p);

/**
 * @brief Rimuove il PCB puntato da p dalla lista dei figli del padre.
 * A differenza della removeChild, p può trovarsi in una posizione arbitraria
 * (ossia non è necessariamente il primo figlio del padre).
 *
 * @param p
 * @return pcb_t* NULL se p non ha un padre, altrimenti l'elemento rimosso (cioè p)
 */
pcb_t *outChild(pcb_t *p);

#endif /* _PROCESS_H */
