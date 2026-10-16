#!/bin/bash
# debug.sh — SmartRollator GDB 调试启动脚本
# 依赖: OpenOCD ≥ 0.12, arm-none-eabi-gdb
#
# 用法:
#   ./debug.sh                    # 启动 GDB 调试 build/SmartRollator.elf
#   ./debug.sh path/to/firmware.elf

set -euo pipefail

FIRMWARE="${1:-build/SmartRollator.elf}"
OPENOCD_CFG="${OPENOCD_CFG:-interface/stlink.cfg -f target/stm32h7x.cfg}"
GDB="${GDB:-arm-none-eabi-gdb}"
GDB_PORT="${GDB_PORT:-3333}"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== SmartRollator Debug Tool ===${NC}"

if [ ! -f "$FIRMWARE" ]; then
    echo -e "${RED}[ERROR] Firmware not found: $FIRMWARE${NC}"
    exit 1
fi

cleanup() {
    echo ""
    echo -e "${YELLOW}Shutting down OpenOCD...${NC}"
    kill $OPENOCD_PID 2>/dev/null || true
    wait $OPENOCD_PID 2>/dev/null || true
}
trap cleanup EXIT INT TERM

echo -e "${YELLOW}Starting OpenOCD server on port $GDB_PORT...${NC}"
openocd $OPENOCD_CFG &
OPENOCD_PID=$!
sleep 2

if ! kill -0 $OPENOCD_PID 2>/dev/null; then
    echo -e "${RED}[ERROR] OpenOCD failed to start.${NC}"
    exit 1
fi

echo -e "${YELLOW}Launching GDB...${NC}"
echo ""

# 常用 GDB 命令提示
cat << 'EOF'
=== GDB Quick Reference ===
  target extended-remote :3333   (already connected)
  monitor reset halt             (reset and halt CPU)
  load                           (flash firmware)
  continue / c                   (run)
  break Controller_Run           (set breakpoint)
  print my_cpg.x                 (print CPG state)
  info threads                   (list FreeRTOS tasks)

EOF

$GDB \
    -ex "target extended-remote localhost:$GDB_PORT" \
    -ex "monitor reset halt" \
    -ex "load" \
    -ex "echo \n[INFO] Firmware loaded. CPU halted at reset vector.\n" \
    -ex "echo Type 'c' to continue or set breakpoints.\n\n" \
    "$FIRMWARE"
