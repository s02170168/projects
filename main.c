#include <stdio.h>
#include <setjmp.h>

#include "buf.h"
#include "list.h"
#include "error.h"
#include "cmd.h"

#define N_OPEN 1
#define N_FORK 2
#define N_EXEC 3

jmp_buf begin;
buf buffer = NULL;
list words = NULL;
cmd commands = NULL;

void invite(void);
void check_status(err_type);
void shell_msg(int, const char *);

int main(int argc, char **argv) {

    setjmp(begin);

    buffer = buf_make();
    words = ls_make();
    commands = cmd_make();
    if(buffer == NULL || words == NULL || commands == NULL){
        err_msg(allocation);
        buf_delete(buffer);
        ls_delete(words);
        cmd_delete(commands);
        return 0;
    }

    /////////////////////////////////////////////////////

    err_type err = no_err;
    char c;
    char c_prev = '\n';
    int sym_got = 0;

    /////////////////////////////////////////////////////

    invite();
    while ( (c = (char) getchar()) != EOF){
        sym_got = 1;
        check_status(err = buf_get_sym(buffer, words, c, err));

        c_prev = c;

        if(c_prev == '\n'){
            ls_upgrade(words);

            //Этап разбиения на команды
            ls_print(words);
            check_status(err = cmd_fill(words, commands));
            ls_clear(words);
            print_structure(commands);
            //Запуск программ


            cmd_clear(commands);
            invite();
            sym_got = 0;
        }
    }

    /////////////////////////not edited/////////////////////

    if((c == EOF && c_prev != '\n') || !sym_got){
        putchar('\n');
    }

    if(err == wait_syntax) {
        err_msg(syntax);
        ls_delete(words);
        buf_delete(buffer);
        cmd_delete(commands);
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
                cmd_delete(commands);
                return 0;
            case no_err:
            default:
                break;
        }
    }
    ls_print(words);

    /////////////////////////////////////////////////////

    buf_delete(buffer);
    ls_delete(words);
    cmd_delete(commands);
    return 0;
}

void invite(){
    printf("%s==> %s", "\033[22;34m", "\033[0m");
}

void check_status(err_type err){
    switch (err){
        case no_err:
        case wait_syntax:
            break;
        default:
            err_msg(err);
            buf_delete(buffer);
            ls_delete(words);
            cmd_delete(commands);

            longjmp(begin, 1);
    }
}

void shell_msg(int msg_type, const char *msg){
    switch (msg_type){
        case N_OPEN:
            printf("%sCan't open file %s%s\n", "\033[22;33m", msg, "\033[0m");
            break;
        case N_FORK:
            printf("%sCan't make new process%s\n", "\033[22;33m", "\033[0m");
            break;
        case N_EXEC:
            printf("%sCan't execute %s%s\n", "\033[22;33m", msg, "\033[0m");
            break;
        default:
            break;
    }
}