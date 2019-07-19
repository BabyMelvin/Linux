/**
 * 使用结构链表
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TSIZE 45

struct film {
    char title[TSIZE];
    int rating;
    struct film *next;
};

char *s_gets(char *st, int n);

int main(void)
{
    struct film *head = NULL;
    struct film *prev, *current;

    char input[TSIZE];
    puts("Enter first movie title:");
    while (s_gets(input, TSIZE) != NULL && input[0] != '\0') {
        current = (struct film*)malloc(sizeof(struct film));
        if (head == NULL)
            head = current;
        else
            prev->next = current;

        current->next = NULL;
        strcpy(current->title, input);
        puts("Enter your rating <0-10>:");
        scanf("%d", &current->rating);
        while(getchar() != '\n')
            continue;

        puts("Enter next movie title (empty line to stop)");
        prev = current;
    }

    if (head == NULL)
        printf("No data entered");
    else
        printf("Here is the movie list:\n");

    current = head;
    while(current != NULL) {
        printf("Movie:%s Rating: %d\n",
                current->title, current->rating);
        current = current->next;
    }

    /* 释放工作 */
    current = head;
    while(current != NULL) {
        head = current->next;
        free(current);
        //TODO 这里要放下面,表示下一个
        current = head;
    }

    printf("Bye.!\n");

    return 0;
}

char *s_gets(char *st, int n)
{
    char *ret_val;
    char *find;
    ret_val = fgets(st, n, stdin);
    if (ret_val) {
        find = strchr(st, '\n');
        if(find)
            *find = '\0';
        else
            while(getchar() != '\n')
                continue;
    }

    return ret_val;
}

