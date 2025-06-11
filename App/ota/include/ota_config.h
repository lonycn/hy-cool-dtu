#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include "ota_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Flash分区配置
#define OTA_PARTITION_BOOTLOADER 0x08000000 // Bootloader分区 (32KB)
#define OTA_PARTITION_APP1 0x08008000       // 主应用分区 (224KB)
#define OTA_PARTITION_APP2 0x08040000       // 备份应用分区 (224KB)
#define OTA_PARTITION_CONFIG 0x08078000     // 配置分区 (32KB)

#define OTA_BOOTLOADER_SIZE 0x8000 // 32KB
#define OTA_APP_SIZE 0x38000       // 224KB
#define OTA_CONFIG_SIZE 0x8000     // 32KB

// OTA配置参数
#define OTA_DEFAULT_CHUNK_SIZE 1024       // 1KB
#define OTA_DEFAULT_TIMEOUT_MS 30000      // 30秒
#define OTA_DEFAULT_RETRY_COUNT 3         // 重试3次
#define OTA_DEFAULT_CHECK_INTERVAL 300000 // 5分钟检查一次

// 网络配置
#define OTA_DEFAULT_SERVER_URL "https://ota.example.com"
#define OTA_DEFAULT_SERVER_PORT 443
#define OTA_DEFAULT_API_VERSION "v1"

// 设备标识配置
#define OTA_DEVICE_TYPE "NANO_DTU"
#define OTA_HARDWARE_VERSION "NANO100SD3BN"
#define OTA_FIRMWARE_VERSION "3.4.2(3283)"

// 安全配置
#define OTA_ENABLE_SIGNATURE_VERIFY 1
#define OTA_ENABLE_ENCRYPTION 0
#define OTA_DEFAULT_CHECKSUM_TYPE OTA_CHECKSUM_SHA256

// 调试配置
#define OTA_DEBUG_ENABLE 1
#define OTA_LOG_LEVEL 2 // 0=ERROR, 1=WARN, 2=INFO, 3=DEBUG

#if OTA_DEBUG_ENABLE
#include <stdio.h>
#define OTA_LOGE(fmt, ...) printf("[OTA ERROR] " fmt "\n", ##__VA_ARGS__)
#define OTA_LOGW(fmt, ...) printf("[OTA WARN] " fmt "\n", ##__VA_ARGS__)
#define OTA_LOGI(fmt, ...) printf("[OTA INFO] " fmt "\n", ##__VA_ARGS__)
#if OTA_LOG_LEVEL >= 3
#define OTA_LOGD(fmt, ...) printf("[OTA DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define OTA_LOGD(fmt, ...)
#endif
#else
#define OTA_LOGE(fmt, ...)
#define OTA_LOGW(fmt, ...)
#define OTA_LOGI(fmt, ...)
#define OTA_LOGD(fmt, ...)
#endif

// 错误代码定义
#define OTA_ERROR_NONE 0
#define OTA_ERROR_INVALID_PARAM -1
#define OTA_ERROR_NETWORK_FAIL -2
#define OTA_ERROR_DOWNLOAD_FAIL -3
#define OTA_ERROR_VERIFY_FAIL -4
#define OTA_ERROR_FLASH_FAIL -5
#define OTA_ERROR_NO_SPACE -6
#define OTA_ERROR_TIMEOUT -7
#define OTA_ERROR_CANCELLED -8
#define OTA_ERROR_UNKNOWN -99

    // 默认配置函数
    ota_config_t *ota_get_default_config(void);

#ifdef __cplusplus
}
#endif

#endif // OTA_CONFIG_H