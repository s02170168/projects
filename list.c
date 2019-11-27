#include "myShellHeaders.h"
#include "list.h"

list ls_make() {
    list temp = (list) malloc(sizeof(*temp));
    if (temp == NULL) {
        return NULL;
    }
    temp->words = NULL;
    temp->types = NULL;
    temp->count = 0;
    return temp;
}

err_type ls_add(list words, string word, word_type type) {
    err_type res = no_err;

    int cnt = words->count;
    string *w_ptr = words->words;
    word_type *t_prt = words->types;
    if (cnt) {
        w_ptr = (string *) realloc(w_ptr, (cnt + 1) * sizeof(word));
        t_prt = (word_type *) realloc(t_prt, (cnt + 1) * sizeof(type));
    } else {
        w_ptr = (string *) malloc(sizeof(word));
        t_prt = (word_type *) malloc(sizeof(type));
    }

    if (w_ptr == NULL) {
        res = allocation;
    } else {
        words->words = w_ptr;
    }

    if (t_prt == NULL) {
        res = allocation;
    } else {
        words->types = t_prt;
    }

    if (res != no_err) {
        return res;
    }

    words->words[cnt] = word;
    words->types[cnt] = type;
    words->count++;
    return no_err;
}

void ls_replacePathVars(list words) {
    for (int i = 0; i < words->count; ++i) {

        if (strcmp(words->words[i], "$HOME") == 0) {
            free(words->words[i]);
            string temp = getenv("HOME");

            int len = 0;
            while (temp[len] != 0) {
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if (strcmp(words->words[i], "$USER") == 0) {
            free(words->words[i]);
            string temp = getenv("USER");

            int len = 0;
            while (temp[len] != 0) {
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if (strcmp(words->words[i], "$SHELL") == 0) {
            free(words->words[i]);
            string temp = getenv("SHELL");

            int len = 0;
            while (temp[len] != 0) {
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if (strcmp(words->words[i], "$EUID") == 0) {
            int temp = geteuid();
            int euid = temp;
            int len = 0;
            while (temp) {
                temp /= 10;
                ++len;
            }

            free(words->words[i]);
            words->words[i] = (string) malloc((len + 1) * sizeof(*words->words[i]));
            for (int j = 0; j < len; ++j) {
                int cnt = 1;
                for (int k = 0; k < len - j - 1; ++k) {
                    cnt *= 10;
                }
                words->words[i][j] = (char) ((euid / cnt) % 10 + '0');
            }
            words->words[i][len] = 0;
        }
    }
}

int ls_null(list words) {
    if (words == NULL) { return 0; }
    int cnt = 0;
    for (int i = 0; i < words->count; ++i) {
        if (words->words[i] == NULL) { ++cnt; }
    }

    return cnt;
}

void ls_print(list words) {
    int cnt = words->count - ls_null(words);
    if (!cnt) { return; }
    printf("%d\n", cnt);
    for (int i = 0; i < words->count; ++i) {
        if (words->words[i] != NULL) {
            printf("%s\n", words->words[i]);
        }
    }
}

void ls_clear(list words) {
    if (words != NULL) {
        for (int i = 0; i < words->count; ++i) {
            free(words->words[i]);
        }
        free(words->words);
        free(words->types);
        words->words = NULL;
        words->types = NULL;
        words->count = 0;
    }
}

void ls_delete(list words) {
    ls_clear(words);
    free(words);
}

err_type ls_argvToWords(list words, char **argv) {
    if (words == NULL) { return allocation; }

    err_type res = no_err;
    int count = 0;
    for (int i = 1; argv[i] != NULL; ++i, ++count);

    string *w_ptr = words->words;
    word_type *t_ptr = words->types;

    w_ptr = (string *) realloc(w_ptr, count * sizeof(*w_ptr));
    t_ptr = (word_type *) realloc(t_ptr, count * sizeof(t_ptr));
    if (w_ptr == NULL) {
        res = allocation;
        if (t_ptr != NULL) {
            free(t_ptr);
        }
    } else {
        words->words = w_ptr;
    }

    if (t_ptr == NULL) {
        res = allocation;
    } else {
        words->types = t_ptr;
    }

    if (res != no_err) { return res; }

    words->count = count;
    for (int i = 0; i < count; ++i) {
        string temp = argv[i + 1];
        words->words[i] = (string) malloc((strlen(temp) + 1) * sizeof(*temp));
        if (words->words[i] == NULL) {
            res = allocation;
            continue;
        }
        strcpy(words->words[i], temp);
        if (!strcmp(temp, "(") || !strcmp(temp, ";") ||
            !strcmp(temp, ")") || !strcmp(temp, ">>") ||
            !strcmp(temp, ">") || !strcmp(temp, "<") ||
            !strcmp(temp, "&&") || !strcmp(temp, "&") ||
            !strcmp(temp, "||") || !strcmp(temp, "|")) {
            words->types[i] = special;
        } else {
            words->types[i] = simple;
        }

    }
    return res;
}

err_type ls_addBrackets(list words, int idx_open, int idx_close){
    
    string open_bracket = (string) malloc(2 * sizeof(*open_bracket));
    if(open_bracket == NULL) { return allocation; }
    string close_bracket = (string) malloc(2 * sizeof(*close_bracket));
    if(close_bracket == NULL){
        free(open_bracket);
        return allocation;
    }
    open_bracket[0] = '(', open_bracket[1] = 0;
    close_bracket[0] = ')', close_bracket[1] = 0;
    
    string *w_ptr = words->words;
    w_ptr = (string *) realloc(w_ptr, (words->count + 2) * sizeof(*w_ptr));
    if(w_ptr == NULL){
        free(open_bracket);
        free(close_bracket);
        return allocation;
    }
    word_type *t_ptr = words->types;
    t_ptr = (word_type *) realloc(t_ptr, (words->count + 2) * sizeof(*t_ptr));
    if(t_ptr == NULL){
        free(open_bracket);
        free(close_bracket);
        free(w_ptr);
        return allocation;
    }


    for (int i = words->count + 1; i > idx_close + 1; --i) {
        w_ptr[i] = w_ptr[i - 2];
        t_ptr[i] = t_ptr[i - 2];
    }
    w_ptr[idx_close + 1] = close_bracket;
    t_ptr[idx_close + 1] = special;
    for (int i = idx_close; i > idx_open; --i) {
        w_ptr[i] = w_ptr[i - 1];
        t_ptr[i] = t_ptr[i - 1];
    }
    w_ptr[idx_open] = open_bracket;
    t_ptr[idx_open] = special;


    words->count += 2;
    words->words = w_ptr;
    words->types = t_ptr;
    
    return no_err;
}

err_type ls_collapseConveyors(list words){
    int start, end;
    int conveyorLength;
    err_type res;

    for (int i = 0; i < words->count; ++i) {
        start = end = i;
        conveyorLength = 1;
        string temp = words->words[end];
        while (end < words->count && !(words->types[end] == special && (!strcmp(temp, ";") || !strcmp(temp, "&")))){
            if(!strcmp(temp, "||") || !strcmp(temp, "&&") || !strcmp(temp, "|")){
                ++conveyorLength;
            }
            ++end;
            if(end < words->count){
                temp = words->words[end];
            }
        }
        if(end == words->count) { return no_err; }
        if(!strcmp(temp, ";")){
            i = end;
            continue;
        }
        if(!strcmp(temp, "&")){
            i = end;
            if(conveyorLength - 1){
                res = ls_addBrackets(words, start, end);
                if(res != no_err) { return res; }
                i += 2;
            }
        }
    }
    return no_err;
}