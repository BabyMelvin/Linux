#ifndef LIST_H
#define LIST_H

#define LIST_POISON1 (void *)0x00100100)
#define LIST_POISON2 (void *)0x00200200)
struct list_head {
	struct list_head *next, *prev;
};

/***********************链表操作成员****************/
#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

/* 通过成员返回整个结构体 
*  ptr: 成员指针
*  type: 整个结构体
*  member:该成员在结构体的变量名称
*/

#define container_of(ptr, type, member) ({\
	const typeof(((type *)0)->member) *__mptr = (ptr); \
	(type *)((char *)__mptr - offsetof(type, member));})

/* 链表头初始化 */
#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name);

/* 获得该入口点的整个结构体*/
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/* 通过给定的类型迭代
*  pos:  type * 迭代cursor
*  head: 链表的头
*  member: 结构体中的成员名
*/
#define list_for_each_entry(pos ,head ,member) \
	for(pos=list_entry((head)->next,typeof(*pos),member); \
		&pos->member != (head); \
		pos = list_entry(pos->member.next, typeof(*pos), member))


/*************************链表：添、删、改、查***************************/
/*
* 1.看链表是否为空
*/
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/*
* 2.添加
*/

static inline void __list_add(struct list_head *_new, 
				struct list_head *prev, 
				struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

static inline void list_add(struct list_head *_new ,struct list_head *head)
{
	__list_add(_new, head, head->next);
}

/*
* 3.删除
*/
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = 
	entry->prev = 
}

/*
* 4.修改
*/

static inline void list_replace(struct list_head *old,
				struct list_head *_new)
{
	_new->next = old->next;
	_new->next->prev = _new;
	_new->prev = old->prev;
	_new->prev->next = _new;
}

/*
* 5.查
*/
#define list_for_each(pos, head) \
	for(pos = (head)->prev;pos!=(head);pos = pos->prev)


#endif /*LIST_H*/