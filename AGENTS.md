# SmartRollator Agent Rules

## 项目身份与事实源

SmartRollator 是基于 STM32H723VGT6 的智能助行车实时控制固件。当前 Git 工作树根目录是唯一正式施工入口：保留完整参考工程，按阶段重建可构建、可测试、可回滚的新基线。现有源码不是已验证发布版。

指令优先级：用户最新明确指令 → 本文件 → `docs/construction/CODEX_MASTER_REQUIREMENTS.md` → `docs/product/PRODUCT_REQUIREMENTS.md` → 其他正式施工文档 → 参考代码与历史资料。

## 每次工作先读 docs

每次新任务、接续施工、会话恢复或上下文压缩后，修改任何工程文件前，重新读取 [docs/INDEX.md](docs/INDEX.md) 的必读项：开工入口、WORKFLOW、DEV_PROGRESS 最新计划与结论、HANDOFF、LOG 最新记录及当前层级/施工树节点。不能以“上轮已读”、聊天摘要或记忆替代磁盘文档；同一未中断增量内无需每条命令都重读。若记录与代码/Git 不符，先核实并补记，不凭上下文猜完成度。

每个施工增量都必须闭环：**读 docs → 在 docs 写计划 → 实施 → 静态自检/适用测试 → 修正文档漂移与交接 → 精确提交 → 推送核对**。细则以 [WORKFLOW](docs/construction/WORKFLOW.md) 为准。中断也要落盘已做、未做和下一步，不只在聊天中交代。

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
rg --files --hidden -g '!.git/**'
```

- `git ls-files` 覆盖受版本控制文件；第二条清单补充本地未跟踪文件。`rg --files --hidden` 补充 `.ioc` 旁的隐藏配置，但仍遵守忽略规则。
- 每次备份同时盘点 `git ls-files --others --exclude-standard` 与 `git ls-files --others --ignored --exclude-standard`，两者都必须没有未解释遗漏；原构建输出、IDE 状态、预览和系统元数据已按全量备份指令纳入。新增疑似凭据先核实，不能用全量备份理由静默泄露密钥。
- 按用户 2026-10-16 最新指令，工作区全部普通文件均纳入 GitHub 备份；`Files/research/` 已属于正式跟踪资料，不再只保留本地。`.git/` 是 Git 内部数据库，不递归提交。
- 历史 `AGENT_CONTEXT.md`、旧交接提示、Word 和模板是参考内容，不替代本文件；其路径、工具版本、完成度和硬件参数必须重新核验。

## 真实目录边界

- STM32 工程：根 `SmartRollator.ioc`、`.mxproject`、`Core/`、`Drivers/`、`Middlewares/`、`MDK-ARM/`；优先保护完整性和相对路径。
- 既有手写模块在 `MDK-ARM/`：Controller、cpg_control、torque_sensor、user_can、data_structs；任务及集成在 `Core/Src/freertos.c`。
- `docs/` 是施工事实源；根 `tools/check_toolchain.sh` 是已建立的工具链探测入口。
- `Files/` 保存报告、HTML、图片、硬件手册、文档生成脚本和历史开发模板。完整路径见资产全索引；旧脚本不默认执行。
- 根 `App/`、`Tests/`、`CMakeLists.txt` 是施工树的未来交付，当前未建立；不存在 `firmware/`。不要把历史模板目录当作已接通的正式构建/测试。
- `reference_manifest.sha256` 与导入清单是历史快照，按其版本/来源解释，不改写历史来适配当前路径。

## 每次施工开始

- 按用户 2026-10-16 指令，当前正式开发入口为 main；接手时仍须核对实际目录、Git 分支、工作树、最近提交和远端状态。保留小增量测试、提交、推送与回滚规则；不因在 main 开发而跳过阶段门槛。
- 在 `docs/construction/DEV_PROGRESS.md` 追加计划，明确范围、测试、回滚点和排除项。
- 未建立远端备份分支前，不开始有意义的代码重建。

## 每个增量结束

- 按用户 2026-10-17 指令，普通 STM32 代码增量允许以有具体文件/检查点/结论的静态自检验收；执行已建立且适用的近邻检查，记录首次失败、修复和复测。未执行的编译、烧录、实机测试明确标注，不能据此宣称阶段或硬件通过；证据边界见 TEST_METRICS。
- 更新 LOG、DEV_PROGRESS、HANDOFF 和受影响层级文档；路径/资产变化同步目录地图、Files 索引及去重记录。
- 只精确暂存本阶段文件；全量备份须先生成逐文件清单，再按清单暂存。提交、推送并核对远端快照后再报告完成。

## 当前阶段

Phase 1 部分完成：P1.01 文档基线、P1.02 工具链检测入口已建立。最近实际状态见 HANDOFF；文档/资产整理不代表固件功能进展。P1.03 必须先确认 CubeMX 与匹配 FW_H7 的实际版本；P1.05 必须确认 MDK/Arm Compiler 并实际编译。

## 禁止事项

- 禁止把生成成功、源码存在、模板存在等同于编译、烧录或实机通过。
- 禁止虚构测试结果、硬件状态、提交或推送状态。
- 禁止 `git reset --hard`、`git clean -fd`、强制推送，或覆盖不明来源改动。
- 禁止上传未获明确授权的密钥、令牌或实验隐私数据。用户本轮全量备份已授权保留现有本地配置路径、IDE 状态及旧生成物；只作现场快照，不能视为可移植配置或当前通过证据。超出托管限制的文件必须记录处理方案，不静默排除。
- 禁止在 CubeMX 自动生成区堆放业务逻辑；只在 USER CODE 区或独立模块实现。
- 文档去重先比较正文、公式/配图、版本时间和用途；同名或主题相近不等于重复。删除旧版前确认远端恢复点，并在去重记录列出旧路径、保留版本和依据。
