# SmartRollator 快速上手指南

> 最后更新：2026-07-01

## 前置条件

### 硬件

- STM32H723VGTx 开发板（或 SmartRollator 主板）
- ST-Link V2/V3 调试器
- 六维力传感器（RS485 接口）
- ZLAC8015D 电机驱动 ×2
- 直流无刷电机 ×2（带霍尔/编码器）
- 24V 电源

### 软件

- Keil MDK-ARM 5.38+（当前）
- 或 arm-none-eabi-gcc 工具链（脚手架目标）
- STM32CubeMX 6.x（修改 .ioc 配置用）
- 终端软件（CoolTerm / minicom / screen）

## 1. 连接硬件

```
┌─────────────────────┐
│   STM32H723VGTx     │
│                     │
│  PA9  ─── USART1 TX → ArUco 视觉 RX
│  PA10 ─── USART1 RX ← ArUco 视觉 TX
│                    │
│  PD5  ─── USART2 TX → RS485 模块 DI → 力传感器
│  PD6  ─── USART2 RX ← RS485 模块 RO ← 力传感器
│  PD4  ─── RS485 DE/RE
│                    │
│  PB8  ─── FDCAN1 RX ← 电机驱动器 CANH/CANL
│  PB9  ─── FDCAN1 TX → (经 CAN 收发器)
│                    │
│  PA13 ─── SWDIO  → ST-Link
│  PA14 ─── SWCLK  → ST-Link
└─────────────────────┘
```

## 2. 编译与烧录

### Keil MDK（推荐当前使用）

```bash
# 1. 打开工程
open firmware/MDK-ARM/SmartRollator.uvprojx

# 2. Build (F7)

# 3. 烧录 (F8) 或点击 Load 按钮
```

### arm-none-eabi-gcc（脚手架）

```bash
# 编译
make

# 烧录
make flash

# 调试
make debug
```

## 3. 串口监控

连接 USART1 到电脑 USB-TTL（115200-8-N-1）：

```bash
# macOS
screen /dev/tty.usbmodem* 115200

# 或使用 monitor.py
python tools/monitor.py --port /dev/tty.usbmodem* --baud 115200
```

启动后应看到：
```
[VISION] USART1 receiver ready, waiting for $ARUCO,...*CS#
```

## 4. 基本操作

### 4.1 CLI 命令

在串口终端输入：

```
h  → 帮助
p  → 打印当前力和命令快照
t  → 切换 100ms 周期流开关
```

### 4.2 验证力传感器

1. 确保 RS485 模块供电正常
2. 施加力到传感器把手
3. 输入 `p` 查看 `F[fx=...,fy=...,fz=...]`
4. 如果 fx/fy/fz 全为 0，检查 RS485 接线和 MODBUS 站号

### 4.3 验证电机

1. 给电机驱动器上 24V 电
2. 给传感器把手施加正向力（前推）
3. 电机应正向转动
4. 输入 `p` 查看 `CMD[mode=1,vel=...,tq=...]`

## 5. 修改控制参数

### CPG 参数（文件级）

编辑 `firmware/App/cpg_control.c`：

```c
#define CPG_DEFAULT_ALPHA      10.0f   // 收敛速度 ↑ → 响应更快
#define CPG_DEFAULT_MU         1.0f    // 目标幅值 ↑ → 振荡更强烈
#define CPG_DEFAULT_M          15.0f   // 虚拟质量 ↓ → 更灵敏
#define CPG_DEFAULT_D_BASE     40.0f   // 基础阻尼 ↑ → 需要更大力
#define CPG_DEFAULT_LAMBDA     20.0f   // CPG 调制增益 ↑ → 阻尼变化更剧烈
```

### 轮径校准

编辑 `firmware/App/Controller.c`：

```c
#define WHEEL_RADIUS_M  0.075f   // 按实测轮径修改（单位：米）
```

## 6. 常见问题排查

| 现象 | 检查项 |
|------|--------|
| 串口无输出 | USART1 TX/RX 是否反接？波特率匹配？ |
| 力传感器无数据 | RS485 A/B 线序？PD4 方向控制？MODBUS 站号是否为 1？ |
| 电机不转 | FDCAN1 终端电阻？驱动器是否使能？24V 供电？ |
| CPG 输出始终为 0 | `force->fx` 是否为非零？检查 `p` 命令输出 |
| 电机抖动/异响 | 速度环 PID 参数是否合适？`MOTOR_RPM_LIMIT` 是否过大？ |

## 7. 下一步

- 阅读 `AGENT_CONTEXT.md` 了解完整工程架构
- 阅读 `docs/architecture.md` 了解系统分层设计
- 阅读 `docs/dataflow.md` 了解数据链路
- 阅读 `docs/api.md` 了解接口定义
