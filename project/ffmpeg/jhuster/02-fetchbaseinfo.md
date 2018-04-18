利用ffmpeg库拿到码流的一些基本信息。
## 1.码流中哪些信息值得关注？
* 是否包含：音频、视频
* 码流的封装格式
* 视频的编码格式
* 音频的编码格式
* 视频的分辨率、帧率、码率
* 音频的采样率、位宽、通道数
* 码流的总时长
* 其他metadata信息，如作者、日期等。

## 2.为什么需要拿到这些信息 ？
* 码流的封装格式-------------->解封装
* 音频、视频的编码格式---------->初始化解码器
* 视频的分辨率、帧率、码率------->视频渲染
* 音频的采样率、位宽、通道数------>初始化音频播放器
* 码流的总时长----------------->展示、拖动
* 其他Metadata信息------------->展示

## 3.关键信息都藏在哪？
这些关键的媒体信息，被称作 **metadata**，常常记录在整个码流的开头或者结尾处，例如：
* **wav 格式**主要由 wav header 头来记录音频的采样率、通道数、位宽等关键信息
* mp4 格式，则存放在 moov box 结构中；
* 而 FLV 格式则记录在 onMetaData 中等等。

我们可以看看 FLV 格式的 onMetaData 记录的信息包含有哪些内容：

|property Name | Type | Comment|
|-----|-----|-----|
|audiocodecid|Number|Audio codec ID used in the file|
|audiodatarate|Number|Audio bit rate in kiobits per second|
|audiodelay|Number|Delay introduced by the audio codec in seconds|
|audiosamplerate|Number|Frequency at which the audio stream in replayed|
|audiosampelsize|Number|Resolution of a single audio sample|
|canSeekToEnd|Boolean|Indicating the last video frame is a key frame|
|creationdatae|String|Creation date and time|
|duration|Number|Total duration of the file in seconds|
|filesize|Number|Total size of the file in bytes|
|framerate|Number|Number of frames per second|
|height|Number|Height of the video in pixels|
|stereo|Boolean|indicating stereo audio|
|videocodecid|Number|Video codec ID int the file|
|videodatarate|Number|Video bit rate in kilobits per second|
|width|Number|Width of the video in pixes|

当然，并不是所有的码流都能简单地通过 "metadata" 解析出这些媒体信息，有些码流还需要通过试读、解码等一系列复杂的操作判断之后，才能准确地判断真实的媒体信息，在 ffmpeg 中，函数 `avformat_find_stream_info`就是干这事的。

## 4.如何从 ffmpeg 取出这些信息 ？
### （1）首先打开码流，并解析“metadata”
播放器要完成的第一件事，就是**打开码流**，然后再**解析码流信息**，在 ffmpeg 中，这两步任务主要通过 `avformat_open_input` 和 `avformat_find_stream_info`函数来完成.前者负责服务器得连接和码流头部信息的拉取，后者则主要负责媒体信息的探测和分析工作，这两部实例代码如下：
```c
AVFormatContext *ic=avformat_alloc_context();
if(avformat_open_input(&ic,url,NULL,NULL)<0){
	LOGE("could not open source %s",url);
	return -1;
}
if(avformat_find_stream_info(ic,NULL)<0){
	LOGE("could not fine stream informations");
	return -1;
}

```
当这两步执行成功后，媒体信息就已经成功保存在了 ffmpeg 相关的结构体成员变量中了，下一步我们看看如何拿到这些信息，为我所用。
### （2）利用 ffmpeg 系统函数 dump 码流信息
ffmpeg 提供了一个函数直接帮助你打印出解析到的媒体信息，用法如下：
```c
av_dump_format(ic,0,ic->filename,0);
```
不过，这样打印的信息还不够，我们希望能通过代码取到每一个关键的媒体信息。因此，下面我们看看如何直接从 AVFormatContext 上下文结构体中提取这些信息。

###（3）手动从 ffmpeg 的上下文结构体中提取

首先，我们看看 AVFormatContext 变量有哪些跟媒体信息有关的成员变量：
```c
struct AVInputFormat*iformat;//记录了封装格式信息
unsigned int nb_stream;//记录了该URL中包含有几路流
AVStream**streams;//一个结构体数组，每个对象记录了一条流的详细信息。
int64_t start_time;//第一帧时间戳
int64_t duration;//码流总时长
int bit_rate;//码流的总码率，bps
AVDictionary *metadata;//一些文件信息头，key/value字符串
```
由此可见，封装格式、总时长和总码率可以拿到了。另外，由于 `AVStream **streams` 还详细记录了每一路流的媒体信息，可以进一步挖一挖，看看它有哪些成员变量。

我们通过 av_find_best_stream 函数来取出指向特定指定路数的 AVStream 对象，比如视频流的 AVStream 和 音频流的 AVStream 对象分别通过如下方法来取到：
```c
int video_stream_idx=av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0)；
AVStream vidoe_stream=ic->streams[video_stream_idx];

int audio_stream_idx=av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
AVStream audio_stream=ic->streams[audio_stream_idx];
```
拿到了 video_stream 和 audio_stream ，我们就可以把 AVStream 结构体中的信息提取出来了，其关键的成员变量如下：
```c
AVCodecContext *codec;//记录了该码流的编码信息
int64_t start_time;//第一帧的时间戳
int64_t duration;//该码流的时长
AVDictionary *metadata;//一些文件信息头，key/value字符串
AVRational avg_frame_rate;//平均帧率
```

到这里，我们拿到了平均的帧率，其中，AVCodecContext 详细记录了每一路流的具体的编码信息，我们再进一步挖一挖，看看 AVCodecContext 有哪些成员变量。
```c
const struct AVCodec*codec;//编码详细信息
enum AVCodecID codec_id;//编码类型
int bit_rate;//平均码率

/*vidoe only*/
int width,height;//图像的宽高尺寸，码流中不一定存在该信息，会由解码后覆盖
enum AVPixelFormat pix_fmt;//原始图像的格式，码流不一定存在该信息，会在解码后覆盖

/*audio only*/
int sample_rate;//音频的采样率
int channels;   //音频的通道数
enum AVSampleFormat sample_fmt;//音频的格式，位置
int frame_size;//每个音频帧的sample个数
```
原来我们最关心的编码类型、图片的宽高、音频的参数藏在这里了！经过层层解析后，我们想要的媒体信息，基本上在这些结构体变量中都找到了。
