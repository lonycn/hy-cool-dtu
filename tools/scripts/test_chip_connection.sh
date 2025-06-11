#!/bin/bash

# 芯片连接测试脚本
# 测试不同的NANO系列芯片型号

echo "🔧 NANO系列芯片连接测试工具"
echo "============================================"

# 常见的NANO系列芯片型号
CHIP_MODELS=(
    "NANO100AS3BN"    # 用户硬件上的实际芯片型号
    "NANO100BE"
    "NANO100B"
    "NANO100A"
    "NANO102SE"
    "NANO102B"
    "NANO103SE"
    "NANO103B"
    "NANO112"
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

test_chip_connection() {
    local chip_model=$1
    print_status "INFO" "测试芯片型号: $chip_model"
    
    # 创建临时J-Link脚本
    local temp_script="/tmp/test_chip_$chip_model.jlink"
    cat > "$temp_script" << EOF
connect
$chip_model
S
4000
h
q
EOF
    
    # 测试连接
    if timeout 8s JLinkExe -CommanderScript "$temp_script" >/dev/null 2>&1; then
        print_status "OK" "$chip_model 连接成功！"
        
        # 读取芯片信息
        cat > "$temp_script" << EOF
connect
$chip_model
S
4000
mem 0x00000000 4
q
EOF
        
        print_status "INFO" "读取芯片信息:"
        local chip_info=$(timeout 5s JLinkExe -CommanderScript "$temp_script" 2>/dev/null | grep -A 5 "Reading from")
        if [ -n "$chip_info" ]; then
            echo "$chip_info" | sed 's/^/    /'
        fi
        
        rm -f "$temp_script"
        return 0
    else
        print_status "ERROR" "$chip_model 连接失败"
        rm -f "$temp_script"
        return 1
    fi
}

# 主测试流程
main() {
    print_status "INFO" "开始测试芯片连接..."
    echo ""
    
    local success_count=0
    local total_count=${#CHIP_MODELS[@]}
    
    for chip in "${CHIP_MODELS[@]}"; do
        echo ""
        if test_chip_connection "$chip"; then
            success_count=$((success_count + 1))
            echo ""
            print_status "INFO" "找到可连接的芯片: $chip"
            print_status "INFO" "建议在配置文件中使用此芯片型号"
            echo ""
            break  # 找到第一个可连接的芯片就停止
        fi
    done
    
    echo ""
    echo "============================================"
    
    if [ $success_count -eq 0 ]; then
        print_status "WARN" "所有芯片型号测试完毕，均无法连接"
        print_status "INFO" "可能的原因:"
        echo "    1. 硬件连接问题 (SWD接线)"
        echo "    2. 目标板未上电"
        echo "    3. 芯片型号不在测试列表中"
        echo "    4. 芯片已损坏或处于特殊状态"
        echo ""
        print_status "INFO" "建议检查:"
        echo "    • VCC (3.3V) 连接"
        echo "    • GND 连接"
        echo "    • SWDIO 连接到 ICE_DAT"
        echo "    • SWCLK 连接到 ICE_CLK"
        echo "    • 目标板电源指示灯是否亮起"
        echo ""
        return 1
    else
        print_status "OK" "芯片连接测试完成！找到 $success_count 个可连接的芯片"
        return 0
    fi
}

# 显示J-Link信息
show_jlink_info() {
    print_status "INFO" "J-Link调试器信息:"
    if command -v JLinkExe >/dev/null 2>&1; then
        local version=$(JLinkExe -? 2>&1 | head -1 || echo "版本信息获取失败")
        echo "    版本: $version"
        
        # 检查USB连接
        if system_profiler SPUSBDataType | grep -q "SEGGER\|J-Link"; then
            local jlink_info=$(system_profiler SPUSBDataType | grep -A 3 "J-Link\|SEGGER" | grep "Serial Number")
            if [ -n "$jlink_info" ]; then
                echo "    $jlink_info"
            fi
        fi
    else
        print_status "ERROR" "J-Link软件未安装"
    fi
    echo ""
}

# 命令行参数处理
case "${1:-}" in
    "--info")
        show_jlink_info
        ;;
    "--help"|"-h")
        echo "用法: $0 [OPTIONS]"
        echo ""
        echo "OPTIONS:"
        echo "  --info    显示J-Link信息"
        echo "  --help    显示帮助信息"
        echo ""
        echo "测试的芯片型号:"
        for chip in "${CHIP_MODELS[@]}"; do
            echo "  • $chip"
        done
        ;;
    "")
        show_jlink_info
        main
        ;;
    *)
        echo "未知选项: $1"
        echo "使用 --help 查看帮助信息"
        exit 1
        ;;
esac 