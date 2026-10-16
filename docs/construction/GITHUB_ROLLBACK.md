# Git 与回滚标准

## 仓库模型

- 正式本地仓库：当前 Git 工作树的仓库根目录，也是当前工程工作目录。
- GitHub 远端：`git@github.com:NeilBaumanMax/SmartRollator.git`。
- 按用户 2026-10-16 指令，`main` 保存当前完整状态并作为继续开发入口；原 build/phase-1-bootstrap 保留。先核对 HANDOFF 和实时 Git 状态，不默认重建旧阶段分支。
- 早期按旧交接方案建立的旁路辅助克隆不再作为正式施工入口或事实源。
- 旧工程首次纳入 Git 前的证据仍由 `reference/archive-20260807` 和 `reference_manifest.sha256` 保留。

## 远端与参考归档

- GitHub 远端：`https://github.com/NeilBaumanMax/SmartRollator.git`。
- `main`：正式施工主线，包含完整可追踪工程；只接收达到当前阶段验收条件的增量。
- `backup/bootstrap-YYYYMMDD`：指向首个治理/施工基线，用于恢复重建起点。
- `reference/archive-YYYYMMDD`：纳入正式 Git 施工前的完整资产归档；与 `main` 使用独立根提交，不合并到施工主线。
- `build/phase-N-short-name`：阶段施工分支；验收前不得把未验证状态合入 `main`。

“全量归档”是指完整保存后续取证所需的源码、CubeMX 配置、Keil 工程定义、生成源码、第三方源码/库、文档、图表和可复现工具；不包括可再生构建产物、IDE 用户状态、操作系统垃圾、临时渲染预览、密钥、隐私数据或机器绝对路径配置。归档分支不是发布分支，其源码存在、旧日志或旧二进制均不构成新固件验证证据。

### 首次分支拓扑

```text
reference/archive-YYYYMMDD  A  纳入 Git 前的旧工程资产快照（独立根提交）

main                        B--C--D  治理基线、审计记录、完整工程基线
backup/bootstrap-YYYYMMDD   B        首个治理基线回滚点
build/phase-N-short-name    D--E...   后续阶段施工
```

远端默认分支应为 `main`。若空仓库首次推送后默认分支不是 `main`，必须在 GitHub 设置中修正并记录；不得删除归档分支来掩盖配置问题。

## 纳入与排除规则

### 必须纳入参考归档

- `SmartRollator.ioc`、`Core/`、`Drivers/`、`Middlewares/`。
- 可移植的 Keil 工程定义、启动文件、链接配置和手写模块。
- `AGENTS.md`、`README.md`、`docs/`、工程 `tools/`，以及集中于 `Files/` 的算法演示、研究说明、生成器和必要图表。旧归档仍保留归档时路径，不追改清单。
- `reference_manifest.sha256`，记录归档候选文件的 SHA-256，便于证明旧参考内容未被误改。

### 必须排除

- `MDK-ARM/SmartRollator/`、`Objects/`、`Listings/` 等编译输出目录。
- `*.o`、`*.d`、`*.crf`、`*.axf`、`*.hex`、`*.map`、构建日志和可再生成的 listing。
- `*.uvguix.*`、`.vscode` 日志/锁文件、`.DS_Store`、临时预览和 Python 缓存。
- 密钥、令牌、个人配置、受试者原始数据及超过托管限制且无 LFS 方案的大文件。

排除旧构建日志意味着不能从仓库声称旧固件通过；如需保留某份日志作为历史证据，应先脱敏并放入 `docs/reference-evidence/`，同时明确其对应旧源码而非当前 `main`。

## Phase 1 首次建立

1. 在正式当前目录执行 `git init -b main`，配置提交身份和 GitHub 远端。
2. 对完整工程按上述纳入/排除规则生成并验证 `reference_manifest.sha256`。
3. 保留 `reference/archive-YYYYMMDD` 独立根提交，作为纳入 Git 前的只读证据。
4. 把正式本地 `main` 绑定到远端 `main`，完整工程作为后续提交纳入，不覆盖既有治理历史。
5. 精确暂存完整工程并推送 `main`；保留 `backup/bootstrap-YYYYMMDD`。
6. 从最新 `main` 创建 `build/phase-1-bootstrap`，后续生成最小工程。
7. 核对所有远端分支哈希、默认分支和当前工作树；把实际值写入 HANDOFF。

任何一步的实际 URL、分支和提交哈希都写入 HANDOFF；本文不预填虚假值。

## 分支与提交

完整的文件级提交序列见 [Git 文件级施工树](GIT_CONSTRUCTION_TREE.md)。该文件是“下一批文件为何创建、同一提交包含哪些文件、运行什么测试”的权威入口；本文件只定义 Git 安全规则。

- 当前继续开发入口为 `main`（用户明确指定）；需要独立隔离阶段时可使用 `build/phase-N-short-name`，纯文档治理可使用 `docs/short-name`。无论分支如何选择，均保留备份、精确提交和测试规则。
- 阶段备份建议：`backup/phase-N-YYYYMMDD`。
- 提交信息建议：`docs: establish construction baseline`、`feat(force): parse validated sensor frames`。
- 一个提交对应一个可解释、可测试、可回滚的增量。

## 精确暂存

```bash
git status --short
git diff -- <path>
git add <explicit-file-1> <explicit-file-2>
git diff --cached --check
git diff --cached --stat
```

禁止默认使用 `git add .` 或 `git add -A`。

## 回滚优先级

1. 从已推送备份创建恢复分支。
2. 对错误提交使用 `git revert <commit>` 保留历史。
3. 对未提交的自有小改动使用精确反向补丁。
4. 只有用户明确授权且目标已核对时，才考虑破坏性命令。

默认禁止：`git reset --hard`、`git clean -fd`、`git push --force`、对不明文件执行 `git checkout --`/`git restore`。

## 推送后的核验

记录本地 `HEAD`、远端目标分支哈希、工作树状态和备份分支。使用 `git ls-remote --heads origin` 逐一核对 `main`、bootstrap 备份和参考归档；只有远端哈希匹配，才可把“已备份/已推送”写入进度或交接。

## GitHub 保护建议

- `main` 禁止强制推送和删除，合并前要求构建/测试检查通过。
- 阶段成熟后启用至少一人审阅；单人施工期也保留 PR 或可审阅提交记录。
- 发布标签只从 `main` 创建；`v0.1.0-parity` 必须同时指向可复现构建与验证证据。
- GitHub 仓库可见性由产品负责人决定；不得从 URL 或本地凭据推断其公开/私有状态。
