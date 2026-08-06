# Codex 开工入口

## 你正在做什么

本项目不是在旧目录上继续堆补丁，而是用旧工程作为只读参考，从零建立一个新的、受 Git 管理的 SmartRollator 固件。施工目标和真实状态以本目录文档为准，聊天记录不构成事实源。

## 开工阅读顺序

1. 根目录 `AGENTS.md`
2. `CODEX_MASTER_REQUIREMENTS.md`
3. `../product/PRODUCT_REQUIREMENTS.md`
4. `ARCHITECTURE.md` 与 `LAYER_CONTRACT.md`
5. `CONSTRUCTION_PLAN.md`
6. `DEV_PROGRESS.md` 与 `HANDOFF.md`
7. 当前阶段对应的 `progress/layers/*.md`

## 第一次接手时必须检查

```bash
pwd
rg --files -g 'AGENTS.md' -g 'docs/**' | sort
git rev-parse --show-toplevel
git status --short --branch
git remote -v
git log --oneline --decorate -8
```

如果目录不是 Git 仓库，如实记录“Git 基线未建立”，然后按 Phase 1 执行；不得假装已有提交或远端备份。

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

- 当前目录保存旧工程及新建立的施工文档。
- 当前目录尚未确认 Git 仓库；以实时 `git` 检查结果为准。
- 当前固件有参考价值，但不能作为新仓库的测试通过证据。
- 下一步是 Phase 1：创建独立重建仓库、首个远端备份分支和最小可构建工程。
