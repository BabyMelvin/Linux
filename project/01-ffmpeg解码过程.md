# FFMPEG解码流程

* 1.注册所有`容器格式`和`CODEC`:`av_register_all()`
* 2.打开文件:`av_open_input_file()`
* 3.从文件中提取流信息:`av_find_stream_info()`
* 4.穷举所有的流，查找其中种类为:`CODEC_TYPE_VIDEO`
* 5.查找对应的**解码器**:`avcodec_find_decoder()`
* 6.打开编解码器:`avcodec_open()`
* 7.为`解码帧`分配内存: `avcodec_alloc_frame()`
* 8.不停地从码流中提取出帧数据: `av_read_frame()`
* 9.判断帧的类型，对于视频帧调用:` avcodec_decode_video()`
* 10.解码完后，释放解码器:`avcodec_close()`
* 11.关闭输入文件:` avformat_close_input_file()`

## 1.基本概念
 `编解码器`、`数据帧`、`媒体流`和`容器`是数字媒体处理系统的四个基本概念。

首先需要统一术语：

* `容器／文件（Conainer/File）`即特定格式的多媒体文件。
* `媒体流（Stream）`：指时间轴上的一段连续数据，如一段声音数据，一段视频数据或一段字幕数据，可以是压缩的，也可以是非压缩的，压缩的数据需要关联特定的编解码器。
* `数据帧／数据包（Frame/Packet）`：通常，一个媒体流由大量的`数据帧`组成，对于`压缩数据`，帧对应着`编解码器`的最小处理单元。通常，分属于不同媒体流的数据帧交错复用于容器之中，参见交错。
* `编解码器`：编解码器以`帧`为单位实现`压缩数据`和`原始数据`之间的相互转换。

在FFMPEG中，使用`AVFormatContext`,`AVStream`、`AVCodecContext`、`AVCodec`及`AVPacket`等结构来抽象这些基本要素.

### 1.1 AVCodecContext
这是一个描述编解码器上下文的数据结构，包含了众多编解码器需要的参数信息，如下列出了部分比较重要的域：

```c
typedef struct AVCodecContext {
	/ **
     *一些编解码器需要/可以像使用extradata Huffman表。
     * MJPEG：Huffman表
     * RV10其他标志
     * MPEG4：全球头（也可以是在比特流或这里）
     *分配的内存应该是FF_INPUT_BUFFER_PADDING_SIZE字节较大
     *，比extradata_size避免比特流器，如果它与读prolems。
     * extradata按字节的内容必须不依赖于架构或CPU的字节顺序。
     * - 编码：设置/分配/释放由libavcodec的。
     * - 解码：由用户设置/分配/释放。
     * /
	uint8_t *extradata;
    int extradata_size;
	/ **
     *这是时间的基本单位，在条件（以秒为单位）
     *帧时间戳派代表出席了会议。对于固定fps的内容，
     *基应该1/framerate和时间戳的增量应该
     *相同的1。
     * - 编码：必须由用户设置。
     * - 解码：libavcodec的设置。
     * /
	 AVRational time_base;
	 /*#########视频##############*/
	 / **
     *图片宽度/高度。
     * - 编码：必须由用户设置。
     * - 解码：libavcodec的设置。
     *请注意：兼容性，它是可能的，而不是设置此
     * coded_width/高解码之前。
     * /
    int width, height;
    ......
	/*#########仅音频#############*/
	int sample_rate; ///< 每秒采样
    int channels;    ///< 音频通道数
	 / **
     *音频采样格式
     * - 编码：由用户设置。
     * - 解码：libavcodec的设置。
     * /
	 enum SampleFormat sample_fmt;  ///< 样本格式
	
    /*####下面的数据不应该被初始化####*/
	/ **
     *每包样品，初始化时调用“init”。
     * /
	 int frame_size;
    int frame_number;   ///<音频或视频帧数量
    char codec_name[32];
    enum AVMediaType codec_type; /* 看到AVMEDIA_TYPE_xxx */
    enum CodecID codec_id; /* see CODEC_ID_xxx */
	/ **
     *的fourcc（LSB在前，所以“的ABCD” - >（“D”<< 24）（“C”<< 16）（“B”<< 8）+“A”）。
     *这是用来解决一些编码错误。
     *分路器应设置什么是编解码器用于识别领域中。
     *如果有分路器等多个领域，在一个容器，然后选择一个
     *最大化使用的编解码器有关的信息。
     *如果在容器中的编解码器标记字段然后32位大分路器应该
     *重新映射到一个表或其他结构的32位编号。也可选择新
     * extra_codec_tag+大小可以添加，但必须证明这是一个明显的优势
     *第一。
     * - 编码：由用户设置，如果没有则默认基础上codec_id将使用。
     * - 解码：由用户设置，将被转换成在初始化libavcodec的大写。
     * /
	 unsigned int codec_tag;
	 ...
	 / **
     *在解码器的帧重排序缓冲区的大小。
     *对于MPEG-2，这是IPB1或0低延时IP。
     * - 编码：libavcodec的设置。
     * - 解码：libavcodec的设置。
     * /
	 int has_b_frames;
	 / **
     *每包的字节数，如果常量和已知或0
     *用于一些WAV的音频编解码器。
     * /
	 int block_align;
      / **
     *从分路器位每个样品/像素（huffyuv需要）。
     * - 编码：libavcodec的设置。
     * - 解码：由用户设置。
     * /
     int bits_per_coded_sample;  
}AVCodecContext;
```

* 如果是单纯使用`libavcodec`，这部分信息需要调用者进行初始化；
* 如果是使用整个FFMPEG库，这部分信息在调用`avformat_open_input`和`avformat_find_stream_info`的过程中根据文件的头信息及媒体流内的头部信息完成初始化。其中几个主要域的释义如下：
	* `extradata/extradata_size`：这个buffer中存放了解码器可能会用到的额外信息，在`av_read_frame`中填充。一般来说，首先，某种具体格式的demuxer在读取格式头信息的时候会填充extradata，其次，如果demuxer没有做这个事情，比如可能在头部压根儿就没有相关的编解码信息，则相应的parser会继续从已经解复用出来的媒体流中继续寻找。在没有找到任何额外信息的情况下，这个buffer指针为空。
	* `time_base`
	* `width/height`：视频的宽和高。
	* `sample_rate/channels`：音频的采样率和信道数目。
	* `sample_fmt`：音频的原始采样格式。
	* `codec_name/codec_type/codec_id/codec_tag`：编解码器的信息。

### 1.2 AVStream

该结构体描述一个媒体流，定义如下：

```c
typedef struct AVStream {
	int index;    /** <在AVFormatContext流的索引* /
	 int id;       /**< 特定格式的流ID */
	 AVCodecContext *codec; /**< codec context */
	  / **
     *流的实时帧率基地。
     *这是所有时间戳可以最低帧率
     *准确代表（它是所有的最小公倍数
     *流的帧率）。请注意，这个值只是一个猜测！
     *例如，如果时间基数为1/90000和所有帧
     *约3600或1800计时器刻度，，然后r_frame_rate将是50/1。
     * /
	 AVRational r_frame_rate;
	  / **
     *这是时间的基本单位，在条件（以秒为单位）
     *帧时间戳派代表出席了会议。对于固定fps的内容，
     *时基应该是1/framerate的时间戳的增量应为1。
     * /
    AVRational time_base;
	......
    / **
     *解码流量的第一帧，在流量时-base分。
     *如果你是绝对100％的把握，设定值
     *它真的是第一帧点。
     *这可能是未定义（AV_NOPTS_VALUE）的。
     *@注意的业余头不弱者受制与正确的START_TIME的业余
     *分路器必须不设定此。
     * /
    int64_t start_time;
	/ **
     *解码：时间流流时基。
     *如果源文件中没有指定的时间，但不指定
     *比特率，这个值将被从码率和文件大小的估计。
     * /
    int64_t duration;
	#if LIBAVFORMAT_VERSION_INT < (53<<16)
    char language[4]; /** ISO 639-2/B 3-letter language code (empty string if undefined) */
#endif
  /* av_read_frame（）支持* /
    enum AVStreamParseType need_parsing;
    struct AVCodecParserContext *parser;
    .....
   /*函数av_seek_frame（）支持* /
    AVIndexEntry *index_entries; / **<仅用于如果格式不notsupport寻求本身。* /
    int nb_index_entries;
    unsigned int index_entries_allocated_size;
    int64_t nb_frames;                 ///< 在此流的帧，如果已知或0
    ......
	//*平均帧率

    AVRational avg_frame_rate;
    ......
}AVStream;
```
主要域的释义如下，其中大部分域的值可以由avformat_open_input根据文件头的信息确定，缺少的信息需要通过调用avformat_find_stream_info读帧及软解码进一步获取：

* `index/id`：index对应流的索引，这个数字是自动生成的，根据index可以从`AVFormatContext::streams`表中索引到该流；而id则是流的标识，依赖于具体的容器格式。比如对于`MPEG TS`格式，`id`就是`pid`。
* `time_base`：流的时间基准，是一个实数，该流中媒体数据的`pts`和`dts`都将以这个时间基准为粒度。通常，使用`av_rescale/av_rescale_q`可以实现不同时间基准的转换。
* `start_time`：流的起始时间，以流的时间基准为单位，通常是该流中第一个帧的`pts`。
* `duration`：流的总时间，以流的时间基准为单位。
* `need_parsing`：对该流`parsing`过程的控制域。
*  `nb_frames`：流内的帧数目。
*  `r_frame_rate/framerate/avg_frame_rate`：帧率相关。
*  `codec`：指向该流对应的`AVCodecContext`结构，调用`avformat_open_input`时生成。
* `parser`：指向该流对应的`AVCodecParserContext`结构，调用`avformat_find_stream_info`时生成

### 1.3 AVFormatContext
这个结构体描述了一个媒体文件或媒体流的构成和基本信息，定义如下：
```c
typedef struct AVFormatContext {
	const AVClass *av_class; /**<由avformat_alloc_context设置的。* /
	struct AVInputFormat *iformat;
    struct AVOutputFormat *oformat;
	 void *priv_data;
    ByteIOContext *pb;
    unsigned int nb_streams;
    AVStream *streams[MAX_STREAMS];
    char filename[1024]; / **<输入或输出的文件名*/
	/*#########流信息#############*/
	int64_t timestamp;
#if LIBAVFORMAT_VERSION_INT < (53<<16)
    char title[512];
    char author[512];
    char copyright[512];
    char comment[512];
    char album[512];
    int year;  /**< ID3 year, 0 if none */
    int track; /**< track number, 0 if none */
    char genre[32]; /**< ID3 genre */
#endif
	int ctx_flags; /** <格式特定的标志，看到AVFMTCTX_xx* /
	/*分处理的私人数据（不直接修改）。* /
    / **此缓冲区只需要当数据包已经被缓冲，但
       不解码，例如，在MPEG编解码器的参数
       流。 * /
	struct AVPacketList *packet_buffer;
	/ **解码元件的第一帧的位置，在
       AV_TIME_BASE分数秒。从来没有设置这个值直接： 推导的AVStream值。 * /
	 int64_t start_time;
	 / **解码流的时间，在AV_TIME_BASE分数
       秒。只设置这个值，如果你知道没有个人流
       工期，也不要设置任何他们。这是从推导
       AVStream值如果没有设置。
	  int64_t duration;
	   / **解码：总的文件大小，如果未知0* /
       int64_t file_size;
	   / **解码：在比特/秒的总流率，如果不
       可用。从来没有直接设置它如果得到file_size和
       时间是已知的如FFmpeg的自动计算。 * /
	   int bit_rate;
	   /* av_read_frame（）支持* /
	    AVStream *cur_st;
#if LIBAVFORMAT_VERSION_INT < (53<<16)
    const uint8_t *cur_ptr_deprecated;
    int cur_len_deprecated;
    AVPacket cur_pkt_deprecated;
#endif
	/* av_seek_frame() 支持 */
    int64_t data_offset; /** 第一包抵消 */
    int index_built;
    int mux_rate;
    unsigned int packet_size;
    int preload;
    int max_delay;
#define AVFMT_NOOUTPUTLOOP -1
#define AVFMT_INFINITEOUTPUTLOOP 0
	** 次循环输出的格式支持它的数量 */
    int loop_output;
    int flags;
	#define AVFMT_FLAG_GENPTS  0x0001 ///< 生成失踪分，即使它需要解析未来框架。
#define AVFMT_FLAG_IGNIDX       0x0002 ///< 忽略指数。
#define AVFMT_FLAG_NONBLOCK    0x0004 ///<从输入中读取数据包时，不要阻止。
#define AVFMT_FLAG_IGNDTS       0x0008 ///< 忽略帧的DTS包含DTS与PTS
#define AVFMT_FLAG_NOFILLIN  0x0010 ///< 不要从任何其他值推断值，只是返回存储在容器中
#define AVFMT_FLAG_NOPARSE      0x0020 ///< 不要使用AVParsers，你还必须设置为FILLIN帧代码的工作，没有解析AVFMT_FLAG_NOFILLIN - >无帧。也在寻求框架不能工作，如果找到帧边界的解析已被禁用
#define AVFMT_FLAG_RTP_HINT     0x0040 ///< 暗示到输出文件添加的RTP
    int loop_input;
   /**解码：对探测数据的大小;编码：未使用。* /
    unsigned int probesize;
    / **
     在此期间，输入*最大时间（在AV_TIME_BASE单位）应
     *进行分析在avformat_find_stream_info（）。
     * /
    int max_analyze_duration;
    const uint8_t *key;
    int keylen;
    unsigned int nb_programs;
    AVProgram **programs;
   / **
     *强迫影片codec_id。
     * Demuxing：由用户设置。
     * /
    enum CodecID video_codec_id;
    / **
     *强迫音频codec_id。
     * Demuxing：由用户设置。
     * /
    enum CodecID audio_codec_id;
    / **
     *强制的：字幕codec_id。
     * Demuxing：由用户设置。
     * /
    enum CodecID subtitle_codec_id;
    / **
     *以字节为单位的最高限额为每个数据流的索引使用的内存。
     *如果该指数超过此大小，条目将被丢弃
     *需要保持一个较小的规模。这可能会导致较慢或更少
     *准确的寻求（分路器）。
     *分路器内存中的一个完整的指数是强制性的将忽略
     *此。
     *混流：未使用
     * demuxing：由用户设置* /
    unsigned int max_index_size;
   / **
     *以字节为单位的最高限额使用帧缓冲内存
     *从实时捕获设备获得。* /
    unsigned int max_picture_buffer;
    unsigned int nb_chapters;
    AVChapter **chapters;
    / **
     *标志启用调试。* /
    int debug;
#define FF_FDEBUG_TS        0x0001
    / **
     *原始数据包从分路器之前，解析和解码。
     *此缓冲区用于缓冲数据包，直到编解码器可以
     *确定，因为不知道不能做解析
     *编解码器。* /
    struct AVPacketList *raw_packet_buffer;
    struct AVPacketList *raw_packet_buffer_end;
    struct AVPacketList *packet_buffer_end;
    AVMetadata *metadata;
    / **
     *剩余的大小可为raw_packet_buffer，以字节为单位。
     *不属于公共API* /
#define RAW_PACKET_BUFFER_SIZE 2500000
    int raw_packet_buffer_remaining_size;
    / **
     *在现实世界中的时间流的开始时间，以微秒
     *自Unix纪元（1970年1月1日起00:00）。也就是说，pts= 0
     在这个现实世界的时间*流被抓获。
     * - 编码：由用户设置。
     * - 解码：未使用。 * /
    int64_t start_time_realtime;

	
}AVFormatContext;
```
这是FFMpeg中最为基本的一个结构，是其他所有结构的根，是一个多媒体文件或流的根本抽象。其中:

* `nb_streams`和`streams`所表示的AVStream结构指针数组包含了所有内嵌媒体流的描述；
*  `iformat`和`oformat`指向对应的`demuxer`和`muxer`指针；
*   `pb`则指向一个控制底层数据读写的`ByteIOContext`结构。
* `start_time`和`duration`是从streams数组的各个AVStream中推断出的多媒体文件的起始时间和长度，以`微妙`为单位。

通常，这个结构由`avformat_open_input`在内部创建并以缺省值初始化部分成员。但是，**如果调用者希望自己创建该结构**，则需要显式为该结构的一些成员置缺省值——如果没有缺省值的话，会导致之后的动作产生异常。以下成员需要被关注：

*  probesize
*  mux_rate
*  packet_size
*  flags
*  max_analyze_duration
*  key
*  max_index_size
*  max_picture_buffer
*  max_delay

### 1.4 AVPacket
AVPacket定义在avcodec.h中，如下：
```c
typedef struct AVPacket {
	/ **
     AVStream->基time_base单位介绍时间戳的时间
     *解压缩包将被提交给用户。
     *可AV_NOPTS_VALUE如果没有存储在文件中。
     *分必须大于或等于DTS作为演示不能发生之前
     *减压，除非要查看十六进制转储。有些格式滥用
     * DTS和PTS/ CTS的条款意味着不同的东西。如时间戳
     *必须转换为真正的PTS / DTS之前，他们在AVPacket存储。 * /
    int64_t pts;
	/ **
     AVStream->基time_base单位时间的减压时间戳记;
     *包解压。
     *可AV_NOPTS_VALUE如果没有存储在文件中。 * /
	int64_t dts;
    uint8_t *data;
    int   size;
    int   stream_index;
    int   flags;
	/ **
     *这个包的时间AVStream->基time_base单位，如果未知。
     *等于next_pts - 在呈现顺序this_pts。* /
    int   duration;
    void  (*destruct)(struct AVPacket *);
    void  *priv;
    int64_t pos;                            ///< 如果未知字节的位置，在流，-1
	/ **
     * AVStream->基time_base单位的时差，这点
     *包从解码器输出的已融合在哪个点
     *独立的前一帧的情况下。也就是说，
     *框架几乎是一致的，没有问题，如果解码开始从
     *第一帧或从这个关键帧。
     * AV_NOPTS_VALUE如果不明。
     *此字段是不是当前数据包的显示时间。
     *
     *这一领域的目的是允许在流，没有寻求
     *在传统意义上的关键帧。它所对应的
     *恢复点SEI的H.264和match_time_delta在螺母。这也是
     *必不可少的一些类型的字幕流，以确保所有
     *后寻求正确显示字幕。* /
    int64_t convergence_duration;
	
} AVPacket;
```
FFMPEG使用`AVPacket`来暂存`解复用之后、解码之前的媒体数据`（一个音/视频帧、一个字幕包等）及附加信息（解码时间戳、显示时间戳、时长等）。其中：

*  `dts`表示解码时间戳，`pts`表示显示时间戳，它们的单位是所属媒体流的时间基准。
*   `stream_index`给出所属媒体流的索引；
*    `data`为数据缓冲区指针，`size`为长度；
*    `duration`为数据的时长，也是以所属媒体流的时间基准为单位；
*  `pos`表示该数据在媒体流中的字节偏移量；   
*  `destruct`为用于释放数据缓冲区的函数指针；
*   `flags`为标志域，其中，最低为置1表示该数据是一个关键帧。

AVPacket结构本身只是个容器，它使用data成员引用实际的数据缓冲区。这个缓冲区通常是由`av_new_packet`创建的，但也可能由FFMPEG的API创建（如`av_read_frame`）。当某个AVPacket结构的数据缓冲区不再被使用时，要需要通过调用`av_free_packet`释放。`av_free_packet`调用的是结构体本身的`destruct`函数，它的值有两种情况：

* 1)av_destruct_packet_nofree或0；
* 2)av_destruct_packet，其中，情况1)仅仅是将data和size的值清0而已，情况2)才会真正地释放缓冲区。

FFMPEG内部使用AVPacket结构建立缓冲区装载数据，同时提供destruct函数，如果FFMPEG打算自己维护缓冲区，则将destruct设为`av_destruct_packet_nofree`，用户调`av_free_packet`清理缓冲区时并不能够将其释放；如果FFMPEG打算将该缓冲区彻底交给调用者，则将destruct设为`av_destruct_packet`，表示它能够被释放。安全起见，如果用户希望自由地使用一个FFMPEG内部创建的AVPacket结构，最好调用`av_dup_packet`进行缓冲区的克隆，将其转化为缓冲区能够被释放的AVPacket，以免对缓冲区的不当占用造成异常错误。`av_dup_packet`会为`destruct`指针为`av_destruct_packet_nofree`的AVPacket新建一个缓冲区，然后将原缓冲区的数据拷贝至新缓冲区，置data的值为新缓冲区的地址，同时设destruct指针为`av_destruct_packet`。
