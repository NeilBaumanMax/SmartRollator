# 给下一个 AI Agent 的启动提示词

> 历史提示词声明（2026-08-07）：本文不再是可执行施工入口。正式仓库现为当前 Git 工作树的仓库根目录；请改读根 `AGENTS.md`、`docs/INDEX.md` 和 `docs/construction/GIT_CONSTRUCTION_TREE.md`，不要执行下文的独立目录初始化指令。

将下面整段内容复制给新的 AI Agent。建议把当前参考工程和新的空目录都放在其可访问的工作区内。

---

你是 SmartRollator 智能助行车固件项目的重建负责人。你的任务不是在旧工程上继续堆改动，而是从零建立一个新的、由 Git 管理、阶段可验证的仓库，并逐步达到旧参考工程当前的功能程度。

## 一、工作目录和保护规则

- 旧参考工程：`/Users/neil/项目/SmartRollator/SmartRollator`
- 建议新仓库：`/Users/neil/项目/SmartRollator/SmartRollator-Rebuild`
- 旧参考工程只能只读检查。不要在旧目录执行 `git init`，不要格式化、移动、覆盖或删除旧文件。
- 所有新代码、构建记录、测试和文档写入新仓库。
- 在执行删除、覆盖 CubeMX 配置、重新生成大量代码或改动硬件输出前，先做只读检查并说明影响。
- 不要一次性复制旧工程形成一个巨大的 initial commit。按阶段重建，每阶段独立提交。

首先完整阅读：

1. `docs/handover/PROJECT_HANDOVER.md`
2. `SmartRollator.ioc`
3. `Core/Src/freertos.c`
4. `MDK-ARM/data_structs.h`
5. `MDK-ARM/torque_sensor.c/.h`
6. `MDK-ARM/user_can.c/.h`
7. `MDK-ARM/Controller.c/.h`
8. `MDK-ARM/cpg_control.c/.h`
9. `project_logic_map.html`
10. `hopf_cpg_interactive_demo.html`

## 二、最终目标

在新 Git 仓库中，从空白 STM32CubeMX 工程逐步实现并验证：

`RS485 力传感器 → 数据新鲜度检查 → Hopf-CPG → 动态阻尼导纳 → m/s 到 rpm → CANopen 双轮电机`

同时实现 USART1 日志、ArUco 位姿帧接收和清晰的扩展接口。当前功能等价完成后打标签 `v0.1.0-parity`。

“功能完成”必须有证据。只有代码存在不能视为完成；必须明确属于以下哪一种状态：

- 仅设计；
- 已实现未编译；
- 已编译；
- 主机测试通过；
- 台架验证通过；
- 实车验证通过。

## 三、当前参考事实

- MCU：STM32H723VGT6，目标 240 MHz，24 MHz HSE。
- RTOS：FreeRTOS + CMSIS-RTOS2。
- USART1：115200，用于日志和 ArUco 文本帧。
- USART2：115200，RS485 力传感器。
- FDCAN1：经典 CAN，参考约 500 kbit/s，用于 ZLAC8015D。
- FDCAN2：已配置但没有明确业务用途。
- 控制周期：1 ms。
- 力传感器任务：参考 10 ms 周期。
- 力数据新鲜度：100 ms。
- 力查询帧：`01 03 04 00 00 0C 44 FF`。
- 响应：29 字节，跳过 3 字节后有两组三轴大端 float；旧代码主要使用第一组 `fx`。
- 电机节点 ID：`0x01`。
- 双轮速度帧：COB-ID `0x300 + node_id`，左/右 int16 小端。
- 当前参考方向：左轮 `+rpm`，右轮 `-rpm`，必须悬空台架确认。
- 轮半径：0.075 m；rpm 限幅：±400。

Hopf-CPG 当前参考参数：

- `alpha = 10`
- `mu = 1`
- `omega = 2π × 0.9 rad/s`
- `epsilon = 0.01`
- `x0 = 0.1, y0 = 0`
- `M = 15`
- `D_base = 40`
- `lambda = 20`
- `D_current` 限制到 15～80
- `v_c` 当前限制到 -0.5～1.2 m/s，但是否允许倒车尚未确认

核心公式：

```text
r² = x² + y²
dx = alpha*(mu-r²)*x - omega*y + epsilon*F_ext
dy = alpha*(mu-r²)*y + omega*x
D_current = clamp(D_base - lambda*x, D_min, D_max)
a = (F_ext - D_current*v) / M
v = clamp(v + a*dt, v_min, v_max)
```

## 四、必须采用的重建顺序

严格按以下顺序推进。除非出现明确阻塞，不要跨阶段实现后面的功能。

1. 冻结参考工程，生成文件清单和 SHA-256。
2. 初始化新 Git 仓库、README、`.gitignore`、工具链和验证模板。
3. 从 CubeMX 生成最小 STM32H723 + FreeRTOS 工程并全量编译。
4. 恢复 USART1、USART2/RS485 和 FDCAN1，分别验证。
5. 建立应用数据类型和模块接口。
6. 重建 CANopen/ZLAC 驱动并完成悬空低速台架测试。
7. 重建力传感器协议，先做主机 fixture 测试，再上板标定。
8. 建立 Force、Control、MotorTx、Log、Safety 任务链。
9. 先实现和验证固定导纳基线。
10. 实现无 HAL 依赖的 Hopf 模块和主机测试。
11. 集成 Hopf 变导纳，并保留固定导纳对照模式。
12. 恢复日志和 ArUco 位姿帧接收。
13. 建立安全状态机，完成当前功能等价验收并打 `v0.1.0-parity`。
14. parity 之后才开发实时步频、视觉安全、电机反馈闭环和人体实验功能。

## 五、Git 工作方式

- `main` 只接收达到阶段验收条件的工作。
- 每个阶段使用独立 `feat/`、`fix/`、`test/` 或 `docs/` 分支。
- 每次提交只表达一个可验证意图。
- CubeMX `.ioc` 和对应生成代码放在同一提交。
- 不提交 AXF、HEX、对象文件、IDE 用户状态和原始受试者数据。
- 推荐 Conventional Commits。
- 提交正文必须写明：目的、关键变更、验证命令/硬件步骤、结果、剩余风险。
- 不要自动 push、开 PR 或发布固件，除非用户明确授权。

每完成一个阶段，向用户报告：

```text
阶段：
状态：设计 / 已实现 / 已编译 / 主机测试 / 台架验证 / 实车验证
修改文件：
验证证据：
Git 提交：
已知问题：
下一步：
需要用户确认：
```

## 六、质量和安全要求

- 最新固件必须全量 Rebuild，目标 0 error；警告必须逐项解释。
- `cpg_control`、力帧解析、视觉帧解析和单位换算必须具备主机测试。
- 协议解析必须检查长度、帧头/功能码、CRC/校验、NaN/Inf 和范围。
- 建立 BOOT、READY、RUN、FAULT、ESTOP 安全状态。
- 力过期、CAN 故障、命令过期或异常数值必须输出零速并记录原因。
- 第一次电机试验必须轮子悬空、低速分级、急停可用、旁边有人监护。
- 未经用户确认，不允许把负速度用于真人实验。
- 视觉信息只通过独立安全层限制速度，不允许直接无约束地修改电机命令。
- 正式日志目标 50～100 Hz，包含力、x/y/r、omega、D、v_cmd、左右轮命令/反馈和安全标志。

## 七、已知旧代码问题，不要原样继承

- 最新源码没有对应的新 Keil 构建产物。
- `NMT_Control` 在头文件和实现中的参数顺序不一致。
- 力响应缺少完整帧和 CRC 校验。
- CAN 心跳过滤/判断逻辑需要依据驱动器手册复核。
- `freertos.c` 混合了过多业务、协议、日志和视觉逻辑。
- 日志与视觉接收使用宏复用缓冲，语义混乱。
- RTOS 路径中存在阻塞式 HAL 调用和 `HAL_Delay`。
- `D_current` 没有作为可记录调试状态输出。
- IMU、CAN worker、watchdog 和电机反馈链路尚未完成。
- 视觉与步频更新尚未进入控制闭环。

先达到“行为等价 + 可验证”，再用独立提交改进架构。不要把大规模重构、算法变化和硬件接入混在一个提交中。

## 八、立即开始时的动作

现在先做只读检查，不改任何旧文件：

1. 确认旧目录不是 Git 仓库，并列出关键文件及时间。
2. 阅读交接文档和关键源码，输出你理解的当前架构。
3. 检查建议新目录是否存在、是否为空、是否已经是 Git 仓库。
4. 给出阶段 0～2 的具体执行计划和将要产生的提交。
5. 如果新目录为空且没有风险，初始化 Git，创建仓库骨架和第一个提交。
6. 在需要使用 CubeMX/Keil GUI、连接硬件或进行电机动作时停止，明确告诉用户要操作什么以及安全条件。

保持持续推进，但不要虚构构建或硬件验证结果。无法在当前环境运行 Keil/CubeMX 时，应生成明确的用户执行步骤和验证记录模板，并将状态标记为“待用户执行”，不能标记为完成。

---

提示词结束。
