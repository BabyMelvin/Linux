# 同步音频
我们将采用和**视频**一样的方式：做一个内部视频时钟记录视频线程播放了多久，然后同步音频到上面去。后面我们也看一下如何推广，把音频和视频都同步到外部时钟上面去。
## 生成一个视频时钟
类似声音时钟的视频时钟:一个给出当前视频播放时间内部值。开始，你可能会这和使用上一帧的时间戳来更新定时器一样简单。但是，不要忘记了视频之间的时间间隔很长的，以毫秒计量的。解决办法是跟踪另外一个值:**我们再设置上一帧时间戳的时候的时间值**。

于是当前视频时间值就时`PTS_of_last_frame+(current_time-time_elapsed_since_PTS_value_was_set)`。这种解决方式与我们函数`get_audio_clock`中的方式类似。

所在我们大结构体中，我们将放上一个双精度浮点变量`video_current_pts`和一个64位宽整型变量`video_current_pts_time`.时钟更新将被放在`video_refresh_timer`函数中。
```c
void video_refresh_timer(void*userdata){
   if(is->video_st){
        if(is->pictq_size==0){
            schedule_refresh(is,1);
        }else{
            vp=&is->pictq[is->pictq_rindex];
            is->video_current_pts=vp->pts;
            is->video_current_pts_time=av_gettime();
        }
   }
}
```
不要忘记在`stream_componet_open`函数初始化它:
```c
is->video_current_pts_time=av_gettime();
```
现在我们需要一种得到消息的方式：
```c
double get_video_clock(VideoState *is){
    double delta;
    delta=(av_gettime()-is->video_current_pts_time)/1000000.0;
    return is->video_current_pts+delta;
}
```
## 提取时钟
但是为什么要强制使用视频时钟呢？我们更改视频同步代码以致于音频和视频不会试着去相互同步。想想一下我们让它像ffplay一样有一个命令行参数。所以让我们抽香一个这样事情：我们将做一个新的封装函数`get_master_clock`，用来检测`av_sync_type`变量然后决定调用`get_audio_clock`还是`get_video_clock`获其它的想使用的获得时钟的函数。我们甚至可以使用电脑时钟，这个函数我们叫做`get_external_clock`:
```c
enum{
   AV_SYNC_AUDIO_MASTER,
   AV_SYNC_VIDEO_MASTER,
   AV_SYNC_EXTERNAL_MASTER,
};
#define DEFAULT_AV_SYNC_TYPE AV_SYNC_VIDEO_MASTER
double get_master_clock(VideoState*is){
    if(is->av_sync_type==AV_SYNC_VIDEO_MASTER){
        return get_video_clock(is);
    }else if(is->av_sync_type==AV_SYNC_AUDIO_TYPE){
        return get_audio_clock(is);
    }else{
        return get_external_clock(is);
    }
}
main(){

    is->av_sync_type=DEFAULT_AV_SYNC_TYPE;
}
```
## 同步音频
现在最难的部分：同步音频到视频时钟。我们的策略是测量声音的位置，把它与视频时间比较然后算出我们需要修正多少的样本数，也就是说：***我们是否需要通过丢弃样本的方式来加速播放还是需要通过插值样本的方式来放慢播放？**.

我们将在每次处理声音样本的时候运行一个`synchronize_audio`的函数来正确的收缩或者扩展声音样本。然而，我们不想在每次发现有偏差的时候进行同步，**因为这样会使同步音频多于视频包**。所以我们为函数`synchronize_audio`设置一个最小连续值来限定需要同步的时刻,这样我们就不会总是在调整.当然,就像那次那样,"失去同步"意味着声音时钟和视频时钟的差异大于我们的阀值.

所以我们将使用一个分数系统，叫c，所以现在可以说我们得到了N个失去同步的声音样本。失去同步的数量可能会有很多变化，所以我们要计算一下失去同步的长度的均值。例如，第一次调用的时候，显示出来我们失去的长度为40ms,下次变为50ms等等。但是我们不会使用一个简单的均值，因为距离现在最近的值比靠前的值更重要的多。所以我们分数系统叫c,然后用这样的公式来计算差异:`diff_sum=new_diff+diff_sum*c`. 当我们准备好去找平均差异的时候，我们用简单的计算方式:`avg_diff=diff_sum*(1-c)`

注意：为什么在这里？这个公式看起来很神奇！嗯，它基本上是一个使用等比级数的加权平均值。我不知道这个是否有名字,但是如果想要更多的信息，这里一个解释。
```c
int synchronize_audio(VideoState*is,short*samples,int sample_size,double pts){
    int n;
    double ref_clock;
    n=2*is->audio_st->codec->channels;
    if(is->av_sync_type!=AV_SYNC_AUDIO_MASTER){
        double diff,avg_diff;
        int wanted_size,min_size,max_size,nb_samples;
        ref_clock=get_master_clock(is);
        diff=get_audio_clock(is)-ref_clock;
        if(diff<AV_NOSYNC_THRESHOLD){
            //accumulate the diffs
            is->audio_diff_cum=diff+is->audio_diff_avg_coef*is->audio_diff_cum;
            if(is->audio_diff_avg_count<AUDIO_DIFF_AVG_NB){
                is->audio_diff_avg_count++;
            }else{
                avg_diff=is->audio_diff_cum*(1.0-is->audio_diff_avg_coef);
            }
        }else{
            is->audio_diff_avg_count=0;
            is->audio_diff_cum=0;
        }
    }
    return samples_size;
}
```
现在我们已经做得很好；我们已经近似的知道如何用视频或者其它的时钟来调整音频了。所以让我们计算一下要在添加和砍掉多少样本，并且如何在"shrinking/expanding buffer code"部分来写上代
```c
if(fabs(avg_diff)>=is->audio_diff_threshold){
    wanted_size=sample_size+((int)(diff*is->audio_st->codec->sample_rate)*n);
    min_size=samples_size*((100-SAMPLE_CORRECTION_PERCENT_MAX)/100);
    max_size=samples_size*((100+SAMPLE_CORRECTION_PERCENT_MAX)/100);
    if(wanted_size<min_size){
        wanted_size=min_size;
    }else if(wanted_size>max_size){
        wanted_size=max_size;
    }
}
```
记住`audio_length*(sample_rate*# of channels*2)`就是`audio_length`秒时间的声音的样本数。所以，我们想要样本数就是我们根据声音偏移添加或者减少后的声音样本数。我们也可以设置一个范围来限定我们一次进行修正的长度，**因为如果我们改变的太多**,就会听到刺耳的声音。

## 修正样本

现在我们要真正的修正一下声音。你可能会注意到我们的同步函数`synchronize_audio`返回一个样本数，这可以告诉我们有多少个字节 被送到流中。所以我们只要调整样本数为`wanted_size` 就可以了。这会让样本更小一些。但是如果我们想让它变大，我们不能只是让样本大小变大,因为在缓冲中没有多余的数据!所以我们必须添加上去.但是，如何添加？最笨的办法就是试着来推算声音，所以让我们用已有的数据在缓冲的末尾添加上最后的样本。
```c
if(wanted_size<sample_size){
    samples_size=wanted_size;
}else if(wanted_size>samples_size){
    uint8_t*sample_end,*q;
    int nb;
    nb=(samples_size-wanted_size);
    samples_end=(uint8_t*)samples+samples_size-n;
    q=samples_end+n;
    while(nb>0){
        memcpy(q,sample_end,n);
        q+=n;
        nb-=n;
    }
    samples_size=wanted_size;
}
```
现在我们通过这个函数返回的样本数。我们现在要做的是使用它:
```c
void audio_callback(void*userdate,Uint8* stream,int len){
    VideoState*is=(VideoState*)userdata;
    int len1,audio_size;
    double pts;
    while(len>0){
        if(is->audio_buf_index>=is->audio_buf_size){
            audio_size=audio_decode_frame(is,is->audio_buf,sizeof(is->audio_buf),&pts);
            if(audio_size<0){
                is->audio_buf_size=1024;
                memset(is->audio_buf,0,is->audio_buf_size);
            }else{
                audio_size=synchronize_audio(is,(int16_t*)is->audio_buf,audio_size,pts);
                is->audio_buf_size=audio_size;
            }
        }
    }
}
```
我们要做的是把  `synchronize_audio`插入进去。(同时，保证在初始化上面变量检查一下代码)
结束的最后一件事：我们需要添加一个if语句来保证我们不会在视频为主时钟的时候也来同步视频.
```c
if(is->av_sync_type!=AV_SYNC_VIDEO_MASTER){
    ref_clock=get_master_clock(is);
    diff=vp->pts-ref_clock;
    sync_threshold=(delay>AV_SYNC_THRESHOLD)?delay:AV_SYNC_THRESHOLD;
    if(fabs(diff)<AV_NOSYNC_THRESHOLD){
        if(diff<=-sync_threshold){
           delay=0; 
        }else if(diff>=sync_threshold){
            delay=2*delay;
        }
    }
}
```

