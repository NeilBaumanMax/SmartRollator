# 当前项目交接

更新时间：2026-10-17。本页是当前摘要；历史过程保留在 DEV_PROGRESS 与 LOG。

## Current State

正式入口为当前 Git 工作树根目录，施工分支 `main`。Phase 0 完成；Phase 1 部分完成：P1.01 文档基线与 P1.02 工具链检测入口已建立，P1.03 最小工程生成、P1.05 编译尚未完成。现有完整 STM32 工程仍为参考基线，不是已验证发布版。

2026-10-16 已整理 Files、补入另一工作区的 23 份独有资料。本轮统一 Agent 导航与真实目录，DOCX 从 9 份收敛为 6 份，保留各同主题组较新版本；工程代码不变。

## 每次接续的固定动作

先按 [INDEX](../INDEX.md) 重读必读项，特别是 [WORKFLOW](WORKFLOW.md)、DEV_PROGRESS 最新计划与结论、LOG 最新记录和当前 L0；新任务、恢复会话或上下文压缩后均适用。先落盘本次计划再改代码，静态自检后修正漂移与交接，再提交推送。2026-10-17 已将这一流程固化；普通 STM32 增量允许具体静态审阅验收，编译/硬件证据仍单列。

## 必须知道的目录边界

- [目录地图](../WORKSPACE_MAP.md)说明全部一级目录、工程模块、隐藏配置和未来目录。
- STM32：根 `.ioc`、`.mxproject`、Core、Drivers、Middlewares、MDK-ARM。当前业务模块在 MDK-ARM，任务集成在 Core/Src/freertos.c。
- 根 App、Tests、CMakeLists.txt 尚未建立；没有 firmware 目录。
- [Files 全索引](../../Files/ASSET_INDEX.md)覆盖当前跟踪参考材料；其中 legacy-workspace 的工具、C/HITL 测试与 GCC Makefile 仍为待适配模板。
- Files/research/FORCE_COUPLED_HOPF_VARIABLE_ADMITTANCE.md 已按用户最新“全部备份”指令纳入 GitHub；不再有仅本地保留的研究笔记。
- 当前工作区普通文件（含旧输出、IDE 状态、预览和系统元数据）均按原路径备份；上层 4 份资料归并至已有相同文件，元数据移入 Files/local-state。清单与边界见 [完整备份记录](../verification/workspace-backup/README.md)。
- [DOCX 版本记录](../../Files/DUPLICATES.md)列出保留项、移除项、依据与恢复点。旧生成器可能重新输出旧版或覆盖人工修订，不能默认执行。

新增资料：2026-10-17 收尾出现 Files/sdk.docx（SDK/VRPN 使用说明），按全部备份要求原样纳入；当前 DOCX 7 份，新增项不是原 Hopf 报告的替代版本。

## Test Baseline

- 2026-10-16 重新执行 `sh tools/check_toolchain.sh`：退出 1，四项 NOT_FOUND，TOOLCHAIN_STATUS=INCOMPLETE；这是缺失环境契约的预期结果。检测范围为 PATH 与脚本标准候选位置，不代表扫描所有磁盘或远程 Windows 主机。
- 最近资料检查：导入 23 份源/目标 SHA-256 相同；Python/shell 语法、链接与文件结构检查通过。
- 本轮只调整施工规则与交接，检查结果见 LOG 的 2026-10-17 条目；不把历史 DOCX/工具链检查当作本轮重新执行结果。
- 固件生成/编译：Not established；正式主机测试：Not established；目标板/台架：Not established。

## Git State

- Remote：`git@github.com:NeilBaumanMax/SmartRollator.git`。
- 当前施工分支：`main`，按用户明确指令承接当前完整状态并继续开发；原 `build/phase-1-bootstrap` 保留。
- 全量备份开工基线：`6a5aaf7087cbc1b2267ba4b048193a0482a4f23c`；远端 `backup/before-complete-workspace-20261016` 保留该状态。
- 本次提升的已交付基线：`ac0c0d30abd368e4124d01d10e91176c724dd8d6`（原阶段分支）；另附本轮主分支状态文档提交。
- 提升前 main：`5403028d255ac16d2e137454fc40513e75eb01f2`，已保存到远端 `backup/main-before-promotion-20261016`。
- 提升方式：快进合并，无强制推送、不删除历史。最新主分支哈希通过 `git log -1` 和 `git ls-remote origin refs/heads/main` 实时核对。
- 近期交付：`4f5af0a` 工具链入口；`1d79da5` Files 整理；`1a9f299` 独有参考资产补入；`ac0c0d3` 文档漂移修正与 DOCX 版本收敛。
- 本轮前备份：`backup/before-docs-dedup-20261016` → `1a9f299e6498579e5dbb4e9f03972e1ace270814`，远端已核对；包含删除前 9 份 DOCX。
- 历史 bootstrap/reference 分支继续保留；根 reference_manifest.sha256 对应早期归档，legacy-workspace/IMPORT_MANIFEST.tsv 对应导入时点，均不作为当前全目录清单。
- 本页随当前整理提交更新；最新提交用 `git log -1` 核对，不能将本页开工基线当成最新 HEAD。

## 本轮交付与接续核验

- 上一次已核实交付 / 本轮开工：main 与 origin/main 均为 `df3da6870a9eebaaf215ac8cfb832cc98d681567`，fetch 后无分叉。
- 本轮远端备份：`backup/before-session-workflow-20261017`，ls-remote 已核对为上述哈希。
- 开工已有 `.DS_Store` 和 `Files/.DS_Store` 变化，按完整备份授权保留并随本轮清单提交；未覆盖原始字节。
- 本轮收尾动作：规则、检查记录和最新清单精确提交后普通推送 main；最终是否已交付须查询 `git log -1`、`git status --short --branch` 和 `git ls-remote origin refs/heads/main`，匹配才确认。文档不预填自身尚未生成的提交哈希或未执行的推送结果。

## Next Tasks

1. 按上述必读项恢复状态，核对本轮规则提交是否已推送及工作树是否干净；在 DEV_PROGRESS 写下一增量计划，核对 tracked/untracked/ignored。
2. 提供或确认 CubeMX 与匹配 FW_H7、MDK 与 Arm Compiler 的位置和版本；需要安装时按用户授权处理。
3. 重跑工具链探测并保存结果；P1.03 的前置条件为 CubeMX/FW_H7 版本确认，P1.05 另需 MDK/编译器。完整探测 COMPLETE 表示四项均确认，不改变分阶段门槛。
4. 按施工树生成最小安全失能工程与清单，之后实际全量编译；不要先推进控制算法或电机阶段。

## Risks / Completion Label

需防止把参考固件、旧报告中的硬件/完成度声明、未接通模板或生成成功误当验证通过。轮系、传感器、节点、时序与阈值仍需确认。整体与 Phase 1：Partially complete；Phase 0 与 P1.02 检测入口子范围：Complete。资料整理不改变上述验收状态。
