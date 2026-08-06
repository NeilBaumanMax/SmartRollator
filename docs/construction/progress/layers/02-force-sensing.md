# 02 — 力传感

- Current Baseline：旧工程有 USART2/RS485 参考实现；协议、CRC、标定和坐标仍需重新取证。
- Scope：传输、帧同步、校验、解析、零点、缩放、时间戳、有效性和错误统计。
- Output Contract：只输出 `ForceSample`，不直接调用控制算法或电机接口。
- Acceptance：金样/坏帧/分包/超时单测；实物静载荷的量纲、方向、零点和重复性证据。
- Evidence：Not established。
- Status：Pending。
