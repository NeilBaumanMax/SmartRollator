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

## 2026-10-16 — 补入另一工作区独有参考资产开工

- Objective：筛选另一工作区独有且有用的资产，复制到 Files 的分类目录；原工作区只读保留。
- Scope：硬件手册、Hopf 文档、历史架构/接口/数据流文档及图表、串口/调试工具、单元与 HITL 测试模板、GCC Makefile 模板；不覆盖现有文件。
- Repository：`build/phase-1-bootstrap`，起点 `1d79da55aa25c312103c8c84f9c74c8516df0c39`，远端同哈希；仅已有 `Files/research/` 未跟踪。
- Backup：复制前建立并核对远端 `backup/before-reference-import-20261016`，指向开工提交。
- Tests：来源/目标 SHA-256 一致、原工程及既有资产不变、无重复内容导入、Python/shell 语法、DOCX/PDF/SVG 结构和导航链接检查、精确暂存及远端哈希。
- Exclusions：不改 STM32 工程，不修建 GCC 构建链，不运行串口、烧录、调试或硬件测试；不安装依赖，不将模板认定为可用正式工具；不提交已有未跟踪研究笔记。
- Rollback：可对本次单一资产导入提交执行 revert；源目录保持不变。

## 2026-10-16 — 独有参考资产补入验证结论

- Delivered：23 份独有原文件按字节复制：2 份硬件 PDF、1 份 Hopf DOCX、5 份历史 Markdown、5 份 SVG、2 份图示文本、1 份 HTML 导出、3 份工具脚本、3 份测试模板和 1 份 GCC Makefile 模板。
- Layout：`Files/hardware/` 保存手册；`Files/Hopf 振荡器.docx` 保存报告；其余放 `Files/legacy-workspace/`，附 README 与来源相对路径/SHA-256 清单。
- Validation：23 份来源未改、复制哈希一致、无与现有资产重复内容；既有工程和资产保持原样；2 个 Python、2 个 shell 语法通过；5 个 SVG XML、DOCX ZIP/XML 和 2 个 PDF 头尾结构检查通过；72 个 Markdown 本地链接通过。
- Limitations：未编译 C 测试，不执行模板、串口、烧录、调试或硬件测试；不将资料说明作为当前硬件/固件通过证据；Phase 1 仍为 Partially complete。
- Backup：远端 `backup/before-reference-import-20261016` 已核对为 `1d79da55aa25c312103c8c84f9c74c8516df0c39`。

## 2026-10-16 — 文档漂移修正与 DOCX 版本收敛开工

- Objective：统一实际目录、当前分支、工程与参考资产边界；让 Agent 能按索引覆盖全部资料；列明 DOCX 重复主题版本并保留最新。
- Baseline：`build/phase-1-bootstrap`，起点 `1a9f299e6498579e5dbb4e9f03972e1ace270814`；远端同哈希，已有 `Files/research/` 未跟踪。
- Scope：AGENTS、README、文档导航、目录地图、当前交接/工具链/L0/施工树相关说明、参考资产全索引、DOCX 去重报告；移除两组中的 3 份旧版 DOCX。
- Backup：先推送 `backup/before-docs-dedup-20261016`；原始文件可从该备份按路径恢复。
- Tests：DOCX ZIP/正文/公式/元数据比对；全文件哈希去重；工程与保留资产哈希不变；Markdown 链接和资产导航覆盖率；工具链只读探测；Git 差异与远端哈希。
- Exclusions：不修改 STM32、生成脚本或保留 DOCX 内容；不重生成/编译/烧录；不改写历史日志和导入清单；未跟踪研究笔记保留原状态。
- Rollback：本轮单一文档/资产整理提交可 revert；不使用破坏性恢复。

## 2026-10-16 — 文档漂移修正与 DOCX 版本收敛验证结论

- Delivered：统一 AGENTS/INDEX 阅读顺序，新增目录地图和 47 项 Files 资产索引（不含索引自身）；更新 README、HANDOFF、工具链证据、施工树、分层契约、Git 规则、ADR 和 L0 进度。当前分支与 main、实际目录与未来 App/Tests、正式工具与历史模板均已区分。
- DOCX：9 份无字节相同副本；应用说明 3 版留最新 LaTeX 版，参数详解 2 版留 2026-06-04 扩展版，共移除 3 份旧主题版本、保留 6 份；具体差异、时间证据限制和旧哈希见 Files/DUPLICATES.md。
- Validation：1669 个受保护文件 SHA-256 不变；6 份保留 DOCX 字节不变；49 个 Keil 文件引用/17 个包含目录条目存在；154 个 Markdown 本地链接通过；索引覆盖全部 48 个当前受跟踪/本轮新增 Files 资产（含索引自身）；参考资产哈希完全重复组 0。
- Toolchain：2026-10-16 运行 `sh tools/check_toolchain.sh`，退出 1，四项 NOT_FOUND / INCOMPLETE，符合检测契约；不代表系统全部位置均无安装。
- Firmware：Not run；未重生成、编译、烧录或实机验证，不改变 Phase 1 整体验收状态。
- Rollback：远端 backup/before-docs-dedup-20261016 已核对为 1a9f299e6498579e5dbb4e9f03972e1ace270814；本轮独立提交可回滚，研究笔记仍为本地未跟踪。

## 2026-10-16 — 当前状态提升到 main 开工

- Objective：按用户明确指令，将当前已提交状态作为 main 主分支继续开发，并推送远端。
- Baseline：当前 build/phase-1-bootstrap 为 ac0c0d30abd368e4124d01d10e91176c724dd8d6；fetch 后 origin/main 和本地 main 均为 5403028d255ac16d2e137454fc40513e75eb01f2，落后 4 个提交，无分叉。
- Scope：统一当前分支文档、快进 main、推送与核对；保留阶段分支及所有备份。
- Backup：main 提升前保存远端 backup/main-before-promotion-20261016；原阶段提交已在远端保存。
- Tests：祖先关系、非文档资产哈希、文档链接与状态漂移、精确暂存、main 和 origin/main/远端 HEAD 一致。
- Exclusions：不强推，不改工程，不宣称 Phase 1 构建完成；不提交已有 Files/research 未跟踪笔记。
- Rollback：从旧 main 备份创建恢复分支，或按提交 revert；不改写 main 历史。

## 2026-10-16 — 主分支提升交付范围

- 当前完整阶段状态从 ac0c0d3 承接到 main，另提交本轮分支状态文档；旧阶段分支和全部备份保留。
- AGENTS、README、开工入口、目录地图、HANDOFF、Git 规则、施工树和 L0 状态统一指向 main；后续按用户指令在 main 继续开发。
- 原 main 备份：backup/main-before-promotion-20261016 → 5403028d255ac16d2e137454fc40513e75eb01f2。
- 验收：仅文档修改；工程及参考文件保持原样；main 快进、推送及本地/远端哈希核对作为交付检查。
- Files/research 继续未跟踪，未纳入推送；Phase 1 整体仍为 Partially complete。

## 2026-10-16 — 上层资料归并与全工作区备份开工

- Objective：按用户最新指令，将上层目录资料归入当前工作区，并将当前工作区全部普通文件备份 GitHub，不遗留未跟踪或忽略文件。
- Baseline：main/origin/main 均为 6a5aaf7087cbc1b2267ba4b048193a0482a4f23c。盘点 1767 个文件，158 个尚未跟踪（含原忽略项），总计约 100 MB 待补；Git 数据库 .git 属于仓库内部元数据，不递归纳入工作区文件清单。
- Scope：上层 4 份内容相同资料归并至既有路径；上层 Finder 元数据单独保存；研究笔记、旧构建输出、IDE 状态、预览和系统元数据按现状纳入；同步忽略政策、AGENTS、索引和交接。
- Backup：远端 backup/before-complete-workspace-20261016 保存开工提交；全部原文件 SHA-256 已留本地核对快照。
- Tests：来源/目标哈希、全目录与 Git 索引集合相等、提交对象内容一致、凭据模式检查、文档链接、Keil 引用、远端快照内容核对；最终 tracked 工作树干净，untracked 和 ignored 均为零。
- Exclusions：不重新生成或编译，不烧录/连接硬件；旧产物不作为当前固件通过证据；不递归提交 .git 内部文件。
- Policy Override：用户最新“全部备份”覆盖此前不提交构建缓存、IDE 状态和本地研究笔记的项目规则。本轮扫描新增候选未命中凭据模式；原始本地路径配置按完整快照保留，不作为可移植配置示例。
- Rollback：可从远端备份创建恢复分支；本次清单记录原上层路径、目标与哈希，便于恢复。

## 2026-10-16 — 全量备份实施与验证范围

- 上层 4 份资料与已有已跟踪内容完全相同，核对源/目标/Git blob 后归并；上层 Finder 元数据移入 Files/local-state，映射见 parent-import.tsv。
- 原 158 个未跟踪/忽略文件均纳入；.gitignore 不再排除旧构建物、IDE 状态、预览或元数据；研究笔记已跟踪。
- 原始 Keil 状态以 binary 属性保护字节及编码；研究 Markdown 原有硬换行保留。首轮检查解码错误与空白诊断、修复与复测详见 LOG。
- 备份清单 files.sha256 覆盖除清单自身外的全部普通文件；自身由 Git blob 核验。最终必须满足磁盘/提交路径一致、内容一致、untracked 与 ignored 均为零，并独立抓取远端 main 验证。
- 所有源代码和既有资料按原始字节保留，不生成或验证新固件；Phase 1 仍为 Partially complete。

- 原始全量快照已形成提交 677c23a，保留研究原文及全部旧状态。其后修复研究笔记 5 处移动前遗留的相对链接，正文不改；原始版本可从快照恢复。

## 2026-10-17 — 每次接续施工闭环 / 开工计划

- Read Before Work：已读 AGENTS、INDEX、开工入口、WORKFLOW、总需求、最新进度/日志/交接、产品/架构/契约、施工计划/当前施工树、L0、目录地图与 Files 索引。
- Objective：参考 idea-to-production-vibecoding 的 Resume 与施工闭环，将 docs 作为每次恢复工作的持久进度入口，落实先计划、实施、自检、漂移修正、提交和推送。
- Affected Layers / Nodes：L0 仓库治理与 P1.01 文档维护；不推进 P1.03/P1.05。
- Repository State：main 与 origin/main 在 fetch 后左右差异为 0/0；开工已有 .DS_Store、Files/.DS_Store 两项 Finder 元数据变动，按此前全部备份授权保留字节并纳入本轮快照。
- Git Baseline：df3da6870a9eebaaf215ac8cfb832cc98d681567。
- Backup Branch：backup/before-session-workflow-20261017，已推送并用 ls-remote 核对为开工提交。
- Planned Files / Interfaces：AGENTS、INDEX、开工入口、WORKFLOW、总需求、测试指标、开工/日志模板、进度/日志/交接/L0、受影响 Git 及产品范围说明、全量清单；无固件接口改动。
- Tests：文档链接、规则一致性、STM32 与非治理资产 SHA-256 保全、diff 空白、磁盘/索引完整覆盖、清单与 Git 对象字节核对；推送后核对 main 远端哈希。
- Acceptance Criteria：新 Agent 不读聊天即可找到最新计划、真实完成项、验证边界和下一步；静态审阅可验收普通代码增量，编译/硬件证据不混淆；本轮治理变更提交推送。
- Rollback Plan：从上述远端备份建恢复分支，或 revert 本轮提交；不覆盖已有改动。
- Explicit Exclusions：不改固件/工具/参考资产，不安装外部 skill，不生成、编译、烧录或连接硬件；不把 Phase 1 标成完成。

## 2026-10-17 — 接续施工规则实施与检查结论

- Delivered：每次恢复先读 docs、代码前计划落盘、普通 STM32 增量具体静态审阅、失败/修复留痕、文档漂移修正、交接和提交推送闭环，统一至 AGENTS/WORKFLOW 及现有模板。
- Drift：消除阶段分支强制要求、旧产物禁止备份、静态审阅不能作为任何验收依据的过时表述；不改硬件/构建证据标准。
- Validation：63 项本轮 Markdown 本地链接、1672 个 STM32 文件 SHA-256 保全、Files 参考资产保全与 diff 空白检查通过；首轮 Finder 元数据变化触发范围断言，说明和复测记录见 LOG。
- Firmware：未修改；生成/编译/烧录/硬件 Not run，Phase 1 仍为 Partially complete。
- Delivery：本轮完成文档自检，按 WORKFLOW 精确提交并推送 main；最终提交/远端哈希由 Git 实时核对。恢复时先查 HANDOFF 的本轮交付核验，避免把开工哈希当作最终 HEAD。

- 收尾新增输入：开工后出现 Files/sdk.docx；只读确认内容与 ZIP 完整性，按全部备份授权原样纳入，补充 Files 索引/DOCX 记录；当前普通文件 1774、DOCX 7，最终覆盖检查重跑。

## 2026-10-17 — 思路讨论接手与研究资料保全 / 开工计划

- Read Before Work：完整读取用户交接提示词；按 INDEX 读取 AGENTS、开工入口、WORKFLOW、目录地图/Files 索引、总需求/产品/架构/契约、施工计划/当前节点、最新进度/交接/日志、00 基础/05 固定导纳/06 Hopf 层级记录、决策/假设；另读旧研究笔记和新导师讨论稿。
- Objective：保留 Files/start_researching 的原始讨论资产，核查参考控制链，落盘固定 M → PBO 个性化 D0 → 局部相位减阻 → 直接动捕/Hopf 相位对照的新研究基线；本次不迁移算法。
- Affected Nodes：P1.01 文档维护与资料备份；05 固定导纳、06 Hopf 的研究方向说明，固件阶段完成度不变。
- Repository State：main 与 origin/main、ls-remote main 均为 bc08d08ae9040f6e8f6ec1d8f3eb59d7bc0ba697；1774 tracked、17 untracked、0 ignored。已有 .DS_Store 与 Files/.DS_Store 变化按既有全量备份授权保留，新增 17 项全部来自用户指定目录。
- Planned Files：新增研究基线与原始交接提示词归档、start_researching 说明、逐项大小/SHA-256 导入清单；旧研究笔记仅加状态指向；更新 INDEX、目录地图、Files 索引/说明/版本记录、DEV_PROGRESS/HANDOFF/LOG、DECISIONS/ASSUMPTIONS、05/06 层级、产品范围澄清及施工树 P9.02 候选说明、备份说明/全量哈希。无 C 接口改动。
- Tests：原始 17 项/提示词哈希；ZIP 完整性及压缩包与解压目录内容对照；SVG XML/PNG 结构、Markdown 与 HTML 本地链接、术语/公式/状态静态审阅；新增文本与归档内容凭据模式检查；全部 STM32 文件保全；diff 空白、磁盘/索引/blob/清单一致性；普通推送 main 并独立抓取远端核验。
- Recovery：已核对远端 main 开工哈希，以及 backup/before-session-workflow-20261017 → df3da6870a9eebaaf215ac8cfb832cc98d681567、reference/archive-20260807 → a0669d7e36e54e78970e8162b0bc47334221c6c5。回滚可 revert 本轮提交；不改写历史，不覆盖原始资料。
- Exclusions：不生成/编译/烧录、不使能电机、不开展人体试验、不运行历史脚本或重做 HTML、不重新开展文献有效性核验、不安装/探测工具链；当前环境检测沿用有日期的历史证据，不宣称本轮验证。
- Git Delivery：先完成检查和文档对齐，再按清单精确暂存、提交与推送；最终哈希及远端状态以实际查询为准。

## 2026-10-17 — 思路讨论接手实施与验证结论

- Delivered：原始 17 项按字节保留，接手提示词逐字节归档；当前研究基线、旧路线状态链接、ADR-010/A-011–A-015、资产索引/地图/版本记录、05/06 层级和 HANDOFF 已对齐。P9.02 不再默认把 Hopf x 映射当作当前研究算法；实施前仍需冻结具体契约。
- Validation：17 项原资料与附件哈希一致、1672 个 STM32 文件不变、旧研究笔记去除新增状态说明后与开工正文完全一致；18 项导入清单核验通过；ZIP 14 个成员与解压文件一致，5 SVG XML/5 PNG chunk-CRC 通过；54 份 Markdown 的 257 个本地链接存在，Files 索引覆盖 75 项（自身除外）；两份 HTML 无本地 href/src 依赖；新增资料/ZIP/提示词常见凭据模式扫描无命中；git diff --check 退出 0。
- Static Review：固定 M/K=0、D0 上界而非均值、A 与 lambda 不同、步/跨步换算、Hopf 径向/相位参数和范围、D_raw/D_applied、PBO 试次间与 V1 非人体边界已区分；参考控制链的反馈/使能/超时差距仅登记。
- Drift：Files/README 原 DOCX 数为 6，按上一交付的 sdk.docx 修正为 7；两份 HTML 对称余弦与新窗口不同，添加说明但不改演示。旧日志保留。
- Evidence Limits：未联网核验导师稿文献，未做浏览器渲染、固件生成/编译/主机算法测试/烧录/台架或人体实验；工具链本轮未重跑，Phase 1 保持 Partially complete，05/06 保持 Pending。
- Delivery：本地检查完成后刷新全量清单、精确暂存、普通推送并独立核验远端，实际交付以 Git 查询为准；未生成自身提交哈希前不预填成功。

- 收尾发现：原始 SVG/HTML 行尾空白与导师稿末尾空行触发首次 staged diff 检查；为保全原文，新增 .gitattributes 的窄范围空白检查例外及原始目录换行保全规则。诊断与复测见 LOG，未修改资产字节。

- 暂存复测通过：41 个精确路径，1795 个普通文件与索引/blob 一致，清单 1794 项，untracked/ignored/未暂存差异均为 0，staged diff 检查退出 0；追加记录后再次刷新清单再提交。

## 2026-10-17 — 推送后远端核验与 Finder 状态补备份 / 计划

- Baseline：研究资料提交 36a9b707f08548624181d99412822bffe26db6ac 已普通推送，本地 HEAD、origin/main 与 ls-remote main 相同；不再将 bc08d08 视为最新交付。
- Scope：独立远端核验时发现根 .DS_Store 再次变化，并新出现 Files/start_researching/.DS_Store；按既有全量备份约定保留，补入资产索引、清单、日志和交接，不改原始研究资料。
- Verification Adjustment：全量独立抓取包含大量旧二进制，下载缓慢，主动终止本任务的冗余下载；改为独立抓取远端 commit/完整树（blob:none），逐文件比较远端 blob ID 与本地原字节按 Git 格式计算的 SHA-1，结合既有本地 blob 与 SHA-256 清单验证。此方式不宣称重新下载了全部远端 blob 内容。
- Tests：更新后磁盘/索引/清单核对，原始资产保全，远端树/逐文件 blob ID、untracked/ignored；精确补交与普通推送。回滚为 revert 本补交；不修改固件，不清理未知文件。

- 补备份检查结论：8 个精确路径，1796 个普通文件与索引/blob 一致，1795 项 SHA-256 清单；258 本地链接/76 资产条目有效，原始资料与固件不变；untracked/ignored/未暂存差异为 0。阶段完成度不变，补交后再核验远端。
