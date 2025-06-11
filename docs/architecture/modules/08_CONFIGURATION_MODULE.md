# 配置管理模块设计文档

## 1. 模块概述

### 1.1 模块功能

配置管理模块负责系统所有配置参数的存储、管理和访问，包括设备配置、网络参数、传感器配置、报警阈值等。支持本地配置、远程配置和配置备份恢复功能。

### 1.2 在系统中的作用

- **参数管理**: 统一管理系统所有配置参数
- **存储服务**: 提供持久化存储和快速访问
- **配置同步**: 支持本地和远程配置同步
- **版本控制**: 配置变更历史和版本管理
- **默认配置**: 提供出厂默认配置和重置功能

### 1.3 与其他模块的关系

```
配置管理模块
    ├─→ 所有业务模块 (提供配置参数)
    ├─← 用户界面模块 (接收配置修改)
    ├─← 通信管理模块 (接收远程配置)
    ├─→ 数据管理模块 (配置存储服务)
    └─→ 系统服务模块 (系统级配置)
```

---

## 2. 功能需求

### 2.1 配置分类体系

| 配置类别   | 描述                        | 存储方式 | 优先级 |
| ---------- | --------------------------- | -------- | ------ |
| 系统配置   | 设备 ID、版本信息、硬件配置 | Flash    | P1     |
| 网络配置   | WiFi、4G、LoRa 连接参数     | Flash    | P1     |
| 传感器配置 | 采集间隔、校准参数、阈值    | Flash    | P1     |
| 报警配置   | 报警条件、通知方式、延迟    | Flash    | P1     |
| 界面配置   | 显示设置、语言、亮度        | Flash    | P2     |
| 调试配置   | 日志级别、调试开关          | RAM      | P3     |

### 2.2 配置参数定义

```c
// 系统配置
typedef struct {
    char device_id[32];         // 设备ID
    char device_name[64];       // 设备名称
    uint16_t device_type;       // 设备类型
    char hardware_version[16];  // 硬件版本
    char firmware_version[16];  // 固件版本
    uint32_t manufacture_date;  // 生产日期
    char serial_number[32];     // 序列号
} system_config_t;

// 网络配置
typedef struct {
    // WiFi配置
    char wifi_ssid[32];
    char wifi_password[64];
    bool wifi_enable;

    // 4G配置
    char apn_name[32];
    char apn_user[32];
    char apn_password[32];
    bool mobile_enable;

    // LoRa配置
    uint32_t lora_frequency;
    uint8_t lora_sf;            // 扩频因子
    uint8_t lora_bw;            // 带宽
    uint16_t lora_addr;         // 设备地址
    bool lora_enable;

    // 服务器配置
    char server_host[128];
    uint16_t server_port;
    char mqtt_topic[64];
    uint16_t heartbeat_interval;
} network_config_t;

// 传感器配置
typedef struct {
    uint16_t sample_interval;   // 采样间隔(秒)
    uint8_t sensor_count;       // 传感器数量
    struct {
        uint8_t sensor_type;    // 传感器类型
        uint8_t sensor_addr;    // 传感器地址
        float temp_offset;      // 温度偏移校准
        float humi_offset;      // 湿度偏移校准
        bool enable;            // 是否启用
    } sensors[MAX_SENSOR_COUNT];
} sensor_config_t;

// 报警配置
typedef struct {
    float temp_high_alarm;      // 温度上限
    float temp_low_alarm;       // 温度下限
    float humi_high_alarm;      // 湿度上限
    float humi_low_alarm;       // 湿度下限
    uint16_t alarm_delay;       // 报警延迟(秒)
    bool enable_sms;            // 短信通知
    bool enable_email;          // 邮件通知
    char sms_number[16];        // 短信号码
    char email_addr[64];        // 邮箱地址
} alarm_config_t;
```

### 2.3 性能要求

| 指标       | 要求      | 备注       |
| ---------- | --------- | ---------- |
| 读取速度   | <10ms     | 单个配置项 |
| 写入速度   | <100ms    | 单个配置项 |
| 批量操作   | <500ms    | 所有配置   |
| 内存占用   | <4KB      | 配置缓存   |
| Flash 占用 | <16KB     | 配置存储区 |
| 写入寿命   | >10000 次 | Flash 写入 |

---

## 3. 接口设计

### 3.1 配置管理接口

```c
// 配置模块初始化
int config_init(void);
int config_deinit(void);

// 配置读取接口
int config_get_system(system_config_t *config);
int config_get_network(network_config_t *config);
int config_get_sensor(sensor_config_t *config);
int config_get_alarm(alarm_config_t *config);

// 配置写入接口
int config_set_system(const system_config_t *config);
int config_set_network(const network_config_t *config);
int config_set_sensor(const sensor_config_t *config);
int config_set_alarm(const alarm_config_t *config);

// 通用配置接口
int config_get_value(const char *key, void *value, size_t size);
int config_set_value(const char *key, const void *value, size_t size);
int config_get_string(const char *key, char *value, size_t max_len);
int config_set_string(const char *key, const char *value);
int config_get_int(const char *key, int *value);
int config_set_int(const char *key, int value);
int config_get_float(const char *key, float *value);
int config_set_float(const char *key, float value);

// 配置管理操作
int config_save(void);
int config_load(void);
int config_reset_to_default(void);
int config_backup(void);
int config_restore(void);
bool config_is_modified(void);

// 配置同步接口
int config_sync_from_server(void);
int config_sync_to_server(void);
int config_register_change_callback(config_change_callback_t callback);
```

### 3.2 配置项定义系统

```c
// 配置项属性
typedef enum {
    CONFIG_TYPE_INT8 = 0,
    CONFIG_TYPE_INT16,
    CONFIG_TYPE_INT32,
    CONFIG_TYPE_UINT8,
    CONFIG_TYPE_UINT16,
    CONFIG_TYPE_UINT32,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_BINARY
} config_type_t;

typedef enum {
    CONFIG_ACCESS_READ_ONLY = 0,
    CONFIG_ACCESS_READ_WRITE,
    CONFIG_ACCESS_WRITE_ONLY
} config_access_t;

// 配置项描述符
typedef struct {
    const char *key;            // 配置项键名
    config_type_t type;         // 数据类型
    config_access_t access;     // 访问权限
    size_t size;                // 数据大小
    const void *default_value;  // 默认值
    const void *min_value;      // 最小值(可选)
    const void *max_value;      // 最大值(可选)
    bool persistent;            // 是否持久化
    const char *description;    // 描述信息
} config_item_desc_t;

// 配置组描述符
typedef struct {
    const char *group_name;
    const config_item_desc_t *items;
    size_t item_count;
} config_group_desc_t;
```

### 3.3 配置变更通知

```c
// 配置变更事件
typedef enum {
    CONFIG_EVENT_CHANGED = 0,
    CONFIG_EVENT_SAVED,
    CONFIG_EVENT_LOADED,
    CONFIG_EVENT_RESET,
    CONFIG_EVENT_SYNC_COMPLETE
} config_event_t;

// 配置变更信息
typedef struct {
    config_event_t event;
    const char *key;
    const void *old_value;
    const void *new_value;
    size_t value_size;
    uint32_t timestamp;
} config_change_info_t;

// 配置变更回调
typedef void (*config_change_callback_t)(const config_change_info_t *info);
```

---

## 4. 架构设计

### 4.1 内部架构图

```
┌─────────────────────────────────────────────────────────┐
│                  配置管理模块架构                        │
├─────────────────────────────────────────────────────────┤
│  应用接口层 (Public API)                                │
├─────────────────────────────────────────────────────────┤
│  配置管理层                                             │
│  ├─ 配置注册器   ├─ 变更监听器   ├─ 验证引擎         │
│  └─ 缓存管理器   └─ 同步管理器   └─ 版本管理器       │
├─────────────────────────────────────────────────────────┤
│  存储抽象层                                             │
│  ├─ 内存存储     ├─ Flash存储     ├─ 文件存储        │
│  └─ 远程存储     └─ 备份存储     └─ 默认配置存储     │
├─────────────────────────────────────────────────────────┤
│  序列化层                                               │
│  ├─ JSON序列化   ├─ 二进制序列化 ├─ 压缩处理         │
├─────────────────────────────────────────────────────────┤
│  硬件抽象层                                             │
│  └─ Flash驱动    └─ 文件系统     └─ 网络接口         │
└─────────────────────────────────────────────────────────┘
```

### 4.2 配置存储设计

```c
// 配置存储布局
#define CONFIG_FLASH_BASE       0x78000     // 配置存储起始地址
#define CONFIG_FLASH_SIZE       0x8000      // 配置存储大小(32KB)
#define CONFIG_SECTOR_SIZE      0x1000      // 扇区大小(4KB)

// 配置存储分区
typedef enum {
    CONFIG_PARTITION_SYSTEM = 0,    // 系统配置
    CONFIG_PARTITION_NETWORK,       // 网络配置
    CONFIG_PARTITION_SENSOR,        // 传感器配置
    CONFIG_PARTITION_ALARM,         // 报警配置
    CONFIG_PARTITION_USER,          // 用户配置
    CONFIG_PARTITION_BACKUP,        // 备份配置
    CONFIG_PARTITION_COUNT
} config_partition_t;

// 配置存储头
typedef struct {
    uint32_t magic;                 // 魔数 0x43464730
    uint16_t version;               // 版本号
    uint16_t checksum;              // 校验和
    uint32_t size;                  // 数据大小
    uint32_t timestamp;             // 时间戳
    uint8_t reserved[16];           // 保留字段
} config_header_t;
```

### 4.3 缓存管理设计

```c
// 配置缓存项
typedef struct config_cache_item {
    char key[CONFIG_KEY_MAX_LEN];
    void *value;
    size_t size;
    bool dirty;                     // 是否已修改
    uint32_t timestamp;             // 最后访问时间
    struct config_cache_item *next;
} config_cache_item_t;

// 配置缓存管理器
typedef struct {
    config_cache_item_t *items;
    size_t item_count;
    size_t max_items;
    size_t total_size;
    size_t max_size;
} config_cache_t;
```

---

## 5. 实现方案

### 5.1 技术选型

| 组件     | 选择             | 理由                   |
| -------- | ---------------- | ---------------------- |
| 存储方式 | Flash + LittleFS | 掉电保存，支持磨损均衡 |
| 序列化   | JSON + 二进制    | JSON 可读，二进制紧凑  |
| 校验算法 | CRC16            | 简单高效，适合嵌入式   |
| 缓存策略 | LRU + 写回       | 平衡性能和安全性       |
| 同步协议 | MQTT + JSON      | 标准化，易于扩展       |

### 5.2 关键算法

#### 5.2.1 配置项快速查找

```c
// 使用哈希表加速配置项查找
#define CONFIG_HASH_SIZE 64

typedef struct config_hash_node {
    char key[CONFIG_KEY_MAX_LEN];
    void *value;
    size_t size;
    struct config_hash_node *next;
} config_hash_node_t;

static config_hash_node_t *config_hash_table[CONFIG_HASH_SIZE];

uint32_t config_hash(const char *key) {
    uint32_t hash = 5381;
    while (*key) {
        hash = ((hash << 5) + hash) + *key++;
    }
    return hash % CONFIG_HASH_SIZE;
}

config_hash_node_t *config_find_node(const char *key) {
    uint32_t index = config_hash(key);
    config_hash_node_t *node = config_hash_table[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}
```

#### 5.2.2 配置数据序列化

```c
// JSON序列化实现
int config_serialize_json(const config_group_desc_t *group, char *buffer, size_t size) {
    cJSON *root = cJSON_CreateObject();
    cJSON *group_obj = cJSON_CreateObject();

    for (size_t i = 0; i < group->item_count; i++) {
        const config_item_desc_t *item = &group->items[i];
        void *value = config_get_raw_value(item->key);

        switch (item->type) {
            case CONFIG_TYPE_INT32:
                cJSON_AddNumberToObject(group_obj, item->key, *(int32_t*)value);
                break;
            case CONFIG_TYPE_FLOAT:
                cJSON_AddNumberToObject(group_obj, item->key, *(float*)value);
                break;
            case CONFIG_TYPE_STRING:
                cJSON_AddStringToObject(group_obj, item->key, (char*)value);
                break;
            case CONFIG_TYPE_BOOL:
                cJSON_AddBoolToObject(group_obj, item->key, *(bool*)value);
                break;
        }
    }

    cJSON_AddItemToObject(root, group->group_name, group_obj);

    char *json_string = cJSON_Print(root);
    strncpy(buffer, json_string, size - 1);
    buffer[size - 1] = '\0';

    free(json_string);
    cJSON_Delete(root);

    return strlen(buffer);
}
```

#### 5.2.3 Flash 磨损均衡

```c
// 简单的磨损均衡算法
typedef struct {
    uint32_t address;
    uint32_t erase_count;
    bool in_use;
} config_sector_info_t;

static config_sector_info_t sector_info[CONFIG_SECTOR_COUNT];

uint32_t config_get_next_sector(void) {
    uint32_t min_erase_count = UINT32_MAX;
    uint32_t best_sector = 0;

    // 找到擦除次数最少的空闲扇区
    for (uint32_t i = 0; i < CONFIG_SECTOR_COUNT; i++) {
        if (!sector_info[i].in_use && sector_info[i].erase_count < min_erase_count) {
            min_erase_count = sector_info[i].erase_count;
            best_sector = i;
        }
    }

    return best_sector;
}
```

### 5.3 错误处理

```c
// 配置错误码
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_INVALID_PARAM = -1,
    CONFIG_ERROR_NOT_FOUND = -2,
    CONFIG_ERROR_STORAGE_FULL = -3,
    CONFIG_ERROR_CHECKSUM = -4,
    CONFIG_ERROR_READ_ONLY = -5,
    CONFIG_ERROR_OUT_OF_RANGE = -6,
    CONFIG_ERROR_FLASH_ERROR = -7,
    CONFIG_ERROR_SYNC_FAILED = -8
} config_error_t;

// 错误处理和恢复
int config_handle_error(config_error_t error) {
    switch (error) {
        case CONFIG_ERROR_CHECKSUM:
            // 校验失败，尝试从备份恢复
            config_restore_from_backup();
            break;

        case CONFIG_ERROR_FLASH_ERROR:
            // Flash错误，切换到备用扇区
            config_switch_to_backup_sector();
            break;

        case CONFIG_ERROR_STORAGE_FULL:
            // 存储满，执行垃圾回收
            config_garbage_collect();
            break;

        default:
            CONFIG_LOG_ERROR("Unhandled config error: %d", error);
            break;
    }

    return 0;
}
```

---

## 6. 开发任务分解

### 6.1 阶段一：核心框架 (P1.1)

| 任务 | 描述                    | 估时 | 状态      | 依赖 |
| ---- | ----------------------- | ---- | --------- | ---- |
| T1.1 | ⚫ 创建配置模块基础框架 | 1d   | ⚫ 未开始 | 无   |
| T1.2 | ⚫ 设计配置项描述系统   | 1.5d | ⚫ 未开始 | T1.1 |
| T1.3 | ⚫ 实现配置注册管理     | 2d   | ⚫ 未开始 | T1.2 |
| T1.4 | ⚫ 实现内存缓存机制     | 1.5d | ⚫ 未开始 | T1.3 |
| T1.5 | ⚫ 核心框架功能测试     | 1d   | ⚫ 未开始 | T1.4 |

**里程碑**: 配置核心框架可用

### 6.2 阶段二：存储系统 (P1.2)

| 任务 | 描述                   | 估时 | 状态      | 依赖      |
| ---- | ---------------------- | ---- | --------- | --------- |
| T2.1 | ⚫ 实现 Flash 存储接口 | 2.5d | ⚫ 未开始 | T1.5      |
| T2.2 | ⚫ 实现数据序列化功能  | 2d   | ⚫ 未开始 | T1.5      |
| T2.3 | ⚫ 实现校验和保护      | 1.5d | ⚫ 未开始 | T2.1      |
| T2.4 | ⚫ 实现磨损均衡算法    | 2d   | ⚫ 未开始 | T2.1      |
| T2.5 | ⚫ 存储系统集成测试    | 1d   | ⚫ 未开始 | T2.1-T2.4 |

**里程碑**: 配置存储系统可用

### 6.3 阶段三：高级功能 (P1.3)

| 任务 | 描述                | 估时 | 状态      | 依赖      |
| ---- | ------------------- | ---- | --------- | --------- |
| T3.1 | ⚫ 实现配置备份恢复 | 2d   | ⚫ 未开始 | T2.5      |
| T3.2 | ⚫ 实现配置变更通知 | 1.5d | ⚫ 未开始 | T2.5      |
| T3.3 | ⚫ 实现配置验证机制 | 2d   | ⚫ 未开始 | T2.5      |
| T3.4 | ⚫ 实现默认配置管理 | 1.5d | ⚫ 未开始 | T3.1      |
| T3.5 | ⚫ 高级功能集成测试 | 1d   | ⚫ 未开始 | T3.1-T3.4 |

**里程碑**: 配置高级功能可用

### 6.4 阶段四：远程同步 (P1.4)

| 任务 | 描述                | 估时 | 状态      | 依赖      |
| ---- | ------------------- | ---- | --------- | --------- |
| T4.1 | ⚫ 实现远程配置协议 | 2.5d | ⚫ 未开始 | T3.5      |
| T4.2 | ⚫ 实现配置同步机制 | 2d   | ⚫ 未开始 | T4.1      |
| T4.3 | ⚫ 实现冲突解决策略 | 1.5d | ⚫ 未开始 | T4.2      |
| T4.4 | ⚫ 性能优化和调试   | 1.5d | ⚫ 未开始 | T4.1-T4.3 |
| T4.5 | ⚫ 完整功能测试     | 1.5d | ⚫ 未开始 | T4.4      |

**里程碑**: 配置模块完整功能

### 6.5 总体进度跟踪

| 阶段          | 总任务数 | 已完成 | 进行中 | 未开始 | 完成率 |
| ------------- | -------- | ------ | ------ | ------ | ------ |
| P1.1 核心框架 | 5        | 0      | 0      | 5      | 0%     |
| P1.2 存储系统 | 5        | 0      | 0      | 5      | 0%     |
| P1.3 高级功能 | 5        | 0      | 0      | 5      | 0%     |
| P1.4 远程同步 | 5        | 0      | 0      | 5      | 0%     |
| **总计**      | **20**   | **0**  | **0**  | **20** | **0%** |

---

## 7. 测试计划

### 7.1 单元测试

| 测试项 | 描述                | 覆盖率要求 | 状态      |
| ------ | ------------------- | ---------- | --------- |
| UT1.1  | ⚫ 配置读写接口测试 | >95%       | ⚫ 未开始 |
| UT1.2  | ⚫ 数据序列化测试   | >90%       | ⚫ 未开始 |
| UT1.3  | ⚫ 校验和验证测试   | >95%       | ⚫ 未开始 |
| UT1.4  | ⚫ 缓存管理测试     | >90%       | ⚫ 未开始 |
| UT1.5  | ⚫ 错误处理测试     | >85%       | ⚫ 未开始 |

### 7.2 集成测试

| 测试项 | 描述              | 测试条件       | 状态      |
| ------ | ----------------- | -------------- | --------- |
| IT1.1  | ⚫ 配置持久化测试 | 掉电重启验证   | ⚫ 未开始 |
| IT1.2  | ⚫ 大量配置项测试 | 1000+配置项    | ⚫ 未开始 |
| IT1.3  | ⚫ 并发访问测试   | 多任务同时访问 | ⚫ 未开始 |
| IT1.4  | ⚫ Flash 磨损测试 | 10000 次写入   | ⚫ 未开始 |
| IT1.5  | ⚫ 配置同步测试   | 远程配置下发   | ⚫ 未开始 |

### 7.3 性能测试

| 测试项 | 指标            | 目标值 | 状态      |
| ------ | --------------- | ------ | --------- |
| PT1.1  | ⚫ 配置读取速度 | <10ms  | ⚫ 未开始 |
| PT1.2  | ⚫ 配置写入速度 | <100ms | ⚫ 未开始 |
| PT1.3  | ⚫ 批量操作速度 | <500ms | ⚫ 未开始 |
| PT1.4  | ⚫ 内存占用     | <4KB   | ⚫ 未开始 |
| PT1.5  | ⚫ Flash 占用   | <16KB  | ⚫ 未开始 |

---

## 8. 风险控制

### 8.1 技术风险

| 风险           | 概率 | 影响 | 应对措施            | 状态      |
| -------------- | ---- | ---- | ------------------- | --------- |
| Flash 写入失败 | 中   | 高   | 实现备份机制和重试  | ⚫ 监控中 |
| 配置数据损坏   | 低   | 高   | 校验和保护+备份恢复 | ⚫ 监控中 |
| 同步冲突       | 中   | 中   | 版本控制+冲突解决   | ⚫ 监控中 |

### 8.2 数据安全风险

| 风险           | 概率 | 影响 | 应对措施         | 状态      |
| -------------- | ---- | ---- | ---------------- | --------- |
| 配置被意外清除 | 低   | 高   | 多重备份保护     | ⚫ 监控中 |
| 敏感配置泄露   | 低   | 中   | 关键配置加密存储 | ⚫ 监控中 |

---

## 9. 交付标准

### 9.1 功能验收标准

- ✅ 支持所有配置类型的读写
- ✅ 配置持久化存储正常
- ✅ 配置备份恢复功能可用
- ✅ 远程配置同步正常工作
- ✅ 配置变更通知机制有效

### 9.2 性能验收标准

- ✅ 配置读取速度<10ms
- ✅ 配置写入速度<100ms
- ✅ 内存占用<4KB
- ✅ Flash 写入寿命>10000 次

### 9.3 质量验收标准

- ✅ 代码覆盖率>90%
- ✅ 静态分析 0 严重问题
- ✅ 掉电测试 100%成功
- ✅ 长期运行稳定性测试通过

---

**模块负责人**: [待分配]  
**预计开发时间**: 16 个工作日  
**创建时间**: 2024 年 12 月  
**最后更新**: 2024 年 12 月
