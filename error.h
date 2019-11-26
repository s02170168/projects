#ifndef ERROR_H
#define ERROR_H

typedef enum error {
    no_err,
    syntax,
    allocation
} err_type;

void err_msg(err_type);

#endif