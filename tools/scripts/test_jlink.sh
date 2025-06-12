#!/bin/bash

# ========================================
# J-Link 连接测试脚本
# ========================================
# 功能: 测试J-Link与NANO100B开发板的连接
# 使用: ./tools/scripts/test_jlink.sh
# ========================================

set -e

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TEST_LOG="$PROJECT_ROOT/build/jlink_test.log"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🔗 J-Link 连接测试工具${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# 创建构建目录
mkdir -p "$PROJECT_ROOT/build"

# 检查J-Link是否安装
echo -e "${YELLOW}🔍 检查J-Link软件...${NC}"
if ! command -v JLinkExe &> /dev/null; then
    echo -e "${RED}❌ J-Link软件未安装${NC}"
    echo -e "${YELLOW}   请运行: brew install --cask segger-jlink${NC}"
    exit 1
fi

JLINK_VERSION=$(JLinkExe -? 2>&1 | grep "V[0-9]" | head -1 || echo "未知版本")
echo -e "${GREEN}✅ J-Link软件已安装: $JLINK_VERSION${NC}"
echo ""

# 检查USB设备
echo -e "${YELLOW}🔍 检查USB设备连接...${NC}"
USB_DEVICES=$(system_profiler SPUSBDataType 2>/dev/null | grep -i "segger\|jlink" || echo "")
if [ -n "$USB_DEVICES" ]; then
    echo -e "${GREEN}✅ 发现J-Link设备:${NC}"
    echo "$USB_DEVICES" | grep -E "(Product ID|Serial Number|Vendor)" | sed 's/^/  /'
else
    echo -e "${RED}⚠️  未检测到J-Link USB设备${NC}"
    echo -e "${YELLOW}   请检查:${NC}"
    echo -e "${YELLOW}   1. J-Link硬件是否连接到电脑${NC}"
    echo -e "${YELLOW}   2. USB线缆是否正常${NC}"
    echo -e "${YELLOW}   3. J-Link驱动是否正确安装${NC}"
fi
echo ""

# 创建测试脚本
echo -e "${YELLOW}📝 创建连接测试脚本...${NC}"
TEST_SCRIPT="$PROJECT_ROOT/build/test_connection.jlink"

cat > "$TEST_SCRIPT" << 'EOF'
// J-Link 连接测试脚本
// 设置接口
si SWD

// 设置速度
speed 4000

// 尝试连接NANO100B
device NANO100B
connect

// 显示芯片信息
ShowInfo

// 读取芯片ID
mem32 0xE0042000 1

// 显示CPU状态
st

// 读取程序计数器
reg PC

// 退出
qc
EOF

echo -e "${GREEN}✅ 测试脚本创建完成${NC}"
echo ""

# 执行连接测试
echo -e "${YELLOW}🔗 开始连接测试...${NC}"
echo -e "${CYAN}目标芯片: NANO100B${NC}"
echo -e "${CYAN}接口类型: SWD${NC}"
echo -e "${CYAN}通信速度: 4MHz${NC}"
echo ""

echo -e "${BLUE}📡 J-Link 输出:${NC}"
echo "----------------------------------------"

# 执行测试并显示输出
if JLinkExe -CommanderScript "$TEST_SCRIPT" -Log "$TEST_LOG" 2>&1 | tee /dev/tty | grep -q "Connected to target"; then
    echo ""
    echo "----------------------------------------"
    echo -e "${GREEN}🎉 J-Link连接测试成功！${NC}"
    echo ""
    
    # 显示关键信息
    echo -e "${CYAN}📋 连接详情:${NC}"
    
    if [ -f "$TEST_LOG" ]; then
        # 提取芯片信息
        if grep -q "Device" "$TEST_LOG"; then
            echo -e "${GREEN}  ✅ 目标芯片: $(grep "Device" "$TEST_LOG" | head -1)${NC}"
        fi
        
        # 提取连接速度
        if grep -q "Speed" "$TEST_LOG"; then
            echo -e "${GREEN}  ✅ 连接速度: $(grep "Speed" "$TEST_LOG" | head -1)${NC}"
        fi
        
        # 提取芯片ID
        if grep -q "0xE0042000" "$TEST_LOG"; then
            echo -e "${GREEN}  ✅ 芯片ID已读取${NC}"
        fi
    fi
    
    echo ""
    echo -e "${CYAN}🚀 接下来可以:${NC}"
    echo -e "${CYAN}  1. 运行烧录脚本: ./flash.sh${NC}"
    echo -e "${CYAN}  2. 使用调试功能进行程序调试${NC}"
    echo -e "${CYAN}  3. 监控串口输出查看程序运行状态${NC}"
    
else
    echo ""
    echo "----------------------------------------"
    echo -e "${RED}❌ J-Link连接测试失败${NC}"
    echo ""
    
    echo -e "${YELLOW}🔧 可能的问题和解决方案:${NC}"
    echo ""
    echo -e "${YELLOW}1. 硬件连接问题:${NC}"
    echo -e "   • 检查J-Link与开发板的连接线${NC}"
    echo -e "   • 确认SWD接口连接正确 (SWDIO, SWCLK, GND, VCC)${NC}"
    echo -e "   • 检查开发板是否正确供电${NC}"
    echo ""
    echo -e "${YELLOW}2. 软件配置问题:${NC}"
    echo -e "   • 确认目标芯片型号 (NANO100B)${NC}"
    echo -e "   • 尝试降低连接速度${NC}"
    echo -e "   • 检查是否有其他调试器软件在运行${NC}"
    echo ""
    echo -e "${YELLOW}3. 目标芯片问题:${NC}"
    echo -e "   • 芯片可能处于深度睡眠模式${NC}"
    echo -e "   • 芯片Flash可能被锁定${NC}"
    echo -e "   • 尝试按下开发板的复位按钮${NC}"
    echo ""
    
    # 显示详细日志
    if [ -f "$TEST_LOG" ]; then
        echo -e "${YELLOW}📄 详细日志信息:${NC}"
        echo "----------------------------------------"
        cat "$TEST_LOG"
        echo "----------------------------------------"
    fi
    
    exit 1
fi

echo ""
echo -e "${BLUE}📁 测试日志已保存到: $TEST_LOG${NC}"
echo -e "${BLUE}========================================${NC}" 