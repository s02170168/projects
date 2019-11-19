#ifndef LIST_H
#define LIST_H

#include "error.h"

typedef enum word_type{
    simple,
    special
} word_type;

typedef char * string;

struct list{
    string * words;
    word_type * types;
    int count;
};
typedef struct list * list;

list ls_make(void);

err_type ls_add(list, string, word_type);

void ls_print(list);

void ls_upgrade(list);

void ls_clear(list);

void ls_delete(list);

#endif