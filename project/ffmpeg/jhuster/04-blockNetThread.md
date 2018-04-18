前面的文章有提到如何提取码流信息、如何读每一帧的数据，这些都是离不开网络操作，例如：使用 ffmpeg 读取一个码流，常规的代码流程示例如下：

```c
AVFormatContex *ic=avformat_alloc_context();
if(avformat_open_input(&ic,url,NULL,NULL)<0){
	return -1;
}
if(avformat_find_stream_info(ic,NULL)){
	return -1;
}

AVPacket avpkt;
av_init_packet(&avpkt);
while(!abort_request){
	int ret=av_read_frame(ic,&avpkt);
	if(ret<0){
		break;
	}
	//processing
}
av_free_packet(&avpacket);
```
其中:
* `avformat_open_input `主要负责连接媒体服务器，以及读取码流的头信息
* `av_read_frame`主要负责每次读取一帧数据，包括解协议和解封装这两个函数，都有可能会出现耗时很长或者阻塞的情况，比如：
	* 网络很烂或者很不稳定
	* 服务器响应比较慢
	* 直播流不存在或者没有数据
	
因此，我们需要一个中断机制，在等待超时或者退出播放的时候，就可以轻松中断掉这个阻塞过程。

ffmpeg 提供了一个很简单的回调机制，即注册一个自定义的回调函数，用于外部中断阻塞的网络操作，用法如下所示： 
```c
static int custom_interrupt_callback(void*arg){
	if(timeout||abort_request){
		return 1;
	}
	return 0;
}
AVFormatContext *ic=avformat_alloc_context();
ic->interrupt_callback=custom_interruput_callback;
ic->interrupt_callback.opaque=custom_arg;
```
当自定义的回调函数返回 1，则会产生中断。因此，我们可以在等待超时或者退出播放器的时候，将 timeout 或者 abort_request 置为 1 来达到中断当前的网络阻塞过程的目的。
