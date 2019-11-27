#include "myShellHeaders.h"
#include "cmd.h"
#include "list.h"

cmd_type cmd_getType(string word) {
    string temp = word;
    if (!strcmp(temp, "(")) { return open_bracket; }
    if (!strcmp(temp, ")")) { return close_bracket; }
    if (!strcmp(temp, ";")) { return next; }
    if (!strcmp(temp, "|")) { return next_pipe; }
    if (!strcmp(temp, "||")) { return next_xor; }
    if (!strcmp(temp, "&&")) { return next_and; }
    if (!strcmp(temp, "&")) { return background; }
    if (!strcmp(temp, "<")) { return file_in; }
    if (!strcmp(temp, ">")) { return file_out_begin; }
    return file_out_end;
}

int cmd_checkBrackets(list words) {
    int brackets = 0;
    int start = 0;
    for (int i = 0; i < words->count; ++i) {
        if (strcmp(words->words[i], "(") == 0) {
            if (!brackets) { start = i; }
            ++brackets;
        } else if (strcmp(words->words[i], ")") == 0) {
            --brackets;
            if (brackets < 0) { return -1; }
            if (brackets == 0) {
                if (i - start < 2) { return -1; }

                if (i < words->count - 1) {
                    if (words->types[i + 1] == simple || !strcmp(words->words[i + 1], "(")) { return -1; }
                }
            }
        }
    }
    return (brackets) ? (-1) : (0);
}

err_type cmd_fill(list words, cmd commands) {
    if (!words->count) { return no_err; }
    if (cmd_checkBrackets(words)) { return syntax; }
    if (words->types[0] == special && cmd_getType(words->words[0]) != open_bracket) {
        return syntax;
    }

    if(ls_collapseConveyors(words) != no_err) { return allocation; }

    int words_cnt = words->count;
    for (int i = 0; i < words_cnt; ++i) {
        if (words->types[i] == simple) {
            if (cmd_addCommand(commands) != no_err) { return allocation; }
            if (cmd_pushArgv(commands, words, &i) != no_err) { return allocation; }
        } else {

            cmd_type type = cmd_getType(words->words[i]);
            if (type != open_bracket && !i) { return syntax; }
            int cur = commands->commandsCount - 1;
            command *temp = commands->commands;

            switch (type) {
                case open_bracket:
                    if (cmd_addCommand(commands) != no_err) { return allocation; }
                    if (cmd_pushSubShell(commands, words, &i) != no_err) { return allocation; }
                    break;
                case next:
                    temp[cur].next_type = 0;
                    if (i < words_cnt - 1 && (words->types[i + 1] == special &&
                                              cmd_getType(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_pipe:
                    temp[cur].next_type = 1;
                    if (i == words_cnt - 1 || (words->types[i + 1] == special &&
                                               cmd_getType(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_xor:
                    temp[cur].next_type = 2;
                    if (i == words_cnt - 1 || (words->types[i + 1] == special &&
                                               cmd_getType(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_and:
                    temp[cur].next_type = 3;
                    if (i == words_cnt - 1 || (words->types[i + 1] == special &&
                                               cmd_getType(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case background:
                    temp[cur].next_type = 0;
                    temp[cur].background = 1;
                    if (i < words_cnt - 1 && (words->types[i + 1] == special &&
                                              cmd_getType(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case file_in:
                    if (i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].file_in = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                case file_out_begin:
                    if (i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].out_type = 1;
                    temp[cur].file_out = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                case file_out_end:
                    if (i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].out_type = 2;
                    temp[cur].file_out = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                default:
                    return syntax;
            }
        }
    }
    ls_clear(words);

    return no_err;
}

cmd cmd_make() {
    cmd temp = (cmd) malloc(sizeof(*temp));
    if (temp == NULL) { return NULL; }

    temp->commands = NULL;
    temp->commandsCount = 0;
    return temp;
}

void cmd_clear(cmd commands) {
    if (commands == NULL) { return; }
    command *temp = commands->commands;
    for (int i = 0; i < commands->commandsCount; ++i) {
        if (temp[i].argv != NULL) {
            char **temp_str = temp[i].argv;
            int j = 0;
            while (temp_str[j] != NULL) {
                free(temp_str[j]);
                ++j;
            }
            free(temp[i].argv);
        }

        if (temp[i].file_in != NULL) {
            free(temp[i].file_in);
        }

        if (temp[i].file_out != NULL) {
            free(temp[i].file_out);
        }

        if (temp[i].sub_shell != NULL) {
            char **temp_str = temp[i].sub_shell;
            int j = 0;
            while (temp_str[j] != NULL) {
                free(temp_str[j]);
                ++j;
            }
            free(temp[i].sub_shell);
        }

    }
    if (commands->commandsCount) {
        free(commands->commands);
    }
    commands->commands = NULL;
    commands->commandsCount = 0;
}

void cmd_delete(cmd commands) {
    if (commands != NULL) {
        cmd_clear(commands);
        free(commands);
    }
}

err_type cmd_addCommand(cmd commands) {
    int cnt = commands->commandsCount;
    command *c_ptr = commands->commands;

    c_ptr = (command *) realloc(c_ptr, (cnt + 1) * sizeof(*c_ptr));
    if (c_ptr == NULL) { return allocation; }

    c_ptr[cnt].argv = NULL;
    c_ptr[cnt].file_in = NULL;
    c_ptr[cnt].out_type = 0;
    c_ptr[cnt].file_out = NULL;
    c_ptr[cnt].background = 0;
    c_ptr[cnt].sub_shell = NULL;
    c_ptr[cnt].next_type = 0;

    commands->commands = c_ptr;
    commands->commandsCount++;
    return no_err;
}

err_type cmd_pushArgv(cmd commands, list words, int *idx) {
    if (!commands->commandsCount) { return allocation; }
    int i = *idx;
    int start = i;
    int count = 0;

    while (i < words->count && words->types[i] == simple) {
        ++i;
        ++count;
    }

    char **argv = (char **) malloc((count + 1) * sizeof(*argv));
    if (argv == NULL) { return allocation; }
    for (int j = 0; j < count; ++j) {
        argv[j] = words->words[start + j];
        words->words[start + j] = NULL;
    }
    argv[count] = NULL;

    *idx = i - 1;
    int number = commands->commandsCount - 1;
    commands->commands[number].argv = argv;

    return no_err;
}

err_type cmd_pushSubShell(cmd commands, list words, int *idx) {
    if (!commands->commandsCount) { return allocation; }
    int i = *idx + 1;
    int start = i;
    int count = 0;

    int brackets = 1;
    while (brackets) {
        if (cmd_getType(words->words[i]) == open_bracket) { ++brackets; }
        if (cmd_getType(words->words[i]) == close_bracket) { --brackets; }
        ++i;
        ++count;
    }
    --count;
    char **sub_shell = (char **) malloc((count + 2) * sizeof(*sub_shell));
    if (sub_shell == NULL) { return allocation; }

    sub_shell[0] = (string) malloc((strlen("./my_shell") + 1) * sizeof(**sub_shell));
    if (sub_shell[0] == NULL) {
        free(sub_shell);
        return allocation;
    }
    strcpy(sub_shell[0], "./my_shell");

    for (int j = 1; j < count + 1; ++j) {
        sub_shell[j] = words->words[start + j - 1];
        words->words[start + j - 1] = NULL;
    }
    sub_shell[count + 1] = NULL;

    *idx = i - 1;
    int number = commands->commandsCount - 1;
    commands->commands[number].sub_shell = sub_shell;

    return no_err;
}

void cmd_printStructure(cmd commands) {
    int cnt = commands->commandsCount;
    if (!cnt) { return; }
    command *temp = commands->commands;

    printf("%s------------%s\n", "\033[22;34m", "\033[0m");
    for (int i = 0; i < cnt; ++i) {
        printf("%sArgument vector: %s", "\033[22;35m", "\033[0m");
        if (temp[i].argv != NULL) {
            for (int j = 0; temp[i].argv[j] != NULL; ++j) {
                printf("%s ", temp[i].argv[j]);
            }
            printf("\n");
        } else {
            printf("null\n");
        }
        printf("%sInput file: %s", "\033[22;35m", "\033[0m");
        if (temp[i].file_in != NULL) {
            printf("%s\n", temp[i].file_in);
        } else {
            printf("stdin\n");
        }
        printf("%sOutput type: %s%d\n", "\033[22;35m", "\033[0m", temp[i].out_type);
        printf("%sOutput file: %s", "\033[22;35m", "\033[0m");
        if (temp[i].file_out != NULL) {
            printf("%s\n", temp[i].file_out);
        } else {
            printf("stdout\n");
        }
        printf("%sBackground: %s%d\n", "\033[22;35m", "\033[0m", temp[i].background);
        printf("%sSub_shell: %s", "\033[22;35m", "\033[0m");
        if (temp[i].sub_shell != NULL) {
            for (int j = 0; temp[i].sub_shell[j] != NULL; ++j) {
                printf("%s ", temp[i].sub_shell[j]);
            }
            printf("\n");
        } else {
            printf("null\n");
        }
        printf("%sNext type: %s%d\n", "\033[22;35m", "\033[0m", temp[i].next_type);
        printf("%s------------%s\n", "\033[22;34m", "\033[0m");
    }
}

#define N_OPEN 1
#define N_FORK 2
#define N_EXEC 3
#define N_CD 4
#define N_EXIT 5

void cmd_shellMessage(int msg_type, const char *msg) {
    switch (msg_type) {
        case N_OPEN:
            printf("%sCan't open file %s%s\n", "\033[22;33m", msg, "\033[0m");
            break;
        case N_FORK:
            printf("%sCan't make new process%s\n", "\033[22;33m", "\033[0m");
            break;
        case N_EXEC:
            printf("%sCan't execute %s%s\n", "\033[22;33m", msg, "\033[0m");
            break;
        case N_CD:
            if (msg == NULL) {
                printf("%sToo many arguments for cd%s\n", "\033[22;33m", "\033[0m");
            } else {
                printf("%sNo such file or directory: %s%s\n", "\033[22;33m", msg, "\033[0m");
            }
            break;
        case N_EXIT:
            printf("%sToo many arguments for exit%s\n", "\033[22;33m", "\033[0m");
            break;
        default:
            break;
    }
}

void cmd_prKill(prStack *processes) {
    while (processes != NULL) {
        kill(processes->pid, SIGKILL);
        int status;
        waitpid(processes->pid, &status, WNOWAIT);
        prStack *temp = processes;
        processes = processes->next;
        free(temp);
    }
}

prStack *cmd_prPush(prStack *processes, pid_t pid) {
    prStack *temp;
    temp = (prStack *) malloc(sizeof(*temp));
    temp->pid = pid;
    temp->next = processes;
    return temp;
}

prStack *cmd_prCheck(prStack *processes) {
    if (processes != NULL) {
        processes->next = cmd_prCheck(processes->next);
        int status;
        if (waitpid(processes->pid, &status, WNOHANG)) {
            prStack *temp = processes->next;
            free(processes);
            return temp;
        } else {
            return processes;
        }

    } else {
        return NULL;
    }
}

int cmd_shellExec(cmd commands, prStack **processes, int *exitStatus) {
    if (commands == NULL) { return 0; }

    int status = 0;
    int file_in, file_out;
    pid_t pid;
    int fd[2];
    int nextIn = 0;
    int goNext = 1;

    prStack *conveyor = NULL;
    int currentProcessStatus = 1;

    for (int i = 0; i < commands->commandsCount; ++i) {
        command temp = commands->commands[i];

        if(!goNext){
            if((temp.next_type == 2 && !currentProcessStatus) ||
                    (temp.next_type == 3 && currentProcessStatus)){
                goNext = 1;
            }
            if(!temp.next_type) { goNext = 1; }
            continue;
        }

        /* Внутренняя команда exit */
        if (temp.argv != NULL && !strcmp(temp.argv[0], "exit")) {
            *exitStatus = 0;
            if (temp.argv[1] != NULL) {
                if (temp.argv[2] != NULL) {
                    cmd_shellMessage(N_EXIT, NULL);
                    continue;
                } else {
                    *exitStatus = atoi(temp.argv[1]);
                }
            }
            return 1;
        }

        /* Внутренняя команда cd */
        if (temp.argv != NULL && !strcmp(temp.argv[0], "cd")) {
            if (temp.argv[1] == NULL) {
                chdir(getenv("HOME"));
            } else if (temp.argv[2] == NULL) {
                if (chdir(temp.argv[1]) == -1) {
                    cmd_shellMessage(N_CD, temp.argv[1]);
                }
            } else {
                cmd_shellMessage(N_CD, NULL);
            }
            continue;
        }

        if (temp.next_type == 1) {
            pipe(fd);
        }

        switch (pid = fork()) {
            /* Процесс создать не удалось */
            case -1:
                cmd_shellMessage(N_FORK, NULL);
                if (temp.next_type == 1) {
                    close(fd[0]);
                    close(fd[1]);
                }
                if (nextIn) {
                    close(nextIn);
                    nextIn = 0;
                }
                continue;

            /* Процесс-сын */
            case 0:
                /* Перенаправление потока ввода */
                if (nextIn) {
                    dup2(nextIn, 0);
                    close(nextIn);
                    nextIn = 0;
                }

                if (temp.next_type == 1) {
                    close(fd[0]);
                    dup2(fd[1], 1);
                    close(fd[1]);
                }

                if (temp.file_in != NULL) {
                    file_in = open(temp.file_in, O_RDONLY);
                    if (file_in == -1 || dup2(file_in, 0) == -1) {
                        cmd_shellMessage(N_OPEN, temp.file_in);
                        continue;
                    }
                    close(file_in);
                }
                /* Перенаправление потока вывода */
                if (temp.file_out != NULL) {
                    if (temp.out_type == 1) {
                        file_out = open(temp.file_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    } else {
                        file_out = open(temp.file_out, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    }
                    if (file_out == -1 || dup2(file_out, 1) == -1) {
                        cmd_shellMessage(N_OPEN, temp.file_out);
                        continue;
                    }
                }


                /* Внутренняя команда pwd */
                if (temp.argv != NULL && !strcmp(temp.argv[0], "pwd")) {
                    char pwd[PATH_MAX];
                    getcwd(pwd, PATH_MAX);
                    printf("%s\n", pwd);
                    exit(0);
                }

                /* Произвольная команда */
                if (temp.argv != NULL) {
                    execvp(temp.argv[0], temp.argv);
                    cmd_shellMessage(N_EXEC, temp.argv[0]);
                } else {
                    execvp(temp.sub_shell[0], temp.sub_shell);
                    cmd_shellMessage(N_EXEC, temp.sub_shell[0]);
                }
                exit(1);

            /* Родительский процесс */
            default:
                if (nextIn) {
                    close(nextIn);
                }
                if (temp.next_type == 1) {
                    close(fd[1]);
                    nextIn = fd[0];
                }

                if (temp.next_type == 1) {
                    conveyor = cmd_prPush(conveyor, pid);
                } else {
                    nextIn = 0;
                    if (temp.background) {
                        *processes = cmd_prPush(*processes, pid);
                        prStack *pr_ptr;
                        while (conveyor != NULL) {
                            *processes = cmd_prPush(*processes, conveyor->pid);
                            pr_ptr = conveyor;
                            conveyor = conveyor->next;
                            free(pr_ptr);
                        }
                    } else {
                        waitpid(pid, &status, 0);
                        while (conveyor != NULL) {
                            conveyor = cmd_prCheck(conveyor);
                        }
                    }
                }

                currentProcessStatus = (WIFEXITED(status) && !WEXITSTATUS(status)) ? (1) : (0);

                if((temp.next_type == 2 && currentProcessStatus) ||
                        (temp.next_type == 3 && !currentProcessStatus)){
                    goNext = 0;
                }

                break;
        }
    }

    if (nextIn) { close(nextIn); }
    while (conveyor) { conveyor = cmd_prCheck(conveyor); }

    cmd_clear(commands);
    return 0;
}