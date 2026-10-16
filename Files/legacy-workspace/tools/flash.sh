#!/bin/bash
# flash.sh — SmartRollator 烧录脚本
# 依赖: OpenOCD ≥ 0.12, ST-Link 调试器
#
# 用法:
#   ./flash.sh                    # 烧录 build/SmartRollator.elf
#   ./flash.sh path/to/firmware.elf
#
# 环境变量:
#   OPENOCD_CFG     OpenOCD 配置 (默认: interface/stlink.cfg -f target/stm32h7x.cfg)

set -euo pipefail

FIRMWARE="${1:-build/SmartRollator.elf}"
OPENOCD_CFG="${OPENOCD_CFG:-interface/stlink.cfg -f target/stm32h7x.cfg}"

# 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== SmartRollator Flash Tool ===${NC}"

# 检查固件文件
if [ ! -f "$FIRMWARE" ]; then
    echo -e "${RED}[ERROR] Firmware not found: $FIRMWARE${NC}"
    echo "Run 'make' first to build."
    exit 1
fi

echo -e "${YELLOW}Firmware: $FIRMWARE${NC}"
echo -e "${YELLOW}Target:   STM32H723VGTx${NC}"
echo ""

# 检查 OpenOCD
if ! command -v openocd &>/dev/null; then
    echo -e "${RED}[ERROR] openocd not found. Install with: brew install openocd${NC}"
    exit 1
fi

# 检查 ST-Link
if ! ls /dev/cu.usbmodem* &>/dev/null 2>&1 && ! ls /dev/ttyACM* &>/dev/null 2>&1; then
    echo -e "${RED}[ERROR] ST-Link not detected. Check USB connection.${NC}"
    exit 1
fi

echo "Starting OpenOCD flash..."

# 烧录
openocd $OPENOCD_CFG \
    -c "init" \
    -c "reset halt" \
    -c "flash write_image erase $FIRMWARE" \
    -c "verify_image $FIRMWARE" \
    -c "reset run" \
    -c "shutdown"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}[SUCCESS] Flash completed.${NC}"
else
    echo -e "${RED}[FAILED] Flash failed. Check connections and retry.${NC}"
    exit 1
fi
