# SmartRollator 文档总入口

本文档是项目文档的唯一导航入口。README 面向项目介绍；`docs/` 面向施工、验证、交接和追溯。

## 强制阅读顺序

1. 根 [AGENTS.md](../AGENTS.md) 与[开工入口](construction/CODEX_START_HERE.md)
2. [当前目录地图](WORKSPACE_MAP.md)与 [Files 资产全索引](../Files/ASSET_INDEX.md)，覆盖工程、隐藏配置、参考资料和本地未跟踪资料入口
3. [总需求与硬约束](construction/CODEX_MASTER_REQUIREMENTS.md)
4. [产品需求](product/PRODUCT_REQUIREMENTS.md)
5. [系统架构](construction/ARCHITECTURE.md)与[分层契约](construction/LAYER_CONTRACT.md)
6. [分阶段施工计划](construction/CONSTRUCTION_PLAN.md)与 [Git 文件级施工树](construction/GIT_CONSTRUCTION_TREE.md)
7. [当前进度](construction/DEV_PROGRESS.md)与[当前交接](construction/HANDOFF.md)
8. [分层进度入口](construction/progress/layers/README.md)，当前阶段读 [00 仓库与工程基础](construction/progress/layers/00-foundation.md)

目录地图描述现在存在的内容；施工树还包含未来计划。历史资料的“已完成”和路径声明不构成当前状态。

## 管理与验证

- [施工工作流](construction/WORKFLOW.md)：一次阶段如何开始、实施、验收和结束。
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
- 每个阶段结束：更新 HANDOFF 的“当前状态”，保证新 Agent 不依赖聊天记录即可继续。
- 未验证的事实必须写为“未建立”或“待验证”，不能写为“通过”。
