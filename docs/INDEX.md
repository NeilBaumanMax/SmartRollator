# SmartRollator 文档总入口

本文档是项目文档的唯一导航入口。README 面向项目介绍；`docs/` 面向施工、验证、交接和追溯。

## 强制阅读顺序

每次新任务、接续施工、恢复会话或上下文压缩后，先重新读取第 1、3、6、7、8 项：第 6 项至少读当前阶段和当前节点，第 7 项读最新计划及其后续结论/日志。首次接手或范围变化时按完整顺序读全部项，其他资料按变更范围补读。聊天摘要、记忆和旧交接不能代替当前磁盘记录。

1. 根 [AGENTS.md](../AGENTS.md) 与[开工入口](construction/CODEX_START_HERE.md)、[施工工作流](construction/WORKFLOW.md)
2. [当前目录地图](WORKSPACE_MAP.md)与 [Files 资产全索引](../Files/ASSET_INDEX.md)，覆盖工程、隐藏配置和全部参考资料；另用 Git 清单发现新增未跟踪/忽略项
3. [总需求与硬约束](construction/CODEX_MASTER_REQUIREMENTS.md)
4. [产品需求](product/PRODUCT_REQUIREMENTS.md)
5. [系统架构](construction/ARCHITECTURE.md)与[分层契约](construction/LAYER_CONTRACT.md)
6. [分阶段施工计划](construction/CONSTRUCTION_PLAN.md)与 [Git 文件级施工树](construction/GIT_CONSTRUCTION_TREE.md)
7. [当前进度](construction/DEV_PROGRESS.md)与[当前交接](construction/HANDOFF.md)、[施工日志最新条目](construction/LOG.md)
8. [分层进度入口](construction/progress/layers/README.md)，当前阶段读 [00 仓库与工程基础](construction/progress/layers/00-foundation.md)

目录地图描述现在存在的内容；施工树还包含未来计划。历史资料的“已完成”和路径声明不构成当前状态。

## 管理与验证

- [全工作区备份记录](verification/workspace-backup/README.md)：上层资料归并、全部普通文件清单和 GitHub 核验方法。

- [施工工作流](construction/WORKFLOW.md)：每次接续和每个施工增量如何读文档、计划、实施、自检、修正漂移、提交与推送。
- [Git 与回滚](construction/GITHUB_ROLLBACK.md)：仓库、分支、提交、远端备份和恢复规则。
- [Git 文件级施工树](construction/GIT_CONSTRUCTION_TREE.md)：逐微提交列出精确文件、功能、接口、测试和回滚点。
- [测试与指标](construction/TEST_METRICS.md)：测试层级、证据标准和完成定义。
- [分层契约](construction/LAYER_CONTRACT.md)：模块边界、依赖方向和接口责任。
- [工具政策](construction/TOOL_POLICY.md)：CubeMX、Keil、脚本、串口与硬件工具的使用边界。
- [决策记录](construction/DECISIONS.md)：已经冻结的架构与产品决策。
- [假设清单](construction/ASSUMPTIONS.md)：尚待实测或确认的事项。
- [施工日志](construction/LOG.md)：按时间追加的真实工作、失败、修复和 Git 状态。

## 模板

- [阶段开工模板](construction/templates/PHASE_START_TEMPLATE.md)
- [工作日志模板](construction/templates/WORK_LOG_TEMPLATE.md)
- [测试报告模板](construction/templates/TEST_REPORT_TEMPLATE.md)

## 当前研究讨论

- [个性化基准阻尼与步态相位减阻研究基线](../Files/research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)：2026-10-17 接手确认的方向、候选公式、代码差距和验证次序；不是实现/硬件通过证据。
- [网页讨论原始资料](../Files/start_researching/README.md)：原始接手提示词、导师稿/配图/ZIP 与两份历史 HTML；按原字节保存。

## 历史与参考材料

- [补入的历史开发资料与工具模板](../Files/legacy-workspace/README.md)：只作参考，未接入正式构建或测试。
- [DOCX 版本核对与清理记录](../Files/DUPLICATES.md)：重复主题、保留版本、旧版哈希和恢复点。
- [Files 参考材料目录](../Files/README.md)：Word、HTML、配图和生成脚本；与 STM32 工程分开存放。

- [既有项目交接](handover/PROJECT_HANDOVER.md)：用于理解现有参考固件；其中独立目录方案已被 ADR-007 取代，不代表当前验证状态。
- [下一位 AI Agent 提示词](handover/NEXT_AI_AGENT_PROMPT.md)：旧版交接提示；新施工以本索引和 `AGENTS.md` 为准。

## 文档维护规则

- 产品范围变化：更新产品需求、总需求、施工计划、决策记录和交接。
- 架构或接口变化：更新架构、分层契约、对应层级文档、日志和交接。
- 每次实际施工：追加进度与日志；不得重写历史以掩盖失败。
- 每个施工增量结束或中断：更新 HANDOFF 的“当前状态”，保证新 Agent 不依赖聊天记录即可继续。
- 未验证的事实必须写为“未建立”或“待验证”，不能写为“通过”。
