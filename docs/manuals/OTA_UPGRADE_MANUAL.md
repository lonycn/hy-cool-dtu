# 冷库 DTU 远程升级模块开发手册

## 1. 概述

### 1.1 模块目标

为冷库 DTU 系统设计一个独立的 OTA（Over-The-Air）远程升级模块，支持通过 4G/LoRa/WiFi 等多种通信方式进行固件远程更新，确保系统的可维护性和可扩展性。

### 1.2 设计原则

- **解耦设计**: 升级模块与主业务逻辑完全分离
- **安全可靠**: 支持固件校验、回滚机制
- **多通道支持**: 兼容现有的通信接口
- **断点续传**: 支持网络中断后的续传功能
- **状态监控**: 提供详细的升级状态反馈

### 1.3 参考开源项目

- **ESP32 OTA**: 成熟的 OTA 实现方案
- **MCUboot**: ARM 官方推荐的安全启动和升级方案
- **TinyOTA**: 轻量级嵌入式 OTA 解决方案
- **AWS IoT OTA**: 云端 OTA 管理方案

## 2. 系统架构设计

### 2.1 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                    云端管理平台                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │
│  │  固件管理   │  │  设备管理   │  │  升级监控   │     │
│  └─────────────┘  └─────────────┘  └─────────────┘     │
└─────────────────────────────────────────────────────────┘
                            │
                    ┌───────┴───────┐
                    │   通信协议     │
                    │ HTTP/MQTT/CoAP │
                    └───────┬───────┘
                            │
┌─────────────────────────────────────────────────────────┐
│                    DTU 设备端                           │
│  ┌─────────────────────────────────────────────────────┐│
│  │              OTA 升级模块                           ││
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ ││
│  │  │  通信接口   │  │  升级管理   │  │  存储管理   │ ││
│  │  └─────────────┘  └─────────────┘  └─────────────┘ ││
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ ││
│  │  │  安全校验   │  │  状态管理   │  │  错误处理   │ ││
│  │  └─────────────┘  └─────────────┘  └─────────────┘ ││
│  └─────────────────────────────────────────────────────┘│
│  ┌─────────────────────────────────────────────────────┐│
│  │              主业务系统                             ││
│  │     (温湿度监控、设备控制、数据采集等)               ││
│  └─────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────┘
```

### 2.2 模块分层设计

```
┌─────────────────────────────────────────┐
│            应用接口层                    │  ← API 接口
├─────────────────────────────────────────┤
│            升级管理层                    │  ← 升级逻辑
├─────────────────────────────────────────┤
│            协议适配层                    │  ← 通信协议
├─────────────────────────────────────────┤
│            硬件抽象层                    │  ← Flash/通信
└─────────────────────────────────────────┘
```

## 3. 功能模块设计

### 3.1 核心功能模块

#### 3.1.1 升级管理模块 (ota_manager.c)

```c
// 升级状态枚举
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_INSTALLING,
    OTA_STATE_COMPLETED,
    OTA_STATE_FAILED,
    OTA_STATE_ROLLBACK
} ota_state_t;

// 升级信息结构
typedef struct {
    char version[16];
    uint32_t size;
    char url[256];
    char md5[33];
    uint32_t timestamp;
} ota_info_t;

// 升级进度回调
typedef void (*ota_progress_cb_t)(uint32_t downloaded, uint32_t total);

// 升级状态回调
typedef void (*ota_state_cb_t)(ota_state_t state, int error_code);
```

#### 3.1.2 通信接口模块 (ota_transport.c)

```c
// 传输接口结构
typedef struct {
    int (*init)(void);
    int (*connect)(const char* url);
    int (*read)(uint8_t* buffer, uint32_t size);
    int (*write)(const uint8_t* buffer, uint32_t size);
    int (*disconnect)(void);
    int (*deinit)(void);
} ota_transport_t;

// 支持的传输方式
extern ota_transport_t ota_transport_http;
extern ota_transport_t ota_transport_mqtt;
extern ota_transport_t ota_transport_lora;
```

#### 3.1.3 存储管理模块 (ota_storage.c)

```c
// Flash 分区定义
#define OTA_PARTITION_APP1      0x08008000  // 主应用分区
#define OTA_PARTITION_APP2      0x08040000  // 备份应用分区
#define OTA_PARTITION_BOOTLOADER 0x08000000  // Bootloader 分区
#define OTA_PARTITION_CONFIG    0x08078000  // 配置分区

// 存储接口
typedef struct {
    int (*init)(void);
    int (*erase)(uint32_t addr, uint32_t size);
    int (*write)(uint32_t addr, const uint8_t* data, uint32_t size);
    int (*read)(uint32_t addr, uint8_t* data, uint32_t size);
    int (*verify)(uint32_t addr, uint32_t size, const char* checksum);
} ota_storage_t;
```

#### 3.1.4 安全校验模块 (ota_security.c)

```c
// 校验算法
typedef enum {
    OTA_CHECKSUM_MD5 = 0,
    OTA_CHECKSUM_SHA256,
    OTA_CHECKSUM_CRC32
} ota_checksum_type_t;

// 数字签名验证
typedef struct {
    uint8_t signature[256];
    uint8_t public_key[256];
    uint32_t signature_len;
    uint32_t key_len;
} ota_signature_t;

// 安全接口
int ota_verify_checksum(const uint8_t* data, uint32_t size,
                       const char* expected, ota_checksum_type_t type);
int ota_verify_signature(const uint8_t* data, uint32_t size,
                        const ota_signature_t* sig);
```

### 3.2 文件结构

```
app/
├── ota/                          # OTA 升级模块
│   ├── include/                  # 头文件
│   │   ├── ota_manager.h
│   │   ├── ota_transport.h
│   │   ├── ota_storage.h
│   │   ├── ota_security.h
│   │   ├── ota_config.h
│   │   └── ota_common.h
│   ├── src/                      # 源文件
│   │   ├── ota_manager.c         # 升级管理
│   │   ├── ota_transport_http.c  # HTTP 传输
│   │   ├── ota_transport_mqtt.c  # MQTT 传输
│   │   ├── ota_transport_lora.c  # LoRa 传输
│   │   ├── ota_storage_flash.c   # Flash 存储
│   │   ├── ota_security.c        # 安全校验
│   │   └── ota_bootloader.c      # Bootloader 接口
│   ├── config/                   # 配置文件
│   │   └── ota_config.c
│   └── test/                     # 测试文件
│       ├── ota_test.c
│       └── ota_mock.c
```

## 4. 接口设计

### 4.1 主要 API 接口

```c
// ota_manager.h

/**
 * @brief 初始化 OTA 模块
 * @param config 配置参数
 * @return 0: 成功, <0: 失败
 */
int ota_init(const ota_config_t* config);

/**
 * @brief 检查固件更新
 * @param info 返回的更新信息
 * @return 0: 有更新, 1: 无更新, <0: 错误
 */
int ota_check_update(ota_info_t* info);

/**
 * @brief 开始固件下载
 * @param info 固件信息
 * @param progress_cb 进度回调
 * @param state_cb 状态回调
 * @return 0: 成功, <0: 失败
 */
int ota_start_download(const ota_info_t* info,
                      ota_progress_cb_t progress_cb,
                      ota_state_cb_t state_cb);

/**
 * @brief 暂停下载
 * @return 0: 成功, <0: 失败
 */
int ota_pause_download(void);

/**
 * @brief 恢复下载
 * @return 0: 成功, <0: 失败
 */
int ota_resume_download(void);

/**
 * @brief 取消升级
 * @return 0: 成功, <0: 失败
 */
int ota_cancel_update(void);

/**
 * @brief 安装固件
 * @return 0: 成功, <0: 失败
 */
int ota_install_firmware(void);

/**
 * @brief 回滚到上一版本
 * @return 0: 成功, <0: 失败
 */
int ota_rollback(void);

/**
 * @brief 获取当前状态
 * @return 当前 OTA 状态
 */
ota_state_t ota_get_state(void);

/**
 * @brief 获取升级进度
 * @param downloaded 已下载字节数
 * @param total 总字节数
 * @return 0: 成功, <0: 失败
 */
int ota_get_progress(uint32_t* downloaded, uint32_t* total);

/**
 * @brief 反初始化 OTA 模块
 */
void ota_deinit(void);
```

### 4.2 配置接口

```c
// ota_config.h

typedef struct {
    // 服务器配置
    char server_url[256];
    uint16_t server_port;
    char device_id[64];
    char api_key[128];

    // 传输配置
    ota_transport_type_t transport_type;
    uint32_t timeout_ms;
    uint32_t retry_count;
    uint32_t chunk_size;

    // 安全配置
    bool enable_signature_verify;
    bool enable_encryption;
    ota_checksum_type_t checksum_type;

    // 存储配置
    uint32_t app_partition_size;
    uint32_t backup_partition_addr;

    // 回调配置
    ota_progress_cb_t progress_callback;
    ota_state_cb_t state_callback;
} ota_config_t;
```

## 5. 实现方案

### 5.1 Bootloader 设计

#### 5.1.1 双分区方案

```c
// bootloader.c

typedef struct {
    uint32_t magic;           // 魔数标识
    uint32_t version;         // 固件版本
    uint32_t size;            // 固件大小
    uint32_t checksum;        // 校验和
    uint32_t flags;           // 标志位
    uint8_t  reserved[12];    // 保留字段
} firmware_header_t;

// 分区状态
typedef enum {
    PARTITION_INVALID = 0,
    PARTITION_VALID,
    PARTITION_PENDING,
    PARTITION_CORRUPTED
} partition_state_t;

// 启动逻辑
void bootloader_main(void) {
    partition_state_t app1_state = check_partition_state(OTA_PARTITION_APP1);
    partition_state_t app2_state = check_partition_state(OTA_PARTITION_APP2);

    // 选择启动分区
    uint32_t boot_addr = select_boot_partition(app1_state, app2_state);

    // 跳转到应用程序
    jump_to_application(boot_addr);
}
```

#### 5.1.2 升级流程

```c
// 升级状态机
typedef enum {
    UPGRADE_IDLE = 0,
    UPGRADE_DOWNLOAD,
    UPGRADE_VERIFY,
    UPGRADE_INSTALL,
    UPGRADE_REBOOT,
    UPGRADE_COMPLETE
} upgrade_state_t;

// 升级流程控制
int ota_upgrade_process(void) {
    static upgrade_state_t state = UPGRADE_IDLE;

    switch (state) {
        case UPGRADE_IDLE:
            if (ota_check_update(&update_info) == 0) {
                state = UPGRADE_DOWNLOAD;
            }
            break;

        case UPGRADE_DOWNLOAD:
            if (ota_download_firmware(&update_info) == 0) {
                state = UPGRADE_VERIFY;
            }
            break;

        case UPGRADE_VERIFY:
            if (ota_verify_firmware() == 0) {
                state = UPGRADE_INSTALL;
            }
            break;

        case UPGRADE_INSTALL:
            if (ota_install_firmware() == 0) {
                state = UPGRADE_REBOOT;
            }
            break;

        case UPGRADE_REBOOT:
            ota_reboot_system();
            break;

        default:
            break;
    }

    return 0;
}
```

### 5.2 通信协议实现

#### 5.2.1 HTTP 传输

```c
// ota_transport_http.c

typedef struct {
    char host[128];
    uint16_t port;
    char path[256];
    uint32_t content_length;
    uint32_t downloaded;
} http_context_t;

int http_download_firmware(const char* url, ota_progress_cb_t progress_cb) {
    http_context_t ctx;
    uint8_t buffer[1024];
    int ret;

    // 解析 URL
    ret = http_parse_url(url, &ctx);
    if (ret < 0) return ret;

    // 建立连接
    ret = http_connect(ctx.host, ctx.port);
    if (ret < 0) return ret;

    // 发送 HTTP 请求
    ret = http_send_request(&ctx);
    if (ret < 0) return ret;

    // 接收数据
    while (ctx.downloaded < ctx.content_length) {
        int len = http_receive(buffer, sizeof(buffer));
        if (len > 0) {
            // 写入 Flash
            ota_storage_write(OTA_PARTITION_APP2 + ctx.downloaded,
                             buffer, len);
            ctx.downloaded += len;

            // 进度回调
            if (progress_cb) {
                progress_cb(ctx.downloaded, ctx.content_length);
            }
        }
    }

    http_disconnect();
    return 0;
}
```

#### 5.2.2 MQTT 传输

```c
// ota_transport_mqtt.c

#define MQTT_TOPIC_UPDATE_CHECK    "device/%s/ota/check"
#define MQTT_TOPIC_UPDATE_DOWNLOAD "device/%s/ota/download"
#define MQTT_TOPIC_UPDATE_STATUS   "device/%s/ota/status"

typedef struct {
    char client_id[64];
    char username[64];
    char password[64];
    bool connected;
} mqtt_context_t;

int mqtt_check_update(ota_info_t* info) {
    char topic[128];
    char payload[256];

    // 构造主题
    snprintf(topic, sizeof(topic), MQTT_TOPIC_UPDATE_CHECK, device_id);

    // 构造消息
    snprintf(payload, sizeof(payload),
             "{\"device_id\":\"%s\",\"current_version\":\"%s\"}",
             device_id, current_version);

    // 发布消息
    mqtt_publish(topic, payload, strlen(payload));

    // 等待响应
    return mqtt_wait_response(info, 5000);
}
```

### 5.3 安全机制实现

#### 5.3.1 数字签名验证

```c
// ota_security.c

#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"

int ota_verify_signature(const uint8_t* firmware, uint32_t size,
                        const ota_signature_t* sig) {
    mbedtls_rsa_context rsa;
    mbedtls_sha256_context sha256;
    uint8_t hash[32];
    int ret;

    // 计算固件哈希
    mbedtls_sha256_init(&sha256);
    mbedtls_sha256_starts(&sha256, 0);
    mbedtls_sha256_update(&sha256, firmware, size);
    mbedtls_sha256_finish(&sha256, hash);
    mbedtls_sha256_free(&sha256);

    // 初始化 RSA 上下文
    mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);

    // 导入公钥
    ret = mbedtls_rsa_import_raw(&rsa, NULL, 0, NULL, 0, NULL, 0,
                                NULL, 0, sig->public_key, sig->key_len);
    if (ret != 0) goto cleanup;

    // 验证签名
    ret = mbedtls_rsa_pkcs1_verify(&rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                  MBEDTLS_MD_SHA256, 32, hash, sig->signature);

cleanup:
    mbedtls_rsa_free(&rsa);
    return ret;
}
```

#### 5.3.2 固件加密

```c
// AES 加密/解密
int ota_decrypt_firmware(const uint8_t* encrypted, uint32_t size,
                        uint8_t* decrypted, const uint8_t* key) {
    mbedtls_aes_context aes;
    uint8_t iv[16] = {0}; // 初始化向量
    int ret;

    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_dec(&aes, key, 256);
    if (ret == 0) {
        ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, size,
                                   iv, encrypted, decrypted);
    }
    mbedtls_aes_free(&aes);

    return ret;
}
```

## 6. 集成指南

### 6.1 与主系统集成

#### 6.1.1 主循环集成

```c
// main_loop.c 中添加

#include "ota/include/ota_manager.h"

static ota_config_t ota_config = {
    .server_url = "https://ota.example.com",
    .server_port = 443,
    .device_id = "DTU_001",
    .transport_type = OTA_TRANSPORT_HTTP,
    .timeout_ms = 30000,
    .retry_count = 3,
    .chunk_size = 1024,
    .enable_signature_verify = true,
    .checksum_type = OTA_CHECKSUM_SHA256
};

void Main_loop_Initial(void) {
    // 原有初始化代码...

    // 初始化 OTA 模块
    if (ota_init(&ota_config) != 0) {
        printf("OTA init failed\n");
    }
}

void Main_loop_while(void) {
    // 原有主循环代码...

    // OTA 处理 (每 10 秒检查一次)
    static uint32_t ota_check_timer = 0;
    if (++ota_check_timer >= 10000) {
        ota_check_timer = 0;
        ota_process();
    }
}
```

#### 6.1.2 Modbus 接口集成

```c
// Modbus.c 中添加 OTA 相关寄存器

#define OTA_REG_STATE           200  // OTA 状态
#define OTA_REG_PROGRESS        201  // 升级进度
#define OTA_REG_CONTROL         202  // 控制命令
#define OTA_REG_VERSION_NEW     203  // 新版本号
#define OTA_REG_ERROR_CODE      204  // 错误代码

void DealWithSingleRegWrite_06(unsigned char *SendTempBuffer) {
    uint16_t addr = SendTempBuffer[2] * 256 + SendTempBuffer[3];
    uint16_t value = SendTempBuffer[4] * 256 + SendTempBuffer[5];

    switch (addr) {
        case OTA_REG_CONTROL:
            switch (value) {
                case 1: // 检查更新
                    ota_check_update_async();
                    break;
                case 2: // 开始升级
                    ota_start_upgrade_async();
                    break;
                case 3: // 取消升级
                    ota_cancel_update();
                    break;
                case 4: // 回滚
                    ota_rollback();
                    break;
            }
            break;

        // 其他寄存器处理...
    }
}

void DealWithMoreRegRead_03(unsigned char *SendTempBuffer) {
    uint16_t start_addr = SendTempBuffer[2] * 256 + SendTempBuffer[3];
    uint16_t reg_count = SendTempBuffer[4] * 256 + SendTempBuffer[5];

    for (int i = 0; i < reg_count; i++) {
        uint16_t addr = start_addr + i;
        uint16_t value = 0;

        switch (addr) {
            case OTA_REG_STATE:
                value = ota_get_state();
                break;
            case OTA_REG_PROGRESS:
                {
                    uint32_t downloaded, total;
                    ota_get_progress(&downloaded, &total);
                    value = total > 0 ? (downloaded * 100 / total) : 0;
                }
                break;
            // 其他寄存器读取...
        }

        // 填充响应数据
        SendTempBuffer[3 + i * 2] = value >> 8;
        SendTempBuffer[4 + i * 2] = value & 0xFF;
    }
}
```

### 6.2 配置文件集成

```c
// global.h 中添加 OTA 相关配置

// OTA 配置地址
#define OTA_CONFIG_BASE_ADDR    250

#define OTA_ENABLE              250  // OTA 使能
#define OTA_SERVER_IP1          251  // 服务器 IP 地址
#define OTA_SERVER_IP2          252
#define OTA_SERVER_IP3          253
#define OTA_SERVER_IP4          254
#define OTA_SERVER_PORT         255  // 服务器端口
#define OTA_CHECK_INTERVAL      256  // 检查间隔(小时)
#define OTA_AUTO_INSTALL        257  // 自动安装
```

## 7. 测试方案

### 7.1 单元测试

```c
// ota_test.c

#include "unity.h"
#include "ota_manager.h"
#include "ota_mock.h"

void setUp(void) {
    ota_mock_init();
}

void tearDown(void) {
    ota_mock_cleanup();
}

void test_ota_init_success(void) {
    ota_config_t config = {
        .server_url = "http://test.com",
        .device_id = "TEST_001"
    };

    TEST_ASSERT_EQUAL(0, ota_init(&config));
}

void test_ota_check_update_available(void) {
    ota_info_t info;

    // 模拟有更新可用
    ota_mock_set_update_available(true);

    TEST_ASSERT_EQUAL(0, ota_check_update(&info));
    TEST_ASSERT_EQUAL_STRING("1.2.0", info.version);
}

void test_ota_download_progress(void) {
    ota_info_t info = {
        .version = "1.2.0",
        .size = 1024,
        .url = "http://test.com/firmware.bin"
    };

    bool progress_called = false;

    ota_start_download(&info, test_progress_callback, NULL);

    // 验证进度回调被调用
    TEST_ASSERT_TRUE(progress_called);
}
```

### 7.2 集成测试

```c
// 完整升级流程测试
void test_full_upgrade_process(void) {
    // 1. 初始化
    TEST_ASSERT_EQUAL(0, ota_init(&test_config));

    // 2. 检查更新
    ota_info_t info;
    TEST_ASSERT_EQUAL(0, ota_check_update(&info));

    // 3. 下载固件
    TEST_ASSERT_EQUAL(0, ota_start_download(&info, NULL, NULL));

    // 4. 等待下载完成
    while (ota_get_state() == OTA_STATE_DOWNLOADING) {
        delay_ms(100);
    }

    // 5. 安装固件
    TEST_ASSERT_EQUAL(0, ota_install_firmware());

    // 6. 验证状态
    TEST_ASSERT_EQUAL(OTA_STATE_COMPLETED, ota_get_state());
}
```

### 7.3 压力测试

```c
// 网络中断测试
void test_network_interruption(void) {
    ota_info_t info = create_test_firmware_info();

    // 开始下载
    ota_start_download(&info, NULL, NULL);

    // 模拟网络中断
    ota_mock_set_network_error(true);
    delay_ms(1000);

    // 恢复网络
    ota_mock_set_network_error(false);

    // 验证能够续传
    TEST_ASSERT_EQUAL(OTA_STATE_DOWNLOADING, ota_get_state());
}
```

## 8. 部署指南

### 8.1 编译配置

```makefile
# Makefile 添加 OTA 模块

OTA_DIR = app/ota
OTA_SOURCES = $(wildcard $(OTA_DIR)/src/*.c)
OTA_INCLUDES = -I$(OTA_DIR)/include

# 添加到编译源文件
SOURCES += $(OTA_SOURCES)

# 添加到包含路径
INCLUDES += $(OTA_INCLUDES)

# 添加预编译宏
DEFINES += -DOTA_ENABLE=1
DEFINES += -DOTA_MAX_FIRMWARE_SIZE=0x38000
DEFINES += -DOTA_CHUNK_SIZE=1024
```

### 8.2 Flash 分区配置

```c
// flash_layout.h

// Flash 总大小: 512KB
#define FLASH_SIZE              0x80000

// Bootloader: 32KB (0x08000000 - 0x08008000)
#define BOOTLOADER_START        0x08000000
#define BOOTLOADER_SIZE         0x8000

// 应用程序1: 224KB (0x08008000 - 0x08040000)
#define APP1_START              0x08008000
#define APP1_SIZE               0x38000

// 应用程序2: 224KB (0x08040000 - 0x08078000)
#define APP2_START              0x08040000
#define APP2_SIZE               0x38000

// 配置区: 32KB (0x08078000 - 0x08080000)
#define CONFIG_START            0x08078000
#define CONFIG_SIZE             0x8000
```

### 8.3 服务器端配置

#### 8.3.1 固件管理 API

```json
// 检查更新 API
POST /api/v1/ota/check
{
    "device_id": "DTU_001",
    "current_version": "1.1.26",
    "hardware_version": "NANO100B"
}

// 响应
{
    "update_available": true,
    "firmware": {
        "version": "1.2.0",
        "size": 245760,
        "url": "https://ota.example.com/firmware/DTU_v1.2.0.bin",
        "checksum": "sha256:abc123...",
        "signature": "rsa:def456...",
        "release_notes": "Bug fixes and improvements"
    }
}
```

#### 8.3.2 下载 API

```http
GET /firmware/DTU_v1.2.0.bin
Range: bytes=0-1023

HTTP/1.1 206 Partial Content
Content-Range: bytes 0-1023/245760
Content-Length: 1024
```

## 9. 监控和维护

### 9.1 日志系统

```c
// ota_log.h

typedef enum {
    OTA_LOG_ERROR = 0,
    OTA_LOG_WARN,
    OTA_LOG_INFO,
    OTA_LOG_DEBUG
} ota_log_level_t;

#define OTA_LOGE(fmt, ...) ota_log(OTA_LOG_ERROR, fmt, ##__VA_ARGS__)
#define OTA_LOGW(fmt, ...) ota_log(OTA_LOG_WARN, fmt, ##__VA_ARGS__)
#define OTA_LOGI(fmt, ...) ota_log(OTA_LOG_INFO, fmt, ##__VA_ARGS__)
#define OTA_LOGD(fmt, ...) ota_log(OTA_LOG_DEBUG, fmt, ##__VA_ARGS__)

void ota_log(ota_log_level_t level, const char* fmt, ...);
```

### 9.2 状态监控

```c
// ota_monitor.c

typedef struct {
    uint32_t total_updates;
    uint32_t successful_updates;
    uint32_t failed_updates;
    uint32_t rollbacks;
    uint32_t last_update_time;
    char last_error[64];
} ota_statistics_t;

void ota_update_statistics(ota_state_t state, int error_code) {
    static ota_statistics_t stats = {0};

    switch (state) {
        case OTA_STATE_COMPLETED:
            stats.successful_updates++;
            stats.last_update_time = get_timestamp();
            break;

        case OTA_STATE_FAILED:
            stats.failed_updates++;
            snprintf(stats.last_error, sizeof(stats.last_error),
                    "Error code: %d", error_code);
            break;

        case OTA_STATE_ROLLBACK:
            stats.rollbacks++;
            break;
    }

    // 保存统计信息到 Flash
    save_statistics_to_flash(&stats);
}
```

## 10. 安全考虑

### 10.1 安全威胁分析

1. **固件篡改**: 使用数字签名验证
2. **中间人攻击**: 使用 HTTPS/TLS 加密传输
3. **重放攻击**: 添加时间戳和随机数验证
4. **降级攻击**: 版本号单调递增检查
5. **拒绝服务**: 限制下载频率和大小

### 10.2 安全措施实现

```c
// 安全配置
typedef struct {
    bool enable_tls;              // 启用 TLS
    bool verify_certificate;      // 验证证书
    bool enable_signature;        // 启用签名验证
    bool enable_encryption;       // 启用固件加密
    bool enable_rollback_protection; // 防回滚保护
    uint32_t max_download_size;   // 最大下载大小
    uint32_t download_timeout;    // 下载超时
} ota_security_config_t;
```

## 11. 性能优化

### 11.1 内存优化

```c
// 使用流式处理减少内存占用
#define OTA_BUFFER_SIZE    1024  // 1KB 缓冲区

int ota_stream_download(const char* url) {
    uint8_t buffer[OTA_BUFFER_SIZE];
    uint32_t offset = 0;

    while (offset < firmware_size) {
        int len = http_read(buffer, OTA_BUFFER_SIZE);
        if (len > 0) {
            // 直接写入 Flash，不缓存整个固件
            flash_write(APP2_START + offset, buffer, len);
            offset += len;
        }
    }

    return 0;
}
```

### 11.2 网络优化

```c
// 断点续传实现
int ota_resume_download(void) {
    uint32_t downloaded = get_downloaded_size();

    // 设置 HTTP Range 头
    char range_header[64];
    snprintf(range_header, sizeof(range_header),
             "Range: bytes=%u-", downloaded);

    // 从断点位置继续下载
    return http_download_with_range(firmware_url, downloaded);
}
```

## 12. 故障排除

### 12.1 常见问题

| 问题     | 原因           | 解决方案                     |
| -------- | -------------- | ---------------------------- |
| 下载失败 | 网络连接问题   | 检查网络配置，增加重试机制   |
| 校验失败 | 固件损坏或篡改 | 重新下载，检查签名           |
| 安装失败 | Flash 写入错误 | 检查 Flash 状态，重新擦除    |
| 启动失败 | 固件不兼容     | 自动回滚到上一版本           |
| 内存不足 | 缓冲区太小     | 调整缓冲区大小或使用流式处理 |

### 12.2 调试工具

```c
// OTA 调试命令
void ota_debug_command(const char* cmd) {
    if (strcmp(cmd, "status") == 0) {
        printf("OTA State: %d\n", ota_get_state());
        printf("Progress: %d%%\n", ota_get_progress_percent());
    } else if (strcmp(cmd, "check") == 0) {
        ota_check_update_async();
    } else if (strcmp(cmd, "download") == 0) {
        ota_start_download_async();
    } else if (strcmp(cmd, "install") == 0) {
        ota_install_firmware();
    } else if (strcmp(cmd, "rollback") == 0) {
        ota_rollback();
    }
}
```

## 13. 总结

本 OTA 升级模块采用模块化设计，具有以下特点：

1. **高度解耦**: 与主业务系统完全分离
2. **多通道支持**: 支持 HTTP/MQTT/LoRa 等多种传输方式
3. **安全可靠**: 完整的安全验证机制
4. **易于集成**: 提供简洁的 API 接口
5. **可扩展性**: 支持新的传输协议和安全算法

通过本手册的指导，可以快速集成 OTA 功能到现有的冷库 DTU 系统中，实现远程固件升级能力。

## 14. 服务器端开发要求

### 14.1 系统架构设计

#### 14.1.1 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                    OTA 管理平台                              │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  Web 控制台 │  │  移动端 APP │  │  API 网关   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  用户管理   │  │  权限管理   │  │  审计日志   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  固件管理   │  │  设备管理   │  │  升级管理   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  任务调度   │  │  消息队列   │  │  监控告警   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │  数据存储   │  │  文件存储   │  │  缓存系统   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

#### 14.1.2 技术栈推荐

```yaml
# 后端技术栈
Backend:
  Framework: Spring Boot / Django / Node.js
  Database: PostgreSQL / MySQL
  Cache: Redis
  Message Queue: RabbitMQ / Apache Kafka
  File Storage: MinIO / AWS S3
  Container: Docker + Kubernetes

# 前端技术栈
Frontend:
  Framework: React / Vue.js / Angular
  UI Library: Ant Design / Element UI
  Build Tool: Webpack / Vite

# 监控运维
DevOps:
  Monitoring: Prometheus + Grafana
  Logging: ELK Stack (Elasticsearch + Logstash + Kibana)
  CI/CD: Jenkins / GitLab CI
```

### 14.2 ROM 版本管理

#### 14.2.1 版本信息模型

```sql
-- 固件版本表
CREATE TABLE firmware_versions (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    version VARCHAR(32) NOT NULL UNIQUE,
    major_version INT NOT NULL,
    minor_version INT NOT NULL,
    patch_version INT NOT NULL,
    build_number INT NOT NULL,
    product_type VARCHAR(32) NOT NULL,
    hardware_version VARCHAR(32) NOT NULL,
    file_path VARCHAR(512) NOT NULL,
    file_size BIGINT NOT NULL,
    checksum_md5 VARCHAR(32) NOT NULL,
    checksum_sha256 VARCHAR(64) NOT NULL,
    signature TEXT,
    release_notes TEXT,
    is_stable BOOLEAN DEFAULT FALSE,
    is_active BOOLEAN DEFAULT TRUE,
    created_by BIGINT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    INDEX idx_product_version (product_type, version),
    INDEX idx_hardware_version (hardware_version),
    INDEX idx_created_at (created_at)
);

-- 版本依赖关系表
CREATE TABLE version_dependencies (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    version_id BIGINT NOT NULL,
    dependency_version_id BIGINT NOT NULL,
    dependency_type ENUM('REQUIRED', 'RECOMMENDED', 'INCOMPATIBLE') NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    FOREIGN KEY (version_id) REFERENCES firmware_versions(id),
    FOREIGN KEY (dependency_version_id) REFERENCES firmware_versions(id),
    UNIQUE KEY uk_version_dependency (version_id, dependency_version_id)
);
```

#### 14.2.2 版本管理 API

```java
// Spring Boot 示例
@RestController
@RequestMapping("/api/v1/firmware")
public class FirmwareController {

    @Autowired
    private FirmwareService firmwareService;

    /**
     * 上传新固件版本
     */
    @PostMapping("/upload")
    public ResponseEntity<FirmwareVersion> uploadFirmware(
            @RequestParam("file") MultipartFile file,
            @RequestParam("version") String version,
            @RequestParam("productType") String productType,
            @RequestParam("hardwareVersion") String hardwareVersion,
            @RequestParam("releaseNotes") String releaseNotes) {

        FirmwareVersion firmware = firmwareService.uploadFirmware(
            file, version, productType, hardwareVersion, releaseNotes);
        return ResponseEntity.ok(firmware);
    }

    /**
     * 获取版本列表
     */
    @GetMapping("/versions")
    public ResponseEntity<PageResult<FirmwareVersion>> getVersions(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int size,
            @RequestParam(required = false) String productType,
            @RequestParam(required = false) String hardwareVersion) {

        PageResult<FirmwareVersion> result = firmwareService.getVersions(
            page, size, productType, hardwareVersion);
        return ResponseEntity.ok(result);
    }

    /**
     * 版本比较
     */
    @GetMapping("/compare")
    public ResponseEntity<VersionCompareResult> compareVersions(
            @RequestParam String fromVersion,
            @RequestParam String toVersion) {

        VersionCompareResult result = firmwareService.compareVersions(
            fromVersion, toVersion);
        return ResponseEntity.ok(result);
    }

    /**
     * 设置版本状态
     */
    @PutMapping("/{versionId}/status")
    public ResponseEntity<Void> updateVersionStatus(
            @PathVariable Long versionId,
            @RequestParam boolean isStable,
            @RequestParam boolean isActive) {

        firmwareService.updateVersionStatus(versionId, isStable, isActive);
        return ResponseEntity.ok().build();
    }
}
```

#### 14.2.3 版本管理服务

```java
@Service
public class FirmwareService {

    @Autowired
    private FirmwareRepository firmwareRepository;

    @Autowired
    private FileStorageService fileStorageService;

    /**
     * 上传固件文件
     */
    public FirmwareVersion uploadFirmware(MultipartFile file, String version,
            String productType, String hardwareVersion, String releaseNotes) {

        // 1. 验证版本号格式
        validateVersionFormat(version);

        // 2. 检查版本是否已存在
        if (firmwareRepository.existsByVersion(version)) {
            throw new BusinessException("版本已存在: " + version);
        }

        // 3. 上传文件到存储系统
        String filePath = fileStorageService.uploadFile(file, "firmware/" + version);

        // 4. 计算文件校验和
        String md5 = calculateMD5(file);
        String sha256 = calculateSHA256(file);

        // 5. 生成数字签名
        String signature = generateSignature(file);

        // 6. 保存版本信息
        FirmwareVersion firmware = new FirmwareVersion();
        firmware.setVersion(version);
        firmware.setProductType(productType);
        firmware.setHardwareVersion(hardwareVersion);
        firmware.setFilePath(filePath);
        firmware.setFileSize(file.getSize());
        firmware.setChecksumMd5(md5);
        firmware.setChecksumSha256(sha256);
        firmware.setSignature(signature);
        firmware.setReleaseNotes(releaseNotes);

        return firmwareRepository.save(firmware);
    }

    /**
     * 版本兼容性检查
     */
    public boolean isVersionCompatible(String currentVersion, String targetVersion) {
        // 实现版本兼容性逻辑
        VersionInfo current = parseVersion(currentVersion);
        VersionInfo target = parseVersion(targetVersion);

        // 主版本号不同时不兼容
        if (current.getMajor() != target.getMajor()) {
            return false;
        }

        // 只允许升级，不允许降级
        return target.compareTo(current) > 0;
    }
}
```

### 14.3 设备管理系统

#### 14.3.1 设备信息模型

```sql
-- 设备信息表
CREATE TABLE devices (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id VARCHAR(64) NOT NULL UNIQUE,
    device_name VARCHAR(128) NOT NULL,
    product_type VARCHAR(32) NOT NULL,
    hardware_version VARCHAR(32) NOT NULL,
    current_firmware_version VARCHAR(32),
    target_firmware_version VARCHAR(32),
    device_status ENUM('ONLINE', 'OFFLINE', 'UPGRADING', 'ERROR') DEFAULT 'OFFLINE',
    last_heartbeat TIMESTAMP,
    location VARCHAR(256),
    owner_id BIGINT,
    group_id BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    INDEX idx_device_id (device_id),
    INDEX idx_product_type (product_type),
    INDEX idx_status (device_status),
    INDEX idx_heartbeat (last_heartbeat)
);

-- 设备分组表
CREATE TABLE device_groups (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    group_name VARCHAR(128) NOT NULL,
    group_type VARCHAR(32) NOT NULL,
    description TEXT,
    parent_group_id BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    INDEX idx_parent_group (parent_group_id)
);

-- 设备属性表
CREATE TABLE device_properties (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id BIGINT NOT NULL,
    property_key VARCHAR(64) NOT NULL,
    property_value TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    FOREIGN KEY (device_id) REFERENCES devices(id),
    UNIQUE KEY uk_device_property (device_id, property_key)
);
```

#### 14.3.2 设备管理 API

```java
@RestController
@RequestMapping("/api/v1/devices")
public class DeviceController {

    @Autowired
    private DeviceService deviceService;

    /**
     * 设备注册
     */
    @PostMapping("/register")
    public ResponseEntity<Device> registerDevice(@RequestBody DeviceRegisterRequest request) {
        Device device = deviceService.registerDevice(request);
        return ResponseEntity.ok(device);
    }

    /**
     * 设备心跳
     */
    @PostMapping("/{deviceId}/heartbeat")
    public ResponseEntity<DeviceHeartbeatResponse> heartbeat(
            @PathVariable String deviceId,
            @RequestBody DeviceHeartbeatRequest request) {

        DeviceHeartbeatResponse response = deviceService.processHeartbeat(deviceId, request);
        return ResponseEntity.ok(response);
    }

    /**
     * 获取设备列表
     */
    @GetMapping
    public ResponseEntity<PageResult<Device>> getDevices(
            @RequestParam(defaultValue = "1") int page,
            @RequestParam(defaultValue = "20") int size,
            @RequestParam(required = false) String productType,
            @RequestParam(required = false) String status,
            @RequestParam(required = false) Long groupId) {

        PageResult<Device> result = deviceService.getDevices(page, size, productType, status, groupId);
        return ResponseEntity.ok(result);
    }

    /**
     * 设备分组管理
     */
    @PostMapping("/groups")
    public ResponseEntity<DeviceGroup> createGroup(@RequestBody DeviceGroup group) {
        DeviceGroup created = deviceService.createGroup(group);
        return ResponseEntity.ok(created);
    }

    /**
     * 批量设备操作
     */
    @PostMapping("/batch")
    public ResponseEntity<BatchOperationResult> batchOperation(
            @RequestBody BatchOperationRequest request) {

        BatchOperationResult result = deviceService.batchOperation(request);
        return ResponseEntity.ok(result);
    }
}
```

### 14.4 OTA 升级计划管理

#### 14.4.1 升级计划模型

```sql
-- 升级计划表
CREATE TABLE upgrade_plans (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    plan_name VARCHAR(128) NOT NULL,
    plan_type ENUM('IMMEDIATE', 'SCHEDULED', 'GRADUAL') NOT NULL,
    source_version VARCHAR(32),
    target_version VARCHAR(32) NOT NULL,
    plan_status ENUM('DRAFT', 'APPROVED', 'RUNNING', 'PAUSED', 'COMPLETED', 'FAILED') DEFAULT 'DRAFT',
    scheduled_time TIMESTAMP,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    rollback_enabled BOOLEAN DEFAULT TRUE,
    max_failure_rate DECIMAL(5,2) DEFAULT 10.00,
    batch_size INT DEFAULT 10,
    batch_interval INT DEFAULT 300, -- 秒
    created_by BIGINT NOT NULL,
    approved_by BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

    INDEX idx_plan_status (plan_status),
    INDEX idx_scheduled_time (scheduled_time),
    INDEX idx_target_version (target_version)
);

-- 升级计划设备表
CREATE TABLE upgrade_plan_devices (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    plan_id BIGINT NOT NULL,
    device_id BIGINT NOT NULL,
    batch_number INT NOT NULL,
    device_status ENUM('PENDING', 'DOWNLOADING', 'INSTALLING', 'SUCCESS', 'FAILED', 'SKIPPED') DEFAULT 'PENDING',
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    error_message TEXT,
    retry_count INT DEFAULT 0,

    FOREIGN KEY (plan_id) REFERENCES upgrade_plans(id),
    FOREIGN KEY (device_id) REFERENCES devices(id),
    UNIQUE KEY uk_plan_device (plan_id, device_id),
    INDEX idx_batch_number (batch_number),
    INDEX idx_device_status (device_status)
);

-- 升级计划审批表
CREATE TABLE upgrade_plan_approvals (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    plan_id BIGINT NOT NULL,
    approver_id BIGINT NOT NULL,
    approval_status ENUM('PENDING', 'APPROVED', 'REJECTED') NOT NULL,
    approval_comment TEXT,
    approved_at TIMESTAMP,

    FOREIGN KEY (plan_id) REFERENCES upgrade_plans(id),
    UNIQUE KEY uk_plan_approver (plan_id, approver_id)
);
```

#### 14.4.2 升级计划服务

```java
@Service
public class UpgradePlanService {

    @Autowired
    private UpgradePlanRepository planRepository;

    @Autowired
    private DeviceService deviceService;

    @Autowired
    private TaskScheduler taskScheduler;

    /**
     * 创建升级计划
     */
    public UpgradePlan createUpgradePlan(CreateUpgradePlanRequest request) {
        // 1. 验证目标版本
        validateTargetVersion(request.getTargetVersion());

        // 2. 筛选符合条件的设备
        List<Device> targetDevices = deviceService.findDevicesForUpgrade(
            request.getDeviceFilter());

        if (targetDevices.isEmpty()) {
            throw new BusinessException("没有找到符合条件的设备");
        }

        // 3. 创建升级计划
        UpgradePlan plan = new UpgradePlan();
        plan.setPlanName(request.getPlanName());
        plan.setPlanType(request.getPlanType());
        plan.setTargetVersion(request.getTargetVersion());
        plan.setScheduledTime(request.getScheduledTime());
        plan.setBatchSize(request.getBatchSize());
        plan.setBatchInterval(request.getBatchInterval());
        plan.setMaxFailureRate(request.getMaxFailureRate());

        plan = planRepository.save(plan);

        // 4. 分配设备到批次
        assignDevicesToBatches(plan, targetDevices);

        return plan;
    }

    /**
     * 执行升级计划
     */
    public void executeUpgradePlan(Long planId) {
        UpgradePlan plan = planRepository.findById(planId)
            .orElseThrow(() -> new BusinessException("升级计划不存在"));

        if (plan.getPlanStatus() != PlanStatus.APPROVED) {
            throw new BusinessException("升级计划未审批通过");
        }

        // 更新计划状态
        plan.setPlanStatus(PlanStatus.RUNNING);
        plan.setStartTime(new Date());
        planRepository.save(plan);

        // 根据计划类型执行
        switch (plan.getPlanType()) {
            case IMMEDIATE:
                executeImmediateUpgrade(plan);
                break;
            case SCHEDULED:
                scheduleUpgrade(plan);
                break;
            case GRADUAL:
                executeGradualUpgrade(plan);
                break;
        }
    }

    /**
     * 分批升级执行
     */
    private void executeGradualUpgrade(UpgradePlan plan) {
        List<UpgradePlanDevice> devices = getDevicesByBatch(plan.getId(), 1);

        for (UpgradePlanDevice device : devices) {
            try {
                // 发送升级指令到设备
                sendUpgradeCommand(device);

                // 更新设备状态
                device.setDeviceStatus(DeviceUpgradeStatus.DOWNLOADING);
                device.setStartTime(new Date());

            } catch (Exception e) {
                device.setDeviceStatus(DeviceUpgradeStatus.FAILED);
                device.setErrorMessage(e.getMessage());
            }
        }

        // 监控当前批次完成情况
        monitorBatchProgress(plan, 1);
    }

    /**
     * 监控批次进度
     */
    private void monitorBatchProgress(UpgradePlan plan, int batchNumber) {
        taskScheduler.scheduleWithFixedDelay(() -> {
            BatchProgressInfo progress = getBatchProgress(plan.getId(), batchNumber);

            if (progress.isCompleted()) {
                // 检查失败率
                if (progress.getFailureRate() > plan.getMaxFailureRate()) {
                    // 失败率过高，暂停计划
                    pauseUpgradePlan(plan.getId(), "失败率超过阈值");
                    return;
                }

                // 启动下一批次
                int nextBatch = batchNumber + 1;
                if (hasNextBatch(plan.getId(), nextBatch)) {
                    scheduleNextBatch(plan, nextBatch);
                } else {
                    // 所有批次完成
                    completeUpgradePlan(plan.getId());
                }
            }
        }, Duration.ofSeconds(30));
    }
}
```

### 14.5 升级失败管理

#### 14.5.1 失败记录模型

```sql
-- 升级失败记录表
CREATE TABLE upgrade_failures (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id BIGINT NOT NULL,
    plan_id BIGINT,
    firmware_version VARCHAR(32) NOT NULL,
    failure_stage ENUM('DOWNLOAD', 'VERIFY', 'INSTALL', 'BOOT', 'ROLLBACK') NOT NULL,
    error_code VARCHAR(32),
    error_message TEXT,
    stack_trace TEXT,
    device_logs TEXT,
    failure_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    resolved BOOLEAN DEFAULT FALSE,
    resolution_notes TEXT,
    resolved_at TIMESTAMP,
    resolved_by BIGINT,

    FOREIGN KEY (device_id) REFERENCES devices(id),
    FOREIGN KEY (plan_id) REFERENCES upgrade_plans(id),
    INDEX idx_device_failure (device_id, failure_time),
    INDEX idx_failure_stage (failure_stage),
    INDEX idx_resolved (resolved)
);

-- 自动恢复策略表
CREATE TABLE recovery_strategies (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    strategy_name VARCHAR(128) NOT NULL,
    failure_pattern VARCHAR(256) NOT NULL, -- 正则表达式匹配失败模式
    recovery_action ENUM('RETRY', 'ROLLBACK', 'SKIP', 'MANUAL') NOT NULL,
    max_retry_count INT DEFAULT 3,
    retry_interval INT DEFAULT 300, -- 秒
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### 14.5.2 失败处理服务

```java
@Service
public class UpgradeFailureService {

    @Autowired
    private UpgradeFailureRepository failureRepository;

    @Autowired
    private RecoveryStrategyRepository strategyRepository;

    @Autowired
    private NotificationService notificationService;

    /**
     * 记录升级失败
     */
    public void recordUpgradeFailure(UpgradeFailureRequest request) {
        UpgradeFailure failure = new UpgradeFailure();
        failure.setDeviceId(request.getDeviceId());
        failure.setPlanId(request.getPlanId());
        failure.setFirmwareVersion(request.getFirmwareVersion());
        failure.setFailureStage(request.getFailureStage());
        failure.setErrorCode(request.getErrorCode());
        failure.setErrorMessage(request.getErrorMessage());
        failure.setDeviceLogs(request.getDeviceLogs());

        failureRepository.save(failure);

        // 触发自动恢复策略
        triggerRecoveryStrategy(failure);

        // 发送告警通知
        sendFailureNotification(failure);
    }

    /**
     * 触发恢复策略
     */
    private void triggerRecoveryStrategy(UpgradeFailure failure) {
        List<RecoveryStrategy> strategies = strategyRepository.findActiveStrategies();

        for (RecoveryStrategy strategy : strategies) {
            if (matchesFailurePattern(failure, strategy.getFailurePattern())) {
                executeRecoveryAction(failure, strategy);
                break;
            }
        }
    }

    /**
     * 执行恢复动作
     */
    private void executeRecoveryAction(UpgradeFailure failure, RecoveryStrategy strategy) {
        switch (strategy.getRecoveryAction()) {
            case RETRY:
                scheduleRetry(failure, strategy);
                break;
            case ROLLBACK:
                initiateRollback(failure);
                break;
            case SKIP:
                markDeviceAsSkipped(failure);
                break;
            case MANUAL:
                createManualTask(failure);
                break;
        }
    }

    /**
     * 失败统计分析
     */
    public FailureAnalysisReport generateFailureReport(FailureReportRequest request) {
        Date startDate = request.getStartDate();
        Date endDate = request.getEndDate();

        // 按失败阶段统计
        Map<FailureStage, Long> stageStats = failureRepository
            .countByFailureStageAndTimePeriod(startDate, endDate);

        // 按错误代码统计
        Map<String, Long> errorCodeStats = failureRepository
            .countByErrorCodeAndTimePeriod(startDate, endDate);

        // 按设备型号统计
        Map<String, Long> deviceTypeStats = failureRepository
            .countByDeviceTypeAndTimePeriod(startDate, endDate);

        // 失败趋势分析
        List<FailureTrendData> trendData = failureRepository
            .getFailureTrendData(startDate, endDate);

        return FailureAnalysisReport.builder()
            .stageStatistics(stageStats)
            .errorCodeStatistics(errorCodeStats)
            .deviceTypeStatistics(deviceTypeStats)
            .trendData(trendData)
            .build();
    }

    /**
     * 自动修复建议
     */
    public List<RepairSuggestion> generateRepairSuggestions(Long deviceId) {
        List<UpgradeFailure> recentFailures = failureRepository
            .findRecentFailuresByDevice(deviceId, 30); // 最近30天

        List<RepairSuggestion> suggestions = new ArrayList<>();

        // 分析失败模式
        Map<String, Long> errorPatterns = recentFailures.stream()
            .collect(Collectors.groupingBy(
                UpgradeFailure::getErrorCode,
                Collectors.counting()));

        for (Map.Entry<String, Long> entry : errorPatterns.entrySet()) {
            String errorCode = entry.getKey();
            Long count = entry.getValue();

            RepairSuggestion suggestion = generateSuggestionForError(errorCode, count);
            if (suggestion != null) {
                suggestions.add(suggestion);
            }
        }

        return suggestions;
    }
}
```

### 14.6 监控和告警系统

#### 14.6.1 监控指标定义

```java
@Component
public class OTAMetricsCollector {

    private final MeterRegistry meterRegistry;
    private final Counter upgradeSuccessCounter;
    private final Counter upgradeFailureCounter;
    private final Timer upgradeTimer;
    private final Gauge activeDevicesGauge;

    public OTAMetricsCollector(MeterRegistry meterRegistry) {
        this.meterRegistry = meterRegistry;

        // 升级成功计数器
        this.upgradeSuccessCounter = Counter.builder("ota.upgrade.success")
            .description("Number of successful upgrades")
            .tag("type", "firmware")
            .register(meterRegistry);

        // 升级失败计数器
        this.upgradeFailureCounter = Counter.builder("ota.upgrade.failure")
            .description("Number of failed upgrades")
            .tag("type", "firmware")
            .register(meterRegistry);

        // 升级耗时
        this.upgradeTimer = Timer.builder("ota.upgrade.duration")
            .description("Time taken for upgrade")
            .register(meterRegistry);

        // 在线设备数量
        this.activeDevicesGauge = Gauge.builder("ota.devices.active")
            .description("Number of active devices")
            .register(meterRegistry, this, OTAMetricsCollector::getActiveDeviceCount);
    }

    public void recordUpgradeSuccess(String deviceType, String version) {
        upgradeSuccessCounter.increment(
            Tags.of("device_type", deviceType, "version", version));
    }

    public void recordUpgradeFailure(String deviceType, String version, String errorCode) {
        upgradeFailureCounter.increment(
            Tags.of("device_type", deviceType, "version", version, "error", errorCode));
    }

    private double getActiveDeviceCount() {
        // 实现获取在线设备数量的逻辑
        return deviceService.getActiveDeviceCount();
    }
}
```

#### 14.6.2 告警规则配置

```yaml
# Prometheus 告警规则
groups:
  - name: ota_alerts
    rules:
      # 升级失败率过高
      - alert: HighUpgradeFailureRate
        expr: |
          (
            rate(ota_upgrade_failure_total[5m]) / 
            (rate(ota_upgrade_success_total[5m]) + rate(ota_upgrade_failure_total[5m]))
          ) > 0.1
        for: 2m
        labels:
          severity: warning
        annotations:
          summary: "OTA upgrade failure rate is high"
          description: "Upgrade failure rate is {{ $value | humanizePercentage }} over the last 5 minutes"

      # 设备离线率过高
      - alert: HighDeviceOfflineRate
        expr: |
          (ota_devices_total - ota_devices_active) / ota_devices_total > 0.2
        for: 5m
        labels:
          severity: critical
        annotations:
          summary: "High device offline rate detected"
          description: "{{ $value | humanizePercentage }} of devices are offline"

      # 升级计划执行时间过长
      - alert: UpgradePlanTimeout
        expr: |
          time() - ota_upgrade_plan_start_time > 3600
        for: 0m
        labels:
          severity: warning
        annotations:
          summary: "Upgrade plan execution timeout"
          description: "Upgrade plan {{ $labels.plan_id }} has been running for more than 1 hour"
```

### 14.7 API 网关和安全

#### 14.7.1 API 网关配置

```yaml
# Spring Cloud Gateway 配置
spring:
  cloud:
    gateway:
      routes:
        # 设备 API 路由
        - id: device-api
          uri: lb://device-service
          predicates:
            - Path=/api/v1/devices/**
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 100
                redis-rate-limiter.burstCapacity: 200
            - name: AuthenticationFilter

        # 固件管理 API 路由
        - id: firmware-api
          uri: lb://firmware-service
          predicates:
            - Path=/api/v1/firmware/**
          filters:
            - name: RequestRateLimiter
              args:
                redis-rate-limiter.replenishRate: 50
                redis-rate-limiter.burstCapacity: 100
            - name: AuthenticationFilter
            - name: AuthorizationFilter
              args:
                required-role: ADMIN
```

#### 14.7.2 安全认证

```java
@Component
public class JWTAuthenticationFilter implements GlobalFilter {

    @Autowired
    private JWTTokenProvider tokenProvider;

    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = exchange.getRequest();

        // 跳过认证的路径
        if (isPublicPath(request.getPath().value())) {
            return chain.filter(exchange);
        }

        String token = extractToken(request);
        if (token == null || !tokenProvider.validateToken(token)) {
            return unauthorized(exchange);
        }

        // 添加用户信息到请求头
        String userId = tokenProvider.getUserIdFromToken(token);
        ServerHttpRequest mutatedRequest = request.mutate()
            .header("X-User-Id", userId)
            .build();

        return chain.filter(exchange.mutate().request(mutatedRequest).build());
    }

    private boolean isPublicPath(String path) {
        return path.startsWith("/api/v1/auth/") ||
               path.startsWith("/api/v1/devices/register");
    }
}
```

### 14.8 数据备份和恢复

#### 14.8.1 数据备份策略

```bash
#!/bin/bash
# 数据库备份脚本

BACKUP_DIR="/backup/ota"
DATE=$(date +%Y%m%d_%H%M%S)
DB_NAME="ota_platform"

# 创建备份目录
mkdir -p $BACKUP_DIR/$DATE

# 备份数据库
mysqldump -h $DB_HOST -u $DB_USER -p$DB_PASSWORD \
  --single-transaction \
  --routines \
  --triggers \
  $DB_NAME > $BACKUP_DIR/$DATE/database.sql

# 备份固件文件
rsync -av /data/firmware/ $BACKUP_DIR/$DATE/firmware/

# 压缩备份文件
tar -czf $BACKUP_DIR/ota_backup_$DATE.tar.gz -C $BACKUP_DIR $DATE

# 清理旧备份（保留30天）
find $BACKUP_DIR -name "ota_backup_*.tar.gz" -mtime +30 -delete

# 上传到云存储
aws s3 cp $BACKUP_DIR/ota_backup_$DATE.tar.gz s3://ota-backups/
```

#### 14.8.2 灾难恢复计划

```yaml
# 灾难恢复配置
disaster_recovery:
  rpo: 1h # 恢复点目标
  rto: 4h # 恢复时间目标

  backup_strategy:
    database:
      frequency: hourly
      retention: 30d
      location: s3://ota-backups/db/

    files:
      frequency: daily
      retention: 90d
      location: s3://ota-backups/files/

  recovery_procedures:
    - name: "Database Recovery"
      steps:
        - "Stop application services"
        - "Restore database from latest backup"
        - "Verify data integrity"
        - "Start application services"

    - name: "File Recovery"
      steps:
        - "Download backup from S3"
        - "Extract files to target location"
        - "Update file permissions"
        - "Verify file integrity"
```

### 14.9 性能优化建议

#### 14.9.1 数据库优化

```sql
-- 创建必要的索引
CREATE INDEX idx_devices_status_heartbeat ON devices(device_status, last_heartbeat);
CREATE INDEX idx_upgrade_plans_status_time ON upgrade_plans(plan_status, scheduled_time);
CREATE INDEX idx_failures_device_time ON upgrade_failures(device_id, failure_time);

-- 分区表设计（按时间分区）
CREATE TABLE upgrade_logs (
    id BIGINT NOT NULL,
    device_id BIGINT NOT NULL,
    log_time TIMESTAMP NOT NULL,
    log_content TEXT,
    PRIMARY KEY (id, log_time)
) PARTITION BY RANGE (YEAR(log_time)) (
    PARTITION p2023 VALUES LESS THAN (2024),
    PARTITION p2024 VALUES LESS THAN (2025),
    PARTITION p2025 VALUES LESS THAN (2026)
);
```

#### 14.9.2 缓存策略

```java
@Service
public class CachedDeviceService {

    @Autowired
    private RedisTemplate<String, Object> redisTemplate;

    @Cacheable(value = "device", key = "#deviceId")
    public Device getDevice(String deviceId) {
        return deviceRepository.findByDeviceId(deviceId);
    }

    @CacheEvict(value = "device", key = "#device.deviceId")
    public Device updateDevice(Device device) {
        return deviceRepository.save(device);
    }

    // 批量缓存设备状态
    public void cacheDeviceStatuses(List<Device> devices) {
        Map<String, DeviceStatus> statusMap = devices.stream()
            .collect(Collectors.toMap(
                Device::getDeviceId,
                Device::getStatus
            ));

        redisTemplate.opsForHash().putAll("device:status", statusMap);
        redisTemplate.expire("device:status", Duration.ofMinutes(5));
    }
}
```

### 14.10 部署和运维

#### 14.10.1 Docker 容器化

```dockerfile
# Dockerfile
FROM openjdk:11-jre-slim

WORKDIR /app

COPY target/ota-platform.jar app.jar

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:8080/actuator/health || exit 1

ENTRYPOINT ["java", "-jar", "app.jar"]
```

```yaml
# docker-compose.yml
version: "3.8"

services:
  ota-platform:
    build: .
    ports:
      - "8080:8080"
    environment:
      - SPRING_PROFILES_ACTIVE=production
      - DB_HOST=mysql
      - REDIS_HOST=redis
    depends_on:
      - mysql
      - redis
    volumes:
      - ./logs:/app/logs
      - ./data:/app/data

  mysql:
    image: mysql:8.0
    environment:
      MYSQL_ROOT_PASSWORD: rootpassword
      MYSQL_DATABASE: ota_platform
    volumes:
      - mysql_data:/var/lib/mysql
    ports:
      - "3306:3306"

  redis:
    image: redis:6-alpine
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data

volumes:
  mysql_data:
  redis_data:
```

#### 14.10.2 Kubernetes 部署

```yaml
# k8s-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ota-platform
spec:
  replicas: 3
  selector:
    matchLabels:
      app: ota-platform
  template:
    metadata:
      labels:
        app: ota-platform
    spec:
      containers:
        - name: ota-platform
          image: ota-platform:latest
          ports:
            - containerPort: 8080
          env:
            - name: SPRING_PROFILES_ACTIVE
              value: "kubernetes"
          resources:
            requests:
              memory: "512Mi"
              cpu: "250m"
            limits:
              memory: "1Gi"
              cpu: "500m"
          livenessProbe:
            httpGet:
              path: /actuator/health
              port: 8080
            initialDelaySeconds: 30
            periodSeconds: 10
          readinessProbe:
            httpGet:
              path: /actuator/health
              port: 8080
            initialDelaySeconds: 5
            periodSeconds: 5

---
apiVersion: v1
kind: Service
metadata:
  name: ota-platform-service
spec:
  selector:
    app: ota-platform
  ports:
    - protocol: TCP
      port: 80
      targetPort: 8080
  type: LoadBalancer
```

通过以上服务器端开发要求的详细说明，您可以构建一个完整的 OTA 远程管理系统，实现对冷库 DTU 设备的全生命周期管理。
