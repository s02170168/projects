#include "myShellHeaders.h"
#include "buf.h"
#include "list.h"

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
                    if ((res = buf_addToList(buffer, words, special)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    if ((res = buf_addSym(buffer, '\\', GOT_BACK_DOUBLE_QUOTE)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
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
                    if ((res = buf_addToList(buffer, words, simple)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_SPECIAL);
                    break;
                case GOT_SPACE:
                    res = buf_addSym(buffer, c, GOT_SPECIAL);
                    break;
                case GOT_SHARP:
                    break;
                case GOT_SPECIAL:
                    if (sym_dup(c, buffer->word[0])) {
                        if ((res = buf_addSym(buffer, c, GOT_SPECIAL)) != no_err) { return res; }
                        res = buf_addToList(buffer, words, special);
                    } else {
                        if ((res = buf_addToList(buffer, words, special)) != no_err) { return res; }
                        res = buf_addSym(buffer, c, GOT_SPECIAL);
                    }
                    break;
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    if ((res = buf_addSym(buffer, '\\', GOT_BACK_DOUBLE_QUOTE)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                default:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
            }
            break;

        case SPACE:
            switch (st) {
                case GOT_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    if ((res = buf_addSym(buffer, '\\', GOT_BACK_DOUBLE_QUOTE)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_BACK_SIMPLE:
                case GOT_SIMPLE:
                    res = buf_addToList(buffer, words, simple);
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
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
                case GOT_DOUBLE_QUOTE:
                case GOT_BACK_DOUBLE_QUOTE:
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
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    if ((res = buf_addSym(buffer, '\\', GOT_BACK_DOUBLE_QUOTE)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
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
                case GOT_QUOTE:
                    buffer->st = GOT_SIMPLE;
                    break;
                case GOT_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    if ((res = buf_addSym(buffer, '\\', GOT_BACK_DOUBLE_QUOTE)) != no_err) { return res; }
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
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

        case DOUBLE_QUOTE:
            switch (st) {
                case GOT_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_DOUBLE_QUOTE:
                    buffer->st = GOT_SIMPLE;
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                case GOT_SPECIAL:
                    res = buf_addToList(buffer, words, special);
                    buffer->st = GOT_DOUBLE_QUOTE;
                    break;
                case GOT_BACK_SIMPLE:
                    res = buf_addSym(buffer, c, GOT_SIMPLE);
                    break;
                case GOT_SHARP:
                    break;
                default:
                    buffer->st = GOT_DOUBLE_QUOTE;
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
                case GOT_QUOTE:
                    res = buf_addSym(buffer, c, GOT_QUOTE);
                    break;
                case GOT_DOUBLE_QUOTE:
                    buffer->st = GOT_BACK_DOUBLE_QUOTE;
                    break;
                case GOT_BACK_DOUBLE_QUOTE:
                    res = buf_addSym(buffer, c, GOT_DOUBLE_QUOTE);
                    break;
                default:
                    buffer->st = GOT_BACK_SIMPLE;
                    break;
            }
            break;
    }

    return res;
}