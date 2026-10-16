# AGENT_CONTEXT.md — SmartRollator 智能助行车

> **面向对象**：其他 AI Agent，读完本文即可理解工程全貌并开始开发。
> **最后更新**：2026-07-01

---

## 1. 工程一句话

**SmartRollator 是一款基于 STM32H723 + FreeRTOS 的智能助行车嵌入式控制系统，核心算法为 Hopf CPG（中枢模式发生器）+ 变导纳控制，通过六维力传感器感知用户意图，驱动双轮差速电机实现柔顺助力。**

---

## 2. 硬件平台

| 项目 | 规格 |
|------|------|
| **MCU** | STM32H723VGTx (ARM Cortex-M7, 550MHz, 1MB Flash, 564KB RAM) |
| **RTOS** | FreeRTOS V10.3.1 + CMSIS-RTOS v2 |
| **力传感器** | 六维力传感器（把手），RS485 MODBUS RTU, 115200-8-N-1, 100Hz 轮询 |
| **电机驱动** | 达妙 ZLAC8015D ×2（左右轮），CANopen 协议，速度模式 |
| **IMU** | 预留接口，当前未接入 |
| **视觉** | ArUco 标记定位，通过 USART1 接收 `$ARUCO,...*CS#` 帧 |
| **CAN** | FDCAN1 (500kbps) → 电机控制；FDCAN2 (~1.3Mbps) → 预留 |
| **串口** | USART1 (CLI/日志/Vision)、USART2 (RS485 力传感器) |
| **调试接口** | SWD (ST-Link) |
| **编译器** | Keil MDK-ARM（当前）；arm-none-eabi-gcc（脚手架目标） |

---

## 3. 目录结构

```
smartrollator/
├── docs/                          # 项目文档
│   ├── architecture.md            # 系统架构说明
│   ├── dataflow.md                # 数据流全景
│   ├── api.md                     # API/接口文档
│   └── getting-started.md         # 快速上手指南
├── firmware/                      # 固件源码
│   ├── Core/                      # CubeMX 生成核心代码
│   │   ├── Inc/                   # 头文件 (main.h, fdcan.h, gpio.h, usart.h, FreeRTOSConfig.h, ...)
│   │   └── Src/                   # 源文件 (main.c, freertos.c, fdcan.c, gpio.c, usart.c, ...)
│   ├── Drivers/                   # STM32H7 HAL 库 + CMSIS
│   │   ├── STM32H7xx_HAL_Driver/
│   │   └── CMSIS/
│   ├── Middlewares/               # FreeRTOS 源码
│   │   └── Third_Party/FreeRTOS/
│   ├── App/                       # ★ 用户核心业务代码
│   │   ├── Controller.h / .c      # 顶层控制器
│   │   ├── cpg_control.h / .c     # Hopf CPG + 变导纳控制
│   │   ├── data_structs.h         # 全局数据结构
│   │   ├── torque_sensor.h / .c   # 力传感器 MODBUS 解析
│   │   └── user_can.h / .c        # CANopen 通信封装
│   └── SmartRollator.ioc          # CubeMX 工程配置
├── tools/                         # 开发工具脚本
│   ├── flash.sh                   # 烧录脚本
│   ├── debug.sh                   # 调试脚本
│   └── monitor.py                 # 串口监控/调参脚本
├── tests/                         # 测试
│   ├── unit/                      # 单元测试模板
│   └── integration/               # 集成测试模板
├── .gitignore
├── Makefile                       # arm-none-eabi-gcc 构建入口
├── README.md                      # 人类开发者文档
└── AGENT_CONTEXT.md               # 本文件
```

---

## 4. 核心数据流链路（完整）

```
┌──────────────┐    RS485 MODBUS     ┌──────────────────┐   ForceQueue    ┌──────────────────┐
│ 六维力传感器  │ ─────────────────→ │ ForceSensorTask   │ ─────────────→ │   ControlTask    │
│ (把手)       │   100Hz, 29B 帧     │ prio=AboveNormal  │  mq_get_latest │ prio=Realtime    │
└──────────────┘                     │ 解析→ForceData_t  │  (只取最新)    │ 1kHz vTaskDelay  │
                                     └──────────────────┘                └────────┬─────────┘
                                                                                   │
                              ┌────────────────────────────────────────────────────┘
                              │
                              ▼
                     ┌─────────────────┐
                     │   Controller_Run │
                     │                  │
                     │  force.fx → F_ext│
                     │         │        │
                     │         ▼        │
                     │    CPG_Step()    │
                     │  ┌────────────┐  │
                     │  │ Hopf 振荡器 │  │  dx = α(μ-r²)x - ωy + ε·F_ext
                     │  │  r²=x²+y²  │  │  dy = α(μ-r²)y + ωx
                     │  │  x+=dx·dt  │  │  α=10, μ=1, ω≈5.655 (0.9Hz)
                     │  │  y+=dy·dt  │  │  ε=0.01
                     │  ├────────────┤  │
                     │  │ 变导纳控制 │  │  D=D_base-λ·x (D∈[15,80])
                     │  │              │  │  a_c=(F-D·v_c)/M
                     │  │ 输出 v_cmd  │  │  v_c += a_c·dt
                     │  │ (m/s)       │  │  M=15, λ=20, D_base=40
                     │  └────────────┘  │
                     │         │        │
                     │    rpm=mps_to_rpm│
                     │    限幅±400rpm   │
                     │         │        │
                     │  out_cmd.target_vel│
                     └────────┬────────┘
                              │
                     MotorCmdQueue
                              │
                              ▼
                     ┌──────────────────┐     CANopen PDO     ┌──────────────────┐
                     │   MotorTxTask    │ ─────────────────→ │  ZLAC8015D ×2    │
                     │ prio=Realtime1   │  0x300+ID, 4B      │  左轮 +rpm       │
                     │ ZLAC_SetVelocity │                     │  右轮 -rpm       │
                     └──────────────────┘                     └──────────────────┘
                              ↑
                              │  FDCAN1 500kbps
                              │
                     ┌──────────────────┐
                     │ CAN_Worker_Task   │  (当前未实现电机反馈接收)
                     └──────────────────┘

┌──────────────┐     USART1          ┌──────────────────┐
│ ArUco 视觉   │ ─────────────────→  │   Log_task       │
│ $ARUCO,...   │    $ARUCO,...*CS#   │ prio=BelowNormal7│
└──────────────┘                     │ CLI: h/p/t       │
                                     │ 100ms 周期打印    │
                                     └──────────────────┘
```

### 监控与心跳

- **monitor_snapshot_t**：全局监控快照（Force + MotorCmd + VisionPose），互斥量保护
- **HeartbeatFlags**：事件标志位，WatchdogTask 检查各任务心跳
- **SystemStatusFlags**：系统状态标志

---

## 5. RTOS 任务清单

| 任务名 | 优先级 | 栈 (words) | 周期 | 职责 |
|--------|--------|-----------|------|------|
| ForceSensorTask | AboveNormal | 512 | 10ms (100Hz) | RS485 轮询力传感器，解析并投递 ForceData_t |
| CAN_Worker_Task | High | 512 | 1ms | CAN 消息收发（待完善） |
| IMUtask | AboveNormal | 512 | 1ms | IMU 数据读取（待接入） |
| ControlTask | Realtime | 1024 | 1ms (1kHz) | 控制主循环：读传感器 → CPG 计算 → 输出命令 |
| WatchdogTask | High7 | 256 | 1ms | 心跳监控，异常处理（待完善） |
| Log_task | BelowNormal7 | 1024 | 5ms | CLI 命令解析 + 100ms 周期状态打印 |
| MotorTxTask | Realtime1 | 512 | 阻塞 | 消费 MotorCmdQueue → CAN 发送速度指令 |

### 同步原语

| 名称 | 类型 | 用途 |
|------|------|------|
| ForceQueueHandle | Queue(8×ForceData_t) | 力传感器 → 控制任务 |
| IMUQueueHandle | Queue(8×IMUData_t) | IMU → 控制任务 |
| IMUProcessedQueueHandle | Queue(8×IMUData_t) | IMU 后处理 |
| MotorFbQueueHandle | Queue(8×MotorFb_t) | 电机反馈 |
| MotorCmdQueueHandle | Queue(8×MotorCmd_t) | 控制命令 → 电机任务 |
| LogQueueHandle | Queue(8×LogMsg_t) | 日志消息队列 |
| MonitorMutex | Mutex | 保护 g_mon 全局快照 |
| ForceRxSemHandle | Semaphore | 力传感器接收完成信号 |
| CANRxSemHandle | Semaphore | CAN 接收完成信号 |
| HeartbeatFlagsHandle | EventFlags | 各任务心跳位 |
| SystemStatusFlagsHandle | EventFlags | 系统状态位 |
| CtrlLoopTimerHandle | Timer | 控制循环定时器（备用） |
| LogFlushTimerHandle | Timer | 日志刷新定时器（备用） |

---

## 6. 数据结构

```c
// 六维力传感器数据
typedef struct {
    float fx, fy, fz;
    float torque_x, torque_y;
    uint32_t timestamp;
} ForceData_t;

// IMU 数据（预留）
typedef struct {
    float accel[3], gyro[3], quat[4];
    float roll, pitch, yaw;
    uint32_t timestamp;
} IMUData_t;

// 电机反馈
typedef struct {
    int32_t pos, vel;
    int16_t torque;
    uint8_t status;
    uint32_t timestamp;
} MotorFb_t;

// 电机控制命令
typedef struct {
    int32_t target_pos, target_vel, target_acc;
    int16_t target_torque;
    uint8_t ctrl_mode;    // 1=速度模式
    uint32_t timestamp;
} MotorCmd_t;

// ArUco 视觉位姿
typedef struct {
    uint16_t seq, marker_id;
    uint8_t status;
    int16_t x_mm, y_mm, z_mm;
    int16_t roll_x10, pitch_x10, yaw_x10, dist_mm;
    uint32_t timestamp;
} VisionPose_t;

// 监控快照
typedef struct {
    ForceData_t  force;
    MotorCmd_t   cmd;
    VisionPose_t vision;
    uint32_t     ts_force, ts_cmd, ts_vision;
} monitor_snapshot_t;
```

---

## 7. CPG 控制参数

| 参数 | 符号 | 默认值 | 含义 | 范围 |
|------|------|--------|------|------|
| alpha | α | 10.0 | Hopf 振荡器收敛速度 | >0.0001 |
| mu | μ | 1.0 | 目标幅值平方 | >0.0001 |
| omega | ω | 2π×0.9 ≈ 5.655 | 振荡角频率 (rad/s) | [-100, 100] |
| epsilon | ε | 0.01 | 外力输入耦合系数 | [-10, 10] |
| M | M | 15.0 | 导纳虚拟质量 | >0.0001 |
| D_base | D₀ | 40.0 | 基础阻尼 | >0.0001 |
| lambda | λ | 20.0 | CPG 状态→阻尼增益 | [-200, 200] |
| WHEEL_RADIUS_M | — | 0.075 | 轮子半径 (m) | — |
| MOTOR_RPM_LIMIT | — | 400.0 | 电机转速上限 (rpm) | — |

### 安全限幅

| 限幅项 | 范围 |
|--------|------|
| 外力 F_ext | ±500 N |
| Hopf 状态 x, y | ±5.0 |
| 导数 dx, dy | ±200.0 |
| 导纳阻尼 D | [15, 80] |
| 导纳速度 v_c | [-0.5, 1.2] m/s |
| 积分步长 dt | [0.1ms, 100ms] |

---

## 8. 时钟树

```
HSE 25MHz → PLL(M=2,N=20) → VCO=250MHz
  ├── PLLP=1 → SYSCLK=250MHz → HCLK=250MHz (DIV1)
  ├── PLLQ=4 → 62.5MHz (FDCAN 时钟源)
  └── PLLR=2 → 125MHz

总线: APB1/2/3/4 = 125MHz (HCLK/2)
FDCAN1: 500kbps  (62.5MHz / 10 / (9+2+1))
FDCAN2: ~1.3Mbps (62.5MHz / 16 / (1+1+1))
USART1/2: 125MHz APB 时钟源
```

---

## 9. 编译/烧录/调试

### 当前方式（Keil MDK-ARM）
- 工程文件：`firmware/MDK-ARM/SmartRollator.uvprojx`
- 编译器：ARMCC V6
- 调试器：ST-Link (SWD)

### 脚手架目标（arm-none-eabi-gcc）
```bash
make              # 编译
make flash        # 烧录 (OpenOCD / ST-Link)
make clean        # 清理
make debug        # 启动 GDB 调试
```

---

## 10. 当前完成度与待办项

### 已完成
- [x] STM32CubeMX 工程配置（时钟/外设/FreeRTOS）
- [x] FreeRTOS 多任务框架（7 个任务 + 消息队列 + 同步原语）
- [x] 六维力传感器 RS485 MODBUS 驱动与解析
- [x] CANopen 电机通信封装（ZLAC8015D PDO/NMT/SDO）
- [x] Hopf CPG + 变导纳控制算法实现（含全链路 NaN/Inf 防护）
- [x] ArUco 视觉帧解析（USART1 `$ARUCO,...*CS#`）
- [x] CLI 命令行交互（h/p/t 命令）
- [x] 监控快照机制（互斥量保护）
- [x] arm-none-eabi-gcc Makefile 模板

### 待办项
- [ ] IMU 传感器接入与数据融合
- [ ] 电机反馈接收（CAN Rx 中断 + TPDO 解析）
- [ ] Watchdog 心跳监控完整逻辑
- [ ] 安全保护：急停、失速检测、传感器超时急停
- [ ] CPG 参数在线调参（通过 CLI）
- [ ] Flash 参数持久化存储
- [ ] 单元测试 + HITL/SITL 测试框架
- [ ] 从 Keil MDK 迁移到 arm-none-eabi-gcc 工具链

---

## 11. 安全约束

1. **电机输出限幅**：转速限制 ±400 rpm，速度 ±1.2 m/s
2. **NaN/Inf 全链路防护**：CPG_Step() 入口对 F_ext、dt、所有内部状态做 CPG_ClampFinite 检查
3. **数据新鲜度**：ControlTask 中力数据 >100ms 未更新则清零，避免过期数据驱动
4. **RS485 方向控制**：发送前 DE=1，发送后等待 TC 置位，再切 DE=0 接收
5. **队列溢出处理**：mq_get_latest 丢弃旧消息，确保控制任务始终使用最新传感器数据
6. **右轮方向**：右轮安装方向与左轮相反，MotorTxTask 中将右轮 rpm 取反，**必须保持幅值严格一致**

---

## 12. 注意事项（供 Agent 开发时参考）

- `main.h` 中通过 `USER CODE BEGIN Includes` 引入 `data_structs.h` 和 `user_can.h`
- CubeMX 生成的 `freertos.c` 包含了所有任务函数体和自定义代码（约 940 行），再次用 CubeMX 生成代码前需备份 `USER CODE` 块
- `Controller.c` 中 `WHEEL_RADIUS_M` 宏影响速度换算，实际装车后需按实测轮径校准
- `user_can.h` 中 `ZLAC_SetVelocity_rpm()` 为 `static inline`，直接包含于调用者编译单元
- 原 `MDK-ARM/` 目录下的 `.o`、`.crf`、`.d`、`.axf` 等为 Keil 编译产物，已在 `.gitignore` 中排除
- 思维导图 txt 和 SVG 图表已归档至 `docs/` 下
