# 1.安装

创建目录和修改环境变量

```
$ mkdir ~/bin
$ PATH=~/bin:$PATH
```

下载repo代码

```
$ curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
$ chmod a+x ~/bin/repo
```

# 2.帮助

```
repo help
```
查询具体命令的帮助

```
repo help <COMMAND>
```

Repo仓创建

```
repo help init
```

状态

```
repo status .
```

# 3.初始化

```
$repo init -u <URL> [<OPTIONS>]
```

例子:

```
$ mkdir WORKING_DIRECTORY
$ cd WORKING_DIRECTORY

repo init -u https://android.googlesource.com/platform/manifest
```

这个命令会在当前文件夹创建一个 .repo 文件夹，它包含

* `repo/` : 其实是一个repo的python源码
* `manifest.xml`: 工作树的主配置文件，不要编辑这个文件
* `projects/`: 每个git工程的`.git`结构
* `local_manifest.xml`: 这个文件不是repo创建的，你可以创建这个文件来代替主配置文件，也就是临时测试用用

选项：

* `-u`: 指定manifest仓库地址
* `-m`: 选择仓库中某个manifest文件，如果没有设置，就使用default.xml
* `-b`: 指定一个分支或修正版本

# 4.拉代码

```
repo sync
 
repo sync PROJECT0 PROJECT1 PROJECT2 ...
```
选项:

* `-j`：多任务，一般8核心可以开到16任务,过多会起反作用
* `-c`:只下载当前分支代码
* `-d`：让工程回退到manifest指定的版本
* `-f`:如果某个工程同步失败，继续同步

# 5.修改repo
如果你想修改repo的结构，应该在`.repo/manifests`文件夹里面修改，然后用**git命令**提交

manifest.xml 文件结构:

```xml
<?xml version="1.0" encoding="UTF-8"?>  
<manifest>  
 
  <remote  name="shift"  
           fetch="git://git.mygit.com/" />  
  <default revision="kk-shift"  
           remote="shift"  
           sync-j="1" />  
 
  <project path="packages/shift/VideoPlayer" name="platform/packages/shift/VideoPlayer" />  
  <include name="another_manifest.xml" />
</manifest>  
```

* manifest:这个是配置的顶层元素，即根标志
* remote:
	* name：在每一个`.git/config`文件的remote项中用到这个name，即表示每个git的远程服务器的名字(这个名字很关键，如果多个remote属性的话，default属性中需要指定default remote)。git pull、get fetch的时候会用到这个remote name。
	* alias：可以覆盖之前定义的remote name，name必须是固定的，但是alias可以不同，可以用来指向不同的remote url
	* fetch：所有git url真正路径的前缀，所有git 的project name加上这个前缀，就是git url的真正路径
	* review：指定Gerrit的服务器名，用于repo upload操作。如果没有指定，则repo upload没有效果

* default:设定所有projects的默认属性值，如果在project元素里没有指定一个属性，则使用default元素的属性值。
	* remote：远程服务器的名字（上面remote属性中提到过，多个remote的时候需要指定default remote，就是这里设置了）
	* revision：所有git的**默认branch**，后面project没有特殊指出revision的话，就用这个branch
	* sync_j ： 在repo sync中默认并行的数目
	* sync_c ：如果设置为true，则只同步指定的分支(revision 属性指定)，而不是所有的ref内容
	* sync_s ： 如果设置为true，则会同步git的子项目
* manifest-server:它的url属性用于指定manifest服务的URL，通常是一个XML RPC 服务它要支持一下RPC方法：
	* GetApprovedManifest(branch, target) ：返回一个manifest用于指示所有projects的分支和编译目标。
	* target参数来自环境变量TARGET_PRODUCT和TARGET_BUILD_VARIANT，组成`$TARGET_PRODUCT-$TARGET_BUILD_VARIANT`
	* GetManifest(tag) ：返回指定tag的manifest
* project:需要clone的单独git
	* name ：git 的名称，用于生成git url。URL格式是：`${remote fetch}/${project name}.git`.其中的 fetch就是上面提到的remote 中的fetch元素，name 就是此处的name
	* path ：clone到本地的git的工作目录，如果没有配置的话，跟name一样
	* remote ：定义remote name，如果没有定义的话就用default中定义的remote name
	* revision ：指定需要获取的git提交点，可以定义成固定的branch，或者是明确的commit 哈希值
	* groups ：列出project所属的组，以空格或者逗号分隔多个组名。所有的project都自动属于"all"组。每一个project自动属于
	* name:'name' 和path:'path'组。例如，它自动属于default, name:monkeys, and path:barrel-of组。如果一个* project属于notdefault组，则，repo sync时不会下载
	* sync_c ：如果设置为true，则只同步指定的分支(revision 属性指定)，而不是所有的ref内容。
	* sync_s ： 如果设置为true，则会同步git的子项目
	* upstream ：在哪个git分支可以找到一个SHA1。用于同步revision锁定的manifest(-c 模式)。该模式可以避免同步整个ref空间
	* annotation ：可以有0个或多个annotation，格式是name-value，repo forall命令是会用来定义环境变量
* include:通过name属性可以引入另外一个manifest文件(路径相对与当前的manifest.xml 的路径)
	* name ：另一个需要导入的manifest文件名字,可以在当前的路径下添加一个another_manifest.xml，这样可以在另一个xml中添加或删除project
* remove-project
	* 从内部的manifest表中删除指定的project。经常用于本地的manifest文件，用户可以替换一个project的定义

# 6. 创建快照

你可以根据当前`.repo`的状态来创建一个配置文件:

```
repo manifest -o snapshot.xml -r
```
这个文件可以用来保存当前的工作状态

恢复一个快照，可以用下面的命令

```
cp snapshot.xml .repo/manifests/
repo init -m snapshot.xml
repo sync -d
```
**注意**：没有commit的修改不会恢复，已经commit的但是没有push的是可以恢复的，但只能从本地恢复。

# 7.切换分支

```
repo start BRANCH_NAME PROJECT_NAME
```
查看分支

```
repo branches
```

提交
```
repo upload
```

安全删除不需要的分支

```
repo prune
```

# 8.基本工作流程

* `repo start`:开始一个新的工作分支Use to start a new topic branch.
* `git commit`:提交修改Use git add to stage changes.
* `repo upload`:上传修改

# 9.一些问题
如果repo长时间没有响应，可以试试杀掉python进程

```
killall python
```
如果想要repo执行时的更多信息，可以加上`--trace`选项