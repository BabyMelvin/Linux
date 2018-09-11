# 1.FFmpeg使用

## 1.1命令格式

```
ffmpeg -i {输入文件路径} -b:v {输出视频码率} {输出视频文件路径}
```

命令行参数

|参数|说明|
|--|--|
|-h|帮助|
|-i filename|输入文件|
|-t duration|设置处理时间，格式为hh:mm:ss|
|-ss position|设置起始时间，格式为hh:mm:ss|
|-b:v bitrate|设置视频码率|
|-b:a bitrate|设置音频码率|
|-r fps|设置帧率|
|-s wxh|设置帧大小，格式为WxH|
|-c:v codec|设置视频解码器|
|-c:a codec|设置音频编码器|
|-ar freq|设置音频采样率|

# 2.ffplay使用
## 2.1命令格式

```
ffplay {输入文件路径}
```

## 2.2快捷键

|快捷键|说明|
|--|--|
|q,ESC|退出|
|f|全屏|
|p,空格|暂停|
|鼠标点击屏幕|跳转到指定位置|