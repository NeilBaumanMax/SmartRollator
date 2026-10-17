# 参考材料

本目录集中存放对话产生的研究说明、展示页、图表及文档生成工具；STM32 工程保留在仓库根目录原有位置。正式施工入口仍为 [docs/INDEX.md](../docs/INDEX.md)。

完整文件导航见 [ASSET_INDEX.md](ASSET_INDEX.md)；DOCX 同主题版本收敛与旧文件恢复见 [DUPLICATES.md](DUPLICATES.md)。

## 内容

| 位置 | 内容 |
|---|---|
| DOCX（共 7 份） | 原 6 份 Hopf/交接资料及 sdk.docx（SDK/VRPN 参考）；不作为当前验证证据 |
| [project_logic_map.html](project_logic_map.html) | 工程逻辑展示页 |
| [hopf_cpg_interactive_demo.html](hopf_cpg_interactive_demo.html) | Hopf 参数与控制链路交互演示 |
| `generated_docs/` | 生成的参数/极限环报告、配图和本地预览 |
| `diagrams/` | 研究与实验设计图表 |
| [handover/SmartRollator项目交接与Git重建指南.docx](handover/SmartRollator项目交接与Git重建指南.docx) | 历史交接 Word 版本 |
| `tools/` | 五个文档生成 Python 脚本和字体配置 |
| `research/` | [当前相位/PBO 研究基线](research/GAIT_PHASE_PBO_ADMITTANCE_BASELINE.md)与保留原文的历史力耦合路线 |
| [start_researching/](start_researching/README.md) | 网页讨论原始 17 项资产、接手提示词与归档说明；HTML 仍为历史对称余弦演示 |
| `local-state/` | 上层目录原始元数据；工作区其他状态文件按原路径备份 |

## 从另一工作区补入的资料

- [硬件手册](hardware/README.md)：力传感器采集器、DM-MC-Board02 电机开发板 PDF。
- [Hopf 振荡器文档](<Hopf 振荡器.docx>)：另一工作区独有的 Word 文档。
- [历史开发资料与工具模板](legacy-workspace/README.md)：架构/接口/数据流、SVG 图示、串口及调试脚本、单元/HITL 测试和 GCC 构建模板；目录内标明适配缺口。

## 路径迁移

- 根目录 DOCX、HTML → `Files/` 下同名文件。
- `generated_docs/`、`diagrams/` → `Files/` 下同名目录，内部层级不变。
- `tools/*.py`、`tools/fontconfig-cjk.conf` → `Files/tools/`；工程工具 `tools/check_toolchain.sh` 留在原处。
- `docs/handover/*.docx` → `Files/handover/`；交接 Markdown 留在原处。
- `docs/research/` → `Files/research/`，内容原样保留。

历史交接正文和根目录 `reference_manifest.sha256` 保留原始路径作为历史证据；查找参考材料时使用以上映射。

## 生成脚本

需要明确重生成时，可从仓库根目录调用 `python3 Files/tools/<脚本名>.py`；路径依据脚本位置解析。现有报告可能含人工修订，先备份目标并核对生成器版本，不能把重新生成等同为保留最新内容。脚本依赖原有 Python 文档/绘图库与字体环境，本次只检查语法及路径，不重新生成或覆盖报告。

- `build_hopf_parameter_doc.py`、`build_limit_cycle_doc.py` 输出到 `Files/generated_docs/`。
- `generate_hopf_parameter_doc.py` 是已淘汰应用版报告的历史生成器，源码保留，不推荐执行；运行会重新生成被移除的旧 DOCX。
- `generate_experiment_design_doc.py` 的新输出位置是 `Files/`，不再写到个人 Documents 目录；工作区外的既有报告未移动。
- `build_project_handover_doc.py` 读取 `docs/handover/` 中的两份 Markdown，输出到 `Files/handover/`。

后续对话生成的展示或参考文件继续放到本目录。固件源码、厂商库、启动文件、Keil 工程及其依赖不能按扩展名批量移入这里。
