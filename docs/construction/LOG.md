# 施工日志

本文件只追加实际发生的工作。必须保留首次失败、修复和复测；未运行的测试明确写 `Not run`。

## 2026-08-07 — 建立施工文档体系

### Plan Replay

研究 Catnip-Forge 的 docs 导航、架构、开发、Git、进度、日志、交接和 feature construction 文档；研究 idea-to-production-vibecoding 的 docs-first、阶段计划、测试、Git 安全和交接规则；适配为嵌入式重建项目。

### Actual Changes

- 建立根级 Agent 规则与 docs 唯一索引。
- 冻结 SmartRollator V1 产品边界、非目标和成功标准。
- 建立 L0–L5 架构、接口责任和唯一安全命令出口。
- 定义 Phase 0–11 重建路线及各阶段验收/回滚点。
- 建立测试证据等级、Git/回滚、工具、决策、假设、进度与交接规范。

### Files Changed

仅新增 `AGENTS.md` 与 `docs/product/`、`docs/construction/` 下 Markdown 文档；未修改固件源码、CubeMX、Keil 或硬件配置。

### Test Log

- 文档存在性与相对链接：Passed。检查 32 个 Markdown 文件，所有相对链接均存在。
- 固件编译：Not run（排除项）。
- 目标板/台架：Not run（排除项）。

### Failures / Fixes / Retests

未观察到文档生成或链接检查执行失败。相对链接检查首次运行即通过。

### Documentation Drift

旧版 `docs/handover/` 仍作为历史参考；新施工以 `docs/INDEX.md` 和 construction 文档为准，已在索引明确区分。

### Git Status

Git baseline、branch、commit、push：Not established。本阶段未初始化 Git。

### Rollback Judgment

当前新增文档可在进入新仓库前人工备份；正式远端回滚点必须由 Phase 1 建立。

### Risks / Next Step

最大风险是硬件参数和参考工程行为尚未逐项验证。下一步只执行 Phase 1，不提前接入电机控制。

## 2026-08-07 — Phase 1 Git 双轨基线（进行中）

### Plan Replay

产品负责人指定远端 `https://github.com/NeilBaumanMax/SmartRollator.git` 并要求完善 Git 设计后全量推送。采用 `main` 干净重建主线、`reference/archive-20260807` 旧资产独立归档和 `backup/bootstrap-20260807` 回滚点的双轨方案。

### Actual Changes

- 扩充 Git 设计，冻结远端、分支拓扑、归档边界、排除规则、核验和保护建议。
- 新增仓库 README、`.gitignore` 和 ADR-006。
- 建立参考归档候选清单；可再生构建产物、IDE 用户状态、临时预览和密钥类文件不纳入 Git。

### Test Log

- 远端引用检查：`git ls-remote` 未返回引用，按空仓库处理；推送后必须再次核对。
- 敏感信息模式扫描：未发现匹配项；此结果不能替代 GitHub secret scanning。
- Markdown 相对链接：Passed，33 个 Markdown 文件的相对目标均存在。
- 归档候选：1577 个文件（清单本身除外），约 249 MiB；未发现单文件超过 100 MB。
- 固件编译、烧录、目标板和台架：Not run（本增量排除项）。

### Failures / Fixes / Retests

- 首次 SHA-256 清单生成：Failed。zsh 循环变量误用特殊变量名 `path`，覆盖命令搜索路径，产生空清单。
- 修复：改用普通变量名并重新生成；1577 项 SHA-256 逐项复核通过。
- 全参考快照 `git diff --cached --check`：Failed。旧 CubeMX/Keil 生成文件包含 CRLF，部分 Markdown 使用行末双空格；为保持参考证据不对旧源码做批量格式化。重建 `main` 的手写变更须单独通过 diff check。

### Git Status

首次提交与推送完成：

- `main` bootstrap：`f34420f913f0fa3a19a67f116f083b769815896d`。
- `backup/bootstrap-20260807`：`f34420f913f0fa3a19a67f116f083b769815896d`。
- `reference/archive-20260807`：`a0669d7e36e54e78970e8162b0bc47334221c6c5`。
- `git ls-remote --symref`：HEAD 指向 `refs/heads/main`；三条远端引用与本地一致。
- GitHub CLI：Not established（本机无 `gh`）；仓库可见性和分支保护未通过 API 核对。

### Risks / Next Step

归档提交包含厂商生成源码/库，只用于取证，不能进入 `main` 或被描述为已验证固件。Phase 1 尚缺 CubeMX/Keil 最小工程和实际构建证据，状态保持 Partially complete。

### Local Clone Failure / Fix / Retest

- 首次克隆后核验：Failed。大对象下载在工具的 30 秒返回窗口后仍在后台运行，但会话未被继续轮询，工作目录暂时只有 `.git`，`HEAD` 指向无效占位引用。
- 修复：继续抓取远端对象和引用，待 `origin/main`、bootstrap 和 reference archive 可解析后检出 `main`；终止一个重复且不再需要的 fetch 会话。
- 复测：Passed。`SmartRollator-Rebuild` 工作树干净并跟踪 `origin/main`；`git fsck --full` 退出码 0；HEAD 为 `b264fe344f998bae9f4df471dfd889743276ce81`。
- 归档复测：从 `reference/archive-20260807` 解包后，`reference_manifest.sha256` 的 1577 项全部 Passed。
- 最后一轮独立 `git ls-remote`：Failed（GitHub 443 临时连接失败）；远端最终状态仍由此前成功 push、后续成功 fetch 得到的三条 `origin/*` 引用交叉验证。网络恢复后应再次执行只读核对。
