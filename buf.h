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
