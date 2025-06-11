#!/bin/bash

# 硬件设备检测和测试脚本
# 检测J-Link调试器、串口设备和目标芯片

set -e

echo "🔌 冷库DTU硬件设备检测工具"
echo "============================================"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
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

# 检测J-Link调试器
check_jlink() {
    print_status "INFO" "检测J-Link调试器..."
    
    # 检查J-Link是否连接
    if system_profiler SPUSBDataType | grep -q "SEGGER\|J-Link"; then
        local jlink_info=$(system_profiler SPUSBDataType | grep -A 5 "J-Link\|SEGGER")
        print_status "OK" "J-Link调试器已连接"
        echo "$jlink_info" | sed 's/^/    /'
        
        # 检查J-Link软件
        if command -v JLinkExe >/dev/null 2>&1; then
            print_status "OK" "J-Link软件已安装"
            local jlink_version=$(JLinkExe -? 2>&1 | head -1 || echo "版本信息获取失败")
            echo "    版本: $jlink_version"
        else
            print_status "WARN" "J-Link软件未安装，请安装SEGGER J-Link软件包"
        fi
        
        return 0
    else
        print_status "ERROR" "J-Link调试器未连接"
        return 1
    fi
}

# 检测串口设备
check_serial() {
    print_status "INFO" "检测串口设备..."
    
    local serial_devices=$(ls /dev/cu.* 2>/dev/null | grep -E "(usb|serial)" || true)
    
    if [ -n "$serial_devices" ]; then
        print_status "OK" "发现串口设备:"
        echo "$serial_devices" | sed 's/^/    /'
        
        # 检测每个串口设备的详细信息
        for device in $serial_devices; do
            if [ -c "$device" ]; then
                print_status "INFO" "设备 $device 可用"
            else
                print_status "WARN" "设备 $device 不可访问"
            fi
        done
        return 0
    else
        print_status "WARN" "未检测到串口设备"
        print_status "INFO" "请检查:"
        echo "    1. USB转串口线是否正确连接"
        echo "    2. 驱动程序是否已安装"
        echo "    3. 设备是否已上电"
        return 1
    fi
}

# 检测目标芯片连接
check_target_chip() {
    print_status "INFO" "检测目标芯片连接..."
    
    if ! command -v JLinkExe >/dev/null 2>&1; then
        print_status "ERROR" "J-Link软件未安装，无法检测目标芯片"
        return 1
    fi
    
    # 创建临时J-Link脚本
    local temp_script="/tmp/jlink_test.jlink"
    cat > "$temp_script" << EOF
connect
NANO100BE
S
4000
h
q
EOF
    
    print_status "INFO" "尝试连接NANO100BE芯片..."
    
    # 执行J-Link连接测试
    if timeout 10s JLinkExe -CommanderScript "$temp_script" >/dev/null 2>&1; then
        print_status "OK" "目标芯片NANO100BE连接正常"
        
        # 获取芯片信息
        cat > "$temp_script" << EOF
connect
NANO100BE
S
4000
mem 0x00000000 4
q
EOF
        
        print_status "INFO" "读取芯片信息..."
        local chip_info=$(timeout 5s JLinkExe -CommanderScript "$temp_script" 2>/dev/null | grep -A 5 "Reading from")
        if [ -n "$chip_info" ]; then
            echo "$chip_info" | sed 's/^/    /'
        fi
        
        rm -f "$temp_script"
        return 0
    else
        print_status "ERROR" "无法连接到目标芯片"
        print_status "INFO" "请检查:"
        echo "    1. SWD连接线是否正确 (SWDIO、SWCLK、VCC、GND)"
        echo "    2. 目标板是否已上电"
        echo "    3. 芯片型号是否为NANO100BE"
        rm -f "$temp_script"
        return 1
    fi
}

# 检测开发工具
check_dev_tools() {
    print_status "INFO" "检测开发工具..."
    
    local tools_ok=0
    
    # 检查ARM工具链
    if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
        local gcc_version=$(arm-none-eabi-gcc --version | head -1)
        print_status "OK" "ARM GCC: $gcc_version"
    else
        print_status "ERROR" "ARM工具链未安装"
        tools_ok=1
    fi
    
    # 检查CMake
    if command -v cmake >/dev/null 2>&1; then
        local cmake_version=$(cmake --version | head -1)
        print_status "OK" "CMake: $cmake_version"
    else
        print_status "WARN" "CMake未安装"
    fi
    
    # 检查PlatformIO
    if command -v pio >/dev/null 2>&1; then
        local pio_version=$(pio --version 2>/dev/null || echo "版本获取失败")
        print_status "OK" "PlatformIO: $pio_version"
    else
        print_status "WARN" "PlatformIO未安装"
    fi
    
    # 检查Python
    if command -v python3 >/dev/null 2>&1; then
        local python_version=$(python3 --version)
        print_status "OK" "Python: $python_version"
    else
        print_status "WARN" "Python3未安装"
    fi
    
    return $tools_ok
}

# 运行诊断测试
run_diagnostics() {
    print_status "INFO" "运行系统诊断..."
    
    echo ""
    echo "系统信息:"
    echo "  操作系统: $(uname -s) $(uname -r)"
    echo "  架构: $(uname -m)"
    echo "  CPU核心数: $(sysctl -n hw.ncpu 2>/dev/null || echo "未知")"
    
    echo ""
    echo "USB设备:"
    system_profiler SPUSBDataType | grep -E "Product ID|Vendor ID|Manufacturer" | head -20
    
    echo ""
    echo "串口设备:"
    ls -la /dev/cu.* 2>/dev/null || echo "  无串口设备"
}

# 生成诊断报告
generate_report() {
    local report_file="hardware_diagnosis_$(date +%Y%m%d_%H%M%S).txt"
    
    print_status "INFO" "生成诊断报告: $report_file"
    
    {
        echo "冷库DTU硬件诊断报告"
        echo "生成时间: $(date)"
        echo "============================================"
        echo ""
        
        echo "## 系统信息"
        uname -a
        echo ""
        
        echo "## USB设备"
        system_profiler SPUSBDataType
        echo ""
        
        echo "## 串口设备"
        ls -la /dev/cu.* 2>/dev/null || echo "无串口设备"
        echo ""
        
        echo "## 开发工具"
        echo "ARM GCC: $(arm-none-eabi-gcc --version 2>/dev/null || echo '未安装')"
        echo "CMake: $(cmake --version 2>/dev/null || echo '未安装')"
        echo "Python: $(python3 --version 2>/dev/null || echo '未安装')"
        echo ""
        
    } > "$report_file"
    
    print_status "OK" "诊断报告已保存: $report_file"
}

# 主检测流程
main() {
    local overall_status=0
    
    echo ""
    
    # 检测各个组件
    check_dev_tools || overall_status=1
    echo ""
    
    check_jlink || overall_status=1
    echo ""
    
    check_serial || overall_status=1
    echo ""
    
    check_target_chip || overall_status=1
    echo ""
    
    # 显示总体结果
    echo "============================================"
    if [ $overall_status -eq 0 ]; then
        print_status "OK" "所有硬件设备检测正常！可以开始开发"
    else
        print_status "WARN" "部分设备检测异常，请查看上面的详细信息"
    fi
    
    # 询问是否生成详细报告
    echo ""
    read -p "是否生成详细诊断报告？(y/N): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        run_diagnostics
        generate_report
    fi
    
    return $overall_status
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [OPTIONS]"
    echo ""
    echo "OPTIONS:"
    echo "  -j, --jlink     只检测J-Link调试器"
    echo "  -s, --serial    只检测串口设备"
    echo "  -c, --chip      只检测目标芯片"
    echo "  -t, --tools     只检测开发工具"
    echo "  -r, --report    生成详细诊断报告"
    echo "  -h, --help      显示帮助信息"
    echo ""
    echo "示例:"
    echo "  $0              # 完整检测"
    echo "  $0 --jlink      # 只检测J-Link"
    echo "  $0 --report     # 生成诊断报告"
}

# 处理命令行参数
case "${1:-}" in
    "-j"|"--jlink")
        check_jlink
        ;;
    "-s"|"--serial")
        check_serial
        ;;
    "-c"|"--chip")
        check_target_chip
        ;;
    "-t"|"--tools")
        check_dev_tools
        ;;
    "-r"|"--report")
        run_diagnostics
        generate_report
        ;;
    "-h"|"--help")
        show_help
        ;;
    "")
        main
        ;;
    *)
        echo "未知选项: $1"
        show_help
        exit 1
        ;;
esac 