#include <hashtable.h>

#include "process.h"  // container of pcb data
#include "semaphore.h"

/**
 * @brief Semaphore table che contiene tutti i semafori presenti nel programma
 * allocati staticamente.
 */
semd_t semd_table[MAX_PROC];

/**
 * @brief Lista dei SEMD liberi o inutilizzati.
 */
LIST_HEAD(semdFree_h);

/**
 * @brief hash table dei semafori attivi
 * (Active Semaphore Hash – ASH)
 * 2^5 = 32 maggiore di MAX_PROC
 */
DEFINE_HASHTABLE(semd_h, 5);

/**
 * @brief ritorna il semaforo con chiave semAdd nella semd_table
 */
static semd_t *find_sem(int *semAdd);

/**
 * @brief setta il semaforo al valore di default (null)
 */
static void reset(semd_t * sem);

/**
 * @brief alloca un semaforo
 * @return NULL se non riesce ad allocare, altrimenti resetta il semaforo
 */
static semd_t *alloc_semd();

/**
 * @brief dealloca un semaforo solo se la coda
 * dei processi è vuota.
 * @return True se è stato rimosso, altrimenti false
 */
static bool free_semd_ifempty(semd_t *sem);


int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *sem = find_sem(semAdd);
    if (sem == NULL) {
        sem = alloc_semd();
        if (sem == NULL) {
            return true;
        }
        sem->s_key = semAdd;
        hash_add(semd_h, &sem->s_link, (u32) sem->s_key);
    }

    struct list_head *blocked_list = &container_of_pcb_data(p)->sem_block;
    list_add_tail(blocked_list, &sem->s_procq);
    p->p_semAdd = semAdd;
    
    return false;
}

pcb_t *removeBlocked(int *semAdd) {
    semd_t *sem = find_sem(semAdd);
    if (sem == NULL) return NULL;

    if (!list_empty(&sem->s_procq)) {
        struct list_head *next = sem->s_procq.next;

        pcb_t *pcb = &container_of_pcb_sem_block(next)->pcb;
        pcb->p_semAdd = NULL;

        list_del(next);
        free_semd_ifempty(sem);
        return pcb;
    }
    return NULL;
}

pcb_t *outBlocked(pcb_t *p) {
    int *semAdd = p->p_semAdd;
    semd_t *sem = find_sem(semAdd);
    if (sem == NULL) return NULL;

    struct list_head *pos = NULL;
    struct list_head *p_list = NULL; 
    list_for_each(pos, &sem->s_procq) {
        if(&container_of_pcb_sem_block(pos)->pcb == p) {
            p_list = pos; 
        }
    }

    if (p_list != NULL) {
        list_del(p_list);
        free_semd_ifempty(sem);

        pcb_t *pcb = &container_of_pcb_sem_block(p_list)->pcb;
        pcb->p_semAdd = NULL;

        return pcb;
    }

    return NULL;
}

pcb_t *headBlocked(int *semAdd) {
    semd_t *sem = find_sem(semAdd);
    if(sem == NULL || list_empty(&sem->s_procq)){
        return NULL;
    }else{
        return &container_of_pcb_sem_block(sem->s_procq.next)->pcb;
    }
}

void initASH() {
    for (int i = 0; i < MAX_PROC; i++) {
        list_add(&semd_table[i].s_freelink, &semdFree_h);
    }
}

static semd_t *find_sem(int *semAdd) {
    struct hlist_head *sem_hlist = &semd_h[hash_min((u32)semAdd, HASH_BITS(semd_h))];
    if (!hlist_empty(sem_hlist)) {
        // prendiamo il primo elemento dobbiamo controllare 
        // che sia veramente uguale a semAdd, risolve collisioni di hash.
        struct hlist_node *pro = NULL;
        hlist_for_each(pro, sem_hlist) {
            semd_t * sem = container_of(pro, semd_t, s_link);
            if(sem->s_key == semAdd)
                return sem;
        }
    }
    
    return NULL;
}

static void reset(semd_t *sem) {
    sem->s_key = NULL;
    INIT_LIST_HEAD(&sem->s_procq);
    INIT_HLIST_NODE(&sem->s_link);
    INIT_LIST_HEAD(&sem->s_freelink);
}

static semd_t *alloc_semd() {
    if (list_empty(&semdFree_h)) {
        return NULL;
    }

    struct list_head *next = semdFree_h.next;
    list_del(next);

    semd_t *sem = container_of(next, semd_t, s_freelink);
    reset(sem);
    return sem;
}

static bool free_semd_ifempty(semd_t *sem) {
    if (list_empty(&sem->s_procq)) {
        hash_del(&sem->s_link);
        list_add(&sem->s_freelink, &semdFree_h);
        return true;
    }

    return false;
}