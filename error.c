#include "error.h"
#include <stdio.h>

void err_msg(err_type err){
    switch (err){
        case syntax:
            printf("%sSyntax error%s\n", "\033[22;31m", "\033[0m");
            break;
        case allocation:
            printf("%sMemory allocation error%s\n", "\033[22;31m", "\033[0m");
            break;
        default:
            break;
    }
}