# 03 — 电机与 CANopen

- Current Baseline：旧工程有 FDCAN1 与 ZLAC8015D 参考代码；节点、单位、方向和故障路径未在新工程验证。
- Scope：CAN 编解码、节点状态、使能/失能、速度命令、反馈、超时和故障。
- Boundary：仅接受安全仲裁产生的 `WheelCommand`；不得接受控制模块直写。
- Acceptance：主机编解码测试、CAN 抓包、抬轮方向/低速/失联/重启测试；故障时确定失能。
- Evidence：Not established。
- Status：Pending。
