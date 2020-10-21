# configparser模块

用于生成和修改常见配置文档
配置文件

```
[default]
serveraliveinterval = 45
compression = yes
compressionlevel = 9

[bitbucket.org]
user = hg

[topsecret.server.com]
port = 50022
forwardx11 = no
```

## 1.生成这样配置文件

```python
import configparser

config = configparser.ConfigParser()
config['default'] = {'ServerAliveInterval':'45',
                     'Compression':'yes',
                     'CompressionLevel':'9'}
config['bitbucket.org'] = {}
config['bitbucket.org']['User'] = 'hg'
config['topsecret.server.com'] = {'port':'50022',
                                'Forwardx11':'no'}
with open('example.ini','w') as configfile:
    config.write(configfile)
```

# 2.读取配置

```python
import configparser
config = configparser.ConfigParser()

print(config.read('example.ini'))

#查看所有的标题
print(config.sections())
#['default', 'bitbucket.org', 'topsecret.server.com']

#查看标题section1下所有key=value的keyoptions = config.options('default')
print(options)
#['serveraliveinterval', 'compression', 'compressionlevel']#查看标题section1下所有key=value的(key,value)格式
items_list = config.items('topsecret.server.com')
print(items_list)
#[('port', '50022'), ('forwardx11', 'no')]
```

# 3.增删改查

```python
import configparser

config = configparser.ConfigParser()
config.read('example.ini',encoding = 'utf-8')

#删除整个标题
config.remove_section('bitbucket.org')

#删除标题下的option
config.remove_option('topsecret.server.com','port')

#添加一个标题
config.add_section('info')
#在标题下添加options
config.set('info','name','derek')

#判断是否存在
print(config.has_section('info'))        #True
print(config.has_option('info','name'))    #True

#将修改的内容存入文件
config.write(open('new_example.ini','w'))
```

修改后内容

```
#修改后的文件

[default]
serveraliveinterval = 45
compression = yes
compressionlevel = 9

[topsecret.server.com]
forwardx11 = no

[info]
name = derek
```