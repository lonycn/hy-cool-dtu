# 冷库 DTU 数据缓存补传开发手册

## 1. 概述

### 1.1 目标

为冷库 DTU 系统实现可靠的数据缓存和补传机制，确保在网络异常时数据不丢失，网络恢复后能够自动补传历史数据。

### 1.2 设计原则

- **数据完整性**: 确保所有采集数据都能可靠存储和传输
- **存储优化**: 高效利用有限的 Flash 存储空间
- **智能重传**: 根据网络状态自适应调整重传策略
- **时间戳管理**: 精确记录数据采集时间，支持时序分析

## 2. 存储架构设计

### 2.1 Flash 存储分区规划

```c
// 存储分区定义
#define FLASH_TOTAL_SIZE        0x80000     // 512KB总容量
#define FLASH_PAGE_SIZE         0x200       // 512字节页大小
#define FLASH_SECTOR_SIZE       0x1000      // 4KB扇区大小

// 数据缓存分区 (64KB)
#define DATA_CACHE_BASE_ADDR    0x08060000  // 缓存起始地址
#define DATA_CACHE_SIZE         0x10000     // 64KB缓存空间
#define DATA_CACHE_SECTORS      16          // 16个扇区

// 配置信息分区 (4KB)
#define CONFIG_BASE_ADDR        0x08070000  // 配置起始地址
#define CONFIG_SIZE             0x1000      // 4KB配置空间

// 系统日志分区 (16KB)
#define LOG_BASE_ADDR           0x08074000  // 日志起始地址
#define LOG_SIZE                0x4000      // 16KB日志空间
```

⚠️ **硬件存储要求**:

```
当前NANO100B Flash容量: 512KB
数据缓存需求: 64KB (约可存储8000条传感器数据)
建议硬件升级:
1. 外部SPI Flash (1-4MB) - 提供更大缓存空间
2. 外部EEPROM (64-256KB) - 提供非易失性配置存储
3. SD卡接口 - 提供海量数据存储能力
```

### 2.2 数据结构设计

```c
// data_cache.h
#ifndef DATA_CACHE_H
#define DATA_CACHE_H

#include <stdint.h>
#include <stdbool.h>

// 数据类型定义
typedef enum {
    DATA_TYPE_SENSOR = 0x01,        // 传感器数据
    DATA_TYPE_ALARM = 0x02,         // 告警数据
    DATA_TYPE_STATUS = 0x03,        // 状态数据
    DATA_TYPE_CONFIG = 0x04,        // 配置数据
    DATA_TYPE_LOG = 0x05            // 日志数据
} data_type_t;

// 数据优先级
typedef enum {
    DATA_PRIORITY_LOW = 0,          // 低优先级
    DATA_PRIORITY_NORMAL = 1,       // 普通优先级
    DATA_PRIORITY_HIGH = 2,         // 高优先级
    DATA_PRIORITY_CRITICAL = 3      // 关键优先级
} data_priority_t;

// 缓存数据头部结构
typedef struct {
    uint32_t magic;                 // 魔数标识 0x44545543 ("DTUC")
    uint32_t timestamp;             // 时间戳
    uint16_t data_type;             // 数据类型
    uint16_t data_length;           // 数据长度
    uint8_t priority;               // 优先级
    uint8_t retry_count;            // 重试次数
    uint8_t checksum;               // 校验和
    uint8_t reserved;               // 保留字段
} __attribute__((packed)) cache_data_header_t;

// 完整缓存数据结构
typedef struct {
    cache_data_header_t header;     // 数据头部
    uint8_t data[0];                // 可变长度数据
} __attribute__((packed)) cache_data_t;

// 传感器数据结构
typedef struct {
    uint16_t sensor_id;             // 传感器ID
    float temperature;              // 温度值
    float humidity;                 // 湿度值
    uint16_t battery_voltage;       // 电池电压(mV)
    uint8_t signal_strength;        // 信号强度
    uint8_t data_quality;           // 数据质量标志
} __attribute__((packed)) sensor_data_t;

// 告警数据结构
typedef struct {
    uint16_t alarm_id;              // 告警ID
    uint8_t alarm_type;             // 告警类型
    uint8_t alarm_level;            // 告警级别
    float alarm_value;              // 告警值
    uint32_t alarm_duration;        // 告警持续时间
    char description[32];           // 告警描述
} __attribute__((packed)) alarm_data_t;

#endif
```

### 2.3 存储管理器

```c
// storage_manager.h
#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "data_cache.h"

// 存储管理器配置
typedef struct {
    uint32_t cache_base_addr;       // 缓存基地址
    uint32_t cache_size;            // 缓存大小
    uint32_t max_data_age_s;        // 最大数据保存时间(秒)
    uint8_t wear_leveling_enable;   // 磨损均衡使能
    uint8_t compression_enable;     // 数据压缩使能
} storage_config_t;

// 存储统计信息
typedef struct {
    uint32_t total_space;           // 总空间
    uint32_t used_space;            // 已用空间
    uint32_t free_space;            // 剩余空间
    uint32_t cached_count;          // 缓存数据条数
    uint32_t write_count;           // 写入次数
    uint32_t erase_count;           // 擦除次数
    uint32_t error_count;           // 错误次数
} storage_stats_t;

// API接口
int storage_manager_init(const storage_config_t* config);
int storage_manager_store(const void* data, uint16_t length, data_type_t type, data_priority_t priority);
int storage_manager_retrieve(cache_data_t* data, uint16_t max_length);
int storage_manager_delete(uint32_t data_id);
int storage_manager_get_stats(storage_stats_t* stats);
int storage_manager_cleanup(void);
int storage_manager_format(void);

#endif
```

## 3. 缓存管理实现

### 3.1 循环缓冲区管理

```c
// storage_manager.c
#include "storage_manager.h"
#include "flash_driver.h"
#include "time_manager.h"

// 存储管理器状态
typedef struct {
    uint32_t write_pointer;         // 写指针
    uint32_t read_pointer;          // 读指针
    uint32_t data_count;            // 数据条数
    bool buffer_full;               // 缓冲区满标志
    storage_config_t config;        // 配置信息
    storage_stats_t stats;          // 统计信息
} storage_manager_t;

static storage_manager_t g_storage_mgr;

/**
 * @brief 初始化存储管理器
 */
int storage_manager_init(const storage_config_t* config) {
    if (!config) return -1;

    memset(&g_storage_mgr, 0, sizeof(g_storage_mgr));
    g_storage_mgr.config = *config;

    // 初始化Flash驱动
    if (flash_driver_init() != 0) {
        return -1;
    }

    // 扫描现有数据，恢复指针位置
    if (storage_scan_and_recover() != 0) {
        // 如果恢复失败，格式化存储区域
        storage_manager_format();
    }

    // 初始化统计信息
    g_storage_mgr.stats.total_space = config->cache_size;
    storage_update_stats();

    return 0;
}

/**
 * @brief 存储数据
 */
int storage_manager_store(const void* data, uint16_t length, data_type_t type, data_priority_t priority) {
    if (!data || length == 0) return -1;

    // 检查存储空间
    uint32_t required_space = sizeof(cache_data_header_t) + length;
    if (required_space > g_storage_mgr.stats.free_space) {
        // 空间不足，清理旧数据
        if (storage_cleanup_old_data(required_space) != 0) {
            return -2; // 空间不足
        }
    }

    // 构造数据头部
    cache_data_header_t header;
    header.magic = 0x44545543;      // "DTUC"
    header.timestamp = time_manager_get_timestamp();
    header.data_type = type;
    header.data_length = length;
    header.priority = priority;
    header.retry_count = 0;
    header.checksum = calculate_checksum(data, length);
    header.reserved = 0;

    // 写入数据头部
    uint32_t write_addr = g_storage_mgr.config.cache_base_addr + g_storage_mgr.write_pointer;
    if (flash_driver_write(write_addr, (uint8_t*)&header, sizeof(header)) != 0) {
        g_storage_mgr.stats.error_count++;
        return -3;
    }

    // 写入数据内容
    write_addr += sizeof(header);
    if (flash_driver_write(write_addr, (uint8_t*)data, length) != 0) {
        g_storage_mgr.stats.error_count++;
        return -3;
    }

    // 更新指针和统计信息
    g_storage_mgr.write_pointer += required_space;
    if (g_storage_mgr.write_pointer >= g_storage_mgr.config.cache_size) {
        g_storage_mgr.write_pointer = 0; // 循环回到开始
        g_storage_mgr.buffer_full = true;
    }

    g_storage_mgr.data_count++;
    g_storage_mgr.stats.write_count++;
    storage_update_stats();

    return 0;
}

/**
 * @brief 检索数据
 */
int storage_manager_retrieve(cache_data_t* data, uint16_t max_length) {
    if (!data || max_length < sizeof(cache_data_header_t)) return -1;

    if (g_storage_mgr.data_count == 0) {
        return 1; // 无数据
    }

    // 读取数据头部
    uint32_t read_addr = g_storage_mgr.config.cache_base_addr + g_storage_mgr.read_pointer;
    if (flash_driver_read(read_addr, (uint8_t*)&data->header, sizeof(cache_data_header_t)) != 0) {
        return -2;
    }

    // 验证魔数
    if (data->header.magic != 0x44545543) {
        // 数据损坏，跳过
        storage_skip_corrupted_data();
        return -3;
    }

    // 检查缓冲区大小
    uint32_t total_size = sizeof(cache_data_header_t) + data->header.data_length;
    if (total_size > max_length) {
        return -4; // 缓冲区太小
    }

    // 读取数据内容
    read_addr += sizeof(cache_data_header_t);
    if (flash_driver_read(read_addr, data->data, data->header.data_length) != 0) {
        return -2;
    }

    // 验证校验和
    uint8_t checksum = calculate_checksum(data->data, data->header.data_length);
    if (checksum != data->header.checksum) {
        // 校验失败，数据损坏
        storage_skip_corrupted_data();
        return -5;
    }

    return 0;
}
```

### 3.2 数据压缩和优化

```c
// data_compression.h
#ifndef DATA_COMPRESSION_H
#define DATA_COMPRESSION_H

#include <stdint.h>

// 压缩算法类型
typedef enum {
    COMPRESS_NONE = 0,              // 无压缩
    COMPRESS_RLE,                   // 行程编码
    COMPRESS_LZ77,                  // LZ77压缩
    COMPRESS_DELTA                  // 差分压缩
} compress_type_t;

// 压缩接口
int data_compress(const uint8_t* input, uint16_t input_len,
                 uint8_t* output, uint16_t* output_len, compress_type_t type);
int data_decompress(const uint8_t* input, uint16_t input_len,
                   uint8_t* output, uint16_t* output_len, compress_type_t type);

// 传感器数据差分压缩
int sensor_data_delta_compress(const sensor_data_t* current, const sensor_data_t* previous,
                              uint8_t* compressed, uint16_t* compressed_len);

#endif
```

## 4. 智能重传机制

### 4.1 重传策略管理

```c
// retransmission_manager.h
#ifndef RETRANSMISSION_MANAGER_H
#define RETRANSMISSION_MANAGER_H

#include "data_cache.h"

// 重传策略
typedef enum {
    RETRY_STRATEGY_IMMEDIATE = 0,   // 立即重传
    RETRY_STRATEGY_FIXED_INTERVAL,  // 固定间隔
    RETRY_STRATEGY_EXPONENTIAL,     // 指数退避
    RETRY_STRATEGY_ADAPTIVE         // 自适应
} retry_strategy_t;

// 网络状态
typedef enum {
    NETWORK_STATUS_UNKNOWN = 0,
    NETWORK_STATUS_GOOD,            // 网络良好
    NETWORK_STATUS_POOR,            // 网络较差
    NETWORK_STATUS_DISCONNECTED     // 网络断开
} network_status_t;

// 重传配置
typedef struct {
    retry_strategy_t strategy;      // 重传策略
    uint8_t max_retry_count;        // 最大重试次数
    uint32_t base_interval_ms;      // 基础间隔
    uint32_t max_interval_ms;       // 最大间隔
    float backoff_factor;           // 退避因子
    bool priority_based;            // 基于优先级
} retransmission_config_t;

// 传输任务
typedef struct {
    uint32_t data_id;               // 数据ID
    cache_data_t* data;             // 数据指针
    uint8_t retry_count;            // 重试次数
    uint32_t next_retry_time;       // 下次重试时间
    uint32_t first_attempt_time;    // 首次尝试时间
    bool in_progress;               // 传输中标志
} transmission_task_t;

// API接口
int retransmission_manager_init(const retransmission_config_t* config);
int retransmission_add_task(const cache_data_t* data);
int retransmission_process(void);
int retransmission_update_network_status(network_status_t status);
int retransmission_get_pending_count(void);

#endif
```

### 4.2 自适应重传实现

```c
// retransmission_manager.c
#include "retransmission_manager.h"
#include "network_monitor.h"
#include "communication_interface.h"

#define MAX_TRANSMISSION_TASKS  100

static retransmission_config_t g_retrans_config;
static transmission_task_t g_transmission_tasks[MAX_TRANSMISSION_TASKS];
static uint8_t g_task_count = 0;
static network_status_t g_network_status = NETWORK_STATUS_UNKNOWN;

/**
 * @brief 初始化重传管理器
 */
int retransmission_manager_init(const retransmission_config_t* config) {
    if (!config) return -1;

    g_retrans_config = *config;
    g_task_count = 0;
    memset(g_transmission_tasks, 0, sizeof(g_transmission_tasks));

    return 0;
}

/**
 * @brief 添加传输任务
 */
int retransmission_add_task(const cache_data_t* data) {
    if (!data || g_task_count >= MAX_TRANSMISSION_TASKS) {
        return -1;
    }

    transmission_task_t* task = &g_transmission_tasks[g_task_count];
    task->data_id = generate_data_id();
    task->data = (cache_data_t*)data;
    task->retry_count = 0;
    task->first_attempt_time = time_manager_get_timestamp();
    task->next_retry_time = task->first_attempt_time;
    task->in_progress = false;

    g_task_count++;

    return 0;
}

/**
 * @brief 处理重传任务
 */
int retransmission_process(void) {
    uint32_t current_time = time_manager_get_timestamp();

    for (int i = 0; i < g_task_count; i++) {
        transmission_task_t* task = &g_transmission_tasks[i];

        if (task->in_progress) continue;

        // 检查是否到达重试时间
        if (current_time >= task->next_retry_time) {
            // 检查网络状态
            if (g_network_status == NETWORK_STATUS_DISCONNECTED) {
                // 网络断开，延迟重试
                task->next_retry_time = current_time + 60000; // 1分钟后重试
                continue;
            }

            // 尝试发送数据
            task->in_progress = true;
            int result = send_data_to_server(task->data);

            if (result == 0) {
                // 发送成功，移除任务
                remove_transmission_task(i);
                i--; // 调整索引
            } else {
                // 发送失败，更新重试信息
                task->retry_count++;
                task->in_progress = false;

                if (task->retry_count >= g_retrans_config.max_retry_count) {
                    // 超过最大重试次数，移除任务
                    printf("Data transmission failed after %d retries\n", task->retry_count);
                    remove_transmission_task(i);
                    i--;
                } else {
                    // 计算下次重试时间
                    task->next_retry_time = calculate_next_retry_time(task);
                }
            }
        }
    }

    return 0;
}

/**
 * @brief 计算下次重试时间
 */
static uint32_t calculate_next_retry_time(transmission_task_t* task) {
    uint32_t interval = g_retrans_config.base_interval_ms;
    uint32_t current_time = time_manager_get_timestamp();

    switch (g_retrans_config.strategy) {
        case RETRY_STRATEGY_IMMEDIATE:
            interval = 0;
            break;

        case RETRY_STRATEGY_FIXED_INTERVAL:
            interval = g_retrans_config.base_interval_ms;
            break;

        case RETRY_STRATEGY_EXPONENTIAL:
            interval = g_retrans_config.base_interval_ms *
                      (1 << task->retry_count) * g_retrans_config.backoff_factor;
            if (interval > g_retrans_config.max_interval_ms) {
                interval = g_retrans_config.max_interval_ms;
            }
            break;

        case RETRY_STRATEGY_ADAPTIVE:
            // 根据网络状态和数据优先级调整间隔
            interval = adaptive_calculate_interval(task);
            break;
    }

    // 根据网络状态调整间隔
    switch (g_network_status) {
        case NETWORK_STATUS_POOR:
            interval *= 2; // 网络较差时延长间隔
            break;
        case NETWORK_STATUS_DISCONNECTED:
            interval = 60000; // 网络断开时1分钟后重试
            break;
        default:
            break;
    }

    return current_time + interval;
}

/**
 * @brief 自适应间隔计算
 */
static uint32_t adaptive_calculate_interval(transmission_task_t* task) {
    uint32_t base_interval = g_retrans_config.base_interval_ms;

    // 根据数据优先级调整
    switch (task->data->header.priority) {
        case DATA_PRIORITY_CRITICAL:
            base_interval /= 4; // 关键数据快速重试
            break;
        case DATA_PRIORITY_HIGH:
            base_interval /= 2; // 高优先级数据较快重试
            break;
        case DATA_PRIORITY_LOW:
            base_interval *= 2; // 低优先级数据延长间隔
            break;
        default:
            break;
    }

    // 根据重试次数调整
    base_interval *= (1 + task->retry_count * 0.5);

    // 根据数据年龄调整
    uint32_t data_age = time_manager_get_timestamp() - task->data->header.timestamp;
    if (data_age > 3600) { // 超过1小时的数据
        base_interval *= 2;
    }

    return base_interval;
}
```

## 5. 网络状态监控

### 5.1 网络质量评估

```c
// network_quality.h
#ifndef NETWORK_QUALITY_H
#define NETWORK_QUALITY_H

#include <stdint.h>
#include <stdbool.h>

// 网络质量指标
typedef struct {
    uint8_t signal_strength;        // 信号强度 (0-100)
    uint32_t latency_ms;            // 延迟 (毫秒)
    uint8_t packet_loss_rate;       // 丢包率 (0-100)
    uint32_t bandwidth_kbps;        // 带宽 (kbps)
    bool is_stable;                 // 连接稳定性
} network_quality_t;

// 网络统计信息
typedef struct {
    uint32_t total_attempts;        // 总尝试次数
    uint32_t success_count;         // 成功次数
    uint32_t failure_count;         // 失败次数
    uint32_t timeout_count;         // 超时次数
    uint32_t avg_response_time;     // 平均响应时间
    float success_rate;             // 成功率
} network_stats_t;

// API接口
int network_quality_init(void);
int network_quality_measure(network_quality_t* quality);
int network_quality_get_stats(network_stats_t* stats);
network_status_t network_quality_evaluate(const network_quality_t* quality);

#endif
```

### 5.2 智能传输调度

```c
// transmission_scheduler.c
#include "transmission_scheduler.h"

/**
 * @brief 智能传输调度
 */
int transmission_scheduler_process(void) {
    network_quality_t quality;

    // 测量网络质量
    if (network_quality_measure(&quality) != 0) {
        return -1;
    }

    // 评估网络状态
    network_status_t status = network_quality_evaluate(&quality);
    retransmission_update_network_status(status);

    // 根据网络状态调整传输策略
    switch (status) {
        case NETWORK_STATUS_GOOD:
            // 网络良好，正常传输
            set_transmission_batch_size(10);
            set_transmission_interval(1000);
            break;

        case NETWORK_STATUS_POOR:
            // 网络较差，减少批量大小，增加间隔
            set_transmission_batch_size(3);
            set_transmission_interval(5000);
            break;

        case NETWORK_STATUS_DISCONNECTED:
            // 网络断开，停止传输
            pause_transmission();
            break;

        default:
            break;
    }

    // 处理重传任务
    retransmission_process();

    return 0;
}
```

## 6. 系统集成

### 6.1 数据采集集成

```c
// data_collection.c 修改
#include "storage_manager.h"
#include "retransmission_manager.h"
#include "time_manager.h"

void collect_and_cache_sensor_data(void) {
    sensor_data_t sensor_data;

    // 读取传感器数据
    sensor_data.sensor_id = 1;
    sensor_data.temperature = read_temperature_sensor();
    sensor_data.humidity = read_humidity_sensor();
    sensor_data.battery_voltage = read_battery_voltage();
    sensor_data.signal_strength = get_signal_strength();
    sensor_data.data_quality = 0;

    // 检查时间有效性
    if (!time_manager_is_time_valid()) {
        sensor_data.data_quality |= 0x01; // 时间无效标志
    }

    // 检查传感器数据有效性
    if (sensor_data.temperature < -50 || sensor_data.temperature > 100) {
        sensor_data.data_quality |= 0x02; // 温度异常标志
    }

    // 存储到缓存
    int result = storage_manager_store(&sensor_data, sizeof(sensor_data),
                                     DATA_TYPE_SENSOR, DATA_PRIORITY_NORMAL);
    if (result != 0) {
        printf("Failed to cache sensor data: %d\n", result);
        return;
    }

    // 尝试立即发送
    if (is_network_available()) {
        cache_data_t cached_data;
        if (storage_manager_retrieve(&cached_data, sizeof(cached_data)) == 0) {
            if (send_data_to_server(&cached_data) == 0) {
                // 发送成功，删除缓存数据
                storage_manager_delete(cached_data.header.timestamp);
            } else {
                // 发送失败，添加到重传队列
                retransmission_add_task(&cached_data);
            }
        }
    } else {
        // 网络不可用，数据保留在缓存中
        printf("Network unavailable, data cached for later transmission\n");
    }
}
```

### 6.2 主循环集成

```c
// main_loop.c 修改
#include "storage_manager.h"
#include "retransmission_manager.h"
#include "transmission_scheduler.h"

// 存储和重传配置
static storage_config_t storage_config = {
    .cache_base_addr = DATA_CACHE_BASE_ADDR,
    .cache_size = DATA_CACHE_SIZE,
    .max_data_age_s = 86400 * 7,    // 7天
    .wear_leveling_enable = 1,
    .compression_enable = 1
};

static retransmission_config_t retrans_config = {
    .strategy = RETRY_STRATEGY_ADAPTIVE,
    .max_retry_count = 5,
    .base_interval_ms = 5000,       // 5秒基础间隔
    .max_interval_ms = 300000,      // 5分钟最大间隔
    .backoff_factor = 1.5,
    .priority_based = true
};

void Main_loop_Initial(void) {
    // 原有初始化代码...

    // 初始化存储管理器
    if (storage_manager_init(&storage_config) != 0) {
        printf("Storage manager init failed\n");
    }

    // 初始化重传管理器
    if (retransmission_manager_init(&retrans_config) != 0) {
        printf("Retransmission manager init failed\n");
    }

    // 初始化传输调度器
    if (transmission_scheduler_init() != 0) {
        printf("Transmission scheduler init failed\n");
    }
}

void Main_loop_while(void) {
    static uint32_t last_data_collection = 0;
    static uint32_t last_transmission_check = 0;
    static uint32_t last_cleanup = 0;

    uint32_t current_time = time_manager_get_timestamp();

    // 数据采集 (每30秒)
    if (current_time - last_data_collection >= 30) {
        collect_and_cache_sensor_data();
        last_data_collection = current_time;
    }

    // 传输调度 (每5秒)
    if (current_time - last_transmission_check >= 5) {
        transmission_scheduler_process();
        last_transmission_check = current_time;
    }

    // 存储清理 (每1小时)
    if (current_time - last_cleanup >= 3600) {
        storage_manager_cleanup();
        last_cleanup = current_time;
    }

    // 原有主循环代码...
}
```

## 7. 配置和调试

### 7.1 存储配置接口

```c
// storage_config.h
#ifndef STORAGE_CONFIG_H
#define STORAGE_CONFIG_H

// 存储配置参数
#define STORAGE_CACHE_SIZE_DEFAULT      65536   // 64KB默认缓存
#define STORAGE_MAX_DATA_AGE_DEFAULT    604800  // 7天默认保存时间
#define STORAGE_COMPRESSION_RATIO       0.7     // 压缩比例

// 重传配置参数
#define RETRANS_MAX_RETRY_DEFAULT       5       // 默认最大重试次数
#define RETRANS_BASE_INTERVAL_DEFAULT   5000    // 默认基础间隔5秒
#define RETRANS_MAX_INTERVAL_DEFAULT    300000  // 默认最大间隔5分钟

// 配置命令
typedef enum {
    STORAGE_CMD_SET_CACHE_SIZE = 0,
    STORAGE_CMD_SET_MAX_AGE,
    STORAGE_CMD_ENABLE_COMPRESSION,
    STORAGE_CMD_FORMAT_CACHE,
    STORAGE_CMD_GET_STATS
} storage_config_cmd_t;

int storage_config_process_command(storage_config_cmd_t cmd, const void* param);

#endif
```

### 7.2 调试接口

```c
// cache_debug.c
#include "storage_manager.h"
#include "retransmission_manager.h"

void cache_debug_print_stats(void) {
    storage_stats_t stats;
    storage_manager_get_stats(&stats);

    printf("=== Storage Statistics ===\n");
    printf("Total Space: %u bytes\n", stats.total_space);
    printf("Used Space: %u bytes (%.1f%%)\n", stats.used_space,
           (float)stats.used_space * 100 / stats.total_space);
    printf("Free Space: %u bytes\n", stats.free_space);
    printf("Cached Count: %u\n", stats.cached_count);
    printf("Write Count: %u\n", stats.write_count);
    printf("Erase Count: %u\n", stats.erase_count);
    printf("Error Count: %u\n", stats.error_count);

    printf("\n=== Retransmission Status ===\n");
    printf("Pending Tasks: %d\n", retransmission_get_pending_count());
}

void cache_debug_dump_data(void) {
    cache_data_t data;
    int count = 0;

    printf("=== Cached Data Dump ===\n");
    while (storage_manager_retrieve(&data, sizeof(data)) == 0 && count < 10) {
        printf("Data %d:\n", count);
        printf("  Timestamp: %u\n", data.header.timestamp);
        printf("  Type: %u\n", data.header.data_type);
        printf("  Length: %u\n", data.header.data_length);
        printf("  Priority: %u\n", data.header.priority);
        printf("  Retry Count: %u\n", data.header.retry_count);
        count++;
    }
}
```

## 8. 测试验证

### 8.1 单元测试

```c
// test_data_cache.c
#include "unity.h"
#include "storage_manager.h"

void test_storage_basic_operations(void) {
    sensor_data_t test_data = {
        .sensor_id = 1,
        .temperature = 25.5,
        .humidity = 60.0,
        .battery_voltage = 3300,
        .signal_strength = 80,
        .data_quality = 0
    };

    // 测试存储
    TEST_ASSERT_EQUAL(0, storage_manager_store(&test_data, sizeof(test_data),
                                             DATA_TYPE_SENSOR, DATA_PRIORITY_NORMAL));

    // 测试检索
    cache_data_t retrieved_data;
    TEST_ASSERT_EQUAL(0, storage_manager_retrieve(&retrieved_data, sizeof(retrieved_data)));

    // 验证数据
    sensor_data_t* sensor = (sensor_data_t*)retrieved_data.data;
    TEST_ASSERT_EQUAL(test_data.sensor_id, sensor->sensor_id);
    TEST_ASSERT_FLOAT_WITHIN(0.1, test_data.temperature, sensor->temperature);
}

void test_storage_overflow_handling(void) {
    // 填满存储空间
    sensor_data_t test_data = {0};
    int stored_count = 0;

    while (storage_manager_store(&test_data, sizeof(test_data),
                               DATA_TYPE_SENSOR, DATA_PRIORITY_NORMAL) == 0) {
        stored_count++;
        if (stored_count > 10000) break; // 防止无限循环
    }

    // 验证能够继续存储（通过清理旧数据）
    TEST_ASSERT_EQUAL(0, storage_manager_store(&test_data, sizeof(test_data),
                                             DATA_TYPE_SENSOR, DATA_PRIORITY_HIGH));
}
```

## 9. 性能优化

### 9.1 存储优化策略

```c
// 批量写入优化
int storage_batch_write(const cache_data_t* data_array, uint8_t count) {
    // 计算总大小
    uint32_t total_size = 0;
    for (int i = 0; i < count; i++) {
        total_size += sizeof(cache_data_header_t) + data_array[i].header.data_length;
    }

    // 检查空间
    if (total_size > g_storage_mgr.stats.free_space) {
        storage_cleanup_old_data(total_size);
    }

    // 批量写入
    uint32_t write_addr = g_storage_mgr.config.cache_base_addr + g_storage_mgr.write_pointer;
    for (int i = 0; i < count; i++) {
        uint32_t data_size = sizeof(cache_data_header_t) + data_array[i].header.data_length;
        flash_driver_write(write_addr, (uint8_t*)&data_array[i], data_size);
        write_addr += data_size;
    }

    // 更新指针
    g_storage_mgr.write_pointer += total_size;
    if (g_storage_mgr.write_pointer >= g_storage_mgr.config.cache_size) {
        g_storage_mgr.write_pointer -= g_storage_mgr.config.cache_size;
    }

    return 0;
}
```

## 10. 故障排除

### 10.1 常见问题

| 问题         | 原因             | 解决方案                   |
| ------------ | ---------------- | -------------------------- |
| 存储空间不足 | 数据积累过多     | 增加清理频率或扩大存储空间 |
| 数据校验失败 | Flash 损坏或干扰 | 增加 ECC 校验或更换 Flash  |
| 重传失败     | 网络持续异常     | 调整重传策略或增加离线存储 |
| 时间戳异常   | RTC 未同步       | 检查时间同步机制           |
| 性能下降     | Flash 磨损       | 启用磨损均衡算法           |

通过实施此数据缓存补传系统，可以确保在各种网络条件下数据的完整性和可靠传输，显著提升系统的可靠性。
