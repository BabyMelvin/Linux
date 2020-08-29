#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *c = (char *) malloc(sizeof(char));
    printf("c = %p\n", c);

    free(c);
    return 0;
}
