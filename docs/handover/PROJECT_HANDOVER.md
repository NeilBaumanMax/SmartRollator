# SmartRollator 项目交接与 Git 重建指南

文档状态：可执行交接基线  
整理日期：2026-08-06  
当前参考工程：`/Users/neil/项目/SmartRollator/SmartRollator`  
建议新仓库：`/Users/neil/项目/SmartRollator/SmartRollator-Rebuild`

> 核心原则：当前目录只作为只读参考。新工程从 STM32CubeMX 空白工程开始，一阶段一提交地重建；每个提交都必须能说明目的、变更、验证结果和已知限制。不要在旧目录直接执行 `git init`，也不要一次性复制全部旧代码后提交。

## 1. 交接目标

本次交接不是简单地把现有文件放进 Git，而是建立一条可追溯、可编译、可测试、可回退的开发历史，使新仓库能够清楚回答以下问题：

- 最初的硬件和 RTOS 工程是如何建立的；
- 力传感器、CANopen 电机、控制器、日志和视觉通路分别何时接入；
- 固定导纳如何演进为 Hopf-CPG 变导纳；
- 每一个阶段是否通过编译、主机测试、台架测试或实车测试；
- 哪些参数是当前参考值，哪些参数已经经过实测；
- 哪些功能只是源码存在，尚未形成运行闭环。

最终“当前功能等价”定义为：

`RS485 把手力 → 数据新鲜度检查 → Hopf-CPG → 动态阻尼导纳 → 线速度/rpm 换算 → CANopen 双轮速度命令`

同时保留 USART1 日志与 ArUco 位姿帧接收能力，并为 IMU、电机反馈、步频更新和视觉安全预留清晰接口。

## 2. 当前项目的证据边界

当前目录没有 `.git`，因此不能恢复逐次修改历史。可用证据包括文件时间、Keil 构建日志、CubeMX 配置、源码和技术文档。

- 最早可核验的旧工程记录：Keil 日志内的 Project File Date 为 2025-10-15。
- 当前工作区中的集中开发时间：2026-05-03 至 2026-05-17。
- 技术文档最后修改时间：2026-06-04。
- 最新 AXF/HEX 与对象文件停留在旧构建，未包含新加入的 `cpg_control.c` 对象文件。
- 因此，当前源码只能称为“设计和实现参考”，不能称为“已验证固件发布版”。

历史说明中的日期是阶段性重建，不代表精确提交时间。新仓库应从现在开始形成真实、可验证的提交历史。

## 3. 当前系统概览

### 3.1 硬件与工具链

| 项目 | 当前参考值 | 重建要求 |
|---|---|---|
| MCU | STM32H723VGT6，Cortex-M7 | 从相同器件建立 CubeMX 工程 |
| CPU 时钟 | 240 MHz | 以 `.ioc` 和生成代码复核 |
| 外部晶振 | 24 MHz HSE | 上板前核对实际硬件 |
| 固件包 | STM32Cube FW_H7 V1.12.1 | 在 README 记录实际安装版本 |
| IDE 目标 | MDK-ARM V5.32 | 当前旧日志实际来自 µVision 5.24.2 |
| 编译器 | 旧日志为 ARMCC 5.06u5 | 新仓库固定实际可用编译器版本 |
| RTOS | FreeRTOS + CMSIS-RTOS2 | 生成后先完成空任务编译 |
| 电机驱动 | ZLAC8015D 双轮驱动 | 使用经典 CAN/CANopen |
| 力传感器 | USART2 + RS485 | 115200，29 字节响应帧 |
| 监控/视觉 | USART1 | 115200，日志与 ArUco 文本帧 |

### 3.2 外设配置

- FDCAN1：当前用于 ZLAC8015D，经典 CAN，参考标称速率约 500 kbit/s。
- FDCAN2：已生成但业务上基本未使用；重建时不要默认启用，先明确用途。
- USART1：115200、8N1，用于日志命令和视觉数据。
- USART2：115200、8N1，RS485 模式，PD4 为 DE，PD5/PD6 为 TX/RX。
- 调试：PA13/PA14，SWD。

### 3.3 软件数据流

```text
ForceSensorTask (约 100 Hz)
  └─ RS485 查询与 29 字节解析
       └─ ForceQueue
            └─ ControlTask (1 ms)
                 ├─ 数据新鲜度检查（100 ms）
                 ├─ Controller_Run
                 │    └─ CPG_Step
                 │         ├─ 更新 Hopf x/y
                 │         ├─ D_current = D_base - lambda*x
                 │         └─ M*a + D_current*v = F_ext
                 └─ MotorCmdQueue
                      └─ MotorTxTask
                           └─ 0x300 + node_id 双轮 rpm
```

USART1 接收的 ArUco 位姿当前只进入监控快照，没有参与速度限制或停车。IMU 和电机反馈结构存在，但控制器当前不使用它们。

## 4. 当前参考实现

### 4.1 主要文件职责

| 文件 | 当前职责 | 重建建议 |
|---|---|---|
| `SmartRollator.ioc` | MCU、时钟、FDCAN、USART、FreeRTOS 配置 | 作为硬件配置事实来源，但重新生成 |
| `Core/Src/freertos.c` | RTOS 对象、采集、控制、日志、视觉解析、电机发送 | 重建时拆分应用模块，避免继续膨胀 |
| `MDK-ARM/data_structs.h` | 力、IMU、电机、视觉、监控数据结构 | 移到 `App/Inc/app_types.h` 或同类目录 |
| `MDK-ARM/torque_sensor.c` | 29 字节力数据解析、RS485 方向控制 | 协议解析与 HAL 收发分离，便于主机测试 |
| `MDK-ARM/user_can.c` | FDCAN 发送、CANopen PDO/SDO、NMT、驱动使能 | 分成 CAN 传输层和 ZLAC 驱动层 |
| `MDK-ARM/Controller.c` | 力输入、CPG 调用、m/s 到 rpm 换算 | 参数集中配置，避免散落宏定义 |
| `MDK-ARM/cpg_control.c` | Hopf-CPG、动态阻尼、导纳积分和保护 | 保持纯 C 模块，不依赖 HAL，建立单元测试 |
| `project_logic_map.html` | 早期工程逻辑说明 | 新仓库使用 README/架构文档替代 |
| `hopf_cpg_interactive_demo.html` | 参数和控制链路交互演示 | 作为算法解释资产保留 |

### 4.2 当前 Hopf-CPG 参数

| 参数 | 当前值 | 含义 |
|---|---:|---|
| `alpha` | 10.0 | 极限环径向收敛速度 |
| `mu` | 1.0 | 目标半径平方，稳定半径为 1 |
| `omega` | `2π × 0.9` rad/s | 默认步频 0.9 Hz |
| `epsilon` | 0.01 | 外力到 Hopf x 通道的耦合增益 |
| `x0, y0` | 0.1, 0.0 | 初始状态 |
| `M` | 15.0 | 导纳虚拟质量 |
| `D_base` | 40.0 | 基础阻尼 |
| `lambda` | 20.0 | x 到阻尼的调制增益 |
| `D` 限幅 | 15 到 80 | 动态阻尼安全范围 |
| `v_c` 限幅 | -0.5 到 1.2 m/s | 当前速度范围，是否允许倒车待确认 |
| 外力限幅 | ±500 N | 输入保护 |
| 状态限幅 | ±5 | x/y 保护 |
| 导数限幅 | ±200 | 状态和速度积分保护 |
| 控制步长 | 0.001 s | 目标控制周期 1 kHz |

控制器参考值：轮半径 0.075 m，电机目标限幅 ±400 rpm。右轮命令取反，用于补偿物理安装方向；必须通过悬空台架验证，不能仅凭源码认定方向正确。

### 4.3 力传感器协议

- 查询帧：`01 03 04 00 00 0C 44 FF`。
- 响应长度：29 字节。
- 当前解析：跳过前 3 字节，按大端依次解析 6 个 float，分别得到两组 XYZ。
- 当前控制只投递并使用第一组 `f1.fx`。
- 当前代码没有完整验证响应帧头、功能码、字节数和 CRC；新仓库应增加协议验证及错误计数。

### 4.4 电机命令协议

- 节点 ID 当前为 `0x01`。
- 速度 PDO COB-ID：`0x300 + node_id`。
- 4 字节数据：左轮 int16 小端、右轮 int16 小端。
- 当前前进命令：`left = wheel_rpm`，`right = -wheel_rpm`。
- 初始化顺序：CAN 启动、RPDO1 配置、Profile Velocity、NMT、Driver Enable。

## 5. 当前完成度

### 5.1 已在源码中实现

- STM32H723 时钟和外设初始化；
- FreeRTOS 任务、队列、互斥锁和事件标志；
- RS485 力传感器轮询和浮点解析；
- CANopen 基本配置和双轮速度下发；
- 1 ms 控制任务和 100 ms 数据新鲜度判断；
- 单 Hopf 振荡器及外力耦合；
- 动态阻尼导纳控制；
- m/s 到 rpm 换算与命令限幅；
- USART1 周期日志与简单命令；
- `$ARUCO,...*CS#` 视觉帧解析和异或校验；
- 参数说明、演示网页、组会材料和实验设计。

### 5.2 尚未形成闭环或未验证

- 最新源码没有新的 Keil 构建产物；
- 没有可核验的最新固件上板记录；
- `CPG_Update_Frequency()` 已存在，但没有由实时步频调用；
- 视觉位姿没有参与限速或停车；
- 电机反馈帧没有进入控制闭环；
- IMU 任务、CAN worker 和 watchdog 主要为占位；
- 正式实验日志缺少 x、y、半径、omega、D_current、v_cmd、左右轮反馈和保护标志；
- 当前日志约 10 Hz，不足以支持 jerk 等动态指标；
- 力方向 `fx`、轮径、rpm 符号和倒车需求未完成实车确认；
- 尚无 E0～E5 实验结果和统计输出。

## 6. 已知问题与重建时处理原则

### 6.1 必须在“功能等价”前解决

| 优先级 | 问题 | 处理要求 |
|---|---|---|
| P0 | 最新源码未重新编译 | 每一阶段保存构建日志；最终必须全量 Rebuild 0 error |
| P0 | `NMT_Control` 头文件与实现参数顺序不一致 | 统一签名并增加最小协议测试 |
| P0 | 电机方向和速度范围未经台架验证 | 首次使能采用低速、轮子悬空、急停可用 |
| P0 | 失效时没有统一安全状态机 | 定义 BOOT/READY/RUN/FAULT/ESTOP，故障输出零速 |
| P0 | 力帧缺少完整性和 CRC 校验 | 拒绝错误帧并记录 timeout/crc/frame_error |
| P1 | FDCAN 心跳判断和过滤逻辑可疑 | 根据驱动器手册重写并用抓包验证 |
| P1 | `freertos.c` 混合日志、视觉、协议和业务 | 拆分模块，生成区只保留任务入口 |
| P1 | `HAL_Delay` 用于 RTOS 运行路径 | 初始化阶段可控使用；周期任务改为 RTOS 延时/状态机 |
| P1 | `D_current` 是局部变量，无法记录 | 增加 debug 状态/getter，不改变控制数学 |
| P1 | 视觉与日志复用缓冲宏，语义混乱 | 独立 ring buffer 或协议接收模块 |

### 6.2 达到当前功能等价后再改进

- 将 CPG 参数存入单一配置结构，并记录参数版本；
- 增加电机反馈闭环、左右轮同步误差和故障码；
- 接入外部步频并进行限速、限变化率和超时保护；
- 视觉测距只通过独立安全层影响速度上限，不直接修改 CPG 状态；
- 将实验日志提升到 50～100 Hz，优先使用 CSV 或二进制帧；
- 增加离线回放和实验数据脚本；
- 增加真实硬件 watchdog 和可验证急停路径。

## 7. 新 Git 仓库方案

### 7.1 仓库建立原则

建议在全新目录建立仓库，旧目录保持只读：

```bash
mkdir -p /Users/neil/项目/SmartRollator/SmartRollator-Rebuild
cd /Users/neil/项目/SmartRollator/SmartRollator-Rebuild
git init -b main
```

首个提交只包含仓库骨架、README、工具链说明、`.gitignore` 和重建计划。不要把当前工程整棵复制为“initial commit”。

### 7.2 建议目录结构

```text
SmartRollator-Rebuild/
├── App/
│   ├── Inc/
│   └── Src/
├── Core/                 # CubeMX 生成
├── Drivers/              # CubeMX/HAL
├── Middlewares/          # FreeRTOS
├── MDK-ARM/              # uvprojx、启动文件、链接配置
├── Tests/
│   ├── host/
│   └── fixtures/
├── docs/
│   ├── architecture/
│   ├── protocols/
│   ├── verification/
│   └── experiments/
├── tools/
├── SmartRollator.ioc
├── README.md
├── CHANGELOG.md
└── .gitignore
```

建议把手写业务代码放在 `App/`，减少 CubeMX 再生成对业务文件的影响。生成文件仍纳入 Git，以便在没有完全相同 CubeMX 环境时也能构建。

### 7.3 `.gitignore` 基线

```gitignore
.DS_Store
*.uvguix.*
*.uvoptx
.vscode/*.log
.vscode/*.lock

MDK-ARM/SmartRollator/
MDK-ARM/Objects/
MDK-ARM/Listings/
*.o
*.d
*.crf
*.axf
*.hex
*.map
*.lst
*.build_log.htm

generated_docs/render_*/
generated_docs/ql_*/
__pycache__/
*.pyc
```

是否跟踪 `.docx` 和大尺寸 PNG 需在仓库开始时确定。推荐以 Markdown 和源码为权威；需要保留的二进制报告使用 Git LFS，并在 README 写清生成方式。

### 7.4 分支和提交规则

- `main`：只保留已达到阶段验收条件的提交。
- 功能分支：`feat/<scope>`，例如 `feat/force-sensor`。
- 修复分支：`fix/<scope>`。
- 文档分支：`docs/<scope>`。
- 一次提交只表达一个可验证意图；生成代码与对应 `.ioc` 修改放在同一提交。
- 不提交 IDE 用户状态、编译产物、串口原始隐私数据或实验受试者信息。

推荐 Conventional Commits：

```text
chore(repo): initialize rebuild workspace
feat(platform): generate STM32H723 FreeRTOS base
feat(can): add classic CAN transport for ZLAC8015D
feat(force): add validated RS485 force-frame parser
feat(control): add fixed admittance controller
feat(cpg): add host-tested Hopf oscillator
feat(control): integrate Hopf variable admittance
test(bench): verify wheel direction and rpm limits
docs(experiment): add E0-E5 validation protocol
```

每个提交消息正文至少记录：为什么改、如何验证、硬件条件、剩余风险。

## 8. 从零重建路线图

### 阶段 0：冻结参考工程

目标：保证旧工程不被重建过程污染。

- 记录旧目录绝对路径；
- 生成文件清单和 SHA-256 校验清单；
- 保存关键文档、参数表和实验方案；
- 将旧工程标记为只读参考，不执行格式化或批量替换。

验收：有 `reference_manifest.sha256`，能证明参考文件未被误改。

### 阶段 1：建立 Git 和工程规范

目标：新仓库可以解释如何构建，但还没有固件功能。

- 初始化 Git；
- 添加 README、目录结构、工具链版本、提交规范；
- 添加 `.gitignore`；
- 添加 `docs/rebuild-plan.md`；
- 建立构建日志模板和硬件验证记录模板。

验收：工作区干净，初始提交不包含旧工程的大包复制。

### 阶段 2：生成最小 STM32H723 工程

目标：得到可编译、可下载、可进入 RTOS 调度器的最小固件。

- 新建 STM32H723VGT6 CubeMX 工程；
- 配置 24 MHz HSE 和 240 MHz 系统时钟；
- 配置 SWD；
- 添加 FreeRTOS/CMSIS-RTOS2；
- 先只保留一个健康检查任务；
- 生成 MDK-ARM 工程并全量编译。

验收：0 error；上板运行；调试器能暂停在任务中；保存构建日志。

### 阶段 3：恢复串口和 FDCAN 外设

目标：外设初始化可独立验证。

- USART1 115200 日志；
- USART2 115200 RS485；
- FDCAN1 经典 CAN 约 500 kbit/s；
- FDCAN2 暂不进入业务，除非硬件用途已确认；
- 输出启动版本、Git commit ID 和复位原因。

验收：串口输出稳定；CAN 分析仪确认位时序；无周期业务逻辑。

### 阶段 4：建立应用数据模型和模块边界

目标：先定义接口，再接硬件。

- 建立 `ForceData`、`MotorCommand`、`MotorFeedback`、`VisionPose`、`ControlDebug`；
- 统一时间戳单位和有效性语义；
- 定义协议层、设备层、控制层和任务层接口；
- 对结构体大小和字段范围做静态断言或测试。

验收：纯头文件和最小实现可编译；接口文档完成。

### 阶段 5：重建 CANopen/ZLAC8015D 驱动

目标：在没有控制算法的情况下可靠使能和驱动双轮。

- 实现经典 CAN 发送；
- 实现 NMT、SDO/PDO 配置和使能序列；
- 统一 `NMT_Control(command, node_id)` 参数顺序；
- 实现双轮 int16 小端速度帧；
- 解析至少心跳、速度反馈和故障码；
- 增加 timeout、bus-off、send-fail 计数。

验收：轮子悬空；±20 rpm、±50 rpm 分级测试；急停可用；方向记录入文档。

### 阶段 6：重建 RS485 力传感器

目标：得到可信、可诊断的力数据。

- 协议解析与 UART 收发分离；
- 验证地址、功能码、字节数、长度和 CRC；
- 解析两组三轴大端 float；
- 处理 NaN、Inf、越界、超时和短帧；
- 记录采样率、错误率和最后有效时间；
- 明确使用哪只把手、哪个轴和正方向。

验收：主机测试使用固定帧 fixture；上板静态零点稳定；5/10/20 N 标定记录完成。

### 阶段 7：建立 RTOS 数据通路

目标：形成传感、控制、发送、日志的确定性任务链。

- ForceSensorTask：建议 100 Hz；
- ControlTask：1 kHz；
- MotorTxTask：消费最新命令，丢弃过期命令；
- LogTask：低优先级，不能阻塞控制；
- Safety/Watchdog：监控传感器、控制和 CAN 心跳；
- 队列采用“取最新值”策略，并统计覆盖/丢弃。

验收：测量任务周期、抖动、栈余量和 CPU 占用；控制日志不得阻塞实时任务。

### 阶段 8：实现固定导纳基线

目标：在引入 CPG 前先证明力到速度链路正确。

```text
M * dv/dt + D * v = F_ext
v[k+1] = clamp(v[k] + ((F_ext - D*v[k]) / M) * dt)
```

- 使用固定 `M`、固定 `D`；
- 完成零力回零、速度限幅和命令 slew-rate 限制；
- 完成 m/s 到 rpm 换算；
- 在台架上验证推力方向和双轮同步。

验收：阶跃力离线曲线正确；台架输出方向正确；传感器超时后在限定时间内零速。

### 阶段 9：实现纯 Hopf 模块和主机测试

目标：先验证振荡器本身，不连接电机。

- `cpg_control.c` 不依赖 HAL/RTOS；
- 验证无输入时半径收敛到 `sqrt(mu)`；
- 验证频率接近 `omega / 2π`；
- 验证不同 dt 下的数值稳定性；
- 验证 NaN、Inf、异常参数和大力输入不会发散；
- 输出 x-y 相图、r(t) 和频率误差。

验收：主机测试自动通过；E0 报告进入 Git。

### 阶段 10：集成 Hopf-CPG 变导纳

目标：达到当前核心算法功能等价。

- 外力耦合到 Hopf x 通道；
- 使用 `D_current = D_base - lambda*x`；
- 限制 D 到 15～80；
- 导纳输出限制到确认后的速度范围；
- 输出完整 `ControlDebug`；
- 保留固定导纳作为可切换对照条件。

验收：离线回放可比较固定导纳和变导纳；全量编译 0 error；台架低速运行稳定。

### 阶段 11：恢复日志和视觉接收

目标：达到当前辅助功能等价，同时满足实验记录需求。

- USART1 命令和日志与视觉协议分离；
- 解析 `$ARUCO,...*CS#`；
- 日志至少包含时间、trial、condition、力、x/y/r、omega、D、v_cmd、左右轮命令/反馈和保护标志；
- 日志频率 50～100 Hz，控制任务仅写入缓冲；
- 视觉数据超时不会影响基本控制。

验收：协议单元测试通过；连续运行无缓冲溢出；日志可被脚本解析。

### 阶段 12：安全状态机和当前功能验收

目标：从“能运行”升级为“可安全实验”。

- 定义 BOOT、READY、RUN、FAULT、ESTOP；
- 力传感器过期、CAN 故障、命令过期或异常数值进入安全零速；
- 所有限幅和故障都有事件记录；
- 轮子悬空完成 E1；
- 完成 E2 标定；
- 形成当前功能等价标签 `v0.1.0-parity`。

验收：测试清单全部签字或记录；标签对应的构建产物可重现。

### 阶段 13：当前程度之后的扩展

- 实时步频调用 `CPG_Update_Frequency()`；
- 视觉测距安全限速/停车；
- 电机反馈闭环；
- IMU 接入；
- E3～E5 人体实验和统计分析。

这些功能不得混入“当前功能等价”提交，应在 parity 标签之后独立开发。

## 9. 验证矩阵

| 层级 | 验证方法 | 必须保存的证据 |
|---|---|---|
| 静态 | 编译、警告检查、接口审查 | 构建日志、编译器版本 |
| 主机单测 | CPG、力帧、视觉帧、单位换算 | 测试输出、fixture |
| 离线仿真 | 极限环、导纳阶跃、限幅 | CSV、图、参数快照 |
| 台架 | CAN、方向、rpm、超时、急停 | 测试表、视频/日志编号 |
| 标定 | 力零偏、线性、重复性 | 标定数据和系数 |
| 集成 | 任务周期、日志、故障注入 | trace、统计和故障记录 |
| 人体实验 | E3～E5 | 伦理/同意、匿名数据、分析脚本 |

任何“已完成”都必须指向至少一种证据。只有源码存在不能算硬件功能完成。

## 10. 版本与发布策略

- `v0.0.x`：平台、驱动、传感器和测试基础。
- `v0.1.0-parity`：达到当前参考工程的核心功能等价并完成台架验证。
- `v0.2.0`：实时步频接入。
- `v0.3.0`：视觉安全层和电机反馈闭环。
- `v0.4.0`：预实验版本。
- `v1.0.0`：论文正式实验使用的冻结固件。

每个标签附带：commit SHA、CubeMX/Keil/编译器版本、参数 JSON、硬件版本、构建日志和验证报告。

## 11. 第一周建议执行顺序

1. 冻结旧工程并生成 SHA-256 清单。
2. 创建新仓库和初始规范提交。
3. 从 CubeMX 建立最小 STM32H723 + FreeRTOS 工程。
4. 完成 USART1 启动日志和 Git commit ID 输出。
5. 恢复 FDCAN1，并用分析仪验证 500 kbit/s。
6. 在独立分支重建 ZLAC 驱动，完成悬空低速测试。
7. 在独立分支重建力帧解析，先做主机 fixture 测试。
8. 合并后再建立固定导纳基线，不要直接跳到 Hopf。

## 12. 交接验收清单

- [ ] 旧目录保持不变，并有校验清单。
- [ ] 新仓库 `main` 已初始化且工作区干净。
- [ ] README 记录硬件和工具链版本。
- [ ] `.ioc` 与生成代码在同一提交中可重现。
- [ ] 每个阶段有独立提交和验证记录。
- [ ] 最新源码全量编译 0 error。
- [ ] CPG、力帧和视觉帧有主机测试。
- [ ] 电机低速、方向、急停和超时已做台架验证。
- [ ] 当前参数进入版本化配置文件。
- [ ] 正式实验日志字段完整且频率足够。
- [ ] `v0.1.0-parity` 标签可重现构建。

## 13. 关键参考资料

- `SmartRollator.ioc`：当前 CubeMX 配置参考。
- `Core/Src/freertos.c`：当前运行链路参考，不建议原样复用架构。
- `MDK-ARM/cpg_control.c/.h`：Hopf-CPG 数学实现参考。
- `MDK-ARM/Controller.c/.h`：线速度和 rpm 映射参考。
- `MDK-ARM/torque_sensor.c/.h`：力帧格式参考。
- `MDK-ARM/user_can.c/.h`：ZLAC CANopen 命令参考，需复核手册。
- `hopf_cpg_interactive_demo.html`：算法行为和参数演示。
- `Hopf振荡器组会汇报讲解稿.docx`：研究动机、当前成果和问答。
- `/Users/neil/Documents/智能助行器Hopf-CPG变导纳控制实验设计与数据处理方案.docx`：E0～E5 实验设计。

## 14. 最终交接结论

当前项目的价值不只是几份源码，而是已经明确了一条研究路线：以力输入建立安全的导纳运动基线，再用 Hopf 极限环提供稳定节律，通过动态阻尼让助行车响应具有步态适应性。重新做项目时，应保留这条研究主线，但必须用 Git、自动测试、构建证据和台架验证把每一步重新做实。

优先目标不是尽快复制到“看起来一样”，而是让 `v0.1.0-parity` 的每个功能都有可追溯证据，并且任何人都能从仓库说明重现构建和验证过程。
