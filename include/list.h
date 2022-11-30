#ifndef _LIST_H
#define _LIST_H

typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_head;
// TODO: implement other list functions

#endif /* _LIST_H */