# 分层施工进度

这里保存每个能力层的“当前基线、施工边界、验收和下一步”。代码状态变化时更新对应文件，同时在 `LOG.md` 留下时间线记录。

| 文件 | 能力 | 当前状态 |
|---|---|---|
| [00-foundation.md](00-foundation.md) | Git、工程、构建 | Pending |
| [01-platform-peripherals.md](01-platform-peripherals.md) | STM32 平台与外设 | Pending |
| [02-force-sensing.md](02-force-sensing.md) | RS485 力传感 | Pending |
| [03-motor-canopen.md](03-motor-canopen.md) | ZLAC/CANopen | Pending |
| [04-rtos-pipeline.md](04-rtos-pipeline.md) | RTOS 数据管线 | Pending |
| [05-admittance-baseline.md](05-admittance-baseline.md) | 固定导纳 | Pending |
| [06-hopf-cpg.md](06-hopf-cpg.md) | Hopf CPG 与集成 | Pending |
| [07-observability-safety.md](07-observability-safety.md) | 可观测性与安全 | Pending |
| [08-system-validation.md](08-system-validation.md) | 系统验收 | Pending |

状态只使用 Pending、In progress、Partially complete、Complete、Blocked，并必须与 `DEV_PROGRESS.md` 一致。
