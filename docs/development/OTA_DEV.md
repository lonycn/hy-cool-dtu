# Nano DTU OTA 升级开发手册 (Mac M4)

## 项目信息

- **项目名称**: Nano/modbus_ver3.4.2(3283)
- **目标芯片**: NANO100B 系列
- **开发环境**: Mac M4 (Apple Silicon)
- **开发语言**: C/C++
- **通信协议**: Modbus RTU/TCP

## 1. Mac M4 开发环境配置

### 1.1 安装基础开发工具

```bash
# 安装 Homebrew (如果还没有安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装基础开发工具
brew install git cmake make
brew install python3 pip
brew install curl wget

# 安装 ARM 交叉编译工具链
brew tap osx-cross/arm
brew install arm-none-eabi-gcc
```

### 1.2 安装开发环境

#### 选项 1: VS Code + PlatformIO (推荐)【✔】

```bash
# 安装 VS Code
brew install --cask visual-studio-code

# 安装 Node.js (PlatformIO 依赖)
brew install node

# VS Code 扩展安装:
# - PlatformIO IDE
# - C/C++
# - Cortex-Debug
```

#### 选项 2: Keil MDK (通过虚拟机)【×】

```bash
# 安装 Parallels Desktop
brew install --cask parallels

# 在 Windows 虚拟机中安装 Keil MDK
# 下载地址: https://www.keil.com/demo/eval/arm.htm
```

### 1.3 安装调试和烧录工具

```bash
# 安装 OpenOCD
brew install open-ocd

# 安装 pyOCD
pip3 install pyocd

# 安装 J-Link 工具
# 下载 macOS 版本: https://www.segger.com/downloads/jlink/
```

### 1.4 安装串口调试工具

```bash
# 安装串口工具
brew install minicom
brew install --cask coolterm

# 或使用系统自带的 screen
```

## 2. 项目结构创建

### 2.1 创建项目目录

```bash
# 创建项目根目录
mkdir -p ~/Projects/hy-cool-dtu
cd ~/Projects/hy-cool-dtu

# 创建项目结构
mkdir -p app/{ota/{include,src,config,test}}
mkdir -p src/{include,ota/{include,src,config,test}}
mkdir -p src/{uart,sht2x,sim7600,si24r1,lcd,led,workled}
mkdir -p src/{alarm,alarm_led,key,dido,i2c,spi,adc,pwm}
mkdir -p src/{switch,lora,watchdong,bl,test_function,linker}
mkdir -p tools/{scripts,debug,test}
mkdir -p docs/{development,manuals,architecture,reports,api}
mkdir -p hardware/{pcb,schematic}
mkdir -p cmake scripts

# 创建主要配置文件
touch CMakeLists.txt platformio.ini flash.sh quick_setup.sh
touch README.md CHANGELOG.md CONTRIBUTING.md LICENSE .gitignore

# 创建工具脚本
touch tools/scripts/{build.sh,flash_firmware.sh,debug.sh,test_jlink.sh}
touch tools/debug/{jlink_config.jlink,openocd_nano100.cfg}

# 创建VS Code配置
mkdir -p .vscode
touch .vscode/{settings.json,launch.json,tasks.json,extensions.json}
```

### 2.2 配置 PlatformIO 项目

```ini
# platformio.ini
[env:nano100]
platform = nuvoton-m0
board = nano100
framework =

upload_protocol = jlink
debug_tool = jlink

build_flags =
    -DOTA_ENABLE=1
    -DMODBUS_VERSION=3.4.2
    -DDTU_VERSION=3283
    -DDEBUG=1

lib_deps =

monitor_speed = 115200
monitor_port = /dev/cu.usbserial*
```

### 2.3 配置 CMake 构建

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 设置交叉编译工具链
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

project(nano-ota VERSION 3.4.2)

# CPU 配置
set(CPU_FLAGS "-mcpu=cortex-m0 -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -fdata-sections -ffunction-sections")

# 包含目录
include_directories(
    include
    include/ota
    include/modbus
    include/app
)

# 源文件
file(GLOB_RECURSE SOURCES "src/*.c")

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 生成输出文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
```

## 3. OTA 模块实现

### 3.1 OTA 配置头文件

```c
// include/ota/ota_config.h
#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// OTA 版本信息
#define OTA_VERSION                "1.0.0"
#define DTU_VERSION                "3.4.2(3283)"

// Flash 配置 (NANO100B 512KB)
#define FLASH_BASE_ADDR            0x00000000
#define BOOTLOADER_SIZE            0x8000      // 32KB
#define APP1_START                 0x00008000  // 32-256KB
#define APP1_SIZE                  0x38000     // 224KB
#define APP2_START                 0x00040000  // 256-480KB
#define APP2_SIZE                  0x38000     // 224KB
#define CONFIG_START               0x00078000  // 480-512KB
#define CONFIG_SIZE                0x8000      // 32KB

// OTA 参数
#define OTA_MAX_FIRMWARE_SIZE      (224 * 1024)
#define OTA_CHUNK_SIZE             1024
#define OTA_RETRY_COUNT            3
#define OTA_TIMEOUT_MS             30000
#define OTA_CHECK_INTERVAL         3600

// Modbus 寄存器地址
#define OTA_REG_STATE              200
#define OTA_REG_PROGRESS           201
#define OTA_REG_CONTROL            202
#define OTA_REG_VERSION_NEW        203
#define OTA_REG_ERROR_CODE         204

#endif // OTA_CONFIG_H
```

### 3.2 OTA 管理器

```c
// include/ota/ota_manager.h
#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include "ota_config.h"

// OTA 状态枚举
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_INSTALLING,
    OTA_STATE_COMPLETED,
    OTA_STATE_FAILED,
    OTA_STATE_ROLLBACK
} ota_state_t;

// OTA 控制命令
typedef enum {
    OTA_CMD_CHECK = 1,
    OTA_CMD_START = 2,
    OTA_CMD_CANCEL = 3,
    OTA_CMD_ROLLBACK = 4
} ota_cmd_t;

// OTA 信息结构
typedef struct {
    char version[16];
    uint32_t size;
    char url[256];
    char md5[33];
    uint32_t timestamp;
} ota_info_t;

// API 函数
int ota_init(void);
int ota_check_update(ota_info_t* info);
int ota_start_download(const ota_info_t* info);
ota_state_t ota_get_state(void);
int ota_get_progress(void);
void ota_process(void);

#endif // OTA_MANAGER_H
```

```c
// src/ota/ota_manager.c
#include "ota/ota_manager.h"
#include <stdio.h>
#include <string.h>

static ota_state_t g_ota_state = OTA_STATE_IDLE;
static int g_ota_progress = 0;
static ota_info_t g_ota_info = {0};

int ota_init(void) {
    printf("[OTA] 初始化 OTA 模块 v%s\n", OTA_VERSION);
    printf("[OTA] DTU 版本: %s\n", DTU_VERSION);

    g_ota_state = OTA_STATE_IDLE;
    g_ota_progress = 0;

    return 0;
}

int ota_check_update(ota_info_t* info) {
    printf("[OTA] 检查固件更新...\n");

    g_ota_state = OTA_STATE_CHECKING;

    // 模拟检查更新逻辑
    strcpy(g_ota_info.version, "3.4.3");
    g_ota_info.size = 200 * 1024;
    strcpy(g_ota_info.url, "http://ota.server.com/firmware.bin");
    strcpy(g_ota_info.md5, "d41d8cd98f00b204e9800998ecf8427e");

    if (info) {
        memcpy(info, &g_ota_info, sizeof(ota_info_t));
    }

    g_ota_state = OTA_STATE_IDLE;
    printf("[OTA] 发现新版本: %s\n", g_ota_info.version);

    return 0; // 0: 有更新
}

int ota_start_download(const ota_info_t* info) {
    if (!info) return -1;

    printf("[OTA] 开始下载: %s\n", info->version);
    g_ota_state = OTA_STATE_DOWNLOADING;
    g_ota_progress = 0;

    // TODO: 实现下载逻辑

    return 0;
}

ota_state_t ota_get_state(void) {
    return g_ota_state;
}

int ota_get_progress(void) {
    return g_ota_progress;
}

void ota_process(void) {
    // OTA 主处理循环
    static uint32_t last_check = 0;
    // TODO: 实现定期检查逻辑
}
```

### 3.3 Modbus OTA 接口

```c
// include/modbus/modbus_ota.h
#ifndef MODBUS_OTA_H
#define MODBUS_OTA_H

#include <stdint.h>

void modbus_ota_init(void);
uint16_t modbus_ota_read_register(uint16_t addr);
void modbus_ota_write_register(uint16_t addr, uint16_t value);

#endif // MODBUS_OTA_H
```

```c
// src/modbus/modbus_ota.c
#include "modbus/modbus_ota.h"
#include "ota/ota_manager.h"
#include <stdio.h>

void modbus_ota_init(void) {
    printf("[MODBUS] OTA 接口初始化完成\n");
}

uint16_t modbus_ota_read_register(uint16_t addr) {
    switch (addr) {
        case OTA_REG_STATE:
            return (uint16_t)ota_get_state();

        case OTA_REG_PROGRESS:
            return (uint16_t)ota_get_progress();

        default:
            return 0;
    }
}

void modbus_ota_write_register(uint16_t addr, uint16_t value) {
    switch (addr) {
        case OTA_REG_CONTROL:
            switch (value) {
                case OTA_CMD_CHECK:
                    printf("[MODBUS] 收到检查更新命令\n");
                    ota_check_update(NULL);
                    break;

                case OTA_CMD_START:
                    printf("[MODBUS] 收到开始升级命令\n");
                    // TODO: 开始升级
                    break;

                case OTA_CMD_CANCEL:
                    printf("[MODBUS] 收到取消升级命令\n");
                    break;

                case OTA_CMD_ROLLBACK:
                    printf("[MODBUS] 收到回滚命令\n");
                    break;
            }
            break;
    }
}
```

## 4. 开发工具脚本

### 4.1 构建脚本

```bash
#!/bin/bash
# tools/scripts/build.sh

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo -e "${BLUE}Nano DTU OTA 构建脚本${NC}"
echo -e "${BLUE}===================${NC}"

# 创建构建目录
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 清理构建
if [ "$1" = "clean" ]; then
    echo -e "${BLUE}清理构建目录...${NC}"
    rm -rf *
fi

# 配置项目
echo -e "${BLUE}配置项目...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译项目
echo -e "${BLUE}编译项目...${NC}"
make -j$(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo -e "${GREEN}构建成功!${NC}"
    ls -la *.hex *.bin 2>/dev/null || echo "未找到输出文件"
else
    echo -e "${RED}构建失败!${NC}"
    exit 1
fi
```

### 4.2 烧录脚本

```bash
#!/bin/bash
# tools/scripts/flash.sh

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
FIRMWARE="$PROJECT_DIR/build/nano-ota.hex"

echo -e "${BLUE}Nano DTU 烧录脚本${NC}"
echo -e "${BLUE}=================${NC}"

# 检查固件文件
if [ ! -f "$FIRMWARE" ]; then
    echo -e "${RED}固件文件不存在: $FIRMWARE${NC}"
    echo "请先运行构建脚本: ./tools/scripts/build.sh"
    exit 1
fi

echo -e "${BLUE}检查调试器连接...${NC}"

# 尝试不同的烧录工具
if command -v pyocd &> /dev/null; then
    echo -e "${GREEN}使用 pyOCD 烧录...${NC}"
    pyocd flash -t nano100 "$FIRMWARE"

elif command -v openocd &> /dev/null; then
    echo -e "${GREEN}使用 OpenOCD 烧录...${NC}"
    openocd -f interface/jlink.cfg -f target/nano100.cfg \
            -c "program $FIRMWARE verify reset exit"

elif command -v JLinkExe &> /dev/null; then
    echo -e "${GREEN}使用 J-Link 烧录...${NC}"
    cat > /tmp/jlink_script << EOF
device NANO100
si 1
speed 4000
loadfile $FIRMWARE
r
g
q
EOF
    JLinkExe < /tmp/jlink_script
    rm /tmp/jlink_script

else
    echo -e "${RED}未找到支持的烧录工具!${NC}"
    echo "请安装: pyocd, openocd 或 J-Link"
    exit 1
fi

echo -e "${GREEN}烧录完成!${NC}"
```

### 4.3 调试脚本

```bash
#!/bin/bash
# tools/scripts/debug.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}Nano DTU 串口调试${NC}"
echo -e "${BLUE}=================${NC}"

# 查找串口设备
SERIAL_PORT=$(ls /dev/cu.usbserial* 2>/dev/null | head -1)

if [ -z "$SERIAL_PORT" ]; then
    SERIAL_PORT=$(ls /dev/cu.SLAB_* 2>/dev/null | head -1)
fi

if [ -z "$SERIAL_PORT" ]; then
    echo -e "${RED}未找到串口设备!${NC}"
    echo "请检查设备连接"
    exit 1
fi

echo -e "${GREEN}找到串口: $SERIAL_PORT${NC}"
echo -e "${BLUE}启动串口监控 (115200 波特率)...${NC}"
echo -e "按 Ctrl+A 然后 X 退出"
echo ""

# 使用 screen 监控串口
screen "$SERIAL_PORT" 115200
```

### 4.4 设备检测脚本

```bash
#!/bin/bash
# tools/scripts/detect_device.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}Nano DTU 设备检测${NC}"
echo -e "${BLUE}=================${NC}"

echo -e "${BLUE}检查 USB 设备...${NC}"
system_profiler SPUSBDataType | grep -i "serial\|jtag\|debug" || echo "未找到相关 USB 设备"

echo -e "\n${BLUE}检查串口设备...${NC}"
ls -la /dev/cu.* 2>/dev/null || echo "未找到串口设备"

echo -e "\n${BLUE}检查调试器...${NC}"

# 检查 J-Link
if command -v JLinkExe &> /dev/null; then
    echo -e "${GREEN}J-Link 软件已安装${NC}"
else
    echo -e "J-Link 软件未安装"
fi

# 检查 pyOCD
if command -v pyocd &> /dev/null; then
    echo -e "${GREEN}pyOCD 已安装${NC}"
    pyocd list 2>/dev/null || echo "未检测到 pyOCD 兼容设备"
else
    echo -e "pyOCD 未安装"
fi

# 检查 OpenOCD
if command -v openocd &> /dev/null; then
    echo -e "${GREEN}OpenOCD 已安装${NC}"
else
    echo -e "OpenOCD 未安装"
fi

echo -e "\n${BLUE}设备检测完成${NC}"
```

## 5. VS Code 配置

### 5.1 调试配置

```json
// .vscode/launch.json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Nano DTU",
      "type": "cortex-debug",
      "request": "launch",
      "servertype": "jlink",
      "cwd": "${workspaceRoot}",
      "executable": "${workspaceRoot}/build/nano-ota",
      "device": "NANO100",
      "interface": "swd",
      "runToMain": true,
      "preLaunchTask": "build"
    }
  ]
}
```

### 5.2 任务配置

```json
// .vscode/tasks.json
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
      }
    },
    {
      "label": "flash",
      "type": "shell",
      "command": "./tools/scripts/flash.sh",
      "dependsOn": "build"
    },
    {
      "label": "clean",
      "type": "shell",
      "command": "./tools/scripts/build.sh",
      "args": ["clean"]
    }
  ]
}
```

## 6. 测试流程

### 6.1 OTA 功能测试

```python
#!/usr/bin/env python3
# tools/scripts/test_ota.py

import serial
import time
import struct

class ModbusOTATest:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)

    def read_register(self, address):
        # 构造 Modbus 读取命令
        cmd = struct.pack('>BBHH', 1, 3, address, 1)
        crc = self.calculate_crc(cmd)
        self.ser.write(cmd + struct.pack('<H', crc))

        response = self.ser.read(7)
        if len(response) >= 5:
            return struct.unpack('>H', response[3:5])[0]
        return None

    def write_register(self, address, value):
        # 构造 Modbus 写入命令
        cmd = struct.pack('>BBHH', 1, 6, address, value)
        crc = self.calculate_crc(cmd)
        self.ser.write(cmd + struct.pack('<H', crc))
        return True

    def calculate_crc(self, data):
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 1:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc

    def test_ota(self):
        print("测试 OTA 功能...")

        # 读取状态
        state = self.read_register(200)
        print(f"OTA 状态: {state}")

        # 发送检查更新命令
        print("发送检查更新命令...")
        self.write_register(202, 1)
        time.sleep(1)

        # 读取更新后的状态
        state = self.read_register(200)
        print(f"检查后状态: {state}")

if __name__ == "__main__":
    import sys
    port = sys.argv[1] if len(sys.argv) > 1 else '/dev/cu.usbserial'

    try:
        tester = ModbusOTATest(port)
        tester.test_ota()
    except Exception as e:
        print(f"测试失败: {e}")
```

### 6.2 本地 OTA 服务器

```python
#!/usr/bin/env python3
# tools/scripts/local_ota_server.py

import http.server
import socketserver
import json
import os

PORT = 8080

class OTAHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/api/v1/ota/check':
            self.handle_check_update()

    def handle_check_update(self):
        response = {
            "update_available": True,
            "firmware": {
                "version": "3.4.3",
                "size": 200 * 1024,
                "url": f"http://localhost:{PORT}/firmware/nano-ota.bin",
                "checksum": "sha256:abc123...",
                "release_notes": "OTA 功能改进"
            }
        }

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(response).encode())

if __name__ == "__main__":
    with socketserver.TCPServer(("", PORT), OTAHandler) as httpd:
        print(f"OTA 服务器运行在端口 {PORT}")
        httpd.serve_forever()
```

## 7. 快速开始

### 7.1 环境设置脚本

```bash
#!/bin/bash
# quick_start.sh

echo "🚀 Nano DTU OTA 开发环境快速设置"
echo "=================================="

# 检查 Homebrew
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew 未安装"
    exit 1
fi

# 安装依赖
echo "📦 安装开发工具..."
brew install git cmake make python3 arm-none-eabi-gcc open-ocd

# 设置权限
echo "🔧 设置脚本权限..."
chmod +x tools/scripts/*.sh

# 检测设备
echo "🔍 检测设备..."
./tools/scripts/detect_device.sh

echo ""
echo "✅ 设置完成！接下来的步骤："
echo "1. 连接硬件设备"
echo "2. 编译项目: ./tools/scripts/build.sh"
echo "3. 烧录固件: ./tools/scripts/flash.sh"
```

### 7.2 开发流程

```bash
# 1. 克隆或创建项目
git clone <your-repo> nano-ota-dev
cd nano-ota-dev

# 2. 运行快速设置
./quick_start.sh

# 3. 构建项目
./tools/scripts/build.sh

# 4. 连接设备并烧录
./tools/scripts/flash.sh

# 5. 启动调试
./tools/scripts/debug.sh

# 6. 测试 OTA 功能
python3 tools/scripts/test_ota.py /dev/cu.usbserial*
```

## 8. 常见问题

### 8.1 编译问题

```bash
# 工具链未找到
brew reinstall arm-none-eabi-gcc

# 权限问题
chmod +x tools/scripts/*.sh

# 依赖缺失
brew install cmake make
```

### 8.2 烧录问题

```bash
# 设备未找到
./tools/scripts/detect_device.sh

# 权限不足
sudo ./tools/scripts/flash.sh

# 驱动问题
# 重新安装 J-Link 驱动
```

### 8.3 调试问题

```bash
# 串口被占用
sudo pkill screen
sudo pkill minicom

# 串口未找到
ls -la /dev/cu.*
```

## 9. 硬件连接

### 9.1 调试器连接 (J-Link)

```
J-Link        →    Nano DTU
VCC (3.3V)    →    VDD
GND           →    GND
SWDIO         →    ICE_DAT
SWCLK         →    ICE_CLK
```

### 9.2 串口连接

```
USB转串口     →    Nano DTU
GND           →    GND
TXD           →    UART_RX (PA1)
RXD           →    UART_TX (PA0)
```

### 9.3 Modbus 连接

```
USB转RS485    →    Nano DTU
A+            →    RS485_A
B-            →    RS485_B
GND           →    GND
```

---

**现在您可以开始 Nano DTU OTA 功能的开发和测试了！** 🎉
