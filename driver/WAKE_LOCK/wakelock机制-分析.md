# android 休眠唤醒机制分析(一) — wake_lock
Android的休眠唤醒主要基于wake_lock机制，只要系统中存在任一有效的wake_lock，系统就不能进入深度休眠，但可以进行设备的浅度休眠操作。wake_lock一般在关闭lcd、tp但系统仍然需要正常运行的情况下使用，比如听歌、传输很大的文件等。本文主要分析driver层wake_lock的实现。

# 1.wake_lock 定义和接口

```c
enum {
	WAKE_LOCK_SUSPEND, // 阻止进入深度休眠模式
	WAKE_LOCK_IDLE,    // 阻止进入空闲模式
	WAKE_LOCK_TYPE_COUNT
};
struct wake_lock{
#ifdef CONFIG_HAS_WAKELOCK
	struct list_head link;//链表节点
	int 	flags;//标志
	const char *name;//名称
	unsigned long expires;//超时时间
#ifdef CONFIG_WAKELOCK_STAT
	struct {
		int count;//使用计数
		int expire_count;//超时计数
		int wakeup_count; //唤醒计数
		ktime_t totoal_time; //锁使用时间
		ktime_t prevent_suspend_time; //锁阻止休眠时间
		ktime_t max_time; //锁使用时间最长的一次
		ktime_t last_time;//锁上次操作时间
	} stat;
#endif 
#endif
};
```
可以看到wake_lock按功能分为休眠锁和空闲锁两种类型，用于阻止系统进入深度休眠模式或者空闲模式。wake_lock的主要部件有锁名称、链表节点、标志位、超时时间，另外还有一个内嵌的结构用于统计锁的使用信息。接下来我们看看wake_lock对外提供的操作接口：

## 1.1内核空间接口

```c
void wake_lock_init(struct wake_lock *lock, int type, const char *name);
void wake_lock_destroy(struct wake_lock *lock);
void wake_lock(struct wake_lock *lock);
void wake_lock_timeout(struct wake_lock *lock, long timeout);
void wake_unlock(struct wake_lock *lock);
```

* 其中`wake_lock_init()`用于初始化一个新锁，type参数指定了锁的类型；
* `wake_lock_destroy()`则注销一个锁；
* `wake_lock()`和`wake_lock_timeout()`用于将初始化完成的锁激活，使之成为有效的永久锁或者超时锁；
* `wake_unlock()`用于解锁使之成为无效锁。另外还有两个接口：

```c
int wake_lock_active(struct wake_lock *lock);
long has_wake_lock(int type);
```

* 其中`wake_lock_active()`用于判断锁当前是否有效，如果有效则返回非0值；
* `has_wake_lock()`用于判断系统中是否还存在有效的type型锁
	* 如果存在**超时锁**则返回最长的一个锁的超时时间
	* 如果存在**永久锁**则返回-1
	* 如果系统中**不存在有效锁**则返回0。
## 2.用户空间接口
wake_lock向用户空间提供了两个文件节点用于申请锁和解锁：

```c
//wake_lock文件的读函数，显示用户空间定义的有效锁
ssize_t wake_lock_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	char *s = buf;
	char *end=buf+PAGE_SIZE;
	struct rb_node *n;
	struct user_wake_lock *l;
	mutex_lock(&tree_lock);
	for(n=rb_first(&user_wake_locks);n!=NULL;n=rb_next(n))){
		l=rb_entry(n,struct user_wake_lock,node);
		if(wake_lock_active(&l->wake_lock)){
			s+=scnprintf(s,end-s,"%s",l->name);
		}
	}
	s += scnprintf(s,end-s,""\n);
	mutex_unlock(&tree_lock);
	mutex_unlock(&tree_lock);
}

//wake_lock文件写函数，初始化并激活用户空间定义的锁
ssize_t wake_lock_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n){
	long timeout;
	struct user_wake_lock *l;
	
	mutex_lock(&tree_lock);
	l=lookup_wake_lock_name(buf,1,&timeout);
	if(IS_ERR(l)){
		n=PTR_ERR(l);
		goto bad_name;
	}
	if(debug_mask & DEBUG_ACCESS)
		pr_info("wake_lock_store:%s,timeout %ld\n",l->name,timeout);
		if(timeout)
			wake_lock_timeout(&l->wake_lock,timeout);
	else
		wake_lock(&l->wake_lock);
bad_name:
	mutex_unlock(&tree_lock);
	return n;
}
//wake_unlock 文件的读函数，显示用户空间的无效锁
ssize_t wake_unlock_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	char *s = buf;
	char *end = buf+PAGESIZE;
	struct rb_node *n;
	struct user_wake_lock *l;
	
	mutex_lock(&tree_lock);
	for(n = rb_first(&user_wake_locks);n!=NULL;n=rb_next(n)){
		l = rb_entry(n,struct user_wake_lock,node);
		if(!wake_lock_active(&l->wake_lock))
			s +=scnprintf(s,end-s,"%s",l->name);
	}
	s +=scnprintf(s,end-s,"\n");
	mutex_unlock(&tree_lock);
	return (s-buf);
}
//wake_unlock文件的写函数，用于用户空间解锁
ssize_t wake_unlock_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n){
	struct user_wake_lock *l;
	mutex_lock(&tree_lock);
	l = lookup_wake_lock_name(buf,0,NULL);
	if(IS_ERR(l)){
		n = PTR_ERR(l);
		goto not_found;
	}
	if(debug_mask & DEBUG_ACCESS){
		pr_info("wake_unlock_store:%s\n",l->name);
	}
	wake_unlock(&l->wake_lock);
not_found:
	mutex_unlock(&tree_lock);
	reutrn n;
}
power_attr(wake_lock);
power_attr(wake_unlock):
```
这两个文件节点分别为"/sys/power/wake_lock"和"/sys/power/wake_unlock"，应用程序可以根据HAL层的接口读写这两个节点。