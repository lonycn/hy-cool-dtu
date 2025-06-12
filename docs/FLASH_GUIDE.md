# 🔥 NANO100B 固件烧录指南

本指南介绍如何使用我们的自动化烧录脚本将固件烧录到 NANO100B 开发板。

## 📋 前提条件

### 硬件要求

- ✅ NANO100B 开发板
- ✅ J-Link 调试器 (或兼容设备)
- ✅ USB 线缆连接 J-Link 到电脑
- ✅ SWD 连接线连接 J-Link 到开发板

### 软件要求

- ✅ macOS/Linux/Windows 系统
- ✅ J-Link 软件: `brew install --cask segger-jlink`
- ✅ PlatformIO: `pip install platformio`
- ✅ ARM 工具链: 自动安装或手动配置

## 🚀 快速开始

### 一键烧录

最简单的方式，适合大多数用户：

```bash
# 确保在项目根目录
cd hy-cool-dtu

# 执行一键烧录
./flash.sh
```

### 完整烧录脚本

如果需要更多控制和详细信息：

```bash
./tools/scripts/flash_firmware.sh
```

## 🔍 预检查

### 测试连接

在正式烧录前，建议先测试硬件连接：

```bash
# 测试J-Link与开发板连接
./tools/scripts/test_jlink.sh
```

**测试成功标志**：

- ✅ 检测到 J-Link USB 设备
- ✅ 成功连接到 NANO100B 芯片
- ✅ 读取到芯片 ID 信息
- ✅ 显示 CPU 状态信息

## 📊 烧录流程详解

### 第 1 步：环境检查

脚本会自动检查：

- J-Link 软件是否安装
- PlatformIO 是否可用
- USB 设备连接状态
- ARM 工具链是否配置

### 第 2 步：清理构建

- 清理之前的编译文件
- 删除旧的固件文件
- 创建新的构建目录

### 第 3 步：编译项目

- 使用 PlatformIO 编译源代码
- 生成 ELF 可执行文件
- 显示内存使用统计

### 第 4 步：打包固件

- 生成二进制文件 (.bin)
- 生成 Intel HEX 文件 (.hex)
- 显示固件大小信息

### 第 5 步：创建烧录脚本

- 自动生成 J-Link 命令脚本
- 配置芯片类型和接口参数
- 设置 Flash 擦除和写入指令

### 第 6 步：执行烧录

- 连接到目标芯片
- 擦除 Flash 存储器
- 写入固件数据
- 验证写入完整性

### 第 7 步：重启验证

- 重启开发板
- 等待初始化完成
- 播放 3 声蜂鸣确认
- 检测串口输出

## 🎯 烧录进度指示

烧录过程中，您会看到以下进度信息：

```
========================================
🚀 NANO100B OTA 固件烧录工具
========================================

[14:32:15] 检查烧录依赖工具...
✅ 依赖工具检查完成

[14:32:16] 清理构建目录...
📊 已清理 .pio 目录
📊 已清理 build 目录
✅ 构建目录清理完成

[14:32:17] 开始编译项目...
📊 正在编译源代码... (预计需要1-2分钟)
✅ 项目编译成功

[14:32:45] 打包固件文件...
📊 生成二进制固件文件...
📊 生成HEX固件文件...
✅ 固件打包完成，大小: 45230 字节

[14:32:46] 开始烧录固件到开发板...
📊 正在连接J-Link调试器...
📊 目标芯片: NANO100B
📊 烧录地址: 0x00000000
📊 接口: SWD, 速度: 4000 kHz

📡 烧录进度:
  Connected to target
  📊 Erasing flash... [100%]
  📊 Programming flash... [100%]
  📊 Verifying flash... [100%]
✅ 固件烧录成功完成！

[14:33:12] 重启开发板并验证...
📊 正在重启开发板...
✅ 开发板重启完成！
📊 请注意听取开发板的蜂鸣声：
📊 🔊 如果重启成功，您应该听到3声 '滴滴滴' 蜂鸣声
```

## 🔊 成功标志

### 听觉确认

烧录成功并重启后，开发板会发出：

- **3 声短促的蜂鸣声** (滴滴滴)
- 间隔约 0.5 秒
- 音调清晰可辨

### 视觉确认

- LED 指示灯按预期闪烁
- 串口输出启动信息
- 无错误提示信息

## 🛠️ 故障排除

### 常见问题

#### 1. J-Link 未检测到

**错误信息**: `❌ J-Link软件未安装`

**解决方案**:

```bash
# macOS
brew install --cask segger-jlink

# Linux
# 下载并安装J-Link Linux包

# Windows
# 下载SEGGER官网安装包
```

#### 2. USB 设备未发现

**错误信息**: `⚠️ 未检测到J-Link USB设备`

**解决方案**:

- 检查 USB 线缆连接
- 确认 J-Link 电源 LED 亮起
- 尝试其他 USB 端口
- 重新插拔设备

#### 3. 目标芯片连接失败

**错误信息**: `❌ J-Link连接测试失败`

**解决方案**:

- 检查 SWD 连接线 (SWDIO, SWCLK, GND, VCC)
- 确认开发板正确供电
- 按下开发板复位按钮
- 尝试降低连接速度

#### 4. 编译失败

**错误信息**: `❌ 项目编译失败，请检查代码`

**解决方案**:

- 检查源代码语法错误
- 确认所有头文件路径正确
- 查看详细编译日志
- 清理并重新编译

#### 5. Flash 写入失败

**错误信息**: 烧录过程中的写入错误

**解决方案**:

- 检查 Flash 是否被锁定
- 尝试完全擦除 Flash
- 确认固件大小未超出限制
- 检查电源稳定性

### 获取详细日志

所有烧录日志都保存在 `build/` 目录：

```bash
# 查看编译日志
cat build/flash.log

# 查看烧录日志
cat build/flash_output.log

# 查看连接测试日志
cat build/jlink_test.log
```

## 🔧 高级用法

### 自定义配置

可以修改 `tools/scripts/flash_firmware.sh` 中的配置：

```bash
# 目标芯片配置
TARGET_CHIP="NANO100B"
FLASH_SIZE="0x80000"     # 512KB Flash
RAM_SIZE="0x10000"       # 64KB RAM

# J-Link配置
JLINK_SPEED="4000"       # 4MHz
JLINK_INTERFACE="SWD"    # SWD接口
```

### 手动烧录

如果自动脚本无法满足需求，可以手动执行：

```bash
# 1. 编译项目
pio run -e nano100b

# 2. 生成固件文件
arm-none-eabi-objcopy -O binary .pio/build/nano100b/firmware.elf firmware.bin

# 3. 手动烧录
JLinkExe -device NANO100B -if SWD -speed 4000
```

### 批量烧录

对于生产环境，可以创建批量烧录脚本：

```bash
#!/bin/bash
# 批量烧录多个设备

for i in {1..10}; do
    echo "烧录设备 #$i"
    ./flash.sh
    echo "请更换下一个设备并按回车继续..."
    read
done
```

## 📚 相关文档

- [J-Link 用户手册](https://www.segger.com/downloads/jlink/UM08001)
- [NANO100B 数据手册](https://www.nuvoton.com/products/microcontrollers/arm-cortex-m0-mcus/nano100-series/)
- [PlatformIO 文档](https://docs.platformio.org/)
- [项目开发指南](docs/development/DEVELOP.md)

## 💬 获取帮助

如果遇到问题：

1. 查看本文档的故障排除部分
2. 检查项目 Issue 页面
3. 运行连接测试脚本获取详细信息
4. 联系项目维护者

---

**提示**: 首次使用建议先运行 `./tools/scripts/test_jlink.sh` 测试连接，确认硬件配置正确后再进行烧录。
