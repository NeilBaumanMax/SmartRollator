# 当前项目交接

更新时间：2026-10-17，思路讨论接手。本页保存当前摘要；历史过程保留在 DEV_PROGRESS 与 LOG。

## Current State

正式入口为当前 Git 工作树根目录，开发分支 main。Phase 0 完成；Phase 1 部分完成：P1.01 文档基线、P1.02 工具链检测入口已建立，P1.03 最小生成、P1.05 真实编译仍未完成。现有 STM32 工程是参考基线，不是已验证发布版；05 固定导纳与 06 Hopf 层级仍 Pending。

本轮接手用户新研究方向并保留 Files/start_researching 原始 17 项资料，另存原始提示词、归档说明和研究摘要。新基线为固定 M → PBO 个性化 D0 → 相位窗口局部减阻 → 动捕直接/Hopf 对照；仅研究方向确认，参数与效果待验证。未迁移固件、重做 HTML、使能电机或开展人体实验。

## 每次接续的固定动作

先按 [INDEX](../INDEX.md) 重读必读项，特别是 [WORKFLOW](WORKFLOW.md)、DEV_PROGRESS 最新计划与结论、LOG 最新记录和当前层级。新任务、恢复会话和上下文压缩后均适用；先计划，再实施、自检、修正漂移和交接、精确提交推送。普通 STM32 静态审阅与真实编译/硬件证据分别记录。

## 当前资料与代码事实

- [目录地图](../WORKSPACE_MAP.md)、[Files 全索引](../../Files/ASSET_INDEX.md)覆盖工程、隐藏配置及原始研究资料。当前业务模块在 MDK-ARM，集成在 Core/Src/freertos.c；根 App、Tests、CMakeLists.txt 尚未建立，没有 firmware 目录。
- [当前研究基线](../../Files/research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)保存方向、候选公式、未决事项与逐文件代码核对；[原始资料说明](../../Files/start_researching/README.md)链接导师稿、配图、ZIP 与本次接手提示词。
- 旧 FORCE_COUPLED 笔记仅加状态指向，力主导估相位、差动阻尼、只用客观指标的提议保留为历史/备选；时间戳、实际反馈、速度混杂控制和统一安全出口要求仍有效。
- 两份新增 HTML 的源码仍为 D0−ΔD·cos(φ−φa)，围绕 D0 对称变化，φ 是跨步角度；不是当前 D0−A·w(ψ) 的每步局部窗口。本轮保持原字节，未做浏览器运行验收。
- 本地 CPG_Step 仍为 εFx 耦合和 D_base−lambda·x；频率接口只有声明/定义，未找到调用；只投递 f1，实际手别/方向未定。CAN 接收反馈解析仍 TODO，Controller 忽略 imu/fb；v_cmd 不是 v_meas。
- ControlTask 启动仍调用 Driver_Enable；过期力置零不等于立即失能，MotorTx 缺少完整时效/安全仲裁；这些是待按阶段处理的差距，本轮未修复。
- PBO 指 Preferential Bayesian Optimization，后续于试次间外部选参，初期 A=0；不取消 V1 非人体范围，不代表 MCU 在线学习。决定见 ADR-010，未决问题见 A-011–A-015。
- 全部普通文件备份规则保持，旧产物/IDE/元数据不是验证证据。DOCX 共 7 份未变；ZIP 与解压目录均保留，记录见 [DUPLICATES](../../Files/DUPLICATES.md)。

## Test Baseline

- 历史工具链证据：2026-10-16 `sh tools/check_toolchain.sh` 退出 1，四项 NOT_FOUND / INCOMPLETE；只覆盖 PATH/脚本候选位置，不证明其他主机无安装。本轮未重跑或安装工具链。
- 本轮结构检查：原 ZIP 的 14 个成员与解压目录逐字节相同，5 个 SVG XML 和 5 个 PNG chunk/CRC 通过；原始 17 项未改。新增资料常见凭据模式扫描无命中，不等于形式化安全保证。
- 本轮 257 个 Markdown 本地链接、75 项 Files 索引、原始提示词及 1672 个工程文件哈希核验通过，公式/术语已静态审阅；1795 个普通文件与索引/blob、1794 项清单及 staged diff 复测通过；原始生成器空白以窄范围属性保留，推送以 LOG 与最终 Git 查询核对，不把静态检查当算法/硬件验证。
- 新基线固件生成/编译、正式主机测试、目标板/台架成功证据仍 Not established；本轮这些运行检查均 Not run。

## Git State / 本轮交付核验

- Remote：git@github.com:NeilBaumanMax/SmartRollator.git；当前 main 继续开发，旧 build/phase-1-bootstrap 保留。
- 本轮开工及上一已核实交付：bc08d08ae9040f6e8f6ec1d8f3eb59d7bc0ba697，本地 main、origin/main 和 ls-remote main 一致。
- 已核对远端恢复点：backup/before-session-workflow-20261017 → df3da6870a9eebaaf215ac8cfb832cc98d681567；backup/before-complete-workspace-20261016 → 6a5aaf7087cbc1b2267ba4b048193a0482a4f23c；reference/archive-20260807 → a0669d7e36e54e78970e8162b0bc47334221c6c5。旧分支不删除，不重写历史。
- 开工 1774 tracked、17 untracked、0 ignored；两项既有 Finder 元数据变化按原全量授权纳入本次快照。原资料 17 项共 3447856 字节，连同提示词的 18 项来源/大小/哈希见[导入清单](../verification/workspace-backup/start-researching-20261017.tsv)。
- 本轮最终文档、资料与 [files.sha256](../verification/workspace-backup/files.sha256) 精确暂存后普通推送 main，再独立抓取远端核验；不得预写推送成功。最终查询 `git log -1`、`git status --short --branch`、`git ls-remote origin refs/heads/main`。开工哈希不是最终 HEAD；下一次接续补记实际交付。
- 回滚使用本轮提交的 revert 或从已核实恢复点新建分支；不 reset/clean/强推。

## Next Tasks

1. 恢复时先重读 docs、核对本轮提交及远端状态，写下一增量计划。
2. 最小工程任务：确认 CubeMX 和匹配 FW_H7 的实际路径/版本，并通过现有只读探测入口记录证据，解决 P1.03 生成前置缺口；P1.05 另需 MDK/Arm Compiler 与真实构建。
3. 研究讨论可先细化相位输入/同步记录字段清单：采集与接收时间、脚侧/事件、步/跨步、相位质量、Fx、v_cmd/v_meas、D_raw/D_applied；先作契约草案，不代表采集或人体实验已授权。
4. 后续控制算法实施前冻结新契约与 P9.02 具体映射，保留直接/Hopf 同映射对照及固定导纳回退，不直接迁移旧 x 调阻尼。

## Risks / Completion Label

整体与 Phase 1：Partially complete。本轮完成的是研究接手和资料保全，不是固件阶段推进；轮系、传感器、拓扑、时序和阈值仍需确认。研究直觉、历史演示与旧生成物不能代替构建、实测或算法收益证据。
