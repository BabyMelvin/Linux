## stat fstat

```c
#include <sys/stat.h>
int stat(const char* restrict pathname,struct stat*restrict buf);
int fstat(int fd,struct stat*buf);
int lstat(const char* restrict pathname,struct stat*restrict buf);
int fstatat(int fd,const char* restrict pathaname,struct stat*restrict buf,int flag);
```

* stat函数:返回与次命名文件相关的信息结构体
* fstat:返回与此文件描述符先关的结构体
* lstat：命名文件是一个链接文件
* fstatat:一个相对于当前打开目录的路径名返回文件统计信息．
    * flag:控制是否跟随着一个符号链接．
        * 当AT_SYMLINK_NOFLLOW时，fstatat不会跟随符号链接，返回符号链接本身信息．
    * 如果fd＝AT_FDCWD
        * pathname是一个相对路径
buf只想的结构:

```c
struct stat{
    mode_t  st_mode; //file type & mode(permission)
    ino_t   st_ino;  //i-node number (serial number)
    dev_t   st_dev;  //device number(file system)
    nlink_t st_nlink;//number of links
    uid_t   st_uid;  //user ID of owner
    gid_t   st_gid; // group IO of owner
    off_t   st_size; //size of bytes,for regular file
    struct  timespec st_atime; /// time of last access
    struct  timespec st_mtime; //time of last modificaiton
    struct  timespec st_ctime; //time of last file status change
    blksize_t st_blksize; //best I/O block size
    blkcnt_t  st_blocks;  //number of disk blocks allocated
};
```

时间的结构体:

```c
struct timespec{
    time_t tv_sec;
    long   tv_nsec;
};
```
stat函数最多是用在`ls -l`命令上

