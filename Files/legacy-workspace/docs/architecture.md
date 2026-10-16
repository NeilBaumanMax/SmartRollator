# SmartRollator 系统架构说明

> 最后更新：2026-07-01

## 一、整体架构

SmartRollator 是一个典型的 **感知 → 决策 → 执行** 闭环嵌入式系统，分为四层：

```
┌──────────────────────────────────────────────────────┐
│                    应用层 (App/)                      │
│  Controller  |  CPG Control  |  Torque Sensor  | CAN │
├──────────────────────────────────────────────────────┤
│                中间件层 (Middlewares/)                │
│                 FreeRTOS V10.3.1                      │
├──────────────────────────────────────────────────────┤
│                 驱动层 (Drivers/)                     │
│          STM32H7xx HAL + CMSIS                       │
├──────────────────────────────────────────────────────┤
│                硬件层 (Hardware)                      │
│  STM32H723VGTx | RS485 | FDCAN | USART | GPIO        │
└──────────────────────────────────────────────────────┘
```

## 二、软件分层详解

### 2.1 应用层 (App/)

用户核心业务代码，独立于 CubeMX 生成代码：

| 模块 | 文件 | 职责 |
|------|------|------|
| Controller | `Controller.h/.c` | 顶层控制：汇总传感器数据 → 调用 CPG → 输出电机命令 |
| CPG Control | `cpg_control.h/.c` | Hopf CPG 振荡器 + 变导纳控制算法 |
| Torque Sensor | `torque_sensor.h/.c` | 六维力传感器 MODBUS 协议解析 + RS485 方向控制 |
| CAN Driver | `user_can.h/.c` | CANopen 封装：PDO 配置、NMT 控制、速度指令发送 |
| Data Structs | `data_structs.h` | 全局数据结构定义 |

### 2.2 核心层 (Core/)

STM32CubeMX 自动生成的 HAL 框架代码：

| 模块 | 关键文件 | 职责 |
|------|---------|------|
| main.c | `Core/Src/main.c` | 系统入口：HAL_Init → 时钟配置 → 外设初始化 → FreeRTOS 启动 |
| freertos.c | `Core/Src/freertos.c` | **核心文件**：所有任务/队列/信号量/互斥量/定时器创建 + 任务函数体 |
| fdcan.c | `Core/Src/fdcan.c` | FDCAN1/2 外设初始化 |
| usart.c | `Core/Src/usart.c` | USART1/2 外设初始化 |
| gpio.c | `Core/Src/gpio.c` | GPIO 时钟使能 |
| stm32h7xx_it.c | `Core/Src/stm32h7xx_it.c` | 中断服务例程 (SysTick/USART1/FDCAN1) |

### 2.3 驱动层 (Drivers/)

| 组件 | 路径 | 说明 |
|------|------|------|
| HAL 库 | `Drivers/STM32H7xx_HAL_Driver/` | STM32H7 硬件抽象层 |
| CMSIS | `Drivers/CMSIS/` | Cortex-M 软件接口标准（含启动文件、系统时钟初始化） |

### 2.4 中间件层 (Middlewares/)

| 组件 | 路径 | 说明 |
|------|------|------|
| FreeRTOS | `Middlewares/Third_Party/FreeRTOS/` | 实时操作系统内核 + CMSIS-RTOS v2 适配层 |

## 三、RTOS 任务调度

### 优先级分配

```
osPriorityRealtime1     (55)  MotorTxTask        ← 最高（保证电机指令低延迟）
osPriorityRealtime      (54)  ControlTask        ← 1kHz 控制主循环
osPriorityHigh7         (47)  WatchdogTask        ← 心跳监控
osPriorityHigh          (40)  CAN_Worker_Task     ← CAN 消息收发
osPriorityAboveNormal   (30)  ForceSensorTask     ← 100Hz 力传感器
osPriorityAboveNormal   (30)  IMUtask             ← IMU 数据读取
osPriorityBelowNormal7  (7)   Log_task            ← 最低（日志/CLI）
```

### 同步机制

任务间使用 FreeRTOS 消息队列传递数据，每条队列容量 8 个元素。关键同步原语：

- **ForceQueue** → 力传感器数据传给 ControlTask
- **MotorCmdQueue** → ControlTask 命令传给 MotorTxTask
- **MonitorMutex** → 保护全局监控快照 `g_mon`
- **HeartbeatFlags** → 各任务心跳汇报给 WatchdogTask

## 四、硬件连接

| 外设 | MCU 引脚/接口 | 外设端 | 参数 |
|------|-------------|--------|------|
| FDCAN1 | PB8(RX)/PB9(TX) | ZLAC8015D 电机驱动 | 500kbps |
| FDCAN2 | PB5(RX)/PB6(TX) | 预留 | ~1.3Mbps |
| USART1 | PA9(TX)/PA10(RX) | ArUco 视觉 / CLI | 115200-8-N-1 |
| USART2 | PD5(TX)/PD6(RX) | 六维力传感器 (RS485) | 115200-8-N-1 |
| RS485 方向 | PD4 | RS485 DE/RE | GPIO 输出 |
| SWD | PA13(SWDIO)/PA14(SWCLK) | ST-Link | — |

## 五、设计原则

1. **CubeMX 用户代码隔离**：所有自定义代码写在 `USER CODE BEGIN/END` 块中，便于 CubeMX 重新生成
2. **只取最新数据**：队列消费端使用 `mq_get_latest()` 丢弃旧消息
3. **安全第一**：全链路 NaN/Inf 检测、状态限幅、数据新鲜度检查
4. **模块解耦**：应用层代码独立于 CubeMX 生成代码，可单独测试

## 六、扩展点

- **新控制模式**：在 `MotorCmd_t.ctrl_mode` 中新增枚举值，在 Controller_Run 和 MotorTxTask 中增加分支
- **新传感器**：参考 ForceSensorTask 模式：创建新任务 + 消息队列 + 在 ControlTask 中消费
- **在线调参**：扩展 CLI 命令，通过串口读写 CPG 参数
