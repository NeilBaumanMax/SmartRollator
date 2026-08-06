# 05 — 固定导纳基线

- Current Baseline：旧工程存在导纳参考实现；参数、单位和离散化需重新核对。
- Scope：纯 C 状态、参数校验、滤波、积分/离散更新、输出饱和、复位和记录回放。
- Boundary：输出 `MotionIntent`，不能直接访问 HAL/RTOS/CAN。
- Acceptance：零输入、阶跃、正负方向、饱和、异常浮点、确定性和台架限幅测试。
- Evidence：Not established。
- Status：Pending。
