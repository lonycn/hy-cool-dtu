#!/bin/bash

# 冷库DTU项目调试脚本
# 支持J-Link和OpenOCD调试器

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
DEBUGGER="${1:-jlink}"
TARGET_CHIP="${2:-NANO100SD3BN}"
FIRMWARE="${3:-$PROJECT_ROOT/build/hy-cool-dtu.elf}"

echo "🐛 冷库DTU调试工具"
echo "📁 项目目录: $PROJECT_ROOT"
echo "🔌 调试器: $DEBUGGER"
echo "🎯 目标芯片: $TARGET_CHIP"
echo "📱 固件文件: $FIRMWARE"

# 检查固件文件
check_firmware() {
    if [[ ! -f "$FIRMWARE" ]]; then
        echo "❌ 固件文件不存在: $FIRMWARE"
        echo "💡 请先运行构建脚本: ./tools/scripts/build.sh"
        exit 1
    fi
    echo "✅ 固件文件检查通过"
}

# 检查调试器连接
check_debugger() {
    echo "🔍 检查调试器连接..."
    
    case "$DEBUGGER" in
        "jlink")
            if ! command -v JLinkExe >/dev/null 2>&1; then
                echo "❌ J-Link软件未安装，请从SEGGER官网下载"
                exit 1
            fi
            
            # 检测J-Link连接
            echo "exit" | JLinkExe -device $TARGET_CHIP -if SWD -speed 4000 -autoconnect 1 >/dev/null 2>&1 || {
                echo "❌ J-Link调试器未连接或目标芯片无法识别"
                echo "💡 请检查:"
                echo "   1. J-Link调试器USB连接"
                echo "   2. SWD连接线"
                echo "   3. 目标板电源"
                exit 1
            }
            echo "✅ J-Link调试器连接正常"
            ;;
            
        "openocd")
            if ! command -v openocd >/dev/null 2>&1; then
                echo "❌ OpenOCD未安装，请安装OpenOCD"
                exit 1
            fi
            echo "✅ OpenOCD检查通过"
            ;;
            
        *)
            echo "❌ 不支持的调试器: $DEBUGGER"
            echo "💡 支持的调试器: jlink, openocd"
            exit 1
            ;;
    esac
}

# J-Link调试
debug_jlink() {
    echo "🚀 启动J-Link调试会话..."
    
    # 创建J-Link脚本
    JLINK_SCRIPT="/tmp/debug_nano100.jlink"
    cat > "$JLINK_SCRIPT" << EOF
device $TARGET_CHIP
si SWD
speed 4000
r
h
loadfile $FIRMWARE
r
g
exit
EOF

    echo "📝 J-Link调试脚本已创建: $JLINK_SCRIPT"
    echo "🔄 加载固件并启动调试..."
    
    # 执行J-Link调试
    JLinkExe -CommanderScript "$JLINK_SCRIPT"
    
    # 启动GDB服务器（在后台）
    echo "🌐 启动J-Link GDB服务器..."
    JLinkGDBServer -device $TARGET_CHIP -if SWD -speed 4000 -port 2331 &
    GDBSERVER_PID=$!
    
    echo "🎯 GDB服务器已启动，端口: 2331"
    echo "💡 在另一个终端中运行GDB:"
    echo "   arm-none-eabi-gdb $FIRMWARE"
    echo "   (gdb) target remote localhost:2331"
    echo "   (gdb) monitor reset"
    echo "   (gdb) continue"
    
    # 等待用户中断
    echo "🛑 按任意键停止调试服务器..."
    read -n 1
    
    # 清理
    kill $GDBSERVER_PID 2>/dev/null || true
    rm -f "$JLINK_SCRIPT"
    echo "✅ 调试会话已结束"
}

# OpenOCD调试
debug_openocd() {
    echo "🚀 启动OpenOCD调试会话..."
    
    # OpenOCD配置文件
    OPENOCD_CFG="$PROJECT_ROOT/tools/debug/openocd_nano100.cfg"
    
    if [[ ! -f "$OPENOCD_CFG" ]]; then
        echo "❌ OpenOCD配置文件不存在: $OPENOCD_CFG"
        exit 1
    fi
    
    # 启动OpenOCD服务器
    echo "🌐 启动OpenOCD服务器..."
    openocd -f "$OPENOCD_CFG" &
    OPENOCD_PID=$!
    
    echo "🎯 OpenOCD服务器已启动，端口: 3333"
    echo "💡 在另一个终端中运行GDB:"
    echo "   arm-none-eabi-gdb $FIRMWARE"
    echo "   (gdb) target remote localhost:3333"
    echo "   (gdb) monitor reset halt"
    echo "   (gdb) load"
    echo "   (gdb) continue"
    
    # 等待用户中断
    echo "🛑 按任意键停止调试服务器..."
    read -n 1
    
    # 清理
    kill $OPENOCD_PID 2>/dev/null || true
    echo "✅ 调试会话已结束"
}

# RTT调试输出
start_rtt() {
    echo "📟 启动RTT调试输出..."
    
    if ! command -v JLinkRTTClient >/dev/null 2>&1; then
        echo "❌ J-Link RTT客户端未找到"
        exit 1
    fi
    
    # 启动RTT主机
    JLinkRTTLogger -device $TARGET_CHIP -if SWD -speed 4000 -RTTChannel 0 rtt_output.log &
    RTT_PID=$!
    
    # 启动RTT客户端
    echo "🔄 连接RTT通道..."
    sleep 2
    JLinkRTTClient
    
    # 清理
    kill $RTT_PID 2>/dev/null || true
    echo "✅ RTT会话已结束"
}

# 内存调试
debug_memory() {
    echo "🧠 内存调试工具..."
    
    # 创建内存调试脚本
    MEMORY_SCRIPT="/tmp/memory_debug.gdb"
    cat > "$MEMORY_SCRIPT" << EOF
# 连接目标
target remote localhost:2331
monitor reset

# 显示内存使用情况
define show_memory
    printf "=== 内存使用情况 ===\n"
    printf "堆栈指针 (SP): 0x%08x\n", \$sp
    printf "程序计数器 (PC): 0x%08x\n", \$pc
    
    # 显示主要内存区域
    printf "\n=== Flash 区域 ===\n"
    x/16wx 0x08000000
    
    printf "\n=== SRAM 区域 ===\n"
    x/16wx 0x20000000
    
    printf "\n=== 寄存器状态 ===\n"
    info registers
end

# 显示任务栈信息
define show_stack
    printf "=== 当前任务栈 ===\n"
    x/32wx \$sp-64
    printf "\n=== 栈回溯 ===\n"
    backtrace
end

# 自定义调试命令
define debug_ota
    printf "=== OTA 调试信息 ===\n"
    # 这里可以添加OTA相关的调试命令
    printf "实现OTA特定的调试逻辑\n"
end

echo "💡 可用的调试命令:\n"
echo "  show_memory  - 显示内存使用情况\n"
echo "  show_stack   - 显示栈信息\n"
echo "  debug_ota    - OTA调试信息\n"
EOF

    echo "📝 内存调试脚本已创建: $MEMORY_SCRIPT"
    echo "🚀 启动GDB内存调试..."
    
    arm-none-eabi-gdb "$FIRMWARE" -x "$MEMORY_SCRIPT"
    
    rm -f "$MEMORY_SCRIPT"
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [DEBUGGER] [TARGET_CHIP] [FIRMWARE] [OPTION]"
    echo ""
    echo "DEBUGGER:"
    echo "  jlink     J-Link调试器 (默认)"
    echo "  openocd   OpenOCD调试器"
    echo ""
    echo "TARGET_CHIP:"
    echo "  NANO100SD3BN  Nuvoton NANO100 系列 (默认)"
    echo ""
    echo "FIRMWARE:"
    echo "  固件ELF文件路径 (默认: build/hy-cool-dtu.elf)"
    echo ""
    echo "OPTION:"
    echo "  rtt       启动RTT调试输出"
    echo "  memory    内存调试模式"
    echo "  help      显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                           # 默认J-Link调试"
    echo "  $0 jlink NANO100SD3BN        # 指定芯片型号"
    echo "  $0 openocd                   # 使用OpenOCD"
    echo "  $0 jlink NANO100SD3BN build/firmware.elf rtt  # RTT输出"
    echo "  $0 memory                    # 内存调试"
}

# 主逻辑
main() {
    # 检查特殊选项
    for arg in "$@"; do
        case "$arg" in
            "help"|"-h"|"--help")
                show_help
                exit 0
                ;;
            "rtt")
                check_debugger
                start_rtt
                exit 0
                ;;
            "memory")
                check_debugger
                check_firmware
                debug_memory
                exit 0
                ;;
        esac
    done
    
    # 标准调试流程
    check_debugger
    check_firmware
    
    case "$DEBUGGER" in
        "jlink")
            debug_jlink
            ;;
        "openocd")
            debug_openocd
            ;;
        *)
            echo "❌ 不支持的调试器: $DEBUGGER"
            show_help
            exit 1
            ;;
    esac
}

# 执行主函数
main "$@" 