#ifndef _QUEUE_H_
#define _QUEUE_H_

#if (__STDC_VERSION__ != 201112L) || (__STDC_VERSION__ != 199901L)
typedef enum {false, true} bool;
#endif

#define MAX_QUEUE 10
typedef int Item;

typedef struct node {
    Item item;
    struct node *next;
} Node;

typedef struct queue {
    Node *front;
    Node *rear;
    int items;
} Queue;

void initialize_queue(Queue *pq);
bool queue_is_full(const Queue *pq);
bool queue_is_empty(const Queue *pq);
int queue_item_count(const Queue *pq);

//队列末尾添加项
bool en_queue(Item item, Queue *pq);

//队列开头删除项
bool de_queue(Item *pitem, Queue *pq);

void empty_queue(Queue *pq);
#endif
