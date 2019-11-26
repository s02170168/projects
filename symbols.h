<<<<<<< HEAD
#ifndef SYMBOLS_H
#define SYMBOLS_H

enum {
=======
enum {
    INCORRECT = 0,
>>>>>>> craft_table
    SIMPLE = 1,
    SPACE = 2,
    NEWLINE = 3,
    SPECIAL = 4,
<<<<<<< HEAD
    SHARP = 5,
    QUOTE = 6,
    BACKSLASH = 0,
    SPECIAL_COUNT = 7
=======
    SPECIAL_COUNT = 7,
>>>>>>> craft_table
};

int sym_type(char);

<<<<<<< HEAD
int sym_dup(char, char);

#endif
=======
int sym_dup(char, char);
>>>>>>> craft_table
