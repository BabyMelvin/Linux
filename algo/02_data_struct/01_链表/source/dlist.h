#ifndef __DLIST_H__
#define __DLIST_H__

typedef struct dlist_elmt_ {
    void *data;
    struct dlist_elmt_ *prev;
    struct dlist_elmt_ *next;
} dlist_elmt_t;

typedef struct d_list_ {
    int size;
    int (*match) (const void *key1, const void *key2);
    void (*destory) (void *data);
    dlist_elmt_t *head;
    dlist_elmt_t *tail;
} dlist_t;

/**
 * 初始化dlist
 * */
void dlist_init(dlist_t *list, void (*destory)(void *data));

/**
 * 销毁list所指定的双向链表
 * */
void dlist_destory(dlist_t *list);

/**
 * 插入成功返回1，否则返回-1
 * */
int dlist_ins_next(dlist_t *list, dlist_elmt_t *element, const void *data);

/**
 * 将元素掺入由list指定双向链表中element元素之前。
 *  操作成功返回0，否则返回-1
 * */
int dlist_ins_prev(dlist_t *list, dlist_elmt_t *element, const void *data);

/**
 * 有list指定双向链表中移除由element所指定的元素
 * 成功返回0, 否则返回-1
 * */
int dlist_remove(dlist_t *list, dlist_elmt_t *element, void **data);

/**
 * 返回链表中元素的个数
 * */
#define DLIST_SIZE(list) ((list)->size)
#define DLIST_HEAD(list) ((list)->head)
#define DLIST_TAIL(list) ((list)->tail)

/**
 * 是链表头则返回1，否则返回0
 * */
#define DLIST_IS_HEAD(element) ((element)->prev == NULL ? 1 : 0)

/**
 * 是链表尾元素则返回1，否则返回0
 * */
#define DLIST_IS_TAIL(element) ((element)->next == NULL ? 1: 0)

/**
 * 返回element所指定的链表元素的数据域
 * */
#define DLIST_HEAD(element) ((element)->data)

/**
 * 返回由参数element所指定的链表元素的后继元素
 * */
#define DLIST_NEXT(element) ((element)->next)

/**
 * 返回由element所指定的元素的下一个元素
 * */
#define DLIST_PREV(element) ((element)->prev)
#endif /* __DLIST_H__ */
