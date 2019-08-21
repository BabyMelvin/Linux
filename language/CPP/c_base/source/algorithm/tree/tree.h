#ifndef _TREE_H_
#define _TREE_H_

#include <stdbool.h>

#define SLEN 20
#define MAXITEMS 10

typedef struct item
{
    char petname[SLEN];
    char petkind[SLEN];
} Item;

typedef struct trnode
{
    Item item;
    struct trnode *left;
    struct trnode *right;
}  Trnode;

typedef struct tree
{
    Trnode *root;
    int size;
} Tree;

void initialize_tree(Tree *ptree);
bool tree_is_empty(const Tree *ptree);
bool tree_is_full(const Tree *ptree);
int tree_item_count(const Tree *ptree);

bool add_item(const Item *pi, Tree *ptree);
bool in_tree(const Item *pi, const Tree *ptree);
void traverse(const Tree *ptree, void (*pfun)(Item item));
void delete_all(Tree *ptree);

#endif
