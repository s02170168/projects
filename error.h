#ifndef ERROR_H
#define ERROR_H

typedef enum error{
    wait_syntax,
    syntax,
    allocation,
    no_err
} err_type;

void err_msg(err_type);

#endif