# SmartRollator

SmartRollator 是基于 STM32H723VGT6、FreeRTOS/CMSIS-RTOS2 的智能助行车实时控制固件重建项目。

当前状态：Phase 0 文档基线完成，Phase 1 正在建立完整 Git 工程与最小构建基线。当前目录是正式本地仓库；旧参考固件已作为可追踪基线纳入，但不能据此宣称构建、烧录或硬件验证通过。

开始工作前请按 [docs/INDEX.md](docs/INDEX.md) 的顺序阅读，并遵守 [AGENTS.md](AGENTS.md)。Git 分支、归档和回滚规则见 [Git 与回滚标准](docs/construction/GITHUB_ROLLBACK.md)。

## 仓库分支

- `main`：包含完整工程基线的唯一施工事实源。
- `backup/bootstrap-*`：已推送的重建起点。
- `reference/archive-*`：旧参考工程可用资产，只读且不与 `main` 合并。

## 当前验证状态

- 文档基线：Passed（历史记录见施工日志；本轮将复核）。
- 新主线固件构建：Not established。
- 主机测试：Not established。
- 目标板/台架：Not established。

上电默认必须无驱动力。V1 仅允许受控研发和非载人台架验证。
