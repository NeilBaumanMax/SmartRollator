# 01 — 平台与外设

- Current Baseline：正式仓库含 STM32H723、FreeRTOS、USART1/2、FDCAN1 参考配置；当前提交尚无重新生成和构建证据。
- Scope：时钟、GPIO、DMA、UART、RS485 方向、FDCAN、定时器、RTOS 初始化和板级自检。
- Boundary：只提供平台适配，不解析业务协议、不计算控制输出。
- Acceptance：实际编译；串口启动日志；UART/CAN 回环或电气检查；初始化失败可见且电机失能。
- Evidence：Not established。
- Status：Pending。
