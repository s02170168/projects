#include <stdio.h>
#include <setjmp.h>

#include "buf.h"
#include "list.h"
#include "error.h"

jmp_buf begin;
const char invitation[] = "==>";

int main() {
    setjmp(begin);

    buf buffer = buf_make();
    list words = ls_make();
    err_type err = no_err;
    char c;
    char c_prev = '\n';


    if(c_prev == '\n'){
        printf(invitation);
    }
    while ( (c = getchar()) != EOF){
        err = buf_get_sym(buffer, words, c, err);
        switch (err){
            case syntax:
            case allocation:
                err_msg(err);
                ls_delete(words);
                buf_delete(buffer);
                longjmp(begin, 1);
            case no_err:
            default:
                break;
        }
        c_prev = c;
        if(c_prev == '\n'){
            printf(invitation);
        }
    }

    if(err == wait_syntax){
        err_msg(syntax);
        ls_delete(words);
        buf_delete(buffer);
        return 0;
    }



    ls_delete(words);
    buf_delete(buffer);
    return 0;
}