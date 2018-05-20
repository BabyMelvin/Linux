## `avformat_open_input`

`int avformat_open_input(AVFormatContext **ic_ptr, const char *filename, AVInputFormat *fmt, AVDictionary **options);`

avformat_open_input完成两个任务：

* 打开一个文件或URL，基于字节流的底层输入模块得到初始化。
* 解析多媒体文件或多媒体流的头信息，创建`AVFormatContext`结构并填充其中的关键字段，依次为各个原始流建立`AVStream`结构。

一个多媒体文件或多媒体流与其包含的原始流的关系如下：

* 多媒体文件/多媒体流 `(movie.mkv)`
	* 原始流 1  (h.264 video)
	* 原始流 2  (aac audio for Chinese)
	* 原始流 3  (aac audio for english)
	* 原始流 4  (Chinese Subtitle)
	* 原始流 5  (English Subtitle)

关于输入参数：

*  `ic_ptr`，这是一个指向指针的指针，用于返回`avformat_open_input`内部构造的一个`AVFormatContext`结构体。
*  `filename`，指定文件名。
*   `fmt`，用于显式指定输入文件的格式，如果设为`空`则自动判断其输入格式。
*   `options`

这个函数通过解析多媒体文件或流的头信息及其他辅助数据，能够获取足够多的关于文件、流和编解码器的信息，但由于任何一种多媒体格式提供的信息都是有限的，而且不同的多媒体内容制作软件对头信息的设置不尽相同，此外这些软件在产生多媒体内容时难免会引入一些错误，因此这个函数并不保证能够获取所有需要的信息，在这种情况下，则需要考虑另一个函数：`avformat_find_stream_info`

### `avformat_find_stream_info`

`int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);`

这个函数主要用于获取必要的编解码器参数，设置到`ic→streams[i]→codec`中。

首先必须得到各媒体流对应编解码器的类型和id，这是两个定义在`avutils.h`和`avcodec.h`中的枚举：
```c
enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,
    AVMEDIA_TYPE_NB
};
enum CodecID {
    CODEC_ID_NONE,
    /* video codecs */
    CODEC_ID_MPEG1VIDEO,
    CODEC_ID_MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
    CODEC_ID_MPEG2VIDEO_XVMC,
    CODEC_ID_H261,
    CODEC_ID_H263,
    ...
};

```
通常，如果某种媒体格式具备完备而正确的头信息，调用`avformat_open_input`即可以得到这两个参数，但若是因某种原因`avformat_open_input`无法获取它们，这一任务将由`avformat_find_stream_info`完成。
其次还要获取各媒体流对应编解码器的时间基准。此外，对于音频编解码器，还需要得到：

*  采样率
*   声道数
*    位宽
*  帧长度（对于某些编解码器是必要的）

 对于视频编解码器，则是：

*   图像大小
*    色彩空间及格式

### `av_read_frame`

`int av_read_frame(AVFormatContext *s, AVPacket *pkt);`

这个函数用于从多媒体文件或多媒体流中读取媒体数据，获取的数据由`AVPacket`结构pkt来存放。对于音频数据，如果是固定比特率，则pkt中装载着一个或多个音频帧；如果是可变比特率，则pkt中装载有一个音频帧。对于视频数据，pkt中装载有一个视频帧。需要注意的是：`再次调用本函数之前`，必须使用`av_free_packe`t释放pkt所占用的资源。
通过`pkt→stream_index`可以查到获取的媒体数据的类型，从而将数据送交相应的解码器进行后续处理。

### `av_seek_frame`

`int av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp, int flags);`

这个函数通过改变媒体文件的读写指针来实现对媒体文件的随机访问，支持以下三种方式：

*  基于时间的随机访问：具体而言就是将媒体文件读写指针定位到某个给定的时间点上，则之后调用av_read_frame时能够读到时间标签等于给定时间点的媒体数据，通常用于实现媒体播放器的快进、快退等功能。
*  基于文件偏移的随机访问：相当于普通文件的seek函数，timestamp也成为文件的偏移量。
*   基于帧号的随机访问：timestamp为要访问的媒体数据的帧号。

关于参数：

* s：是个`AVFormatContext`指针，就是`avformat_open_input`返回的那个结构。
* `stream_index`：指定媒体流，如果是基于时间的随机访问，则第三个参数timestamp将以此媒体流的时间基准为单位；如果设为负数，则相当于不指定具体的媒体流，FFMPEG会按照特定的算法寻找缺省的媒体流，此时，timestamp的单位为`AV_TIME_BASE`（微秒）。
* `timestamp`：时间标签，单位取决于其他参数。
* flags：定位方式，`AVSEEK_FLAG_BYTE`表示基于字节偏移，`AVSEEK_FLAG_FRAME`表示基于帧号，其它表示基于时间。

### `av_close_input_file`

`void av_close_input_file(AVFormatContext *s);`
关闭一个媒体文件：释放资源，关闭物理IO。

### `avcodec_find_decoder`

```c
AVCodec *avcodec_find_decoder(enum CodecID id);
AVCodec *avcodec_find_decoder_by_name(const char *name);
```
根据给定的`codec id`或`解码器名称`从系统中搜寻并返回一个`AVCodec`结构的指针。

### `avcodec_open`

```c
int avcodec_open(AVCodecContext *avctx, AVCodec *codec);
```
此函数根据输入的`AVCodec`指针具体化`AVCodecContext`结构。在调用该函数之前，需要首先调用`avcodec_alloc_context`分配一个`AVCodecContext`结构，或调用`avformat_open_input`获取媒体文件中对应媒体流的`AVCodecContext`结构；此外还需要通过`avcodec_find_decoder`获取`AVCodec`结构。
这一函数还将初始化对应的`解码器`。

### `avcodec_decode_video2`

`int avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, AVPacket *avpkt);`

解码一个视频帧。`got_picture_ptr`指示是否有`解码数据`输出。输入数据在AVPacket结构中，输出数据在AVFrame结构中。AVFrame是定义在avcodec.h中的一个数据结构：
```c
typedef struct AVFrame {
    FF_COMMON_FRAME
} AVFrame;
```
FF_COMMON_FRAME定义了诸多数据域，大部分由FFMpeg内部使用，对于用户来说，比较重要的主要包括：

```c
#define FF_COMMON_FRAME \
......
    uint8_t *data[4];\
    int linesize[4];\
    int key_frame;\
    int pict_type;\
    int64_t pts;\        
    int reference;\   
......
```
FFMpeg内部以`planar`的方式存储原始图像数据，即将图像像素分为`多个平面（R/G/B或Y/U/V）`，`data`数组内的指针分别指向`四个像素平面的起始位置`，`linesize`数组则存放各个`存贮`各个平面的缓冲区的`行宽`：

```c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++data[0]->#################################++++++++++++
++++++++++++###########picture data##########++++++++++++
            ........................
++++++++++++#################################++++++++++++  
|<-------------------line_size[0]---------------------->|

```
此外，`key_frame`标识该图像是否是关键帧；`pict_typ`e表示该图像的编码类型：`I(1)/P(2)/B(3)……`；`pts`是以`time_base`为单位的时间标签，对于部分解码器如`H.261`、`H.263`和`MPEG4`，可以从`头信息`中获取；`reference`表示该`图像`是否被用作参考。

### `avcodec_decode_audio4`

`int avcodec_decode_audio4(AVCodecContext *avctx, AVFrame *frame, int *got_frame_ptr, AVPacket *avpkt);`

解码一个`音频帧`。输入数据在AVPacket结构中，输出数据在frame中，`got_frame_ptr`表示是否有数据输出。

### `avcodec_close`

`int avcodec_close(AVCodecContext *avctx);`
关闭解码器，释放`avcodec_open`中分配的资源。

