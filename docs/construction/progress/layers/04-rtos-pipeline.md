# 04 — RTOS 数据管线

- Current Baseline：旧工程使用 FreeRTOS/CMSIS-RTOS2；任务划分和时序只作参考。
- Scope：传感、控制、执行器、日志任务，队列/快照，心跳、周期统计、看门狗和背压。
- Boundary：调度数据，不承载协议字节解析或控制数学。
- Acceptance：周期/抖动/栈/CPU 数据；队列满、日志拥塞、输入超时和任务失活测试。
- Evidence：Not established。
- Status：Pending。
