# 冷库 DTU 重构功能规划文档

## 版本信息

- **文档版本**: V1.0.0
- **创建日期**: 2024 年 12 月
- **目标**: 基于现状分析，规划重构后的功能架构
- **重构目标**: 现代化、可扩展、高性能的 DTU 系统

---

## 1. 重构目标与原则

### 1.1 重构目标

**主要目标**:

1. **架构现代化**: 引入 RTOS 和分层架构
2. **模块解耦**: 降低模块间耦合度，提高可维护性
3. **性能提升**: 优化实时性和资源利用率
4. **扩展性增强**: 支持新功能快速集成
5. **代码质量**: 提升代码规范性和可读性

**技术指标**:

- Modbus 响应时间: <50ms (提升 50%)
- 系统实时性: 支持 ms 级任务调度
- 内存利用率: <80% (保留 20%扩展空间)
- 模块耦合度: 降低到低耦合级别
- 代码复用率: >60%

### 1.2 设计原则

1. **分层解耦**: 清晰的层次结构，模块间松耦合
2. **接口标准**: 统一的接口定义和调用规范
3. **配置驱动**: 配置参数化，减少硬编码
4. **错误处理**: 完善的异常处理和恢复机制
5. **可测试性**: 支持单元测试和集成测试
6. **向前兼容**: 保持与现有系统的兼容性

---

## 2. 技术选型与架构设计

### 2.1 核心技术选型

#### 2.1.1 实时操作系统

**推荐方案**: Zephyr RTOS

**选择理由**:

- ✅ 轻量级，适合资源受限环境
- ✅ 模块化设计，可裁剪
- ✅ 丰富的驱动支持
- ✅ 活跃的社区和发展前景
- ✅ 支持设备树配置

**替代方案**: FreeRTOS (备选)

- 成熟稳定，文档丰富
- 学习成本较低
- 社区支持好

#### 2.1.2 通信协议栈

**Modbus 协议栈**:

- 基于 Zephyr Modbus 子系统
- 支持 RTU/ASCII/TCP 多种模式
- 统一的协议接口

**网络协议栈**:

- TCP/IP: Zephyr 网络栈
- LoRa: LoRaWAN 标准协议
- 4G: 标准 AT 命令接口

#### 2.1.3 存储管理

**文件系统**:

- LittleFS (推荐) - 专为嵌入式设计
- FAT (可选) - 兼容性好

**配置管理**:

- 设备树 (Device Tree) - 硬件配置
- KConfig - 软件配置
- NVS (Non-Volatile Storage) - 运行时配置

### 2.2 系统架构设计

#### 2.2.1 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                  应用层 (Application Layer)             │
├─────────────────────────────────────────────────────────┤
│  业务逻辑服务  │  用户界面服务  │  数据管理服务          │
├─────────────────────────────────────────────────────────┤
│                  中间件层 (Middleware Layer)             │
├─────────────────────────────────────────────────────────┤
│  协议栈服务   │  设备管理服务  │  配置管理服务           │
├─────────────────────────────────────────────────────────┤
│                  系统服务层 (System Service Layer)       │
├─────────────────────────────────────────────────────────┤
│  通信服务     │  存储服务     │  时间服务    │  日志服务  │
├─────────────────────────────────────────────────────────┤
│                  硬件抽象层 (HAL Layer)                  │
├─────────────────────────────────────────────────────────┤
│  驱动接口     │  设备接口     │  总线接口    │  IO接口    │
├─────────────────────────────────────────────────────────┤
│                  Zephyr RTOS 内核                       │
├─────────────────────────────────────────────────────────┤
│                  硬件平台 (NANO100B)                     │
└─────────────────────────────────────────────────────────┘
```

#### 2.2.2 任务架构设计

```c
// 任务优先级分配 (数字越小优先级越高)
#define TASK_PRIORITY_CRITICAL      1    // 关键安全任务
#define TASK_PRIORITY_HIGH          3    // 高优先级任务
#define TASK_PRIORITY_NORMAL        5    // 普通任务
#define TASK_PRIORITY_LOW           7    // 低优先级任务
#define TASK_PRIORITY_BACKGROUND    9    // 后台任务

// 主要任务定义
typedef struct {
    const char *name;
    k_thread_entry_t entry;
    size_t stack_size;
    int priority;
    uint32_t options;
} task_config_t;

// 任务配置表
static const task_config_t task_configs[] = {
    {"safety_monitor",  safety_monitor_task,  1024, TASK_PRIORITY_CRITICAL, 0},
    {"sensor_collect",  sensor_collect_task,  2048, TASK_PRIORITY_HIGH,     0},
    {"comm_process",    comm_process_task,    3072, TASK_PRIORITY_HIGH,     0},
    {"data_process",    data_process_task,    2048, TASK_PRIORITY_NORMAL,   0},
    {"ui_display",      ui_display_task,      1536, TASK_PRIORITY_NORMAL,   0},
    {"storage_mgmt",    storage_mgmt_task,    1024, TASK_PRIORITY_LOW,      0},
    {"health_monitor",  health_monitor_task,  1024, TASK_PRIORITY_LOW,      0},
};
```

---

## 3. 功能模块重构规划

### 3.1 核心功能模块架构

#### 3.1.1 数据采集模块 (Sensor Module)

**模块职责**:

- 传感器数据采集
- 数据预处理和校准
- 采集频率控制
- 传感器状态监控

**接口设计**:

```c
// 传感器接口定义
typedef struct {
    int (*init)(const struct device *dev);
    int (*read)(const struct device *dev, sensor_value_t *val);
    int (*config)(const struct device *dev, const sensor_config_t *cfg);
    int (*trigger_set)(const struct device *dev, const sensor_trigger_t *trig);
} sensor_driver_api_t;

// 传感器数据结构
typedef struct {
    float temperature;      // 温度值
    float humidity;         // 湿度值
    uint32_t timestamp;     // 时间戳
    uint8_t quality;        // 数据质量
    uint8_t status;         // 传感器状态
} sensor_reading_t;

// 公共API
int sensor_init(void);
int sensor_read_all(sensor_reading_t *readings, size_t count);
int sensor_register_callback(sensor_callback_t callback);
int sensor_set_sampling_rate(uint32_t rate_ms);
```

#### 3.1.2 通信管理模块 (Communication Module)

**模块职责**:

- 多协议通信管理
- 通信状态监控
- 数据路由和转发
- 连接管理

**协议支持架构**:

```c
// 通信协议枚举
typedef enum {
    COMM_PROTOCOL_MODBUS_RTU,
    COMM_PROTOCOL_MODBUS_TCP,
    COMM_PROTOCOL_LORA,
    COMM_PROTOCOL_4G,
    COMM_PROTOCOL_WIFI,
    COMM_PROTOCOL_COUNT
} comm_protocol_t;

// 通信接口定义
typedef struct {
    int (*init)(const comm_config_t *config);
    int (*send)(const void *data, size_t len);
    int (*receive)(void *buffer, size_t max_len, k_timeout_t timeout);
    int (*connect)(const char *address);
    int (*disconnect)(void);
    comm_status_t (*get_status)(void);
} comm_driver_api_t;

// 消息结构
typedef struct {
    comm_protocol_t protocol;
    uint8_t *payload;
    size_t payload_len;
    uint8_t priority;
    uint32_t timeout_ms;
    char topic[64];         // 用于MQTT等协议
    uint8_t qos;           // 服务质量
} comm_message_t;

// 公共API
int comm_init(void);
int comm_register_protocol(comm_protocol_t proto, const comm_driver_api_t *api);
int comm_send(comm_protocol_t proto, const comm_message_t *msg);
int comm_receive(comm_protocol_t proto, comm_message_t *msg, k_timeout_t timeout);
int comm_set_callback(comm_protocol_t proto, comm_callback_t callback);
```

#### 3.1.3 数据管理模块 (Data Management Module)

**模块职责**:

- 数据存储和检索
- 历史数据管理
- 数据同步和备份
- 数据压缩和归档

**存储架构**:

```c
// 数据类型定义
typedef enum {
    DATA_TYPE_SENSOR,       // 传感器数据
    DATA_TYPE_ALARM,        // 报警数据
    DATA_TYPE_CONFIG,       // 配置数据
    DATA_TYPE_LOG,          // 日志数据
    DATA_TYPE_COUNT
} data_type_t;

// 数据存储接口
typedef struct {
    int (*write)(data_type_t type, const void *data, size_t len);
    int (*read)(data_type_t type, void *buffer, size_t len, uint32_t timestamp);
    int (*delete)(data_type_t type, uint32_t timestamp);
    int (*query)(data_type_t type, uint32_t start_time, uint32_t end_time,
                 data_query_result_t *result);
    int (*backup)(data_type_t type);
    int (*restore)(data_type_t type);
} data_storage_api_t;

// 公共API
int data_mgmt_init(void);
int data_store(data_type_t type, const void *data, size_t len);
int data_retrieve(data_type_t type, void *buffer, size_t len, uint32_t timestamp);
int data_query_range(data_type_t type, uint32_t start, uint32_t end,
                     data_query_result_t *result);
```

#### 3.1.4 报警管理模块 (Alarm Module)

**模块职责**:

- 实时状态监控
- 报警条件检测
- 报警消息生成
- 报警处理和恢复

**报警架构**:

```c
// 报警级别定义
typedef enum {
    ALARM_LEVEL_INFO,       // 信息
    ALARM_LEVEL_WARNING,    // 警告
    ALARM_LEVEL_ERROR,      // 错误
    ALARM_LEVEL_CRITICAL,   // 严重
    ALARM_LEVEL_COUNT
} alarm_level_t;

// 报警类型定义
typedef enum {
    ALARM_TYPE_TEMP_HIGH,      // 温度过高
    ALARM_TYPE_TEMP_LOW,       // 温度过低
    ALARM_TYPE_HUMIDITY_HIGH,  // 湿度过高
    ALARM_TYPE_HUMIDITY_LOW,   // 湿度过低
    ALARM_TYPE_SENSOR_FAULT,   // 传感器故障
    ALARM_TYPE_COMM_FAULT,     // 通信故障
    ALARM_TYPE_POWER_FAULT,    // 电源故障
    ALARM_TYPE_COUNT
} alarm_type_t;

// 报警数据结构
typedef struct {
    alarm_type_t type;
    alarm_level_t level;
    uint32_t timestamp;
    float value;                // 触发值
    float threshold;            // 阈值
    char description[128];      // 描述信息
    uint8_t status;            // 状态 (active/cleared)
} alarm_event_t;

// 公共API
int alarm_init(void);
int alarm_register_rule(alarm_type_t type, const alarm_rule_t *rule);
int alarm_check(const sensor_reading_t *reading);
int alarm_get_active(alarm_event_t *alarms, size_t max_count);
int alarm_acknowledge(alarm_type_t type);
int alarm_clear(alarm_type_t type);
```

#### 3.1.5 配置管理模块 (Configuration Module)

**模块职责**:

- 系统参数管理
- 配置持久化
- 参数校验
- 配置版本管理

**配置架构**:

```c
// 配置类型定义
typedef enum {
    CONFIG_TYPE_SYSTEM,     // 系统配置
    CONFIG_TYPE_SENSOR,     // 传感器配置
    CONFIG_TYPE_COMM,       // 通信配置
    CONFIG_TYPE_ALARM,      // 报警配置
    CONFIG_TYPE_UI,         // 界面配置
    CONFIG_TYPE_COUNT
} config_type_t;

// 配置参数结构
typedef struct {
    char key[32];           // 参数名
    config_value_t value;   // 参数值
    config_type_t type;     // 数据类型
    uint32_t flags;         // 标志位
    char description[64];   // 描述
} config_param_t;

// 公共API
int config_init(void);
int config_set(const char *key, const config_value_t *value);
int config_get(const char *key, config_value_t *value);
int config_save(config_type_t type);
int config_load(config_type_t type);
int config_reset(config_type_t type);
int config_validate(const char *key, const config_value_t *value);
```

### 3.2 用户界面模块重构

#### 3.2.1 显示管理 (Display Module)

**功能增强**:

- 多页面管理
- 动画效果支持
- 主题切换
- 自适应布局

**架构设计**:

```c
// 显示页面定义
typedef enum {
    DISPLAY_PAGE_MAIN,      // 主页面
    DISPLAY_PAGE_SENSOR,    // 传感器页面
    DISPLAY_PAGE_ALARM,     // 报警页面
    DISPLAY_PAGE_SETTING,   // 设置页面
    DISPLAY_PAGE_STATUS,    // 状态页面
    DISPLAY_PAGE_COUNT
} display_page_t;

// 显示元素结构
typedef struct {
    uint8_t x, y;           // 位置
    uint8_t width, height;  // 尺寸
    display_type_t type;    // 类型
    const void *data;       // 数据指针
    uint32_t flags;         // 标志位
} display_element_t;

// 公共API
int display_init(void);
int display_set_page(display_page_t page);
int display_update_element(const display_element_t *element);
int display_clear(void);
int display_set_backlight(uint8_t level);
```

#### 3.2.2 输入管理 (Input Module)

**功能增强**:

- 多种输入方式支持
- 按键组合功能
- 长按短按识别
- 输入事件队列

**架构设计**:

```c
// 输入事件定义
typedef enum {
    INPUT_EVENT_KEY_PRESS,     // 按键按下
    INPUT_EVENT_KEY_RELEASE,   // 按键释放
    INPUT_EVENT_KEY_LONG,      // 长按
    INPUT_EVENT_KEY_COMBO,     // 组合键
    INPUT_EVENT_COUNT
} input_event_type_t;

// 输入事件结构
typedef struct {
    input_event_type_t type;
    uint32_t key_code;
    uint32_t timestamp;
    uint32_t duration;      // 持续时间
} input_event_t;

// 公共API
int input_init(void);
int input_register_callback(input_callback_t callback);
int input_get_event(input_event_t *event, k_timeout_t timeout);
int input_set_key_config(uint32_t key_code, const key_config_t *config);
```

---

## 4. 系统服务重构

### 4.1 时间管理服务

**功能增强**:

- 高精度时间戳
- 时区支持
- NTP 同步
- RTC 备份

**接口设计**:

```c
// 时间服务API
int time_service_init(void);
uint64_t time_get_timestamp_ms(void);
int time_set_rtc(const struct tm *tm);
int time_get_rtc(struct tm *tm);
int time_sync_ntp(const char *server);
int time_set_timezone(int8_t timezone);
```

### 4.2 日志管理服务

**功能设计**:

- 多级别日志
- 自动归档
- 远程日志
- 性能监控

**接口设计**:

```c
// 日志级别定义
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

// 日志API
#define LOG_DBG(fmt, ...) log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...) log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WRN(fmt, ...) log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

int log_init(void);
int log_write(log_level_t level, const char *file, int line, const char *fmt, ...);
int log_set_level(log_level_t level);
int log_enable_remote(bool enable);
```

### 4.3 看门狗服务

**功能增强**:

- 多级看门狗
- 任务监控
- 自动恢复
- 状态记录

**接口设计**:

```c
// 看门狗API
int watchdog_init(void);
int watchdog_register_task(const char *name, uint32_t timeout_ms);
int watchdog_feed(const char *name);
int watchdog_enable(bool enable);
int watchdog_get_status(watchdog_status_t *status);
```

---

## 5. 数据流设计

### 5.1 数据采集流程

```
传感器数据 -> 数据预处理 -> 数据校验 -> 数据存储
           |            |           |
           -> 实时显示   -> 报警检测  -> 远程传输
```

### 5.2 通信数据流程

```
接收数据 -> 协议解析 -> 数据路由 -> 业务处理 -> 响应生成 -> 数据发送
```

### 5.3 配置数据流程

```
配置更新 -> 参数校验 -> 配置持久化 -> 模块通知 -> 配置生效
```

---

## 6. 接口标准化

### 6.1 统一错误码定义

```c
// 通用错误码
typedef enum {
    DTU_OK = 0,                 // 成功
    DTU_ERROR_GENERIC = -1,     // 通用错误
    DTU_ERROR_INVALID_PARAM,    // 参数无效
    DTU_ERROR_TIMEOUT,          // 超时
    DTU_ERROR_BUSY,             // 设备忙
    DTU_ERROR_NOT_READY,        // 设备未就绪
    DTU_ERROR_NO_MEMORY,        // 内存不足
    DTU_ERROR_IO,               // IO错误
    DTU_ERROR_NETWORK,          // 网络错误
    DTU_ERROR_PROTOCOL,         // 协议错误
    DTU_ERROR_COUNT
} dtu_error_t;
```

### 6.2 统一回调函数定义

```c
// 通用回调函数类型
typedef void (*sensor_callback_t)(const sensor_reading_t *reading);
typedef void (*comm_callback_t)(comm_protocol_t proto, const comm_message_t *msg);
typedef void (*alarm_callback_t)(const alarm_event_t *alarm);
typedef void (*input_callback_t)(const input_event_t *event);
```

### 6.3 统一配置结构

```c
// 通用配置值类型
typedef union {
    bool bool_val;
    int32_t int_val;
    float float_val;
    char string_val[64];
    uint8_t bytes_val[32];
} config_value_t;
```

---

## 7. 性能优化策略

### 7.1 内存优化

- 使用内存池管理
- 零拷贝数据传输
- 栈大小优化
- 内存碎片监控

### 7.2 任务调度优化

- 基于优先级的抢占式调度
- 任务时间片配置
- 中断响应优化
- 空闲任务功耗管理

### 7.3 通信优化

- 数据缓冲管理
- 批量数据传输
- 通信压缩算法
- 连接池管理

---

## 8. 安全性设计

### 8.1 数据安全

- 数据加密存储
- 通信数据加密
- 数字签名验证
- 访问权限控制

### 8.2 系统安全

- 安全启动验证
- 代码完整性检查
- 防篡改机制
- 安全固件更新

### 8.3 通信安全

- TLS/SSL 支持
- 证书管理
- 密钥交换
- 身份认证

---

## 9. 测试策略

### 9.1 单元测试

- 每个模块独立测试
- 接口功能测试
- 边界条件测试
- 异常处理测试

### 9.2 集成测试

- 模块间接口测试
- 数据流测试
- 性能测试
- 压力测试

### 9.3 系统测试

- 端到端功能测试
- 可靠性测试
- 兼容性测试
- 用户验收测试

---

## 10. 迁移策略

### 10.1 分阶段迁移

**阶段一: 核心架构**

- RTOS 集成
- 基础服务实现
- 核心模块重构

**阶段二: 功能迁移**

- 数据采集模块
- 通信模块
- 用户界面模块

**阶段三: 增强功能**

- 高级功能实现
- 性能优化
- 安全增强

### 10.2 兼容性保证

- 数据格式兼容
- 配置参数兼容
- 通信协议兼容
- 用户接口兼容

### 10.3 风险控制

- 并行开发策略
- 回滚方案准备
- 充分测试验证
- 分批部署实施

---

**文档更新**: 随重构进展持续更新功能规划
**下一步**: 制定详细的开发计划和时间安排
