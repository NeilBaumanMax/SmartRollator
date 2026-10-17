# 05 — 固定导纳基线

- Current Baseline：旧工程存在导纳参考实现；参数、单位和离散化需重新核对。
- Scope：纯 C 状态、参数校验、滤波、积分/离散更新、输出饱和、复位和记录回放。
- Boundary：输出 `MotionIntent`，不能直接访问 HAL/RTOS/CAN。
- Acceptance：零输入、阶跃、正负方向、饱和、异常浮点、确定性和台架限幅测试。
- Evidence：Not established。
- Status：Pending。

## 2026-10-17 研究方向接手

已保存[当前研究基线](../../../../Files/research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)：初期固定 M、K=0，先验证定阻尼及 v_cmd/v_meas 的区别。PBO 为后续试次间选 D0，A=0 起步，不是本层已实现功能，也不解除 V1 非人体边界。现有 Controller 仍调用 CPG_Step，未建立独立固定导纳正式测试证据；Status 保持 Pending。
