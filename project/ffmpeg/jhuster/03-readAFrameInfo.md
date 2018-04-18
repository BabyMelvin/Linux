之前如何提取整个音视频码流的媒体信息，包括：封装格式、编码格式、视频的分辨率、帧率、码率、音频的采样率、位宽、通道数等等，而本文则关注得更细一点，看看如何利用 ffmpeg 读取码流中每一帧的信息。

## 1.  每一帧的哪些信息值得关注 ？
* 音频帧还是视频帧
* 关键帧还是非关键帧
* 帧的数据和大小
* 时间戳信息

## 2.  为什么要关注这些信息 ？
* 音频帧还是视频帧 ----> 分别送入音频/视频解码器
* 关键帧还是非关键帧 --> 追帧优化
* 帧的数据和大小 -----> 取出帧的内容
* 时间戳信息 ---------> 音视频同步

## 3. 如何从 ffmpeg 取出这些信息 ？
ffmpeg 提供了一个函数 `av_read_frame` 来完成解封装的过程，它会从码流里面提取每一个音频、视频帧，它使用了结构体`AVPacket`来记录每一帧的信息。
读取一帧数据的代码示例如下（ic 即为 AVFormatContext 对象，码流的上下文句柄）：

```c
AVPacket avpkt;
av_init_packet(&avpkt);
while(!interrupt){
	int ret=av_read_frame(ic,&avpkt);
	if(ret<0){
		break;
	}
	//processing
}

av_free_packet(&avpkt);
```
每循环一次，就从码流中解封装并且提取了一帧数据，并存放在了 AVPacket 结构体中。

### 3.1 如何判断是音频帧还是视频帧
上一篇文章我们提到过，使用下面的方法，获取码流中的 `video_stream_idx` 和 `audio_stream_idx`
```c
int video_stream_idx=av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
int audio_stream_idx=av_find_best_stream(ic,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
```
那么，此时就派上用场了，每一个 AVPacket 都有一个成员变量：stream_index，由该成员变量即可判断这个 Packet 到底是音频还是视频了：
```c
if(avpkt.stream_index==video_stream_idx){
	LOGD("read a video frame");
}else if(avpkt.stream_index==audio_stream_idx){
	LOGD("read audio frame");
}
```
### 3.2 如何判断是否为关键帧
判断是否为关键帧的方法也比较简单，示例如下：
```c
if(avpkt.flags&AV_PKT_FLAG_KEY){
	LOGD("read a key frame");
}
```

### 3.3 如何获取帧的数据和大小
帧的数据和大小直接定义在 AVPacket 结构体中，对应的成员变量如下：
```c
//压缩编码的数据，一帧音频/视频
uint8_t*data;

//数据的大小
int size;
```

### 3.4 如何获取帧的时间戳信息
每一个帧都可能携带**有2个时间戳信息**，一个是**解码时间戳**`dts`，一个是**显示时间戳**`pts`，解码时间戳告诉我们什么时候需要解码，显示时间戳告诉我们什么时候需要显示，只有在码流中存在`B帧`的情况下，这两个时间戳才会不一致。

这些时间戳信息不一定存在于码流中（取决于生产端），如果不存在，则其值为：`AV_NOPTS_VALUE`.

一定要选择正确地方式打印时间戳，时间戳是使用 long long 来表示的，即 int64_t，因此打印的时候，需要使用`%lld`来打印，例如：
```c
while(!interrupt){
	int ret=av_read_frame(player->ic,&avpkt);
	if(ret<0){
		break;
	}
	if(avpkt.stream_idx==video_stream_idx){
		LOGD("read video frame,timestamp=%lld\n",avpkt.pts);
	}else if(avpkt.stream_idx==audio_stream_idx){
		LOGD("read audio frame,timestamp=%lld\n",avpkt.pts);
	}
}
```
由此，我们就可以通过这些 log 信息调试一下某一段音视频流的时间戳是否正确，比如是否出现了时间戳的回滚和错乱，则必然会导致播放端出现音视频不同步或者显示异常等情况。