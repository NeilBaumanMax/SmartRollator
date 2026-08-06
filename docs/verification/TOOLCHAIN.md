# Phase 1 工具链证据

状态：P1.02 检测入口已建立；当前主机工具链不完整。

## 目标组合

| 组件 | 项目要求 | 当前主机实测 | 证据边界 |
|---|---|---|---|
| 主机 | 能运行生成与构建工具 | macOS 26.5.2，arm64 | `sw_vers`、`uname -m`，2026-08-07 |
| STM32CubeMX | 实际版本待确认 | Not found | 命令候选和两个标准 macOS 应用位置均未发现 |
| STM32Cube FW_H7 | 实际安装版本待确认 | Not found | 两个标准用户 Repository 位置均未发现 H7 包 |
| Keil MDK | 实际版本待确认 | Not found | `uv4`、`UV4.exe`、`cbuild` 和标准应用候选未发现 |
| Arm Compiler | 实际版本待确认 | Not found | `armclang`、`armcc` 及 Windows 默认候选未发现 |

## 参考工程声明值

当前参考文件包含以下历史声明，只能用于选择候选版本，不能作为本机安装或当前构建证据：

- `SmartRollator.ioc`：`STM32Cube FW_H7 V1.12.1`。
- `SmartRollator.ioc`：`MDK-ARM V5.32`。
- `SmartRollator.ioc`：MCU `STM32H723VGTx`。

## 检测入口

```bash
sh tools/check_toolchain.sh
```

脚本只读取命令可用性、标准安装候选和版本元数据，不安装软件、不启动电机相关工具、不生成工程。输出只包含产品名、版本和状态，不输出发现位置的绝对路径。

若 `UV4.exe` 可用但命令行无法安全读取版本，可先在 MDK 的 About 对话框人工核对，再仅对本次检测传入 `SMARTROLLATOR_MDK_VERSION=<verified-version>`。该变量只补充已发现可执行文件的版本，不能把缺失的 MDK 伪装为 Found；人工来源仍须写入构建摘要。

退出码约定：

- `0`：CubeMX、MDK、Arm Compiler 和 FW_H7 均已找到且版本可确认；`TOOLCHAIN_STATUS=COMPLETE`。
- `1`：任一必需组件缺失或版本仍为 `UNVERIFIED`；`TOOLCHAIN_STATUS=INCOMPLETE`。

## 当前结论

P1.02 可以完成为“检测和证据格式已冻结”，但 P1.03/P1.05 尚不具备执行条件：

- P1.03 在已确认的 CubeMX 与匹配 FW_H7 包可用前不得替换 `.ioc` 或生成源码。
- P1.05 需要可用且版本已记录的 MDK/Arm Compiler 环境；当前 macOS 主机未建立该环境。
- 不允许用参考工程的旧生成文件或旧构建输出冒充新工程生成/编译结果。

安装、许可证配置或选择 Windows 构建环境属于后续明确授权范围，不在 P1.02 内执行。
