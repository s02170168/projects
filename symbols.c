#include "symbols.h"

const char specials[SPECIAL_COUNT] = {
<<<<<<< HEAD
        '>', '|', '&',
        '<', ';', '(',
        ')',
};

int sym_type(char c) {
    if (c == '\t' || c == ' ') {
        return SPACE;
    }
    if (c == '\n') {
        return NEWLINE;
    }
    for (int i = 0; i < SPECIAL_COUNT; ++i) {
        if (c == specials[i]) {
            return SPECIAL;
        }
    }
    if (c == '#') {
        return SHARP;
    }
    if (c == '"') {
        return QUOTE;
    }
    if (c == '\\') {
        return BACKSLASH;
    }
    return SIMPLE;
}

int sym_dup(char c1, char c2) {
    if ((c1 == c2) && ((c1 == '>') || (c1 == '|') || (c1 == '&'))) {
        return 1;
    }
    return 0;
}
=======
        '>',
        '|',
        '&',
        '<',
        ';',
        '(',
        ')'
};

int sym_type(char c){
    if(('a' <= c && c <= 'z') ||
       ('A' <= c && c <= 'Z') ||
       ('0' <= c && c <= '9') ||
       (c == '$') || (c == '/') || (c == '.') || (c == '_')){
        return SIMPLE;
    }
    if(c == '\t' || c == ' '){
        return SPACE;
    }
    if(c == '\n'){
        return NEWLINE;
    }
    for (int i = 0; i < SPECIAL_COUNT; ++i) {
        if(c == specials[i]){
            return SPECIAL;
        }
    }
    return INCORRECT;
}

int sym_dup(char c1, char c2){
    if( (c1 == c2) && ((c1 == '>') || (c1 == '|') || (c1 == '&')) ){
        return 1;
    }
    return 0;
}
>>>>>>> craft_table
