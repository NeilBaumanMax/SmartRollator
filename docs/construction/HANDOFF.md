# 当前项目交接

更新时间：2026-08-07

## Current State

SmartRollator 已在正式当前仓库完成 docs-first 施工体系、完整工程基线、远端和回滚点。Phase 0 完成；Phase 1 部分完成：Git 子范围完成，CubeMX/Keil 最小可复现工程尚未重新生成或编译。当前 `main` 中的参考固件可供取证，但其编译、烧录和硬件状态不能视为已验证事实。

## Completed

- 产品范围、V1 非目标和成功标准已冻结。
- L0–L5 分层、纯 C 控制域和唯一安全命令出口已确定。
- Phase 0–11 路线、测试等级、Git 回滚和工具规范已建立。
- 硬件/参数未知项已集中进入 `ASSUMPTIONS.md`。
- 当前进度、日志、层级状态和模板已建立。
- GitHub 远端、默认 `main`、bootstrap 备份和只读参考归档已建立并核对哈希。
- Git 文件级施工树已建立：Phase 1–11 共 66 个微提交节点，逐项定义精确文件、功能/接口、测试和回滚点。

## Incomplete

- CubeMX/Keil 最小安全失能工程尚未在正式仓库重新生成和编译。
- 所有固件迁移、主机测试、目标板和台架验证尚未执行。
- 轮系、传感器、CAN 节点、时序、安全阈值和 ArUco 角色仍有未确认项。

## Next Tasks

1. 阅读 `AGENTS.md` 和 `docs/INDEX.md` 规定的必读材料。
2. 确认当前目录是正式 Git 工作树的仓库根目录，实时核对分支、远端和工作树。
3. 按 `GIT_CONSTRUCTION_TREE.md` 从最新 `main` 创建并先行推送 `build/phase-1-bootstrap`。
4. 从 P1.02 开始，确认本机 CubeMX、MDK、Arm Compiler 和 STM32Cube FW_H7 的实际版本并记录。
5. 按 P1.03 生成最小安全失能工程并实际全量编译；保存生成清单、哈希和完整构建证据。

## Required Reading

`CODEX_START_HERE.md` → `CODEX_MASTER_REQUIREMENTS.md` → 产品需求 → 架构/分层契约 → 施工计划 → `GIT_CONSTRUCTION_TREE.md` → 开发进度 → 本交接 → `progress/layers/00-foundation.md`。

## Important Files

- 项目入口：`AGENTS.md`、`docs/INDEX.md`
- 产品事实源：`docs/product/PRODUCT_REQUIREMENTS.md`
- 施工事实源：本目录全部文档
- 文件级提交事实源：`docs/construction/GIT_CONSTRUCTION_TREE.md`
- 旧工程详细参考：`docs/handover/PROJECT_HANDOVER.md`

## Test Baseline

- 文档检查：Passed（34 个 Markdown 相对链接；施工树 11 个阶段、66 个微提交节点；当前模型漂移扫描 0 命中）。
- 固件构建：Not established。
- 主机测试：Not established。
- 目标板/台架测试：Not established。

## Git State

- Repository：`https://github.com/NeilBaumanMax/SmartRollator.git`；默认分支 `main`。
- Bootstrap Commit：`f34420f913f0fa3a19a67f116f083b769815896d`。
- Full Workspace Baseline：`2cd83025946a8ab55553f834773f7f7bc52bd955`，已从正式当前目录推送到 `main`。
- Backup Branch：`backup/bootstrap-20260807`，远端哈希与 bootstrap commit 一致。
- Reference Archive：`reference/archive-20260807` → `a0669d7e36e54e78970e8162b0bc47334221c6c5`。
- Push Status：Passed；完整工程已进入远端 `main`，bootstrap 和 reference archive 仍保留。
- Working Tree：正式本地仓库为当前 Git 工作树的仓库根目录；已初始化并绑定 `origin/main`。任何旁路克隆都不再作为施工入口或事实源。

## Risks

- 误把旧工程行为或旧生成文件当作新工程验证结果。
- 未确认轮系/传感器参数就接通电机。
- CubeMX 再生成覆盖手写逻辑。
- AI Agent 同时跨多个阶段施工，造成无法定位和回滚。

## Completion Label

整体项目：Partially complete。Phase 0：Complete。Phase 1：Partially complete。下一动作：完成 Phase 1 最小工程生成和构建。
