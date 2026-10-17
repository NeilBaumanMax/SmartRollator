# 当前工作区目录地图

核对日期：2026-10-17。当前目录为 `git rev-parse --show-toplevel` 返回的正式仓库根目录。当前施工分支是 `main`，已纳入原阶段分支的工具链入口及资料整理状态；按用户指令在 main 继续开发，切换分支后仍须重新盘点。

## 已存在的工程与工具

| 路径 | 职责及读取重点 |
|---|---|
| `SmartRollator.ioc`、`.mxproject` | CubeMX 配置及工程元数据；隐藏文件也属于工程 |
| `Core/Inc/`、`Core/Src/` | 入口、外设、RTOS 配置与集成；优先看 main.c、freertos.c 和对应头文件 |
| `Drivers/` | HAL/CMSIS、附带库/示例/厂商脚本；保持整体原结构，不按扩展名清理 |
| `Middlewares/Third_Party/FreeRTOS/` | FreeRTOS/CMSIS-RTOS2 及现有移植层 |
| `MDK-ARM/SmartRollator.uvprojx` | 正式存在的 Keil 工程定义；启动文件、RTE/DebugConfig 等均留在本目录 |
| `MDK-ARM/Controller.c/.h`、`cpg_control.c/.h` | 当前控制器、Hopf/变导纳参考实现 |
| `MDK-ARM/torque_sensor.c/.h`、`user_can.c/.h`、`data_structs.h` | 传感器、CANopen、公共数据结构参考实现 |
| `tools/check_toolchain.sh` | 只读工具链检测；缺失/版本未确认返回 1 |

按用户最新全量备份指令，`MDK-ARM/` 下旧编译输出、IDE 状态及日志现已按原路径跟踪；不作为当前通过证据。完整覆盖范围见 [全工作区备份记录](verification/workspace-backup/README.md)。

## 文档和参考材料

| 路径 | 定位 / 入口 |
|---|---|
| `AGENTS.md`、`README.md` | Agent 规则与项目总入口 |
| `docs/construction/` | 需求约束、设计、施工树、进度、日志、交接、分层与模板；[总索引](INDEX.md) |
| `docs/product/` | 产品范围与非目标 |
| `docs/verification/` | 工具链证据及构建记录规范；记录日期与测试边界 |
| `docs/handover/` | 旧交接 Markdown，仅历史参考 |
| `Files/` | 保留的 DOCX、HTML 演示、[全量参考资产索引](../Files/ASSET_INDEX.md) |
| `Files/generated_docs/`、`Files/diagrams/` | 专题报告、配图及原始预览缓存；均纳入备份 |
| `Files/tools/` | 文档生成器与字体配置；不属于固件工具链 |
| `Files/hardware/` | 设备手册；实物型号/协议适用性待核实 |
| `Files/handover/` | 旧交接 Word，未同步当前施工状态 |
| `Files/legacy-workspace/` | 补入的旧架构/接口/图示、串口/调试工具、C/HITL 测试及 GCC 模板；[适配缺口](../Files/legacy-workspace/README.md) |
| `Files/research/` | [当前步态相位/PBO 基线](../Files/research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)及历史力耦合笔记；区分当前方向与备选路线 |
| `Files/start_researching/` | 网页 ChatGPT 讨论原始资料：导师稿、PNG/SVG/Mermaid、ZIP、两份历史 HTML 及本次接手提示词；[说明](../Files/start_researching/README.md) |
| `Files/local-state/` | 上层目录原始 Finder 元数据及说明 |
| `reference_manifest.sha256` | 只读历史参考归档清单，原路径仅对归档版本有效 |

## 计划目录与现状的区别

根目录 `App/`、`Tests/`、`CMakeLists.txt` 尚未建立；未来逐阶段按 [Git 施工树](construction/GIT_CONSTRUCTION_TREE.md) 创建。`Files/legacy-workspace/tests/` 是历史模板，`Makefile.gcc.template` 是草案，不是正式测试或构建入口。没有 `firmware/` 目录。

## Agent 如何覆盖资料

先读 AGENTS 和 INDEX，再按本地图定位工程；读取 Files 全索引后按任务选择原文件。用 `git ls-files` 核对已跟踪范围，用 `git ls-files --others --exclude-standard` 核对本地额外资料。Word/PDF 需用能读取其内容的工具，文件名只能导航，不能替代正文证据。厂商库不用每次全读，但必须保留且能定位依赖。

资产删改应同步 [DOCX 版本核对记录](../Files/DUPLICATES.md)。历史导入清单和日志保留原貌，明确指向当时状态。
