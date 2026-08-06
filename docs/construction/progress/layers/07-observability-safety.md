# 07 — 可观测性与安全

- Current Baseline：旧工程有串口日志；统一故障模型、安全仲裁和指标基线未建立。
- Scope：状态机、唯一命令出口、限幅/斜率、超时、急停、任务健康、结构化日志及 ArUco 输入有效期。
- Boundary：日志只读系统状态；视觉数据不能绕过仲裁。
- Acceptance：上电失能、输入/CAN/任务故障、急停、恢复条件、日志拥塞和坏视觉帧测试。
- Evidence：Not established。
- Status：Pending。
