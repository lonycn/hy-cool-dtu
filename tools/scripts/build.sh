#!/bin/bash

# 冷库DTU项目构建脚本
# 支持CMake和PlatformIO两种构建方式

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_TYPE="${1:-Release}"
BUILD_SYSTEM="${2:-cmake}"

echo "🏗️  冷库DTU项目构建工具"
echo "📁 项目目录: $PROJECT_ROOT"
echo "🔨 构建类型: $BUILD_TYPE"
echo "⚙️  构建系统: $BUILD_SYSTEM"

# 检查工具链
check_toolchain() {
    echo "🔍 检查工具链..."
    
    if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
        echo "❌ arm-none-eabi-gcc 未找到，请安装ARM工具链"
        exit 1
    fi
    
    if [[ "$BUILD_SYSTEM" == "cmake" ]] && ! command -v cmake >/dev/null 2>&1; then
        echo "❌ cmake 未找到，请安装CMake"
        exit 1
    fi
    
    if [[ "$BUILD_SYSTEM" == "pio" ]] && ! command -v pio >/dev/null 2>&1; then
        echo "❌ platformio 未找到，请安装PlatformIO"
        exit 1
    fi
    
    echo "✅ 工具链检查通过"
}

# CMake构建
build_cmake() {
    echo "🔨 使用CMake构建..."
    
    cd "$PROJECT_ROOT"
    
    # 创建构建目录
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 配置项目
    echo "⚙️  配置CMake项目..."
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          ..
    
    # 编译项目
    echo "🔨 编译项目..."
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    # 显示构建结果
    echo "📊 构建结果:"
    ls -la hy-cool-dtu.*
    
    echo "✅ CMake构建完成!"
}

# PlatformIO构建
build_platformio() {
    echo "🔨 使用PlatformIO构建..."
    
    cd "$PROJECT_ROOT"
    
    # 构建项目
    echo "🔨 编译项目..."
    pio run -e nano100b
    
    # 显示构建结果
    echo "📊 构建结果:"
    ls -la .pio/build/nano100b/
    
    echo "✅ PlatformIO构建完成!"
}

# 清理构建
clean_build() {
    echo "🧹 清理构建文件..."
    
    if [[ "$BUILD_SYSTEM" == "cmake" ]]; then
        rm -rf "$BUILD_DIR"
        echo "✅ CMake构建文件已清理"
    elif [[ "$BUILD_SYSTEM" == "pio" ]]; then
        cd "$PROJECT_ROOT"
        pio run -t clean
        echo "✅ PlatformIO构建文件已清理"
    fi
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [BUILD_TYPE] [BUILD_SYSTEM] [OPTIONS]"
    echo ""
    echo "BUILD_TYPE:"
    echo "  Debug     调试版本 (默认优化级别: -Og)"
    echo "  Release   发布版本 (默认优化级别: -Os)"
    echo ""
    echo "BUILD_SYSTEM:"
    echo "  cmake     使用CMake构建 (默认)"
    echo "  pio       使用PlatformIO构建"
    echo ""
    echo "OPTIONS:"
    echo "  clean     清理构建文件"
    echo "  help      显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                    # Release + CMake"
    echo "  $0 Debug cmake        # Debug + CMake"
    echo "  $0 Release pio        # Release + PlatformIO"
    echo "  $0 clean              # 清理构建文件"
}

# 主逻辑
main() {
    case "$1" in
        "clean")
            clean_build
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            check_toolchain
            
            if [[ "$BUILD_SYSTEM" == "cmake" ]]; then
                build_cmake
            elif [[ "$BUILD_SYSTEM" == "pio" ]]; then
                build_platformio
            else
                echo "❌ 不支持的构建系统: $BUILD_SYSTEM"
                show_help
                exit 1
            fi
            ;;
    esac
}

# 执行主函数
main "$@" 