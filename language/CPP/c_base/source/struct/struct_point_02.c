/**
 * 使用指针和malloc
 **/

#include <stdio.h>
#include <string.h>

struct namect {
    char *fname;
    char *lname;
    int letters;
};

void getinfo(struct namect *);
void makeinfo(struct namect *);
void showinfo(const struct namect *);
void cleanup(struct namect *);

int main(void)
{
    struct namect person;

    getinfo(&person);
    makeinfo(&person);
    showinfo(&person);
    cleanup(&person);

    return 0;
}

void getinfo(struct namect *pst)
{
    char tmp[81];

    printf("please enter yor first name.\n");

    gets(tmp);
    pst->fname = (char *)malloc(strlen(tmp) + 1);
    strcpy(pst->fname, tmp);

    printf("please enter your last name\n");
    gets(tmp);
    pst->lname = (char *)malloc(strlen(tmp) + 1);
    strcpy(pst->lname, tmp);
}

void makeinfo(struct namect *pst)
{
    pst->letters = strlen(pst->fname) +
        strlen(pst->lname);
}

void showinfo(const struct namect *pst)
{
    printf("%s %s, your name contains %d letters\n",
            pst->fname, pst->lname, pst->letters);
}

void cleanup(struct namect *pst)
{
    free(pst->fname);
    free(pst->lname);
}
