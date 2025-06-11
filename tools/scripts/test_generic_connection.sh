#!/bin/bash

# 通用ARM内核连接测试脚本

echo "🔧 通用ARM内核连接测试"
echo "============================================"

# 通用ARM内核类型
GENERIC_CORES=(
    "Cortex-M0+"
    "Cortex-M0"
    "ARM7"
    "ARM9"
    "Cortex-M3"
)

# 可能的NANO100变种
NANO_VARIANTS=(
    "NANO100"
    "NANO100A"
    "NANO100B"
    "NANO100AS"
)

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    local status=$1
    local message=$2
    case $status in
        "OK")
            echo -e "${GREEN}✅ $message${NC}"
            ;;
        "WARN")
            echo -e "${YELLOW}⚠️  $message${NC}"
            ;;
        "ERROR")
            echo -e "${RED}❌ $message${NC}"
            ;;
        "INFO")
            echo -e "${BLUE}ℹ️  $message${NC}"
            ;;
    esac
}

test_connection() {
    local target=$1
    print_status "INFO" "测试连接: $target"
    
    local temp_script="/tmp/test_$target.jlink"
    cat > "$temp_script" << EOF
connect
$target
S
4000
h
q
EOF
    
    if timeout 8s JLinkExe -CommanderScript "$temp_script" >/dev/null 2>&1; then
        print_status "OK" "$target 连接成功！"
        
        # 尝试读取芯片ID
        cat > "$temp_script" << EOF
connect
$target
S
4000
mem32 0xE0042000 1
q
EOF
        
        print_status "INFO" "读取芯片ID:"
        local chip_id=$(timeout 5s JLinkExe -CommanderScript "$temp_script" 2>/dev/null | grep "0xE0042000")
        if [ -n "$chip_id" ]; then
            echo "    $chip_id"
        fi
        
        rm -f "$temp_script"
        return 0
    else
        print_status "ERROR" "$target 连接失败"
        rm -f "$temp_script"
        return 1
    fi
}

# 尝试自动检测
auto_detect() {
    print_status "INFO" "尝试自动检测目标..."
    
    local temp_script="/tmp/auto_detect.jlink"
    cat > "$temp_script" << EOF
connect
?
S
4000
h
q
EOF
    
    print_status "INFO" "J-Link自动检测结果:"
    if timeout 10s JLinkExe -CommanderScript "$temp_script" 2>&1 | tee /tmp/jlink_output.txt; then
        # 查找可能的目标设备
        local detected=$(grep -i "Found" /tmp/jlink_output.txt || grep -i "device" /tmp/jlink_output.txt || echo "未检测到设备")
        echo "检测结果: $detected"
    fi
    
    rm -f "$temp_script" /tmp/jlink_output.txt
}

# 主测试流程
main() {
    print_status "INFO" "开始通用连接测试..."
    echo ""
    
    # 首先尝试自动检测
    auto_detect
    echo ""
    
    # 测试通用内核
    print_status "INFO" "测试通用ARM内核:"
    for core in "${GENERIC_CORES[@]}"; do
        echo ""
        if test_connection "$core"; then
            print_status "INFO" "找到兼容的内核: $core"
            print_status "INFO" "可以使用此内核类型进行开发"
            return 0
        fi
    done
    
    echo ""
    print_status "INFO" "测试NANO100变种:"
    for variant in "${NANO_VARIANTS[@]}"; do
        echo ""
        if test_connection "$variant"; then
            print_status "INFO" "找到兼容的芯片: $variant"
            print_status "INFO" "可以使用此芯片类型进行开发"
            return 0
        fi
    done
    
    echo ""
    print_status "WARN" "所有测试均失败"
    print_status "INFO" "可能需要手动指定芯片参数"
    return 1
}

# 显示调试信息
show_debug_info() {
    print_status "INFO" "调试信息:"
    echo "目标芯片: NANO100AS3BN"
    echo "ARM内核: Cortex-M0+"
    echo "Flash大小: 512KB"
    echo "SRAM大小: 32KB"
    echo ""
    
    print_status "INFO" "建议的连接参数:"
    echo "Interface: SWD"
    echo "Speed: 4000 kHz"
    echo "Voltage: 3.3V"
    echo ""
}

case "${1:-}" in
    "--debug")
        show_debug_info
        ;;
    "--help"|"-h")
        echo "用法: $0 [OPTIONS]"
        echo "OPTIONS:"
        echo "  --debug   显示调试信息"
        echo "  --help    显示帮助"
        ;;
    "")
        show_debug_info
        main
        ;;
esac 