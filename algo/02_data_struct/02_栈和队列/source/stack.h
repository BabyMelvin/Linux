#ifndef __STACK_H__
#define __STACK_H__
#include <stdlib.h>
#include "list.h"

typedef list_t stack_t;

#define stack_init list_init
#define stack_destory list_destory

int stack_push(stack_t *stack, const void *data);
int stack_pop(stack_t *stack, void **data);

// 栈顶部元中存储的数据，如果栈为空，则返回NULL
#define stack_peek(stack) ((stack)->head == NULL ? NULL : (stack)->head->data)
#define stack_size list_size
#endif /* __STACK_H__ */
