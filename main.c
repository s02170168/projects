#include "myShellHeaders.h"
#include "buf.h"
#include "list.h"
#include "cmd.h"

buf buffer = NULL;
list words = NULL;
cmd commands = NULL;
prStack *processes = NULL;
prStack *conveyor = NULL;

void invite(void);

void checkAndContinue(err_type);

void checkAndExit(err_type);

void sigHandler(int);

jmp_buf begin;

int main(int argc, char **argv) {
    signal(SIGINT, sigHandler);
    /* Программа с аргументами */
    if (argc - 1) {
        words = ls_make();
        checkAndExit(ls_argvToWords(words, argv));
        commands = cmd_make();
        checkAndExit(cmd_fill(words, commands));
        int exitStatus = 0;
        int status = cmd_shellExec(commands, &processes, &conveyor, &exitStatus);

        ls_delete(words);
        cmd_delete(commands);
        while (processes) { processes = cmd_prCheck(processes); }
        exit(status);
    }

    /* Программа без аргументов */
    setjmp(begin);
    buffer = buf_make();
    words = ls_make();
    commands = cmd_make();

    int status = 0;
    int exitStatus = 0;

    if (buffer == NULL || words == NULL || commands == NULL) {
        err_msg(allocation);
        buf_delete(buffer);
        ls_delete(words);
        cmd_delete(commands);
        return 0;
    }

    err_type err = no_err;
    char c;
    char c_prev = '\n';
    int sym_got = 0;

    invite();
    while ((c = (char) getchar()) != EOF) {
        sym_got = 1;
        checkAndContinue(err = buf_getSym(buffer, words, c, err));

        c_prev = c;

        if (c_prev == '\n') {
            ls_replacePathVars(words);

            //Этап разбиения на команды
            checkAndContinue(cmd_fill(words, commands));
            //Запуск программ
            status = cmd_shellExec(commands, &processes, &conveyor, &exitStatus);
            if(exitStatus){
                buf_delete(buffer);
                ls_delete(words);
                cmd_delete(commands);
                cmd_prKill(processes);
                exit(status);
            }
            processes = cmd_prCheck(processes);

            //Переход к следующей строке
            invite();
            sym_got = 0;
        }
    }

    if ((c == EOF && c_prev != '\n') || !sym_got) { putchar('\n'); }

    checkAndExit(buf_getSym(buffer, words, '\n', err));
    checkAndExit(cmd_fill(words, commands));
    status = cmd_shellExec(commands, &processes, &conveyor, &exitStatus);

    buf_delete(buffer);
    ls_delete(words);
    cmd_delete(commands);
    while (processes) { processes = cmd_prCheck(processes); }
    exit(status);
}

void invite() {
    char hostName[PATH_MAX];
    gethostname(hostName, PATH_MAX);
    printf("%s%s@%s%s==> %s", "\033[01;36m", getenv("USER"), hostName, "\033[22;34m", "\033[0m");
}

void checkAndContinue(err_type err) {
    if (err == no_err) { return; }

    err_msg(err);
    buf_delete(buffer);
    ls_delete(words);
    cmd_delete(commands);

    longjmp(begin, 1);
}

void checkAndExit(err_type err) {
    if (err == no_err) { return; }
    err_msg(err);
    ls_delete(words);
    buf_delete(buffer);
    cmd_delete(commands);
    cmd_prKill(processes);
    cmd_prKill(conveyor);
    exit(0);
}

/* Обработчик сигнала SIGINT */
/* При получении сигнала процесс освобождает всю выделенную память */
/* и завершается */
void sigHandler(int signal) {
    buf_delete(buffer);
    ls_delete(words);
    cmd_delete(commands);
    cmd_prKill(processes);
    cmd_prKill(conveyor);
    exit(1);
}