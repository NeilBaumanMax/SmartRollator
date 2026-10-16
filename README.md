# SmartRollator

SmartRollator 是基于 STM32H723VGT6、FreeRTOS/CMSIS-RTOS2 的智能助行车实时控制固件重建项目。

当前状态：Phase 0 文档基线完成，Phase 1 部分完成；当前施工分支为 `build/phase-1-bootstrap`，P1.02 检测入口已建立。当前目录是正式本地仓库；旧参考固件已作为可追踪基线纳入，但不能据此宣称构建、烧录或硬件验证通过。

开始工作前请按 [docs/INDEX.md](docs/INDEX.md) 的顺序阅读，并遵守 [AGENTS.md](AGENTS.md)。Git 分支、归档和回滚规则见 [Git 与回滚标准](docs/construction/GITHUB_ROLLBACK.md)。

## 仓库分支

- `main`：已验收的早期完整工程基线；当前工具链入口与资料整理在 `build/phase-1-bootstrap`，尚未合入 main。
- `backup/bootstrap-*`：已推送的重建起点。
- `reference/archive-*`：旧参考工程可用资产，只读且不与 `main` 合并。

## 当前验证状态

- 文档与资料检查：结果见最新施工日志；文件级导航见 [目录地图](docs/WORKSPACE_MAP.md)与 [Files 全索引](Files/ASSET_INDEX.md)。
- 新主线固件构建：Not established。
- 主机测试：Not established。
- 目标板/台架：Not established。

上电默认必须无驱动力。V1 仅允许受控研发和非载人台架验证。

## 工作区布局

- STM32 工程：根目录 `SmartRollator.ioc`、`.mxproject`、`Core/`、`Drivers/`、`Middlewares/` 和 `MDK-ARM/` 保持原有结构；Keil 入口为 `MDK-ARM/SmartRollator.uvprojx`。
- [Files 参考材料](Files/README.md)：对话产生的 Word 文档、HTML 展示页、配图、文档生成脚本和研究笔记。
- `docs/`：产品、施工、验证与历史交接 Markdown；`tools/check_toolchain.sh`：工程工具链检测入口。
- `reference_manifest.sha256`：只读历史归档清单，对应 `reference/archive-20260807` 的原始路径；当前参考材料新路径见 `Files/README.md`，不要把历史清单当作整理后的全工作区清单。
