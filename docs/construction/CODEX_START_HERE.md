# Codex 开工入口

## 你正在做什么

本项目在当前正式 Git 仓库内，以已归档的旧工程为参考，按阶段重新建立可构建、可测试、可回滚的 SmartRollator 固件。不得把当前完整工程基线误认为已验证发布版。施工目标和真实状态以本目录文档为准，聊天记录不构成事实源。

## 开工阅读顺序

唯一顺序见 [docs/INDEX.md](../INDEX.md)。先读根 AGENTS、[目录地图](../WORKSPACE_MAP.md)和 [Files 全索引](../../Files/ASSET_INDEX.md)，再读需求、架构、施工树、当前进度与交接和当前层级。不要把旧工作区的 AGENT_CONTEXT 当作施工入口。

## 第一次接手时必须检查

```bash
pwd
git ls-files
git ls-files --others --exclude-standard
rg --files --hidden -g '!.git/**' -g '!.DS_Store'
git rev-parse --show-toplevel
git status --short --branch
git remote -v
git log --oneline --decorate -8
```

正式施工目录固定为当前 Git 工作树的仓库根目录。如果该目录不是 Git 仓库或远端不匹配，立即停止施工并按 `GITHUB_ROLLBACK.md` 恢复；不得在旁路目录静默建立替代事实源。

## 单阶段施工闭环

1. 在 `DEV_PROGRESS.md` 追加阶段开工记录。
2. 确认远端备份分支和回滚提交确实存在。
3. 只实施本阶段 Included 范围。
4. 每个小增量立刻运行最邻近检查。
5. 在 `LOG.md` 记录首次失败、修复和复测，不删除失败证据。
6. 检查文档漂移并更新层级文档与 HANDOFF。
7. 精确暂存、提交、推送；验证远端后才报告阶段完成。

## 状态词

- `Not established / 未建立`：测试、工具链或基线不存在。
- `Not run / 未运行`：具备条件但本轮没执行。
- `Failed / 失败`：执行过且未达到条件。
- `Passed / 通过`：有本轮命令、输出或人工证据。
- `Partially complete / 部分完成`：部分交付已验证，但阶段验收未全部满足。
- `Blocked / 阻塞`：缺少权限、硬件、信息或外部条件，已不能安全推进。

## 当前起点

- 当前目录是正式本地 Git 仓库；实际施工在 `build/phase-1-bootstrap`。`main` 保存早期完整基线，尚未合入本阶段工具链与 Files 整理；不要为了接手而自动切回 main。
- `reference/archive-20260807` 保存纳入 Git 前的只读参考快照；不能作为当前构建通过证据。
- Git/远端/回滚基线已建立；最小可复现 CubeMX/Keil 构建仍未建立。
- 下一步是完成 Phase 1 的工具链冻结、最小安全失能工程和真实全量编译。
