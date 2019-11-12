#include "list.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

list ls_make(){
    list temp = (list) malloc(sizeof(*temp));
    temp->wordsCount = 0;
    temp->words = NULL;
    return temp;
}

void ls_add(list ls, string word){
    int cnt = ls->wordsCount;
    if(cnt){
        ls->words = (string*) realloc(ls->words, (cnt + 1) * sizeof(word));
    } else{
        ls->words = (string*) malloc(sizeof(word));
    }
    if(ls->words == NULL){
        return;
    }
    ls->words[cnt] = word;
    ls->wordsCount++;
}

int ls_null(list ls){
    int temp = 0;
    for (int i = 0; i < ls->wordsCount; ++i) {
        if(ls->words[i] == NULL){
            temp++;
        }
    }
    return temp;
}

void ls_print(list ls){
    string *temp = ls->words;
    if(ls->wordsCount - ls_null(ls)){
        printf("%d\n", ls->wordsCount - ls_null(ls));
    }
    for (int i = 0; i < ls->wordsCount; ++i) {
        if(temp[i] != NULL){
            printf("%s\n", temp[i]);
        }
    }
}

void ls_sort(list ls){
    string *words = ls->words;
    string temp;
    for (int i = ls->wordsCount - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if(words[j] != NULL && words[j + 1] != NULL
            && strcmp(words[j], words[j + 1]) > 0){
                temp = words[j];
                words[j] = words[j + 1];
                words[j + 1] = temp;
            } else if(words[j] == NULL){
                words[j] = words[j + 1];
                words[j + 1] = NULL;
            }
        }
    }
}

void ls_clear(list ls){
	if(ls == NULL){
		return;
	}

    string *temp = ls->words;
	if(temp == NULL){
		ls->wordsCount = 0;
		return;
	}

    for (int i = 0; i < ls->wordsCount; ++i) {
        if(temp[i] != NULL){
            free(temp[i]);
        }
    }
    free(ls->words);
    ls->words = NULL;
    ls->wordsCount = 0;
}

void ls_delete(list ls){
    if(ls != NULL){
		ls_clear(ls);
        free(ls);
    }
    ls = NULL;
}
