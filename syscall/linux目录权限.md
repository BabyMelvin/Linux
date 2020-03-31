# 关于Linux下s、t、i、a权限

文件权限除了r、w、x外还有s、t、i、a权限：首先我们利用umask查看系统的权限为四位，首位就是特殊权限位，SetUID为4，SetGID为2，t为1

`$umask` : 0022

# s权限解读(setuid 和 setgid)

## 1.SetUID权限解读(针对所属主设置s权限，数字表示为4)

SetUID:当一个可执行程序具有SetUID权限，用户执行这个程序时，将以这个程序所有者的身份执行。前提是这个文件是可执行文件，可就是具有x权限(**属组必须先设置相应的x权限**)。chmod命令不进行必要的完整性检查，即使不设置x权限就设置s权限，chmod也不会报错，当我们ls -l时看到rwS，大写S说明s权限未生效）

1.**设置权限的方法**:

```
chmod u+s xxx
chmod 4777 xxxx
```
解释:我们利用umask知道权限的设置是四位数，后三位是ugo对应的属性，首位就是特殊位权限的表示，SetUID的权限位是4，因此可以用chmod 4777 xxx设置s权限。

例1: linux的passwd命令就具有s权限，因此普通用户可以修改自己的密码:

```
$ls -la `which passwd`
-rwsr-xr-x 1 root root 47032  5月 17  2017 /usr/bin/passwd
```
解释:passwd的所属这是root，所属组也是root，具有s权限，因此其他用户执行passwd的时候是以passwd的所属组root执行。

例2:将touch赋予s权限，查看其创建的文件的所属组
(1)qlq用户touch没有被赋予s权限的时候创建一个文件(所属主是当前用户，所属组是当前用户的组:)

```
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ls
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ touch test1file
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll
total 0
-rw-rw-r-- 1 qlq qlq 0 Mar 30 12:54 test1file
```

(2)root用户将touch赋予s权限:

```
[root@iz2ze46xi6pjjj69ailg9lz ~]# chmod u+s `which touch`　　#赋予所属主s权限
[root@iz2ze46xi6pjjj69ailg9lz ~]# ll `which touch`　　　　#查看touch权限
-rwsr-xr-x. 1 root root 62488 Nov  6  2016 /usr/bin/touch
```

(3)qlq用户再次创建一个文件查看所属主(所属主是root，所属组是当前用户所属组)

```
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ touch test1file2
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll ./test1file2
-rw-rw-r-- 1 root qlq 0 Mar 30 12:59 ./test1file2
```

2.**收回s权限**

```
chmod u-s xxx
chmod  0777   xxx
```
 例如:收回上面的touch的s权限之后再次创建文件:
 (1)root用户收回权限

 ```
 [root@iz2ze46xi6pjjj69ailg9lz ~]# chmod 0755 `which touch`    #收回权限
 [root@iz2ze46xi6pjjj69ailg9lz ~]# ll `which touch`　　　　　　#查看权限
 -rwxr-xr-x. 1 root root 62488 Nov  6  2016 /usr/bin/touch
 ```
 第一位为特殊权限位。因此用0就表示没有特殊权限。
 (2)qlq用户再次创建一个文件查看信息:(所属主变成当前用户)

 ```
 [qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll ./test1file3
 -rw-rw-r-- 1 qlq qlq 0 Mar 30 13:06 ./test1file3
 ```

 (3)查看系统具有SetUID或者GID的文件

 ```
 [root@iz2ze46xi6pjjj69ailg9lz ~]# find / -perm 4755 -o -perm 2755  #跟据权限查看，-o表示or，4代表SetUID，2代表SetGID
 /usr/lib/polkit-1/polkit-agent-helper-1
 /usr/sbin/netreport
 /usr/sbin/postqueue
 /usr/sbin/pam_timestamp_check
 /usr/sbin/unix_chkpwd
 /usr/sbin/postdrop
 /usr/sbin/usernetctl
 /usr/bin/at
 /usr/bin/gpasswd
 /usr/bin/passwd
 /usr/bin/write
 /usr/bin/su
 ```
 注意:
 　　vim和vi不能被授予s权限，否则好多只读文件都可以被以root身份保存。

(4)当我们针对一个不具备x权限的文件设置s权限时无效，权限变为大写S，大写S说明s权限未生效

```
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ chmod u+s ./test1file
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll
total 0
-rwSrw-r-- 1 qlq  qlq 0 Mar 30 12:54 test1file
```

## 2.SetGID解读(针对所属组设置权限,数字表示为2)
一个可执行文件具有SetGID权限表示运行这个程序的时候是以这个程序的所属组的身份运行，同样这个文件需要具有x权限(必须可以运行)

设置权限方法

```
chmod g+s xxx
chmod 2777 xxx
```
收回权限方法:

```
chmod g-s xxx
chmod 0777 xxx
```
例1:针对上面的touch同时设置SetUID和SetGID权限之后创建文件
(1)root用户设置权限

```
[root@iz2ze46xi6pjjj69ailg9lz ~]# chmod 6755 `which touch`
[root@iz2ze46xi6pjjj69ailg9lz ~]# ll `which touch`
-rwsr-sr-x. 1 root root 62488 Nov  6  2016 /usr/bin/touch
```

(2)qlq用户创建一个文件:(所属主为root，所属组是root所在的组)

```
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ touch test1file4
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll | grep test1file4
 -rw-rw-r-- 1 root root 0 Mar 30 13:19 test1file4
``` 
例三:收回touch的SetGID权限:

```
[root@iz2ze46xi6pjjj69ailg9lz ~]# chmod g-s `which touch`
[root@iz2ze46xi6pjjj69ailg9lz ~]# ll `which touch`
    -rwxr-xr-x. 1 root root 62488 Nov  6  2016 /usr/bin/touc例2:将touch的权限只设置SetGID并创建文件:
```    
(1)root用户设置权限

```
[root@iz2ze46xi6pjjj69ailg9lz ~]# chmod 2755 `which touch`
[root@iz2ze46xi6pjjj69ailg9lz ~]# ll `which touch`
    -rwxr-sr-x. 1 root root 62488 Nov  6  2016 /usr/bin/touch
```   

(2)qlq用户创建一个文件:(所属主为qlq，所属组是root所在的组)
 
 ```
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ touch test1file5
[qlq@iz2ze46xi6pjjj69ailg9lz test]$ ll | grep test1file5
    -rw-rw-r-- 1 qlq  root 0 Mar 30 13:21 test1file5
```

注意:上面的SID和GID要求用户必须有执行文件的权利，如果有执行权限执行的时候s位才会生效:

# 3.黏着位 t  解读(针对others设置，可以实现文件夹共享)
t ：设置粘着位，一般针对权限是777的文件夹设置权限。如果文件设置了t权限则只有属主和root有删除文件的权限(没有意义)

如果权限为777的目录设置t权限，索引用户可以在这个目录下面创建文件和删除自己创建的文件，删除其他人创建的文件权限不被允许(当然root可以删除所有人创建的权限)。(为了共享目录，例如临时文件夹)

设置权限

```
chmod  o+t  xxx
chmod  1777 xxx
```
收回权限

```
chmod o-t  xxx　　
chmod 0777 xxx
```
例1:linux的存放临时文件夹的tmp目录就有t权限:

```
[root@iz2ze46xi6pjjj69ailg9lz /]# ll / | grep tmp
drwxrwxrwt. 11 root root    4096 Mar 30 13:13 tmp
```

例2:创建一目录并设置t权限：

(1)root用户创建目录并设置权限为777+t权限

```
[root@iz2ze46xi6pjjj69ailg9lz test]# chmod 777 /mydir/
[root@iz2ze46xi6pjjj69ailg9lz test]# ll /|grep mydir
drwxrwxrwx   2 root root    4096 Mar 30 13:32 mydir
[root@iz2ze46xi6pjjj69ailg9lz test]# chmod o+t /mydir/
[root@iz2ze46xi6pjjj69ailg9lz test]# ll /|grep mydir
drwxrwxrwt   2 root root    4096 Mar 30 13:32 mydir
```
(2)root用户在此目录下创建几个文件

```
[root@iz2ze46xi6pjjj69ailg9lz mydir]# touch rootfle1
[root@iz2ze46xi6pjjj69ailg9lz mydir]# touch rootfle2
[root@iz2ze46xi6pjjj69ailg9lz mydir]# ll
total 0
-rw-r--r-- 1 root root 0 Mar 30 13:35 rootfle1
-rw-r--r-- 1 root root 0 Mar 30 13:35 rootfle2
```
（3）qlq在此目录下创建文件并删除文件(可以创建文件与删除自己创建的文件，删除root创建的文件被提示不允许)

```
z2ze46xi6pjjj69ailg9lz /]$ cd /mydir/
[qlq@iz2ze46xi6pjjj69ailg9lz mydir]$ touch qlqfile1
[qlq@iz2ze46xi6pjjj69ailg9lz mydir]$ touch qlqfile2
[qlq@iz2ze46xi6pjjj69ailg9lz mydir]$ rm rootfle1
rm: remove write-protected regular empty file ‘rootfle1’? y
rm: cannot remove ‘rootfle1’: Operation not permitted
[qlq@iz2ze46xi6pjjj69ailg9lz mydir]$ rm -f qlqfile1
[qlq@iz2ze46xi6pjjj69ailg9lz mydir]$ ll
total 0
-rw-rw-r-- 1 qlq  qlq  0 Mar 30 13:35 qlqfile2
-rw-r--r-- 1 root root 0 Mar 30 13:35 rootfle1
-rw-r--r-- 1 root root 0 Mar 30 13:35 rootfle2
```

# 4.i与a权限解读
i：不可修改权限  例：`chattr u+i filename` 则filename文件就不可修改，无论任何人，如果需要修改需要先删除i权限，用`chattr -i filename`就可以了。查看文件是否设置了i权限用lsattr filename。

a：只追加权限， 对于日志系统很好用，这个权限让目标文件只能追加，不能删除，而且不能通过编辑器追加。可以使用`chattr +a`设置追加权限。
