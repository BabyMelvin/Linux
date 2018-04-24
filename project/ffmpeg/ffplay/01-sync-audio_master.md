## 1.PTS和DTS
音频和视频流都是一些关于以多快速度和多少时间来播放它们的信息在里面。音频有**采样**，视频有**帧率**，如果简单通过帧数和帧率计算，很有可能不同步的情况。作为补充有DTS和PTS机制。谅解这两参数，需要了解电影的存放方式。B、P、I帧。B前后双向预测，P帧向前预测，I帧完整的一副图像。这也就也是了我们可能在调用`avcodec_decode_video`以后会得不到一帧图像。

对于电影，帧是这样显示的I B B P。现在我们需要在显示B帧之前知道P帧中的信息。因此，帧可能会按照这样的方式来存储：IPBB。

这就是为什么我们会有一个解码时间戳和一个显示时间戳的原因。
* 解码时间戳告诉我们什么时候需要解码
* 显示时间戳告诉我们什么时候需要显示。
在这种情况下，我们的流可以是这样的：
```c
STREAM: I P B B
DTS   : 1 2 3 4
PTS   ：1 4  3 2
```
**通常PTS和DTS只有在流中有B帧的时候会不同**
### 1.1得到PTS和DTS
当我们调用`av_read_frame()`得到一个包的时候，PTS和DTS的信息也会保存在包中。但是我们**真正想要的PTS是我们刚解码出来的原始帧的PTS**，这样我们才能知道什么时候来显示它。然而，我们从`avcode_decode_video()`函数中得到的帧只是一个`AVFrame`,其中并没有包含游泳的PTS值(**注意**：`AVFrame`并没有包含时间戳信息，但当我们得到的帧时候并不是我们想要的样子)。然而，ffmpeg重新排序包以便被`avcodec_decode_video（）`函数处理的包的DTS可以总是与其返回的PTS相同。但是，**另一个警告**：我们并不能总是得到这个信息。

不用担心，因为有另外一种办法可以找帧的PTS，我们可以让程序重新来排序包。我们保存一帧的第一个包的PTS：这将作为整个这一帧的PTS。我们可以通过函数`avcodec_decode_video()`来计算出哪个包是一帧的第一个包。
**怎么样实现？** 任何时候当一个包开始一帧的时候，`avcodec_decode_video()`将调用一个函数为一帧申请一个缓冲。当然，ffmpeg允许我们重新定义哪个分配内存的函数。所以我们制作了一个新的函数来保存一个包的时间戳。

当然，尽管那样，我们可能还是得不到一个正确的时间戳。我么将在后面处理这个问题。

## 2.同步
现在，知道了什么时候来显示一个视频帧真好，但是我们怎样来实际操作呢？

当我们显示了一帧以后，我们计算出下一帧的显示时间。然后我们简单设置一个新的定时器来。你可能会想，我们检查下一帧PTS值而不是系统时钟来看超时是否到。这种方式可以工作，但是两种情况要处理：

* 首先，要知道下一个PTS是什么。现在我们添加视频速率到我们PTS中---太对了！**然而，有些电影需要帧重复，这意味着我们重新播放当前帧**。这将导致程序显示下一帧太快了。所以我们需要重新计算它们。
* 第二，正如程序现在这样，视频和音频播放很欢快，一点也不受同步的影响。如果一切都工作的很好，我们不必担心。但是，你的**电脑**并不是最好的，很多**视频文件**也不是完好的。所以，我们有三种选择：`同步音频到视频`，`同步视频到音频`，`都同步到外部时钟(例如电脑时钟)`。从现在开始，我们将同步视频到音频。

### 2.1获得帧的时间戳
用代码来实现，将需要为我们的大结构体添加一些成员，但是我们会根据需要来做。首先，让我们看一下视频线程。记住，在这里我们得到了解码线程输出到队列的包。这里我们需要的是从`avcodec_decode_video`函数中得到帧的时间戳。我们讨论第一种方式是上次处理的包中得到DTS，这是很容易的：
```c
double pts;
for(;;){
	if(packet_queue_get(&is->videoq,packet,1)<0){
		//means we quit getting packets
		break;
	}
	pts=0;
	//decode video frame
	len1=avocdec_decode_video(is->video_st->codec,pFrame,&frameFinished,packet->data,packet->size);
	if(packet->dts!=AV_NOPTS_VALUE){
		pts=packet->dts;
	}else{
		pts=0;
	}
	pts*=av_q2d(is->video_st->time_base);
}
```
如果我们得不到PTS就把它设置为0。但是我们所说的如果DTS不能帮助我们，我们需要使用这一帧的第一个包的PTS。我们通过让ffmpeg使用我们自己的申请帧程序来实现。下面的是函数的格式：
```c
int get_buffer(struct AVCodecContext*c,AVFrame*pic);
void release_buffer(struct AVCodecContext*c,AVFrame*pic);
```
申请函数没有告诉我们关于包的任何事情，所以我们要自己每次在得到一个包的时候把PTS保存到一个全局变量中去。我们自己可以读到它。然后我们把值保存到AVFrame结构体中的变量中去。所以，开始函数：
```c
uint64_t global_video_pkt_pts=AV_NOPTS_VALUE;
int our_get_buffer(struct AVCodecCOntext*c,AVFrame*pic){
	int ret=avocdec_default_get_buffer(c,pic);
	uint64_t*pts=av_malloc(sizeof(uint64_t));
	*pts=global_video_pkt_pts;
	pic->opaque=pts;
	return ret;
}
void our_release_buffer(struct AVCodecContext*c,AVFrame *pic){
	if(pic)av_free(&pic->opaque);
	avcodec_default_release_buffer(c,pic);
}
```
函数`avcodec_default_get_buffer`和`avcodec_default_release_buffer`是ffmpeg中默认的申请缓冲函数，函数`av_free`是一个内存管理函数，它不但把内存释放而且把指针设置为NULL。
现在到了我们流打开的函数`stream_component_open`，我们添加几行来告诉ffmpeg如何去做：
```c
codecCtx->get_buffer=our_get_buffer;
codecCtx->release_buffer=our_release_buffer;
```
现在我们必要添加代码来保存PTS到全局变量中，然后在需要的时候来使用它。
```c
for(;;){
	if(packet_queue_get&is->videoq,packet,1)<0){
		//means we quit getting packets
		break;
	}
	pts=0;
	//save global pts to the stored in pFrame in first call
	global_video_pkt_pts=packet->pts;
	
	//decodec video frame
	len1=avcodec_decode_video(is->video_st->codec,pFrame,&frameFinished,packet->data,packet->size);
	if(packet->dts==AV_NOPTS_VALUE&&pFrame->opaque&&*(uint64_t*)pFrame->opaque!=AV_NOPTS_VALUE){
		pts=*(uint64_t*)pFrame->opaque;
	}else if(packet->dts!AV_NOPTS_VALUE){
		pts=packet->dts;
	}else{
		pts=0;
	}
	pts*=av_q2d(is->video_st->time_base);
}
```
**技术提示**：你可能已经注意到我们使用int64来表示PTS。这是因为PTS是以整型来保存的。这个值是一个时间戳相当于时间的度量，用来以流的`time_base`为单位进行时间度量。例如，如果一个流是24帧每秒，值为42的PTS表示这一帧应该排在第42帧的位置，如果我们每秒有24帧的话（这里并不完全正确)。

我们可以通过除以帧率来把这个值转化为秒。流中的time_base值表示`1/framerate`（对于固定帧率来说），所以得到秒为单位的PTS，需要乘以`time_base`。
### 2.2使用PTS来同步
现在我们得到了PTS。我们要注意前面讨论的两个同步的问题。我们将定义一个`synchronize_video`，它可以更新同步的PTS。**这个函数也能最终处理得到不到PTS的情况。**，同时我们要知道下一帧的时间以便于设置正确的**刷新率**我们可以使用内部的反应当前视频已播放时间的时钟`video_clock`来完成这个功能。我们把这些值添加到大结构体中
```c
typedef struct VideoState{
double video_clock;
```
下面是函数`synchronize_video`，它可以很好的自我注解:
```c
double synchronize_video(VideoState*is,AVFrame*src_frame,double pts){
	double frame_delay;
	if(pts!=0){
		is->video_clock=pts;
	}else{
		pts=is->video_clock;
	}
	frame_delay=av_q2d(is->video_st->codec->time_base);
	frame_delay+=src_frame->repeat_pict*(frame_delay*0.5);
	is->video_clock+=frame_delay;
	return pts;
}
```
你也会注意到我们计算了重复的帧。现在让我们得到正确的PTS并且使用`queue_picture`来队列化帧，添加一个新的时间戳参数pts；
```c
//did we get a video frame?
if(frameFinished){
	pts=sychronize_video(is,pFrame,pts);
	if(queue_picture(is,pFrame,pts)<0{
		break;
	}
}
```
对于queue_picture来说唯一改变的事情就是我们把时间戳值pts保存到VideoPicture结构体中，我们必需添加一个时间戳变量到结构体中:
```c
typedef struct VideoPicture{
	double pts;
}
int queue_picture(VideoState*is,AVFrame*PFrame,double pts){ert 
	if(vp->bmp){
		...convert picture
		vp->pts=pts;
		... alert queue
	}
}
```
现在我们图像队列中的所有图像都了正确的时间戳值，所以让我们看一下视频刷新函数。你会记得我们上次用`80s`的刷新时间来欺骗它，那么我们将会算出实际的值。

我们的**策略**是通过一个简单计算前一帧和现在这一帧的时间戳来预测下一个时间戳的时间。同时，我们需要同步视频到音频。我们将设置一个音频时间`audio_clock`；一个内部值记录了我们正在播放的音频的位置。就像从任意的mp3播放器中读出来的数字一样。既然我们把视频同步到音频，视频线程使用了一个值来算出是否太快还是太慢。

我们将在后面来实现这些代码:假设我们有音频时间函数`get_audio_clock`。一旦我们有这个函数，在音频和视频失去同步的时候应该做什么呢？

* 简单笨的方法试着用跳过正确帧或者其他方式来解决
* 替代方法，我们可以调整下一刷新的值:
	* 如果视频时间戳太落后音频时间，我们加倍计算延迟。
	* 如果时间戳太领先音频时间，我们加快刷新。

既然我们有了调整过的时间延迟，我们将它和我们通过`frame_timer`计算出来进行比较。这个帧时间`frame_timer`将会统计数电影播放中所有的延时。换句话说，这个`frame_timer`就是**指我们什么时候来显示下一帧**。我们简单的添加新的帧定时器延时，把它和电脑的系统时间进行比较，然后使用那个值来调度下一次刷新。
```c
void video_refresh_timer(void*userdata){
	VideoState*is=(VideoState*)userdata;
	VideoPicture*vp;
	double actual_delay,delay,sync_threshold,ref_clock,diff;
	if(is->video_st){
		if(is->pictq_size==0){
			schedule_refresh(is,1);
		}else{
			vp=&is->pictq[is->pictq_rindex];
			delay=vp->pts-is->frame_last_pts;
			if(delay<0||delay>=1.0){
				delay=is->frame_last_delay;
			}
			is->frame_last_delay=delay;
			is->frame_last_pts=vp->pts;
			ref_clock=get_audio_clock(is);
			diff=vp->pts-ref_clock;
			sync_threshold=(delay>AV_SYNC_THRESHOLD)?delay:AV_SYNC_THRESHOLD;
			if(diff<=-sync_threshold){
				delay=0;
			}else if(diff>=sync_threshold){
				delay=2*delay;
			}
			is->frame_timer+=delay;
			actual_delay=is->frame_timer-(av_gettime()/1000000.0);
			if(actual_delay<0.010){
				actual_delay=0.010;
			}
			schedule_refresh(is,(int)(actual_delay*1000+0.5));
			video_display(is);
			if(++is->pictq_rindex==VIDEO_PICTURE_QUEUE_SIZE){
				is->pictq_rindex=0;
			}
			SDL_LockMutex(is->pictq_mutex);
			is->pictq_size--;
			SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);
		}
	}else{
		schedule_refresh(is,100);
	}
}
```
我们在这里做了很多检查：

* 首先，我们保证现在的时间戳和上一个时间戳之间的处以delay是有意义的。如果不是的话，我们就猜测着用上次的延迟。
* 接着，我们有一个同步阀值，因为在同步的时候事情并不总是那么完美的。在ffplay中使用0.01作为它的值。我们也保证阀值不会比时间戳之间的间隔短。
* 最后，我们把最小的刷新值设置为10毫秒。

事实上这里我们应该跳过这一帧，但是我们不想为此烦恼。

我们给大结构体添加了很多变量，所以不要忘记检查一下代码。同时也不要忘记在函数`stream_component_open`中初始化时间`frame_timer`和前面的帧延迟`frame_delay`.
```c
is->frame_timer=()av_gettime(double)/100000.0;
is->frame_last_delay=4-e-3;
```
