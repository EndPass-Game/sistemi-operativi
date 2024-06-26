#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <pandos_types.h>

#include "def-syscall.h"

void syscallHandler();

/**
 * @brief Questa system call crea un nuovo processo
 * come figlio del chiamante. Il primo parametro
 * contiene lo stato che deve avere il processo.
 *
 * @param statep è un puntatore allo stato del processo
 *
 * @param supportp è un puntatore alla struttura di
 * supporto del processo
 *
 * @param Ns descrive il namespace di un determinato tipo
 * da associare al processo, senza specificare il
 * namespace (NULL) verra’ ereditato quello del
 * padre.
 *
 * @return il pid del processo creato (indirizzo di memoria del pcb)
 */
memaddr sysCreateProcess(state_t *statep, support_t *supportp, nsd_t *ns);

/**
 * @brief Quando invocata, la SYS2 termina il processo
 * indicato dal secondo parametro insieme a tutta
 * la sua progenie.
 *
 * Se il secondo parametro e’ 0 il bersaglio e’ il
 * processo invocante.
 */
void sysTerminateProcess(memaddr pid);

/**
 * @brief Operazione di richiesta di un semaforo binario. Il
 * valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L’indirizzo della variabile agisce da identificatore
 * per il semaforo.
 */
void sysPasseren(int *semaddr);

/**
 * @brief Operazione di rilascio su un semaforo binario. Il
 * valore del semaforo è memorizzato nella
 * variabile di tipo intero passata per indirizzo.
 * L’indirizzo della variabile agisce da identificatore
 * per il semaforo.
 */
void sysVerhogen(int *semaddr);

/**
 * @brief – Effettua un’operazione di I/O. CmdValues e’ un vettore di 2
 * interi (per I terminali) o 4 interi (altri device).
 * – La system call carica I registri di device con i valori di
 * CmdValues scrivendo il comando cmdValue nei registri
 * cmdAddr e seguenti, e mette in pausa il processo
 * chiamante fino a quando non si e’ conclusa.
 * – L’operazione è bloccante, quindi il chiamante viene sospeso
 * sino alla conclusione del comando. Il valore ritornato deve
 * essere zero se ha successo, -1 in caso di errore. Il contenuto
 * del registro di status del dispositivo potra’ essere letto nel
 * corrispondente elemento di cmdValues.
 *
 */
int sysDoIO(int *cmdAddr, int *cmdValues);

/**
 * @brief – Quando invocata, la NSYS6 restituisce il tempo
 * di esecuzione (in microsecondi) del processo che
 * l’ha chiamata fino a quel momento.
 *
 * Questa system call implica la registrazione del
 * tempo passato durante l’esecuzione di un
 * processo.
 */
int sysGetTime(void);

/**
 * @brief – Equivalente a una Passeren sul semaforo dell’Interval Timer.
 * – Blocca il processo invocante fino al prossimo tick del
 * dispositivo.
 *
 */
void sysClockWait(void);

/**
 * @brief Restituisce un puntatore alla struttura di supporto del
 * processo corrente, ovvero il campo p_supportStruct del
 * pcb_t.
 *
 */
support_t *sysGetSupportPtr(void);

/**
 * @brief Restituisce l’identificatore del processo invocante se parent
 * == 0, quello del genitore del processo invocante altrimenti.
 * – Se il parent non e’ nello stesso PID namespace del processo
 * figlio, questa funzione ritorna 0 (se richiesto il pid del
 * padre)!
 *
 */
int sysGetProcessID(int parent);

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
int sysGetChildren(int *children, int size);

/**
 * @brief issue the syscall to begin the input output
 * blocks on the semaphore if it's not available
 */
void beginIO(int dev_num, pcb_t *cmdValues);

/**
 * @brief finalize the syscall to end the input output
 * unblocks the semaphore
 */
void endIO(int dev_num);

#endif /* _SYSCALL_H */
