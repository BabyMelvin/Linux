/**
 * 二叉树实现
 *      不包含插入和删除非叶子结点的操作
 * */
#ifndef __BITREE_H__
#define __BITREE_H__

typedef struct BiTreeNode_ {
    void *data;
    struct BiTreeNode_ *left;
    struct BiTreeNode_ *right;
} BiTreeNode;

typedef struct BiTree_ {
    int size;
    int (*compare) (const void *key1, const void *key2);
    void (*destroy) (void *data);
    BiTreeNode *root;
} BiTree;

void bitree_init (BiTree *tree, void (*destory)(void *data));
void bitree_destroy (BiTree *tree);

/**
 * 插入一个结点，成为node所指定结点的左子结点。
 *  如果node已有左子结点，则返回-1
 * */

int bitree_ins_left (BiTree *tree, BiTreeNode *node, const void *data);
int bitree_ins_right (BiTree *tree, BiTreeNode *node, const void *data);
void bitree_rem_left (BiTree *tree, BiTreeNode *node);
void bitree_rem_right (BiTree *tree, BiTreeNode *node);

/**
 * 将left和right合成单棵二叉树,data所表示merge的根结点
 * */
int bitree_merge (BiTree *merge, BiTree *left, BiTree *right, const void *data);

#define bitree_size(tree) ((tree)->size)
#define bitree_root(tree) ((tree)->root)
#define bitree_is_eob(node) ((node) == NULL)
#define bitree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)
#define bitree_data(node) ((node)->data)
#define bitree_left(node) ((node)->left)
#define bitree_right(node) ((node)->right)
#endif /* __BITREE_H__ */
