#ifndef __CLIST_H__
#define __CLIST_H__

#include <stdlib.h>

typedef struct clist_elmt_ {
    void *data;
    struct clist_elmt_ *next;
} clist_elmt_t;

typedef struct clist_ {
    int size;
    int (*match)(const void *key1, const void *key2);
    void (*destory)(void *data);
    clist_elmt_t *head;
} clist_t;

/*Public Interface*/
void clist_init(clist_t *list, void (*destory)(void *data));
void clist_destory(clist_t *list);
int clist_ins_next(clist_t *list, clist_elmt_t *element, const void *data);
int clist_rem_next(clist_t *list, clist_elmt_t *element, void **data);

#define clist_size(list) ((list)->size)
#define clist_head(list) ((list)->head)
#define clist_data(element) ((element)->data)
#define clist_next(element) ((element)->next)
#endif /* __CLIST_H__ */
