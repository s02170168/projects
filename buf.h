#ifndef BUF_H
#define BUF_H

#include "list.h"
#include "error.h"

typedef enum state{
    GOT_SIMPLE,
    GOT_SPECIAL,
    GOT_SPACE,
    GOT_SHARP,
    GOT_QUOTE,
    GOT_BACKSLASH
} state;

struct buf{
    string word;
    int length;
    state st;
};
typedef struct buf * buf;

buf buf_make(void);

err_type buf_get_sym(buf, list, char, err_type);

void buf_delete(buf);

#endif