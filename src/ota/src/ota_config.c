/**
 * @file ota_config.c
 * @brief OTA配置实现文件
 * @version 1.0
 * @date 2024-06-05
 */

#include "../include/ota_config.h"
#include <string.h>

// 默认OTA配置
static ota_config_t default_ota_config = {
    .server_url = "https://ota.coolstorage.com",
    .server_port = 443,
    .device_id = "NANO_DTU_001",
    .api_key = "default_api_key",
    .transport_type = OTA_TRANSPORT_HTTP,
    .timeout_ms = 30000,
    .retry_count = 3,
    .chunk_size = 1024,
    .enable_signature_verify = true,
    .enable_encryption = false,
    .checksum_type = OTA_CHECKSUM_SHA256,
    .app_partition_size = 0x38000, // 224KB
    .backup_partition_addr = 0x08040000,
    .progress_callback = NULL,
    .state_callback = NULL};

/**
 * @brief 获取默认OTA配置
 * @return 默认配置指针
 */
const ota_config_t *ota_get_default_config(void)
{
    return &default_ota_config;
}

/**
 * @brief 验证OTA配置有效性
 * @param config 配置指针
 * @return 0: 有效, <0: 无效
 */
int ota_validate_config(const ota_config_t *config)
{
    if (!config)
    {
        return -1;
    }

    // 检查必要字段
    if (strlen(config->server_url) == 0 ||
        strlen(config->device_id) == 0)
    {
        return -2;
    }

    // 检查分区大小
    if (config->app_partition_size == 0)
    {
        return -3;
    }

    // 检查网络参数
    if (config->timeout_ms == 0 || config->retry_count == 0)
    {
        return -4;
    }

    return 0;
}

/**
 * @brief 复制OTA配置
 * @param dest 目标配置
 * @param src 源配置
 * @return 0: 成功, <0: 失败
 */
int ota_copy_config(ota_config_t *dest, const ota_config_t *src)
{
    if (!dest || !src)
    {
        return -1;
    }

    memcpy(dest, src, sizeof(ota_config_t));
    return 0;
}