# FFMEPG 日志打印
## 1.FFMPEG 打印日志的函数
FFMEPG有一套自己的日志系统，它使用av_log()函数来打印日志，其位于`<libavutil/log.h>`
```c
/*
* send the specified message to the log if the level is less than or equal to the 
* current av_log_level.By default,all logging messages are sent to stderr.This 
* behavior can be altered by setting a different logging callback function.
* @ see av_log_set_callback
*/
void av_log(void*avcl,int level,const char*fmt,...);
```
* `avcl`:指定一个包含AVClass的结构体，指定该log所属的结构体，如`AVFormatContext`,`AVContext`,`AVCodecContext`等等，可以设置为NULL
* `level`:log 的级别
* `fmt`:和C语言一样

## 2.FFMPEG日志级别
LOG级别是一个int类型，其可选数值及其含义如下:
```c
//print no output
#define AV_LOG_QUIET -8

//something went really wrong and we will crash now
#define AV_LOG_PANIC  0

//something went wrong and recovery is not possible.
//for example,no header was found for a format which depends on headers or an illegal
// combination of parameters is used
#define AV_LOG_FATAL  8

//something went wrong and cannot losslessly be recovered.However,not all future data is affected.
#define AV_LOG_ERROR  16

//something somehow does not look correct.This may or may not lead to problems.
//An example would be the use of -vstrict -2
#define AV_LOG_WARNING 24

//standard information
#define AV_LOG_INFO    32

//detailed information
#define AV_LOG_VERBOSE 40

//stuff which is only useful for libav* developers
#define AV_LOG_DEBUG   48
```

## 3.FFMPEG 设置和获取当前日志级别
由一个全局变量来控制那个级别以上的日志会打印输出，设置和获取这个全局变量的函数如下:
```
int av_log_get_level(void);
void av_log_set_level(int level);
```
## 4.FFMPEG日志打印函数的使用示例

```
av_log(NULL,AV_LOG_DEBUG,"hello world\n");
av_log(NULL,AV_LOG_ERROR,"error:%d\n",errorCode);
```

## 5.FFMPEG日志打印函数封装
```
#ifndef _SYS_LOG_
#define _SYS_LOG_
#include <libavutil/log.h>

#define LOGD(format,...) av_log(NULL,AV_LOG_DEBUG,format,##__VA_ARGS__);
#define LOGV(format,...) av_log(NULL,AV_LOG_VERBOSE,format,##__VA_ARGS__);
#define LOGI(format,...) av_log(NULL,AV_LOG_INFO,format,##__VA_ARGS__);
#define LOGW(format,...) av_log(NULL,AV_LOG_WARNING,format,##__VA_ARGS__);
#define LOGE(format,...) av_log(NULL,AV_LOG_ERROR,format,##__VA_ARGS__);
#endif
```

## 6.Android中打印FFMPEG日志
由于FFMPEG默认使用的是printf来打印日志，而Android系统有着一套自己的LOG系统，因此需要让FFMPEG的日志重新定向使用Andoird的日志系统，具体方法描述如下:

通过FFMPEG的av_log_set_callback()注册一个LOG callback funtion,FFMPEG就会把LOG打印功能重新定向到callback function中，代码示例如下:
```c
#ifdef _ANDROID_API_
#include <android/log.h>
#define ALOG(level,TAG,...) ((void)__android_log_vprint(level,TAG,__VA_ARGS__))
#define SYS_LOG_TAG "nmplayer"
static void syslog_print(void*ptr,int level,const char*fmt,va_list lv){
    switch(level){
        case AV_LOG_DEBUG:
            ALOG(ANDROID_LOG_DEBUG,SYS_LOG_TAG,fmt,vl);
            break;
        case AV_LOG_VERBOSE:
            ALOG(ANDROID_LOG_VERBOSE,SYS_LOG_TAG,fmt,vl);
            break;
        case AV_LOG_INFO:
            ALOG(ANDROID_LOG_INFO,SYS_LOG_TAG,fmt,vl);
            break;
        case AV_LOG_WARNING:
            ALOG(ANDROID_LOG_WARNING,SYS_LOG_TAG,fmt,vl);
            break;
        case AV_LOG_ERROR:
            ALOG(ANDROID_LOG_ERROR,SYS_LOG_TAG,fmt,vl);
            break;
    }
}
static void syslog_init(){
   av_log_set_callback(syslog_print); 
}
#endif
```
在代码初始化的地方调用`syslog_init()`后，就可以使用`av_log()`在Android平台输出调试日志了

