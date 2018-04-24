## 软件缩放

软件缩放库`libswscale`
进来ffmpeg添加了新的接口：libswscale来处理图像的缩放.
但是在前面我们使用`img_convert`来吧RGB转换成YUV12，我们现在使用新的接口。新街口更加标准和快速，而且我相信里面有了MMX优化代码。换句话说，它是做缩放更好的方式。

我们将用来缩放的基本函数是`sw_scale`.但一开始，我们必需建立一个SwsContext的概念。这将让我们进行想要的转换，然后把它传递给`sws_scale`函数。类似于SQL中预备阶段或者是在Python中编译的规则表达式regexp。要准备这个上下文，我们使用sws_getContext函数,它需要我们源的宽度和高度，我们想要的宽度和高度，源的格式和想要转换成的格式,同时还有一些其它的参数和标志.然后我们像使用`img_convert`一样来使用sws_scale函数，唯一不同的是我们传递给的是SwsContext:
```c
#include <ffmpeg/swscale.h>//include the header!

int queue_picture(VideoState*is,AVFrame*pFrame,double pts){
    static struct SwsContext*img_convert_ctx;
    ...
    if(vp->bmp){
        SDL_LockYUVOverlay(vp->bmp);
        dst_pix_fmt=PIX_FMT_YUV420P;
        pict.data[0]=vp->bmp->pixels[0];
        pict.data[1]=vp->bmp->pixels[2];
        pict.data[2]=vp->bmp->pixels[1];
        pict.linesize[0]=vp->bmp->pitches[0];
        pict.linesize[1]=vp->bmp->pitches[2];
        pict.linesize[2]=vp->bmp->pitches[1];
        //convert the image into YUV format that SDL uses
        if(img_convert_ctx==NULL){
            int w=is->video_st->codec->width;
            int h=is->video_st->codec->height;
            img_convert_ctx=sws_getContext(w,h,is->video_st->codec->pix_fmt,w,h,dst_pix_fmt,SWS_BICUBIC,NULL,NULL,NULL);
            if(img_convert_ctx==NULL){
                fprintf(stderr,"cannot initialize the conversion context!\n");
                exit(1);
            } 
        }
        sws_scale(img_convert_ctx,pFrame->data,pFrame->linesize,0,is->video_st->codec->height,pict.data,pict.linesize);
    }
}
```
我们把新的缩放器放到了合适的位置。希望这会让你知道libswscale做什么。


## 有待优化的问题
还有很多内容需要完成,很多要添加的性能:

* 错误处理。我们代码中的错误处理是无穷的，多处理一些会更好。
* 暂停。我们不能暂停电影，这是一个很有用的功能。我们可以在打结构体中使用一个内嵌暂停变量,当用户暂停的时候就设置它.然后我们的音频,视频和解码线程检测到它后就不再输出任何东西.我们也使用`av_read_play`来支持网络。这很容易解释，很是你却不能明显的计算出
* 按字节跳转。如果你可以按照字节而不是秒的方式来计算出跳转位置，那么对于像VOB文件一样的有不连续时间戳的视频文件来说,定位会更加精确.
* 丢弃帧.如果视频落后的太多,我们应当吧下一帧丢弃掉而不是设置一个短的刷新时间.
* 支持网络。现在电影播放器还不能播放网络媒体流
* 支持想YUV文件一样的原始视频流。如果我们的播放器支持的话，因为我们不能猜测出时基的大小，我们应该加入一些参数来进行相应的设置。
* 全屏
* 多种参数，例如：不同图像格式；参数ffplay.c中的命令开关
* 其他事情，例如;在结构体中的音频缓冲区应该对齐。
