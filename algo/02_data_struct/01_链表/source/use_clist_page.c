#include "clist.h"
#include "page.h"

/**
 * current 指向循环链表中的一个元素
 * */
int replace_page(clist_elmt_t **current)
{
    /*Circle through the list of pages until one is found to replace*/
    while (((page_t *)(*current)->data)->reference != 0) {
        ((page_t *)(*current)->data)->reference = 0;
        *current = clist_next(*current);
    }

    return ((page_t *)(*current)->data)->number;
}
