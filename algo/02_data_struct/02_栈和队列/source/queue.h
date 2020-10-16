#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdlib.h>
#include "list.h"

typedef list_t queue_t;

#define queue_init list_init
int queue_enqueue(queue_t *queue, const void *data);
int queue_dequeue(queue_t *queue, void **data);
#define queue_peek(queue) ((queue)->head == NULL ? NULL : (queue)->head->data)
#define queue_size list_size
#endif /* __QUEUE_H__ */
