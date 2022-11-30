#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include "list.h"
#include "macros.h"

typedef struct pcb_t pcb_t;  // forward declaration
// #include "process.h"

// TODO: sostituisci on struttura corretto
struct hlist_node {
    struct hlist_node *next, **pprev;
};

/**
 * @brief semaphore descriptor data structure
 * 
 */
typedef struct semd_t {
    struct hlist_node s_link;
    struct list_head s_freeLink;

    /* Semaphore key */
    int *s_key;

    /* PCBs blocked on the semaphore */
    struct list_head s_procQ;
} semd_t;

semd_t semd_table[MAX_PROC];

/**
 * @brief Lista dei SEMD liberi o inutilizzati.
 */
list_head semdFree_h; 

/**
 * @brief hash dei semafori attivi
 * (Active Semaphore Hash – ASH)
 */
list_head semd_h; 

/**
 * @brief Viene inserito il PCB puntato da p nella 
 * semAdd. Se il semaforo corrispondente non è
 * presente nella ASH, alloca un nuovo SEMD dalla
 * lista di quelli liberi (semdFree) e lo inserisce nella
 * ASH, settando I campi in maniera opportuna (i.e.
 * key e s_procQ).
 * 
 * @param semAdd chiave della coda dei
 * processi bloccati associata al SEMD
 * @param p PCB da inserire
 * @return int True se non è possibile allocare un
 * nuovo SEMD perché la lista di quelli liberi è vuota, altrimenti FALSE;
 */
int insertBlocked(int *semAdd, pcb_t *p);

/**
 * @brief Rimuove il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD della ASH
 * Se la coda dei processi bloccati per il semaforo * diventa vuota, rimuove il descrittore 
 * corrispondente dalla ASH e lo inserisce nella coda dei * descrittori liberi (semdFree_h).
 * 
 * @param semAdd la chiave della coda dei processi bloccati associata al SEMD della ASH
 * @return pcb_t* il primo PCB dalla coda dei processi bloccati (s_procq) associata al SEMD
 * se esiste, NULL.
 */
pcb_t* removeBlocked(int *semAdd);

/**
 * @brief Rimuove il PCB puntato da p dalla coda del semaforo
 * su cui è bloccato (indicato da p->p_semAdd). 
 * Se la* coda dei processi bloccati per il semaforo diventa
 * vuota, rimuove il descrittore corrispondente dalla ASH e lo inserisce nella coda dei
 * 
 * @param p il PCB da rimuovere
 * @return pcb_t* NULL se p non è presente nella coda dei processi bloccati associata al SEMD, altrimenti p
 */
pcb_t* outBlocked(pcb_t *p);

/**
 * @brief Peek il puntatore al PCB che si trova in testa alla coda (non lo rimuove)
 * 
 * @param semAdd la chiave dei processi bloccati associata al SEMD
 * @return pcb_t* il PCB che si trova in testa alla coda dei processi associata al SEMD, NULL se 
 * non compare nella * ASH oppure se compare ma la sua coda dei processi è vuota.
 */
pcb_t* headBlocked(int *semAdd);

/**
 * @brief Inizializza la lista dei semdFree in
 * modo da contenere tutti gli elementi
 * della semdTable. Questo metodo
 * viene invocato una volta sola durante
 * l’inizializzazione della struttura dati.
 */
void initASH();

#endif /* _SEMAPHORE_H */