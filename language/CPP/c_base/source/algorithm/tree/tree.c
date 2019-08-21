#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

/* 局部类型 */
typedef struct pair {
    Trnode *parent;
    Trnode *child;
} Pair;

/* 局部函数 */
static Trnode *make_node(const Item *pi);
static bool to_left(const Item *i1, const Item *i2);
static bool to_right(const Item *l1, const Item *i2);
static void add_node(Trnode *new_node, Trnode *root);
static void in_order(const Trnode *root, void (*pfun)(Item item));
static Pair seek_item(const Item *pi, const Tree *ptree);
static void delete_node(Trnode **ptr);
static void delete_all_nodes(Trnode *ptr);

void initialize_tree(Tree *ptree)
{
    ptree->root = NULL;
    ptree->size = 0;
}

bool tree_is_empty(const Tree *ptree)
{
    return ptree->root == NULL ? true:false;
}

bool tree_is_full(const Tree *ptree)
{
    return ptree->size >= MAXITEMS ? true:false;
}

int tree_item_count(const Tree *ptree)
{
    return ptree->size;
}

bool add_item(const Item *pi, Tree *ptree)
{
    Trnode *new_node;
    if (tree_is_full(ptree)) {
        fprintf(stderr, "Tree is full\n");
        return false;
    }

    new_node = make_node(pi);
    if (new_node == NULL) {
        fprintf(stderr, "Couldn't create node.\n");
        return false;
    }

    if(seek_item(pi, ptree).child != NULL) {
        fprintf(stderr, "Attempted to add duplicate item\n");
        return false;
    }

    /* 成功创建一个新节点 */
    ptree->size ++;

    if (ptree->root == NULL) {
        ptree->root = new_node;
    } else {
        add_node(new_node, ptree->root);
    }

    return true;
}

bool in_tree(const Item *pi, const Tree *ptree)
{
    return (seek_item(pi, ptree).child == NULL)?false:true;
}

bool delete_item(const Item *pi, Tree *ptree)
{
    Pair look;
    look = seek_item(pi, ptree);
    if (look.child == NULL)
        return false;

    if (look.parent == NULL)
        delete_node(&ptree->root);
    else if (look.parent->left == look.child)
        delete_node(&look.parent->left);
    else
        delete_node(&look.parent->right);

    ptree->size --;

    return true;
}

void traverse(const Tree *ptree, void (*pfun)(Item item))
{
    if (ptree != NULL) {
        in_order(ptree->root, pfun);
    }
}

void delete_all_nodes(Tree *ptree)
{
    if (ptree != NULL) {
        delete_all_nodes(ptree->root);
        ptree->root = NULL;
        ptree->size = 0;
    }
}

/***********************局部函数***************************/
static void in_order(const Trnode *root, void (*pfun)(Item item))
{
    if (root != NULL) {
        in_order(root->left, pfun);
        (*pfun)(root->item);
        in_order(root->right, pfun);
    }
}

static void delete_all_nodes(Trnode *root)
{
    Trnode *pright;

    if (root != NULL) {
        pright = root->right;
        delete_all_nodes(root->left);
        free(root);
        delete_all_nodes(pright);
    }
}

/**
 * */
static void add_node(Trnode *new_node, Trnode *root)
{
    if (to_left(&new_node->item, &root->item)) {
        if (root->left == NULL)
            root->left = new_node;
        else
            add_node(new_node, root->left);
    } else if (to_right(&new_node->item, &root->item)) {
        if (root->right) {
            root->right = new_node;
        } else {
            add_node(new_node, root->right);
        }
    } else {
        fprintf(stderr, "location error in add_node\n");
        //exit(1);
    }
}

static bool to_left(const Item *i1, const Item *i2)
{
    return (strcmp(i1->petname, i2->petname) < 0 ||
            strcmp(i1->petkind, i2->petkind) < 0) ? true:false;
}

static bool to_right(const Item *i1, const Item *i2)
{
    return (strcmp(i1->petname, i2->petname) > 0 ||
            strcmp(i1->petkind, i2->petkind) > 0) ? true:false;
}

static Trnode *make_node(const Item *pi)
{
    Trnode *new_node;
    
    new_node = (Trnode *)malloc(sizeof(Trnode));
    if (new_node != NULL) {
        new_node->item = *pi;
        new_node->left =  NULL;
        new_node->right = NULL;
    } else {
        fprintf(stderr, "%s malloc fail\n", __func__);
        return NULL;
    }

    return new_node;
}

/**
 * 寻找防止item的位置
 *
 * 返回值：没找到,look.child设置为NULL
 *          根节点则 look.parent = NULL
 * */
static Pair seek_item(const Item *pi, const Tree *ptree)
{
    Pair look;

    //根节点 parent = NULL;
    look.parent = NULL;
    look.child = ptree->root;

    if(look.child == NULL)
        return look;

    while (look.child != NULL) {
        if(to_left(pi, &(look.child->item))) {
            look.parent = look.child;
            look.child = look.child->left;
        } else if (to_right(pi, &(look.child->item))) {
            look.parent = look.child;
            look.child = look.child->right;
        } else {
            break;
        }
    }

    return look;
}

/**
 * ptr是指向目标节点的父节点指针成员的地址
 * */
static void delete_node(Trnode **ptr)
{
    Trnode *temp;
    if ((*ptr)->left == NULL) {
        temp = *ptr;
        *ptr = (*ptr)->right;
        free(temp);
    } else if ((*ptr)->right == NULL) {
        temp = *ptr;
        *ptr = (*ptr)->left;
        free(temp);
    } else {
        //被删除的节点有两个子节点
        //找到重新连接右子树的位置
        for (temp = (*ptr)->left; temp->right != NULL; temp = temp->right)
            continue;

        temp->right = (*ptr)->right;
        temp = *ptr;
        *ptr = (*ptr)->left;
        free(temp);
    }
}
