#include "clist.h"
#include "page.h"

/**
 * current 指向循环链表中的一个元素
 * */
int replace_page(CListElmt **current)
{
    /*Circle through the list of pages until one is found to replace*/
    while (((Page *)(*current)->data)->reference != 0) {
        ((Page *)(*current)->data)->reference = 0;
        *current = clist_next(*current);
    }

    return ((Page *)(*current)->data)->number;
}
