# Git 文件级施工树

状态：V1 文件与微提交设计基线（2026-08-07）

正式仓库：当前 Git 工作树的仓库根目录

远端：`git@github.com:NeilBaumanMax/SmartRollator.git`

## 1. 文档用途

本文把 `CONSTRUCTION_PLAN.md` 的能力阶段拆成可提交、可测试、可回滚的文件级增量。它回答：

- 下一个提交创建或替换哪些精确文件；
- 每个手写文件承担什么功能、公开什么接口；
- 哪个测试最先证明该文件有效；
- 失败时回退到哪个父提交；
- 哪些文件由 CubeMX/Keil 生成，不能由 Agent 手写伪造。

本文是计划，不是完成证明。只有对应文件、测试记录、提交哈希和远端哈希全部存在时，才能把某个节点标为 Complete。接口名在首次实现前视为冻结候选；如需改变，先更新本文、架构/契约和 ADR。

## 2. 当前已建立的根节点

```text
reference/archive-20260807  a0669d7  纳入 Git 前的完整参考快照（独立根，只读）

main
  f34420f  docs: establish construction and Git baseline
  b264fe3  docs: record remote Git baseline
  97208dc  docs: record clone recovery verification
  2cd8302  chore(repo): track complete SmartRollator workspace
  75d5609  docs: record active repository baseline
```

`reference/archive-*` 不合入 `main`。`main` 中现有固件是参考基线，不代表当前构建或硬件通过。后续每个阶段从最新已验收 `main` 创建分支；一个微提交只承担下表中的一个节点。

## 3. 分支与提交拓扑

```text
main
 ├─ docs/git-design-tree               文档漂移与本文件
 ├─ build/phase-1-bootstrap            P1.02 → P1.06（P1.01 在 docs 分支完成）
 ├─ build/phase-2-platform             P2.01 → P2.07
 ├─ build/phase-3-contracts            P3.01 → P3.05
 ├─ build/phase-4-motor-canopen        P4.01 → P4.07
 ├─ build/phase-5-force-sensor         P5.01 → P5.06
 ├─ build/phase-6-rtos-pipeline        P6.01 → P6.07
 ├─ build/phase-7-fixed-admittance     P7.01 → P7.06
 ├─ build/phase-8-hopf                 P8.01 → P8.05
 ├─ build/phase-9-cpg-integration      P9.01 → P9.05
 ├─ build/phase-10-observability       P10.01 → P10.06
 └─ build/phase-11-validation          P11.01 → P11.06 → v0.1.0-parity
```

分支不能跨阶段偷带后续文件。每个节点提交前运行该行的最近测试；阶段结束再运行该阶段完整测试集。

表内 `path/name.h/.c` 是精确双文件简写，严格表示 `path/name.h` 与 `path/name.c`，不表示目录通配。表内“最近测试”列出现的仓库相对文件路径也属于该节点的精确提交范围：文件尚不存在时同节点创建，已存在时同节点修改。目录级厂商生成物仅允许按 4.1 节的生成清单展开。

## 4. 目录责任与创建规则

| 路径 | 类型 | 创建/修改规则 |
|---|---|---|
| `SmartRollator.ioc` | 生成配置事实源 | 只由固定版本 CubeMX 修改；与对应生成文件同提交 |
| `Core/` | CubeMX 入口 | 只允许生成内容和 USER CODE 钩子；业务逻辑不得堆入 |
| `Drivers/` | 厂商生成/第三方 | 由 CubeMX 固件包生成；不做业务修改 |
| `Middlewares/` | FreeRTOS/第三方 | 由固定包生成；本地配置通过 `Core/Inc/FreeRTOSConfig.h` |
| `MDK-ARM/` | Keil 工程 | 跟踪可移植工程、启动和链接配置；排除用户状态和输出 |
| `App/common/` | 跨层类型 | 单位、错误码、时间戳、有效性；不得依赖 HAL/RTOS |
| `App/platform/` | L0 适配 | 封装 HAL/RTOS 资源，不包含协议或控制数学 |
| `App/devices/` | L1 协议设备 | 字节编解码、设备状态，不包含控制策略 |
| `App/services/` | L2 实时服务 | 任务、队列、快照、健康和背压 |
| `App/control/` | L3 控制域 | 纯 C、调用者持有状态、无 HAL/RTOS/打印 |
| `App/safety/` | L4 安全仲裁 | 唯一执行器命令出口、状态机、限幅和失效降级 |
| `App/observability/` | L5 可观测性 | 结构化日志、故障和指标；不得阻塞控制 |
| `Tests/host/` | T1 | 主机单元、属性和回放测试 |
| `Tests/fixtures/` | T1 输入 | 金样帧、坏帧、记录切片和参数快照 |
| `Tests/bench/` | T2/T3 | 台架步骤、接线、安全前置条件和证据索引 |
| `Files/` | 对话参考资产 | DOCX、HTML、配图、文档生成脚本；不承载 STM32 构建依赖 |
| `docs/verification/` | 证据索引 | 构建/测试摘要；大型原始数据放外部并记录哈希 |

### 4.1 生成文件的精确性

CubeMX 首次重新生成前，无法诚实预填数百个厂商文件的最终清单。P1.03 必须生成：

- `docs/verification/generated/phase-1-files.txt`：所有生成文件相对路径，一行一个；
- `docs/verification/generated/phase-1.sha256`：每个生成文件 SHA-256；
- `docs/verification/generated/README.md`：CubeMX、FW_H7、目标工具链、生成时间和命令/人工步骤。

清单与哈希生成后，它们才是 `Core/`、`Drivers/`、`Middlewares/`、`MDK-ARM/` 每个生成文件的精确事实源。禁止为了满足计划而伪造尚未生成的文件名。

## 5. Phase 1 — 正式仓库与最小工程

目标：建立可复现、可编译、上电无驱动力的最小 STM32H723VGT6 + FreeRTOS 工程。

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P1.01 文档事实统一 | `docs/INDEX.md`; `docs/product/PRODUCT_REQUIREMENTS.md`; `docs/construction/CODEX_MASTER_REQUIREMENTS.md`, `CODEX_START_HERE.md`, `CONSTRUCTION_PLAN.md`, `DECISIONS.md`, `DEV_PROGRESS.md`, `GITHUB_ROLLBACK.md`, `GIT_CONSTRUCTION_TREE.md`, `HANDOFF.md`, `LOG.md`, `WORKFLOW.md`; `docs/construction/progress/layers/00-foundation.md`, `01-platform-peripherals.md`, `06-hopf-cpg.md`, `08-system-validation.md`; `docs/construction/templates/PHASE_START_TEMPLATE.md`, `TEST_REPORT_TEMPLATE.md`, `WORK_LOG_TEMPLATE.md`; `docs/handover/PROJECT_HANDOVER.md`, `NEXT_AI_AGENT_PROMPT.md` | 正式路径、当前状态、文件级提交树 | Markdown 链接；旧路径扫描；`git diff --check` | `75d5609` |
| P1.02 工具链冻结 | 新建 `docs/verification/TOOLCHAIN.md`, `tools/check_toolchain.sh`, `docs/verification/builds/README.md` | 记录 CubeMX/MDK/Arm Compiler/Pack 版本；脚本只检测不安装；定义构建证据命名 | `sh tools/check_toolchain.sh`; 人工核对工具版本 | P1.01 |
| P1.03 最小 CubeMX 生成 | 替换 `SmartRollator.ioc`; 生成 `Core/Inc/main.h`, `stm32h7xx_hal_conf.h`, `stm32h7xx_it.h`, `FreeRTOSConfig.h`; `Core/Src/main.c`, `stm32h7xx_hal_msp.c`, `stm32h7xx_it.c`, `system_stm32h7xx.c`, `freertos.c`; 生成 `Drivers/`, `Middlewares/`; `MDK-ARM/SmartRollator.uvprojx`, `startup_stm32h723xx.s`; 新建三份 generated 清单 | MCU、24 MHz HSE、240 MHz 时钟、SWD、CMSIS-RTOS2、一个空健康任务；不配置电机业务 | 生成成功；清单无绝对路径；SHA-256 自检 | P1.02 |
| P1.04 上电失能钩子 | 新建 `App/platform/boot_safety.h/.c`; 修改 `Core/Src/main.c` USER CODE 调用 | `BootSafety_Init()` 在外设/RTOS 前设置安全输出；`BootSafety_IsAsserted()` 供自检；不得使能 CAN 电机 | 主机编译 stub；人工审查启动顺序 | P1.03 |
| P1.05 Keil 全量构建 | 新建 `docs/verification/builds/phase-1-minimal.md`; 如必要只修改 `MDK-ARM/SmartRollator.uvprojx` | 记录完整 Rebuild 命令、版本、0 error、警告解释、产物外部哈希 | Keil Rebuild；构建日志摘要 | P1.04 |
| P1.06 Phase 1 收尾 | 更新 `README.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `LOG.md`, `progress/layers/00-foundation.md`; 新建 `backup/phase-1-YYYYMMDD` 远端分支 | 固化构建入口、真实状态和回滚哈希 | 干净检出复测；远端哈希核对 | P1.05 |

P1.04 的候选接口：

```c
void BootSafety_Init(void);
bool BootSafety_IsAsserted(void);
```

## 6. Phase 2 — 平台与外设骨架

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P2.01 板级映射 | 新建 `App/platform/board_config.h`; 修改 `SmartRollator.ioc` 与对应生成文件 | 引脚、时钟、UART/FDCAN 句柄映射；只放已确认常量 | 静态断言；CubeMX 再生成 diff | Phase 1 tag |
| P2.02 单调时钟 | 新建 `App/platform/monotonic_clock.h/.c` | `MonotonicClock_NowMs()`, `MonotonicClock_ElapsedMs()`；处理 uint32 回绕 | `Tests/host/test_monotonic_clock.c` | P2.01 |
| P2.03 USART1 适配 | 新建 `App/platform/uart_log_port.h/.c`; 修改生成的 `usart.h/.c` | `UartLogPort_Init()`, `UartLogPort_TryWrite()`；有界非阻塞发送 | 串口启动日志；拥塞返回测试 | P2.02 |
| P2.04 USART2/RS485 适配 | 新建 `App/platform/rs485_port.h/.c`; 修改 `usart.h/.c` | `Rs485Port_Init()`, `Rs485Port_StartTx()`, `Rs485Port_StartRx()`, `Rs485Port_Abort()`；方向控制不使用业务延时 | DE 时序示波/逻辑分析；超时测试 | P2.03 |
| P2.05 FDCAN1 适配 | 新建 `App/platform/fdcan_port.h/.c`; 修改生成的 `fdcan.h/.c` | `FdcanPort_Init()`, `FdcanPort_TrySend()`, `FdcanPort_Read()`, `FdcanPort_GetBusState()` | 500 kbit/s 分析仪；发送失败可见 | P2.04 |
| P2.06 启动自检 | 新建 `App/platform/platform_self_test.h/.c` | `PlatformSelfTest_Run()`, `PlatformSelfTest_GetResult()`；失败保持 BootSafety | 逐外设故障注入 | P2.05 |
| P2.07 收尾 | 更新 `docs/verification/bench/platform.md`, `LOG.md`, `HANDOFF.md`, `progress/layers/01-platform-peripherals.md` | 记录引脚、波特率、位速率和 T2 证据 | Phase 2 完整测试 | P2.06 |

## 7. Phase 3 — 数据契约与主机测试框架

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P3.01 公共状态 | 新建 `App/common/app_status.h`, `app_time.h`, `app_units.h` | 统一 `AppStatus`, `AppTimestampMs`, 单位/范围注释；无函数副作用 | `Tests/host/test_common_contracts.c` | Phase 2 tag |
| P3.02 领域类型 | 新建 `App/common/app_types.h` | `ForceSample`, `MotionIntent`, `WheelCommand`, `DriverStatus`, `VisionPose`, `SystemHealth`; 每个含时间戳/有效性 | sizeof/字段/单位静态断言 | P3.01 |
| P3.03 参数模型 | 新建 `App/common/app_config.h/.c` | `AppConfig_SetDefaults()`, `AppConfig_Validate()`；所有参数含单位、范围和来源 | `Tests/host/test_app_config.c` | P3.02 |
| P3.04 主机入口 | 新建 `Tests/host/CMakeLists.txt`, `Tests/host/test_main.c`, `Tests/host/test_assert.h`, 根 `CMakeLists.txt` | 单一 `cmake/ctest` 入口；非零失败码 | 干净主机构建与 ctest | P3.03 |
| P3.05 收尾 | 更新 `LAYER_CONTRACT.md`, `ARCHITECTURE.md`, `LOG.md`, `HANDOFF.md`, `progress/layers/00-foundation.md` | 冻结字段、所有权和依赖 | 头文件依赖扫描 | P3.04 |

## 8. Phase 4 — ZLAC8015D / CANopen

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P4.01 CANopen 编解码 | 新建 `App/devices/canopen_codec.h/.c` | `Canopen_EncodeNmt()`, `Canopen_EncodeSdoWrite()`, `Canopen_DecodeSdo()`, `Canopen_DecodeHeartbeat()`, `Canopen_EncodeVelocityPdo()` | `Tests/host/test_canopen_codec.c` | Phase 3 tag |
| P4.02 驱动参数 | 新建 `App/devices/zlac8015d_config.h/.c` | `ZlacConfig_SetDefaults()`, `ZlacConfig_Validate()`；节点、COB-ID、方向、rpm 范围 | `Tests/host/test_zlac_config.c` | P4.01 |
| P4.03 设备状态机 | 新建 `App/devices/zlac8015d.h/.c` | `Zlac_Init()`, `Zlac_RequestDisable()`, `Zlac_RequestEnable()`, `Zlac_ProcessFrame()`, `Zlac_BuildVelocityCommand()`, `Zlac_GetStatus()` | `Tests/host/test_zlac_state_machine.c` | P4.02 |
| P4.04 故障与反馈 | 扩展 `App/devices/zlac8015d.h/.c`; 新建 `Tests/fixtures/canopen/heartbeat.txt`, `velocity.txt`, `fault.txt`, `README.md`; 新建 `Tests/host/test_zlac_feedback.c` | 心跳、实际速度、故障码、bus-off、超时 | `Tests/host/test_zlac_feedback.c` fixture 回放与超时测试 | P4.03 |
| P4.05 最小安全层 | 新建 `App/safety/safety_state.h/.c`, `App/safety/safety_arbiter.h/.c` | `SafetyState_Init()`, `SafetyState_Update()`, `SafetyArbiter_Apply()`；默认输出 disable/zero | `Tests/host/test_safety_state.c`, `test_safety_arbiter.c` | P4.04 |
| P4.06 平台接入 | 新建 `App/devices/motor_device.h/.c`; 修改最小任务入口 | `MotorDevice_InitDisabled()`, `MotorDevice_Poll()`, `MotorDevice_Apply()`；只接受仲裁后的命令 | 抬轮 ±20/±50 rpm；断线归零 | P4.05 |
| P4.07 收尾 | 新建 `Tests/bench/motor-canopen.md`, `docs/verification/bench/motor-canopen.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/03-motor-canopen.md` | 记录节点、方向、单位、急停和证据 | Phase 4 完整测试 | P4.06 |

## 9. Phase 5 — RS485 力传感器

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P5.01 CRC 与字节工具 | 新建 `App/devices/modbus_crc.h/.c`, `byte_order.h` | `ModbusCrc16()`, 大端 float 安全解码 | `Tests/host/test_modbus_crc.c` | Phase 4 tag |
| P5.02 帧协议 | 新建 `App/devices/force_sensor_protocol.h/.c` | `ForceProtocol_BuildRequest()`, `ForceProtocol_ParseResponse()`；验证地址/功能/字节数/长度/CRC/有限值 | `Tests/host/test_force_protocol.c` | P5.01 |
| P5.03 金样与坏帧 | 新建 `Tests/fixtures/force/valid.bin`, `bad_crc.bin`, `short.bin`, `nan.bin`; 新建 fixture README | 固定输入来源、期望值和哈希 | fixture 全量回放 | P5.02 |
| P5.04 设备状态机 | 新建 `App/devices/force_sensor.h/.c` | `ForceSensor_Init()`, `ForceSensor_StartPoll()`, `ForceSensor_OnRx()`, `ForceSensor_OnTimeout()`, `ForceSensor_GetLatest()`, `ForceSensor_GetStats()` | `Tests/host/test_force_sensor.c` | P5.03 |
| P5.05 标定 | 新建 `App/devices/force_calibration.h/.c`, `docs/verification/calibration/force.md` | `ForceCalibration_Apply()`, `ForceCalibration_Validate()`；零点、缩放、坐标和范围 | 0/5/10/20 N 台架记录 | P5.04 |
| P5.06 收尾 | 新建 `Tests/bench/force-sensor.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/02-force-sensing.md` | 采样率、错误率、方向和最后有效时间 | Phase 5 完整测试 | P5.05 |

## 10. Phase 6 — RTOS 数据管线

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P6.01 队列契约 | 新建 `App/services/app_queues.h/.c` | `AppQueues_Init()`, typed put/get；容量与溢出计数 | `Tests/host/test_queue_policy.c` | Phase 5 tag |
| P6.02 最新值快照 | 新建 `App/services/latest_snapshot.h/.c` | `LatestSnapshot_Publish*()`, `LatestSnapshot_Read*()`；时间戳和一致性 | `Tests/host/test_latest_snapshot.c` | P6.01 |
| P6.03 传感服务 | 新建 `App/services/sensor_service.h/.c` | `SensorService_Init()`, `SensorService_RunOnce()`；100 Hz 候选周期，不阻塞控制 | 主机调度 stub；目标周期测量 | P6.02 |
| P6.04 控制服务 | 新建 `App/services/control_service.h/.c` | `ControlService_Init()`, `ControlService_RunOnce()`；只消费领域对象 | 陈旧输入拒绝测试 | P6.03 |
| P6.05 电机服务 | 新建 `App/services/motor_service.h/.c` | `MotorService_Init()`, `MotorService_RunOnce()`；丢弃过期命令，只调用 SafetyArbiter/MotorDevice | 过期/队列满测试 | P6.04 |
| P6.06 健康服务 | 新建 `App/services/health_service.h/.c` | `HealthService_Heartbeat()`, `HealthService_Evaluate()`；任务失活与输入超时 | `Tests/host/test_health_service.c` | P6.05 |
| P6.07 RTOS 接入与收尾 | 修改 `Core/Src/freertos.c` USER CODE 入口；新建 `docs/verification/timing/rtos.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/04-rtos-pipeline.md` | 任务只调用服务 RunOnce；记录周期、抖动、栈和 CPU | T2 时序/拥塞/看门狗测试 | P6.06 |

## 11. Phase 7 — 固定导纳基线

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P7.01 数值工具 | 新建 `App/control/control_math.h/.c` | `Control_ClampFinite()`, `Control_IsFinite()`, `Control_SlewLimit()` | `Tests/host/test_control_math.c` | Phase 6 tag |
| P7.02 固定导纳 | 新建 `App/control/fixed_admittance.h/.c` | `FixedAdmittance_Init()`, `Reset()`, `ValidateParams()`, `Step()`；调用者持有状态 | `Tests/host/test_fixed_admittance.c` | P7.01 |
| P7.03 运动映射 | 新建 `App/control/motion_mapping.h/.c` | `MotionMapping_ToWheelRpm()`, `MotionMapping_Validate()`；轮径/轮距/方向显式参数 | `Tests/host/test_motion_mapping.c` | P7.02 |
| P7.04 命令限幅 | 新建 `App/safety/command_limits.h/.c` | `CommandLimits_Apply()`；速度、加速度、斜率、有限值和原因位 | `Tests/host/test_command_limits.c` | P7.03 |
| P7.05 策略接口 | 新建 `App/control/control_strategy.h/.c` | `ControlStrategy_Init()`, `Select()`, `Step()`；此时只允许 FIXED_ADMITTANCE | `Tests/host/test_control_strategy.c` | P7.04 |
| P7.06 回放/台架/收尾 | 新建 `Tests/fixtures/replay/fixed-admittance.csv`, `Tests/host/test_fixed_replay.c`, `Tests/bench/fixed-admittance.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/05-admittance-baseline.md` | 阶跃、符号、饱和、失联和抬轮证据 | Phase 7 完整测试 | P7.05 |

## 12. Phase 8 — Hopf CPG

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P8.01 参数与状态 | 新建 `App/control/hopf_cpg.h` | `HopfParams`, `HopfState`, `HopfDebug`; 单位、范围、无 HAL 类型 | 头文件独立编译 | Phase 7 tag |
| P8.02 纯 C 实现 | 新建 `App/control/hopf_cpg.c` | `Hopf_Init()`, `Hopf_Reset()`, `Hopf_ValidateParams()`, `Hopf_SetFrequencyHz()`, `Hopf_Step()` | `Tests/host/test_hopf_cpg.c` | P8.01 |
| P8.03 数值长测 | 新建 `Tests/host/test_hopf_long_run.c`, `Tests/fixtures/hopf/default-params.txt` | 收敛、幅频、dt 敏感性、NaN/Inf、长时有限值 | ctest 长测标签 | P8.02 |
| P8.04 离线轨迹 | 新建 `tools/analyze_hopf.py`, `docs/verification/control/hopf.md` | 从测试 CSV 生成相图/r(t)/频率误差；不修改算法 | 固定输入输出哈希 | P8.03 |
| P8.05 收尾 | 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/06-hopf-cpg.md` | 冻结通过阈值和参数版本 | Phase 8 完整测试 | P8.04 |

## 13. Phase 9 — CPG 变导纳集成

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P9.01 变导纳模块 | 新建 `App/control/variable_admittance.h/.c` | `VariableAdmittance_Init()`, `Reset()`, `ValidateParams()`, `Step()`；输出 `D_current` 和速度 | `Tests/host/test_variable_admittance.c` | Phase 8 tag |
| P9.02 调制映射 | 新建 `App/control/cpg_modulation.h/.c` | `CpgModulation_ComputeDamping()`；Hopf x 到阻尼，显式上下限 | `Tests/host/test_cpg_modulation.c` | P9.01 |
| P9.03 策略集成 | 修改 `control_strategy.h/.c` | 增加 VARIABLE_ADMITTANCE；模式切换先复位并受 SafetyState 许可 | `test_control_strategy.c` 切换瞬态 | P9.02 |
| P9.04 A/B 回放 | 新建 `Tests/host/test_control_ab_replay.c`, `tools/compare_control_modes.py`, `docs/verification/control/fixed-vs-cpg.md` | 同一输入比较固定/变导纳，不选择“更好”替代安全阈值 | 可复现 CSV/指标 | P9.03 |
| P9.05 台架与收尾 | 新建 `Tests/bench/cpg-integration.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/06-hopf-cpg.md` | 低速切换、极端输入、失联，保持固定模式可回退 | Phase 9 完整测试 | P9.04 |

## 14. Phase 10 — 可观测性与视觉输入

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P10.01 遥测模型 | 新建 `App/observability/telemetry.h/.c` | `Telemetry_Init()`, `Telemetry_Publish()`, `Telemetry_TryRead()`；结构化字段、丢弃计数 | `Tests/host/test_telemetry.c` | Phase 9 tag |
| P10.02 故障记录 | 新建 `App/observability/fault_log.h/.c` | `FaultLog_Record()`, `FaultLog_ReadLatest()`；有界环形缓冲 | `Tests/host/test_fault_log.c` | P10.01 |
| P10.03 日志服务 | 新建 `App/services/log_service.h/.c` | `LogService_Init()`, `LogService_RunOnce()`；50–100 Hz 目标、拥塞降级 | 日志压力/控制不阻塞测试 | P10.02 |
| P10.04 视觉协议 | 新建 `App/devices/vision_protocol.h/.c`, `Tests/fixtures/vision/valid.bin`, `bad_checksum.bin`, `short.bin`, `README.md` | `VisionProtocol_ParseAruco()`；长度、字段范围、异或校验和超时 | `Tests/host/test_vision_protocol.c` 与四个 fixture 文件 | P10.03 |
| P10.05 视觉角色适配 | 新建 `App/devices/vision_input.h/.c` | `VisionInput_OnFrame()`, `VisionInput_GetLatest()`；默认只记录，若限速须走 SafetyArbiter | 超时/坏帧/角色配置测试 | P10.04 |
| P10.06 收尾 | 新建 `docs/verification/observability/telemetry.md`; 更新 `docs/construction/LOG.md`, `HANDOFF.md`, `DEV_PROGRESS.md`, `progress/layers/07-observability-safety.md` | 字段、频率、丢日志和视觉失效证据 | Phase 10 完整测试 | P10.05 |

## 15. Phase 11 — 系统安全与等价验收

| 节点 | 精确文件 | 文件功能/主要接口 | 最近测试 | 回滚点 |
|---|---|---|---|---|
| P11.01 完整安全转换 | 扩展 `safety_state.h/.c`, `safety_arbiter.h/.c`; 新建 `Tests/host/test_safety_transitions.c` | BOOT/SELF_TEST/READY/ENABLED/DEGRADED/FAULT/SAFE_STOP；所有异常可达无驱动力 | 状态转换表全覆盖 | Phase 10 tag |
| P11.02 故障注入 | 新建 `Tests/host/test_fault_injection.c`, `Tests/bench/fault-injection.md` | 力超时、CAN 故障、命令过期、任务失活、NaN、急停、重启 | T1 + T3 故障矩阵 | P11.01 |
| P11.03 性能与长稳 | 新建 `Tests/bench/long-run.md`, `docs/verification/system/timing.md`, `docs/verification/system/long-run.md` | 周期/抖动/CPU/栈/日志带宽/长稳结果 | T2/T4 记录 | P11.02 |
| P11.04 等价矩阵 | 新建 `docs/verification/system/parity-matrix.md` | 逐功能比较参考行为、当前实现和证据；无证据写 Not run | 人工审查每行证据 | P11.03 |
| P11.05 发布清单 | 新建 `CHANGELOG.md`, `docs/verification/releases/v0.1.0-parity.md` | commit、工具链、参数、硬件、构建和测试索引 | 干净克隆复现 | P11.04 |
| P11.06 收尾与标签 | 更新 `docs/construction/DEV_PROGRESS.md`, `LOG.md`, `HANDOFF.md`, `progress/layers/00-foundation.md`, `01-platform-peripherals.md`, `02-force-sensing.md`, `03-motor-canopen.md`, `04-rtos-pipeline.md`, `05-admittance-baseline.md`, `06-hopf-cpg.md`, `07-observability-safety.md`, `08-system-validation.md`; 创建 `release/v0.1.0-parity` 与注释标签 | 只在全部适用验收通过后标 Complete | 远端分支/标签哈希核对 | P11.05 |

## 16. 每个微提交的固定检查

```bash
git status --short --branch
git diff -- <explicit-files>
git add <explicit-files>
git diff --cached --check
git diff --cached --stat
```

随后执行该节点表中的最近测试。提交正文必须包含：

```text
Why:
Files and interfaces:
Validation command or hardware step:
Result:
Known limitations:
Rollback parent:
```

禁止把两个节点压成一个大提交；禁止把首次失败从 `LOG.md` 删除；禁止在相邻测试失败时继续创建下一个功能文件。

## 17. 文件创建完成判定

一个文件只有同时满足以下条件才算“已创建”：

1. 文件存在于当前施工分支，不只是本文列名；
2. 公开接口、单位、范围、所有权、线程上下文和错误语义已写清；
3. 最近测试已执行并记录真实结果；
4. 文件被精确暂存到单一目的提交；
5. 提交已推送并核对远端哈希；
6. `DEV_PROGRESS.md`、对应层级页和 `HANDOFF.md` 状态一致。

缺少任一项时，只能写 Planned、In progress 或 Partially complete，不能写 Complete。
