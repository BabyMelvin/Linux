#include <stdio.h>
//用到函数和names
#include "names_st.h"

int main(void)
{
    names candiate;
    get_names(&candiate);
    printf("Let's welcome ");
    show_names(&candiate);
    printf(" to this programe!\n");
    return 0;
}
