# SmartRollator Agent Rules

## 项目身份

SmartRollator 是基于 STM32H723VGT6 的智能助行车实时控制固件。当前工作的目标是从干净仓库重新施工，并以可构建、可测试、可回滚的方式达到现有参考固件的能力。

## 指令优先级

1. 用户最新明确指令
2. 本文件
3. `docs/construction/CODEX_MASTER_REQUIREMENTS.md`
4. `docs/product/PRODUCT_REQUIREMENTS.md`
5. 其他施工文档
6. 现有参考代码

## 开工前必读

按 `docs/INDEX.md` 的顺序阅读；至少读取 `CODEX_START_HERE.md`、`DEV_PROGRESS.md`、`HANDOFF.md` 和当前阶段文档。

## 每阶段开始

- 检查当前目录、Git 分支、工作树、最近提交和远端状态。
- 在 `DEV_PROGRESS.md` 追加施工计划，明确范围、测试、回滚点和排除项。
- 未建立远端备份分支前，不开始有意义的代码重建。

## 每阶段结束

- 执行本阶段规定的最邻近测试，原样记录首次失败、修复和复测结果。
- 更新 `LOG.md`、`DEV_PROGRESS.md`、`HANDOFF.md` 及受影响的层级文档。
- 只精确暂存本阶段文件；提交并推送后再报告完成。

## 当前阶段

Phase 0：施工文档基线。下一阶段是 Phase 1：创建全新 Git 仓库与最小可构建工程。

## 禁止事项

- 禁止把“生成成功”等同于“编译、烧录或实机通过”。
- 禁止虚构测试结果、硬件状态、提交或推送状态。
- 禁止使用 `git reset --hard`、`git clean -fd`、强制推送，或覆盖不明来源改动。
- 禁止把密钥、个人路径、实验隐私数据和大型生成物提交进仓库。
- 禁止在 CubeMX 自动生成区堆放业务逻辑；只在 USER CODE 区或独立模块中实现。
