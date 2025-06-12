# 冷库 DTU 远程控制模块 - 开发说明

## 项目结构

```
hy-cool-dtu/                                     # 项目根目录
├── app/                                         # 应用程序入口层
│   ├── ota/                                    # OTA应用层模块
│   │   ├── include/                            # OTA应用层头文件
│   │   ├── src/                                # OTA应用层源文件
│   │   ├── config/                             # OTA配置文件
│   │   └── test/                               # OTA测试文件
│   └── main.c                                  # 主程序入口
├── src/                                         # 源代码目录
│   ├── include/                                # 项目头文件
│   │   ├── constants.h                         # 常量定义
│   │   ├── core_cm0plus.h                      # ARM核心定义
│   │   ├── global.h                            # 全局定义
│   │   ├── Nano100Series.h                     # NANO100芯片定义
│   │   └── [其他系统头文件]                    # 标准库头文件
│   ├── ota/                                    # OTA核心模块
│   ├── uart/                                   # UART通信模块
│   ├── sht2x/                                  # 温湿度传感器
│   ├── sim7600/                                # 4G通信模块
│   ├── si24r1/                                 # LoRa通信模块
│   ├── lcd/                                    # LCD显示模块
│   ├── led/ & workled/                         # LED控制模块
│   ├── alarm/ & alarm_led/                     # 报警系统
│   ├── key/                                    # 按键处理模块
│   ├── dido/                                   # 数字IO控制
│   ├── i2c/                                    # I2C通信模块
│   ├── spi/                                    # SPI通信模块
│   ├── adc/                                    # ADC采集模块
│   ├── pwm/                                    # PWM控制模块
│   ├── switch/                                 # 开关控制模块
│   ├── lora/                                   # LoRa协议模块
│   ├── watchdong/                              # 看门狗模块
│   ├── bl/                                     # 引导加载器
│   ├── test_function/                          # 测试功能模块
│   ├── linker/                                 # 链接器脚本
│   ├── main_loop.c                             # 主循环程序
│   ├── Modbus.c                                # Modbus协议实现
│   └── [其他功能源文件]                        # 各种功能模块源文件
├── tools/                                       # 开发工具链
│   ├── scripts/                                # 开发脚本
│   │   ├── flash_firmware.sh                   # 完整烧录脚本
│   │   ├── test_jlink.sh                       # J-Link测试
│   │   └── [其他脚本]                          # 各种开发脚本
│   ├── debug/                                  # 调试配置
│   └── test/                                   # 测试工具
├── docs/                                        # 文档系统
│   ├── development/                            # 开发文档
│   ├── manuals/                                # 用户手册
│   ├── architecture/                           # 架构设计
│   └── [其他文档]                              # 各种项目文档
├── hardware/                                    # 硬件相关文件
├── cmake/                                       # CMake配置
├── .vscode/                                     # IDE配置
├── CMakeLists.txt                              # CMake主配置
├── platformio.ini                              # PlatformIO配置
├── flash.sh                                    # 快速烧录脚本
├── README.md                                   # 项目说明文档
└── [其他配置文件]                              # 各种项目配置
```

## 核心文件说明

### 1. 主程序文件

#### `main.c`

- **位置**: `Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/SampleCode/ExampleCode/NANO100B_UART_Rx_Wakeup_PDMA/main.c`
- **功能**: 系统初始化和程序入口
- **关键函数**:
  - `main()`: 程序入口
  - `SYS_Init1()`: 系统时钟和外设初始化
  - `main1()`: 主要初始化流程

#### `main_loop.c`

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/main_loop.c`
- **功能**: 主循环逻辑和系统控制
- **关键函数**:
  - `main_loop()`: 主循环入口
  - `Main_loop_while()`: 主循环处理
  - `Main_loop_Initial()`: 循环初始化

### 2. 通信协议文件

#### `Modbus.c`

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/Modbus.c`
- **功能**: Modbus RTU 协议实现
- **支持功能码**:
  - 0x03: 读保持寄存器
  - 0x04: 读输入寄存器
  - 0x06: 写单个寄存器
  - 0x10: 写多个寄存器
  - 0x41: 读历史数据
  - 0x44: 读分页数据

#### 串口通信文件

- `uart0_Modbus.c`: UART0 Modbus 通信
- `uart1_Modbus.c`: UART1 Modbus 通信
- `uart2_Modbus.c`: UART2 PC 通信
- `uart3_Modbus.c`: UART3 LoRa 通信
- `uart4_dealdata.c`: UART4 数据处理

### 3. 传感器和采集模块

#### SHT2x 温湿度传感器

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/sht2x/`
- **文件**:
  - `SHT2x.c`: 传感器驱动
  - `I2C_HAL.c`: I2C 硬件抽象层
  - `shx.h`: 头文件定义

#### ADC 采集模块

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/adc/adc.c`
- **功能**: 模拟量采集，支持 NTC 温度检测

### 4. 通信模块

#### SIM7600 4G 模块

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/SIM7600/`
- **主要文件**:
  - `Sim7600_StatusMachine.c`: 状态机控制
  - `Sim7600Send.c`: 数据发送
  - `Simply600ReceiveDeal.c`: 数据接收处理
  - `cJSON.c`: JSON 数据处理

#### LoRa 无线模块

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/lora/lora.c`
- **功能**: LoRa 无线通信控制

### 5. 控制和显示模块

#### 数字 IO 控制

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/dido/dido.c`
- **功能**: 数字输入输出控制，继电器控制

#### LCD 显示

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/lcd/lcd.c`
- **功能**: LCD 显示屏控制

#### LED 控制

- **位置**: `Nano/modbus_ver3.4.2(3283)/app/led/`
- **功能**: 状态指示 LED 控制

## 业务逻辑分析

### 1. 系统启动流程

```
系统上电 → 硬件初始化 → 外设初始化 → 参数加载 → 进入主循环
```

1. **硬件初始化**:

   - 时钟系统配置
   - GPIO 端口配置
   - 串口初始化
   - 定时器配置

2. **外设初始化**:

   - 温湿度传感器初始化
   - ADC 初始化
   - LCD 初始化
   - LoRa 模块初始化

3. **参数加载**:
   - 从 Flash 读取配置参数
   - 初始化变量列表
   - 设置默认值

### 2. 主循环逻辑

```c
while(1) {
    // 1. 串口通信处理
    uart_communication_handler();

    // 2. 传感器数据采集
    sensor_data_collection();

    // 3. 数据处理和存储
    data_processing_and_storage();

    // 4. 设备控制逻辑
    device_control_logic();

    // 5. 显示更新
    display_update();

    // 6. 报警处理
    alarm_processing();

    // 7. 通信数据发送
    communication_data_send();

    // 8. 系统状态检查
    system_status_check();
}
```

### 3. 数据流向

```
传感器数据 → ADC/I2C采集 → 数据处理 → 存储到VarList → Modbus响应 → 远程传输
```

### 4. 通信架构

```
上位机/云平台
    ↓ (Modbus RTU/4G/LoRa)
DTU主控制器
    ↓ (RS485/LoRa)
现场传感器/执行器
```

## 关键数据结构

### 1. 全局变量数组

```c
// 变量数组 - 存储实时数据
unsigned char VarList[VarNum];

// 参数数组 - 存储配置参数
unsigned char ParaList[ParaNum];

// 历史数据结构
struct Hisrory_Struct {
    unsigned char AlarmLevel;
    unsigned char PowerVolume;
    unsigned char MiniterStartFlag[8];
    unsigned char MiniterDelay[8];
    unsigned char Alarm1Count[8];
    uint16_t AlarmCountOverFlag[8];
    uint16_t MiniterDelaySet[8];
    unsigned char AlarmStatus1;
    unsigned char AlarmFlag;
    unsigned char TempCount;
    unsigned char AlarmRecordFlag;
    unsigned char AlarmFlagBak;
};
```

### 2. 寄存器地址映射

```c
// 版本信息
#define cMainVersion        6
#define cSubVersion         7
#define cModifiedVersion    8

// 实时数据
#define cRealDataAct        9
#define cTemp              11  // 温度
#define cHumi              12  // 湿度

// 报警相关
#define AlarmStartAddr     104
```

## 配置宏定义

### 1. 产品类型配置

```c
// 当前配置为LoRa网关模式
#define LoraGateway

// 其他可选配置
//#define LoraNode      // LoRa节点模式
//#define Bl02d         // BL02D产品
//#define BH_Bl03d      // BH_BL03D产品
//#define Thermoberg03d // Thermoberg产品
```

### 2. 功能配置

```c
// 传感器类型
#define cSensorTye_OnlyRs485    0
#define cSensorTye_OnlyNtc      1
#define cSensorTye_Rs485_ntc    2

// 通信配置
#define cSendLength    100
#define cBufferLen     200
```

## 开发指南

### 1. 环境搭建

1. **安装 Keil MDK**:

   - 下载并安装 Keil MDK 5.x
   - 安装新唐科技器件支持包

2. **硬件准备**:

   - NANO100B 开发板
   - J-Link 调试器
   - 串口调试工具

3. **项目导入**:
   - 打开 Keil 项目文件
   - 配置目标器件
   - 设置调试器

### 2. 编译配置

1. **宏定义配置**:

   - 在`global.h`中配置产品类型
   - 根据硬件配置选择功能模块

2. **编译选项**:
   - 优化级别设置
   - 调试信息配置
   - 输出格式选择

### 3. 调试方法

1. **串口调试**:

   - 使用串口助手监控通信数据
   - 发送 Modbus 命令测试功能

2. **在线调试**:
   - 设置断点调试程序流程
   - 监控变量值变化
   - 分析程序执行时序

### 4. 二次开发指南

#### 添加新的传感器

1. **创建传感器驱动文件**:

   ```c
   // 在App目录下创建新的传感器目录
   // 实现传感器初始化、读取、配置函数
   ```

2. **修改主循环**:

   ```c
   // 在main_loop.c中添加传感器调用
   // 在数据采集部分添加新传感器读取
   ```

3. **更新 Modbus 映射**:
   ```c
   // 在global.h中定义新的寄存器地址
   // 在Modbus.c中添加对应的读写处理
   ```

#### 添加新的通信接口

1. **创建通信模块**:

   ```c
   // 实现通信协议栈
   // 添加数据收发处理
   ```

2. **集成到主循环**:
   ```c
   // 在主循环中添加通信处理
   // 配置相应的中断处理
   ```

#### 修改控制逻辑

1. **分析现有控制流程**:

   - 理解当前的控制算法
   - 确定修改点和影响范围

2. **实现新的控制逻辑**:
   - 在相应的控制模块中修改
   - 确保与其他模块的兼容性

### 5. 测试验证

1. **功能测试**:

   - 温湿度采集测试
   - 通信功能测试
   - 控制输出测试

2. **性能测试**:

   - 响应时间测试
   - 稳定性测试
   - 功耗测试

3. **兼容性测试**:
   - 不同传感器兼容性
   - 通信协议兼容性
   - 上位机软件兼容性

## 常见问题和解决方案

### 1. 编译问题

**问题**: 编译时出现未定义的宏或函数
**解决**: 检查头文件包含和宏定义配置

### 2. 通信问题

**问题**: Modbus 通信无响应
**解决**:

- 检查波特率配置
- 验证设备地址设置
- 确认 CRC 校验

### 3. 传感器问题

**问题**: 温湿度读取异常
**解决**:

- 检查 I2C 连接
- 验证传感器电源
- 确认传感器地址

### 4. 存储问题

**问题**: 参数保存失败
**解决**:

- 检查 Flash 写入权限
- 验证存储地址范围
- 确认数据格式

## 版本管理

### 版本号规则

- 主版本号.子版本号.修订版本号
- 当前版本: V1.1.26

### 修改记录

详细的修改历史记录在`电箱控制程序修改说明.C`文件中，包含：

- 修改日期
- 修改内容
- 影响范围
- 测试验证

## 注意事项

1. **安全考虑**:

   - 看门狗保护机制
   - 异常处理和恢复
   - 数据完整性校验

2. **性能优化**:

   - 中断处理时间控制
   - 内存使用优化
   - 功耗管理

3. **维护性**:

   - 代码注释规范
   - 模块化设计
   - 配置参数化

4. **扩展性**:
   - 预留扩展接口
   - 模块化架构
   - 配置灵活性
