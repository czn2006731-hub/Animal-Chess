# 斗兽棋 Animal Fight

一个部署在 Ubuntu 的双人在线斗兽棋：C++17 + Drogon 提供 HTTP 与 WebSocket 服务，Vue 3 多页面前端提供登录、匹配和对战体验。

## 目录

- `frontend/`：Vite 多页面前端（`login.html`、`match.html`、`room.html`）
- `backend/`：C++17 + Drogon 服务端，按用户、数据库、房间、工具、匹配和 Session 六个模块组织
- `database/`：MySQL 初始化脚本
- `deploy/`：后续部署用的 systemd、备份配置

## 本地前端开发

```bash
cd frontend
npm install
npm run dev
```

## 服务端构建（Ubuntu）

```bash
cmake -S backend -B backend/build -DCMAKE_BUILD_TYPE=Release
cmake --build backend/build -j"$(nproc)"
```

将 `backend/config.json.example` 复制为 `backend/config.json` 后，填入 MySQL 业务账号信息再启动服务。

