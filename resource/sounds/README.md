# 测试铃声资源

将音频文件放在此目录，在 `resource/sounds.qrc` 中注册后重新编译，即可在「声音」页面「测试音」下拉框中选用。

- 内置测试音：程序生成的 PCM（纯音、双音、扫频等）
- 资源文件：如 `for_elise_tune.mp3`（通过 Qt Multimedia 播放，需系统解码器支持 MP3）
