# SmartRollator 数据流说明

> 最后更新：2026-07-01

## 一、总览

```
传感器层                    处理层                        控制层                   执行层
┌─────────┐   RS485    ┌──────────────┐   ForceQueue   ┌────────────┐  MotorCmdQueue  ┌──────────┐
│力传感器 │ ────────→ │ForceSensorTask│ ─────────────→ │ControlTask │ ─────────────→ │MotorTxTask│
│(100Hz)  │           │(AboveNormal) │               │(Realtime)  │                │(Realtime1)│
└─────────┘           └──────────────┘               └─────┬──────┘                └────┬─────┘
                                                           │                          │
                                                    ┌──────┴──────┐            ┌──────┴──────┐
                                                    │ Controller  │            │ CANopen PDO │
                                                    │   _Run()    │            │ 0x300+ID    │
                                                    │             │            │ 4B/帧       │
                                                    │ CPG_Step()  │            └──────┬──────┘
                                                    │ → v_cmd m/s │                  │
                                                    │ → rpm 转换  │            ┌──────┴──────┐
                                                    └─────────────┘            │ ZLAC8015D×2│
┌─────────┐   USART1   ┌──────────────┐                                        │ 左轮+rpm   │
│ArUco视觉│ ────────→ │  Log_task    │                                        │ 右轮-rpm   │
│$ARUCO   │  115200   │(BelowNorm7)  │                                        └────────────┘
└─────────┘           │CLI + 100ms日志│
                      └──────────────┘
```

## 二、传感器数据流

### 2.1 力传感器 (RS485 MODBUS RTU, 100Hz)

```
┌─────────────────────────────────────────────────────────────────┐
│                   ForceSensorTask (100Hz)                       │
│                                                                 │
│  1. RS485_SetMode(1)           // 设为发送模式                  │
│  2. HAL_UART_Transmit(查询帧)  // 8 字节: 01 03 04 00 00 00 0C │
│  3. 等待 TC 置位              // 确保发送完成                  │
│  4. RS485_SetMode(0)           // 切换接收模式                  │
│  5. HAL_UART_Receive(29 字节)  // 阻塞接收响应                  │
│  6. processForceData()         // 解析 → ForceData_t f1, f2    │
│  7. osMessageQueuePut(f1)      // 投递到 ForceQueueHandle      │
│  8. Monitor_UpdateForce(&f1)   // 更新全局快照                  │
│  9. osDelay(10)               // 100Hz 周期                    │
└─────────────────────────────────────────────────────────────────┘
```

**查询帧格式**：`01 03 04 00 00 00 0C 44 FF`（站号1，读保持寄存器，起始0x0004，12字节）

**响应帧**：29 字节，`processForceData()` 解析为 fx/fy/fz，torque_x/y 当前为 0。

### 2.2 ArUco 视觉 (USART1, 中断驱动)

```
USART1 RX 中断 (HAL_UART_RxCpltCallback)
  │
  ├── 逐字节接收 → 行缓冲区 log_line_buf
  │
  ├── 检测到 \r/\n → log_line_ready = true
  │
  └── Log_task 消费:
        ├── 首字节为 '$' → parse_aruco_frame()
        │     ├── 校验 XOR checksum
        │     ├── 解析 $ARUCO,seq,status,marker_id,x,y,z,roll,pitch,yaw,dist*CS#
        │     ├── Monitor_UpdateVision(&vision)
        │     └── uart1_printf_ln 打印位姿
        └── 其他 → CLI 命令解析 (h/p/t)
```

## 三、控制数据流 (1kHz)

```
ControlTask (vTaskDelayUntil, period=1ms)
  │
  ├── 1. mq_get_latest(ForceQueueHandle) → last_force
  │      (只取最新一条，丢弃旧数据)
  │
  ├── 2. mq_get_latest(MotorFbQueueHandle) → last_fb
  │      (当前队列可能为空)
  │
  ├── 3. 新鲜度检查
  │      fresh(now, timestamp, 100ms)
  │      过期 → 清零 (memset + timestamp=0)
  │
  ├── 4. Controller_Run(&last_imu, &last_force, &last_fb, &out_cmd)
  │      │
  │      ├── F_ext = force->fx       // 使用 X 向力
  │      │
  │      ├── CPG_Step(&my_cpg, F_ext, 0.001)
  │      │   ├── 输入/状态/步长限幅 (防 NaN/Inf)
  │      │   ├── Hopf 振荡器欧拉积分: x += dx·dt, y += dy·dt
  │      │   ├── 变导纳: D = D_base - λ·x, a_c = (F - D·v_c)/M
  │      │   ├── v_c += a_c·dt → 限幅 [-0.5, 1.2] m/s
  │      │   └── 返回 v_cmd (m/s)
  │      │
  │      ├── rpm = mps_to_rpm(v_cmd)   // 线速度 → 轮速
  │      ├── clamp(rpm, ±400)          // 转速限幅
  │      └── out_cmd.target_vel = rpm
  │
  ├── 5. Monitor_UpdateCmd(&out_cmd)
  │
  ├── 6. osMessageQueuePut(MotorCmdQueueHandle, &out_cmd)
  │
  └── 7. osEventFlagsSet(HeartbeatFlags, HB_CTRL)
```

## 四、执行数据流

```
MotorTxTask (阻塞等待)
  │
  ├── osMessageQueueGet(MotorCmdQueueHandle, &cmd, osWaitForever)
  │
  ├── if (cmd.ctrl_mode == 1)  // 速度模式
  │     left_rpm  = (int16_t)cmd.target_vel
  │     right_rpm = (int16_t)(-cmd.target_vel)   // 右轮反向
  │     ZLAC_SetVelocity_rpm(DRIVE_ID, left_rpm, right_rpm)
  │     │
  │     └── CAN_Send(0x300+ID, 4B, {left_l, left_h, right_l, right_h})
  │         └── FDCAN1, 500kbps → ZLAC8015D 驱动器
```

## 五、监控/日志数据流

```
┌────────────────────────────────────────────────────────┐
│                 monitor_snapshot_t g_mon               │
│                  (互斥量保护)                           │
│                                                        │
│  ForceSensorTask ──→ Monitor_UpdateForce() ──→ g_mon  │
│  ControlTask     ──→ Monitor_UpdateCmd()   ──→ g_mon  │
│  Log_task        ──→ Monitor_UpdateVision()──→ g_mon  │
│                                                        │
│  Log_task (100ms) ──→ Monitor_ReadSnapshot(&snap)      │
│                     ──→ uart1_printf_ln() 打印         │
└────────────────────────────────────────────────────────┘
```

## 六、关键时序

| 事件 | 周期 | 延迟 |
|------|------|------|
| 力传感器采样 | 10ms (100Hz) | ≤10ms |
| 控制循环 | 1ms (1kHz) | ≤1ms（队列已有数据时） |
| 电机指令下发 | 事件驱动（阻塞队列） | 控制输出后 <0.5ms |
| 日志打印 | 100ms | — |
| 视觉帧接收 | 中断驱动 | ≤1ms |
