typedef char* string;

struct list{
    string* words;
    int wordsCount;
};

typedef struct list* list;

list ls_make(void);

void ls_add(list, string);

void ls_sort(list);

void ls_print(list);

void ls_clear(list);

void ls_delete(list);