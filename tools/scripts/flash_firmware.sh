#!/bin/bash

# ========================================
# NANO100B OTA 固件烧录脚本
# ========================================
# 功能说明：
# 1. 自动编译项目源码
# 2. 打包生成固件文件
# 3. 通过J-Link烧录到NANO100B开发板
# 4. 烧录完成后自动重启开发板
# 5. 重启成功会听到3声蜂鸣声
# ========================================

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
NC='\033[0m' # No Color

# 项目配置
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
FIRMWARE_DIR="${BUILD_DIR}/firmware"
LOG_FILE="${BUILD_DIR}/flash.log"

# 目标芯片配置
TARGET_CHIP="NANO100B"
FLASH_SIZE="0x80000"     # 512KB Flash
RAM_SIZE="0x10000"       # 64KB RAM
FLASH_BASE="0x00000000"
RAM_BASE="0x20000000"

# J-Link配置
JLINK_SPEED="4000"
JLINK_INTERFACE="SWD"

# 打印函数
print_header() {
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}========================================${NC}"
}

print_step() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')] $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_progress() {
    echo -e "${MAGENTA}📊 $1${NC}"
}

# 检查依赖工具
check_dependencies() {
    print_step "检查烧录依赖工具..."
    
    # 检查J-Link是否安装
    if ! command -v JLinkExe &> /dev/null; then
        print_error "J-Link软件未安装，请运行: brew install --cask segger-jlink"
        exit 1
    fi
    
    # 检查PlatformIO CLI
    if ! command -v pio &> /dev/null; then
        print_error "PlatformIO CLI未安装，请运行: pip install platformio"
        exit 1
    fi
    
    # 检查设备连接
    if ! lsusb 2>/dev/null | grep -i "segger\|jlink" &> /dev/null; then
        if ! system_profiler SPUSBDataType 2>/dev/null | grep -i "segger\|jlink" &> /dev/null; then
            print_warning "未检测到J-Link设备，请检查硬件连接"
        fi
    fi
    
    print_success "依赖工具检查完成"
}

# 清理构建目录
clean_build() {
    print_step "清理构建目录..."
    
    cd "$PROJECT_ROOT"
    
    # 清理PlatformIO构建文件
    if [ -d ".pio" ]; then
        rm -rf .pio
        print_progress "已清理 .pio 目录"
    fi
    
    # 清理构建目录
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_progress "已清理 build 目录"
    fi
    
    # 创建构建目录
    mkdir -p "$BUILD_DIR"
    mkdir -p "$FIRMWARE_DIR"
    
    print_success "构建目录清理完成"
}

# 编译项目
compile_project() {
    print_step "开始编译项目..."
    
    cd "$PROJECT_ROOT"
    
    # 显示编译进度
    print_progress "正在编译源代码... (预计需要1-2分钟)"
    
    # 尝试使用PlatformIO编译
    print_progress "尝试使用PlatformIO编译..."
    if pio run -e nano100b --verbose > "$LOG_FILE" 2>&1; then
        print_success "PlatformIO编译成功"
        
        # 显示编译统计信息
        if [ -f ".pio/build/nano100b/firmware.elf" ]; then
            print_progress "固件统计信息："
            arm-none-eabi-size .pio/build/nano100b/firmware.elf | while read line; do
                echo -e "${WHITE}  $line${NC}"
            done
        fi
        return 0
    else
        print_warning "PlatformIO编译失败，尝试使用CMake..."
        
        # 备选：使用CMake编译
        mkdir -p build
        cd build
        
        if cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake > "$LOG_FILE" 2>&1; then
            if make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) >> "$LOG_FILE" 2>&1; then
                print_success "CMake编译成功"
                
                # 查找生成的ELF文件
                if [ -f "hy-cool-dtu.elf" ]; then
                    print_progress "固件统计信息："
                    arm-none-eabi-size hy-cool-dtu.elf | while read line; do
                        echo -e "${WHITE}  $line${NC}"
                    done
                    return 0
                fi
            fi
        fi
        
        print_error "所有编译方法都失败了"
        echo -e "${RED}编译日志：${NC}"
        tail -20 "$LOG_FILE"
        
        print_warning "可能的解决方案："
        echo -e "${YELLOW}1. 检查源代码语法错误${NC}"
        echo -e "${YELLOW}2. 确认PlatformIO平台配置正确${NC}"
        echo -e "${YELLOW}3. 安装必要的编译依赖${NC}"
        echo -e "${YELLOW}4. 检查CMake工具链配置${NC}"
        
        exit 1
    fi
}

# 打包固件
package_firmware() {
    print_step "打包固件文件..."
    
    cd "$PROJECT_ROOT"
    
    # 检查编译输出文件 - 优先检查PlatformIO输出
    ELF_FILE=""
    if [ -f ".pio/build/nano100b/firmware.elf" ]; then
        ELF_FILE=".pio/build/nano100b/firmware.elf"
        print_progress "使用PlatformIO输出: $ELF_FILE"
    elif [ -f "build/hy-cool-dtu.elf" ]; then
        ELF_FILE="build/hy-cool-dtu.elf"
        print_progress "使用CMake输出: $ELF_FILE"
    else
        print_error "未找到编译输出文件"
        print_error "预期文件位置:"
        print_error "  .pio/build/nano100b/firmware.elf (PlatformIO)"
        print_error "  build/hy-cool-dtu.elf (CMake)"
        exit 1
    fi
    
    BIN_FILE="$FIRMWARE_DIR/firmware.bin"
    HEX_FILE="$FIRMWARE_DIR/firmware.hex"
    
    # 生成二进制文件
    print_progress "生成二进制固件文件..."
    if arm-none-eabi-objcopy -O binary "$ELF_FILE" "$BIN_FILE"; then
        print_success "二进制文件生成成功"
    else
        print_error "二进制文件生成失败"
        exit 1
    fi
    
    # 生成HEX文件
    print_progress "生成HEX固件文件..."
    if arm-none-eabi-objcopy -O ihex "$ELF_FILE" "$HEX_FILE"; then
        print_success "HEX文件生成成功"
    else
        print_error "HEX文件生成失败"
        exit 1
    fi
    
    # 显示文件信息
    if [ -f "$BIN_FILE" ]; then
        FIRMWARE_SIZE=$(stat -f%z "$BIN_FILE" 2>/dev/null || stat -c%s "$BIN_FILE" 2>/dev/null)
        print_success "固件打包完成，大小: ${FIRMWARE_SIZE} 字节"
        print_progress "固件文件: $BIN_FILE"
        print_progress "HEX文件: $HEX_FILE"
        print_progress "源文件: $ELF_FILE"
    else
        print_error "固件打包失败"
        exit 1
    fi
}

# 创建J-Link烧录脚本
create_jlink_script() {
    print_step "创建J-Link烧录脚本..."
    
    JLINK_SCRIPT="$BUILD_DIR/flash_script.jlink"
    
    cat > "$JLINK_SCRIPT" << EOF
// ========================================
// J-Link 烧录脚本 for NANO100B
// ========================================

// 连接目标芯片
si ${JLINK_INTERFACE}
speed ${JLINK_SPEED}
device ${TARGET_CHIP}
connect

// 显示芯片信息
ShowInfo

// 停止CPU
halt

// 解锁Flash (如果需要)
w4 0x40000200 0x59
w4 0x40000200 0x16
w4 0x40000200 0x88

// 擦除Flash
erase

// 加载固件
loadfile $FIRMWARE_DIR/firmware.bin $FLASH_BASE

// 验证固件
verifyfile $FIRMWARE_DIR/firmware.bin $FLASH_BASE

// 重启并运行
reset
go

// 退出
qc
EOF

    print_success "J-Link烧录脚本创建完成"
}

# 执行烧录
flash_firmware() {
    print_step "开始烧录固件到开发板..."
    
    # 创建J-Link脚本
    create_jlink_script
    
    JLINK_SCRIPT="$BUILD_DIR/flash_script.jlink"
    FLASH_LOG="$BUILD_DIR/flash_output.log"
    
    print_progress "正在连接J-Link调试器..."
    print_progress "目标芯片: $TARGET_CHIP"
    print_progress "烧录地址: $FLASH_BASE"
    print_progress "接口: $JLINK_INTERFACE, 速度: $JLINK_SPEED kHz"
    
    # 执行烧录 (显示实时进度)
    echo -e "${YELLOW}📡 烧录进度:${NC}"
    
    if JLinkExe -CommanderScript "$JLINK_SCRIPT" -Log "$FLASH_LOG" 2>&1 | while read line; do
        # 过滤并显示重要信息
        if echo "$line" | grep -E "(Connecting|Connected|Erasing|Programming|Verifying|Reset|Info|Error|Warning)" &> /dev/null; then
            echo -e "${WHITE}  $line${NC}"
        elif echo "$line" | grep -E "(\[.*%\]|Loading|Writing)" &> /dev/null; then
            echo -e "${CYAN}  📊 $line${NC}"
        fi
    done; then
        # 检查烧录结果
        if grep -q "Error" "$FLASH_LOG"; then
            print_error "烧录过程中发现错误"
            echo -e "${RED}错误详情：${NC}"
            grep "Error" "$FLASH_LOG"
            exit 1
        else
            print_success "固件烧录成功完成！"
        fi
    else
        print_error "烧录过程失败"
        echo -e "${RED}详细日志：${NC}"
        cat "$FLASH_LOG"
        exit 1
    fi
}

# 重启开发板并验证
restart_and_verify() {
    print_step "重启开发板并验证..."
    
    # 再次重启确保固件运行
    RESTART_SCRIPT="$BUILD_DIR/restart_script.jlink"
    
    cat > "$RESTART_SCRIPT" << EOF
// 重启脚本
si ${JLINK_INTERFACE}
speed ${JLINK_SPEED}
device ${TARGET_CHIP}
connect
reset
go
qc
EOF

    print_progress "正在重启开发板..."
    JLinkExe -CommanderScript "$RESTART_SCRIPT" > /dev/null 2>&1
    
    sleep 2  # 等待重启完成
    
    print_success "开发板重启完成！"
    print_progress "请注意听取开发板的蜂鸣声："
    print_progress "🔊 如果重启成功，您应该听到3声 '滴滴滴' 蜂鸣声"
    
    # 尝试检测串口输出
    print_step "尝试检测串口输出..."
    
    SERIAL_PORTS=(
        "/dev/cu.usbserial*"
        "/dev/tty.usbserial*"
        "/dev/cu.SLAB_USBtoUART*"
        "/dev/tty.SLAB_USBtoUART*"
    )
    
    FOUND_PORT=""
    for pattern in "${SERIAL_PORTS[@]}"; do
        for port in $pattern; do
            if [ -e "$port" ]; then
                FOUND_PORT="$port"
                break 2
            fi
        done
    done
    
    if [ -n "$FOUND_PORT" ]; then
        print_success "发现串口: $FOUND_PORT"
        print_progress "您可以使用以下命令监控串口输出:"
        echo -e "${WHITE}  screen $FOUND_PORT 115200${NC}"
        echo -e "${WHITE}  或者: pio device monitor${NC}"
    else
        print_warning "未发现USB串口设备，请检查串口连接"
    fi
}

# 显示烧录总结
show_summary() {
    print_header "烧录完成总结"
    
    echo -e "${GREEN}🎉 固件烧录成功完成！${NC}"
    echo ""
    echo -e "${WHITE}📋 本次烧录信息：${NC}"
    echo -e "${WHITE}  • 目标芯片: $TARGET_CHIP${NC}"
    echo -e "${WHITE}  • 固件文件: $FIRMWARE_DIR/firmware.bin${NC}"
    
    if [ -f "$FIRMWARE_DIR/firmware.bin" ]; then
        FIRMWARE_SIZE=$(stat -f%z "$FIRMWARE_DIR/firmware.bin" 2>/dev/null || stat -c%s "$FIRMWARE_DIR/firmware.bin" 2>/dev/null)
        echo -e "${WHITE}  • 固件大小: $FIRMWARE_SIZE 字节${NC}"
    fi
    
    echo -e "${WHITE}  • 烧录时间: $(date '+%Y-%m-%d %H:%M:%S')${NC}"
    echo ""
    echo -e "${YELLOW}🔊 请注意：${NC}"
    echo -e "${YELLOW}  如果开发板重启成功，您应该听到3声蜂鸣声${NC}"
    echo ""
    echo -e "${CYAN}📝 下一步操作：${NC}"
    echo -e "${CYAN}  1. 检查开发板LED指示灯状态${NC}"
    echo -e "${CYAN}  2. 监控串口输出确认程序运行${NC}"
    echo -e "${CYAN}  3. 测试OTA功能是否正常${NC}"
    echo ""
    echo -e "${WHITE}📁 相关文件位置：${NC}"
    echo -e "${WHITE}  • 编译日志: $LOG_FILE${NC}"
    echo -e "${WHITE}  • 烧录日志: $BUILD_DIR/flash_output.log${NC}"
    echo -e "${WHITE}  • 固件文件: $FIRMWARE_DIR/${NC}"
}

# 主函数
main() {
    print_header "NANO100B OTA 固件烧录工具"
    
    echo -e "${WHITE}开始执行固件烧录流程...${NC}"
    echo ""
    
    # 执行烧录步骤
    check_dependencies
    echo ""
    
    clean_build
    echo ""
    
    compile_project
    echo ""
    
    package_firmware
    echo ""
    
    flash_firmware
    echo ""
    
    restart_and_verify
    echo ""
    
    show_summary
}

# 处理脚本中断
trap 'print_error "烧录过程被中断"; exit 1' INT TERM

# 确保脚本从项目根目录运行
if [ ! -f "platformio.ini" ]; then
    print_error "请从项目根目录运行此脚本"
    print_error "当前目录: $(pwd)"
    print_error "应该包含: platformio.ini"
    exit 1
fi

# 执行主函数
main "$@" 