# 00 — 仓库与工程基础

- Current Baseline：施工文档、GitHub 远端、当前完整 `main` 开发基线与保留的 `build/phase-1-bootstrap`、bootstrap 备份、旧参考归档和 P1.02 检测入口已建立；当前主机工具链不完整，CubeMX/Keil 最小工程基线未建立。
- Scope：Phase 1 与 Phase 3 的仓库骨架、可复现构建、公共类型和主机测试入口。
- Key Deliverables：`.gitignore`、README/docs、`.ioc`、Keil 工程、工具版本、构建命令、测试入口、远端备份。
- Acceptance：干净检出可构建；目标板上电保持电机失能；远端回滚分支可访问。
- Evidence：`main` bootstrap `f34420f9`；`backup/bootstrap-20260807` 同哈希；`reference/archive-20260807` 为 `a0669d7e`；远端引用已核对；P1.02 脚本语法/缺失契约/路径脱敏 Passed，2026-10-16 重新探测四项必需工具均为 NOT_FOUND（限脚本候选范围），退出 1 / INCOMPLETE。
- Next Action：确认 CubeMX + 匹配 FW_H7 的版本后进入 P1.03；P1.05 另需 MDK + Arm Compiler。重跑只读检测记录真实状态，四项均确认才输出 Complete。
- Status：Partially complete。

## 2026-10-16 工作区整理

参考 DOCX、HTML、生成脚本和图表归入 `Files/`；施工 `docs/` 与工具链脚本保留。1669 个受保护文件哈希与移动前一致，Keil 49 个文件引用及 17 个包含目录条目存在。本轮只确认文件与路径完整，未执行固件构建或硬件测试，Phase 1 状态仍为 Partially complete。

## 2026-10-16 独有参考资产补全

23 份外部工作区资产补入 Files，来源未修改、复制哈希一致。GCC Makefile、串口/调试工具及测试保留为历史模板，正式 `tools/check_toolchain.sh` 与 STM32 工程不变；未建立新的构建或主机测试基线。

## 2026-10-16 文档漂移与版本收敛

AGENTS、INDEX、目录地图与 Files 全索引已建立一致入口，明确当前分支和未来 App/Tests 的区别。本轮 1669 个受保护文件哈希不变，49 个 Keil 文件引用及 17 个包含目录条目存在；DOCX 由 9 份收敛为 6 份，保留文件字节不变。工具链复测返回 1 / INCOMPLETE；Phase 1 仍为 Partially complete。

## 2026-10-16 全工作区备份

按用户最新要求纳入研究笔记、旧编译输出、IDE 状态、预览和系统元数据，普通文件以哈希清单、磁盘/Git 集合及远端对象核验覆盖；.git 内部数据库不重复提交。原工程和参考资产字节保留，备份完成不改变 Phase 1 构建/硬件验收状态。

## 2026-10-17 每次接续施工闭环

AGENTS 与 INDEX 强制每次新任务/接续/会话恢复/上下文压缩后重读当前 docs；WORKFLOW 统一先计划、实施、自检、修正漂移和交接、提交推送。普通 STM32 代码增量可用具体静态审阅验收，实际编译/硬件节点仍须真实证据。当前固件与工具未改，Phase 1 保持 Partially complete；验证记录见 LOG 同日条目。
