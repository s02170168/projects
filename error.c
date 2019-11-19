#include "error.h"
#include <stdio.h>

void err_msg(err_type err){
    switch (err){
        case syntax:
            printf("Syntax error\n");
            break;
        case allocation:
            printf("Memory allocation error\n");
            break;
        default:
            break;
    }
}