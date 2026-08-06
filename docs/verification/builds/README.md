# 构建证据目录

本目录保存可审阅的构建摘要，不提交 IDE 用户状态、可再生二进制、含个人路径的完整原始日志或许可证信息。

## 文件命名

```text
phase-<N>-<purpose>-<YYYYMMDD>.md
```

Phase 1 最小工程使用：

```text
phase-1-minimal-<YYYYMMDD>.md
```

## 每份摘要必须包含

1. 对应提交完整哈希与分支；
2. CubeMX、STM32Cube FW 包、MDK 和编译器实际版本；
3. 目标 MCU、工程配置和完整 Rebuild 命令或人工步骤；
4. 首次构建的 errors/warnings 原样摘要；
5. 修复内容与独立复测结果；
6. 产物名称、外部证据位置和 SHA-256；
7. 明确写出 Not run 的生成、烧录、目标板或台架步骤；
8. 执行人和时间，但不得包含个人绝对路径或密钥。

## 结果词

- `Passed`：实际命令/步骤完成且满足该次验收条件。
- `Failed`：实际执行但未满足条件，必须保留错误摘要。
- `Not run`：本轮未执行。
- `Not established`：尚无可执行环境或入口。

“CubeMX 生成成功”不能写成“Keil 编译通过”；“Keil 编译通过”也不能替代烧录、目标板或台架验证。
