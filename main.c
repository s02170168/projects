#include <stdio.h>
#include <setjmp.h>
#include "buf.c"

jmp_buf begin;
const char invitation[] = "==>";

int main(void) {
    setjmp(begin);

    list words = ls_make();
    buf *buffer = buf_make();
    int trouble = 0;
    int sym_is_got = 0;

	char c;
	static char c_prev = '\n';

	if(c_prev == '\n'){
	    printf(invitation);
	}
	while((c = getchar()) != EOF){
	    sym_is_got = 1;
        buf_getlex(c, words, buffer, &trouble);
        c_prev = c;
        if(trouble == 2){
            printf("Некорректный символ во входной строке\n");
            ls_delete(words);
            free(buffer);
            longjmp(begin, 1);
        }
        if(trouble == 3){
            printf("Ошибка выделения динамической памяти\n");
            ls_delete(words);
            free(buffer);
            longjmp(begin, 1);
        }
        if(c_prev == '\n'){
            printf(invitation);
            sym_is_got = 0;
        }
    }

	if((c == EOF && c_prev != '\n') || !sym_is_got){
	    putchar('\n');
	}

    if(trouble == 1){
        printf("Некорректный символ во входной строке\n");
        ls_clear(words);
    }

    if(buffer->size){
		c = '\n';
        buf_getlex(c, words, buffer, &trouble);
        if(trouble == 2){
            printf("Некорректный символ во входной строке\n");
            ls_clear(words);
            ls_delete(words);
            free(buffer);
            longjmp(begin, 1);
        }
        if(trouble == 3){
            printf("Ошибка выделения динамической памяти\n");
            ls_clear(words);
            ls_delete(words);
            free(buffer);
            longjmp(begin, 1);
        }
    }

	if(words->wordsCount){
		ls_sort(words);
		ls_print(words);
		ls_clear(words);
	}

    free(buffer);
    ls_delete(words);
    return 0;
}
