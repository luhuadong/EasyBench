# EasyBench

![](docs/EasyBench_banner.png)

EasyBench 是一个面向嵌入式 Linux 设备的轻量级出厂测试、硬件诊断和板级验证工具，支持图形界面、命令行执行、插件化测试项和测试报告导出。

通过模块化的设计思想对功能进行划分，主要包括：

- 板载硬件设备检测（包括液晶显示屏、触摸屏、摄像头、麦克风、硬盘、串口等等）
- 系统监控（包括 CPU、内存、硬盘、网络等系统资源）
- 设备功能配置（包括终端通道、工作模式、视频分辨率设置等）



## 安装依赖

Ubuntu / Debian：

```shell
sudo apt install cmake pkg-config \
    qtbase5-dev qtmultimedia5-dev \
    libasound2-dev libgl1-mesa-dev
```

使用 Qt 6 时额外安装：

```shell
sudo apt install qt6-base-dev qt6-multimedia-dev libqt6core5compat6-dev
```



## 编译步骤（CMake）

1. 配置并编译（自动优先选用已安装的 Qt 6，否则使用 Qt 5）：

   ```shell
   cmake -B build
   cmake --build build -j$(nproc)
   ```

2. 指定 Qt 版本：

   ```shell
   cmake -B build -DEASYBENCH_FORCE_QT5=ON   # 强制 Qt 5
   cmake -B build -DEASYBENCH_FORCE_QT6=ON   # 强制 Qt 6（需 Qt 6.2+）
   ```

3. 可执行文件位于 `build/easybench`。

4. 安装到系统（推荐）：

   ```shell
   sudo ./scripts/cmake-install.sh
   # 等价于: sudo cmake --install build --prefix /usr
   ```

   安装内容：`/usr/bin/easybench`、`/etc/easybench/easybench.conf`（首次创建，不覆盖已有配置）、`share/applications/easybench.desktop`、`share/easybench/deploy/`、`share/easybench/fonts/LiHeiPro.ttf`、Freedesktop 图标。安装结束时会尝试刷新图标缓存与 desktop 数据库。

5. 发布打包（CMake staging / CPack）：

   ```shell
   ./packaging/build-staging.sh          # dist/easybench-<ver>-linux-<arch>.tar.gz
   cd build && cpack -G TGZ             # 或 DEB（Debian/Ubuntu）
   ```

   详见 [packaging/README.md](packaging/README.md)。遗留自解压安装包：`./packaging/package.sh`。

6. 仅在当前用户目录安装启动器（无需 root，便于 Ubuntu 桌面调试）：

   ```shell
   chmod +x scripts/install-desktop-local.sh
   ./scripts/install-desktop-local.sh
   ```

7. 交叉编译时，在 `cmake -B build` 前加载目标平台的 SDK 环境（例如 Yocto 的 `environment-setup-*`），并确保 `CMAKE_PREFIX_PATH` 指向目标 Qt 安装路径。

8. 裁剪体积（嵌入式部署可选）：

   ```shell
   arm-poky-linux-gnueabi-strip build/easybench
   ```



## 目录结构

```
EasyBench/
├── CMakeLists.txt          # 构建入口
├── src/                    # 应用源码
│   ├── main.cpp
│   ├── app/                # 主窗口、配置、启动检查、路径解析
│   ├── widgets/            # 通用控件（PageWidget、StatusBar 等）
│   ├── pages/              # 功能页面（网络、升级、版本等）
│   └── modules/            # 功能模块（audio、network、update、monitor…）
├── config/                 # 运行时配置模板
├── deploy/                 # 产线工具与数据（eepromARMtool、I210 OTP）
├── resource/               # Qt 资源（qrc、图标、QSS、字体）
├── cmake/                  # EasyBenchInstall / Packaging、desktop 模板
├── packaging/              # staging、CPack、遗留 .run 安装包
├── scripts/                # 开发辅助脚本
├── docs/                   # 文档
└── legacy/                 # 遗留 qmake / JSON / Seat 样例
```

## 配置与路径

- 主配置模板：`config/easybench.conf`
- 搜索顺序：`/etc/easybench/` → `/etc/gbox/`（兼容）→ 程序目录旁 `config/` → 开发树 `config/`
- 产线工具：`deploy/bin/eepromARMtool`，数据：`deploy/data/I210NIC-origin.otp`（安装后位于 `share/easybench/deploy/`）

## 遗留 qmake 构建

`legacy/easybench.pro` 仅供参考，推荐使用 CMake。

## 注意事项

- 默认通过 CMake 检测 Qt 版本；摄像头页面在 Qt 5 与 Qt 6 下使用各自的多媒体 API。
- Qt 6 构建依赖 `Qt6::Core5Compat`，以兼容代码中的 `QTextCodec` 与 `QRegExp`。
- 中文字体由 `EbPaths::chineseFontFile()` 自动查找（安装路径、`resource/fonts/` 等）。
- 应用窗口与任务栏图标来自内嵌资源 `:/images/logo.png`；系统菜单/dock 图标名称为 `easybench`，需通过 `cmake --install` 或 `scripts/install-desktop-local.sh` 安装到图标主题路径。`StartupWMClass=easybench` 与 `QApplication::setDesktopFileName()` 配合，便于 Ubuntu dock 正确分组与固定。



## 更新图标

图标文件位于 resource 目录，如果更换了图标，只需重新编译即可（让 Qt 资源重新打包新图）：

```bash
cmake --build build -j$(nproc)
```

如果使用 Ubuntu 系统，且之前装过桌面图标，还要再装一次并刷新缓存，dock 才会显示新 logo：

```bash
sudo cmake --install build
# 或仅本机用户目录：
./scripts/install-desktop-local.sh
```

Ubuntu dock 若仍显示旧图，可注销再登录，或执行：

```bash
gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor
```



## 配色风格

主色调：橙黄色 `FFD54B`
