# 当前工作区目录地图

核对日期：2026-10-16。当前目录为 `git rev-parse --show-toplevel` 返回的正式仓库根目录。当前施工分支是 `build/phase-1-bootstrap`；切换分支后必须重新盘点，main 不一定含阶段分支的最新资料。

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

`MDK-ARM/` 下现有编译输出和 IDE 用户状态可能被 Git 忽略。它们不作为当前通过证据，也不因整理参考资料而删除。

## 文档和参考材料

| 路径 | 定位 / 入口 |
|---|---|
| `AGENTS.md`、`README.md` | Agent 规则与项目总入口 |
| `docs/construction/` | 需求约束、设计、施工树、进度、日志、交接、分层与模板；[总索引](INDEX.md) |
| `docs/product/` | 产品范围与非目标 |
| `docs/verification/` | 工具链证据及构建记录规范；记录日期与测试边界 |
| `docs/handover/` | 旧交接 Markdown，仅历史参考 |
| `Files/` | 保留的 DOCX、两份 HTML 演示、[全量参考资产索引](../Files/ASSET_INDEX.md) |
| `Files/generated_docs/`、`Files/diagrams/` | 专题报告及配图；预览缓存被忽略 |
| `Files/tools/` | 文档生成器与字体配置；不属于固件工具链 |
| `Files/hardware/` | 设备手册；实物型号/协议适用性待核实 |
| `Files/handover/` | 旧交接 Word，未同步当前施工状态 |
| `Files/legacy-workspace/` | 补入的旧架构/接口/图示、串口/调试工具、C/HITL 测试及 GCC 模板；[适配缺口](../Files/legacy-workspace/README.md) |
| `Files/research/` | 本地未跟踪研究笔记；当前有 FORCE_COUPLED_HOPF_VARIABLE_ADMITTANCE.md，干净克隆不保证存在 |
| `reference_manifest.sha256` | 只读历史参考归档清单，原路径仅对归档版本有效 |

## 计划目录与现状的区别

根目录 `App/`、`Tests/`、`CMakeLists.txt` 尚未建立；未来逐阶段按 [Git 施工树](construction/GIT_CONSTRUCTION_TREE.md) 创建。`Files/legacy-workspace/tests/` 是历史模板，`Makefile.gcc.template` 是草案，不是正式测试或构建入口。没有 `firmware/` 目录。

## Agent 如何覆盖资料

先读 AGENTS 和 INDEX，再按本地图定位工程；读取 Files 全索引后按任务选择原文件。用 `git ls-files` 核对已跟踪范围，用 `git ls-files --others --exclude-standard` 核对本地额外资料。Word/PDF 需用能读取其内容的工具，文件名只能导航，不能替代正文证据。厂商库不用每次全读，但必须保留且能定位依赖。

资产删改应同步 [DOCX 版本核对记录](../Files/DUPLICATES.md)。历史导入清单和日志保留原貌，明确指向当时状态。
