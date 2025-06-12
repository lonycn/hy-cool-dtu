#!/bin/bash

# ========================================
# NANO100B 快速烧录脚本
# ========================================
# 使用方法: ./flash.sh
# 功能: 一键编译并烧录固件到开发板
# ========================================

set -e

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🚀 NANO100B 快速烧录工具${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# 检查是否在项目根目录
if [ ! -f "platformio.ini" ]; then
    echo -e "${RED}❌ 错误: 请在项目根目录运行此脚本${NC}"
    echo -e "${RED}   当前目录应包含 platformio.ini 文件${NC}"
    exit 1
fi

# 检查工具依赖
echo -e "${YELLOW}🔍 检查工具依赖...${NC}"

if ! command -v pio &> /dev/null; then
    echo -e "${RED}❌ PlatformIO CLI 未安装${NC}"
    echo -e "${YELLOW}   请运行: pip install platformio${NC}"
    exit 1
fi

if ! command -v JLinkExe &> /dev/null; then
    echo -e "${RED}❌ J-Link 软件未安装${NC}"
    echo -e "${YELLOW}   请运行: brew install --cask segger-jlink${NC}"
    exit 1
fi

echo -e "${GREEN}✅ 工具依赖检查完成${NC}"
echo ""

# 调用完整的烧录脚本
echo -e "${YELLOW}📡 启动完整烧录流程...${NC}"
echo ""

if [ -x "tools/scripts/flash_firmware.sh" ]; then
    exec ./tools/scripts/flash_firmware.sh "$@"
else
    echo -e "${RED}❌ 找不到烧录脚本: tools/scripts/flash_firmware.sh${NC}"
    echo -e "${YELLOW}   请确保文件存在且有执行权限${NC}"
    exit 1
fi 