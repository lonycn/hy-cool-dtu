# HY Cool DTU with OTA Upgrade Module

冷库 DTU 设备固件，集成 OTA 远程升级功能，基于 NANO100B 系列微控制器开发。

## 项目概述

### 基本信息

- **项目名称**: HY Cool DTU with OTA
- **目标硬件**: NANO100B 系列 (Cortex-M0+)
- **Flash 容量**: 512KB (双分区 OTA 设计)
- **RAM 容量**: 64KB
- **当前版本**: v1.1.26

### 主要功能

- **温湿度监控**: 支持多路温湿度传感器
- **Modbus 通信**: 支持 Modbus RTU/TCP 协议
- **LoRa 通信**: 支持 LoRa 无线通信
- **数据存储**: 历史数据记录和查询
- **报警管理**: 温度报警和状态监控
- **OTA 升级**: 远程固件升级功能
- **多串口支持**: 4 路 UART 接口

## 项目结构

```
App_with_ota/
├── README.md                    # 项目说明文档
├── CMakeLists.txt              # CMake构建配置
├── Makefile                    # 传统Makefile构建
├──
├── include/                    # 头文件目录
│   ├── global.h               # 全局定义和变量
│   ├── sys.h                  # 系统函数声明
│   └── rtc.h                  # RTC函数声明
├──
├── linker/                     # 链接器脚本
│   └── nano100_512k.ld       # NANO100B 512KB Flash链接脚本
├──
├── ota/                        # OTA升级模块
│   ├── include/               # OTA头文件
│   │   ├── ota_manager.h      # OTA管理器
│   │   └── ota_config.h       # OTA配置
│   ├── src/                   # OTA源文件
│   │   ├── ota_manager.c      # OTA核心逻辑
│   │   └── ota_config.c       # OTA配置实现
│   ├── config/                # OTA配置文件
│   └── test/                  # OTA测试文件
├──
├── uart/                       # UART通信模块
│   ├── SerInt-uart0.c         # UART0中断处理
│   ├── SerInt_uart1.c         # UART1中断处理
│   ├── SerInt_uart2.c         # UART2中断处理
│   ├── SerInt_uart3.c         # UART3中断处理
│   ├── SerInt_uart4.c         # UART4中断处理
│   └── Simdata.c              # 模拟数据处理
├──
├── led/                        # LED控制模块
│   └── led.c                  # LED驱动
├──
├── alarm/                      # 报警模块
│   └── SetAlarmFlag.c         # 报警标志设置
├──
├── main_loop.c                 # 主程序循环
├── Modbus.c                   # Modbus协议实现
├── sys_flash.c                # Flash操作
├── switch.c                   # 开关控制
├── io.c                       # IO操作
├── RI300CRC.c                 # CRC校验
├── datastruct.c               # 数据结构
├── Base_Para.C                # 基础参数
├── HistoryData.C              # 历史数据处理
├── HistoryDataRead.C          # 历史数据读取
├── HistoryDataWrite.C         # 历史数据写入
├── Tempalarm.C                # 温度报警
├── alarm.C                    # 报警处理
├── BL02D_Special.C            # BL02D特定功能
├── BL03D_BH_Para.C            # BL03D参数处理
└── [各种UART Modbus文件]      # 多串口Modbus实现
```

## OTA 升级功能

### Flash 分区布局

```
地址范围              大小    用途
0x08000000-0x08008000  32KB   Bootloader
0x08008000-0x08040000  224KB  应用程序分区1 (主分区)
0x08040000-0x08078000  224KB  应用程序分区2 (OTA分区)
0x08078000-0x08080000  32KB   配置数据分区
```

### OTA 寄存器地址

- **200**: OTA 状态寄存器
- **201**: 升级进度寄存器
- **202**: OTA 控制命令寄存器
- **203**: 新版本信息寄存器
- **204**: 错误代码寄存器

### OTA 控制命令

- **1**: 检查更新
- **2**: 开始升级
- **3**: 取消升级
- **4**: 回滚到上一版本

## 构建说明

### 环境要求

- **操作系统**: macOS / Linux / Windows WSL
- **编译器**: ARM GCC 工具链 (arm-none-eabi-gcc)
- **构建工具**: CMake 3.20+ 或 Make
- **调试工具**: OpenOCD + J-Link

### macOS 环境设置

```bash
# 安装ARM工具链
brew install --cask gcc-arm-embedded

# 安装CMake
brew install cmake

# 安装OpenOCD
brew install openocd

# 验证工具链
arm-none-eabi-gcc --version
```

### 使用 CMake 构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
make

# 或者指定并行编译
make -j4
```

### 使用 Makefile 构建

```bash
# 编译项目
make

# 查看内存使用情况
make size

# 查看项目信息
make info

# 清理构建产物
make clean
```

### 构建产物

编译成功后会生成以下文件：

- `hy_cool_dtu_ota.elf` - ELF 可执行文件
- `hy_cool_dtu_ota.bin` - 二进制固件文件
- `hy_cool_dtu_ota.hex` - Intel HEX 格式文件
- `hy_cool_dtu_ota.map` - 内存映射文件
- `hy_cool_dtu_ota.lst` - 反汇编列表文件

## 烧录和调试

### 硬件连接

1. **J-Link 调试器**连接到 NANO100B 的 SWD 接口
2. **串口**连接到 UART0 (115200,8,N,1)
3. **电源**供应 3.3V

### 烧录固件

```bash
# 使用CMake
make flash

# 使用Makefile
make flash

# 手动烧录
openocd -f interface/jlink.cfg -f target/nano100b.cfg \
  -c "program hy_cool_dtu_ota.elf verify reset exit"
```

### 启动调试

```bash
# 启动调试会话
make debug

# 或手动启动
openocd -f interface/jlink.cfg -f target/nano100b.cfg &
arm-none-eabi-gdb hy_cool_dtu_ota.elf \
  -ex "target extended-remote localhost:3333"
```

## Modbus 寄存器地址

### OTA 相关寄存器

| 地址 | 名称         | 类型 | 说明                                            |
| ---- | ------------ | ---- | ----------------------------------------------- |
| 200  | OTA_STATUS   | R    | OTA 状态 (0:空闲, 1:检查中, 2:下载中, 3:安装中) |
| 201  | OTA_PROGRESS | R    | 升级进度 (0-100%)                               |
| 202  | OTA_CONTROL  | W    | 控制命令 (1:检查, 2:开始, 3:取消, 4:回滚)       |
| 203  | OTA_VERSION  | R    | 新版本号                                        |
| 204  | OTA_ERROR    | R    | 错误代码                                        |

### 系统信息寄存器

| 地址 | 名称             | 类型 | 说明       |
| ---- | ---------------- | ---- | ---------- |
| 10   | MAIN_VERSION     | R    | 主版本号   |
| 11   | SUB_VERSION      | R    | 子版本号   |
| 12   | MODIFIED_VERSION | R    | 修订版本号 |

## 开发指南

### 添加新功能

1. 在对应模块目录添加源文件
2. 更新 CMakeLists.txt 或 Makefile 中的源文件列表
3. 在 include 目录添加必要的头文件
4. 更新 global.h 中的函数声明

### 代码规范

- 使用 C99 标准
- 函数名采用小写下划线分隔
- 宏定义采用全大写下划线分隔
- 每个模块包含对应的头文件保护

### OTA 集成说明

OTA 模块已集成到主程序中：

1. `main_loop.c`中包含 OTA 模块初始化
2. `Modbus.c`中添加了 OTA 寄存器处理
3. 通过 Modbus 协议可以远程控制 OTA 升级

## 故障排除

### 编译错误

1. **找不到 arm-none-eabi-gcc**: 检查工具链是否正确安装
2. **头文件找不到**: 检查 include 路径配置
3. **链接错误**: 检查链接器脚本路径

### 烧录问题

1. **找不到 J-Link**: 检查 J-Link 驱动和连接
2. **目标芯片识别失败**: 确认硬件连接和电源
3. **权限错误**: 在 Linux/macOS 上可能需要 sudo 权限

### 运行时问题

1. **程序不启动**: 检查链接器脚本和启动代码
2. **串口无输出**: 检查波特率和硬件连接
3. **OTA 升级失败**: 检查网络连接和服务器配置

## 许可证

本项目仅供内部开发使用，未经授权不得外传。

## 版本历史

- **v1.1.26** - 集成 OTA 升级模块
- **v1.1.25** - 优化 Modbus 通信
- **v1.1.24** - 修复温度采集问题

## 技术支持

如有技术问题，请联系开发团队。
