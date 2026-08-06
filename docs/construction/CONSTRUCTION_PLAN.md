# SmartRollator 分阶段施工计划

每一阶段只在交付物、测试、文档、提交和远端备份均满足后标记完成。阶段编号代表依赖顺序；如需调整，先写决策记录。

## Phase 0 — 施工文档基线

- Goal：冻结产品、架构、施工、测试、Git 和交接规则。
- Included：`AGENTS.md`、文档索引、产品需求、架构、计划、工作流、测试、Git、进度与交接。
- Excluded：Git 初始化、固件迁移、构建或硬件测试。
- Dependencies：现有工程作为只读参考；两个范式仓库。
- Planned Deliverables：本 `docs/` 体系。
- Tests：Markdown 链接、必需章节、状态一致性检查。
- Acceptance Criteria：新 Agent 不看聊天即可说明当前状态和下一阶段动作。
- Rollback Point：文件级备份；Git 回滚点未建立。

## Phase 1 — 正式仓库与最小工程

- Goal：在已建立的正式 Git 仓库中冻结工具链、远端备份和最小可构建 STM32H723 工程。
- Included：仓库治理复核、`.gitignore`、README、CubeMX `.ioc`、Keil 工程、最小安全失能启动。
- Excluded：设备协议、控制算法、电机使能。
- Dependencies：正式仓库路径和 GitHub 远端已确认；需确认实际 CubeMX/MDK/Pack 版本。
- Planned Deliverables：首个可复现构建提交、`backup/bootstrap-*` 远端分支。
- Tests：干净克隆/生成/Keil 编译；记录工具版本和完整结果。
- Acceptance Criteria：远端提交可检出且零警告或警告均有解释；上电无驱动力。
- Rollback Point：远端 bootstrap 基线提交。

## Phase 2 — 平台与外设骨架

- Goal：建立 GPIO、时钟、UART1、UART2/RS485、FDCAN1、DMA/定时器和 RTOS 基础。
- Included：板级映射、初始化、自检日志、回环或最小外设检查。
- Excluded：完整协议和闭环控制。
- Dependencies：Phase 1；真实引脚和时钟确认。
- Tests：编译、串口启动日志、CAN/UART 电气与回环检查。
- Acceptance Criteria：所有外设失败可见；系统保持安全失能。
- Rollback Point：远端 platform-baseline 提交。

## Phase 3 — 数据模型与主机测试框架

- Goal：冻结单位、坐标系、时间戳、有效性和纯 C 测试入口。
- Included：核心结构体、参数边界、测试构建、静态检查基础。
- Excluded：真实设备通信。
- Dependencies：Phase 1。
- Tests：边界值、非法输入、时间回绕/超时和确定性测试。
- Acceptance Criteria：控制域可脱离 HAL 编译和运行测试。
- Rollback Point：远端 contracts-tests 提交。

## Phase 4 — ZLAC8015D / CANopen

- Goal：可靠完成双节点发现、状态读取、失能与受限速度命令。
- Included：FDCAN 适配、CANopen 编解码、ZLAC 状态机、超时和错误。
- Excluded：力控闭环。
- Dependencies：Phase 2、3；节点 ID 和设备手册确认。
- Tests：编解码单测、CAN 分析仪记录、抬轮台架方向/限速/失联测试。
- Acceptance Criteria：所有故障路径进入失能；左右轮方向和单位有证据。
- Rollback Point：远端 motor-canopen 提交。

## Phase 5 — RS485 力传感器

- Goal：稳定输出已校验、已标定、带时间戳的 `ForceSample`。
- Included：收发控制、帧解析、CRC、零点、缩放、超时、错误统计。
- Excluded：电机闭环。
- Dependencies：Phase 2、3；协议和标定信息确认。
- Tests：金样帧、坏帧、分包/粘包、超时、实物静载荷与符号检查。
- Acceptance Criteria：无效数据不进入有效样本；坐标系和单位已冻结。
- Rollback Point：远端 force-sensor 提交。

## Phase 6 — RTOS 数据管线

- Goal：以确定周期连接传感、控制、执行器和日志任务。
- Included：任务、队列/快照、优先级、周期统计、看门狗和背压。
- Excluded：复杂控制策略。
- Dependencies：Phase 3–5。
- Tests：任务频率、队列溢出、CPU/栈余量、输入失联和日志拥塞。
- Acceptance Criteria：关键路径有预算；异常不会阻塞或输出陈旧命令。
- Rollback Point：远端 rtos-pipeline 提交。

## Phase 7 — 固定导纳基线

- Goal：建立可测试、可调参、可安全启停的固定导纳控制基线。
- Included：离散模型、滤波、饱和、参数校验、主机单测和受限台架接入。
- Excluded：Hopf 调制。
- Dependencies：Phase 3、6。
- Tests：零输入、阶跃、符号、饱和、异常浮点、记录回放、抬轮台架。
- Acceptance Criteria：输出可解释、稳定并且安全层保持最终控制权。
- Rollback Point：远端 fixed-admittance 提交。

## Phase 8 — Hopf CPG 模块

- Goal：以独立纯 C 模块实现数值稳定、确定性的 Hopf 振子。
- Included：参数、状态、步进、复位、限幅、离线轨迹和单元测试。
- Excluded：直接电机输出和最终调制策略。
- Dependencies：Phase 3、7。
- Tests：收敛、频率、振幅、步长敏感性、非法参数、长时运行。
- Acceptance Criteria：数学指标达到 `TEST_METRICS.md` 冻结阈值，无 HAL/RTOS 依赖。
- Rollback Point：远端 hopf-module 提交。

## Phase 9 — CPG 变导纳集成

- Goal：在不破坏固定导纳基线的前提下完成可切换调制。
- Included：调制映射、模式切换、参数边界、日志、A/B 回放。
- Excluded：人体试验和自动参数学习。
- Dependencies：Phase 7、8。
- Tests：离线 A/B、切换瞬态、极端输入、失联和受限台架。
- Acceptance Criteria：可随时退回固定导纳；任何模式都不绕过安全仲裁。
- Rollback Point：远端 cpg-integration 提交。

## Phase 10 — 可观测性与视觉输入

- Goal：统一日志、故障、性能指标并按已确认角色接入 ArUco 数据。
- Included：结构化遥测、日志降级、视觉帧验证/超时。
- Excluded：自主导航。
- Dependencies：Phase 6、9；产品负责人确认 ArUco 角色。
- Tests：日志压力、帧损坏、输入超时、指标完整性。
- Acceptance Criteria：日志不阻塞控制；视觉失效不产生危险输出。
- Rollback Point：远端 observability-vision 提交。

## Phase 11 — 系统安全与等价验收

- Goal：在正式仓库完成参考能力等价和故障注入验证。
- Included：完整台架矩阵、通信中断、看门狗、急停、重启、长稳和文档复核。
- Excluded：载人测试和产品认证。
- Dependencies：Phase 1–10。
- Tests：`TEST_METRICS.md` 全部适用项；对参考行为进行可解释比较。
- Acceptance Criteria：所有关键风险有通过证据或明确阻塞；工作树干净并推送。
- Rollback Point：`v0.1.0-parity` 标签及远端 release 分支。

## 文件级实施入口

上述阶段只定义能力与验收边界。每个阶段的微提交顺序、精确文件、主要接口、最近测试和回滚点统一见 [Git 文件级施工树](GIT_CONSTRUCTION_TREE.md)。实现时若文件名或接口需要变化，必须先更新该树和对应 ADR，再编码。
