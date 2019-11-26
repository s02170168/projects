<<<<<<< HEAD
#include "myShellHeaders.h"
#include "buf.h"
#include "list.h"
#include "symbols.h"

buf buf_make() {
    buf temp = (buf) malloc(sizeof(*temp));
    if (temp == NULL) {
        return NULL;
    }
    temp->word = NULL;
    temp->length = 0;
    temp->st = GOT_SPACE;
    return temp;
}

void buf_clear(buf buffer, state st) {
    if (buffer != NULL) {
        if (buffer->word != NULL) {
            free(buffer->word);
            buffer->word = NULL;
        }
        buffer->length = 0;
        buffer->st = st;
    }
}

void buf_delete(buf buffer) {
    if (buffer != NULL) {
        buf_clear(buffer, GOT_SPACE);
        free(buffer);
    }
}

err_type buf_addSym(buf buffer, char c, state st) {
    int length = buffer->length;
    string w_ptr = buffer->word;

    if (length) {
        w_ptr = (string) realloc(w_ptr, (length + 1) * sizeof(c));
    } else {
        w_ptr = (string) malloc(sizeof(c));
    }

    if (w_ptr == NULL) {
        return allocation;
    } else {
        buffer->word = w_ptr;
    }

    buffer->word[length] = c;
    buffer->st = st;
    buffer->length++;
    return no_err;
}

err_type buf_addToList(buf buffer, list words, word_type type) {
    if (!buffer->length) {
        buffer->st = GOT_SPACE;
        return no_err;
    }
    err_type res = buf_addSym(buffer, 0, GOT_SPACE);
    if (res != no_err) {
        return res;
    }
    res = ls_add(words, buffer->word, type);
    if (res != no_err) {
        return res;
    }
    buffer->word = NULL;
    buf_clear(buffer, GOT_SPACE);
    return no_err;
}

err_type buf_getSym(buf buffer, list words, char c, err_type err) {
    err_type res = err;
    state st = buffer->st;

    switch (sym_type(c)) {
        case SIMPLE:
            switch (st) {
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    if (res != no_err) {
                        return res;
                    }
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_QUOTE:
                case GOT_BACK_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_SHARP:
                    break;
                default:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
            }
            break;

        case SPECIAL:
            switch (st) {
                case GOT_SIMPLE:
                    res = buf_addToList(buffer, words, simple);
                    if (res != no_err) {
                        return res;
                    }
                    res = buf_addSym(buffer, c, GOT_SPECIAL);
                    break;
                case GOT_SPACE:
                    res = buf_addSym(buffer, c, GOT_SPECIAL);
                    break;
                case GOT_SHARP:
                    break;
                case GOT_SPECIAL:
                    if (sym_dup(c, buffer->word[0])) {
                        res = buf_addSym(buffer, c, GOT_SPECIAL);
                        if (res != no_err) {
                            return res;
                        }
                        res = buf_addToList(buffer, words, special);
                    } else {
                        res = buf_addToList(buffer, words, special);
                        if (res != no_err) {
                            return res;
                        }
                        res = buf_addSym(buffer, c, GOT_SPECIAL);
                    }
                    break;
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_BACK_QUOTE:
                case GOT_QUOTE:
                default:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
            }
            break;

        case SPACE:
            switch (st) {
                case GOT_QUOTE:
                case GOT_BACK_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_BACK_SIMPLE:
                case GOT_SIMPLE:
                    res = buf_addToList(buffer, words, simple);
                    break;
                default:
                    break;
            }
            break;

        case NEWLINE:
            switch (st) {
                case GOT_SIMPLE:
                case GOT_BACK_SIMPLE:
                    res = buf_addToList(buffer, words, simple);
                    break;
                case GOT_QUOTE:
                case GOT_BACK_QUOTE:
                    res = syntax;
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    break;
                default:
                    buffer->st = GOT_SPACE;
                    break;
            }
            break;

        case SHARP:
            switch (st) {
                case GOT_QUOTE:
                case GOT_BACK_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_SHARP:
                    break;
                case GOT_SPACE:
                    buffer->st = GOT_SHARP;
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    buffer->st = GOT_SHARP;
                    break;
                case GOT_SIMPLE:
                default:
                    res = buf_addToList(buffer, words, simple);
                    buffer->st = GOT_SHARP;
                    break;
            }
            break;

        case QUOTE:
            switch (st) {
                case GOT_BACK_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_QUOTE:
                    buffer->st = GOT_SIMPLE;
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    buffer->st = GOT_QUOTE;
                    break;
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_SHARP:
                    break;
                default:
                    buffer->st = GOT_QUOTE;
                    break;
            }
            break;

        case BACKSLASH:
            switch (st) {
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    buffer->st = GOT_BACK_SIMPLE;
                    break;
                case GOT_SHARP:
                    break;
                case GOT_SIMPLE:
                    buffer->st = GOT_BACK_SIMPLE;
                    break;
                case GOT_BACK_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_QUOTE:
                    buffer->st = GOT_BACK_QUOTE;
                    break;
                default:
                    buffer->st = GOT_BACK_SIMPLE;
                    break;
            }
            break;
    }

    return res;
}
=======
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
>>>>>>> craft_table
