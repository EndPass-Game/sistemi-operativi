#ifndef _UTILS_H
#define _UTILS_H

// put here the functions you need to use in more than one file,
// mainly used to help you implement other things!

#include <list.h>
#include <types.h>

/**
 * @brief sets size bytes of ptr to byte
 */
void memset(void *ptr, char byte, u32 size);

/**
 * @brief deleta `list` dalla lista doppiamente concatenata e
 * la reinizializza a un valore valido, senza aver così problemi
 * di doppio delete.
 */

static inline void list_delete_safe(struct list_head *list) {
    list_del(list);
    INIT_LIST_HEAD(list);
}

/**
 * @brief garantisce che la lista `new` non sia concatenata in nessuna
 * altra lista prima di essere aggiunta in coda
 */
static inline void list_add_tail_safe(struct list_head *new, struct list_head *lista) {
    list_delete_safe(new);
    list_add_tail(new, lista);
}

/**
 * @brief garantisce che la lista `new` non sia concatenata in nessuna
 * altra lista prima di essere aggiunta in lista
 */
static inline void list_add_safe(struct list_head *new, struct list_head *list) {
    list_delete_safe(new);
    list_add(new, list);
}


/**
 * @brief Due pointer byte per byte di lungezza size
 */
void memcpy(void *ptr, void* ptr2, u32 size) ;
#endif  // _UTILS_H
