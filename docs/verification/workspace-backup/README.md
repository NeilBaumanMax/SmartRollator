# 全工作区 GitHub 备份

日期：2026-10-16；主分支：main。按用户最新要求，当前仓库根目录下的所有普通文件均纳入备份，覆盖原先的研究笔记、编译产物、IDE 配置/日志/锁文件、Word 预览及 Finder 元数据。`.git/` 是 Git 内部对象库和运行状态，不嵌套提交；提交历史通过 GitHub 正常保存。

## 上层目录归并

| 上层文件 | 当前工作区目标 | 处理 |
|---|---|---|
| Hopf 振荡器.docx | Files/Hopf 振荡器.docx | 与已有跟踪文件完全一致，归并后移除上层副本 |
| mermaid.html | Files/legacy-workspace/docs/archive/mermaid.html | 同上 |
| 力传感器采集器.pdf | Files/hardware/力传感器采集器.pdf | 同上 |
| 达妙科技DM-MC-Board02电机开发板使用说明书V1.1.pdf | Files/hardware/达妙科技DM-MC-Board02电机开发板使用说明书V1.1.pdf | 同上 |
| .DS_Store | Files/local-state/parent-folder.DS_Store | 按原始字节移动保存 |

归并前已核对源、目标及既有 Git 对象 SHA-256；未新增四份内容相同的重复资料。原路径/目标/哈希见 [parent-import.tsv](parent-import.tsv)。上层目录整理后仅保留当前工程目录；Finder 后续可能重新生成系统元数据，不影响本次快照。

## 备份范围与证据

- 开工 main/origin/main：6a5aaf7087cbc1b2267ba4b048193a0482a4f23c；远端回滚分支 backup/before-complete-workspace-20261016。
- 初次盘点 1767 个工作区普通文件；其中 158 个未跟踪（含忽略文件），合计 100203295 字节。
- 上述 158 项全部按原路径纳入；研究笔记也随仓库提供，不再只保留本地。
- 原始配置中的本地路径按用户完整备份要求保留；新增候选经过私钥/常见令牌及凭据赋值模式扫描，无命中。该扫描不是所有可能秘密的形式化保证。
- 工作区源码和原资产不做内容清理或格式化；旧 AXF/HEX、日志与对象文件不是当前源码构建通过证据。
- [files.sha256](files.sha256) 列出最终所有普通文件的仓库相对路径与 SHA-256（该清单自身除外，避免自引用；清单本身仍跟踪并用 Git 对象校验）。
- 目录导航见 [WORKSPACE_MAP](../../WORKSPACE_MAP.md) 与 [Files 全索引](../../../Files/ASSET_INDEX.md)。

## 原始格式保全

旧 Keil 输出/IDE 状态和系统元数据以 .gitattributes 的 binary 属性保存，禁止 Git 自动换行转换和文本合并。研究笔记的两处 Markdown 行尾双空格是原始硬换行，仅对该文件关闭 blank-at-eol 检查，字节不改。其余新增施工文本照常进行空白检查。

## 验收方法

比较磁盘文件路径集合与 git ls-files；未跟踪列表和忽略列表都必须为空。逐项对比磁盘字节、Git 索引/提交 blob 与哈希清单，推送后独立浅抓取远端 main 并核验全部 blob。最终具体检查结果记入 LOG；不能仅以 git status 干净推断忽略文件已备份。

这是当前时间点的全量快照。以后新建文件或运行 IDE/编译器可能改变已跟踪状态，应按同一流程补备份；不承诺尚未产生的文件已在 GitHub。
