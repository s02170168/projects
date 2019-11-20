#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "buf.h"
#include "list.h"
#include "error.h"

jmp_buf begin;
const char invitation[] = "==> " ;

int main(int argc, char **argv) {

    if(argc > 1){

        return 0;
    }

    setjmp(begin);
    buf buffer = buf_make();
    list words = ls_make();
    int sym_got = 0;

    //Лексический этап

    err_type err = no_err;
    char c;
    char c_prev = '\n';

    printf(invitation);
    while ( (c = (char) getchar()) != EOF){
        sym_got = 1;
        err = buf_get_sym(buffer, words, c, err);
        switch (err){
            case syntax:
            case allocation:
                err_msg(err);
                ls_delete(words);
                buf_delete(buffer);
                longjmp(begin, 1);
            default:
                break;
        }

        c_prev = c;
        if(c_prev == '\n'){
            //Этап разбиения на команды
            ls_upgrade(words);
            ls_print(words);
            ls_clear(words);


            //Возврат к лексическому этапу
            printf(invitation);
            sym_got = 0;
        }
    }

    if((c == EOF && c_prev != '\n') || !sym_got){
        putchar('\n');
    }

    if(err == wait_syntax) {
        err_msg(syntax);
        ls_delete(words);
        buf_delete(buffer);
        return 0;
    }

    if(buffer->length){
        err = buf_get_sym(buffer, words, '\n', err);
        switch (err){
            case syntax:
            case allocation:
                err_msg(err);
                ls_delete(words);
                buf_delete(buffer);
                return 0;
            case no_err:
            default:
                break;
        }
    }
    buf_delete(buffer);

    //Синтаксический этап
    ls_upgrade(words);
    ls_print(words);
    ls_clear(words);
    //Последовательный запуск команд
    ls_delete(words);

    return 0;
}