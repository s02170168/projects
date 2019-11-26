#ifndef LIST_H
#define LIST_H

#include "myShellHeaders.h"

/* Private */
typedef enum word_type {
    simple,
    special
} word_type;

typedef char *string;

struct list {
    string *words;
    word_type *types;
    int count;
};
typedef struct list *list;


/* Constructor & Destructor */
list ls_make(void);

void ls_delete(list);


/* Public */
err_type ls_add(list, string, word_type);

err_type ls_argvToWords(list, char **);

void ls_print(list);

void ls_replacePathVars(list);

void ls_clear(list);

#endif