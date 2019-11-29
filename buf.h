#ifndef BUF_H
#define BUF_H

#include "myShellHeaders.h"
#include "list.h"

/* Private */
typedef enum state {
    GOT_SIMPLE,
    GOT_SPECIAL,
    GOT_SPACE,
    GOT_SHARP,
    GOT_QUOTE,
    GOT_DOUBLE_QUOTE,
    GOT_BACK_SIMPLE,
    GOT_BACK_DOUBLE_QUOTE
} state;

struct buf {
    string word;
    int length;
    state st;
};
typedef struct buf *buf;

void buf_clear(buf, state);

err_type buf_addSym(buf, char, state);

err_type buf_addToList(buf, list, word_type);


/* Constructor & Destructor */
buf buf_make(void);

void buf_delete(buf);


/* Public */
err_type buf_getSym(buf, list, char, err_type);

#endif