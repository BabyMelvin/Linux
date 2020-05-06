#include <stdlib.h>
#include <string.h>
#include "clist.h"

void clist_init(CList *list, void (*destory)(void *data))
{
    list->size = 0;
    list->destory = destory;
    list->head = NULL;

    return;
}

void clist_destory (CList *list)
{
    void *data;

    while (clist_size(list) > 0) {
        if (clist_rem_next(list, list->head, (void **)&data) == 0
                && list->destory != NULL)
            list->destory(data);
    }

    memset(list, 0, sizeof(CList));

    return;
}

int clist_ins_next (CList *list, CListElmt *elemnt, const void *data)
{
    CListElmt *new_element;

    if ((new_element = (CListElmt *)malloc(sizeof(CListElmt))) == NULL)
        return -1;

    new_element->data = (void *)data;
    if (clist_size(list) == 0) {
        new_element->next = new_element;
        list->head = new_element;
    } else {
        new_element->next = element->next;
        element->next = new_element;
    }

    list->size ++;

    return 0;
}

int clist_rem_next (CList *list, CListElmt *element, void **data)
{
    CListElmt *old_element;

    if (clist_size(list) == 0)
        return -1;

    *data = element->next->data;
    if (element->next == element) {
        old_element = element->next;
        list->head = NULL;
    } else {
        old_element = element->next;
        element->next = element->next->next;
        if (old_element == clist_head(list))
            list->head = old_element->next;
    }

    free(old_element);
    list_size --;

    return 0;
}
