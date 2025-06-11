# OTA 远程升级模块设计文档

## 1. 模块概述

### 1.1 模块功能

OTA（Over-The-Air）远程升级模块提供固件的远程更新能力，支持通过多种通信方式（4G/LoRa/WiFi）进行固件下载、验证、安装和回滚，确保系统的可维护性和可扩展性。

### 1.2 在系统中的作用

- **远程维护**: 支持远程固件升级，降低维护成本
- **安全升级**: 提供固件验证、数字签名、回滚机制
- **多通道支持**: 兼容现有的多种通信接口
- **断点续传**: 支持网络中断后的续传功能
- **状态监控**: 提供详细的升级状态反馈

### 1.3 与其他模块的关系

```
OTA 远程升级模块
    ├─← 通信管理模块 (网络传输服务)
    ├─← 配置管理模块 (升级配置参数)
    ├─→ 数据管理模块 (升级日志存储)
    ├─→ 用户界面模块 (升级状态显示)
    ├─→ 报警管理模块 (升级异常报警)
    └─→ 系统服务模块 (系统重启控制)
```

---

## 2. 功能需求

### 2.1 核心功能

| 功能模块 | 描述                     | 优先级 |
| -------- | ------------------------ | ------ |
| 升级检查 | 检查是否有新固件版本     | P1     |
| 固件下载 | 从服务器下载固件包       | P1     |
| 安全验证 | 校验和验证、数字签名验证 | P1     |
| 固件安装 | 写入新固件到备用分区     | P1     |
| 启动切换 | 切换到新固件启动         | P1     |
| 回滚机制 | 升级失败时回滚到旧版本   | P1     |
| 断点续传 | 网络中断后继续下载       | P2     |
| 状态监控 | 升级进度和状态反馈       | P2     |

### 2.2 支持的传输协议

```c
// 支持的传输协议
typedef enum {
    OTA_TRANSPORT_HTTP = 0,     // HTTP/HTTPS协议
    OTA_TRANSPORT_MQTT,         // MQTT协议
    OTA_TRANSPORT_LORA,         // LoRa协议
    OTA_TRANSPORT_FTP,          // FTP协议
    OTA_TRANSPORT_COUNT
} ota_transport_type_t;
```

### 2.3 固件分区设计

```c
// Flash分区布局 (基于NANO100B 128KB)
#define BOOTLOADER_START        0x08000000  // Bootloader: 8KB
#define BOOTLOADER_SIZE         0x2000

#define APP1_START              0x08002000  // 主应用: 56KB
#define APP1_SIZE               0xE000

#define APP2_START              0x08010000  // 备份应用: 56KB
#define APP2_SIZE               0xE000

#define CONFIG_START            0x0801E000  // 配置区: 4KB
#define CONFIG_SIZE             0x1000

#define OTA_INFO_START          0x0801F000  // OTA信息: 4KB
#define OTA_INFO_SIZE           0x1000
```

### 2.4 性能要求

| 指标     | 要求     | 备注           |
| -------- | -------- | -------------- |
| 下载速度 | 1-10KB/s | 取决于网络条件 |
| 校验时间 | <30s     | 56KB 固件      |
| 安装时间 | <60s     | Flash 写入     |
| 重启时间 | <10s     | 系统重启       |
| 成功率   | >95%     | 正常网络条件   |

---

## 3. 接口设计

### 3.1 主要 API 接口

```c
// OTA模块初始化
int ota_init(const ota_config_t* config);
int ota_deinit(void);

// 升级管理接口
int ota_check_update(ota_info_t* info);
int ota_start_download(const ota_info_t* info, ota_progress_cb_t progress_cb, ota_state_cb_t state_cb);
int ota_pause_download(void);
int ota_resume_download(void);
int ota_cancel_update(void);
int ota_install_firmware(void);
int ota_rollback(void);

// 状态查询接口
ota_state_t ota_get_state(void);
int ota_get_progress(uint32_t* downloaded, uint32_t* total);
int ota_get_version_info(ota_version_info_t* info);

// 回调注册接口
int ota_register_progress_callback(ota_progress_cb_t callback);
int ota_register_state_callback(ota_state_cb_t callback);
int ota_register_error_callback(ota_error_cb_t callback);
```

### 3.2 数据结构定义

```c
// OTA状态枚举
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_INSTALLING,
    OTA_STATE_REBOOTING,
    OTA_STATE_COMPLETED,
    OTA_STATE_FAILED,
    OTA_STATE_ROLLBACK
} ota_state_t;

// OTA配置结构
typedef struct {
    ota_transport_type_t transport_type;
    char server_url[256];
    uint16_t server_port;
    char device_id[64];
    char api_key[128];
    uint32_t timeout_ms;
    uint32_t retry_count;
    uint32_t chunk_size;
    bool enable_signature_verify;
    bool enable_encryption;
    ota_checksum_type_t checksum_type;
} ota_config_t;

// 固件信息结构
typedef struct {
    char version[32];
    uint32_t size;
    char url[256];
    char checksum[64];
    char signature[512];
    uint32_t timestamp;
    char release_notes[256];
} ota_info_t;

// 版本信息结构
typedef struct {
    char current_version[32];
    char hardware_version[32];
    uint32_t build_time;
    char build_info[64];
} ota_version_info_t;

// 升级进度信息
typedef struct {
    uint32_t downloaded_bytes;
    uint32_t total_bytes;
    uint8_t percentage;
    uint32_t speed_bps;
    uint32_t remaining_time;
} ota_progress_info_t;
```

### 3.3 回调函数定义

```c
// 进度回调函数
typedef void (*ota_progress_cb_t)(const ota_progress_info_t* progress);

// 状态回调函数
typedef void (*ota_state_cb_t)(ota_state_t state, int error_code);

// 错误回调函数
typedef void (*ota_error_cb_t)(ota_error_t error, const char* message);

// 错误类型枚举
typedef enum {
    OTA_ERROR_NONE = 0,
    OTA_ERROR_NETWORK,
    OTA_ERROR_DOWNLOAD,
    OTA_ERROR_VERIFY,
    OTA_ERROR_INSTALL,
    OTA_ERROR_ROLLBACK,
    OTA_ERROR_TIMEOUT,
    OTA_ERROR_NO_SPACE,
    OTA_ERROR_INVALID_FIRMWARE
} ota_error_t;
```

---

## 4. 架构设计

### 4.1 内部架构图

```
┌─────────────────────────────────────────────────────────┐
│                  OTA 远程升级模块架构                    │
├─────────────────────────────────────────────────────────┤
│  应用接口层 (Public API)                                │
├─────────────────────────────────────────────────────────┤
│  升级管理层                                             │
│  ├─ 升级控制器   ├─ 状态管理器   ├─ 进度监控器       │
│  └─ 错误处理器   └─ 回滚管理器   └─ 版本管理器       │
├─────────────────────────────────────────────────────────┤
│  协议处理层                                             │
│  ├─ HTTP客户端   ├─ MQTT客户端   ├─ LoRa传输         │
│  └─ 断点续传     └─ 重试机制     └─ 传输加密         │
├─────────────────────────────────────────────────────────┤
│  安全验证层                                             │
│  ├─ 校验和验证   ├─ 数字签名     ├─ 固件解密         │
│  └─ 完整性检查   └─ 版本验证     └─ 权限验证         │
├─────────────────────────────────────────────────────────┤
│  存储管理层                                             │
│  ├─ Flash分区    ├─ 固件写入     ├─ 备份管理         │
│  └─ 磨损均衡     └─ 坏块管理     └─ 启动切换         │
├─────────────────────────────────────────────────────────┤
│  硬件抽象层                                             │
│  └─ Flash驱动    └─ 网络接口     └─ 看门狗控制       │
└─────────────────────────────────────────────────────────┘
```

### 4.2 状态机设计

```c
// OTA状态机
typedef struct {
    ota_state_t current_state;
    ota_state_t previous_state;
    uint32_t state_enter_time;
    uint32_t timeout_ms;
    bool state_changed;
} ota_state_machine_t;

// 状态转换函数
int ota_state_transition(ota_state_t new_state);
bool ota_state_is_timeout(void);
void ota_state_handle_timeout(void);
```

### 4.3 Bootloader 设计

```c
// Bootloader固件头
typedef struct {
    uint32_t magic;           // 魔数 0x4F544131
    uint32_t version;         // 固件版本
    uint32_t size;            // 固件大小
    uint32_t checksum;        // CRC32校验和
    uint32_t timestamp;       // 构建时间戳
    uint32_t entry_point;     // 入口地址
    uint8_t signature[256];   // 数字签名
    uint8_t reserved[244];    // 保留字段
} firmware_header_t;

// 启动信息
typedef struct {
    uint32_t magic;           // 魔数 0x424F4F54
    uint32_t active_slot;     // 活动分区 (0=APP1, 1=APP2)
    uint32_t boot_count;      // 启动计数
    uint32_t last_boot_time;  // 最后启动时间
    bool update_flag;         // 升级标志
    bool rollback_flag;       // 回滚标志
    uint8_t reserved[16];     // 保留字段
} boot_info_t;
```

---

## 5. 实现方案

### 5.1 技术选型

| 组件     | 选择           | 理由               |
| -------- | -------------- | ------------------ |
| 网络协议 | HTTP/HTTPS     | 标准化，易于实现   |
| 加密算法 | AES-256        | 安全性高，硬件支持 |
| 签名算法 | RSA-2048       | 成熟可靠           |
| 校验算法 | CRC32 + SHA256 | 平衡性能和安全性   |
| 存储方案 | 双分区 A/B     | 安全升级，支持回滚 |
| 压缩算法 | LZSS           | 适合嵌入式系统     |

### 5.2 关键算法

#### 5.2.1 固件下载算法

```c
int ota_download_firmware(const ota_info_t* info) {
    uint32_t downloaded = 0;
    uint32_t total_size = info->size;
    uint8_t buffer[OTA_CHUNK_SIZE];

    // 检查续传点
    downloaded = ota_get_download_progress();

    while (downloaded < total_size) {
        uint32_t chunk_size = MIN(OTA_CHUNK_SIZE, total_size - downloaded);

        // 设置HTTP Range头进行断点续传
        int ret = http_download_range(info->url, downloaded,
                                    downloaded + chunk_size - 1,
                                    buffer, chunk_size);
        if (ret < 0) {
            if (ret == HTTP_ERROR_TIMEOUT) {
                // 超时重试
                continue;
            } else {
                return OTA_ERROR_DOWNLOAD;
            }
        }

        // 写入Flash
        ret = ota_write_firmware_chunk(downloaded, buffer, chunk_size);
        if (ret < 0) {
            return OTA_ERROR_INSTALL;
        }

        downloaded += chunk_size;

        // 更新进度
        ota_update_progress(downloaded, total_size);

        // 检查是否被取消
        if (ota_is_cancelled()) {
            return OTA_ERROR_CANCELLED;
        }

        // 喂狗
        watchdog_feed();
    }

    return OTA_SUCCESS;
}
```

#### 5.2.2 固件验证算法

```c
int ota_verify_firmware(const ota_info_t* info) {
    uint32_t calculated_crc = 0;
    uint8_t calculated_hash[32];
    uint8_t buffer[1024];

    // 1. CRC32校验
    uint32_t addr = APP2_START;
    uint32_t remaining = info->size;

    while (remaining > 0) {
        uint32_t read_size = MIN(sizeof(buffer), remaining);
        flash_read(addr, buffer, read_size);

        calculated_crc = crc32_update(calculated_crc, buffer, read_size);
        addr += read_size;
        remaining -= read_size;
    }

    // 比较CRC32
    uint32_t expected_crc = strtoul(info->checksum, NULL, 16);
    if (calculated_crc != expected_crc) {
        return OTA_ERROR_VERIFY;
    }

    // 2. SHA256校验（如果提供）
    if (strlen(info->signature) > 0) {
        sha256_context_t sha_ctx;
        sha256_init(&sha_ctx);

        addr = APP2_START;
        remaining = info->size;

        while (remaining > 0) {
            uint32_t read_size = MIN(sizeof(buffer), remaining);
            flash_read(addr, buffer, read_size);

            sha256_update(&sha_ctx, buffer, read_size);
            addr += read_size;
            remaining -= read_size;
        }

        sha256_final(&sha_ctx, calculated_hash);

        // 验证数字签名
        if (rsa_verify_signature(calculated_hash, sizeof(calculated_hash),
                                info->signature, strlen(info->signature)) != 0) {
            return OTA_ERROR_VERIFY;
        }
    }

    return OTA_SUCCESS;
}
```

#### 5.2.3 启动分区切换

```c
int ota_switch_boot_partition(void) {
    boot_info_t boot_info;

    // 读取当前启动信息
    if (flash_read(OTA_INFO_START, &boot_info, sizeof(boot_info)) != 0) {
        return OTA_ERROR_INSTALL;
    }

    // 验证魔数
    if (boot_info.magic != BOOT_INFO_MAGIC) {
        // 初始化启动信息
        memset(&boot_info, 0, sizeof(boot_info));
        boot_info.magic = BOOT_INFO_MAGIC;
        boot_info.active_slot = 0; // 默认APP1
    }

    // 切换到备用分区
    boot_info.active_slot = (boot_info.active_slot == 0) ? 1 : 0;
    boot_info.update_flag = true;
    boot_info.boot_count = 0;
    boot_info.last_boot_time = get_timestamp();

    // 写入新的启动信息
    if (flash_erase(OTA_INFO_START, OTA_INFO_SIZE) != 0) {
        return OTA_ERROR_INSTALL;
    }

    if (flash_write(OTA_INFO_START, &boot_info, sizeof(boot_info)) != 0) {
        return OTA_ERROR_INSTALL;
    }

    return OTA_SUCCESS;
}
```

### 5.3 错误处理和恢复

```c
// 升级失败处理
void ota_handle_upgrade_failure(ota_error_t error) {
    switch (error) {
        case OTA_ERROR_DOWNLOAD:
            // 下载失败，重试或切换服务器
            if (ota_retry_count < MAX_RETRY_COUNT) {
                ota_retry_download();
            } else {
                ota_abort_upgrade();
            }
            break;

        case OTA_ERROR_VERIFY:
            // 校验失败，重新下载
            ota_clear_download_cache();
            ota_restart_download();
            break;

        case OTA_ERROR_INSTALL:
            // 安装失败，回滚
            ota_rollback();
            break;

        default:
            ota_abort_upgrade();
            break;
    }
}

// 自动回滚机制
void ota_check_boot_status(void) {
    boot_info_t boot_info;
    flash_read(OTA_INFO_START, &boot_info, sizeof(boot_info));

    if (boot_info.update_flag) {
        boot_info.boot_count++;

        if (boot_info.boot_count >= MAX_BOOT_ATTEMPTS) {
            // 启动失败次数过多，回滚
            boot_info.active_slot = (boot_info.active_slot == 0) ? 1 : 0;
            boot_info.rollback_flag = true;
            boot_info.update_flag = false;
            boot_info.boot_count = 0;

            flash_erase(OTA_INFO_START, OTA_INFO_SIZE);
            flash_write(OTA_INFO_START, &boot_info, sizeof(boot_info));

            // 重启
            system_reboot();
        } else {
            // 更新启动计数
            flash_erase(OTA_INFO_START, OTA_INFO_SIZE);
            flash_write(OTA_INFO_START, &boot_info, sizeof(boot_info));
        }
    }
}
```

---

## 6. 开发任务分解

### 6.1 阶段一：基础框架 (P1.1)

| 任务 | 描述                       | 估时 | 状态      | 依赖       |
| ---- | -------------------------- | ---- | --------- | ---------- |
| T1.1 | ⚫ 创建 OTA 模块基础框架   | 1d   | ⚫ 未开始 | 无         |
| T1.2 | ⚫ 设计 API 接口和数据结构 | 1.5d | ⚫ 未开始 | T1.1       |
| T1.3 | ⚫ 实现状态机管理          | 2d   | ⚫ 未开始 | T1.2       |
| T1.4 | ⚫ 实现 Flash 分区管理     | 2d   | ⚫ 未开始 | T1.2       |
| T1.5 | ⚫ 基础框架测试            | 1d   | ⚫ 未开始 | T1.3, T1.4 |

**里程碑**: OTA 基础框架可用

### 6.2 阶段二：网络下载 (P1.2)

| 任务 | 描述                    | 估时 | 状态      | 依赖       |
| ---- | ----------------------- | ---- | --------- | ---------- |
| T2.1 | ⚫ 实现 HTTP 下载客户端 | 3d   | ⚫ 未开始 | T1.5       |
| T2.2 | ⚫ 实现 MQTT 下载协议   | 2.5d | ⚫ 未开始 | T1.5       |
| T2.3 | ⚫ 实现断点续传功能     | 2d   | ⚫ 未开始 | T2.1       |
| T2.4 | ⚫ 实现重试和超时机制   | 1.5d | ⚫ 未开始 | T2.1, T2.2 |
| T2.5 | ⚫ 网络下载集成测试     | 1d   | ⚫ 未开始 | T2.1-T2.4  |

**里程碑**: 网络下载功能可用

### 6.3 阶段三：安全验证 (P1.3)

| 任务 | 描述                     | 估时 | 状态      | 依赖      |
| ---- | ------------------------ | ---- | --------- | --------- |
| T3.1 | ⚫ 实现 CRC32 校验算法   | 1.5d | ⚫ 未开始 | T2.5      |
| T3.2 | ⚫ 实现 SHA256 校验算法  | 2d   | ⚫ 未开始 | T2.5      |
| T3.3 | ⚫ 实现 RSA 数字签名验证 | 2.5d | ⚫ 未开始 | T3.2      |
| T3.4 | ⚫ 实现固件加密解密      | 2d   | ⚫ 未开始 | T3.3      |
| T3.5 | ⚫ 安全验证集成测试      | 1d   | ⚫ 未开始 | T3.1-T3.4 |

**里程碑**: 安全验证功能可用

### 6.4 阶段四：升级安装 (P1.4)

| 任务 | 描述                    | 估时 | 状态      | 依赖      |
| ---- | ----------------------- | ---- | --------- | --------- |
| T4.1 | ⚫ 实现 Bootloader 设计 | 3d   | ⚫ 未开始 | T3.5      |
| T4.2 | ⚫ 实现固件安装机制     | 2.5d | ⚫ 未开始 | T4.1      |
| T4.3 | ⚫ 实现回滚机制         | 2d   | ⚫ 未开始 | T4.2      |
| T4.4 | ⚫ 实现升级状态监控     | 1.5d | ⚫ 未开始 | T4.2      |
| T4.5 | ⚫ 完整升级流程测试     | 2d   | ⚫ 未开始 | T4.1-T4.4 |

**里程碑**: OTA 模块完整功能

### 6.5 总体进度跟踪

| 阶段          | 总任务数 | 已完成 | 进行中 | 未开始 | 完成率 |
| ------------- | -------- | ------ | ------ | ------ | ------ |
| P1.1 基础框架 | 5        | 0      | 0      | 5      | 0%     |
| P1.2 网络下载 | 5        | 0      | 0      | 5      | 0%     |
| P1.3 安全验证 | 5        | 0      | 0      | 5      | 0%     |
| P1.4 升级安装 | 5        | 0      | 0      | 5      | 0%     |
| **总计**      | **20**   | **0**  | **0**  | **20** | **0%** |

---

## 7. 测试计划

### 7.1 单元测试

| 测试项 | 描述                | 覆盖率要求 | 状态      |
| ------ | ------------------- | ---------- | --------- |
| UT1.1  | ⚫ 固件下载功能测试 | >90%       | ⚫ 未开始 |
| UT1.2  | ⚫ 校验算法测试     | >95%       | ⚫ 未开始 |
| UT1.3  | ⚫ 分区管理测试     | >90%       | ⚫ 未开始 |
| UT1.4  | ⚫ 状态机测试       | >85%       | ⚫ 未开始 |
| UT1.5  | ⚫ 错误处理测试     | >85%       | ⚫ 未开始 |

### 7.2 集成测试

| 测试项 | 描述                | 测试条件        | 状态      |
| ------ | ------------------- | --------------- | --------- |
| IT1.1  | ⚫ 完整升级流程测试 | 正常网络环境    | ⚫ 未开始 |
| IT1.2  | ⚫ 网络中断恢复测试 | 模拟网络中断    | ⚫ 未开始 |
| IT1.3  | ⚫ 掉电重启测试     | 升级过程中掉电  | ⚫ 未开始 |
| IT1.4  | ⚫ 固件损坏回滚测试 | 损坏固件安装    | ⚫ 未开始 |
| IT1.5  | ⚫ 大文件升级测试   | 接近 Flash 限制 | ⚫ 未开始 |

### 7.3 压力测试

| 测试项 | 指标              | 目标值   | 状态      |
| ------ | ----------------- | -------- | --------- |
| ST1.1  | ⚫ 升级成功率     | >95%     | ⚫ 未开始 |
| ST1.2  | ⚫ 下载速度       | 1-10KB/s | ⚫ 未开始 |
| ST1.3  | ⚫ 升级时间       | <10 分钟 | ⚫ 未开始 |
| ST1.4  | ⚫ 回滚时间       | <30 秒   | ⚫ 未开始 |
| ST1.5  | ⚫ Flash 写入次数 | >1000 次 | ⚫ 未开始 |

---

## 8. 风险控制

### 8.1 技术风险

| 风险           | 概率 | 影响 | 应对措施            | 状态      |
| -------------- | ---- | ---- | ------------------- | --------- |
| 升级过程中断电 | 中   | 高   | 实现原子性升级+回滚 | ⚫ 监控中 |
| 网络传输不稳定 | 高   | 中   | 断点续传+重试机制   | ⚫ 监控中 |
| 固件验证失败   | 低   | 高   | 多重校验+数字签名   | ⚫ 监控中 |
| Flash 写入失败 | 低   | 高   | 坏块管理+备份分区   | ⚫ 监控中 |

### 8.2 安全风险

| 风险         | 概率 | 影响 | 应对措施       | 状态      |
| ------------ | ---- | ---- | -------------- | --------- |
| 恶意固件注入 | 低   | 高   | RSA 签名验证   | ⚫ 监控中 |
| 中间人攻击   | 低   | 中   | HTTPS 加密传输 | ⚫ 监控中 |

---

## 9. 交付标准

### 9.1 功能验收标准

- ✅ 支持远程固件版本检查
- ✅ 支持多协议固件下载
- ✅ 支持断点续传功能
- ✅ 支持固件完整性验证
- ✅ 支持安全升级和回滚

### 9.2 性能验收标准

- ✅ 升级成功率>95%
- ✅ 下载速度 1-10KB/s
- ✅ 升级时间<10 分钟
- ✅ 回滚时间<30 秒

### 9.3 安全验收标准

- ✅ 通过校验和验证测试
- ✅ 通过数字签名验证测试
- ✅ 通过回滚机制测试
- ✅ 通过掉电恢复测试

---

**模块负责人**: [待分配]  
**预计开发时间**: 22 个工作日  
**创建时间**: 2024 年 12 月  
**最后更新**: 2024 年 12 月

## 10. 服务器端开发要求

### 10.1 系统架构概述

OTA 服务器端采用微服务架构，为设备提供完整的远程升级管理能力：

```
┌─────────────────────────────────────────────────────────┐
│                  OTA 管理平台架构                        │
├─────────────────────────────────────────────────────────┤
│  Web 管理界面 (React + TypeScript)                      │
├─────────────────────────────────────────────────────────┤
│  API 网关层 (Nginx + Kong)                             │
├─────────────────────────────────────────────────────────┤
│  业务服务层                                             │
│  ├─ 版本管理服务 ├─ 升级计划服务 ├─ 设备管理服务       │
│  ├─ 文件管理服务 ├─ 通知服务     ├─ 统计分析服务       │
│  └─ 权限管理服务 └─ 日志服务     └─ 监控告警服务       │
├─────────────────────────────────────────────────────────┤
│  数据持久层                                             │
│  ├─ MySQL 数据库 ├─ Redis 缓存   ├─ MinIO 文件存储     │
│  └─ InfluxDB     └─ Elasticsearch └─ MongoDB            │
└─────────────────────────────────────────────────────────┘
```

### 10.2 核心功能模块

#### 10.2.1 固件版本管理系统

**功能需求**:

- 固件版本生命周期管理（开发 → 测试 → 预发布 → 正式发布 → 废弃）
- 多产品线、多硬件版本兼容性管理
- 固件包上传、存储、分发管理
- 版本依赖关系和升级路径规划
- 增量更新包生成和管理

**数据库设计**:

```sql
-- 固件版本表
CREATE TABLE firmware_versions (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    version_name VARCHAR(64) NOT NULL,
    version_code INT NOT NULL,
    product_line VARCHAR(32) NOT NULL,
    hardware_version VARCHAR(32) NOT NULL,
    file_size BIGINT NOT NULL,
    file_path VARCHAR(512) NOT NULL,
    file_hash VARCHAR(128) NOT NULL,
    signature TEXT,
    status ENUM('DRAFT', 'TESTING', 'PRE_RELEASE', 'RELEASED', 'DEPRECATED'),
    compatibility_info JSON,
    release_notes TEXT,
    created_by BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    released_at TIMESTAMP NULL,
    INDEX idx_version_product(product_line, version_code),
    INDEX idx_status(status),
    UNIQUE KEY uk_version(product_line, hardware_version, version_code)
);

-- 版本依赖关系表
CREATE TABLE version_dependencies (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    from_version_id BIGINT NOT NULL,
    to_version_id BIGINT NOT NULL,
    dependency_type ENUM('UPGRADE', 'DOWNGRADE', 'CROSS_BRANCH'),
    min_required_version VARCHAR(64),
    is_direct_upgrade BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (from_version_id) REFERENCES firmware_versions(id),
    FOREIGN KEY (to_version_id) REFERENCES firmware_versions(id)
);
```

**API 接口设计**:

```javascript
// 固件版本管理 API
POST / api / v1 / firmware / upload; // 上传固件包
GET / api / v1 / firmware / versions; // 获取版本列表
GET / api / v1 / firmware / { id }; // 获取版本详情
PUT / api / v1 / firmware / { id } / status; // 更新版本状态
DELETE / api / v1 / firmware / { id }; // 删除版本
POST / api / v1 / firmware / { id } / sign; // 对固件进行数字签名
GET / api / v1 / firmware / compatibility; // 获取兼容性矩阵
```

#### 10.2.2 OTA 升级计划管理

**功能需求**:

- 升级计划创建、编辑、执行、暂停、取消
- 分批次升级策略（灰度发布、蓝绿部署）
- 升级时间窗口和调度策略
- 升级前置条件检查和设备筛选
- 升级过程监控和异常处理

**核心数据结构**:

```sql
-- 升级计划表
CREATE TABLE ota_plans (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    plan_name VARCHAR(128) NOT NULL,
    description TEXT,
    firmware_version_id BIGINT NOT NULL,
    target_devices JSON,           -- 目标设备筛选条件
    upgrade_strategy ENUM('IMMEDIATE', 'SCHEDULED', 'GRADUAL', 'CANARY'),
    schedule_config JSON,          -- 调度配置
    batch_config JSON,             -- 分批配置
    pre_conditions JSON,           -- 前置条件
    status ENUM('DRAFT', 'READY', 'RUNNING', 'PAUSED', 'COMPLETED', 'FAILED', 'CANCELLED'),
    created_by BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    started_at TIMESTAMP NULL,
    completed_at TIMESTAMP NULL,
    FOREIGN KEY (firmware_version_id) REFERENCES firmware_versions(id)
);

-- 升级批次表
CREATE TABLE ota_batches (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    plan_id BIGINT NOT NULL,
    batch_number INT NOT NULL,
    device_count INT NOT NULL,
    started_at TIMESTAMP NULL,
    completed_at TIMESTAMP NULL,
    success_count INT DEFAULT 0,
    failed_count INT DEFAULT 0,
    status ENUM('PENDING', 'RUNNING', 'COMPLETED', 'FAILED', 'CANCELLED'),
    FOREIGN KEY (plan_id) REFERENCES ota_plans(id)
);
```

**升级策略配置示例**:

```json
{
  "strategy": "GRADUAL",
  "batches": [
    {
      "percentage": 5,
      "wait_hours": 24,
      "success_threshold": 95
    },
    {
      "percentage": 20,
      "wait_hours": 12,
      "success_threshold": 95
    },
    {
      "percentage": 100,
      "wait_hours": 0,
      "success_threshold": 90
    }
  ],
  "auto_rollback": {
    "enabled": true,
    "failure_threshold": 10,
    "timeout_minutes": 30
  }
}
```

#### 10.2.3 设备版本管理系统

**功能需求**:

- 设备固件版本状态实时跟踪
- 设备升级历史记录和审计
- 设备健康状态监控
- 设备分组和标签管理
- 批量设备操作和状态查询

**数据库设计**:

```sql
-- 设备信息表
CREATE TABLE devices (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id VARCHAR(64) UNIQUE NOT NULL,
    device_name VARCHAR(128),
    product_line VARCHAR(32) NOT NULL,
    hardware_version VARCHAR(32) NOT NULL,
    current_firmware_version VARCHAR(64),
    target_firmware_version VARCHAR(64),
    last_seen_at TIMESTAMP,
    status ENUM('ONLINE', 'OFFLINE', 'UPGRADING', 'ERROR'),
    location JSON,
    tags JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_device_id(device_id),
    INDEX idx_product_line(product_line),
    INDEX idx_status(status)
);

-- 设备升级记录表
CREATE TABLE device_upgrade_records (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id VARCHAR(64) NOT NULL,
    plan_id BIGINT,
    batch_id BIGINT,
    from_version VARCHAR(64),
    to_version VARCHAR(64),
    status ENUM('PENDING', 'DOWNLOADING', 'INSTALLING', 'SUCCESS', 'FAILED', 'ROLLBACK'),
    progress INT DEFAULT 0,
    error_code VARCHAR(32),
    error_message TEXT,
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    duration_seconds INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_id(device_id),
    INDEX idx_plan_id(plan_id),
    INDEX idx_status(status),
    FOREIGN KEY (plan_id) REFERENCES ota_plans(id)
);
```

#### 10.2.4 升级失败管理系统

**功能需求**:

- 升级失败自动检测和分类
- 失败原因分析和统计
- 自动重试和回滚机制
- 失败设备批量恢复
- 故障预警和通知系统

**失败处理策略**:

```javascript
// 失败处理配置
const failureHandlingConfig = {
  retryPolicy: {
    maxRetries: 3,
    retryDelayMinutes: [5, 15, 60],
    retryConditions: ["NETWORK_ERROR", "TIMEOUT", "DOWNLOAD_FAILED"],
  },
  rollbackPolicy: {
    autoRollback: true,
    rollbackConditions: [
      "INSTALL_FAILED",
      "BOOT_FAILED",
      "VERIFICATION_FAILED",
    ],
    rollbackTimeoutMinutes: 30,
  },
  alertPolicy: {
    failureThreshold: 5, // 连续失败5次触发告警
    alertChannels: ["EMAIL", "SMS", "WEBHOOK"],
    escalationLevels: [
      { threshold: 5, severity: "WARNING" },
      { threshold: 10, severity: "CRITICAL" },
    ],
  },
};
```

### 10.3 系统监控和统计

#### 10.3.1 实时监控指标

```javascript
// 关键监控指标
const monitoringMetrics = {
  deviceMetrics: {
    totalDevices: "设备总数",
    onlineDevices: "在线设备数",
    upgradingDevices: "正在升级设备数",
    errorDevices: "异常设备数",
  },
  upgradeMetrics: {
    activeUpgradePlans: "活跃升级计划",
    todayUpgradeCount: "今日升级次数",
    upgradeSuccessRate: "升级成功率",
    averageUpgradeTime: "平均升级时长",
  },
  systemMetrics: {
    apiResponseTime: "API响应时间",
    downloadBandwidth: "下载带宽使用",
    storageUsage: "存储空间使用",
    systemUptime: "系统可用时间",
  },
};
```

#### 10.3.2 数据分析和报表

**统计维度**:

- 按时间维度：小时、日、周、月统计
- 按设备维度：产品线、硬件版本、地理位置
- 按版本维度：版本采用率、升级成功率
- 按故障维度：失败原因分布、影响范围

**报表 API**:

```javascript
GET / api / v1 / analytics / upgrade - statistics; // 升级统计数据
GET / api / v1 / analytics / device - distribution; // 设备分布统计
GET / api / v1 / analytics / failure - analysis; // 失败分析报告
GET / api / v1 / analytics / version - adoption; // 版本采用率统计
```

### 10.4 安全和权限管理

#### 10.4.1 访问控制设计

```sql
-- 用户权限表
CREATE TABLE user_permissions (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    user_id BIGINT NOT NULL,
    resource_type ENUM('FIRMWARE', 'DEVICE', 'PLAN', 'SYSTEM'),
    resource_id VARCHAR(64),
    permission ENUM('READ', 'WRITE', 'DELETE', 'EXECUTE'),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_user_resource(user_id, resource_type)
);
```

#### 10.4.2 安全策略

- **身份认证**: JWT + OAuth2.0
- **权限控制**: RBAC 基于角色的访问控制
- **数据加密**: 固件包 AES-256 加密存储
- **传输安全**: HTTPS + 证书校验
- **审计日志**: 所有操作完整审计追踪

### 10.5 部署和运维

#### 10.5.1 系统部署方案

**容器化部署**:

```yaml
# docker-compose.yml
version: "3.8"
services:
  ota-api:
    image: ota-server:latest
    ports:
      - "8080:8080"
    environment:
      - MYSQL_HOST=mysql
      - REDIS_HOST=redis
      - MINIO_HOST=minio

  mysql:
    image: mysql:8.0
    environment:
      MYSQL_ROOT_PASSWORD: ${MYSQL_PASSWORD}
      MYSQL_DATABASE: ota_system

  redis:
    image: redis:7-alpine

  minio:
    image: minio/minio:latest
    command: server /data --console-address ":9001"
    ports:
      - "9000:9000"
      - "9001:9001"
```

#### 10.5.2 运维监控

**监控方案**:

- **应用监控**: Prometheus + Grafana
- **日志管理**: ELK Stack (Elasticsearch + Logstash + Kibana)
- **链路追踪**: Jaeger 分布式追踪
- **告警通知**: AlertManager + 企业微信/钉钉

### 10.6 开发规范和标准

#### 10.6.1 API 设计规范

```javascript
// RESTful API 设计标准
const apiStandards = {
  baseUrl: "https://api.ota-system.com/v1",
  authentication: "Bearer Token",
  requestFormat: "JSON",
  responseFormat: {
    success: {
      code: 200,
      message: "操作成功",
      data: {},
      timestamp: "2024-12-XX 10:00:00",
    },
    error: {
      code: 4001,
      message: "参数错误",
      error: "详细错误信息",
      timestamp: "2024-12-XX 10:00:00",
    },
  },
  pagination: {
    page: 1,
    pageSize: 20,
    total: 100,
    totalPages: 5,
  },
};
```

#### 10.6.2 开发测试要求

**测试覆盖率要求**:

- 单元测试覆盖率 ≥ 80%
- 集成测试覆盖核心业务流程
- 端到端测试覆盖关键用户场景
- 性能测试支持 1000+ 并发设备

**代码质量要求**:

- SonarQube 代码质量检查通过
- 安全漏洞扫描无高危问题
- 代码评审通过率 100%
- 文档覆盖率 ≥ 90%

---

**服务器端开发负责人**: [待分配]  
**预计开发时间**: 45 个工作日  
**技术栈**: Spring Boot + MySQL + Redis + React  
**部署方式**: Docker + Kubernetes
