# Packaging

## 推荐：CMake 安装与 CPack

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 系统安装（/usr）
sudo ./scripts/cmake-install.sh

# 或生成发布用目录树
./packaging/build-staging.sh
# 产物: dist/tuxibit-<version>-linux-<arch>.tar.gz

# CPack 安装包（在 build 目录）
cd build
cpack -G TGZ
cpack -G DEB    # 需要 dpkg-deb，依赖见 cmake/TuxiBitPackaging.cmake
```

## 安装布局（PREFIX=/usr）

| 路径 | 内容 |
|------|------|
| `/usr/bin/tuxibit` | 主程序 |
| `/etc/tuxibit/tuxibit.conf` | 站点配置（首次安装创建，不覆盖已有） |
| `/etc/tuxibit/tuxibit.conf.example` | 配置模板 |
| `/usr/share/applications/tuxibit.desktop` | 桌面项 |
| `/usr/share/tuxibit/deploy/` | 产线工具与 OTP 数据 |
| `/usr/share/tuxibit/fonts/` | 可选中文字体 |
| `/etc/gbox/tuxibit.conf` | 兼容符号链接（指向新路径） |

## 遗留自解压包

面向无法使用 `cmake --install` 的嵌入式产线环境：

```bash
./packaging/package.sh   # 生成 tuxibit.run
sudo ./tuxibit.run
```

内部仍调用 `packaging/install.sh`，逻辑与 CMake 安装布局一致。
