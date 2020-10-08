#include <stdlib.h>
#include <string.h>
#include "dlist.h"

void dlist_init(dlist_t *list, void (*destory)(void *data))
{
    list->size = 0;
    list->destory = destory;
    list->head = NULL;
    list->tail = NULL;

    return;
}

void dlist_destory(dlist_t *list)
{
    void *data;

    /*Remove each element*/
    while (DLIST_SIZE(list) > 0) {
        if (dlist_remove(list, DLIST_TAIL(list), (void **)&data) == 0
                && list->destory != NULL) {
            /*Call a user-defined function to free dynamically allocated data.*/
            list->destory(data);
        }
    }

    /*No operations are allowed now,but clear the structure as a precation*/
    memset(list, 0, sizeof(dlist_t));

    return;
}

int dlist_ins_next(dlist_t *list, dlist_elmt_t *element, const void *data)
{
    dlist_elmt_t *new_element;

    /*Do not allow a NULL element unless the list empty*/
    if (element == NULL && DLIST_SIZE(list) != 0) {
        return -1;
    }

    /*Allocate storage for the element*/
    if (new_element = (dlist_elmt_t *)malloc(sizeof(dlist_elmt_t)) == NULL) {
        return -1;
    }

    /*insert the new element into the list*/
    if (DLIST_SIZE(list) == 0) {
        /*Handle insertion when the list is empty*/
        list->head = new_element;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_element;
    } else {
        /*Handle insertion when the list is not empty*/
        new_element->next = element->next;
        new_element->prev = element;

        if (element->next == NULL) {
            list->tail = new_element;
        } else {
            element->next->prev = new_element;
        }

        element->next = new_element;
    }

    /*adjust the size of the list to account for the inserted element*/
    list->size ++;
    return 0;
}

int dlist_ins_prev(dlist_t *list, dlist_elmt_t *element, const void *data)
{
    dlist_elmt_t *new_element;

    /*Do not allow a NULL element unless the list is empty*/
    if (element == NULL && DLIST_SIZE(list) != 0)
        return -1;

    /*Allocate storage to be managed by the abstrate datatype*/
    if ((new_element = (dlist_elmt_t *)malloc(sizeof(dlist_elmt_t))) == NULL)
        return -1;

    /*Insert the new element into the list*/
    new_element->data = (void *)data;
    if (DLIST_SIZE(list) == 0) {
        /*Handle insertion when the list is empty*/
        list->head = new_element;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_element;
    } else {
        /*Handle insertion when the list is not empty*/
        new_element->next = element;
        new_element->prev = element->prev;

        if (element->prev == NULL)
            list->head = new_element;
        else
            element->prev->next = new_element;

        element->prev =new_element;
    }

    /*Adjust the size of the list to account for the new element*/
    list->size ++;

    return 0;
}

int dlist_remove(dlist_t *list, dlist_elmt_t *element, void **data)
{
    /*Do not allow a NULL element or removal from an empty list*/
    if (element == NULL || DLIST_SIZE(list) == 0)
        return -1;

    /*Remove the element from the list*/
    *data = element->data;

    if (element == list->head) {
        /*Handle removal from the head of the list*/
        list->head = element->next;
        if (list->head == NULL)
            list->tail = NULL;
        else
            element->next->prev = NULL;
    } else {
        /*Handle removal from other than the head of the list*/
        element->prev->next = element->next;
        if (element->next == NULL)
            list->tail = element->prev;
        else
            element->next->prev = element->prev;
    }
    /*Free the storage allocated by the abstrate datatype*/
    free(element);

    /*Adjust the size of the list to account for the removal element*/
    list->size --;
    return 0;
}
