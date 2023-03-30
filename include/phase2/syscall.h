#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <pandos_types.h>

#include "def-syscall.h"

typedef struct context_t {
    unsigned int stackPtr, status, pc;
} context_t;

typedef struct support_t {
    int sup_asid;
    state_t sup_exceptState[2];
    context_t sup_exceptContext[2];
} support_t;

/**
 * @brief Questa system call crea un nuovo processo
 * come figlio del chiamante. Il primo parametro
 * contiene lo stato che deve avere il processo. Se la
 * system call ha successo il valore di ritorno è zero
 * altrimenti è -1.
 *
 * @param supportp è un puntatore alla struttura di
 * supporto del processo
 *
 * @param Ns descrive il namespace di un determinato tipo
 * da associare al processo, senza specificare il
 * namespace (NULL) verra’ ereditato quello del
 * padre.
 *
 * NOTE: Al momento della creazione di un processo
 * e’ necessario creare per questo un id
 * univoco che lo identifichi.
 * L’id puo’ essere (per esempio) un numero
 * progressivo non nullo (diverso da 0, basta che sia unico)
 * oppure l’indirizzo * della struttura pcb_t corrispondente.
 *
 * restituisce il pid del processo creato
 */
int SYSCALL(SYSCALL_CREATEPROCESS, state_t *statep, support_t *supportp, nsd_t *ns);

/**
 * @brief Quando invocata, la SYS2 termina il processo
 * indicato dal secondo parametro insieme a tutta
 * la sua progenie.
 *
 * Se il secondo parametro e’ 0 il bersaglio e’ il
 * processo invocante.
 */
void SYSCALL(SYSCALL_TERMPROCESS, int pid, 0, 0);

/**
 * @brief Operazione di richiesta di un semaforo binario. Il
 * valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L’indirizzo della variabile agisce da identificatore
 * per il semaforo.
 */
void SYSCALL(SYSCALL_PASSEREN, int *semaddr, 0, 0);

/**
 * @brief Operazione di rilascio su un semaforo binario. Il
 * valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L’indirizzo della variabile agisce da identificatore
 * per il semaforo.
 *
 */
void SYSCALL(SYSCALL_VERHOGEN, int *semaddr, 0, 0);

/**
 * @brief – Quando invocata, la NSYS6 restituisce il tempo
 * di esecuzione (in microsecondi) del processo che
 * l’ha chiamata fino a quel momento.
 *
 * Questa System call implica la registrazione del
 * tempo passato durante l’esecuzione di un
 * processo.
 */
int SYSCALL(SYSCALL_GETTIME, 0, 0, 0);

/**
 * @brief – Equivalente a una Passeren sul semaforo dell’Interval Timer.
 * – Blocca il processo invocante fino al prossimo tick del
 * dispositivo.
 *
 */
int SYSCALL(SYSCALL_CLOCKWAIT, 0, 0, 0);

/**
 * @brief Restituisce un puntatore alla struttura di supporto del
 * processo corrente, ovvero il campo p_supportStruct del
 * pcb_t.
 *
 */
support_t *SYSCALL(SYSCALL_GETSUPPORTPTR, 0, 0, 0);

/**
 * @brief Restituisce l’identificatore del processo invocante se parent
 * == 0, quello del genitore del processo invocante altrimenti.
 * – Se il parent non e’ nello stesso PID namespace del processo
 * figlio, questa funzione ritorna 0 (se richiesto il pid del
 * padre)!
 *
 */
int SYSCALL(SYSCALL_GETPROCESSID, int parent, 0, 0);

/**
 * @brief – Un processo che invoca questa system call riceve la lista dei
 * suoi figli dentro all’array children.
 * – Il campo size indica la dimensione massima da caricare
 * dentro l’array children.
 * – La system call ritorna il numero di figli di un processo (che
 * potrebbe non aver caricato in children se troppo piccolo).
 * Se un figlio non e’ nello stesso namespace PID del
 * chiamante, questa funzione non ritorna quel figlio nell’array
 * childrens!
 *
 */
int SYSCALL(SYSCALL_GET_CHILDREN, int *children, int size, 0);

#endif /* _SYSCALL_H */
