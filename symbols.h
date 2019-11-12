enum {
    INCORRECT = 0,
    SIMPLE = 1,
    SPACE = 2,
    NEWLINE = 3,
    SPECIAL = 4,
    SPECIAL_COUNT = 7,
};

int sym_type(char);

int sym_dup(char, char);