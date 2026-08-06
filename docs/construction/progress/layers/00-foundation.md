# 00 — 仓库与工程基础

- Current Baseline：施工文档、GitHub 远端、干净 `main`、bootstrap 备份、旧参考归档和 P1.02 检测入口已建立；当前主机工具链不完整，CubeMX/Keil 最小工程基线未建立。
- Scope：Phase 1 与 Phase 3 的仓库骨架、可复现构建、公共类型和主机测试入口。
- Key Deliverables：`.gitignore`、README/docs、`.ioc`、Keil 工程、工具版本、构建命令、测试入口、远端备份。
- Acceptance：干净检出可构建；目标板上电保持电机失能；远端回滚分支可访问。
- Evidence：`main` bootstrap `f34420f9`；`backup/bootstrap-20260807` 同哈希；`reference/archive-20260807` 为 `a0669d7e`；远端引用已核对；P1.02 脚本语法/缺失契约/路径脱敏 Passed，当前主机四项必需工具均为 Not found。
- Next Action：提供或授权建立 CubeMX + 匹配 FW_H7、MDK + Arm Compiler 环境，重跑检测至 Complete 后进入 P1.03。
- Status：Partially complete。
