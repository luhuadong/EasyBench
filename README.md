## DeviceCab

DeviceCab 是一个运行于嵌入式 Linux 平台，集系统监控、设备测试和席位功能配置于一体的软件工具。DeviceCab 基于 Qt 图形界面库进行开发，采用 C++ 编程语言。

通过模块化的设计思想对功能进行划分，主要包括：

- 板载硬件设备检测（包括液晶显示屏、触摸屏、摄像头、麦克风、硬盘、串口等等）
- 系统监控（包括CPU、内存、硬盘、网络等系统资源）
- 席位功能配置（包括席位通道、工作模式、视频分辨率设置等）



### 安装依赖

```shell
sudo apt install libpulse-dev
sudo apt install libgl1-mesa-dev
```



### 编译步骤

1. 进入工程目录，配置编译环境

   ```shell
   $ source /opt/fsl-imx-x11/3.14.52-1.1.0/environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi 
   ```

2. 根据 gyt_box.pro 生成 Makefile 构建文件

   ```shell
   $ qmake
   ```

3. 编译可执行程序

   ```shell
   $ make
   ```

4. 裁剪可执行程序的体积

   ```shell
   $ arm-poky-linux-gnueabi-strip gyt_box
   ```

5. 检查

   ```shell
   $ file gyt_box
   gyt_box: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, for GNU/Linux 2.6.32, BuildID[sha1]=d6509b96c10dd2b9d837e6125d1e68dcf4b1cba0, stripped
   ```




### 注意事项

默认使用 Qt5，如果你使用 Qt4，需要在 gyt_common.h 文件中做修改。将如下内容

```c
#define QT_VERSION_4     0  /* Recommend to use Qt5 strongly */
#define QT_VERSION_5     1
```

修改为

```c
#define QT_VERSION_4     1  /* Recommend to use Qt5 strongly */
#define QT_VERSION_5     0
```

并在 gyt_box.pro 文件将下面这一行注释掉

```shell
QT += multimedia multimediawidgets
```

