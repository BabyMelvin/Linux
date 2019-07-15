/**
 * 结构数组
 */
#include <stdio.h>
#define MAXTITL 40
#define MAXAUTL 40
#define MAXBKS 100

struct book {
    char title[MAXTITL];
    char author[MAXAUTL];
    float value;
};

int main(void)
{
    struct book library[MAXBKS];
    int count = 0;
    int index;

    printf("please enter the book title.\n");
    //'\0'检测是否空字符串
    while(count < MAXBKS && gets(library[count].title)!=NULL
            && library[count].title[0] != '\0') {
        printf("Now enter the author\n");
        gets(library[count].author);
        printf("Now enter the value\n");
        scanf("%f", &library[count++].value);

        //scanf忽略掉了空格和换行符,把\n放在输入流中，需要getchar()处理
        while(getchar() != '\n')
            continue;//清空输入行

        if(count < MAXBKS)
            printf("Enter the next title\n");
    }

    if(count > 0) {
        printf("Here is the list of your books:\n");
        for(index = 0; index < count; index++)
            printf("%s by %s: $%.2f\n", library[index].title,
                    library[index].author, library[index].value);
    } else {
        printf("No books ? too bad\n");
    }

    return 0;
}

