# OS 模块
os模块提供了不少与操作系统相关联的函数。

```python
>>> import os
>>> os.getcwd()      # 返回当前的工作目录
'C:\\Python34'
>>> os.chdir('/server/accesslogs')   # 修改当前的工作目录
>>> os.system('mkdir today')   # 执行系统命令 mkdir 
0
```

建议使用 "import os" 风格而非 "from os import *"。这样可以保证随操作系统不同而有所变化的 `os.open()` 不会覆盖内置函数 `open()`。

在使用 os 这样的大型模块时内置的`dir()`和`help()`函数非常有用:
```python
>>> import os
>>> dir(os)
<returns a list of all module functions>
>>> help(os)
<returns an extensive manual page created from the module's docstrings>
```

```
os.getcwd() #获取当前工作目录，即当前python脚本工作的目录路径
os.chdir()　　#当前目录
os.chdir("dirname")  #改变当前脚本工作目录；相当于shell下cd    os.chdir(r"c:\Users")
os.curdir  #返回当前目录: ('.')，相当于shell下cd.
os.pardir # 获取当前目录的父目录字符串名：('..')，相当于shell下cd.. 返回上一层目录
os.makedirs('dirname1/dirname2')    #可生成多层递归目录      os.makedirs(r"c:\a\b\c")
os.removedirs('dirname1')    #若目录为空，则删除，并递归到上一级目录，如若也为空，则删除，依此类推      os.removedirs(r"c:\a\b\c")
os.mkdir('dirname')    #生成单级目录；相当于shell中mkdir dirname
os.rmdir('dirname')   # 删除单级空目录，若目录不为空则无法删除，报错；相当于shell中rmdir dirname
os.listdir('dirname')    #列出指定目录下的所有文件和子目录，包括隐藏文件，并以列表方式打印        os.listdir("c:\\test")
os.remove()  #删除一个文件                  os.remove(r"c:\oldboy.txt")
os.rename("oldname","newname")  #重命名文件/目录      os.rename("c:\\test","c:\\test2")                    
os.stat('path/filename') # 获取文件/目录信息          os.stat("c:\\test2")
os.sep    #输出操作系统特定的路径分隔符，win下为"\\",Linux下为"/"
os.linesep    #输出当前平台使用的行终止符，win下为"\r\n",Linux下为"\n"
os.pathsep    #输出用于分割文件路径的字符串 
os.name    #输出字符串指示当前使用平台。win->'nt'; Linux->'posix'
os.system("bash command")  #运行shell命令，直接显示
os.environ  #获取系统环境变量
os.path.abspath(path)  #返回path规范化的绝对路径
os.path.split(path)  #将path分割成目录和文件名二元组返回
os.path.dirname(path)  #返回path的目录。其实就是os.path.split(path)的第一个元素
os.path.basename(path) # 返回path最后的文件名。如何path以／或\结尾，那么就会返回空值。即os.path.split(path)的第二个元素
os.path.exists(path)  #如果path存在，返回True；如果path不存在，返回False
os.path.isabs(path)  #如果path是绝对路径，返回True
os.path.isfile(path)  #如果path是一个存在的文件，返回True。否则返回False
os.path.isdir(path)  #如果path是一个存在的目录，则返回True。否则返回False
os.path.join(path1[, path2[, ...]])  #将多个路径组合后返回，第一个绝对路径之前的参数将被忽略
os.path.getatime(path)  #返回path所指向的文件或者目录的最后存取时间
os.path.getmtime(path)  #返回path所指向的文件或者目录的最后修改时间
os.path.getsize(path)  #返回path的大小
```

# sys模块

```
sys.argv           命令行参数List，第一个元素是程序本身路径
sys.exit(n)        退出程序，正常退出时exit(0)
sys.version        获取Python解释程序的版本信息
sys.maxint         最大的Int值
sys.path           返回模块的搜索路径，初始化时使用PYTHONPATH环境变量的值
sys.platform       返回操作系统平台名称
```

# shutil模块
高级的 文件、文件夹、压缩包 处理模块

拷贝文件和权限
针对日常的文件和目录管理任务，:mod:shutil 模块提供了一个易于使用的高级接口:

```python
>>> import shutil
>>> shutil.copyfile('data.db', 'archive.db')
>>> shutil.move('/build/executables', 'installdir')
```

* shutil.copy2(src, dst) 拷贝文件和状态信息

* shutil.copystat(src, dst) 仅拷贝状态的信息，包括：mode bits, atime, mtime, flags

* shutil.copymode(src, dst) 仅拷贝权限。内容、组、用户均不变

* shutil.copytree(src, dst, symlinks=False, ignore=None) 递归的去拷贝文件夹

* shutil.rmtree(path[, ignore_errors[, onerror]]) 递归的去删除文件

# logging模块

很多程序都有记录日志的需求，并且日志中包含的信息即有正常的程序访问日志，还可能有错误、警告等信息输出，python的logging模块提供了标准的日志接口，你可以通过它存储各种格式的日志，logging的日志可以分为 debug、info、warning、error、critical5个级别

# 1.模块初始

```python
import logging

logging.warning('wrong password more than 3 times')
logging.critical('server is down')
logging.error('error message')


结果：
WARNING:root:wrong password more than 3 times
CRITICAL:root:server is down
ERROR:root:error message
```
