#!/bin/bash

# 冷库DTU项目完整性测试脚本
# 验证开发环境配置和项目结构

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ERRORS=0
WARNINGS=0

echo "🧪 冷库DTU项目完整性测试"
echo "📁 项目目录: $PROJECT_ROOT"
echo "📅 测试时间: $(date)"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 记录错误
record_error() {
    echo -e "${RED}❌ $1${NC}"
    ((ERRORS++))
}

# 记录警告
record_warning() {
    echo -e "${YELLOW}⚠️ $1${NC}"
    ((WARNINGS++))
}

# 记录成功
record_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

# 记录信息
record_info() {
    echo -e "${BLUE}ℹ️ $1${NC}"
}

# 检查工具链
check_toolchain() {
    echo ""
    echo "🔧 检查工具链..."
    
    # ARM GCC
    if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
        VERSION=$(arm-none-eabi-gcc --version | head -n1)
        record_success "ARM GCC: $VERSION"
    else
        record_error "ARM GCC 未安装"
    fi
    
    # CMake
    if command -v cmake >/dev/null 2>&1; then
        VERSION=$(cmake --version | head -n1)
        record_success "CMake: $VERSION"
    else
        record_warning "CMake 未安装 (可选)"
    fi
    
    # PlatformIO
    if command -v pio >/dev/null 2>&1; then
        VERSION=$(pio --version)
        record_success "PlatformIO: $VERSION"
    else
        record_warning "PlatformIO 未安装 (可选)"
    fi
    
    # J-Link
    if command -v JLinkExe >/dev/null 2>&1; then
        record_success "J-Link 软件已安装"
    else
        record_warning "J-Link 软件未安装 (调试需要)"
    fi
    
    # OpenOCD
    if command -v openocd >/dev/null 2>&1; then
        VERSION=$(openocd --version 2>&1 | head -n1)
        record_success "OpenOCD: $VERSION"
    else
        record_warning "OpenOCD 未安装 (可选调试器)"
    fi
    
    # Python
    if command -v python3 >/dev/null 2>&1; then
        VERSION=$(python3 --version)
        record_success "Python: $VERSION"
    else
        record_warning "Python3 未安装 (测试工具需要)"
    fi
    
    # clang-format
    if command -v clang-format >/dev/null 2>&1; then
        VERSION=$(clang-format --version)
        record_success "clang-format: $VERSION"
    else
        record_warning "clang-format 未安装 (代码格式化需要)"
    fi
}

# 检查项目结构
check_project_structure() {
    echo ""
    echo "📁 检查项目结构..."
    
    # 必需目录
    REQUIRED_DIRS=(
        "App"
        "Nano"
        "tools/scripts"
        "tools/debug"
        "tools/test"
        ".vscode"
        "cmake"
        "docs"
        "hardware"
    )
    
    for dir in "${REQUIRED_DIRS[@]}"; do
        if [[ -d "$PROJECT_ROOT/$dir" ]]; then
            record_success "目录存在: $dir"
        else
            record_error "目录缺失: $dir"
        fi
    done
    
    # 必需文件
    REQUIRED_FILES=(
        "CMakeLists.txt"
        "platformio.ini"
        "README.md"
        "OTA_DEV.md"
        "quick_setup.sh"
        ".clang-format"
        ".vscode/settings.json"
        ".vscode/launch.json"
        ".vscode/tasks.json"
        ".vscode/extensions.json"
        "tools/scripts/build.sh"
        "tools/scripts/flash.sh"
        "tools/scripts/debug.sh"
        "tools/scripts/detect_device.sh"
        "tools/debug/openocd_nano100.cfg"
        "tools/test/test_ota.py"
    )
    
    for file in "${REQUIRED_FILES[@]}"; do
        if [[ -f "$PROJECT_ROOT/$file" ]]; then
            record_success "文件存在: $file"
        else
            record_error "文件缺失: $file"
        fi
    done
}

# 检查脚本权限
check_script_permissions() {
    echo ""
    echo "🔐 检查脚本权限..."
    
    SCRIPTS=(
        "quick_setup.sh"
        "tools/scripts/build.sh"
        "tools/scripts/flash.sh"
        "tools/scripts/debug.sh"
        "tools/scripts/detect_device.sh"
        "tools/test/test_ota.py"
    )
    
    for script in "${SCRIPTS[@]}"; do
        if [[ -x "$PROJECT_ROOT/$script" ]]; then
            record_success "脚本可执行: $script"
        else
            record_warning "脚本不可执行: $script"
            chmod +x "$PROJECT_ROOT/$script" 2>/dev/null && {
                record_info "已修复权限: $script"
            } || {
                record_error "无法修复权限: $script"
            }
        fi
    done
}

# 检查配置文件
check_config_files() {
    echo ""
    echo "⚙️ 检查配置文件..."
    
    # CMakeLists.txt
    if [[ -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
        if grep -q "NANO100SD3BN" "$PROJECT_ROOT/CMakeLists.txt"; then
            record_success "CMakeLists.txt 包含目标芯片配置"
        else
            record_warning "CMakeLists.txt 缺少目标芯片配置"
        fi
    fi
    
    # platformio.ini
    if [[ -f "$PROJECT_ROOT/platformio.ini" ]]; then
        if grep -q "nano100b" "$PROJECT_ROOT/platformio.ini"; then
            record_success "platformio.ini 包含目标板配置"
        else
            record_warning "platformio.ini 缺少目标板配置"
        fi
    fi
    
    # VS Code 配置
    if [[ -f "$PROJECT_ROOT/.vscode/settings.json" ]]; then
        if grep -q "arm-none-eabi-gcc" "$PROJECT_ROOT/.vscode/settings.json"; then
            record_success "VS Code 配置包含ARM工具链"
        else
            record_warning "VS Code 配置缺少ARM工具链"
        fi
    fi
}

# 测试构建系统
test_build_system() {
    echo ""
    echo "🔨 测试构建系统..."
    
    cd "$PROJECT_ROOT"
    
    # 测试 CMake 配置
    if command -v cmake >/dev/null 2>&1; then
        mkdir -p build_test
        cd build_test
        
        if cmake .. >/dev/null 2>&1; then
            record_success "CMake 配置成功"
        else
            record_error "CMake 配置失败"
        fi
        
        cd ..
        rm -rf build_test
    fi
    
    # 测试构建脚本
    if [[ -x "$PROJECT_ROOT/tools/scripts/build.sh" ]]; then
        if "$PROJECT_ROOT/tools/scripts/build.sh" help >/dev/null 2>&1; then
            record_success "构建脚本可用"
        else
            record_error "构建脚本执行失败"
        fi
    fi
}

# 测试硬件检测
test_hardware_detection() {
    echo ""
    echo "🔌 测试硬件检测..."
    
    if [[ -x "$PROJECT_ROOT/tools/scripts/detect_device.sh" ]]; then
        # 运行硬件检测（非阻塞）
        timeout 10s "$PROJECT_ROOT/tools/scripts/detect_device.sh" >/dev/null 2>&1 && {
            record_success "硬件检测脚本运行正常"
        } || {
            record_info "硬件检测脚本执行完成（可能无硬件连接）"
        }
    fi
}

# 测试Python工具
test_python_tools() {
    echo ""
    echo "🐍 测试Python工具..."
    
    if command -v python3 >/dev/null 2>&1; then
        # 检查Python模块
        PYTHON_MODULES=("requests" "hashlib" "pathlib")
        
        for module in "${PYTHON_MODULES[@]}"; do
            if python3 -c "import $module" 2>/dev/null; then
                record_success "Python模块可用: $module"
            else
                record_warning "Python模块缺失: $module"
            fi
        done
        
        # 测试OTA测试工具
        if [[ -x "$PROJECT_ROOT/tools/test/test_ota.py" ]]; then
            if python3 "$PROJECT_ROOT/tools/test/test_ota.py" --help >/dev/null 2>&1; then
                record_success "OTA测试工具可用"
            else
                record_error "OTA测试工具执行失败"
            fi
        fi
    fi
}

# 生成测试报告
generate_report() {
    echo ""
    echo "📊 测试报告"
    echo "============================================"
    echo -e "✅ 成功项目: $(($(grep -c "✅" /tmp/test_output 2>/dev/null || echo 0)))"
    echo -e "⚠️  警告项目: $WARNINGS"
    echo -e "❌ 错误项目: $ERRORS"
    
    if [[ $ERRORS -eq 0 ]]; then
        echo ""
        echo -e "${GREEN}🎉 项目配置完整，开发环境就绪！${NC}"
        echo ""
        echo "📝 接下来可以："
        echo "   1. 运行 './tools/scripts/build.sh' 构建项目"
        echo "   2. 运行 './tools/scripts/detect_device.sh' 检测硬件"
        echo "   3. 使用 Cursor/VS Code 开始开发"
        echo "   4. 运行 'python3 tools/test/test_ota.py test' 测试OTA功能"
    else
        echo ""
        echo -e "${RED}❌ 项目配置存在问题，请修复后重试${NC}"
        echo ""
        echo "🔧 建议修复步骤："
        echo "   1. 运行 './quick_setup.sh' 安装缺失工具"
        echo "   2. 检查工具链安装路径"
        echo "   3. 重新运行此测试脚本"
    fi
}

# 主函数
main() {
    # 重定向输出用于统计
    exec 5>&1
    exec 1> >(tee /tmp/test_output)
    
    check_toolchain
    check_project_structure
    check_script_permissions
    check_config_files
    test_build_system
    test_hardware_detection
    test_python_tools
    
    # 恢复输出
    exec 1>&5
    
    generate_report
    
    # 清理临时文件
    rm -f /tmp/test_output
    
    # 返回适当的退出代码
    if [[ $ERRORS -eq 0 ]]; then
        exit 0
    else
        exit 1
    fi
}

# 如果直接运行脚本
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi 