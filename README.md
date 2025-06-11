# 冷库 DTU 远程监控系统

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)]()

> 基于 Nuvoton NANO100 系列微控制器的冷库环境监控与 OTA 远程升级系统

## 🌟 项目特性

- 🌡️ **多传感器监控** - 温湿度、压力、门禁状态实时监测
- 📡 **多通信方式** - 支持 4G/LoRa/WiFi/Modbus 通信
- 🔄 **OTA 远程升级** - 安全可靠的无线固件更新
- ⚡ **低功耗设计** - 智能休眠与唤醒机制
- 🛡️ **故障诊断** - 完整的系统诊断和告警功能
- 📊 **数据缓存** - 断网续传，数据零丢失
- 🔧 **模块化架构** - 易于扩展和维护

## 🏗️ 项目结构

```
hy-cool-dtu/
├── 📁 src/                     # 主要源代码
│   ├── 📂 ota/                 # OTA升级模块
│   ├── 📂 alarm_led/           # 告警LED驱动
│   ├── 📂 i2c/                 # I2C通信模块
│   ├── 📂 pwm/                 # PWM控制模块
│   ├── 📂 sim7600/             # 4G通信模块
│   ├── 📂 test_function/       # 测试功能模块
│   ├── 📂 uart/                # 串口通信
│   ├── 📂 sht2x/               # 温湿度传感器
│   ├── 📂 lora/                # LoRa通信
│   └── 📂 watchdog/            # 看门狗模块
├── 📁 app/                     # 应用程序代码
│   ├── 📄 main.c               # 主程序入口
│   └── 📂 ota/                 # OTA应用层
├── 📁 tools/                   # 开发工具链
│   ├── 📂 scripts/             # 构建和调试脚本
│   ├── 📂 debug/               # 调试配置
│   └── 📂 test/                # 测试工具
├── 📁 docs/                    # 项目文档
│   ├── 📂 development/         # 开发指南
│   ├── 📂 manuals/             # 用户手册
│   ├── 📂 architecture/        # 架构设计
│   ├── 📂 reports/             # 分析报告
│   └── 📂 api/                 # API文档
├── 📁 hardware/                # 硬件相关文件
│   ├── 📂 pcb/                 # PCB设计文件
│   └── 📂 schematic/           # 原理图
├── 📁 cmake/                   # CMake配置文件
├── 📁 .vscode/                 # VS Code/Cursor配置
├── 🔧 CMakeLists.txt           # CMake主配置
├── ⚡ platformio.ini           # PlatformIO配置
├── 🎨 .clang-format            # 代码格式化配置
└── 🚀 quick_setup.sh           # 快速环境配置
```

## 🚀 快速开始

### 环境要求

- **操作系统**: macOS (M1/M2/M3/M4) / Linux / Windows
- **开发工具**: Cursor/VS Code + ARM 工具链
- **调试器**: J-Link / OpenOCD
- **目标芯片**: Nuvoton NANO100SD3BN

### 快速安装

```bash
# 1. 克隆项目
git clone https://github.com/lonycn/hy-cool-dtu.git
cd hy-cool-dtu

# 2. 自动配置开发环境
./quick_setup.sh

# 3. 验证环境
./tools/scripts/test_project.sh
```

### 开发工作流

#### 🔨 构建项目

```bash
# CMake构建 (推荐)
./tools/scripts/build.sh                # Debug构建
./tools/scripts/build.sh Release       # Release构建

# PlatformIO构建
./tools/scripts/build.sh Debug pio
```

#### 🔥 烧写固件

```bash
# 自动检测并烧写
./tools/scripts/flash.sh build/hy-cool-dtu.bin

# 指定调试器烧写
./tools/scripts/flash.sh build/hy-cool-dtu.bin jlink
```

#### 🐛 调试程序

```bash
# J-Link调试
./tools/scripts/debug.sh jlink

# OpenOCD调试
./tools/scripts/debug.sh openocd

# RTT实时日志
./tools/scripts/debug.sh rtt

# 内存调试
./tools/scripts/debug.sh memory
```

#### 🔍 硬件检测

```bash
# 检测连接的硬件
./tools/scripts/detect_device.sh

# 测试芯片连接
./tools/scripts/test_chip_connection.sh
```

#### 🧪 OTA 测试

```bash
# 运行OTA测试套件
python3 tools/test/test_ota.py test

# 启动OTA测试服务器
python3 tools/test/test_ota.py server --port 8080

# 生成测试固件
python3 tools/test/test_ota.py generate --version 1.2.0 --size 128
```

### IDE 开发

#### Cursor/VS Code

1. **打开项目**: 使用 Cursor 打开项目文件夹
2. **安装扩展**: 自动提示安装推荐扩展
3. **选择任务**: `Cmd+Shift+P` → "Tasks: Run Task"
4. **开始调试**: `F5` 或使用调试面板

#### 可用任务

| 任务类别 | 任务名称              | 描述            |
| -------- | --------------------- | --------------- |
| **构建** | Build Project (CMake) | 默认 Debug 构建 |
| **构建** | Build Release (CMake) | Release 构建    |
| **调试** | Debug (J-Link)        | J-Link 调试     |
| **调试** | Debug OTA Module      | OTA 专用调试    |
| **测试** | Test OTA Module       | OTA 功能测试    |
| **工具** | Format Code           | 代码格式化      |

## 📚 文档指南

### 开发文档

- [开发指南](docs/development/DEVELOP.md) - 详细开发流程
- [OTA 开发手册](docs/development/OTA_DEV.md) - OTA 功能实现
- [开发日志](docs/development/OTA_DEV_LOG.md) - 开发过程记录

### 用户手册

- [OTA 升级手册](docs/manuals/OTA_UPGRADE_MANUAL.md) - OTA 使用指南
- [看门狗开发手册](docs/manuals/WATCHDOG_DEVELOPMENT_MANUAL.md) - 看门狗功能
- [数据缓存重传手册](docs/manuals/DATA_CACHE_RETRANSMISSION_MANUAL.md) - 数据处理
- [RTC 时间管理手册](docs/manuals/RTC_TIME_MANAGEMENT_MANUAL.md) - 时间管理

### 架构设计

- [AI 集成技术扩展](docs/architecture/AI_INTEGRATION_TECHNICAL_EXTENSION.md) - AI 功能扩展
- [下一代开发提案](docs/architecture/NEXT_GENERATION_DEVELOPMENT_PROPOSAL.md) - 未来规划

### 分析报告

- [系统诊断报告](docs/reports/SYSTEM_DIAGNOSIS_REPORT.md) - 系统分析

## 🛠️ 技术栈

### 硬件平台

- **MCU**: Nuvoton NANO100SD3BN (ARM Cortex-M0)
- **Flash**: 128KB 程序存储
- **RAM**: 16KB 数据存储
- **通信**: UART/SPI/I2C/Modbus

### 开发工具

- **构建系统**: CMake + PlatformIO
- **调试器**: J-Link + OpenOCD
- **IDE**: Cursor/VS Code + Cortex-Debug
- **工具链**: ARM GCC

### 软件特性

- **RTOS**: FreeRTOS (可选)
- **通信协议**: Modbus RTU/TCP, HTTP/MQTT
- **加密**: AES256 + RSA 签名
- **存储**: Flash 分区管理
- **诊断**: 完整日志系统

## 🔧 配置说明

### Modbus 寄存器映射

| 地址范围 | 功能       | 访问权限 |
| -------- | ---------- | -------- |
| 0-99     | 传感器数据 | 只读     |
| 100-199  | 系统状态   | 只读     |
| 200-299  | OTA 控制   | 读写     |
| 300-399  | 配置参数   | 读写     |

### OTA 状态码

| 状态码 | 含义        | 描述     |
| ------ | ----------- | -------- |
| 0      | IDLE        | 空闲状态 |
| 1      | CHECKING    | 检查更新 |
| 2      | DOWNLOADING | 下载固件 |
| 3      | INSTALLING  | 安装固件 |
| 4      | SUCCESS     | 升级成功 |
| -1     | ERROR       | 升级失败 |

## 🤝 贡献指南

我们欢迎社区贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与项目开发。

### 开发流程

1. Fork 项目到您的 GitHub
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

### 代码规范

- 遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- 使用 `clang-format` 格式化代码
- 添加必要的注释和文档
- 确保所有测试通过

## 📄 许可证

本项目基于 MIT 许可证开源 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🔗 相关链接

- [项目主页](https://github.com/lonycn/hy-cool-dtu)
- [问题反馈](https://github.com/lonycn/hy-cool-dtu/issues)
- [Wiki 文档](https://github.com/lonycn/hy-cool-dtu/wiki)
- [发布版本](https://github.com/lonycn/hy-cool-dtu/releases)

## 📞 支持与反馈

如果您在使用过程中遇到问题或有任何建议，请：

1. 查看 [FAQ](docs/FAQ.md)
2. 提交 [Issue](https://github.com/lonycn/hy-cool-dtu/issues)
3. 发送邮件至：support@your-domain.com

---

**⭐ 如果这个项目对您有帮助，请给我们一个 Star！**
