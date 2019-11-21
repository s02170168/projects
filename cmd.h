#ifndef CMD_H
#define CMD_H

#include "list.h"

typedef struct command{
    char **argv;
    string file_in;
    int out_type;
    string file_out;
    int background;
    char **sub_shell;
    int next_type;
} command;

struct cmd{
    command *commands;
    int count;
};
typedef struct cmd * cmd;

err_type cmd_add(cmd);

cmd cmd_make(void);

err_type cmd_fill(list, cmd);

err_type cmd_push_argv(cmd, list, int *);

err_type cmd_push_sub_shell(cmd, list, int *);

void cmd_clear(cmd);

void cmd_delete(cmd);

void print_structure(cmd);

#endif