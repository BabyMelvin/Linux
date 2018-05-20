#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "libavtil/avstring.h"
#include "libavformat/avformat.h"
#include "libacdevice/avdevice.h"
#include "libaccodec/opt.h"
#include "libswscale/swscale.h"

#define DECODED_AUDIO_BUFFER_SIZE 192000

struct options{
	int streadID;
	int frame;
	int nodec;
	int bplay;
	int thread_count;
	int64_t lstart;
	char finput[256];
	char foutput1[256];
	char foutput2[256];
};

int parse_options(struct options*opts,int argc,char**argv){
	int optidx;
	char* optstr;
	if(argc<2)return -1;
	opts->streamId=-1;
	opts->lstart=-1;
	opts->frames=-1;
	opts->foutput1[0]=0;
	opts->foutput2[0]=0;
	opts-nodec=0;
	opts->bplay=0;
	opts->thread_count=0;
	strcpy(opts->finput,argv[1]);
	optidx=2;
	while(){
	}
}

int main(int argc,char**argv){
	AVFormatContext* pCtx=0;
	AVCodecContext* pCodecCtx=0;
	AVCodec* pCodec=0;
	AVPacket packet;
	FILE* fpo1=NULL;
	FILE* fpo2=NULL;
	int nframe;
	int err;
	int got_picture;
	int picwidth,picheight,linesize;
	unsigned char*pBuf;
	int i;
	int64_t timestamp;
	struct options opt;
	int usefo=0;
	struct audio_dsp dsp;
	int duses;
	float usecs1=0;
	float usecs2=0;
	struct timeval elapsed1,elapsed2;
	int decoded=0;
	
	av_register_all();
	av_log_set_callback(log_callback);
	
	if(parse_options(&opt,argc,argv)<0||(strlen(opt.finput)==0){
		show_help(argv[0]);
		return 0;
	}
	err=avformat_open_input(&pCtx,opt.finput,0,0);
	if(err<0){
		printf("input error:%d",err);
		goto fail;
	}
	err=avformat_find_stream_info(pCtx,0);
	if(err<0){
		printf("fidn stream info:%d\n",err);
		goto fail;
	}
	if(opt.streamId<0){
		av_dump_format(pCtx,0,pCtx->filename,0);
		goto fail;
	}else{
		printf("额外数据流%d(%dB)",opt.streamId,pCtx->streas[opt.streamId]->codec->extradata_size);
		for(i=0;i<pCtx->streams[opt.streamId]->codec->extradata_size;i++){
			if(i%16==0)printf("\n");
			printf("%2x",pCtx->streams[opt.streamId]->codec->extradata[i]);
		}
	}
	//尝试打开输出文件
	if(strlen(opt.foutput1)&&strlen(opt.foutput2)){
		fpo1=fopen(opt.foutput1,"wb");
		fpo2=fopen(opt.foutput2,"wb");
		if(!fpo1||!fop2){
			printf("->error 打开输出文件\n");
			goto fail;
		}
		usefo=1;
	}else{
		usefo=0;
	}
	if(opt.streamId>=pCtx->nb_streams){
		printf("->streamId error\n");
		goto fail;
	}
	if(opt.lstart>0){
		err=av_seek_frame(pCtx,opt.streamId,opt.lstart,AV_SEEK_FLAG_ANY);
		if(err<0){
			printf("->av_seek_frame error\n");
			goto fail;
		}
	}
	//解码器设置
	if(!opt.nodec){
		//prepare codec
		pCodecCtx=pCtx->streams[opt.streamId]->codec;
		if(opt.thread_count<=16&& opt.thread_count>0){
			pCodecCtx->thread_count=opt.thread_count;
			pCodecCtx->thread_type=FF_THREAD_FRAME;
		}
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		if(!pCodec){
			printf("不能找到编码器");
			goto fail;
		}
		err=avcode_open2(pCodecCtx,pCodec,0);
		if(err<0){
			printf("avcode_open error\n");
			goto fail;
		}
		pFrame=avcodec_alloc_frame();
		//准备设备
		if(opt.bplay){
			//音频设备
			dsp.audio_fd=open(OSS_DEVICE,O_WRONLY);
			if(dsp.audio_fd==-1){
				printf("无法打开音频设备\n");
				goto fail;
			}
			dsp.channels=pCodecCtx->channels;
			dsp.speed=pCodecCtx->sample_rate;
			dsp.format=map_formats(pCodecCtx->sample_fmt);
			if(set_audio(&dsp)<0){
				printf("不能设置音频设备\n");
				goto fail;
			}
			//视频设备
		}
	}
	nframe=0;
	//dusecs解码时间, usecs总时间？
	while(nframe<opt.frames||opt.frames==-1){
		gettimeofday(&elapsed1,NULL);
		err=av_read_frame(pCtx,&packet);
		if(err<0){
			printf("av_read_frame error\n");
			goto fail;
		}
		gettimeofday(&elapsed2,NULL);
		dusecs=(elapsed2.tv_sec-elapsed1.tv_sec)*1000000+(elapsed2.tv_usec-elapsed1.tv_usec);
		usecs2+=dusecs;
		timestamp=av_rescale_q(packet.dts,pCtx->streams[packet.stream_index]%6lld,pts:%6lld",
							nframe++,packet.stream_index,packet.size,timestamp,packet.dts,packet.pts);
		if(packet.stream_index==opt.streamId){
#if 0
			for(i=0;i<16;/*packet_size;*/;i++){
				if(i%16==0)printf(\n pktdata);
				printf("%2x",packet.data[i]);
			}
			printf("/n");
#end if
			if(usefo){
				fwrite(packet.data,packet.size,1,fpo1);
				fflush(fpo1);
			}
			if(pCtx->streams[opt.streamId]->codec->codec_type==AVMEDIA_TYPE_VIDEO&&!opt.nodec){
				picheight=pCtx->streams[opt.streamId]->codec->height;
				picwidth=pCtx->streams[opt.streamId]->Codec->width;
				
				gettimeofday(&elapse1,NULL);
				avcodec_decodec_video2(pCodecCtx,pFrame,&got_picture,&packet);
				decoded++;
				gettimeofday(&elapsed2,NULL);
				dusecs(elapsed2.tv_sec-elapsed1.tv_sec)+1000+(elapsed2.tv_sec-elapsed1.tv_sec);
				usecs1+=dusecs;
				if(got_picture){
					printf("videotype:%d,ref%d,pts%lld,pkt.pts%lld,pkt_dts%lld",pFrame->pict_type,pFrame->refrence,pFrame->pts,pFrame->pkt_pts,pFrame->pkt_dts);
					if(pCtx->streams[opt.streamId]->codec->pix_fmt==PIX_FMT_YUV420P){
						if(usefo){
							linesize=pFrame->linesize[0];
							pBuf=pFrame->data[0];
							for(i=0;i<picheight;i++){
								fwrite(pBuf,picwidth,1,fpo2);
								pBuf+=linesize;
							}
							linesize=pFrame->linesize[1];
							pBuf=pFrame->data[1];
							for(i=0;i<picheight/2;i++){
								fwrite(pBuf,picwidth/2,1,fpo2);
								pBuf+=linesize;
							}
							linesize=pFrame->linesize[2];
							pBuf=pFrame->data[2];
							for(i=0;i<picheight/2;i++){
								fwrite(pBuf,picwidth/2,1,fpo2);
								pBuf+=linesize;
							}
							fflush(fpo2);
						}
						if(op.bplay){
							//show picture;
						}
						av_free_packet(&packet);
					}
				}else if(pCtx->streams[opt.streamId]->codec->codec_type==AVMEDIA_TYPE_AUDIO&&!opt.nodec){
					int got;
					gettimeofday(&elapse1,NULL);
					avcode_decode_audio4(pCodecCtx,pFrame,&got,&packet);
					decoded++;
					gettimeofday(&elapsed2,NULL);
					dusecs(elapsed2.tv_sec-elapsed1.tv_sec)+1000+(elapsed2.tv_sec-elapsed1.tv_sec);
					usecs1+=dusecs;
					if(got){
						printf("audio:%5dB raw data,decoding time:%d",pFrame->linesize[0],dusecs);
						if(usefo){
							fwrite(pFrame->data[0],pFrame->linesize[0],1,fpo2);
							ffluse(fpo2);
						}
						if(opt.bplay){
							play_pcm(&dsp,pFrame->data[0],pFrame-linesize[0]);
							
						}
					}
				}
				if(!opt.ndec&&pCodecCtx){
					avcodec_close(pCodecCtx);
				}
				if(!opt.nodec&&pCodecCtx){
					avcodec_close(pCodecCtx);
				}
				printf("帧解析,averaget%.2fhs per frame\n",nframe,uses2/nframe);
				printf("帧解码，平均%.2f per frame \n",deocded,usecs1/decoded);
			
			}
		}
	}
fail:
	if(!pCtx){
		avformat_close_input(&pCtx);
	}
	if(fpo1){
		fclose(fpo1);
	}
	if(fpo2){
		fclose(fpo2);
	}
	 if (!pFrame)
    {
        av_free(pFrame);
    }
    if (!usefo && (dsp.audio_fd != -1))
    {
        close(dsp.audio_fd);
    }
	return 0;
}