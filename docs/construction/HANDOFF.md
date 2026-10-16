# 当前项目交接

更新时间：2026-10-16。本页是当前摘要；历史过程保留在 DEV_PROGRESS 与 LOG。

## Current State

正式入口为当前 Git 工作树根目录，施工分支 `build/phase-1-bootstrap`。Phase 0 完成；Phase 1 部分完成：P1.01 文档基线与 P1.02 工具链检测入口已建立，P1.03 最小工程生成、P1.05 编译尚未完成。现有完整 STM32 工程仍为参考基线，不是已验证发布版。

2026-10-16 已整理 Files、补入另一工作区的 23 份独有资料。本轮统一 Agent 导航与真实目录，DOCX 从 9 份收敛为 6 份，保留各同主题组较新版本；工程代码不变。

## 必须知道的目录边界

- [目录地图](../WORKSPACE_MAP.md)说明全部一级目录、工程模块、隐藏配置和未来目录。
- STM32：根 `.ioc`、`.mxproject`、Core、Drivers、Middlewares、MDK-ARM。当前业务模块在 MDK-ARM，任务集成在 Core/Src/freertos.c。
- 根 App、Tests、CMakeLists.txt 尚未建立；没有 firmware 目录。
- [Files 全索引](../../Files/ASSET_INDEX.md)覆盖当前跟踪参考材料；其中 legacy-workspace 的工具、C/HITL 测试与 GCC Makefile 仍为待适配模板。
- Files/research/FORCE_COUPLED_HOPF_VARIABLE_ADMITTANCE.md 当前只存在于本地未跟踪状态；保留，不混入本轮提交。
- [DOCX 版本记录](../../Files/DUPLICATES.md)列出保留项、移除项、依据与恢复点。旧生成器可能重新输出旧版或覆盖人工修订，不能默认执行。

## Test Baseline

- 2026-10-16 重新执行 `sh tools/check_toolchain.sh`：退出 1，四项 NOT_FOUND，TOOLCHAIN_STATUS=INCOMPLETE；这是缺失环境契约的预期结果。检测范围为 PATH 与脚本标准候选位置，不代表扫描所有磁盘或远程 Windows 主机。
- 最近资料检查：导入 23 份源/目标 SHA-256 相同；Python/shell 语法、链接与文件结构检查通过。
- 本轮工程保全、DOCX 核对、导航覆盖等实际结果见 LOG 最后条目；不把历史检查数量当作本轮结果。
- 固件生成/编译：Not established；正式主机测试：Not established；目标板/台架：Not established。

## Git State

- Remote：`git@github.com:NeilBaumanMax/SmartRollator.git`。
- `main`：本轮核对远端为 `5403028d255ac16d2e137454fc40513e75eb01f2`，不是当前施工分支。
- 当前施工分支：`build/phase-1-bootstrap`；本轮开工基线 `1a9f299e6498579e5dbb4e9f03972e1ace270814`，此前已推送。
- 近期交付：`4f5af0a` 工具链入口；`1d79da5` Files 整理；`1a9f299` 独有参考资产补入。
- 本轮前备份：`backup/before-docs-dedup-20261016` → `1a9f299e6498579e5dbb4e9f03972e1ace270814`，远端已核对；包含删除前 9 份 DOCX。
- 历史 bootstrap/reference 分支继续保留；根 reference_manifest.sha256 对应早期归档，legacy-workspace/IMPORT_MANIFEST.tsv 对应导入时点，均不作为当前全目录清单。
- 本页随当前整理提交更新；最新提交用 `git log -1` 核对，不能将本页开工基线当成最新 HEAD。

## Next Tasks

1. 按 docs/INDEX.md 的唯一顺序读取；盘点 tracked、untracked，必要时分类 ignored 文件。
2. 提供或确认 CubeMX 与匹配 FW_H7、MDK 与 Arm Compiler 的位置和版本；需要安装时按用户授权处理。
3. 重跑工具链探测并保存结果；P1.03 的前置条件为 CubeMX/FW_H7 版本确认，P1.05 另需 MDK/编译器。完整探测 COMPLETE 表示四项均确认，不改变分阶段门槛。
4. 按施工树生成最小安全失能工程与清单，之后实际全量编译；不要先推进控制算法或电机阶段。

## Risks / Completion Label

需防止把参考固件、旧报告中的硬件/完成度声明、未接通模板或生成成功误当验证通过。轮系、传感器、节点、时序与阈值仍需确认。整体与 Phase 1：Partially complete；Phase 0 与 P1.02 检测入口子范围：Complete。资料整理不改变上述验收状态。
