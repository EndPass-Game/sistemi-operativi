#pragma once


typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_head;
// TODO: implement other list functions