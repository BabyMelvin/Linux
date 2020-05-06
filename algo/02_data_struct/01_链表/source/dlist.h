#ifndef __DLIST_H__
#define __DLIST_H__

typedef struct DListElmt_ {
    void *data;
    struct DListElmt_ *prev;
    struct DListElmt_ *next;
} DListElmt;

typedef struct DList_ {
    int size;
    int (*match) (const void *key1, const void *key2);
    void (*destory) (void *data);
    DListElmt *head;
    DListElmt *tail;
} DList;

/**
 * 初始化dlist
 * */
void dlist_init(DList *list, void (*destory)(void *data));

/**
 * 销毁list所指定的双向链表
 * */
void dlist_destory(DList *list);

/**
 * 插入成功返回1，否则返回-1
 * */
int dlist_ins_next(DList *list, DListElmt *element, const void *data);

/**
 * 将元素掺入由list指定双向链表中element元素之前。
 *  操作成功返回0，否则返回-1
 * */
int dlist_ins_prev(DList *list, DListElmt *element, const void *data);

/**
 * 有list指定双向链表中移除由element所指定的元素
 * 成功返回0, 否则返回-1
 * */
int dlist_remove(DList *list, DListElmt *element, void **data);

/**
 * 返回链表中元素的个数
 * */
#define dlist_size(list) ((list)->size)
#define dlist_head(list) ((list)->head)
#define dlist_tail(list) ((list)->tail)

/**
 * 是链表头则返回1，否则返回0
 * */
#define dlist_is_head(element) ((element)->prev == NULL ? 1 : 0)

/**
 * 是链表尾元素则返回1，否则返回0
 * */
#define dlist_is_tail(element) ((element)->next == NULL ? 1: 0)

/**
 * 返回element所指定的链表元素的数据域
 * */
#define dlist_data(element) ((element)->data)

/**
 * 返回由参数element所指定的链表元素的后继元素
 * */
#define dlist_next(element) ((element)->next)

/**
 * 返回由element所指定的元素的下一个元素
 * */
#define dlist_prev(element) ((element)->prev)
#endif /* __DLIST_H__ */
