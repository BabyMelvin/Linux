 ffmpeg 提供的一个非常好用的健值对工具：AVDictionary，特别是对于没有 map 容器的 c 代码，可以充分利用它来配置和定义播放器的参数，ffmpeg 本身也有很多 API 通过它来传递参数。
## 1.AVDictionary 的用法简介
AVDictionary 所在的头文件在 `libavutil/dict.h`，其定义如下：
```c
struct AVDictionary{
	int count;
	AVDictionaryEntry *elems;
};
```
其中，AVDictionaryEntry 的定义如下：
```c
typedef struct AVDictionaryEntry{
	char*key;
	char*value;
} AVDictionaryEntry;
```

## 下面就用示例的方式简单介绍下用法
### （1）创建一个字典
```c
AVDictionary*d=NULL;
```

### （2） 销毁一个字典
```c
av_dict_free(&d);
```

### （3）添加一对 key-value
```c
av_dict_set(&d, "name", "jhuster", 0);
av_dict_set_int(&d,"age","29",0);
```

### (4)获取 key 的值
```c
AVDictionaryEntry *t=NULL;
t=av_dict_get(d,"name",NULL,AV_DICT_IGNORE_SUFFIX);
av_log(NULL,AV_LOG_DEBUG,"name:%s",t->value);

t=av_dict_get(d,"age",NULL,AV_DICT_IGNORE_SUFFIX);
av_log(NULL,AV_LOG_DEBUG,"age:%d",(int)(*t->value));
```
### （5） 遍历字典
```c
AVDictionaryEntry*t=NULL;
while((t=av_dict_get(d,"",t,AV_DICT_IGONORE_SUFFIX))){
	av_log(NULL,AV_LOG_DEBUG,"%s,%s",t->key,t->value);
}
```

## 2.  ffmpeg 参数的传递
ffmpeg 中很多 API 都是靠 AVDictionary 来传递参数的，比如常用的：

```c
int avformat_open_input(AVFormatContext**ps,const char *url,AVInputFormat*fmt,AVDictionary**options);
```
最后一个参数就是 AVDictionary，我们可以在打开码流前指定各种参数，比如：`探测时间`、`超时时间`、`最大延时`、`支持的协议的白名单`等等，例如：
```c
AVDictionary*options=NULL;
av_dict_set(&options,"probesize","4096",0);
av_dict_set(&options,"max_delay","5000000",0);

AVFormatContext *ic=avformat_alloc_context();
if(avformat_open_input(&ic,url,NULL,&options)<0){
	LOGE();
	return -1;
}
```
那么，我们怎么知道 ffmpeg 的这个 API 支持哪些可配置的参数呢 ？
我们可以查看 ffmpeg 源码，比如 avformat_open_input 是结构体 AVFormatContext 提供的 API，在 libavformat/options_table.h 中定义了 AVFormatContext 所有支持的 options 选项，如下所示：

<a href="https://www.ffmpeg.org/doxygen/trunk/libavformat_2options__table_8h-source.html">options选项</a>

同理，AVCodec 相关 API 支持的 options 选项则可以在 libavcodec/options_table.h 文件中找到，如下所示：

<a href="https://www.ffmpeg.org/doxygen/3.1/libavcodec_2options__table_8h_source.html">options选项</a>