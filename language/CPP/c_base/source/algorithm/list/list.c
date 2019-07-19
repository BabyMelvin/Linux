#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define MAX_LIST_SIZE  20
static void copy_to_node(Item item, Node *pnode);

void initialize_list(List *plist)
{
    plist->head = NULL;
    plist->size = 0;
}

bool list_is_empty(const List *plist)
{
    return plist->size == 0 ? true : false;
}

bool list_is_full(const List *plist)
{
    return plist->size >= 20 ? true : false;
}

unsigned int list_item_count(const List *plist)
{
    return plist->size;
}

bool add_item(Item item, List *plist)
{
    Node *pnew;
    Node *scan;
    
    pnew = (Node *)malloc(sizeof(Node));
    if(pnew == NULL)
        return false;

    copy_to_node(item, pnew);
    pnew->next = NULL;

    if (plist->head == NULL)
        plist->head = pnew;
    else {
        scan = plist->head;
        while(scan->next != NULL)
            scan = scan->next;

        scan->next = pnew;
    }
    plist->size++;

    return true;
}
    
void traverse(const List *plist, void (*pfun)(Item item))
{
    Node *pnode = plist->head;
    while (pnode != NULL) {
        (*pfun)(pnode->item);
        pnode = pnode->next;
    }
}

void empty_list(List *plist)
{
    Node *psave;
    Node *phead = plist->head;
    psave = phead;

    while (psave != NULL) {
        psave = phead->next;
        free(psave);
        phead = psave;
    }
}

static void copy_to_node(Item item, Node *pnode)
{
    pnode->item = item;
}
