#ifndef _PROCESS_H
#define _PROCESS_H

#include <list.h>
#include <pandos_types.h>

#include "macros.h"

/**
 * @brief Inizializza la lista pcbFree in modo da contenere tutti gli elementi della
 * pcbFree_table. Questo metodo deve  essere chiamato UNA SOLA sola in fase di
 * inizializzazione della struttura dati.
 */
void initPcbs(void);

/**
 * @brief Inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree_h)
 * Si assume che il pointer p non sia già libero
 */
void freePcb(pcb_t *p);

/**
 * @brief Rimuove un elemento dalla pcbFree, inizializza tutti i campi
 * (NULL/0) e le liste a vuote (punta a se stesso) e restituisce l’elemento rimosso
 * dalla lista dei pcb liberi.
 *
 * @return NULL se la pcbFree_h è vuota. altrimenti l'elemento rimosso.
 */
pcb_t *allocPcb();

// LISTA DEI PCB

/**
 * @brief Crea una lista di PCB, inizializzandola come lista vuota
 *
 * @param head elemento sentinella della lista
 */
void mkEmptyProcQ(struct list_head *head);

/**
 * @param head elemento sentinella della lista
 * @return int Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti
 */
int emptyProcQ(struct list_head *head);

/**
 * @brief Inserisce l’elemento puntato da p nella coda dei processi puntata da head.
 *
 * @param head elemento sentinella della lista
 * @param p pcb da inserire
 */
void insertProcQ(struct list_head *head, pcb_t *p);

/**
 * @brief Peek alla testa della lista, SENZA MODIFICARLA
 *
 * @param head elemento sentinella della lista
 * @return pcb_t *la testa della coda dei processi da head, NULL se la coda non ha elementi.
 */
pcb_t *headProcQ(struct list_head *head);

/**
 * @brief Rimuove il primo elemento dalla coda dei processi puntata da head. e lo restituisce.
 *
 * @param head elemento sentinella della lista
 * @return pcb_t* NULL se la coda è vuota. Altrimenti ritorna il puntatore all’elemento rimosso dalla lista.
 */
pcb_t *removeProcQ(struct list_head *head);

/**
 * @brief Rimuove il PCB puntato da p dalla coda dei processi puntata da head.
 *
 * @param head elemento sentinella della lista
 * @param p una posizione arbitraria all'interno della coda
 * @return pcb_t* NULL se p non è presente, altrimenti il puntatore a p
 */
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

int getQueueLen(struct list_head *head);

/**
 * @brief funzione utilizzata per fare un check sull'esistenza dei figli
 *
 * @param p
 * @return int Restituisce TRUE se il PCB puntato da p non ha figli, FALSE altrimenti.
 */
int emptyChild(pcb_t *p);

/**
 * @brief Inserisce il PCB puntato da child come figlio del PCB puntato da parent.
 * Si assume che child sia stato correttamente allocato e che non abbia un padre.
 */
void insertChild(pcb_t *parent, pcb_t *child);

/**
 * @brief Rimuove il primo figlio del PCB puntato da parent. Se parent non ha figli, restituisce NULL.
 *
 * @param parent
 * @return pcb_t* NULL se parent non ha figli, altrimenti il puntatore al primo figlio di parent
 */
pcb_t *removeChild(pcb_t *parent);

/**
 * @brief Rimuove il PCB puntato da child dalla lista dei figli del padre.
 * A differenza della removeChild, child può trovarsi in una posizione arbitraria
 * (ossia non è necessariamente il primo figlio del padre).
 *
 * @param child
 * @return pcb_t* NULL se child non ha un padre, altrimenti l'elemento rimosso (cioè child)
 */
pcb_t *outChild(pcb_t *child);

#endif /* _PROCESS_H */
