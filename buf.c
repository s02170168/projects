#include "buf.h"
#include <malloc.h>

buf *buf_make(){
    buf *temp = (buf *) malloc(sizeof(*temp));
    temp->size = 0;
    temp->str = NULL;
    temp->state = GOT_SPACE;
}

void buf_in(buf *buffer, char *block, int size, enum state v){
    buffer->str = block;
    buffer->size = size;
    buffer->state = v;

    block = NULL;
}

void buf_out(buf *buffer, char **block, int *size, enum state *v){
    *block = buffer->str;
    *size = buffer->size;
    *v = buffer->state;

    buffer->str = NULL;
    buffer->size = 0;
}

void err_overflow(void *block1, void* block2, int *st){
    block1 = block2;
    *st = 3;
}

void buf_getlex(char c, list words, buf *buffer, int *st){
    string temp = NULL;
    int len = 0;
    int maxLen = 0;
    enum state v = GOT_SPACE;

    buf_out(buffer, &temp, &maxLen, &v);
    len = maxLen;

    string tempptr;
    string *wordsptr;

	if(*st == 1){
        if(c == '\n'){
            *st = 2;
        }
        if(temp != NULL){
            free(temp);
            temp = NULL;
        }
        len = maxLen = 0;
        v = GOT_SPACE;
    } else switch (sym_type(c)) {
		case SIMPLE:
	        if (v == GOT_SPECIAL) {
				v = GOT_SPACE;
                tempptr = temp;
                temp = (string) realloc(temp, (len + 1) * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }

                temp[len] = 0;

                wordsptr = words->words;
                ls_add(words, temp);
                if(words->words == NULL){
                    err_overflow(words->words, wordsptr, st);
                    break;
                }
                temp = NULL;
                len = maxLen = 0;
            }
            if (v == GOT_SPACE) {
                v = GOT_WORD;
                len = 0;
                maxLen = MIN_LEN;
                tempptr = temp;
                temp = (string) malloc(maxLen * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }
            } else if (len == maxLen) {
                maxLen += MIN_LEN;
                tempptr = temp;
                temp = (string) realloc(temp, maxLen * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }
            }
            temp[len++] = c;
            break;
        case SPACE:
            if (v == GOT_WORD || v == GOT_SPECIAL) {
                v = GOT_SPACE;

                tempptr = temp;
                temp = (string) realloc(temp, (len + 1) * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }
                temp[len] = 0;

                wordsptr = words->words;
                ls_add(words, temp);
                if(words->words == NULL){
                    err_overflow(words->words, wordsptr, st);
                     break;
                 }
                 temp = NULL;
                 len = maxLen = 0;
            }
            break;

        case SPECIAL:
            if (v == GOT_WORD) {
                v = GOT_SPACE;

                tempptr = temp;
                temp = (string) realloc(temp, (len + 1) * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }

                temp[len] = 0;

                wordsptr = words->words;
                ls_add(words, temp);
                if(words->words == NULL){
                    err_overflow(words->words, wordsptr, st);
                    break;
                }

                temp = NULL;
                len = maxLen = 0;
            }
            if (v == GOT_SPECIAL) {
                if (sym_dup(c, temp[len - 1])) {
                    v = GOT_SPACE;

                    tempptr = temp;
                    temp = (string) realloc(temp, (len + 2) * sizeof(c));
                    if(temp == NULL){
                        err_overflow(temp, tempptr, st);
                        break;
                    }

                    temp[len] = c, temp[len + 1] = 0;

                    wordsptr = words->words;
                    ls_add(words, temp);
                    if(words->words == NULL){
                        err_overflow(words->words, wordsptr, st);
                        break;
                    }

                    temp = NULL;
                    len = maxLen = 0;
                } else {

                    tempptr = temp;
                    temp = (string) realloc(temp, (len + 1) * sizeof(c));
                    if(temp == NULL){
                        err_overflow(temp, tempptr, st);
                        break;
                    }

                    temp[len] = 0;

                    wordsptr = words->words;
                    ls_add(words, temp);
                    if(words->words == NULL){
                        err_overflow(words->words, wordsptr, st);
                        break;
                    }

                    len = 0, maxLen = MIN_LEN;

                    tempptr = temp;
                    temp = (string) calloc(maxLen, sizeof(c));
                    if(temp == NULL){
                        err_overflow(temp, tempptr, st);
                        break;
                    }

                    temp[len] = c;
                    len++;
                }
            } else{
                v = GOT_SPECIAL;
                len = 0, maxLen = MIN_LEN;

                tempptr = temp;
                temp = (string) calloc(maxLen, sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }
                temp[len] = c;
                len++;
            }
            break;

        case NEWLINE:
            if (v == GOT_WORD || v == GOT_SPECIAL) {
                v = GOT_SPACE;
                        
                tempptr = temp;
                temp = (string) realloc(temp, (len + 1) * sizeof(c));
                if(temp == NULL){
                    err_overflow(temp, tempptr, st);
                    break;
                }
                temp[len] = 0;

                wordsptr = words->words;
                ls_add(words, temp);
                if(words->words == NULL){
                    err_overflow(words->words, wordsptr, st);
                    break;
                }
                temp = NULL;
                len = maxLen = 0;
            }
            ls_sort(words);
            ls_print(words);
            ls_clear(words);
            break;
        case INCORRECT:
            *st = 1;
            break;
    }


    buf_in(buffer, temp, len, v);
}
