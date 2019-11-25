#ifndef SYMBOLS_H
#define SYMBOLS_H

enum {
    INCORRECT = 0,
    SIMPLE = 1,
    SPACE = 2,
    NEWLINE = 3,
    SPECIAL = 4,
    SHARP = 5,
    QUOTE = 6,
    BACKSLASH = 7,
    SPECIAL_COUNT = 7
};

int sym_type(char);

int sym_dup(char, char);

#endif