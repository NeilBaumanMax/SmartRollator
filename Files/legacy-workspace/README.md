# 另一工作区补入的参考资料

导入日期：2026-10-16。来源：本机另一份 SmartRollator 工作区（外层开发资料与内层同名 STM32 工程的布局）。来源文件只读保留，本目录保存独有参考资产，不替代 [正式施工文档](../../docs/INDEX.md)。

原始 23 个文件全部按字节复制，来源相对路径、目标路径和 SHA-256 见 [IMPORT_MANIFEST.tsv](IMPORT_MANIFEST.tsv)。其中两份手册在 `Files/hardware/`，一份 Word 文档在 `Files/`；其余 20 份保存在本目录。

## 文档与图示

- [历史上下文 AGENT_CONTEXT.md](AGENT_CONTEXT.md)：旧实现的数据流、参数、任务和接口说明。只作历史参考，不是本仓库的 Agent 指令或当前验收结论。
- [架构说明](docs/architecture.md)、[数据流](docs/dataflow.md)、[接口说明](docs/api.md)：帮助理解现有参考代码。
- [旧快速上手指南](docs/getting-started.md)：保留操作思路，其构建路径尚未适配。
- [系统架构图](docs/archive/SmartRollator_图1_系统架构总览.svg)、[控制算法图](docs/archive/SmartRollator_图2_ControlTask算法.svg)、[辅助数据流图](docs/archive/SmartRollator_图3_辅助数据流.svg)、[安全保护图](docs/archive/SmartRollator_图4_安全保护.svg)、[数据流全景图](docs/archive/SmartRollator_数据流全景图.svg)。
- [思维导图文本](docs/archive/SmartRollator_思维导图.txt)、[数据流文本](docs/archive/SmartRollator_数据流全景图.txt)、[原始 HTML 导出](docs/archive/mermaid.html)。HTML 是 Cocoa 导出的转义源码展示，保留作参考，不标为已可运行的交互页。

## 工具与测试模板的真实状态

| 文件 | 用途 | 使用前待处理事项 |
|---|---|---|
| [monitor.py](tools/monitor.py) | 串口显示、命令与 CSV 采集 | 依赖 pyserial；有串口发送能力，本轮只检查语法，未连接设备 |
| [flash.sh](tools/flash.sh) | OpenOCD 烧录流程参考 | 默认 ELF 尚未建立；ST-Link 检测依赖串口设备名，不能当作通用探测方式；未实测 |
| [debug.sh](tools/debug.sh) | OpenOCD/GDB 调试流程参考 | 会执行 load；自定义 GDB_PORT 未同步传给 OpenOCD；未实测 |
| [CPG 单测模板](tests/unit/test_cpg_control.c) | 参数、有限值、极限环和限幅用例 | 引用不存在的 firmware/App；需适配包含路径、接口和测试构建，未编译/运行 |
| [Controller 单测模板](tests/unit/test_controller.c) | 控制器边界与限幅用例 | 同样引用旧路径，硬件 mock 与内部状态隔离需复核；未编译/运行 |
| [HITL 模板](tests/integration/test_cpg_hitl.py) | 串口采集和输出范围判定 | 依赖 pyserial/设备；需要补全判定覆盖，不能作为完整安全验收；未运行 |
| [Makefile.gcc.template](Makefile.gcc.template) | GCC 迁移构建草案 | firmware/App、GCC FreeRTOS port、链接脚本未落实；不放在工程根目录作为正式 make 入口 |

历史文档中的 `firmware/Core/`、`firmware/Drivers/`、`firmware/Middlewares/` 对应当前仓库根目录同名目录；旧 `firmware/App/` 所指业务文件目前在根 `MDK-ARM/`。这些映射仅帮助阅读，不能靠改目录名认定 GCC 移植或测试已完成。硬件参数与完成度声明必须按现有代码、硬件证据和正式施工文档重新核实。

## 去重与排除

- 未重复复制 STM32 工程、已有 DOCX/图表/HTML/预览和内容一致的生成器。
- 两个旧文档生成器仅保留个人绝对路径，而当前版本已经修正；不引入旧版本。
- 外层旧 README 和 .gitignore 的说明与模板已有重叠、目录规则未适配，未覆盖本仓库文件。
- 未复制系统垃圾、构建缓存和个人路径配置。

本次是资料补全，不推进 Phase 1 的生成/构建验收，也不接入未来正式 Tests 目录。
