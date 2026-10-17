# Codex 开工入口

## 你正在做什么

本项目在当前正式 Git 仓库内，以已归档的旧工程为参考，按阶段重新建立可构建、可测试、可回滚的 SmartRollator 固件。不得把当前完整工程基线误认为已验证发布版。施工目标和真实状态以本目录文档为准，聊天记录不构成事实源。

## 开工阅读顺序

唯一顺序及每次恢复必读子集见 [docs/INDEX.md](../INDEX.md)。新任务、会话恢复和上下文压缩后都重新读取当前计划、进度、日志与交接；不要只复述聊天摘要。先读根 AGENTS、[目录地图](../WORKSPACE_MAP.md)和 [Files 全索引](../../Files/ASSET_INDEX.md)，再读需求、架构、施工树、当前进度与交接和当前层级。不要把旧工作区的 AGENT_CONTEXT 当作施工入口。

## 每次接续时必须检查

```bash
pwd
git ls-files
git ls-files --others --exclude-standard
rg --files --hidden -g '!.git/**'
git rev-parse --show-toplevel
git status --short --branch
git remote -v
git log --oneline --decorate -8
```

正式施工目录固定为当前 Git 工作树的仓库根目录。如果该目录不是 Git 仓库或远端不匹配，立即停止施工并按 `GITHUB_ROLLBACK.md` 恢复；不得在旁路目录静默建立替代事实源。

## 每个施工增量闭环

1. 先按 [WORKFLOW](WORKFLOW.md) 核实磁盘状态，再在 `DEV_PROGRESS.md` 追加本次开工记录；代码修改必须在计划落盘之后。
2. 确认远端备份分支和回滚提交确实存在。
3. 只实施本阶段 Included 范围。
4. 每个小增量立刻执行最邻近检查；普通 STM32 代码按 [TEST_METRICS](TEST_METRICS.md) 做具体静态自检，不把自检当成编译/实机证据。
5. 在 `LOG.md` 记录首次失败、修复和复测，不删除失败证据。
6. 检查文档漂移并更新层级文档与 HANDOFF。
7. 精确暂存、提交、推送；验证远端后才报告本次增量完成。若先有代码提交，再补文档提交，两者都推送；阶段完成另看阶段验收条件。
8. 中断时写清已做、未做、工作树改动、下一条动作和阻塞，恢复时从这些磁盘记录继续。

## 状态词

- `Not established / 未建立`：测试、工具链或基线不存在。
- `Not run / 未运行`：具备条件但本轮没执行。
- `Failed / 失败`：执行过且未达到条件。
- `Passed / 通过`：有本轮命令、输出或人工证据。
- `Partially complete / 部分完成`：部分交付已验证，但阶段验收未全部满足。
- `Blocked / 阻塞`：缺少权限、硬件、信息或外部条件，已不能安全推进。

## 当前起点

- 当前目录是正式本地 Git 仓库；按用户 2026-10-16 指令，当前完整状态已作为 main 开发基线。接手以 main 为入口，原 build/phase-1-bootstrap 保留，不自动回到旧阶段分支。
- `reference/archive-20260807` 保存纳入 Git 前的只读参考快照；不能作为当前构建通过证据。
- Git/远端/回滚基线已建立；最小可复现 CubeMX/Keil 构建仍未建立。
- 下一步是完成 Phase 1 的工具链冻结、最小安全失能工程和真实全量编译。
