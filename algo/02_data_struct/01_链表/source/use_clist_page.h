#include "clist.h"

typedef struct page_ {
    int number;
    int reference;
} page_t;

int replace_page(clist_elmt_t **current);
