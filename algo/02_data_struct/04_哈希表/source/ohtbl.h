/* 开地址哈希函数接口 */

#ifndef __OHTBL_H__
#define __OHTBL_H__

typedef struct OH_tbl_ {
    int positions;
    void *vacated; //倡导
    int (*h1)(const void *key);
    int (*h2)(const void *key);
    int (*match)(const void *key1, const void *key2);
    void (*destroy)(void *data);
    int size;
    void **table;
} OH_tbl_t;

/**
 * positions指定槽位个数
 * 函数指针h1和h2用来指定用户定义的辅助哈希函数完成散列过程
 * */
int ohtbl_init(OH_tbl_t *htbl, int positions, int (*h1)(const void *key),
        int (*h2)(const void *key), int (*match)(const void *key1,
            const void *key2), void (*destory)(void *data));

/**
 * 销毁htbl指定的开地址哈希表
 * */

void ohtbl_destory(OH_tbl_t *htbl);

/**
 * 插入一个新元素，新元素包含一个指向data的指针
 * data内容调用者管理 
 * 成功返回1，否则返回-1
 * */
int ohtbl_insert(OH_tbl_t *htbl, const void *data);

/**
 * 删除元素
 * */
int ohtbl_remove(OH_tbl_t *htbl, void **data);

/**
 * 查找htbl指定开地址哈希表中是否与data相匹配
 * */
int ohtbl_lookup(OH_tbl_t *htbl, void *data);

#define ohtbl_size(htbl) ((htbl)->size)
#endif /* __OHTBL_H__ */
