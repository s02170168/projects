#include <stdio.h>
#include <malloc.h>

#include "list.h"

list ls_make(){
    list temp = (list) malloc(sizeof(*temp));
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

void ls_print(list words){
    for (int i = 0; i < words->count; ++i) {
        if(words->words[i] != NULL){
            printf("%s    ", words->words[i]);
            printf((words->types[i] == simple) ? ("simple\n") : ("special\n"));
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