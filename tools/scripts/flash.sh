#!/bin/bash

# 固件烧录脚本
# 支持J-Link和OpenOCD两种烧录方式

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
FIRMWARE_FILE=""
FLASH_TOOL="${1:-jlink}"

echo "⚡ 冷库DTU固件烧录工具"
echo "📁 项目目录: $PROJECT_ROOT"
echo "🔧 烧录工具: $FLASH_TOOL"

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

# 查找固件文件
find_firmware() {
    print_status "INFO" "查找固件文件..."
    
    # 查找.bin文件
    local bin_files=$(find "$BUILD_DIR" -name "*.bin" 2>/dev/null || true)
    local hex_files=$(find "$BUILD_DIR" -name "*.hex" 2>/dev/null || true)
    local elf_files=$(find "$BUILD_DIR" -name "*.elf" 2>/dev/null || true)
    
    if [[ -n "$bin_files" ]]; then
        FIRMWARE_FILE=$(echo "$bin_files" | head -1)
        print_status "OK" "找到二进制文件: $FIRMWARE_FILE"
        return 0
    elif [[ -n "$hex_files" ]]; then
        FIRMWARE_FILE=$(echo "$hex_files" | head -1)
        print_status "OK" "找到HEX文件: $FIRMWARE_FILE"
        return 0
    elif [[ -n "$elf_files" ]]; then
        FIRMWARE_FILE=$(echo "$elf_files" | head -1)
        print_status "OK" "找到ELF文件: $FIRMWARE_FILE"
        return 0
    else
        print_status "ERROR" "未找到固件文件，请先编译项目"
        return 1
    fi
}

# 检查工具是否可用
check_flash_tool() {
    case "$FLASH_TOOL" in
        "jlink")
            if ! command -v JLinkExe >/dev/null 2>&1; then
                print_status "ERROR" "J-Link未安装，请安装SEGGER J-Link软件包"
                return 1
            fi
            print_status "OK" "J-Link工具可用"
            ;;
        "openocd")
            if ! command -v openocd >/dev/null 2>&1; then
                print_status "ERROR" "OpenOCD未安装，请安装OpenOCD"
                return 1
            fi
            print_status "OK" "OpenOCD工具可用"
            ;;
        *)
            print_status "ERROR" "不支持的烧录工具: $FLASH_TOOL"
            return 1
            ;;
    esac
}

# 使用J-Link烧录
flash_with_jlink() {
    print_status "INFO" "使用J-Link烧录固件..."
    
    # 创建J-Link脚本
    local jlink_script="/tmp/flash_firmware.jlink"
    
    cat > "$jlink_script" << EOF
connect
NANO100SD3BN
S
4000
h
loadfile $FIRMWARE_FILE 0x08008000
r
g
q
EOF
    
    print_status "INFO" "执行J-Link烧录命令..."
    if JLinkExe -CommanderScript "$jlink_script"; then
        print_status "OK" "固件烧录成功！"
        rm -f "$jlink_script"
        return 0
    else
        print_status "ERROR" "固件烧录失败"
        rm -f "$jlink_script"
        return 1
    fi
}

# 使用OpenOCD烧录
flash_with_openocd() {
    print_status "INFO" "使用OpenOCD烧录固件..."
    
    local openocd_cfg="/tmp/openocd_flash.cfg"
    
    cat > "$openocd_cfg" << EOF
source [find interface/jlink.cfg]
source [find target/nano100.cfg]

init
reset halt
flash write_image erase $FIRMWARE_FILE 0x08008000
reset run
shutdown
EOF
    
    print_status "INFO" "执行OpenOCD烧录命令..."
    if openocd -f "$openocd_cfg"; then
        print_status "OK" "固件烧录成功！"
        rm -f "$openocd_cfg"
        return 0
    else
        print_status "ERROR" "固件烧录失败"
        rm -f "$openocd_cfg"
        return 1
    fi
}

# 验证烧录结果
verify_flash() {
    print_status "INFO" "验证烧录结果..."
    
    case "$FLASH_TOOL" in
        "jlink")
            local verify_script="/tmp/verify_flash.jlink"
            cat > "$verify_script" << EOF
connect
NANO100SD3BN
S
4000
verifyfile $FIRMWARE_FILE 0x08008000
q
EOF
            
            if JLinkExe -CommanderScript "$verify_script" >/dev/null 2>&1; then
                print_status "OK" "固件验证成功"
                rm -f "$verify_script"
                return 0
            else
                print_status "WARN" "固件验证失败"
                rm -f "$verify_script"
                return 1
            fi
            ;;
        *)
            print_status "INFO" "跳过验证步骤"
            return 0
            ;;
    esac
}

# 显示烧录信息
show_flash_info() {
    print_status "INFO" "烧录信息摘要:"
    echo "  固件文件: $(basename "$FIRMWARE_FILE")"
    echo "  文件大小: $(ls -lh "$FIRMWARE_FILE" | awk '{print $5}')"
    echo "  烧录地址: 0x08008000"
    echo "  目标芯片: NANO100BE"
    echo "  烧录工具: $FLASH_TOOL"
}

# 重置设备
reset_device() {
    print_status "INFO" "重置设备..."
    
    case "$FLASH_TOOL" in
        "jlink")
            local reset_script="/tmp/reset_device.jlink"
            cat > "$reset_script" << EOF
connect
NANO100SD3BN
S
4000
r
g
q
EOF
            JLinkExe -CommanderScript "$reset_script" >/dev/null 2>&1
            rm -f "$reset_script"
            ;;
    esac
    
    print_status "OK" "设备重置完成"
}

# 主烧录流程
main() {
    echo ""
    
    # 检查工具
    if ! check_flash_tool; then
        exit 1
    fi
    
    # 查找固件
    if ! find_firmware; then
        exit 1
    fi
    
    # 显示信息
    show_flash_info
    echo ""
    
    # 确认烧录
    read -p "确认烧录固件到设备？(y/N): " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_status "INFO" "取消烧录"
        exit 0
    fi
    
    # 执行烧录
    case "$FLASH_TOOL" in
        "jlink")
            if flash_with_jlink; then
                verify_flash
                reset_device
            else
                exit 1
            fi
            ;;
        "openocd")
            if flash_with_openocd; then
                print_status "OK" "烧录完成"
            else
                exit 1
            fi
            ;;
    esac
    
    echo ""
    print_status "OK" "固件烧录流程完成！"
    print_status "INFO" "设备应该会自动重启并运行新固件"
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [FLASH_TOOL] [OPTIONS]"
    echo ""
    echo "FLASH_TOOL:"
    echo "  jlink     使用J-Link烧录 (默认)"
    echo "  openocd   使用OpenOCD烧录"
    echo ""
    echo "OPTIONS:"
    echo "  --verify  仅验证已烧录的固件"
    echo "  --reset   仅重置设备"
    echo "  --info    显示固件信息"
    echo "  --help    显示帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                # 使用J-Link烧录"
    echo "  $0 openocd        # 使用OpenOCD烧录"
    echo "  $0 --verify       # 验证固件"
    echo "  $0 --reset        # 重置设备"
}

# 处理命令行参数
case "${1:-}" in
    "--verify")
        find_firmware && verify_flash
        ;;
    "--reset")
        reset_device
        ;;
    "--info")
        find_firmware && show_flash_info
        ;;
    "--help"|"-h")
        show_help
        ;;
    "")
        main
        ;;
    *)
        main
        ;;
esac 