#ifndef __CHTBL_H__
#define __CHTBL_H__

typedef struct CHTbl_ {
	int buckets;
	int (*h)(const void *key);
	int (*match)(const void *key1, const void *key2);
	void (*destory)(void *data);
	int size;
	List *table;
} CHTbl;
/**
* 成功返回0，失败返回-1
* 	哈希表中分配桶的个数由buckets指定
* 	函数指针h指向用户定义的哈希函数，此函数会将键进行散列
*	指针match判断两个键是否匹配
* 复杂度O(m)，m是哈希表中"桶“的个数
* */
int chtbl_init(CHTbl *htbl, int buckets, int (*h)(const void *key),
	int (*match)(const void *key1, const void *key2),
	void (*destory)(void *data));
	
void chtbl_destory(CHTbl *htbl);

int chtbl_insert(CHTbl *htbl, const void *data);
int chtbl_remove(CHTbl *htbl, void **data);

/**
* 如果哈希表中找到元素，则返回0，否则返回-1
* 	查找htbl指定链式哈希表中是否与data相匹配的元素。如果找到
*  	 在函数返回时，data将指向哈希表中相配配元素中的数据
**/
int chtbl_lookup(const CHTbl *htbl, void **data);

#define chtbl_size(htbl) ((htbl)->size)
#endif /* __CHTBL_H__ */