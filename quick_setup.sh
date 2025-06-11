#!/bin/bash

# Nano DTU OTA 开发环境快速设置脚本 (Mac M4)
# 作者: AI Assistant
# 项目: Nano/modbus_ver3.4.2(3283)

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m'

echo -e "${BLUE}${BOLD}"
echo "🚀 Nano DTU OTA 开发环境快速设置 (Mac M4)"
echo "=================================================="
echo -e "${NC}"

# 检查 macOS 版本
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo -e "${RED}❌ 此脚本仅适用于 macOS${NC}"
    exit 1
fi

# 检查是否为 Apple Silicon
if [[ $(uname -m) != "arm64" ]]; then
    echo -e "${YELLOW}⚠️  警告: 检测到非 Apple Silicon 处理器${NC}"
    read -p "是否继续? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo -e "${BLUE}📋 步骤 1: 检查依赖工具${NC}"

# 检查 Homebrew
if ! command -v brew &> /dev/null; then
    echo -e "${YELLOW}📦 安装 Homebrew...${NC}"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
    echo -e "${GREEN}✅ Homebrew 已安装${NC}"
fi

echo -e "${BLUE}📋 步骤 2: 安装开发工具${NC}"

# 安装基础工具
echo -e "${YELLOW}🔧 安装基础开发工具...${NC}"
brew install git cmake make python3 curl wget

# 安装 ARM 工具链
echo -e "${YELLOW}🔧 安装 ARM 交叉编译工具链...${NC}"
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    brew tap osx-cross/arm
    brew install arm-none-eabi-gcc arm-none-eabi-binutils
else
    echo -e "${GREEN}✅ ARM 工具链已安装${NC}"
fi

# 安装调试工具
echo -e "${YELLOW}🔧 安装调试工具...${NC}"
brew install open-ocd
pip3 install pyocd

# 安装串口工具
echo -e "${YELLOW}🔧 安装串口工具...${NC}"
brew install minicom
brew install --cask coolterm

echo -e "${BLUE}📋 步骤 3: 创建项目结构${NC}"

# 创建目录结构
echo -e "${YELLOW}📁 创建目录结构...${NC}"
mkdir -p {src,include,lib,build,tools,docs,tests}
mkdir -p src/{ota,modbus,app}
mkdir -p include/{ota,modbus,app}
mkdir -p tools/{scripts,firmware}

# 创建 .gitignore
cat > .gitignore << 'EOF'
# Build files
build/
*.o
*.elf
*.hex
*.bin
*.map

# IDE files
.vscode/settings.json
.vscode/c_cpp_properties.json

# OS files
.DS_Store
Thumbs.db

# Temporary files
*.tmp
*.log
/tmp/

# Python
__pycache__/
*.pyc
EOF

echo -e "${BLUE}📋 步骤 4: 创建开发脚本${NC}"

# 创建构建脚本
mkdir -p tools/scripts
cat > tools/scripts/build.sh << 'EOF'
#!/bin/bash
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo -e "${BLUE}🔨 Nano DTU OTA 构建脚本${NC}"
echo -e "${BLUE}====================${NC}"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

if [ "$1" = "clean" ]; then
    echo -e "${BLUE}🧹 清理构建目录...${NC}"
    rm -rf *
fi

echo -e "${BLUE}⚙️  配置项目...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo -e "${BLUE}🔨 编译项目...${NC}"
make -j$(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ 构建成功!${NC}"
    ls -la *.hex *.bin 2>/dev/null || echo "等待添加源文件..."
else
    echo -e "${RED}❌ 构建失败!${NC}"
    exit 1
fi
EOF

# 创建设备检测脚本
cat > tools/scripts/detect_device.sh << 'EOF'
#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🔍 Nano DTU 设备检测${NC}"
echo -e "${BLUE}=================${NC}"

echo -e "${BLUE}📱 检查 USB 设备...${NC}"
system_profiler SPUSBDataType | grep -i "serial\|jtag\|debug\|j-link\|st-link" || echo "未找到相关 USB 设备"

echo -e "\n${BLUE}🔌 检查串口设备...${NC}"
if ls /dev/cu.* &> /dev/null; then
    echo -e "${GREEN}找到串口设备:${NC}"
    ls -la /dev/cu.* | grep -E "(usbserial|SLAB|usbmodem)"
else
    echo "未找到串口设备"
fi

echo -e "\n${BLUE}🛠️  检查调试工具...${NC}"

if command -v arm-none-eabi-gcc &> /dev/null; then
    echo -e "${GREEN}✅ ARM GCC 工具链已安装${NC}"
    arm-none-eabi-gcc --version | head -1
else
    echo -e "${RED}❌ ARM GCC 工具链未安装${NC}"
fi

if command -v openocd &> /dev/null; then
    echo -e "${GREEN}✅ OpenOCD 已安装${NC}"
else
    echo "OpenOCD 未安装"
fi

if command -v pyocd &> /dev/null; then
    echo -e "${GREEN}✅ pyOCD 已安装${NC}"
    pyocd list 2>/dev/null || echo "未检测到兼容设备"
else
    echo "pyOCD 未安装"
fi

if [ -d "/Applications/SEGGER" ] || command -v JLinkExe &> /dev/null; then
    echo -e "${GREEN}✅ J-Link 工具已安装${NC}"
else
    echo "J-Link 工具未安装"
fi

echo -e "\n${BLUE}✅ 设备检测完成${NC}"
EOF

# 创建调试脚本
cat > tools/scripts/debug.sh << 'EOF'
#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🐛 Nano DTU 串口调试${NC}"
echo -e "${BLUE}=================${NC}"

# 查找串口设备
SERIAL_PORT=$(ls /dev/cu.usbserial* 2>/dev/null | head -1)
if [ -z "$SERIAL_PORT" ]; then
    SERIAL_PORT=$(ls /dev/cu.SLAB_* 2>/dev/null | head -1)
fi
if [ -z "$SERIAL_PORT" ]; then
    SERIAL_PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)
fi

if [ -z "$SERIAL_PORT" ]; then
    echo -e "${RED}❌ 未找到串口设备!${NC}"
    echo "请检查设备连接，确保:"
    echo "1. USB 线连接正常"
    echo "2. 设备电源开启"
    echo "3. 驱动程序已安装"
    exit 1
fi

echo -e "${GREEN}🔌 找到串口: $SERIAL_PORT${NC}"
echo -e "${BLUE}📺 启动串口监控 (115200 波特率)...${NC}"
echo -e "按 ${YELLOW}Ctrl+A 然后 X${NC} 退出"
echo ""

# 使用 screen 监控串口
screen "$SERIAL_PORT" 115200
EOF

# 设置脚本权限
chmod +x tools/scripts/*.sh

echo -e "${BLUE}📋 步骤 5: 创建 VS Code 配置${NC}"

# 创建 VS Code 配置
mkdir -p .vscode

cat > .vscode/launch.json << 'EOF'
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Nano DTU (J-Link)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "jlink",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/nano-ota",
            "device": "NANO100",
            "interface": "swd",
            "runToMain": true,
            "preLaunchTask": "build"
        },
        {
            "name": "Debug Nano DTU (OpenOCD)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/nano-ota",
            "configFiles": [
                "interface/jlink.cfg",
                "target/nano100.cfg"
            ],
            "runToMain": true,
            "preLaunchTask": "build"
        }
    ]
}
EOF

cat > .vscode/tasks.json << 'EOF'
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "./tools/scripts/build.sh",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "clean",
            "type": "shell",
            "command": "./tools/scripts/build.sh",
            "args": ["clean"],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "detect device",
            "type": "shell",
            "command": "./tools/scripts/detect_device.sh",
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "debug serial",
            "type": "shell",
            "command": "./tools/scripts/debug.sh",
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "new"
            }
        }
    ]
}
EOF

cat > .vscode/extensions.json << 'EOF'
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "marus25.cortex-debug",
        "platformio.platformio-ide",
        "twxs.cmake",
        "ms-python.python"
    ]
}
EOF

echo -e "${BLUE}📋 步骤 6: 检测设备状态${NC}"
./tools/scripts/detect_device.sh

echo -e "${GREEN}${BOLD}"
echo "🎉 设置完成！"
echo "============="
echo -e "${NC}"

echo -e "${GREEN}✅ 已创建的内容:${NC}"
echo "📁 项目目录结构"
echo "🛠️  开发工具脚本"
echo "⚙️  VS Code 配置文件"
echo "📚 开发手册 (OTA_DEV.md)"

echo -e "\n${BLUE}📋 接下来的步骤:${NC}"
echo "1. 📖 阅读开发手册: ${YELLOW}cat OTA_DEV.md${NC}"
echo "2. 🔧 连接硬件设备 (参考手册第9章)"
echo "3. 📝 添加源代码文件 (参考手册第3章)"
echo "4. 🔨 编译项目: ${YELLOW}./tools/scripts/build.sh${NC}"
echo "5. 🐛 开始调试: ${YELLOW}./tools/scripts/debug.sh${NC}"

echo -e "\n${BLUE}💻 VS Code 用户:${NC}"
echo "1. 打开项目: ${YELLOW}code .${NC}"
echo "2. 安装推荐扩展 (VS Code 会自动提示)"
echo "3. 使用 ${YELLOW}Cmd+Shift+P${NC} 运行任务"
echo "4. 使用 ${YELLOW}F5${NC} 开始调试"

echo -e "\n${GREEN}🚀 开发环境设置完成！开始您的 OTA 开发之旅吧！${NC}" 