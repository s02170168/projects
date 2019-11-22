#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "list.h"

list ls_make(){
    list temp = (list) malloc(sizeof(*temp));
    if(temp == NULL){
        return NULL;
    }
    temp->words = NULL;
    temp->types = NULL;
    temp->count = 0;
    return temp;
}

err_type ls_add(list words, string word, word_type type){
    err_type res = no_err;

    int cnt = words->count;
    string *w_ptr = words->words;
    word_type *t_prt = words->types;
    if(cnt){
        w_ptr = (string *) realloc(w_ptr, (cnt + 1) * sizeof(word));
        t_prt = (word_type *) realloc(t_prt, (cnt + 1) * sizeof(type));
    } else{
        w_ptr = (string *) malloc(sizeof(word));
        t_prt = (word_type *) malloc(sizeof(type));
    }

    if(w_ptr == NULL){
        res = allocation;
    } else{
        words->words = w_ptr;
    }

    if(t_prt == NULL){
        res = allocation;
    } else{
        words->types = t_prt;
    }

    if(res != no_err){
        return res;
    }

    words->words[cnt] = word;
    words->types[cnt] = type;
    words->count++;
    return no_err;
}

void ls_upgrade(list words){
    for (int i = 0; i < words->count; ++i) {

        if(strcmp(words->words[i], "$HOME") == 0){
            free(words->words[i]);
            string temp = getenv("HOME");

            int len = 0;
            while (temp[len] != 0){
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if(strcmp(words->words[i], "$USER") == 0){
            free(words->words[i]);
            string temp = getenv("USER");

            int len = 0;
            while (temp[len] != 0){
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if(strcmp(words->words[i], "$SHELL") == 0){
            free(words->words[i]);
            string temp = getenv("SHELL");

            int len = 0;
            while (temp[len] != 0){
                ++len;
            }

            words->words[i] = (string) malloc((len + 1) * sizeof(*temp));
            for (int j = 0; j < len; ++j) {
                words->words[i][j] = temp[j];
            }
            words->words[i][len] = 0;

        } else if(strcmp(words->words[i], "$EUID") == 0){
            int temp = geteuid();
            int euid = temp;
            int len = 0;
            while (temp){
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

int ls_null(list words){
    if(words == NULL) { return 0; }
    int cnt = 0;
    for (int i = 0; i < words->count; ++i) {
        if(words->words[i] == NULL) { ++cnt; }
    }

    return cnt;
}

void ls_print(list words) {
    int cnt = words->count - ls_null(words);
    if(!cnt) { return; }
    printf("%d\n", cnt);
    for (int i = 0; i < words->count; ++i) {
        if (words->words[i] != NULL) {
            printf("%s\n", words->words[i]);
        }
    }
}

void ls_clear(list words){
    if(words != NULL){
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

void ls_delete(list words){
    ls_clear(words);
    free(words);
}