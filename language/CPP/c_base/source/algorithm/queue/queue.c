#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

static void copy_to_node(Item item, Node *pn);
static void copy_to_item(Node *pn, Item *pi);

void initialize_queue(Queue *pq)
{
   pq->front = NULL; 
   pq->rear = NULL;
   pq->items = 0;
}

bool queue_is_full(const Queue *pq)
{
    return pq->items >= MAX_QUEUE ? true:false;
}

bool queue_is_empty(const Queue *pq)
{
    return pq->items == 0;
}

int queue_item_count(const Queue *pq)
{
    return pq->items;
}

bool en_queue(Item item, Queue *pq)
{
    Node *pnew;
    if(queue_is_full(pq))
        return false;

    pnew = (Node *)malloc(sizeof(Node));
    if (pnew == NULL) {
        fprintf(stderr, "Unable to allocate memory!\n");
        return false;
    }

    copy_to_node(item, pnew);
    if (queue_is_empty(pq))
        pq->front = pnew;
    else
        pq->rear->next = pnew;

    pq->rear = pnew;
    pq->items ++;

    return true;
}

bool de_queue(Item *pitem, Queue *pq)
{
    Node *pt;
    if(queue_is_empty(pq))
        return false;

    copy_to_item(pq->front, pitem);

    pt = pq->front;
    pq->front = pq->front->next;
    free(pt);

    pq->items --;

    if (pq->items == 0)
        pq->rear = NULL;

    return true;
}

void empty_queue(Queue *pq)
{
    Item dummy;
    while(!queue_is_empty(pq))
        de_queue(&dummy, pq);
}

static void copy_to_node(Item item, Node *pn)
{
    pn->item = item;
}

static void copy_to_item(Node *pn, Item *pi)
{
    *pi = pn->item;
}
