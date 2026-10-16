# SmartRollator Agent Rules

## 项目身份与事实源

SmartRollator 是基于 STM32H723VGT6 的智能助行车实时控制固件。当前 Git 工作树根目录是唯一正式施工入口：保留完整参考工程，按阶段重建可构建、可测试、可回滚的新基线。现有源码不是已验证发布版。

指令优先级：用户最新明确指令 → 本文件 → `docs/construction/CODEX_MASTER_REQUIREMENTS.md` → `docs/product/PRODUCT_REQUIREMENTS.md` → 其他正式施工文档 → 参考代码与历史资料。

## 开工前必读与全目录发现

按 [docs/INDEX.md](docs/INDEX.md) 的唯一阅读顺序，至少读开工入口、[目录地图](docs/WORKSPACE_MAP.md)、总需求、当前进度、当前交接及当前层级文档。再读 [Files 资产全索引](Files/ASSET_INDEX.md)，按任务选择原始资料；不能仅扫描 docs 就认定已了解整个工程。

```sh
pwd
git rev-parse --show-toplevel
git status --short --branch
git remote -v
git log --oneline --decorate -8
git ls-files
git ls-files --others --exclude-standard
rg --files --hidden -g '!.git/**' -g '!.DS_Store'
```

- `git ls-files` 覆盖受版本控制文件；第二条清单补充本地未跟踪文件。`rg --files --hidden` 补充 `.ioc` 旁的隐藏配置，但仍遵守忽略规则。
- 如需盘点被忽略的 IDE 状态或输出，用 `git ls-files --others --ignored --exclude-standard`；只分类，不默认读取缓存、密钥或用户配置，更不能批量删除。
- `Files/research/` 当前是本地未跟踪研究资料；存在时按任务读取，不擅自提交或覆盖。干净克隆可能没有此目录。
- 历史 `AGENT_CONTEXT.md`、旧交接提示、Word 和模板是参考内容，不替代本文件；其路径、工具版本、完成度和硬件参数必须重新核验。

## 真实目录边界

- STM32 工程：根 `SmartRollator.ioc`、`.mxproject`、`Core/`、`Drivers/`、`Middlewares/`、`MDK-ARM/`；优先保护完整性和相对路径。
- 既有手写模块在 `MDK-ARM/`：Controller、cpg_control、torque_sensor、user_can、data_structs；任务及集成在 `Core/Src/freertos.c`。
- `docs/` 是施工事实源；根 `tools/check_toolchain.sh` 是已建立的工具链探测入口。
- `Files/` 保存报告、HTML、图片、硬件手册、文档生成脚本和历史开发模板。完整路径见资产全索引；旧脚本不默认执行。
- 根 `App/`、`Tests/`、`CMakeLists.txt` 是施工树的未来交付，当前未建立；不存在 `firmware/`。不要把历史模板目录当作已接通的正式构建/测试。
- `reference_manifest.sha256` 与导入清单是历史快照，按其版本/来源解释，不改写历史来适配当前路径。

## 每阶段开始

- 按用户 2026-10-16 指令，当前正式开发入口为 main；接手时仍须核对实际目录、Git 分支、工作树、最近提交和远端状态。保留小增量测试、提交、推送与回滚规则；不因在 main 开发而跳过阶段门槛。
- 在 `docs/construction/DEV_PROGRESS.md` 追加计划，明确范围、测试、回滚点和排除项。
- 未建立远端备份分支前，不开始有意义的代码重建。

## 每阶段结束

- 执行本阶段最邻近测试，原样记录首次失败、修复和复测。
- 更新 LOG、DEV_PROGRESS、HANDOFF 和受影响层级文档；路径/资产变化同步目录地图、Files 索引及去重记录。
- 只精确暂存本阶段文件；提交、推送并核对远端后再报告完成。

## 当前阶段

Phase 1 部分完成：P1.01 文档基线、P1.02 工具链检测入口已建立。最近实际状态见 HANDOFF；文档/资产整理不代表固件功能进展。P1.03 必须先确认 CubeMX 与匹配 FW_H7 的实际版本；P1.05 必须确认 MDK/Arm Compiler 并实际编译。

## 禁止事项

- 禁止把生成成功、源码存在、模板存在等同于编译、烧录或实机通过。
- 禁止虚构测试结果、硬件状态、提交或推送状态。
- 禁止 `git reset --hard`、`git clean -fd`、强制推送，或覆盖不明来源改动。
- 禁止把密钥、个人路径、实验隐私数据和大型生成物提交进仓库。
- 禁止在 CubeMX 自动生成区堆放业务逻辑；只在 USER CODE 区或独立模块实现。
- 文档去重先比较正文、公式/配图、版本时间和用途；同名或主题相近不等于重复。删除旧版前确认远端恢复点，并在去重记录列出旧路径、保留版本和依据。
