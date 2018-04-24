# 快进快退
处理快进快退命令,现在我们来为我们的播放器加入一些快进和快退的功能,因为如果你不能全局搜索一部电影是很让人讨厌的。同时，这将告诉你`av_seek_frame`是多么容易使用;

我们在将电影播放中使用左右向键和方向键来表示向后和向前一小段，使用上下键表示向前向后一大段。这里一小段是10秒，一大段是60秒。所以我们需要设置我们的主循环来捕捉键盘事件。然而我们捕捉到键盘事件后不能直接调用`av_seek_frame`函数。我们主要的解码线程`decode_thread`的循环中做这些。所以，我们要添加变量到大结构体中，用来包含新的跳转位置和一些跳转标志:
```c
int seek_req;
int seek_flags;
int64_t seek_pos;
```
现在让我们在主循环中捕捉按键:
```c
for(;;){
    double incr,pos;
    SDL_WaitEvent(&event);
    switch(event.type){
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLk_LEFT:
                    incr=-10.0;
                    goto do_seek;
                case SDLK_RIGHT:
                    incr=10.0;
                    goto do_seek;
                case SDLK_UP:
                    incr=60.0;
                    goto do_seek;
                case SDLK_DOWN:
                    incr=60.0;
                    goto do_seek;
                    do_seek:
                    if(global_video_state){
                        pos=get_master_clock(global_video_state);
                        pos+=incr;
                        stream_seek(global_video_state,(int64_t)(pos*AV_TIME_BASE),incr);
                    }

            }
            break;
        default:
            break;
    }
    break;
}
```
为了检查按键，我们先查一下是否有`SDL_KEYDOWN`事件。然后我们使用`event.key.keysym.sym`来判断哪个按键被按下.一旦我们知道了如何跳转，我们就来计算新的时间，方法为把增加的时间值加到从函数get_master_clock中得到的时间上。然后我们调用`stream_seek`函数来设置`seek_pos`等变量。我们新的时间转化为`avcodec`中的内部时间戳单位。在流中调用那个时间戳将使用帧而不是秒来计算，公式为`seconds=frames*time_base(fps)`.默认的avcodec的值为1,000,000fps(所以2秒的内部时间戳为2,000,000);在后面我们来看下一下为什么要把这个值进行转换。

这就是我们的`stream_seek`函数。请注意我们设置了一个标志为后退服务:
```c
void stream_seek(VideoState*is,int64_t pos,int rel){
    if(!is->seek_req){
        is->seek_pos=pos;
        is->seek_flags=rel<0?AVSEEK_FLAG_BACKWARD:0;
        is->seek_req=1;
    }
}
```
现在让我们看一下如果在`decode_thread`中实现跳转。你会注意到我们已经在源文件中标记了一个叫做"seek stuff goes here"的部分.现在代码将写在这里。
跳转是围绕`av_seek_frame`的函数的。 这里函数用到了一个格式上下文，一个流，一个时间戳和一组标记来作为它的参数.这个函数将会跳转到你所给的时间戳的位置。时间戳单位是你传递给函数的留的时基time_base。 然而，你并不是必需要给它一个流(流可以用-1代替). 如果你这样做了，时基time_base将会是avcodec中的内部时间戳单位,或者是1000000fps。这就是为什么我们在设置seek_pos的时候会把位置乘以`AV_TIME_BASER`的原因.

但是，如果给`av_seek_frame`函数的stream参数传递-1，你有时会在播放某些文件的时候遇到问题(比较少见)，所以我们会取文件中的第一个流并且把它传递给`av_seek_frame`函数。不要忘记我们也要把时间戳`timestamp`的单位进行转化。

```c
if(is->seek_req){
    int stream_index=-1;
    int64_t seek_target=is->seek_pos;
    if(is->videoStream>=0){
        stream_index=is->videoStream;
    }else if(is->audioStream>=0){
        stream_index=is->audioStream;
    }
    if(stream_index>=0){
        seek_target=av_rescale_q(seek_target,AV_TIME_BASE_Q,pFormatCtx->streams[stream_index]->time_base);
    }
    if(av_seek_frame(is->pFrameCtx,stream_index,seek_target,is->seek_flags)<0){
        fprintf(stderr,"%s,error while seeking\n",is->pFormatCtx->filename);
    }else{
}
```
这里`av_rescalse_q(a,b,c)`是用来把时间戳从一个时基调整到另一个时基时候调用。它的基本动作是计算`a*b/c`,但是这个函数还是必要的，因为直接计算会溢出的情况。`AV_TIME_BASE_Q`是`AV_TIME_BASE`作为分母后的版本。它们是很不相同的：

* `AV_TIME_BASE*time_in_seconds=avcodec_timestamp`而`AV_TIME_BASE_Q*avcodec_timestamp=time_in_seconds`(注意:`AV_TIME_BASE_Q`实际上是一个AVRational对象，所以你必需使用avcodec中特定的q函数来处理它)

## 清空缓冲
我们已经正确设置了跳转位置，但是我们还没有结束。记住我们有一个堆放了很多包的队列。既然我们跳转到了不同的位置，我们必需把队列中的内容清空否则电影是不会跳转的。不仅如此，avcodec也有它自己的内部缓冲,也需要每次被清空。

要实现这个，我们需要首先写一个函数来清空我们的包队列。然后我们需要一种命令声音和视频线程来清空avocdec内部缓冲的办法。我们可以在清空队列后把特定的包放入到队列中，然后当它们检测到特定的包时候，它们就会把自己的内部缓冲清空。

让我们开始写清空函数。其实很简单的，所以我直接把代码写在下面。
```c
static void packet_queue_flush(PacketQueue *q){
    AVPacketList *pkt,*pkt1;
    SDL_LockMutex(q->mutex);
    for(pkt=q->first_pkt;pkt!=NULL;pkt=pkt1){
        pkt1=pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt=NULL;
    q->first_pkt=NULL;
    q->nb_packets=0;
    q->size=0;
    SDL_UnlockMutex(q->mutex);
}
```
既然队列已经清空了，我们放入"请空包"。但是开始我们要定义和创建这个包:
```c
AVPacket flush_pkt;
main(){
...
    av_init_packet(&flush_pkt);
    flush_pkt.data="FLUSH";
...
}
```

现在我们把这个包放到队列中:
```c
}else{
  if(is->audioStream>=0){
    packet_queue_flush(&is->audioq);
    packet_queue_put(&is->audioq,&flush_pkt);
  }
  if(is->videoStream>=0){
    packet_queue_flush(&is->videoq);
    packet_queue_put(&is->videoq,&flush_pkt);
  }
}
```
这些代码片段是接着前面`decode_thread`中的代码片段的.我们也需要修改`packet_queue_put`函数才不至于直接简单复制了这个包:
```c
int packet_queue_put(PacketQueue *q,AVPacket *pkt){
    AVPacketList *pkt1;
    if(pkt!=&flush_pkt&&av_dup_packet(pkt)<0){
        return -1;
    }
}
```
然后在声音线程和视频线程中，我们在`packet_queue_get`后立即调用函数`avocdec_flush_buffers`:
```c
if(packet_queue_get(&is->audioq,pkt,1)<0){
    return -1;
}
if(packet->data==flush_pkt.data){
    avcodec_flush_buffers(is->audio_st->codec);
    continue;
}
```
上面代码片段与视频线程中的一样，只要把"audio"换成"video".


