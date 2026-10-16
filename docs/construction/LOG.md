# 施工日志

本文件只追加实际发生的工作。必须保留首次失败、修复和复测；未运行的测试明确写 `Not run`。

## 2026-08-07 — 建立施工文档体系

### Plan Replay

研究 Catnip-Forge 的 docs 导航、架构、开发、Git、进度、日志、交接和 feature construction 文档；研究 idea-to-production-vibecoding 的 docs-first、阶段计划、测试、Git 安全和交接规则；适配为嵌入式重建项目。

### Actual Changes

- 建立根级 Agent 规则与 docs 唯一索引。
- 冻结 SmartRollator V1 产品边界、非目标和成功标准。
- 建立 L0–L5 架构、接口责任和唯一安全命令出口。
- 定义 Phase 0–11 重建路线及各阶段验收/回滚点。
- 建立测试证据等级、Git/回滚、工具、决策、假设、进度与交接规范。

### Files Changed

仅新增 `AGENTS.md` 与 `docs/product/`、`docs/construction/` 下 Markdown 文档；未修改固件源码、CubeMX、Keil 或硬件配置。

### Test Log

- 文档存在性与相对链接：Passed。检查 32 个 Markdown 文件，所有相对链接均存在。
- 固件编译：Not run（排除项）。
- 目标板/台架：Not run（排除项）。

### Failures / Fixes / Retests

未观察到文档生成或链接检查执行失败。相对链接检查首次运行即通过。

### Documentation Drift

旧版 `docs/handover/` 仍作为历史参考；新施工以 `docs/INDEX.md` 和 construction 文档为准，已在索引明确区分。

### Git Status

Git baseline、branch、commit、push：Not established。本阶段未初始化 Git。

### Rollback Judgment

当前新增文档可在进入新仓库前人工备份；正式远端回滚点必须由 Phase 1 建立。

### Risks / Next Step

最大风险是硬件参数和参考工程行为尚未逐项验证。下一步只执行 Phase 1，不提前接入电机控制。

## 2026-08-07 — Phase 1 Git 双轨基线（进行中）

### Plan Replay

产品负责人指定远端 `https://github.com/NeilBaumanMax/SmartRollator.git` 并要求完善 Git 设计后全量推送。采用 `main` 干净重建主线、`reference/archive-20260807` 旧资产独立归档和 `backup/bootstrap-20260807` 回滚点的双轨方案。

### Actual Changes

- 扩充 Git 设计，冻结远端、分支拓扑、归档边界、排除规则、核验和保护建议。
- 新增仓库 README、`.gitignore` 和 ADR-006。
- 建立参考归档候选清单；可再生构建产物、IDE 用户状态、临时预览和密钥类文件不纳入 Git。

### Test Log

- 远端引用检查：`git ls-remote` 未返回引用，按空仓库处理；推送后必须再次核对。
- 敏感信息模式扫描：未发现匹配项；此结果不能替代 GitHub secret scanning。
- Markdown 相对链接：Passed，33 个 Markdown 文件的相对目标均存在。
- 归档候选：1577 个文件（清单本身除外），约 249 MiB；未发现单文件超过 100 MB。
- 固件编译、烧录、目标板和台架：Not run（本增量排除项）。

### Failures / Fixes / Retests

- 首次 SHA-256 清单生成：Failed。zsh 循环变量误用特殊变量名 `path`，覆盖命令搜索路径，产生空清单。
- 修复：改用普通变量名并重新生成；1577 项 SHA-256 逐项复核通过。
- 全参考快照 `git diff --cached --check`：Failed。旧 CubeMX/Keil 生成文件包含 CRLF，部分 Markdown 使用行末双空格；为保持参考证据不对旧源码做批量格式化。重建 `main` 的手写变更须单独通过 diff check。

### Git Status

首次提交与推送完成：

- `main` bootstrap：`f34420f913f0fa3a19a67f116f083b769815896d`。
- `backup/bootstrap-20260807`：`f34420f913f0fa3a19a67f116f083b769815896d`。
- `reference/archive-20260807`：`a0669d7e36e54e78970e8162b0bc47334221c6c5`。
- `git ls-remote --symref`：HEAD 指向 `refs/heads/main`；三条远端引用与本地一致。
- GitHub CLI：Not established（本机无 `gh`）；仓库可见性和分支保护未通过 API 核对。

### Risks / Next Step

归档提交包含厂商生成源码/库，只用于取证，不能进入 `main` 或被描述为已验证固件。Phase 1 尚缺 CubeMX/Keil 最小工程和实际构建证据，状态保持 Partially complete。

### Local Clone Failure / Fix / Retest

- 首次克隆后核验：Failed。大对象下载在工具的 30 秒返回窗口后仍在后台运行，但会话未被继续轮询，工作目录暂时只有 `.git`，`HEAD` 指向无效占位引用。
- 修复：继续抓取远端对象和引用，待 `origin/main`、bootstrap 和 reference archive 可解析后检出 `main`；终止一个重复且不再需要的 fetch 会话。
- 复测：Passed。`SmartRollator-Rebuild` 工作树干净并跟踪 `origin/main`；`git fsck --full` 退出码 0；HEAD 为 `b264fe344f998bae9f4df471dfd889743276ce81`。
- 归档复测：从 `reference/archive-20260807` 解包后，`reference_manifest.sha256` 的 1577 项全部 Passed。
- 最后一轮独立 `git ls-remote`：Failed（GitHub 443 临时连接失败）；远端最终状态仍由此前成功 push、后续成功 fetch 得到的三条 `origin/*` 引用交叉验证。网络恢复后应再次执行只读核对。

## 2026-08-07 — 正式本地仓库路径纠正

- 产品负责人指出当前工程目录缺少 `.git`；核对确认此前仓库建在旁边目录，交付入口与用户预期不一致。
- 首次 `git init -b main`：Failed，沙箱拒绝写入 `.git`，错误为 `Operation not permitted`。
- 权限修复后复测：Passed，当前目录成功输出 `Initialized empty Git repository`。
- 配置本地提交身份和 SSH 远端；`git fetch origin` 成功取得 main、bootstrap 和 reference archive。
- 本地 `main` 已安全绑定 `origin/main`，未覆盖当前工程文件；下一步把完整工程精确暂存、提交并推送。
- 精确暂存：1549 个变更文件；未发现 `.gitignore` 规定的意外构建输出进入暂存区。
- `git diff --cached --check`：Failed，81276 条输出来自旧 CubeMX/Keil 文件的 CRLF/行末空白；为保留参考证据未批量改写。
- Markdown 相对链接：Passed，33 个 Markdown 文件。
- 完整工程提交：`2cd83025946a8ab55553f834773f7f7bc52bd955`（`chore(repo): track complete SmartRollator workspace`）。
- 推送：Passed，远端 `main` 从 `97208dc` 推进到 `2cd8302`。

## 2026-08-07 — 施工文档漂移修正与 Git 文件级施工树

### Plan Replay

在正式当前仓库的独立文档分支修正仍指向旁路重建目录或“另建新仓库”的当前指令，并把 Phase 1–11 的 Git 设计细化到可独立验证、回滚的文件级微提交。

### Actual Changes

- 统一当前事实源：正式施工入口和唯一源均为当前 Git 工作树的仓库根目录。
- 新增 `GIT_CONSTRUCTION_TREE.md`：11 个阶段、66 个微提交节点，逐项定义精确文件、职责/接口、测试与回滚点。
- 在工作流和阶段模板中增加施工树节点、计划文件与接口字段。
- 保留追加式历史记录，并给两份旧交接材料增加历史状态声明，避免其被误作当前指令。
- 同步索引、总要求、总计划、Git/回滚、交接、产品需求和受影响层级页。

### Test Log

- Markdown 相对链接：Passed，检查 34 个 Markdown 文件。
- 施工树结构：Passed，识别 11 个阶段段落和 66 个微提交节点。
- 当前模型漂移扫描：Passed，排除追加式历史记录后，旧正式路径/另建仓库指令为 0 命中。
- 状态一致性：Passed，Phase 0 为 Complete、Phase 1 和 L0 为 Partially complete。
- 手写变更空白检查：`git diff --check` Passed。
- 固件编译、烧录、目标板和台架：Not run（本轮排除项）。

### Failures / Fixes / Retests

- 第一次复测脚本：Failed。zsh 数组展开把全部 Markdown 路径作为一个长文件名传给解析器，得到无效的“1 files”结果。
- 第一次修复：改为逐文件读取；链接检查正确通过 34 个文件，但结构检查标题正则未包含章节编号，脚本提前退出。
- 第二次修复：按实际 `## N. Phase N` 标题匹配，并把历史决策与当前事实扫描分开；随后全链路复测 Passed。
- 上述失败均属于验收脚本，不是文档链接、固件构建或硬件失败。未把文档生成成功解释为固件验证成功。

### Git Status

- 开工基线：`75d5609f9115c704c3d36dacc3eab1994442b126`。
- 工作分支：`docs/git-design-tree`；修改前已推送同名远端备份分支。
- 提交与最终远端哈希：完成精确暂存和推送后由命令输出核对，并在最终交付报告中给出。

### Risks / Next Step

施工树是实现契约，不是已有固件能力证明。下一步必须从 P1.02 工具链证据开始，不可跳过 P1.03 的实际构建就进入外设或电机控制阶段。

## 2026-08-07 — P1.02 工具链检测与证据格式

### Plan Replay

从 `main` 的 `5403028d255ac16d2e137454fc40513e75eb01f2` 创建并预先推送 `build/phase-1-bootstrap`，只建立只读工具检测和构建证据格式，不安装、生成或编译。

### Actual Changes

- 新增 `tools/check_toolchain.sh`：检测 CubeMX、MDK、Arm Compiler 和 STM32Cube FW_H7；缺失或版本未确认时返回非零。
- 新增 `docs/verification/TOOLCHAIN.md`：区分本机实测与参考 `.ioc` 历史声明。
- 新增 `docs/verification/builds/README.md`：冻结构建摘要命名、必需证据和结果词。
- 修正根 `AGENTS.md` 的当前阶段漂移；同步 P1.02 进度、交接和 L0 层级状态。

### Actual Environment

- Host：macOS 26.5.2，arm64。
- STM32CubeMX：Not found。
- Keil MDK：Not found。
- Arm Compiler：Not found。
- STM32Cube FW_H7 Repository：Not found。
- Reference-only candidates：FW_H7 V1.12.1、MDK-ARM V5.32；不是当前主机安装证据。

### Test Log

- `sh -n tools/check_toolchain.sh`：Passed。
- `sh tools/check_toolchain.sh`：按缺失环境契约返回 1；输出 `TOOLCHAIN_STATUS=INCOMPLETE`，四个缺失项断言 Passed。
- 输出个人绝对路径扫描：Passed，0 命中。
- Markdown 相对链接：Passed，36 个文件。
- `git diff --check`：Passed。
- CubeMX 生成、Keil 编译、烧录、目标板、台架：Not run。

### Failures / Fixes / Retests

- 首次只读盘点：Failed。zsh 展开不存在的 `/Applications/Keil*` 时产生 `no matches found`，后续 Repository 和参考元数据检查未执行。
- 修复：不再用未受控应用通配，改为显式应用候选和 `find` 名称检查。
- 复测：Passed。完整输出确认四项必需组件均未找到，并从 `.ioc` 提取到历史候选版本。

### Safety / Scope Judgment

P1.02 完成不表示工具链完整。由于脚本真实返回 INCOMPLETE，P1.03 和 P1.05 的环境门槛未满足；未修改任何固件、CubeMX 或 Keil 工程文件。

## 2026-10-16 — 工作区参考材料归入 Files

### Plan Replay / Actual Changes

用户要求优先保全 STM32 工程并分离对话参考材料。开工时 `build/phase-1-bootstrap` 与远端均为 `4f5af0a`，仅 `docs/research/` 未跟踪。先保存本地哈希快照，推送并核对 `backup/workspace-before-files-20261016`，再移动 26 个本地资产到 `Files/`。STM32 工程、工具链检测入口与历史归档清单保持原样。

3 个生成脚本修正路径：两个取消个人目录硬编码，交接生成器继续读取原 `docs/handover/` Markdown 并输出到 `Files/handover/`；另两个脚本随移动自然解析到 `Files/generated_docs/`。更新预览忽略规则、README、文档索引、施工树、进度、交接和 L0 状态。未跟踪研究笔记只移动，不纳入提交；原有两个忽略的预览和 `.DS_Store` 仍不提交。

### Test Log

- Python SHA-256 检查：1669 个受保护文件全部一致；23 个仅移动文件内容一致，3 个路径调整脚本审查通过。
- Python XML 解析 `MDK-ARM/SmartRollator.uvprojx`：49 个 FilePath 和 17 个 IncludePath 条目存在。
- Python AST/compile 与路径赋值检查：5 个生成器语法有效、输入存在、输出均位于 Files。未执行生成器，未覆盖现有报告。
- Markdown 本地链接：43 项通过；2 个 HTML 展示页无缺失本地资源引用。
- `sh -n tools/check_toolchain.sh`：Passed；未重跑环境探测，历史工具安装状态不作为本轮新事实。
- `git diff --check`：Passed。
- 首次失败/修复/复测：无，首轮检查通过。
- CubeMX 生成、Keil 编译、烧录、目标板、台架：Not run。

### Rollback / Git Delivery

回滚基线为远端 `backup/workspace-before-files-20261016` 的 `4f5af0a6059f3061b00c421e75340eca4bf92257`。本次精确暂存已跟踪资产的旧新路径和治理文档，不包含研究笔记及预览缓存；提交后推送当前阶段分支并核对远端 HEAD。历史 `reference_manifest.sha256` 不改写，其路径仍对应只读归档分支。

## 2026-10-16 — 补入另一工作区独有参考资产

### Plan / Changes

按用户要求筛选另一工作区独有且有用文件，先在 `1d79da5` 建立并核对远端 `backup/before-reference-import-20261016`。新增 23 份原始文件，按 hardware、历史 docs/archive、tools、tests 分类进入 Files；附来源相对路径与 SHA-256 清单。源目录只读，未覆盖当前任何工程文件。已有重复资产、仅带旧个人路径的生成器、重复且未适配的外层 README/.gitignore 不导入。

### Compatibility Review

旧文档/单测/Makefile 使用尚不存在的 firmware/App 布局；flash.sh 的 ST-Link 探测依赖串口名称；debug.sh 的 GDB_PORT 未配置到 OpenOCD；HITL 判定覆盖未完整。作为待适配模板保存，不放入正式 tools/Tests 或根 Makefile。mermaid.html 是 Cocoa 导出的转义源码，未宣称交互页面可用。两份手册的实物适用性仍待核对。

### Verification

- Python SHA-256：23 份源文件未变、目标内容一致，与现有资产无内容重复；既有 STM32 工程、研究笔记和其他资产未变。
- Python AST/compile：2 个 Python 脚本语法 Passed；`bash -n`：2 个 shell 脚本语法 Passed。未执行脚本。
- 5 个 SVG XML、1 个 DOCX ZIP/XML、2 个 PDF 文件头/结束标记结构检查 Passed；未重新渲染文档。
- Markdown 本地链接：72 项 Passed。
- `git diff --check`：Passed。首轮上述检查均通过，无修复复测。
- C 单元测试编译、GCC/Keil 构建、CubeMX 生成、串口、烧录、调试、HITL/台架：Not run。

### Delivery / Rollback

精确暂存新增资产、导航及施工记录，不包含 `Files/research/`。本轮单一提交在当前 `build/phase-1-bootstrap` 分支交付，提交后推送并核对远端哈希。回滚父提交 `1d79da55aa25c312103c8c84f9c74c8516df0c39`；不推进 Phase 1 工程验收。

## 2026-10-16 — 文档漂移修正、Agent 导航和 DOCX 版本收敛

### Findings / Changes

当前 main 远端为 5403028，实际施工分支为 build/phase-1-bootstrap，开工 HEAD 为 1a9f299；旧页面未充分区分。根 App/Tests/CMakeLists.txt 是规划，现有业务在 MDK-ARM，资料和旧模板在 Files。修正 AGENTS、INDEX 与开工入口，新增 WORKSPACE_MAP 和 ASSET_INDEX，重写 HANDOFF 当前摘要并更新工具链、README、Git/层级/施工树说明；历史进度、原始导入内容和清单不改写。

DOCX 共 9 份，SHA-256 均不同。应用说明三版正文高度一致、主要区别为公式修订；留 LaTeX 版。参数两版为同用途改写，较新的 generated_docs 版本包含扩展内容；按用户指令留新，旧版独有行文/图表可从备份恢复。3 份旧文件移除，6 份保留内容不变。未编辑或重生成 Word，未宣称完成视觉排版验收。

### Tests / Evidence

- DOCX OOXML 只读解析：9 份 ZIP 正文、公式对象、媒体与 core properties；排除 2013 年 python-docx 默认时间，结合内容及本轮文件时间做版本判断。依据及旧文件 SHA-256 完整列于 Files/DUPLICATES.md。
- SHA-256 前后核对：1669 个工程/工具/归档受保护文件不变；6 份保留 DOCX 不变；此前参考资产除已说明文档改动及 3 份删除外保持原样。
- 当前 Files 48 项资产（含全索引自身）字节重复组 0；ASSET_INDEX 显式覆盖其余 47 项。本地研究笔记与忽略预览单列，未静默漏掉。
- Keil XML 路径：49 个文件引用与 17 个包含目录条目全部存在。
- Markdown：154 个本地链接通过；AGENTS/INDEX/开工入口阅读顺序一致；实际目录和未来目录均有说明。
- `sh tools/check_toolchain.sh` 输出：HOST_OS=Darwin，HOST_ARCH=arm64；STM32CUBEMX、KEIL_MDK、ARM_COMPILER、STM32CUBE_FW_H7 均 NOT_FOUND；TOOLCHAIN_STATUS=INCOMPLETE，退出 1。这是缺失环境的预期返回，不是测试误报成功。
- `git diff --check`：Passed。以上首次检查通过，无失败修复重测项。
- 固件生成、编译、C 主机单测、串口、烧录和硬件验证：Not run。

### Git / Recovery

远端备份 backup/before-docs-dedup-20261016 → 1a9f299e6498579e5dbb4e9f03972e1ace270814 已核对，包含全部删除前 DOCX。精确暂存本轮文件，当前阶段分支提交后推送并核对远端。Files/research 原未跟踪笔记继续保留本地，不纳入提交。

## 2026-10-16 — 将当前状态提升为 main 开发基线

用户明确要求推送远端并以当前状态作为主分支继续开发。fetch 后旧 main/origin/main 均为 5403028，当前阶段为 ac0c0d3；rev-list 左右计数 0/4，祖先检查通过，可以快进。先建立远端 backup/main-before-promotion-20261016 保存旧 main，原阶段提交也已推送。

同步 AGENTS、README、目录地图、开工入口、HANDOFF、Git 规则、施工树及 L0 说明，消除“尚未合入 main”的当前状态声明。历史条目不重写。当前开发入口改为 main，不代表工具链或固件阶段验收完成。

最近检查：祖先关系通过；非本轮文档的全部已有文件哈希保持一致；Markdown 本地链接与 diff 空白检查通过。工程代码、参考资产、本地未跟踪研究笔记均未改动；未运行生成、编译、烧录或硬件测试。交付使用 main 快进合并与普通推送，最后核对本地 main、origin/main 和远端 main 哈希；禁止强推。

## 2026-10-16 — 上层资料归并与全量 GitHub 备份

### Plan / Changes

开工 main/origin/main 为 6a5aaf7；远端 backup/before-complete-workspace-20261016 保存该提交。盘点 1767 个普通文件、158 个原未跟踪/忽略文件，待补 100203295 字节。上层 4 份文档/HTML 与现有 Git 内容完全相同，按 SHA-256 归并；原上层 .DS_Store 移至 Files/local-state/parent-folder.DS_Store，原路径、动作、哈希记录在 parent-import.tsv。

用户最新全量备份要求覆盖原排除政策。研究笔记、旧对象/镜像/日志、IDE 状态、预览和系统元数据均按原路径纳入；仅 .git 内部 Git 数据不重复提交。全部新候选未命中私钥/常见令牌/凭据赋值扫描；现有配置路径按完整现场快照保留。全量快照不代表工具链、构建或实机验收通过。

### First Failures / Fixes / Retests

- 首次读取 git diff --cached --check 输出时，Python text=True 以 UTF-8 解码旧 Keil 内容失败：`UnicodeDecodeError: 'utf-8' codec can't decode byte 0xb5 in position 1864: invalid start byte`。改用原始 bytes 捕获，展示时采用替换解码，文件原字节不变。
- 完整空白检查返回 2，33294 行诊断；旧 Keil 文本包含历史行尾/空白。将旧输出、IDE 状态和系统元数据声明为 binary 快照，禁止自动换行/文本合并；复测仅剩研究笔记第 3、4 行的 Markdown 行尾双空格。
- 这两处是原文硬换行；只对该研究文件使用 whitespace=-blank-at-eol，保留原文。再次 git diff --cached --check 返回 0。未为消除诊断修改任何旧输出或研究内容。

### Verification / Delivery

对全部普通文件生成 files.sha256（自身由 Git blob 核验），按明确逐文件列表暂存；校验原工程/参考文件哈希、磁盘与 Git 树路径集合、提交对象内容、文档链接和 Keil 引用。最终推送 main 后独立浅抓取 GitHub main，再比较完整树及逐文件 SHA-256，确认没有未跟踪或忽略的遗漏文件。此过程不运行固件、编译、串口或硬件测试。远端核验结果由交付时实际输出确认；若失败不能报告完成。

### 原始快照后的研究链接修复

- 第一次将研究笔记也纳入全量链接检查时失败：`AssertionError: (PosixPath('Files/research/FORCE_COUPLED_HOPF_VARIABLE_ADMITTANCE.md'), '../product/PRODUCT_REQUIREMENTS.md')`。该笔记此前从 docs/research 移到 Files/research 时保留原文，末尾 5 处链接仍指向旧目录。
- 先创建原始快照提交 `677c23a`，保证研究笔记、旧构建/IDE 状态等原始字节在 Git 历史中完整保存；随后仅修正研究笔记第 558–562 行的 5 个相对链接到 ../../docs 下的原目标，研究正文未改。
- 同步本记录、备份说明与哈希清单后再复核；当前最终副本使用修复后链接，原文仍可从 677c23a 按路径恢复。

- 链接修复复测：176 个本地 Markdown 链接全部通过（包括研究笔记）；49 个 Keil 文件引用和 17 个包含目录条目存在；1724 个原工程/参考资产字节不变，原始研究笔记哈希与 677c23a 内对象一致。
