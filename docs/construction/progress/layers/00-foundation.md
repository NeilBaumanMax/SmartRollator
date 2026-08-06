# 00 — 仓库与工程基础

- Current Baseline：施工文档、GitHub 远端、干净 `main`、bootstrap 备份和旧参考归档已建立；CubeMX/Keil 最小工程基线未建立。
- Scope：Phase 1 与 Phase 3 的仓库骨架、可复现构建、公共类型和主机测试入口。
- Key Deliverables：`.gitignore`、README/docs、`.ioc`、Keil 工程、工具版本、构建命令、测试入口、远端备份。
- Acceptance：干净检出可构建；目标板上电保持电机失能；远端回滚分支可访问。
- Evidence：`main` bootstrap `f34420f9`；`backup/bootstrap-20260807` 同哈希；`reference/archive-20260807` 为 `a0669d7e`；远端引用已核对。
- Next Action：在独立重建工作目录创建 `build/phase-1-bootstrap`，生成并实际编译最小安全失能工程。
- Status：Partially complete。
