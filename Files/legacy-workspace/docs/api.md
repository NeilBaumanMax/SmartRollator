# SmartRollator API / 接口文档

> 最后更新：2026-07-01

## 一、外部通信接口

### 1.1 USART1 — CLI / ArUco 视觉 (115200-8-N-1)

#### CLI 命令

| 命令 | 功能 | 响应示例 |
|------|------|---------|
| `h` | 帮助 | 列出所有命令 |
| `p` | 单次快照 | `F: fx=123 fy=45 fz=12 ts=12345 ms` |
| `t` | 切换周期流 | `[LOG] stream ON` |

#### ArUco 视觉帧格式

```
$ARUCO,seq,status,marker_id,x_mm,y_mm,z_mm,roll_x10,pitch_x10,yaw_x10,dist_mm*CS#
```

| 字段 | 类型 | 说明 |
|------|------|------|
| seq | uint16 | 帧序号 |
| status | uint8 | 状态码 |
| marker_id | uint16 | ArUco 标记 ID |
| x_mm, y_mm, z_mm | int16 | 位置 (mm) |
| roll_x10, pitch_x10, yaw_x10 | int16 | 姿态角 ×10 |
| dist_mm | int16 | 距离 (mm) |
| CS | hex | XOR 校验和 |

### 1.2 USART2 — 力传感器 MODBUS RTU (115200-8-N-1)

#### 查询帧（MCU → 传感器）

```
01 03 04 00 00 00 0C 44 FF
```

| 字节 | 含义 |
|------|------|
| 01 | 站号 |
| 03 | 功能码（读保持寄存器） |
| 04 00 | 起始地址 |
| 00 0C | 寄存器数量（12 字节） |
| 44 FF | CRC-16 |

#### 响应帧（传感器 → MCU）

29 字节，由 `processForceData()` 解析为 `ForceData_t`:

```c
typedef struct {
    float fx;        // X 向力 (N) — 控制输入
    float fy;        // Y 向力 (N)
    float fz;        // Z 向力 (N)
    float torque_x;  // X 轴力矩 (Nm)
    float torque_y;  // Y 轴力矩 (Nm)
    uint32_t timestamp; // HAL_GetTick()
} ForceData_t;
```

### 1.3 FDCAN1 — 电机控制 (500kbps)

#### CANopen PDO 速度指令 (RPDO1)

| CAN ID | DLC | 数据 |
|--------|-----|------|
| 0x300 + ID | 4 | left_rpm(L,H), right_rpm(L,H) |

由 `ZLAC_SetVelocity_rpm()` 函数发送。

#### CANopen 初始化序列

```
1. CAN_Init()                    // FDCAN 过滤器配置
2. RPDO1_Config(DRIVE_ID)       // 配置 RPDO1 映射
3. Profile_Velocity_Init(ID, accset) // 速度模式初始化
4. NMT_Control(0x01, ID)        // NMT Start
5. Driver_Enable(ID)            // 使能驱动器
```

---

## 二、内部 API

### 2.1 Controller_Run

```c
void Controller_Run(
    const IMUData_t   *imu,      // IMU 数据（当前未用）
    const ForceData_t *force,    // 力传感器数据
    const MotorFb_t   *fb,       // 电机反馈（当前未用）
    MotorCmd_t        *out_cmd   // 输出命令
);
```

**职责**：将 `force->fx` 输入 CPG 控制器，输出换算后的电机转速命令。

**调用周期**：1kHz (ControlTask)

**输出**：`out_cmd->target_vel` (rpm), `out_cmd->ctrl_mode=1`

### 2.2 CPG_Init / CPG_Step / CPG_Update_Frequency

```c
void CPG_Init(CPG_Handle_t *hcpg);

float CPG_Step(CPG_Handle_t *hcpg, float F_ext, float dt);
// 返回: 车体目标线速度 v_cmd (m/s), 范围 [-0.5, 1.2]

void CPG_Update_Frequency(CPG_Handle_t *hcpg, float freq_hz);
// 可运行时调整 CPG 振荡频率, 有效范围 [0.3, 2.0] Hz
```

### 2.3 CPG_Handle_t 参数结构

```c
typedef struct {
    float alpha;    // 收敛速度, 默认 10.0
    float mu;       // 目标幅值平方, 默认 1.0
    float omega;    // 角频率 (rad/s), 默认 2π×0.9
    float epsilon;  // 外力耦合系数, 默认 0.01
    float x, y;     // Hopf 振荡器状态
    float M;        // 虚拟质量, 默认 15.0
    float D_base;   // 基础阻尼, 默认 40.0
    float lambda;   // CPG→阻尼增益, 默认 20.0
    float v_c;      // 导纳输出速度 (m/s)
} CPG_Handle_t;
```

### 2.4 力传感器 API

```c
int processForceData(
    const uint8_t *hexData,      // MODBUS 响应帧
    size_t         dataLength,   // 帧长度 (29)
    ForceData_t   *force1,       // [输出] 把手1
    ForceData_t   *force2        // [输出] 把手2
);
// 返回: 0=成功

void RS485_SetMode(uint8_t mode);
// mode=1: 发送 (DE=1, RE=0)
// mode=0: 接收 (DE=0, RE=1)
```

### 2.5 CAN API

```c
void CAN_Init(void);
uint8_t RPDO1_Config(uint8_t ID);    // 配置速度指令 RPDO
uint8_t NMT_Control(uint8_t ID, uint8_t Data0); // NMT 状态控制
uint8_t Driver_Enable(uint8_t ID);   // 驱动器使能
uint8_t Profile_Velocity_Init(uint8_t ID, uint8_t AccSet);
uint8_t CAN_Send(uint32_t ID, uint32_t Data_Size, uint8_t* Data);

// inline 辅助
static inline uint8_t ZLAC_SetVelocity_rpm(uint8_t id, int16_t left_rpm, int16_t right_rpm);
```

---

## 三、数据结构（完整定义见 `data_structs.h`）

| 结构体 | 用途 | 大小 |
|--------|------|------|
| ForceData_t | 力传感器数据 | 24B |
| IMUData_t | IMU 姿态数据（预留） | 52B |
| MotorFb_t | 电机反馈 | 20B |
| MotorCmd_t | 电机控制命令 | 24B |
| LogMsg_t | 日志消息 | 136B |
| VisionPose_t | ArUco 视觉位姿 | 28B |
| monitor_snapshot_t | 监控快照 | 80B |
