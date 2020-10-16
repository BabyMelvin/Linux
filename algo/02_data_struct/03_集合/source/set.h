#ifndef __SET_H__
#define __SET_H__

typedef list_t set_t;
void set_init(set_t *set, int (*match)(const void *key1, const void *key2),
        void (*destory)(void *data));

#define set_destory list_destory
/**
 * 插入操作成功返回0；如果插入的成员在集合中已经存在返回1；否则返回-1
 * 复杂度O(n)
 * */
int set_insert(set_t *set, const void *data);

/**
 * 如果移除操作成功返回0，否则返回-1
 * */
int set_remove(set_t *set, void **data);

/**
 * 成功返回0，否则返回-1
 * 建立一个集合，其结果为set1和set2所指定结合的并集.返回后,setu就代表这个并集
 * */
int set_union(set_t *setu, const set_t *set1, const set_t *set2);

/**
 * 交集计算成功返回0，否则返回-1
 * 复杂度O(mn)
 * */
int set_intersection(set_t *seti, const set_t *set1, const set_t *set2);


/**
 * 计算差集成功返回0，否则返回-1
 * */
int set_difference(set_t *setd, const set_t *set1, const set_t *set2);

int set_is_member(const set_t *set, const void *data);
int set_is_subset(const set_t *set1, const set_t *set2);
int set_is_equal(const set_t *set1, const set_t *set2);
#define set_size(set) ((set)->size)
#endif /* __SET_H__ */
