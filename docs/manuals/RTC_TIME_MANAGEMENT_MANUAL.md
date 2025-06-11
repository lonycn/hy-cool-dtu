# 冷库 DTU RTC 时间管理开发手册

## 1. 概述

### 1.1 目标

为冷库 DTU 系统实现精确的时间管理机制，确保数据时间戳准确性，支持多种时间同步源，提供可靠的定时功能。

### 1.2 设计原则

- **多源同步**: 支持 NTP、4G 网络、GPS、手动设置等多种时间源
- **自动校准**: 定期时间同步和漂移校正
- **容错机制**: 主备时间源切换
- **低功耗**: 优化 RTC 功耗设计

## 2. 硬件 RTC 设计

### 2.1 NANO100B RTC 特性

```c
// NANO100B RTC寄存器定义
#define RTC_BASE        0x40008000
#define RTC_INIT        (RTC_BASE + 0x00)    // 初始化寄存器
#define RTC_RWEN        (RTC_BASE + 0x04)    // 读写使能
#define RTC_FREQADJ     (RTC_BASE + 0x08)    // 频率调整
#define RTC_TIME        (RTC_BASE + 0x0C)    // 时间寄存器
#define RTC_CAL         (RTC_BASE + 0x10)    // 日历寄存器
#define RTC_CLKFMT      (RTC_BASE + 0x14)    // 时钟格式
#define RTC_WEEKDAY     (RTC_BASE + 0x18)    // 星期
#define RTC_TALM        (RTC_BASE + 0x1C)    // 时间闹钟
#define RTC_CALM        (RTC_BASE + 0x20)    // 日历闹钟
#define RTC_LEAPYEAR    (RTC_BASE + 0x24)    // 闰年指示
#define RTC_INTEN       (RTC_BASE + 0x28)    // 中断使能
#define RTC_INTSTS      (RTC_BASE + 0x2C)    // 中断状态
#define RTC_TICK        (RTC_BASE + 0x30)    // 时钟滴答

// RTC时间结构
typedef struct {
    uint16_t year;          // 年 (2000-2099)
    uint8_t month;          // 月 (1-12)
    uint8_t day;            // 日 (1-31)
    uint8_t hour;           // 时 (0-23)
    uint8_t minute;         // 分 (0-59)
    uint8_t second;         // 秒 (0-59)
    uint8_t weekday;        // 星期 (0-6, 0=Sunday)
} rtc_time_t;
```

### 2.2 硬件要求检查

```
⚠️ 硬件改动要求：
1. 外部32.768kHz晶振：NANO100B需要外部32.768kHz晶振作为RTC时钟源
   - 晶振精度：±20ppm或更高
   - 负载电容：通常12.5pF
   - 连接引脚：X32KI (PC.14), X32KO (PC.15)

2. 备用电池（可选）：
   - 3V锂电池（CR2032）
   - 备用电源管理电路
   - 低功耗设计以延长电池寿命

3. PCB布局要求：
   - 32.768kHz晶振尽量靠近MCU
   - 避免高频信号干扰
   - 良好的接地设计
```

### 2.3 RTC 驱动实现

```c
// rtc_driver.h
#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// RTC配置
typedef struct {
    bool enable_24hour_format;      // 24小时格式
    bool enable_leap_year;          // 闰年检测
    uint32_t tick_period;           // 滴答周期
    bool enable_alarm;              // 闹钟功能
} rtc_config_t;

// RTC状态
typedef struct {
    bool is_running;                // RTC运行状态
    bool time_valid;                // 时间有效性
    uint32_t power_on_time;         // 上电时间
    int32_t frequency_offset;       // 频率偏移(ppm)
} rtc_status_t;

// API接口
int rtc_driver_init(const rtc_config_t* config);
int rtc_driver_set_time(const rtc_time_t* time);
int rtc_driver_get_time(rtc_time_t* time);
int rtc_driver_set_alarm(const rtc_time_t* alarm_time);
int rtc_driver_enable_alarm(bool enable);
int rtc_driver_calibrate(int32_t offset_ppm);
int rtc_driver_get_status(rtc_status_t* status);
uint32_t rtc_driver_get_timestamp(void);
bool rtc_driver_is_leap_year(uint16_t year);

#endif
```

```c
// rtc_driver.c
#include "rtc_driver.h"
#include "NUC100Series.h"

static rtc_config_t g_rtc_config;
static bool g_rtc_initialized = false;

/**
 * @brief 初始化RTC驱动
 */
int rtc_driver_init(const rtc_config_t* config) {
    if (!config) return -1;

    g_rtc_config = *config;

    // 使能RTC时钟
    CLK_EnableModuleClock(RTC_MODULE);

    // 等待RTC稳定
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL32K_STB_Msk));

    // 开启RTC写入权限
    RTC_WaitAccessEnable();

    // 配置RTC
    if (config->enable_24hour_format) {
        RTC->CLKFMT |= RTC_CLKFMT_24HEN_Msk;
    }

    // 设置滴答周期
    if (config->tick_period > 0) {
        RTC->TICK = config->tick_period;
        RTC->INTEN |= RTC_INTEN_TICKIEN_Msk;
    }

    // 启用RTC
    RTC->INIT = RTC_INIT_ACTIVE_Msk;

    g_rtc_initialized = true;

    return 0;
}

/**
 * @brief 设置RTC时间
 */
int rtc_driver_set_time(const rtc_time_t* time) {
    if (!time || !g_rtc_initialized) return -1;

    // 验证时间有效性
    if (!is_valid_time(time)) return -2;

    RTC_WaitAccessEnable();

    // 设置时间
    RTC->TIME = (time->hour << RTC_TIME_HR_Pos) |
                (time->minute << RTC_TIME_MIN_Pos) |
                (time->second << RTC_TIME_SEC_Pos);

    // 设置日期
    RTC->CAL = (time->year << RTC_CAL_YEAR_Pos) |
               (time->month << RTC_CAL_MON_Pos) |
               (time->day << RTC_CAL_DAY_Pos);

    // 设置星期
    RTC->WEEKDAY = time->weekday;

    return 0;
}

/**
 * @brief 获取RTC时间
 */
int rtc_driver_get_time(rtc_time_t* time) {
    if (!time || !g_rtc_initialized) return -1;

    uint32_t time_reg = RTC->TIME;
    uint32_t cal_reg = RTC->CAL;

    time->second = (time_reg & RTC_TIME_SEC_Msk) >> RTC_TIME_SEC_Pos;
    time->minute = (time_reg & RTC_TIME_MIN_Msk) >> RTC_TIME_MIN_Pos;
    time->hour = (time_reg & RTC_TIME_HR_Msk) >> RTC_TIME_HR_Pos;

    time->day = (cal_reg & RTC_CAL_DAY_Msk) >> RTC_CAL_DAY_Pos;
    time->month = (cal_reg & RTC_CAL_MON_Msk) >> RTC_CAL_MON_Pos;
    time->year = (cal_reg & RTC_CAL_YEAR_Msk) >> RTC_CAL_YEAR_Pos;

    time->weekday = RTC->WEEKDAY & RTC_WEEKDAY_WEEKDAY_Msk;

    return 0;
}

/**
 * @brief 获取时间戳
 */
uint32_t rtc_driver_get_timestamp(void) {
    rtc_time_t time;
    if (rtc_driver_get_time(&time) != 0) {
        return 0;
    }

    return rtc_time_to_timestamp(&time);
}

/**
 * @brief RTC频率校准
 */
int rtc_driver_calibrate(int32_t offset_ppm) {
    if (!g_rtc_initialized) return -1;

    // NANO100B支持±511ppm的频率调整
    if (offset_ppm < -511 || offset_ppm > 511) {
        return -2;
    }

    RTC_WaitAccessEnable();

    if (offset_ppm >= 0) {
        RTC->FREQADJ = offset_ppm;
    } else {
        RTC->FREQADJ = (1 << 31) | (-offset_ppm);
    }

    return 0;
}
```

## 3. 时间同步系统

### 3.1 时间同步架构

```c
// time_sync.h
#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include "rtc_driver.h"

// 时间同步源
typedef enum {
    TIME_SYNC_SOURCE_NONE = 0,
    TIME_SYNC_SOURCE_NTP,           // NTP服务器
    TIME_SYNC_SOURCE_4G,            // 4G网络时间
    TIME_SYNC_SOURCE_GPS,           // GPS时间
    TIME_SYNC_SOURCE_MANUAL,        // 手动设置
    TIME_SYNC_SOURCE_BACKUP         // 备用时间源
} time_sync_source_t;

// 时间同步状态
typedef enum {
    TIME_SYNC_STATUS_IDLE = 0,
    TIME_SYNC_STATUS_SYNCING,
    TIME_SYNC_STATUS_SUCCESS,
    TIME_SYNC_STATUS_FAILED,
    TIME_SYNC_STATUS_TIMEOUT
} time_sync_status_t;

// 时间同步配置
typedef struct {
    time_sync_source_t primary_source;     // 主时间源
    time_sync_source_t backup_source;      // 备用时间源
    uint32_t sync_interval_s;               // 同步间隔(秒)
    uint32_t sync_timeout_s;                // 同步超时(秒)
    uint32_t max_drift_s;                   // 最大允许漂移(秒)
    char ntp_server[64];                    // NTP服务器地址
    int8_t timezone_offset;                 // 时区偏移(小时)
    bool auto_dst;                          // 自动夏令时
} time_sync_config_t;

// 时间同步信息
typedef struct {
    time_sync_source_t active_source;       // 当前活跃源
    time_sync_status_t status;              // 同步状态
    uint32_t last_sync_time;                // 最后同步时间
    int32_t time_offset_ms;                 // 时间偏移(毫秒)
    uint32_t sync_count;                    // 同步次数
    uint32_t sync_fail_count;               // 同步失败次数
    bool time_valid;                        // 时间有效性
} time_sync_info_t;

// API接口
int time_sync_init(const time_sync_config_t* config);
int time_sync_start(void);
int time_sync_stop(void);
int time_sync_manual(time_sync_source_t source);
int time_sync_set_time(const rtc_time_t* time, time_sync_source_t source);
int time_sync_get_info(time_sync_info_t* info);
void time_sync_process(void);

#endif
```

### 3.2 NTP 时间同步实现

```c
// ntp_client.h
#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <stdint.h>

// NTP数据包结构
typedef struct {
    uint8_t li_vn_mode;         // Leap Indicator, Version, Mode
    uint8_t stratum;            // Stratum
    uint8_t poll;               // Poll interval
    uint8_t precision;          // Precision
    uint32_t root_delay;        // Root delay
    uint32_t root_dispersion;   // Root dispersion
    uint32_t reference_id;      // Reference identifier
    uint64_t reference_ts;      // Reference timestamp
    uint64_t origin_ts;         // Origin timestamp
    uint64_t receive_ts;        // Receive timestamp
    uint64_t transmit_ts;       // Transmit timestamp
} __attribute__((packed)) ntp_packet_t;

// NTP客户端配置
typedef struct {
    char server[64];            // NTP服务器地址
    uint16_t port;              // 端口号(默认123)
    uint32_t timeout_ms;        // 超时时间
    uint8_t retry_count;        // 重试次数
} ntp_client_config_t;

// API接口
int ntp_client_init(const ntp_client_config_t* config);
int ntp_client_sync(rtc_time_t* synced_time, int32_t* offset_ms);
int ntp_client_get_time(const char* server, rtc_time_t* time);

#endif
```

```c
// ntp_client.c
#include "ntp_client.h"
#include "network_interface.h"
#include <string.h>

#define NTP_EPOCH_OFFSET    2208988800UL    // 1900-1970年的秒数差
#define NTP_VERSION         4
#define NTP_MODE_CLIENT     3

static ntp_client_config_t g_ntp_config;

/**
 * @brief 初始化NTP客户端
 */
int ntp_client_init(const ntp_client_config_t* config) {
    if (!config) return -1;

    g_ntp_config = *config;
    if (g_ntp_config.port == 0) {
        g_ntp_config.port = 123;
    }

    return 0;
}

/**
 * @brief NTP时间同步
 */
int ntp_client_sync(rtc_time_t* synced_time, int32_t* offset_ms) {
    if (!synced_time) return -1;

    ntp_packet_t packet = {0};
    int socket_fd = -1;
    int ret = -1;

    // 创建UDP套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        return -1;
    }

    // 设置超时
    struct timeval timeout;
    timeout.tv_sec = g_ntp_config.timeout_ms / 1000;
    timeout.tv_usec = (g_ntp_config.timeout_ms % 1000) * 1000;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // 构造NTP请求包
    packet.li_vn_mode = (0 << 6) | (NTP_VERSION << 3) | NTP_MODE_CLIENT;

    // 设置发送时间戳
    uint64_t send_time = get_ntp_timestamp();
    packet.transmit_ts = htonll(send_time);

    // 发送NTP请求
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(g_ntp_config.port);

    if (inet_aton(g_ntp_config.server, &server_addr.sin_addr) == 0) {
        // 域名解析
        struct hostent* host = gethostbyname(g_ntp_config.server);
        if (!host) {
            goto cleanup;
        }
        memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    }

    if (sendto(socket_fd, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        goto cleanup;
    }

    // 接收NTP响应
    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(socket_fd, &packet, sizeof(packet), 0,
                 (struct sockaddr*)&server_addr, &addr_len) < 0) {
        goto cleanup;
    }

    // 解析NTP响应
    uint64_t receive_time = get_ntp_timestamp();
    uint64_t server_time = ntohll(packet.transmit_ts);

    // 计算时间偏移
    int64_t offset = (int64_t)(server_time - receive_time);
    if (offset_ms) {
        *offset_ms = (int32_t)(offset * 1000 / (1ULL << 32));
    }

    // 转换为本地时间
    uint32_t unix_time = (uint32_t)(server_time >> 32) - NTP_EPOCH_OFFSET;
    timestamp_to_rtc_time(unix_time, synced_time);

    ret = 0;

cleanup:
    if (socket_fd >= 0) {
        close(socket_fd);
    }

    return ret;
}
```

### 3.3 4G 网络时间同步

```c
// gsm_time_sync.h
#ifndef GSM_TIME_SYNC_H
#define GSM_TIME_SYNC_H

#include "rtc_driver.h"

// GSM时间信息
typedef struct {
    rtc_time_t time;            // 时间
    int8_t timezone;            // 时区
    bool dst_active;            // 夏令时状态
    uint8_t signal_quality;     // 信号质量
} gsm_time_info_t;

// API接口
int gsm_time_sync_init(void);
int gsm_time_get_network_time(gsm_time_info_t* time_info);
int gsm_time_sync_rtc(rtc_time_t* synced_time);

#endif
```

```c
// gsm_time_sync.c
#include "gsm_time_sync.h"
#include "sim7600_driver.h"

/**
 * @brief 获取GSM网络时间
 */
int gsm_time_get_network_time(gsm_time_info_t* time_info) {
    if (!time_info) return -1;

    char response[128];

    // 发送AT+CCLK?命令获取网络时间
    if (sim7600_send_command("AT+CCLK?", response, sizeof(response), 5000) != 0) {
        return -1;
    }

    // 解析响应: +CCLK: "24/03/15,10:30:45+32"
    char* time_str = strstr(response, "\"");
    if (!time_str) return -2;

    time_str++; // 跳过引号

    // 解析时间字符串
    int year, month, day, hour, minute, second, timezone;
    if (sscanf(time_str, "%d/%d/%d,%d:%d:%d%d",
               &year, &month, &day, &hour, &minute, &second, &timezone) != 7) {
        return -3;
    }

    // 转换为RTC时间格式
    time_info->time.year = 2000 + year;
    time_info->time.month = month;
    time_info->time.day = day;
    time_info->time.hour = hour;
    time_info->time.minute = minute;
    time_info->time.second = second;
    time_info->timezone = timezone / 4; // 转换为小时

    return 0;
}
```

## 4. 时间管理器

### 4.1 统一时间管理

```c
// time_manager.h
#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "rtc_driver.h"
#include "time_sync.h"

// 时间管理器配置
typedef struct {
    rtc_config_t rtc_config;            // RTC配置
    time_sync_config_t sync_config;     // 同步配置
    bool enable_auto_sync;              // 自动同步
    bool enable_drift_correction;       // 漂移校正
    uint32_t calibration_interval_h;    // 校准间隔(小时)
} time_manager_config_t;

// 时间管理器状态
typedef struct {
    bool rtc_running;                   // RTC运行状态
    bool time_synchronized;             // 时间已同步
    time_sync_source_t active_source;   // 活跃时间源
    uint32_t last_sync_timestamp;       // 最后同步时间戳
    int32_t drift_ppm;                  // 时钟漂移(ppm)
    uint32_t sync_success_count;        // 同步成功次数
    uint32_t sync_fail_count;           // 同步失败次数
} time_manager_status_t;

// API接口
int time_manager_init(const time_manager_config_t* config);
int time_manager_start(void);
int time_manager_stop(void);
int time_manager_get_time(rtc_time_t* time);
int time_manager_set_time(const rtc_time_t* time);
uint32_t time_manager_get_timestamp(void);
int time_manager_sync_now(time_sync_source_t source);
int time_manager_get_status(time_manager_status_t* status);
bool time_manager_is_time_valid(void);
void time_manager_process(void);

#endif
```

### 4.2 时间管理器实现

```c
// time_manager.c
#include "time_manager.h"
#include "ntp_client.h"
#include "gsm_time_sync.h"

static time_manager_config_t g_tm_config;
static time_manager_status_t g_tm_status;
static uint32_t g_last_sync_attempt = 0;
static uint32_t g_last_calibration = 0;

/**
 * @brief 初始化时间管理器
 */
int time_manager_init(const time_manager_config_t* config) {
    if (!config) return -1;

    g_tm_config = *config;
    memset(&g_tm_status, 0, sizeof(g_tm_status));

    // 初始化RTC
    if (rtc_driver_init(&config->rtc_config) != 0) {
        return -1;
    }

    // 初始化时间同步
    if (time_sync_init(&config->sync_config) != 0) {
        return -1;
    }

    // 检查RTC时间有效性
    rtc_time_t current_time;
    if (rtc_driver_get_time(&current_time) == 0) {
        if (current_time.year >= 2024) {
            g_tm_status.time_synchronized = true;
        }
    }

    return 0;
}

/**
 * @brief 启动时间管理器
 */
int time_manager_start(void) {
    g_tm_status.rtc_running = true;

    // 启动时间同步
    if (g_tm_config.enable_auto_sync) {
        time_sync_start();
    }

    return 0;
}

/**
 * @brief 获取当前时间
 */
int time_manager_get_time(rtc_time_t* time) {
    if (!time) return -1;

    return rtc_driver_get_time(time);
}

/**
 * @brief 获取时间戳
 */
uint32_t time_manager_get_timestamp(void) {
    return rtc_driver_get_timestamp();
}

/**
 * @brief 立即同步时间
 */
int time_manager_sync_now(time_sync_source_t source) {
    rtc_time_t synced_time;
    int32_t offset_ms = 0;
    int ret = -1;

    switch (source) {
        case TIME_SYNC_SOURCE_NTP:
            ret = ntp_client_sync(&synced_time, &offset_ms);
            break;

        case TIME_SYNC_SOURCE_4G:
            {
                gsm_time_info_t gsm_time;
                ret = gsm_time_get_network_time(&gsm_time);
                if (ret == 0) {
                    synced_time = gsm_time.time;
                }
            }
            break;

        default:
            return -1;
    }

    if (ret == 0) {
        // 应用时区偏移
        apply_timezone_offset(&synced_time, g_tm_config.sync_config.timezone_offset);

        // 设置RTC时间
        rtc_driver_set_time(&synced_time);

        // 更新状态
        g_tm_status.time_synchronized = true;
        g_tm_status.active_source = source;
        g_tm_status.last_sync_timestamp = time_manager_get_timestamp();
        g_tm_status.sync_success_count++;

        // 如果偏移较大，进行频率校准
        if (abs(offset_ms) > 1000) {
            int32_t drift_ppm = calculate_drift_ppm(offset_ms,
                g_tm_status.last_sync_timestamp - g_last_sync_attempt);
            rtc_driver_calibrate(drift_ppm);
            g_tm_status.drift_ppm = drift_ppm;
        }
    } else {
        g_tm_status.sync_fail_count++;
    }

    g_last_sync_attempt = time_manager_get_timestamp();

    return ret;
}

/**
 * @brief 时间管理器处理函数
 */
void time_manager_process(void) {
    if (!g_tm_status.rtc_running) return;

    uint32_t current_time = time_manager_get_timestamp();

    // 自动时间同步
    if (g_tm_config.enable_auto_sync) {
        uint32_t sync_interval = g_tm_config.sync_config.sync_interval_s;

        if (current_time - g_last_sync_attempt >= sync_interval) {
            // 尝试主时间源同步
            if (time_manager_sync_now(g_tm_config.sync_config.primary_source) != 0) {
                // 主时间源失败，尝试备用时间源
                if (g_tm_config.sync_config.backup_source != TIME_SYNC_SOURCE_NONE) {
                    time_manager_sync_now(g_tm_config.sync_config.backup_source);
                }
            }
        }
    }

    // 定期校准
    if (g_tm_config.enable_drift_correction) {
        uint32_t calibration_interval = g_tm_config.calibration_interval_h * 3600;

        if (current_time - g_last_calibration >= calibration_interval) {
            perform_drift_calibration();
            g_last_calibration = current_time;
        }
    }
}

/**
 * @brief 时间有效性检查
 */
bool time_manager_is_time_valid(void) {
    if (!g_tm_status.time_synchronized) {
        return false;
    }

    // 检查时间是否在合理范围内
    rtc_time_t current_time;
    if (time_manager_get_time(&current_time) != 0) {
        return false;
    }

    // 时间应该在2024年之后
    if (current_time.year < 2024) {
        return false;
    }

    // 检查最后同步时间
    uint32_t time_since_sync = time_manager_get_timestamp() - g_tm_status.last_sync_timestamp;
    uint32_t max_drift_time = g_tm_config.sync_config.max_drift_s;

    return time_since_sync < max_drift_time;
}
```

## 5. 系统集成

### 5.1 主循环集成

```c
// main_loop.c 修改
#include "time_manager.h"

// 时间管理器配置
static time_manager_config_t tm_config = {
    .rtc_config = {
        .enable_24hour_format = true,
        .enable_leap_year = true,
        .tick_period = 0,
        .enable_alarm = false
    },
    .sync_config = {
        .primary_source = TIME_SYNC_SOURCE_NTP,
        .backup_source = TIME_SYNC_SOURCE_4G,
        .sync_interval_s = 3600,        // 1小时同步一次
        .sync_timeout_s = 30,
        .max_drift_s = 86400,           // 24小时最大漂移
        .ntp_server = "pool.ntp.org",
        .timezone_offset = 8,           // 北京时间 UTC+8
        .auto_dst = false
    },
    .enable_auto_sync = true,
    .enable_drift_correction = true,
    .calibration_interval_h = 24        // 24小时校准一次
};

void Main_loop_Initial(void) {
    // 原有初始化代码...

    // 初始化时间管理器
    if (time_manager_init(&tm_config) != 0) {
        printf("Time manager init failed\n");
    }

    // 启动时间管理器
    time_manager_start();

    // 立即进行一次时间同步
    time_manager_sync_now(TIME_SYNC_SOURCE_NTP);
}

void Main_loop_while(void) {
    // 原有主循环代码...

    // 时间管理器处理
    time_manager_process();

    // 其他任务处理...
}
```

### 5.2 数据采集时间戳

```c
// data_collection.c 修改
#include "time_manager.h"

typedef struct {
    uint32_t timestamp;         // 时间戳
    float temperature;          // 温度
    float humidity;             // 湿度
    uint16_t sensor_id;         // 传感器ID
    uint8_t data_quality;       // 数据质量
} sensor_data_t;

void collect_sensor_data(void) {
    sensor_data_t data;

    // 获取当前时间戳
    data.timestamp = time_manager_get_timestamp();

    // 检查时间有效性
    if (!time_manager_is_time_valid()) {
        printf("Warning: Time not synchronized, data timestamp may be inaccurate\n");
        data.data_quality |= DATA_QUALITY_TIME_INVALID;
    }

    // 读取传感器数据
    data.temperature = read_temperature_sensor();
    data.humidity = read_humidity_sensor();
    data.sensor_id = 1;

    // 存储或发送数据
    store_sensor_data(&data);
}
```

## 6. 配置和调试

### 6.1 时间配置接口

```c
// time_config.h
#ifndef TIME_CONFIG_H
#define TIME_CONFIG_H

// 时间配置参数
#define TIME_SYNC_INTERVAL_DEFAULT      3600    // 默认同步间隔(秒)
#define TIME_SYNC_TIMEOUT_DEFAULT       30      // 默认同步超时(秒)
#define TIME_MAX_DRIFT_DEFAULT          86400   // 默认最大漂移(秒)
#define TIME_TIMEZONE_DEFAULT           8       // 默认时区(北京时间)
#define TIME_CALIBRATION_INTERVAL       24      // 默认校准间隔(小时)

// NTP服务器列表
#define NTP_SERVER_PRIMARY              "pool.ntp.org"
#define NTP_SERVER_BACKUP               "time.nist.gov"
#define NTP_SERVER_CHINA                "ntp.aliyun.com"

// 时间配置命令
typedef enum {
    TIME_CMD_SET_TIME = 0,
    TIME_CMD_SYNC_NOW,
    TIME_CMD_SET_TIMEZONE,
    TIME_CMD_SET_NTP_SERVER,
    TIME_CMD_ENABLE_AUTO_SYNC,
    TIME_CMD_GET_STATUS
} time_config_cmd_t;

int time_config_process_command(time_config_cmd_t cmd, const void* param);

#endif
```

### 6.2 调试接口

```c
// time_debug.c
#include "time_manager.h"

void time_debug_print_status(void) {
    time_manager_status_t status;
    rtc_time_t current_time;

    time_manager_get_status(&status);
    time_manager_get_time(&current_time);

    printf("=== Time Manager Status ===\n");
    printf("RTC Running: %s\n", status.rtc_running ? "Yes" : "No");
    printf("Time Synchronized: %s\n", status.time_synchronized ? "Yes" : "No");
    printf("Active Source: %d\n", status.active_source);
    printf("Current Time: %04d-%02d-%02d %02d:%02d:%02d\n",
           current_time.year, current_time.month, current_time.day,
           current_time.hour, current_time.minute, current_time.second);
    printf("Timestamp: %u\n", time_manager_get_timestamp());
    printf("Drift: %d ppm\n", status.drift_ppm);
    printf("Sync Success: %u\n", status.sync_success_count);
    printf("Sync Fail: %u\n", status.sync_fail_count);
    printf("Time Valid: %s\n", time_manager_is_time_valid() ? "Yes" : "No");
}

void time_debug_test_sync(void) {
    printf("Testing NTP sync...\n");
    if (time_manager_sync_now(TIME_SYNC_SOURCE_NTP) == 0) {
        printf("NTP sync successful\n");
    } else {
        printf("NTP sync failed\n");
    }

    printf("Testing 4G sync...\n");
    if (time_manager_sync_now(TIME_SYNC_SOURCE_4G) == 0) {
        printf("4G sync successful\n");
    } else {
        printf("4G sync failed\n");
    }
}
```

## 7. 测试验证

### 7.1 单元测试

```c
// test_time_manager.c
#include "unity.h"
#include "time_manager.h"

void test_rtc_basic_operations(void) {
    rtc_time_t test_time = {
        .year = 2024, .month = 3, .day = 15,
        .hour = 10, .minute = 30, .second = 45
    };

    TEST_ASSERT_EQUAL(0, time_manager_set_time(&test_time));

    rtc_time_t read_time;
    TEST_ASSERT_EQUAL(0, time_manager_get_time(&read_time));

    TEST_ASSERT_EQUAL(test_time.year, read_time.year);
    TEST_ASSERT_EQUAL(test_time.month, read_time.month);
    TEST_ASSERT_EQUAL(test_time.day, read_time.day);
}

void test_timestamp_conversion(void) {
    rtc_time_t time = {2024, 1, 1, 0, 0, 0, 0};
    uint32_t timestamp = rtc_time_to_timestamp(&time);

    rtc_time_t converted_time;
    timestamp_to_rtc_time(timestamp, &converted_time);

    TEST_ASSERT_EQUAL(time.year, converted_time.year);
    TEST_ASSERT_EQUAL(time.month, converted_time.month);
    TEST_ASSERT_EQUAL(time.day, converted_time.day);
}

void test_time_sync(void) {
    // 模拟NTP同步
    TEST_ASSERT_EQUAL(0, time_manager_sync_now(TIME_SYNC_SOURCE_NTP));
    TEST_ASSERT_TRUE(time_manager_is_time_valid());
}
```

## 8. 注意事项

### 8.1 硬件要求总结

```
✅ 必需硬件改动：
1. 32.768kHz晶振及负载电容
2. 晶振布局优化（靠近MCU，避免干扰）

🔄 可选硬件改动：
1. 备用电池电路（提高断电时间保持能力）
2. 温度补偿晶振（提高精度）
```

### 8.2 软件注意事项

- RTC 初始化必须在系统启动早期完成
- 时间同步需要网络连接，注意网络状态检查
- 频繁的时间同步会增加功耗和网络流量
- 时间戳溢出问题（2038 年问题）需要考虑

### 8.3 精度考虑

- 32.768kHz 晶振精度：±20ppm（约 1.7 秒/天）
- NTP 同步精度：通常在几十毫秒内
- 4G 网络时间精度：通常在 1 秒内
- 温度对晶振频率的影响：约-0.04ppm/°C²

## 9. 故障排除

### 9.1 常见问题

| 问题           | 原因                 | 解决方案                     |
| -------------- | -------------------- | ---------------------------- |
| RTC 不工作     | 32.768kHz 晶振未起振 | 检查晶振连接和负载电容       |
| 时间漂移严重   | 晶振精度差或温度影响 | 更换高精度晶振或增加温度补偿 |
| NTP 同步失败   | 网络连接问题         | 检查网络状态和 DNS 解析      |
| 时间跳变       | 多个时间源冲突       | 检查时间源优先级设置         |
| 断电后时间丢失 | 无备用电源           | 添加备用电池电路             |

通过实施此 RTC 时间管理系统，可以确保数据时间戳的准确性，为数据分析和系统运维提供可靠的时间基准。
