# 冷库 DTU 看门狗机制开发手册

## 1. 概述

### 1.1 目标

为冷库 DTU 系统实现双重看门狗保护机制，确保系统在异常情况下能够自动恢复，提高系统可靠性。

### 1.2 设计原则

- **双重保护**: 硬件看门狗 + 软件看门狗
- **分层监控**: 系统级 + 任务级监控
- **智能恢复**: 异常检测 + 自动恢复
- **可配置性**: 支持参数配置和动态调整

## 2. 硬件看门狗设计

### 2.1 NANO100B 硬件看门狗特性

```c
// NANO100B看门狗寄存器定义
#define WDT_BASE        0x40004000
#define WDT_CTL         (WDT_BASE + 0x00)    // 控制寄存器
#define WDT_ALTCTL      (WDT_BASE + 0x04)    // 替代控制寄存器
#define WDT_RSTCNT      (WDT_BASE + 0x08)    // 重置计数器

// 看门狗超时时间配置
typedef enum {
    WDT_TIMEOUT_2POW4   = 0x0,  // 2^4 * WDT_CLK
    WDT_TIMEOUT_2POW6   = 0x1,  // 2^6 * WDT_CLK
    WDT_TIMEOUT_2POW8   = 0x2,  // 2^8 * WDT_CLK
    WDT_TIMEOUT_2POW10  = 0x3,  // 2^10 * WDT_CLK
    WDT_TIMEOUT_2POW12  = 0x4,  // 2^12 * WDT_CLK
    WDT_TIMEOUT_2POW14  = 0x5,  // 2^14 * WDT_CLK
    WDT_TIMEOUT_2POW16  = 0x6,  // 2^16 * WDT_CLK
    WDT_TIMEOUT_2POW18  = 0x7   // 2^18 * WDT_CLK
} wdt_timeout_t;
```

### 2.2 硬件看门狗驱动实现

```c
// hw_watchdog.h
#ifndef HW_WATCHDOG_H
#define HW_WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>

// 硬件看门狗配置
typedef struct {
    wdt_timeout_t timeout;          // 超时时间
    bool enable_reset;              // 使能系统复位
    bool enable_interrupt;          // 使能中断
    uint32_t clock_source;          // 时钟源选择
} hw_watchdog_config_t;

// API接口
int hw_watchdog_init(const hw_watchdog_config_t* config);
int hw_watchdog_start(void);
int hw_watchdog_stop(void);
int hw_watchdog_feed(void);
int hw_watchdog_get_status(void);
void hw_watchdog_isr(void);

#endif
```

```c
// hw_watchdog.c
#include "hw_watchdog.h"
#include "NUC100Series.h"

static hw_watchdog_config_t g_wdt_config;
static volatile bool g_wdt_timeout_flag = false;

/**
 * @brief 初始化硬件看门狗
 */
int hw_watchdog_init(const hw_watchdog_config_t* config) {
    if (!config) return -1;

    // 保存配置
    g_wdt_config = *config;

    // 使能看门狗时钟
    CLK_EnableModuleClock(WDT_MODULE);

    // 配置看门狗
    uint32_t ctl_reg = 0;
    ctl_reg |= (config->timeout << WDT_CTL_TOUTSEL_Pos);

    if (config->enable_reset) {
        ctl_reg |= WDT_CTL_RSTEN_Msk;
    }

    if (config->enable_interrupt) {
        ctl_reg |= WDT_CTL_INTEN_Msk;
        NVIC_EnableIRQ(WDT_IRQn);
    }

    WDT->CTL = ctl_reg;

    return 0;
}

/**
 * @brief 启动硬件看门狗
 */
int hw_watchdog_start(void) {
    WDT->CTL |= WDT_CTL_WDTEN_Msk;
    return 0;
}

/**
 * @brief 停止硬件看门狗
 */
int hw_watchdog_stop(void) {
    WDT->CTL &= ~WDT_CTL_WDTEN_Msk;
    return 0;
}

/**
 * @brief 喂看门狗
 */
int hw_watchdog_feed(void) {
    WDT->RSTCNT = 0x00005AA5;
    return 0;
}

/**
 * @brief 看门狗中断处理
 */
void WDT_IRQHandler(void) {
    if (WDT->CTL & WDT_CTL_IF_Msk) {
        // 清除中断标志
        WDT->CTL |= WDT_CTL_IF_Msk;

        // 设置超时标志
        g_wdt_timeout_flag = true;

        // 调用用户处理函数
        hw_watchdog_isr();
    }
}

/**
 * @brief 看门狗超时中断处理（用户实现）
 */
__weak void hw_watchdog_isr(void) {
    // 用户可以重写此函数处理看门狗超时
    // 例如：记录日志、保存关键数据等
}
```

## 3. 软件看门狗设计

### 3.1 软件看门狗架构

```c
// sw_watchdog.h
#ifndef SW_WATCHDOG_H
#define SW_WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_MONITORED_TASKS     16
#define TASK_NAME_MAX_LEN       16

// 任务监控状态
typedef enum {
    TASK_STATUS_NORMAL = 0,     // 正常
    TASK_STATUS_WARNING,        // 警告
    TASK_STATUS_TIMEOUT,        // 超时
    TASK_STATUS_DISABLED        // 禁用
} task_status_t;

// 任务监控信息
typedef struct {
    char name[TASK_NAME_MAX_LEN];       // 任务名称
    uint32_t timeout_ms;                // 超时时间(ms)
    uint32_t last_feed_time;            // 最后喂狗时间
    task_status_t status;               // 任务状态
    uint32_t timeout_count;             // 超时次数
    bool is_critical;                   // 是否关键任务
    bool enabled;                       // 是否启用监控
} task_monitor_t;

// 软件看门狗配置
typedef struct {
    uint32_t check_interval_ms;         // 检查间隔
    uint32_t max_timeout_count;         // 最大超时次数
    bool auto_recovery;                 // 自动恢复
    void (*timeout_callback)(const char* task_name, uint32_t timeout_count);
    void (*recovery_callback)(const char* task_name);
} sw_watchdog_config_t;

// API接口
int sw_watchdog_init(const sw_watchdog_config_t* config);
int sw_watchdog_start(void);
int sw_watchdog_stop(void);
int sw_watchdog_register_task(const char* name, uint32_t timeout_ms, bool is_critical);
int sw_watchdog_unregister_task(const char* name);
int sw_watchdog_feed_task(const char* name);
int sw_watchdog_enable_task(const char* name, bool enable);
int sw_watchdog_get_task_status(const char* name, task_monitor_t* status);
void sw_watchdog_process(void);

#endif
```

### 3.2 软件看门狗实现

```c
// sw_watchdog.c
#include "sw_watchdog.h"
#include "system_timer.h"
#include <string.h>
#include <stdio.h>

static sw_watchdog_config_t g_sw_wdt_config;
static task_monitor_t g_task_monitors[MAX_MONITORED_TASKS];
static uint8_t g_task_count = 0;
static bool g_sw_wdt_running = false;

/**
 * @brief 初始化软件看门狗
 */
int sw_watchdog_init(const sw_watchdog_config_t* config) {
    if (!config) return -1;

    g_sw_wdt_config = *config;
    g_task_count = 0;
    g_sw_wdt_running = false;

    // 清空任务监控表
    memset(g_task_monitors, 0, sizeof(g_task_monitors));

    return 0;
}

/**
 * @brief 启动软件看门狗
 */
int sw_watchdog_start(void) {
    g_sw_wdt_running = true;
    return 0;
}

/**
 * @brief 停止软件看门狗
 */
int sw_watchdog_stop(void) {
    g_sw_wdt_running = false;
    return 0;
}

/**
 * @brief 注册任务监控
 */
int sw_watchdog_register_task(const char* name, uint32_t timeout_ms, bool is_critical) {
    if (!name || g_task_count >= MAX_MONITORED_TASKS) {
        return -1;
    }

    // 检查任务是否已存在
    for (int i = 0; i < g_task_count; i++) {
        if (strcmp(g_task_monitors[i].name, name) == 0) {
            return -2; // 任务已存在
        }
    }

    // 添加新任务
    task_monitor_t* task = &g_task_monitors[g_task_count];
    strncpy(task->name, name, TASK_NAME_MAX_LEN - 1);
    task->timeout_ms = timeout_ms;
    task->last_feed_time = system_get_tick();
    task->status = TASK_STATUS_NORMAL;
    task->timeout_count = 0;
    task->is_critical = is_critical;
    task->enabled = true;

    g_task_count++;

    return 0;
}

/**
 * @brief 任务喂狗
 */
int sw_watchdog_feed_task(const char* name) {
    if (!name) return -1;

    for (int i = 0; i < g_task_count; i++) {
        if (strcmp(g_task_monitors[i].name, name) == 0) {
            g_task_monitors[i].last_feed_time = system_get_tick();
            if (g_task_monitors[i].status == TASK_STATUS_TIMEOUT) {
                g_task_monitors[i].status = TASK_STATUS_NORMAL;
                // 调用恢复回调
                if (g_sw_wdt_config.recovery_callback) {
                    g_sw_wdt_config.recovery_callback(name);
                }
            }
            return 0;
        }
    }

    return -1; // 任务未找到
}

/**
 * @brief 软件看门狗处理函数（需要定期调用）
 */
void sw_watchdog_process(void) {
    if (!g_sw_wdt_running) return;

    uint32_t current_time = system_get_tick();

    for (int i = 0; i < g_task_count; i++) {
        task_monitor_t* task = &g_task_monitors[i];

        if (!task->enabled) continue;

        uint32_t elapsed = current_time - task->last_feed_time;

        if (elapsed > task->timeout_ms) {
            // 任务超时
            if (task->status != TASK_STATUS_TIMEOUT) {
                task->status = TASK_STATUS_TIMEOUT;
                task->timeout_count++;

                // 调用超时回调
                if (g_sw_wdt_config.timeout_callback) {
                    g_sw_wdt_config.timeout_callback(task->name, task->timeout_count);
                }

                // 关键任务超时处理
                if (task->is_critical &&
                    task->timeout_count >= g_sw_wdt_config.max_timeout_count) {
                    // 触发系统重启
                    system_reset();
                }
            }
        } else if (elapsed > task->timeout_ms * 0.8) {
            // 警告状态
            task->status = TASK_STATUS_WARNING;
        } else {
            // 正常状态
            task->status = TASK_STATUS_NORMAL;
        }
    }
}
```

## 4. 看门狗管理器

### 4.1 统一管理接口

```c
// watchdog_manager.h
#ifndef WATCHDOG_MANAGER_H
#define WATCHDOG_MANAGER_H

#include "hw_watchdog.h"
#include "sw_watchdog.h"

// 看门狗管理器配置
typedef struct {
    hw_watchdog_config_t hw_config;    // 硬件看门狗配置
    sw_watchdog_config_t sw_config;    // 软件看门狗配置
    bool enable_hw_watchdog;           // 启用硬件看门狗
    bool enable_sw_watchdog;           // 启用软件看门狗
    uint32_t hw_feed_interval_ms;      // 硬件看门狗喂狗间隔
} watchdog_manager_config_t;

// 系统状态
typedef struct {
    bool hw_watchdog_active;           // 硬件看门狗状态
    bool sw_watchdog_active;           // 软件看门狗状态
    uint32_t hw_feed_count;            // 硬件喂狗次数
    uint32_t sw_timeout_count;         // 软件超时次数
    uint32_t system_reset_count;       // 系统重启次数
} watchdog_status_t;

// API接口
int watchdog_manager_init(const watchdog_manager_config_t* config);
int watchdog_manager_start(void);
int watchdog_manager_stop(void);
int watchdog_manager_register_task(const char* name, uint32_t timeout_ms, bool is_critical);
int watchdog_manager_feed_task(const char* name);
int watchdog_manager_get_status(watchdog_status_t* status);
void watchdog_manager_process(void);

#endif
```

### 4.2 管理器实现

```c
// watchdog_manager.c
#include "watchdog_manager.h"
#include "system_timer.h"
#include "system_reset.h"

static watchdog_manager_config_t g_wdt_mgr_config;
static watchdog_status_t g_wdt_status;
static uint32_t g_last_hw_feed_time = 0;

/**
 * @brief 初始化看门狗管理器
 */
int watchdog_manager_init(const watchdog_manager_config_t* config) {
    if (!config) return -1;

    g_wdt_mgr_config = *config;
    memset(&g_wdt_status, 0, sizeof(g_wdt_status));

    // 初始化硬件看门狗
    if (config->enable_hw_watchdog) {
        if (hw_watchdog_init(&config->hw_config) != 0) {
            return -1;
        }
    }

    // 初始化软件看门狗
    if (config->enable_sw_watchdog) {
        if (sw_watchdog_init(&config->sw_config) != 0) {
            return -1;
        }
    }

    return 0;
}

/**
 * @brief 启动看门狗管理器
 */
int watchdog_manager_start(void) {
    // 启动硬件看门狗
    if (g_wdt_mgr_config.enable_hw_watchdog) {
        hw_watchdog_start();
        g_wdt_status.hw_watchdog_active = true;
    }

    // 启动软件看门狗
    if (g_wdt_mgr_config.enable_sw_watchdog) {
        sw_watchdog_start();
        g_wdt_status.sw_watchdog_active = true;
    }

    g_last_hw_feed_time = system_get_tick();

    return 0;
}

/**
 * @brief 看门狗管理器处理函数
 */
void watchdog_manager_process(void) {
    uint32_t current_time = system_get_tick();

    // 处理软件看门狗
    if (g_wdt_mgr_config.enable_sw_watchdog) {
        sw_watchdog_process();
    }

    // 处理硬件看门狗喂狗
    if (g_wdt_mgr_config.enable_hw_watchdog) {
        if (current_time - g_last_hw_feed_time >= g_wdt_mgr_config.hw_feed_interval_ms) {
            hw_watchdog_feed();
            g_wdt_status.hw_feed_count++;
            g_last_hw_feed_time = current_time;
        }
    }
}

/**
 * @brief 软件看门狗超时回调
 */
void sw_watchdog_timeout_callback(const char* task_name, uint32_t timeout_count) {
    printf("Task timeout: %s, count: %d\n", task_name, timeout_count);
    g_wdt_status.sw_timeout_count++;
}

/**
 * @brief 硬件看门狗超时处理
 */
void hw_watchdog_isr(void) {
    // 记录重启原因
    system_set_reset_reason(RESET_REASON_WATCHDOG);

    // 保存关键数据
    system_save_critical_data();

    // 系统将自动重启
}
```

## 5. 系统集成

### 5.1 主循环集成

```c
// main_loop.c 修改
#include "watchdog_manager.h"

// 看门狗配置
static watchdog_manager_config_t wdt_config = {
    .enable_hw_watchdog = true,
    .enable_sw_watchdog = true,
    .hw_feed_interval_ms = 1000,    // 1秒喂一次硬件看门狗
    .hw_config = {
        .timeout = WDT_TIMEOUT_2POW14,  // 约4秒超时
        .enable_reset = true,
        .enable_interrupt = true,
        .clock_source = 0
    },
    .sw_config = {
        .check_interval_ms = 500,       // 500ms检查一次
        .max_timeout_count = 3,         // 最大超时3次
        .auto_recovery = true,
        .timeout_callback = sw_watchdog_timeout_callback,
        .recovery_callback = NULL
    }
};

void Main_loop_Initial(void) {
    // 原有初始化代码...

    // 初始化看门狗管理器
    if (watchdog_manager_init(&wdt_config) != 0) {
        printf("Watchdog manager init failed\n");
    }

    // 注册关键任务
    watchdog_manager_register_task("main_loop", 5000, true);
    watchdog_manager_register_task("sensor_read", 3000, true);
    watchdog_manager_register_task("communication", 10000, false);
    watchdog_manager_register_task("data_process", 2000, false);

    // 启动看门狗
    watchdog_manager_start();
}

void Main_loop_while(void) {
    // 主循环喂狗
    watchdog_manager_feed_task("main_loop");

    // 原有主循环代码...

    // 看门狗处理
    watchdog_manager_process();

    // 其他任务处理...
}
```

### 5.2 任务级集成示例

```c
// sensor_task.c
void sensor_task_process(void) {
    // 任务开始时喂狗
    watchdog_manager_feed_task("sensor_read");

    // 传感器读取逻辑
    if (read_temperature_sensor() != 0) {
        // 传感器读取失败，记录错误
        return;
    }

    if (read_humidity_sensor() != 0) {
        // 传感器读取失败，记录错误
        return;
    }

    // 任务完成时再次喂狗
    watchdog_manager_feed_task("sensor_read");
}

// communication_task.c
void communication_task_process(void) {
    watchdog_manager_feed_task("communication");

    // 通信处理逻辑
    if (process_modbus_communication() != 0) {
        return;
    }

    if (process_lora_communication() != 0) {
        return;
    }

    watchdog_manager_feed_task("communication");
}
```

## 6. 配置和调试

### 6.1 配置参数说明

```c
// watchdog_config.h
#ifndef WATCHDOG_CONFIG_H
#define WATCHDOG_CONFIG_H

// 硬件看门狗配置
#define HW_WDT_TIMEOUT_MS       4000    // 硬件看门狗超时时间
#define HW_WDT_FEED_INTERVAL    1000    // 喂狗间隔

// 软件看门狗配置
#define SW_WDT_CHECK_INTERVAL   500     // 检查间隔
#define SW_WDT_MAX_TIMEOUT      3       // 最大超时次数

// 任务超时配置
#define MAIN_LOOP_TIMEOUT       5000    // 主循环超时
#define SENSOR_TASK_TIMEOUT     3000    // 传感器任务超时
#define COMM_TASK_TIMEOUT       10000   // 通信任务超时
#define DATA_TASK_TIMEOUT       2000    // 数据处理超时

#endif
```

### 6.2 调试接口

```c
// watchdog_debug.h
#ifndef WATCHDOG_DEBUG_H
#define WATCHDOG_DEBUG_H

// 调试命令
typedef enum {
    WDT_DEBUG_GET_STATUS = 0,
    WDT_DEBUG_GET_TASKS,
    WDT_DEBUG_RESET_COUNTERS,
    WDT_DEBUG_SIMULATE_TIMEOUT,
    WDT_DEBUG_ENABLE_TASK,
    WDT_DEBUG_DISABLE_TASK
} wdt_debug_cmd_t;

// 调试接口
int watchdog_debug_command(wdt_debug_cmd_t cmd, const char* param);
void watchdog_debug_print_status(void);
void watchdog_debug_print_tasks(void);

#endif
```

## 7. 测试验证

### 7.1 单元测试

```c
// test_watchdog.c
#include "unity.h"
#include "watchdog_manager.h"

void test_watchdog_init(void) {
    watchdog_manager_config_t config = {0};
    TEST_ASSERT_EQUAL(0, watchdog_manager_init(&config));
}

void test_task_registration(void) {
    TEST_ASSERT_EQUAL(0, watchdog_manager_register_task("test_task", 1000, false));
    TEST_ASSERT_EQUAL(-2, watchdog_manager_register_task("test_task", 1000, false)); // 重复注册
}

void test_task_feeding(void) {
    watchdog_manager_register_task("feed_test", 1000, false);
    TEST_ASSERT_EQUAL(0, watchdog_manager_feed_task("feed_test"));
    TEST_ASSERT_EQUAL(-1, watchdog_manager_feed_task("nonexistent")); // 不存在的任务
}

void test_timeout_detection(void) {
    // 注册短超时任务
    watchdog_manager_register_task("timeout_test", 100, false);

    // 等待超时
    delay_ms(200);
    watchdog_manager_process();

    // 检查任务状态
    task_monitor_t status;
    watchdog_manager_get_task_status("timeout_test", &status);
    TEST_ASSERT_EQUAL(TASK_STATUS_TIMEOUT, status.status);
}
```

### 7.2 集成测试

```c
// 系统级测试
void test_system_recovery(void) {
    // 模拟系统异常
    simulate_system_hang();

    // 等待看门狗复位
    delay_ms(5000);

    // 检查系统是否重启
    TEST_ASSERT_TRUE(system_was_reset_by_watchdog());
}
```

## 8. 注意事项

### 8.1 硬件要求

- ✅ **无需硬件改动**: NANO100B 内置硬件看门狗
- ✅ **时钟配置**: 确保看门狗时钟源稳定
- ✅ **复位电路**: 确保复位电路正常工作

### 8.2 软件要求

- 定期调用`watchdog_manager_process()`
- 关键任务必须及时喂狗
- 合理设置超时时间
- 避免在中断中进行复杂的看门狗操作

### 8.3 性能影响

- 硬件看门狗：几乎无性能影响
- 软件看门狗：每次检查约消耗 10-50μs
- 建议检查间隔不小于 100ms

## 9. 故障排除

### 9.1 常见问题

| 问题         | 原因               | 解决方案                         |
| ------------ | ------------------ | -------------------------------- |
| 系统频繁重启 | 看门狗超时时间过短 | 增加超时时间或优化任务执行时间   |
| 看门狗不工作 | 时钟配置错误       | 检查看门狗时钟源配置             |
| 任务超时误报 | 系统时钟不准确     | 校准系统时钟或使用硬件定时器     |
| 喂狗失败     | 任务名称错误       | 检查任务注册和喂狗时的名称一致性 |

### 9.2 调试方法

1. 使用调试接口查看看门狗状态
2. 监控任务超时计数
3. 记录系统重启原因
4. 分析任务执行时间

通过实施此看门狗机制，系统可靠性将显著提升，能够有效防止系统死机并实现自动恢复。
