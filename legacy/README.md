# Legacy artifacts

本目录存放已不再作为主构建路径使用的遗留文件，仅供参考或产线兼容：

- `easybench.pro` — 旧 qmake 工程（主构建请用 CMake）
- `easybench.json` — 历史版本信息 JSON，已由 `config/` + 系统探测替代
- `seat/` — 原 `conf/` 下 Seat 产线 XML 配置样例

新配置请使用仓库根目录下的 `config/easybench.conf`。

应用源码已迁入 `src/`（`app/`、`widgets/`、`pages/`、`modules/`），请使用 CMake 构建。
