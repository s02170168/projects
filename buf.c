#include <malloc.h>

#include "buf.h"
#include "list.h"
#include "symbols.h"
#include "error.h"

buf buf_make(){
    buf temp = (buf) malloc(sizeof(*temp));
    temp->word = NULL;
    temp->length = 0;
    temp->st = GOT_SPACE;
    return temp;
}

void buf_delete(buf buffer){
    if(buffer != NULL){
        free(buffer->word);
        free(buffer);
    }
}

err_type buf_get_sym(buf buffer, list words, char c, err_type err){
    
}