/* 简单链表类型 */
#ifndef LIST_H_
#define LIST_H_

typedef enum {false, true} bool;

#define TSIZE 45

struct film {
    char title[TSIZE];
    int rating;
};

typedef struct film Item;

typedef struct node
{
    Item item;
    struct node *next;
} Node;

//typedef Node *List;

typedef struct list
{
    Node *head;
    int size;
} List;

void initialize_list(List *plist);
bool list_is_empty(const List *plist);
bool list_is_full(const List *plist);
unsigned int list_item_count(const List *plist);
bool add_item(Item item, List *plist);

//把函数作用于链表的每一项
void traverse(const List *list, void(*pfun)(Item item));

/*释放已经分配的内存*/
void empty_list(List *list);
#endif
