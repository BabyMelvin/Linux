#include "clist.h"

typedef struct Page_ {
    int number;
    int reference;
} Page;

int replace_page(CListElmt **current);
