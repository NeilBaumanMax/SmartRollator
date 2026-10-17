# 06 — Hopf CPG

- Current Baseline：正式仓库中有 CPG 参考模块，但没有可继承的当前编译、主机测试或台架证据。
- Scope：纯 C Hopf 振子、参数/状态/步进/复位、离线指标、导纳调制和可切换集成。
- Boundary：先独立验证振子，再接导纳；永远不直接写电机。
- Acceptance：幅频、收敛、步长、有限值、参数边界、A/B 回放、模式切换和受限台架。
- Evidence：Not established。
- Status：Pending。

## 2026-10-17 研究方向接手

[当前研究基线](../../../../Files/research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)并列保留动捕直接与人体同步 Hopf，以同一 D0−A·w(ψ) 局部减阻映射对照。每步/跨步语义、相位质量/时延、停止/失联与旧 εFx 耦合隔离均待设计验证。旧 CPG_Step 仍为力耦合及 D_base−lambda·x；没有人体锁相/窗口实现证据。旧力主导/差动路线保留为备选；P9.02 已注明迁移前需重新冻结映射契约。Status 保持 Pending，未运行本层测试。
