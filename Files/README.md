# 参考材料

本目录集中存放对话产生的研究说明、展示页、图表及文档生成工具；STM32 工程保留在仓库根目录原有位置。正式施工入口仍为 [docs/INDEX.md](../docs/INDEX.md)。

## 内容

| 位置 | 内容 |
|---|---|
| 本目录的 `.docx` | Hopf 应用说明、公式版本、参数说明、组会讲稿 |
| [project_logic_map.html](project_logic_map.html) | 工程逻辑展示页 |
| [hopf_cpg_interactive_demo.html](hopf_cpg_interactive_demo.html) | Hopf 参数与控制链路交互演示 |
| `generated_docs/` | 生成的参数/极限环报告、配图和本地预览 |
| `diagrams/` | 研究与实验设计图表 |
| [handover/SmartRollator项目交接与Git重建指南.docx](handover/SmartRollator项目交接与Git重建指南.docx) | 历史交接 Word 版本 |
| `tools/` | 五个文档生成 Python 脚本和字体配置 |
| `research/`（本地） | 整理前未跟踪的研究笔记；保留未提交状态，不属于本次提交内容 |

## 路径迁移

- 根目录 DOCX、HTML → `Files/` 下同名文件。
- `generated_docs/`、`diagrams/` → `Files/` 下同名目录，内部层级不变。
- `tools/*.py`、`tools/fontconfig-cjk.conf` → `Files/tools/`；工程工具 `tools/check_toolchain.sh` 留在原处。
- `docs/handover/*.docx` → `Files/handover/`；交接 Markdown 留在原处。
- `docs/research/` → `Files/research/`，内容原样保留。

历史交接正文和根目录 `reference_manifest.sha256` 保留原始路径作为历史证据；查找参考材料时使用以上映射。

## 生成脚本

可在仓库根目录运行 `python3 Files/tools/<脚本名>.py`，路径依据脚本位置解析，不依赖当前目录或个人绝对路径。脚本依赖原有 Python 文档/绘图库与字体环境，本次只检查语法及路径，不重新生成或覆盖报告。

- `build_hopf_parameter_doc.py`、`build_limit_cycle_doc.py` 输出到 `Files/generated_docs/`。
- `generate_hopf_parameter_doc.py` 输出到 `Files/`，图表位于 `Files/diagrams/`。
- `generate_experiment_design_doc.py` 的新输出位置是 `Files/`，不再写到个人 Documents 目录；工作区外的既有报告未移动。
- `build_project_handover_doc.py` 读取 `docs/handover/` 中的两份 Markdown，输出到 `Files/handover/`。

后续对话生成的展示或参考文件继续放到本目录。固件源码、厂商库、启动文件、Keil 工程及其依赖不能按扩展名批量移入这里。
