#ifndef _UTILS_H
#define _UTILS_H

// put here the functions you need to use in more than one file,
// mainly used to help you implement other things!

#include <types.h>
#include <list.h>

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

#endif  // _UTILS_H
