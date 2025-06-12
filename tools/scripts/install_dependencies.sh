#!/bin/bash

# ========================================
# NANO100B 开发环境依赖安装脚本
# ========================================
# 功能: 自动安装和验证所有必需的开发工具
# 支持: macOS, Linux, Windows (WSL)
# ========================================

set -e

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
NC='\033[0m'

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

print_info() {
    echo -e "${WHITE}ℹ️  $1${NC}"
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        print_info "检测到操作系统: macOS"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        print_info "检测到操作系统: Linux"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
        print_info "检测到操作系统: Windows"
    else
        print_error "不支持的操作系统: $OSTYPE"
        exit 1
    fi
}

# 检查工具是否已安装
check_tool() {
    local tool=$1
    local name=$2
    
    if command -v "$tool" &> /dev/null; then
        local version=$($tool --version 2>&1 | head -1 || echo "未知版本")
        print_success "$name 已安装: $version"
        return 0
    else
        print_warning "$name 未安装"
        return 1
    fi
}

# 安装 Homebrew (macOS)
install_homebrew() {
    if [[ "$OS" == "macos" ]]; then
        print_step "检查 Homebrew..."
        if ! command -v brew &> /dev/null; then
            print_step "安装 Homebrew..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            print_success "Homebrew 安装完成"
        else
            print_success "Homebrew 已安装"
        fi
    fi
}

# 安装 ARM 工具链
install_arm_toolchain() {
    print_step "安装 ARM 工具链..."
    
    case "$OS" in
        "macos")
            if ! check_tool "arm-none-eabi-gcc" "ARM GCC"; then
                brew install --cask gcc-arm-embedded
            fi
            ;;
        "linux")
            if ! check_tool "arm-none-eabi-gcc" "ARM GCC"; then
                sudo apt-get update
                sudo apt-get install -y gcc-arm-none-eabi
            fi
            ;;
        "windows")
            print_info "请手动下载安装 ARM GCC 工具链"
            print_info "下载地址: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm"
            ;;
    esac
}

# 安装 J-Link
install_jlink() {
    print_step "安装 J-Link 软件..."
    
    case "$OS" in
        "macos")
            if ! check_tool "JLinkExe" "J-Link"; then
                brew install --cask segger-jlink
            fi
            ;;
        "linux")
            if ! check_tool "JLinkExe" "J-Link"; then
                print_info "请手动下载安装 J-Link 软件"
                print_info "下载地址: https://www.segger.com/downloads/jlink/"
            fi
            ;;
        "windows")
            print_info "请手动下载安装 J-Link 软件"
            print_info "下载地址: https://www.segger.com/downloads/jlink/"
            ;;
    esac
}

# 安装 CMake
install_cmake() {
    print_step "安装 CMake..."
    
    case "$OS" in
        "macos")
            if ! check_tool "cmake" "CMake"; then
                brew install cmake
            fi
            ;;
        "linux")
            if ! check_tool "cmake" "CMake"; then
                sudo apt-get update
                sudo apt-get install -y cmake
            fi
            ;;
        "windows")
            print_info "请手动下载安装 CMake"
            print_info "下载地址: https://cmake.org/download/"
            ;;
    esac
}

# 安装 Python 和 PlatformIO
install_platformio() {
    print_step "安装 PlatformIO..."
    
    # 检查 Python
    if ! check_tool "python3" "Python3"; then
        case "$OS" in
            "macos")
                brew install python3
                ;;
            "linux")
                sudo apt-get update
                sudo apt-get install -y python3 python3-pip
                ;;
            "windows")
                print_info "请手动安装 Python3"
                print_info "下载地址: https://www.python.org/downloads/"
                return
                ;;
        esac
    fi
    
    # 安装 PlatformIO
    if ! check_tool "pio" "PlatformIO"; then
        pip3 install platformio
        print_success "PlatformIO 安装完成"
    fi
}

# 验证硬件连接
verify_hardware() {
    print_step "验证硬件连接..."
    
    # 检查 J-Link USB 设备
    case "$OS" in
        "macos")
            if system_profiler SPUSBDataType 2>/dev/null | grep -i "segger\|jlink" &> /dev/null; then
                print_success "检测到 J-Link USB 设备"
            else
                print_warning "未检测到 J-Link USB 设备"
            fi
            ;;
        "linux")
            if lsusb 2>/dev/null | grep -i "segger\|jlink" &> /dev/null; then
                print_success "检测到 J-Link USB 设备"
            else
                print_warning "未检测到 J-Link USB 设备"
            fi
            ;;
    esac
}

# 生成环境报告
generate_report() {
    print_header "环境验证报告"
    
    echo -e "${WHITE}📋 系统信息${NC}"
    echo -e "${WHITE}  操作系统: $OS${NC}"
    echo -e "${WHITE}  时间: $(date)${NC}"
    echo ""
    
    echo -e "${WHITE}🛠️  工具状态${NC}"
    
    # 检查各个工具
    check_tool "arm-none-eabi-gcc" "ARM GCC" && echo -e "${WHITE}    $(arm-none-eabi-gcc --version | head -1)${NC}"
    check_tool "cmake" "CMake" && echo -e "${WHITE}    $(cmake --version | head -1)${NC}"
    check_tool "JLinkExe" "J-Link" && echo -e "${WHITE}    $(JLinkExe -\? 2>&1 | grep "SEGGER" | head -1)${NC}"
    check_tool "pio" "PlatformIO" && echo -e "${WHITE}    $(pio --version)${NC}"
    check_tool "python3" "Python3" && echo -e "${WHITE}    $(python3 --version)${NC}"
    
    echo ""
    echo -e "${CYAN}🚀 接下来可以运行烧录脚本: ./flash.sh${NC}"
}

# 主函数
main() {
    print_header "NANO100B 开发环境安装工具"
    
    detect_os
    echo ""
    
    install_homebrew
    echo ""
    
    install_arm_toolchain
    echo ""
    
    install_jlink
    echo ""
    
    install_cmake
    echo ""
    
    install_platformio
    echo ""
    
    verify_hardware
    echo ""
    
    generate_report
}

# 执行主函数
main "$@" 