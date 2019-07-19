#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void show_movies(Item item);
char *s_gets(char *st, int n);

int main(void)
{
    List movies;
    Item temp;
    
    /* 初始化 */
    initialize_list(&movies);
    if (list_is_full(&movies)) {
        fprintf(stderr, "No memory available! Bye!\n");
        exit(1);
    }

    /* 获取用户输入并存储 */
    puts("Enter first movie title:");
    while (s_gets(temp.title, TSIZE) != NULL &&
            temp.title[0] != '\0') {
        puts("Enter your rating <0-10>:");
        scanf("%d", &temp.rating);
        while (getchar() != '\n')
            continue;

        if (add_item(temp, &movies) == false) {
            fprintf(stderr, "Problem allocating memory\n");
            break;
        }

        if (list_is_full(&movies)) {
            puts("The list is now full.");
            break;
        }

        puts("Enter next movie title (empty line to stop)");
    }
    
    /* 显示 */
    if (list_is_empty(&movies)) {
        printf("No dat entered");
    } else {
        printf("Here is the movie list:\n");
        traverse(&movies, show_movies);
    }
    printf("You entered %d movies.\n", list_item_count(&movies));

    /* 清理 */
    empty_list(&movies);
    printf("Bye!\n");
    
    return 0;
}

void show_movies(Item item)
{
    printf("Movie: %s Rating: %d\n", item.title, item.rating);
}

char *s_gets(char *st, int n)
{
    char *ret_val;
    char *find;
    ret_val = fgets(st, n, stdin);
    if(ret_val)
    {
        find = strchr(st, '\n');
        if(find)
            *find = '\0';
        else
            while(getchar() != '\n')
                continue;
    }

    return ret_val;
}
