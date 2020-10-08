#ifndef __LIST_H__
#define __LIST_H__

typedef struct list_elmt_ {
    void *data;
    struct list_elmt_ *next;
} list_elmt;

typedef struct list_ {
    int size;
    int (*match)(const void *key1, const void *key2);
    void (*destory)(void *data);
    list_elmt *head;
    list_elmt *tail;
} list_t;

/**
 * 初始化由list指定的链表
 * 复杂度为O(1)
 * */
void list_init (list_t *list, void (*destory)(void *data));

/**
 * 销毁由list指定的链表,list_init 传入NULL，或者该函数指针
 * 复杂度为O(n)
 * */
void list_destory (list_t *list);

/**
 * 如果成功插入元素成功返回0，否则返回-1
 * 复杂度为O(1)
 * */
int list_ins_next(list_t *list, list_elmt *element, const void *data);

/**
 * 如果删除成功返回0，否则返回-1
 * 复杂度为O(1)
 * */
int list_rem_next(list_t *list, list_elmt *element, void **data);

/**
 * 返回元素中的个数
 * 复杂度为O(1)
 * */
#define LIST_SIZE(list) ((list)->size)

/**
 * 指向链表中头元素的指针
 * 复杂度为O(1)
 * */
#define LIST_HEAD(list) ((list)->head)

/**
 * 指向链表中尾元素
 * 复杂度为O(1)
 * */
#define LIST_TAIL(list) ((list)->tail)

/**
 * 如果element所指定的元素是链表头节点返回1，否则返回0
 * 复杂度为O(1)
 * */
#define LIST_IS_HEAD(list, element) ((element) == (list)->head ? 1 : 0)


/**
 * 如果element所指定的元素是链表末尾节点则返回1，否则返回0
 * 复杂度为O(1)
 * */
#define LIST_IS_TAIL(element) ((element)->next == NULL ? 1 : 0)

/**
 * 节点中保存的数据
 * 复杂度为O(1)
 * */
#define LIST_DATA(element) ((element)->data)

/**
 * 返回element所指定的节点下一个节点
 * 复杂度为O(1)
 * */
#define LIST_NEXT(element) ((element)->next)
#endif /* __LIST_H__ */
