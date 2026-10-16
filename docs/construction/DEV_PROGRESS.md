# 开发进度

本文件按时间追加阶段计划和结论。历史记录不因后来成功而删除；最新状态以最下方记录和 `HANDOFF.md` 为准。

## 总览

| 阶段 | 状态 | 说明 |
|---|---|---|
| Phase 0 施工文档基线 | Complete | 文档结构已建立；仅文档验收，不代表固件验证 |
| Phase 1 正式仓库与最小工程 | Partially complete | Git/远端/归档基线已建立；CubeMX/Keil 最小工程和构建尚未建立 |
| Phase 2–11 | Pending | 依赖前置阶段 |

## 2026-08-07 — Phase 0 开工

- Objective：建立适用于 SmartRollator 从零重建的 docs-first 施工体系。
- Affected Layers：产品与工程治理；不改 L0–L5 固件代码。
- Repository State：当前目录经此前检查不是 Git 仓库；旧工程作为参考。
- Planned Files：根 `AGENTS.md`、`docs/INDEX.md`、产品与 construction 文档、阶段/日志模板。
- Tests：文档文件存在性、相对链接、必需章节和状态一致性检查。
- Git Baseline：Not established。
- Backup Branch：Not established。
- Rollback Plan：本阶段只有新增 Markdown 文件；进入新仓库后由 Phase 1 建立正式回滚点。
- Acceptance Criteria：新 Agent 无需聊天记录即可说明产品边界、阶段计划、测试标准、Git 规则和下一步。
- Explicit Exclusions：不初始化 Git、不迁移固件、不宣称构建/烧录/实机通过。

## 2026-08-07 — Phase 0 结论

- Status：Complete（文档范围）。
- Delivered：施工入口、事实源、产品需求、架构、分层契约、Phase 0–11 计划、测试/Git/工具规则、决策、假设、日志、交接和层级进度页。
- Validation：Markdown 结构和链接检查；结果见同日 `LOG.md`。
- Firmware Validation：Not run；本阶段明确排除。
- Git/Push：Not established；当前目录未建立 Git 基线。
- Next：Phase 1，创建独立重建仓库和远端 bootstrap 回滚点，再建立最小可构建工程。

## 2026-08-07 — Phase 1 开工

- Objective：在指定 GitHub 远端建立干净重建主线、旧参考归档和首个可验证回滚点。
- Affected Layers：L0 仓库/工程基础；本增量不改变固件运行行为。
- Repository State：旧参考目录不是 Git 仓库；远端 `https://github.com/NeilBaumanMax/SmartRollator.git` 经 `git ls-remote` 未返回引用；建议新目录尚不存在。
- Planned Files：`.gitignore`、`README.md`、`reference_manifest.sha256`、AGENTS/docs 施工基线及 Git 设计记录。
- Tests：候选文件清单、敏感信息扫描、Markdown 相对链接、`git diff --check`、远端分支哈希核对。
- Git Baseline：Not established；本阶段建立。
- Backup Branch：计划 `backup/bootstrap-20260807`。
- Rollback Plan：`main` 和参考归档使用独立根提交；通过 bootstrap 分支恢复重建起点，参考归档保持只读。
- Acceptance Criteria：`main`、`backup/bootstrap-20260807`、`reference/archive-20260807` 均已推送且远端哈希与本地一致；默认分支待核对；工作树无未解释改动。
- Explicit Exclusions：不生成 CubeMX 工程、不编译/烧录、不接通电机、不把旧构建产物或 IDE 用户状态上传、不宣称旧固件已验证。

## 2026-08-07 — Phase 1 Git 基线结论

- Status：Partially complete；Git 子范围完成，最小可构建工程子范围未开始。
- Repository：`https://github.com/NeilBaumanMax/SmartRollator.git`，默认分支 `main`。
- Main Baseline：`f34420f913f0fa3a19a67f116f083b769815896d`（后续文档状态提交将推进 `main`）。
- Backup Branch：`backup/bootstrap-20260807` → `f34420f913f0fa3a19a67f116f083b769815896d`。
- Reference Archive：`reference/archive-20260807` → `a0669d7e36e54e78970e8162b0bc47334221c6c5`。
- Remote Verification：`git ls-remote --symref` 确认 HEAD 指向 `refs/heads/main`，三条分支远端哈希与本地一致。
- Validation：Markdown 相对链接 Passed；归档清单 1577 项 SHA-256 Passed；敏感模式扫描无匹配；GitHub 推送 Passed。
- Not Run：CubeMX 生成、Keil 编译、烧录、目标板和台架测试。
- Next：在独立 `SmartRollator-Rebuild` 工作目录从 `main` 创建 `build/phase-1-bootstrap`，生成并实际编译最小安全失能工程。

## 2026-08-07 — Phase 1 本地仓库路径纠正

- Trigger：产品负责人指出当前工程目录没有 `.git`，无法看到或使用本地仓库。
- First Attempt：在当前目录执行 `git init -b main`，因沙箱对 `.git` 的写权限限制失败，错误为 `Operation not permitted`。
- Fix：取得明确目录写权限后重新执行，成功创建 `/Users/neil/项目/SmartRollator/SmartRollator/.git`。
- Repository State：本地 `main` 已绑定 `origin/main`；远端 main、backup 和 reference 分支已抓取；完整工程正作为精确暂存范围纳入 `main`。
- Design Correction：正式施工入口改为当前工程目录；旁边的 `SmartRollator-Rebuild` 不再是正式入口。
- Full Workspace Baseline：`2cd83025946a8ab55553f834773f7f7bc52bd955`，1549 个变更文件已提交。
- Push：Passed，`main` 从 `97208dc` 推进到 `2cd8302`。
- Validation：暂存区未包含意外构建输出；33 个 Markdown 相对链接 Passed；旧生成文件的 CRLF/行末空白按原始证据保留。

## 2026-08-07 — 施工文档漂移与 Git 文件级设计开工

- Objective：统一当前正式仓库事实，并把 V1 Git 施工树细化到微提交、精确文件和主要接口。
- Affected Layers：工程治理与全部 L0–L5 的施工规划；不修改固件行为。
- Repository State：正式仓库为当前 Git 工作树根目录；分支 `docs/git-design-tree`；起点 `75d5609`；开工时工作树干净。
- Planned Files：`CODEX_START_HERE.md`、`CODEX_MASTER_REQUIREMENTS.md`、`CONSTRUCTION_PLAN.md`、`GITHUB_ROLLBACK.md`、新增 `GIT_CONSTRUCTION_TREE.md`、当前交接和受影响层级页。
- Tests：旧路径/旧仓库模型扫描、Markdown 相对链接、必需章节、状态一致性、`git diff --check`、远端分支哈希核对。
- Git Baseline：`75d5609f9115c704c3d36dacc3eab1994442b126`。
- Backup Branch：`docs/git-design-tree` 已在改文档前推送到远端并指向开工基线。
- Rollback Plan：放弃文档分支或对本轮单一文档提交执行 `git revert`；不改写历史日志。
- Acceptance Criteria：所有当前事实源只指向正式当前仓库；历史材料明确标为历史；每个计划微提交列出精确文件、功能/接口、测试和回滚点。
- Explicit Exclusions：不生成 CubeMX、不开启电机、不修改参考固件、不声称编译或硬件通过。

## 2026-08-07 — 施工文档漂移与 Git 文件级设计结论

- Status：Complete（文档治理子范围）；Phase 1 整体仍为 Partially complete。
- Delivered：修正当前事实源中的旧路径/双仓库模型；新增 `GIT_CONSTRUCTION_TREE.md`，把 Phase 1–11 拆为 66 个文件级微提交节点。
- Precision：每个节点列出精确文件、文件职责/主要接口、最邻近测试和回滚点；CubeMX/MDK 厂商生成文件采用“精确生成文件清单 + SHA-256 清单”约束，禁止在生成前臆造内容。
- Historical Records：`LOG.md`、本文件既有条目和 `docs/handover/` 原始内容不改写；历史交接页已增加失效范围提示。
- Validation：34 个 Markdown 相对链接 Passed；11 个阶段段落、66 个施工节点结构检查 Passed；当前事实源旧仓库模型扫描 0 命中；状态一致性 Passed；`git diff --check` Passed。
- Firmware Validation：Not run；未执行 CubeMX 生成、Keil 编译、烧录、目标板或台架测试。
- Rollback：本轮改动集中于 `docs/git-design-tree`；可按提交执行 `git revert`，不依赖历史改写。
- Next：从最新 `main` 建立并先行推送 `build/phase-1-bootstrap`，从 P1.02 开始记录工具链，再执行 P1.03 最小工程生成。

## 2026-08-07 — P1.02 工具链冻结开工

- Objective：用只读、可重复执行的检测入口确认本机可用于 P1.03/P1.05 的 CubeMX、MDK、Arm Compiler 和 STM32Cube FW_H7 状态，并冻结证据格式。
- Affected Layers：L0 工具链与构建治理；不修改 MCU 配置、生成源码或固件行为。
- Repository State：从已验收 `main` 提交 `5403028d255ac16d2e137454fc40513e75eb01f2` 创建 `build/phase-1-bootstrap`；开工时工作树干净。
- Construction Tree Nodes：P1.02。
- Planned Files：新建 `docs/verification/TOOLCHAIN.md`、`docs/verification/builds/README.md`、`tools/check_toolchain.sh`；收尾时只更新 `AGENTS.md`、`DEV_PROGRESS.md`、`LOG.md`、`HANDOFF.md`、`progress/layers/00-foundation.md`。
- Planned Interfaces：`sh tools/check_toolchain.sh`，只输出检测状态和候选版本，不安装、不修改工具；任一必需工具未确认时返回非零。
- Tests：POSIX shell 语法检查；实际执行检测；输出不得含个人绝对路径；Markdown 相对链接；`git diff --check`；远端分支哈希核对。
- Git Baseline：`5403028d255ac16d2e137454fc40513e75eb01f2`。
- Backup Branch：`build/phase-1-bootstrap` 已在修改前推送到 `origin` 并指向开工基线。
- Rollback Plan：P1.02 形成单一提交；失败可放弃分支或对该提交执行 `git revert`，不触碰参考归档。
- Acceptance Criteria：检测脚本如实区分 Found/Not found/Unverified；文档记录本机实际结果、P1.03 阻塞项和构建证据命名；提交推送并核对哈希。
- Explicit Exclusions：不安装软件、不启动 GUI、不生成 CubeMX、不编译/烧录、不连接或使能电机、不把个人路径写入仓库。

## 2026-08-07 — P1.02 工具链冻结结论

- Status：Complete（P1.02 检测与证据格式子范围）；Phase 1 整体仍为 Partially complete。
- Delivered：`tools/check_toolchain.sh`、`docs/verification/TOOLCHAIN.md`、`docs/verification/builds/README.md`；并同步当前阶段、日志、交接和 L0 状态。
- Actual Host Result：macOS arm64；STM32CubeMX、Keil MDK、Arm Compiler、STM32Cube FW_H7 均为 Not found；参考 `.ioc` 中的 FW_H7 V1.12.1 / MDK-ARM V5.32 仅记为历史候选。
- Validation：`sh -n` Passed；脚本实际运行按设计返回 1 和 `TOOLCHAIN_STATUS=INCOMPLETE`；缺失项断言 Passed；个人路径输出扫描 Passed；36 个 Markdown 相对链接 Passed；`git diff --check` Passed。
- First Failure：首次人工盘点因 zsh 对不存在的 `/Applications/Keil*` 通配展开报 `no matches found` 而提前退出；改为目录查找和显式候选后完整复测 Passed。
- Firmware Validation：Not run；未生成、编译、烧录或执行目标板/台架测试。
- P1.03 Gate：Blocked by environment；在可确认版本的 CubeMX 与匹配 FW_H7 包可用前，不替换 `.ioc`、`Core/`、`Drivers/`、`Middlewares/` 或 `MDK-ARM/`。
- P1.05 Gate：Blocked by environment；需要可用且版本已确认的 MDK/Arm Compiler 构建环境。
- Next：由产品负责人提供现有工具位置/Windows 构建环境，或明确授权安装所需工具；随后重新运行检测，只有状态 Complete 才进入 P1.03。

## 2026-10-16 — 工作区参考材料整理开工

- Objective：保持 STM32 工程完整，把对话生成的 DOCX、HTML、生成脚本、配图和研究笔记集中到 `Files/`。
- Scope：只移动参考资产并修正生成脚本路径；`docs/` 保留施工事实源，`tools/check_toolchain.sh` 保留原入口。
- Repository State：`build/phase-1-bootstrap`，起点 `4f5af0a6059f3061b00c421e75340eca4bf92257`；远端分支同哈希；开工前仅 `docs/research/` 未跟踪。
- Tests：工程全文件 SHA-256 前后比对；Keil 源文件/包含目录引用检查；移动资产哈希核对；生成脚本语法和路径检查；Markdown/HTML 本地链接；Git 差异及远端哈希。
- Rollback Point：`4f5af0a`；移动前推送 `backup/workspace-before-files-20261016`；本次整理提交可单独 revert。未跟踪研究笔记仅移动并保留未跟踪状态。
- Exclusions：不改动 `.ioc`、`.mxproject`、`Core/`、`Drivers/`、`Middlewares/`、`MDK-ARM/`；不再生成报告，不安装工具，不推进 P1.03，不编译/烧录或实机测试；历史归档清单保持原始路径和内容。

## 2026-10-16 — 工作区参考材料整理验证结论

- Delivered：26 个本地文件迁入 `Files/`，其中 22 个原有 Git 跟踪资产；保留预览缓存与未跟踪研究笔记，原始报告不重生成。3 个脚本仅修正输入/输出路径，其余 23 个迁移文件字节一致。
- Validation：1669 个受保护文件 SHA-256 与开工快照完全一致（含完整工程、原有本地输出、工具链脚本及历史清单）；Keil 49 个文件引用与 17 个包含目录条目均存在；5 个生成脚本语法和路径检查通过；43 个 Markdown 相对链接与 2 个 HTML 的本地引用检查通过；`sh -n tools/check_toolchain.sh`、`git diff --check` 通过。
- Failures：本轮验证首次执行全部通过，无修复重测项。
- Firmware Validation：Not run；只验证工程文件与路径完整性，不代表编译、烧录或实机通过；不推进 P1.03。
- Backup：远端 `backup/workspace-before-files-20261016` 已核对为 `4f5af0a6059f3061b00c421e75340eca4bf92257`。本轮整理在 `build/phase-1-bootstrap` 形成独立可回滚提交。
