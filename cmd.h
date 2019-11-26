#ifndef CMD_H
#define CMD_H

#include "myShellHeaders.h"
#include "list.h"

/* Private */
typedef struct command {
    char **argv;
    char **sub_shell;
    string file_in;
    string file_out;
    int out_type;
    int background;
    int next_type;
} command;

struct cmd {
    command *commands;
    int commandsCount;
};

typedef struct cmd *cmd;

typedef struct prStack {
    pid_t pid;
    int number;
    struct prStack *next;
} prStack;

typedef enum cmd_type {
    open_bracket,
    close_bracket,
    next,
    next_pipe,
    next_xor,
    next_and,
    background,
    file_in,
    file_out_begin,
    file_out_end,
} cmd_type;

cmd_type cmd_getType(string);

int cmd_checkBrackets(list);

void cmd_clear(cmd);

err_type cmd_addCommand(cmd);

err_type cmd_pushArgv(cmd, list, int *);

err_type cmd_pushSubShell(cmd, list, int *);

void cmd_shellMessage(int, const char *);

void cmd_prKill(prStack *);

prStack *cmd_pushPr(prStack *, pid_t);


/* Constructor & Destructor */
cmd cmd_make(void);

void cmd_delete(cmd);


/* Public */
err_type cmd_fill(list, cmd);

void cmd_printStructure(cmd);

int cmd_shellExec(cmd, prStack **, int *);

prStack *cmd_prCheck(prStack *);

#endif