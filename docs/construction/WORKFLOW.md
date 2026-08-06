# 施工工作流

## 1. 开工前检查

```bash
pwd
git rev-parse --show-toplevel
git status --short --branch
git remote -v
git log --oneline --decorate -8
```

然后阅读必读文档、确认当前阶段、检查不明改动，并在 `DEV_PROGRESS.md` 追加阶段计划。没有已推送基线时先执行备份，不碰代码。

## 2. 阶段开工计划

必须写明：Objective、Affected Layers、Repository State、Planned Files、Tests、Git Baseline、Backup Branch、Rollback Plan、Acceptance Criteria、Explicit Exclusions。

## 3. 实施微循环

```text
一个内聚小改动 → 最近的静态/单元/构建检查 → 记录结果 → 下一个改动
```

- 先建立可观测性，再接通危险输出。
- 先测试纯逻辑，再接目标板，再接台架。
- 一旦现实与文档不一致，先判断是实现漂移还是需求应变更；不可静默选择。
- 遇到失败保留第一份错误文本，说明根因、修复和复测。

## 4. 阶段收尾

1. 运行阶段规定的完整测试集。
2. 更新对应层级文档、`LOG.md`、`DEV_PROGRESS.md`、`HANDOFF.md`。
3. 检查 `git diff --check` 和工作树，确认没有密钥、缓存或无关文件。
4. 精确 `git add <file...>`，审阅 staged diff。
5. 创建单一目的提交并推送。
6. 核对远端哈希/分支；报告 Complete、Partially complete 或 Blocked。

## 5. 变更控制

- 范围变化：先更新产品需求/施工计划和决策记录。
- 架构变化：先更新架构/分层契约，再实施。
- 参数变化：注明来源、单位、旧值、新值和验证证据。
- 生成代码变化：保留 `.ioc` 事实源，记录工具版本，禁止只提交不可复现的生成差异。

## 6. 交接要求

HANDOFF 必须让完全不了解聊天历史的人回答：现在在哪个仓库和分支、最近验证了什么、哪些未完成、下一条命令是什么、有哪些风险、如何回滚。
