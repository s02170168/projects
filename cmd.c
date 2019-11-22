#include <malloc.h>
#include <string.h>

#include "cmd.h"
#include "list.h"
#include "error.h"

typedef enum cmd_type{
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

cmd_type cmd_get_type(string word){
    string temp = word;
    if(!strcmp(temp, "(")) { return open_bracket; }
    if(!strcmp(temp, ")")) { return close_bracket; }
    if(!strcmp(temp, ";")) { return next; }
    if(!strcmp(temp, "|")) { return next_pipe; }
    if(!strcmp(temp, "||")) { return next_xor; }
    if(!strcmp(temp, "&&")) { return next_and; }
    if(!strcmp(temp, "&")) { return background; }
    if(!strcmp(temp, "<")) { return file_in; }
    if(!strcmp(temp, ">")) { return file_out_begin; }
    return file_out_end;
}

int cmd_check_brackets(list words){
    int brackets = 0;
    int start = 0;
    for (int i = 0; i < words->count; ++i) {
        if(strcmp(words->words[i], "(") == 0){
            if(!brackets){ start = i; }
            ++brackets;
        } else if(strcmp(words->words[i], ")") == 0){
            --brackets;
            if(brackets < 0) { return -1; }
            if(!brackets && (i - start < 2)) { return -1; }
        }
    }

    return (brackets) ? (-1) : (0);
}

err_type cmd_fill(list words, cmd commands){
    if(!words->count) { return no_err; }
    if(cmd_check_brackets(words)) { return syntax; }
    if(words->types[0] == special && cmd_get_type(words->words[0]) != open_bracket){
        return syntax;
    }

    int words_cnt = words->count;
    for (int i = 0; i < words_cnt; ++i) {
        if(words->types[i] == simple){
            if(cmd_add(commands) != no_err) { return allocation; }
            if(cmd_push_argv(commands, words, &i) != no_err) { return allocation; }
        } else{

            cmd_type type = cmd_get_type(words->words[i]);
            if(type != open_bracket && !i) { return syntax; }
            int cur = commands->count - 1;
            command *temp = commands->commands;

            switch (type){
                case open_bracket:
                    if(cmd_add(commands) != no_err) { return allocation; }
                    if(cmd_push_sub_shell(commands, words, &i) != no_err) { return allocation; }
                    break;
                case next:
                    temp[cur].next_type = 0;
                    if(i < words_cnt - 1 && (words->types[i + 1] == special &&
                        cmd_get_type(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_pipe:
                    temp[cur].next_type = 1;
                    if(i == words_cnt - 1 || (words->types[i + 1] == special &&
                        cmd_get_type(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_xor:
                    temp[cur].next_type = 2;
                    if(i == words_cnt - 1 || (words->types[i + 1] == special &&
                        cmd_get_type(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case next_and:
                    temp[cur].next_type = 3;
                    if(i == words_cnt - 1 || (words->types[i + 1] == special &&
                        cmd_get_type(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case background:
                    temp[cur].next_type = 0;
                    temp[cur].background = 1;
                    if(i < words_cnt - 1 && (words->types[i + 1] == special &&
                        cmd_get_type(words->words[i + 1]) != open_bracket)) { return syntax; }
                    break;
                case file_in:
                    if(i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].file_in = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                case file_out_begin:
                    if(i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].out_type = 1;
                    temp[cur].file_out = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                case file_out_end:
                    if(i == words_cnt - 1 || words->types[i + 1] == special) { return syntax; }
                    temp[cur].out_type = 2;
                    temp[cur].file_out = words->words[i + 1];
                    words->words[++i] = NULL;
                    break;
                default:
                    return syntax;
            }
        }
    }

    return no_err;
}

cmd cmd_make(){
    cmd temp = (cmd) malloc(sizeof(*temp));
    if(temp == NULL){ return NULL; }

    temp->commands = NULL;
    temp->count = 0;
    return temp;
}

void cmd_clear(cmd commands){
    if(commands == NULL) { return; }

    command *temp = commands->commands;
    for (int i = 0; i < commands->count; ++i) {
        if(temp[i].argv != NULL){
            char **temp_str = temp[i].argv;
            int j = 0;
            while (temp_str[j] != NULL){
                free(temp_str[j]);
                ++j;
            }
            free(temp[i].argv);
        }

        if(temp[i].file_in != NULL){
            free(temp[i].file_in);
        }

        if(temp[i].file_out != NULL){
            free(temp[i].file_out);
        }

        if(temp[i].sub_shell != NULL){
            char **temp_str = temp[i].sub_shell;
            int j = 0;
            while (temp_str[j] != NULL){
                free(temp_str[j]);
                ++j;
            }
            free(temp[i].sub_shell);
        }

    }
    if(commands->count){
        free(commands->commands);
    }
    commands->commands = NULL;
    commands->count = 0;
}

void cmd_delete(cmd commands){
    if(commands != NULL){
        cmd_clear(commands);
        free(commands);
    }
}

err_type cmd_add(cmd commands){
    int cnt = commands->count;
    command *c_ptr = commands->commands;

    c_ptr = (command *) realloc(c_ptr, (cnt + 1) * sizeof(*c_ptr));
    if(c_ptr == NULL) { return allocation; }

    c_ptr[cnt].argv = NULL;
    c_ptr[cnt].file_in = NULL;
    c_ptr[cnt].out_type = 0;
    c_ptr[cnt].file_out = NULL;
    c_ptr[cnt].background = 0;
    c_ptr[cnt].sub_shell = NULL;
    c_ptr[cnt].next_type = 0;

    commands->commands = c_ptr;
    commands->count++;
    return no_err;
}

err_type cmd_push_argv(cmd commands, list words, int *idx){
    if(!commands->count) { return allocation; }
    int i = *idx;
    int start = i;
    int count = 0;

    while (i < words->count && words->types[i] == simple){
        ++i;
        ++count;
    }

    char **argv = (char **) malloc((count + 1) * sizeof(*argv));
    if(argv == NULL) { return allocation; }
    for (int j = 0; j < count; ++j) {
        argv[j] = words->words[start + j];
        words->words[start + j] = NULL;
    }
    argv[count] = NULL;

    *idx = i - 1;
    int number = commands->count - 1;
    commands->commands[number].argv = argv;

    return no_err;
}

err_type cmd_push_sub_shell(cmd commands, list words, int *idx){
    if(!commands->count) { return allocation; }
    int i = *idx + 1;
    int start = i;
    int count = 0;

    int brackets = 1;
    while (brackets){
        if(cmd_get_type(words->words[i]) == open_bracket) { ++brackets;  }
        if(cmd_get_type(words->words[i]) == close_bracket) { --brackets;  }
        ++i;
        ++count;
    }
    --count;
    char **sub_shell = (char **) malloc((count + 1) * sizeof(*sub_shell));
    if(sub_shell == NULL) { return allocation; }
    for (int j = 0; j < count; ++j) {
        sub_shell[j] = words->words[start + j];
        words->words[start + j] = NULL;
    }
    sub_shell[count] = NULL;

    *idx = i - 1;
    int number = commands->count - 1;
    commands->commands[number].sub_shell = sub_shell;

    return no_err;
}

void print_structure(cmd commands){
    int cnt = commands->count;
    if(!cnt) { return; }
    command *temp = commands->commands;

    printf("%s------------%s\n", "\033[22;34m", "\033[0m");
    for (int i = 0; i < cnt; ++i) {
        printf("%sArgument vector: %s", "\033[22;35m", "\033[0m");
        if(temp[i].argv != NULL){
            for (int j = 0; temp[i].argv[j] != NULL; ++j) {
                printf("%s ", temp[i].argv[j]);
            }
            printf("\n");
        } else{
            printf("null\n");
        }
        printf("%sInput file: %s", "\033[22;35m", "\033[0m");
        if(temp[i].file_in != NULL){
            printf("%s\n", temp[i].file_in);
        } else{
            printf("stdin\n");
        }
        printf("%sOutput type: %s%d\n", "\033[22;35m", "\033[0m", temp[i].out_type);
        printf("%sOutput file: %s", "\033[22;35m", "\033[0m");
        if(temp[i].file_out != NULL){
            printf("%s\n", temp[i].file_out);
        } else{
            printf("stdout\n");
        }
        printf("%sBackground: %s%d\n", "\033[22;35m", "\033[0m", temp[i].background);
        printf("%sSub_shell: %s", "\033[22;35m", "\033[0m");
        if(temp[i].sub_shell != NULL){
            for (int j = 0; temp[i].sub_shell[j] != NULL; ++j) {
                printf("%s ", temp[i].sub_shell[j]);
            }
            printf("\n");
        } else{
            printf("null\n");
        }
        printf("%sNext type: %s%d\n", "\033[22;35m", "\033[0m", temp[i].next_type);
        printf("%s------------%s\n", "\033[22;34m", "\033[0m");
    }
}