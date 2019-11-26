<<<<<<< HEAD
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
    GOT_BACK_SIMPLE,
    GOT_BACK_QUOTE
} state;

struct buf {
    string word;
    int length;
    state st;
};
typedef struct buf *buf;

void buf_clear(buf, state);


/* Constructor & Destructor */
buf buf_make(void);

void buf_delete(buf);


/* Public */
err_type buf_addSym(buf, char, state);

err_type buf_addToList(buf, list, word_type);

err_type buf_getSym(buf, list, char, err_type);

#endif
=======
#include "list.c"
#include "symbols.c"

enum state{
    GOT_SPACE,
    GOT_WORD,
    GOT_SPECIAL,
};

enum { MIN_LEN = 3 };

struct buf{
    char *str;
    int size;
    enum state state;
};

typedef struct buf buf;

buf *buf_make(void);

void buf_in(buf *, char *, int, enum state);

void buf_out(buf *, char **, int*, enum state *);

void err_overflow(void *, void *, int *);

void buf_getlex(char, list, buf *, int *);
>>>>>>> craft_table
