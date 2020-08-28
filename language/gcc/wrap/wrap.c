#include <stdio.h>
#include <stdlib.h>

// 只声明不定义 __real_malloc
void * __real_malloc (size_t size);

void *__wrap_malloc (size_t size)
{
    printf("__wrap_malloc called, size:(%d)\n", size);

    // 通过__real_malloc调用正真的malloc
    return __real_malloc(size);
}
