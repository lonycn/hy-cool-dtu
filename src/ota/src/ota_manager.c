#include "ota_manager.h"
#include "ota_config.h"
#include <string.h>

// 静态变量
static ota_config_t g_ota_config;
static ota_state_t g_ota_state = OTA_STATE_IDLE;
static uint32_t g_downloaded = 0;
static uint32_t g_total_size = 0;
static bool g_ota_initialized = false;

// 默认配置
static ota_config_t default_config = {
    .server_url = OTA_DEFAULT_SERVER_URL,
    .server_port = OTA_DEFAULT_SERVER_PORT,
    .device_id = "",
    .api_key = "",
    .transport_type = OTA_TRANSPORT_HTTP,
    .timeout_ms = OTA_DEFAULT_TIMEOUT_MS,
    .retry_count = OTA_DEFAULT_RETRY_COUNT,
    .chunk_size = OTA_DEFAULT_CHUNK_SIZE,
    .enable_signature_verify = OTA_ENABLE_SIGNATURE_VERIFY,
    .enable_encryption = OTA_ENABLE_ENCRYPTION,
    .checksum_type = OTA_DEFAULT_CHECKSUM_TYPE,
    .app_partition_size = OTA_APP_SIZE,
    .backup_partition_addr = OTA_PARTITION_APP2,
    .progress_callback = NULL,
    .state_callback = NULL};

/**
 * @brief 获取默认配置
 */
ota_config_t *ota_get_default_config(void)
{
    return &default_config;
}

/**
 * @brief 初始化OTA模块
 */
int ota_init(const ota_config_t *config)
{
    if (config == NULL)
    {
        OTA_LOGE("Invalid config parameter");
        return OTA_ERROR_INVALID_PARAM;
    }

    // 复制配置
    memcpy(&g_ota_config, config, sizeof(ota_config_t));

    // 设置默认设备ID（如果为空）
    if (strlen(g_ota_config.device_id) == 0)
    {
        snprintf(g_ota_config.device_id, sizeof(g_ota_config.device_id),
                 "DTU_%s", OTA_HARDWARE_VERSION);
    }

    // 初始化状态
    g_ota_state = OTA_STATE_IDLE;
    g_downloaded = 0;
    g_total_size = 0;
    g_ota_initialized = true;

    OTA_LOGI("OTA模块初始化完成");
    OTA_LOGI("设备ID: %s", g_ota_config.device_id);
    OTA_LOGI("服务器: %s:%d", g_ota_config.server_url, g_ota_config.server_port);

    return OTA_ERROR_NONE;
}

/**
 * @brief 检查固件更新
 */
int ota_check_update(ota_info_t *info)
{
    if (!g_ota_initialized)
    {
        OTA_LOGE("OTA模块未初始化");
        return OTA_ERROR_INVALID_PARAM;
    }

    if (info == NULL)
    {
        OTA_LOGE("Invalid info parameter");
        return OTA_ERROR_INVALID_PARAM;
    }

    g_ota_state = OTA_STATE_CHECKING;

    // 模拟检查更新过程
    OTA_LOGI("检查固件更新...");

    // 这里应该实现实际的网络请求
    // 暂时返回无更新
    strcpy(info->version, OTA_FIRMWARE_VERSION);
    info->size = 0;
    strcpy(info->url, "");
    strcpy(info->md5, "");
    info->timestamp = 0;

    g_ota_state = OTA_STATE_IDLE;

    OTA_LOGI("当前已是最新版本");
    return 1; // 无更新
}

/**
 * @brief 开始固件下载
 */
int ota_start_download(const ota_info_t *info,
                       ota_progress_cb_t progress_cb,
                       ota_state_cb_t state_cb)
{
    if (!g_ota_initialized || info == NULL)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("开始下载固件: %s", info->version);

    g_ota_state = OTA_STATE_DOWNLOADING;
    g_total_size = info->size;
    g_downloaded = 0;

    // 更新回调函数
    if (progress_cb)
    {
        g_ota_config.progress_callback = progress_cb;
    }
    if (state_cb)
    {
        g_ota_config.state_callback = state_cb;
    }

    // 触发状态回调
    if (g_ota_config.state_callback)
    {
        g_ota_config.state_callback(g_ota_state, OTA_ERROR_NONE);
    }

    return OTA_ERROR_NONE;
}

/**
 * @brief 暂停下载
 */
int ota_pause_download(void)
{
    if (!g_ota_initialized)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("暂停下载");
    return OTA_ERROR_NONE;
}

/**
 * @brief 恢复下载
 */
int ota_resume_download(void)
{
    if (!g_ota_initialized)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("恢复下载");
    return OTA_ERROR_NONE;
}

/**
 * @brief 取消升级
 */
int ota_cancel_update(void)
{
    if (!g_ota_initialized)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("取消升级");
    g_ota_state = OTA_STATE_IDLE;
    g_downloaded = 0;
    g_total_size = 0;

    if (g_ota_config.state_callback)
    {
        g_ota_config.state_callback(g_ota_state, OTA_ERROR_CANCELLED);
    }

    return OTA_ERROR_NONE;
}

/**
 * @brief 安装固件
 */
int ota_install_firmware(void)
{
    if (!g_ota_initialized)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("开始安装固件");
    g_ota_state = OTA_STATE_INSTALLING;

    if (g_ota_config.state_callback)
    {
        g_ota_config.state_callback(g_ota_state, OTA_ERROR_NONE);
    }

    // 模拟安装过程
    // 实际实现应该包括Flash擦除、写入等操作

    g_ota_state = OTA_STATE_COMPLETED;
    if (g_ota_config.state_callback)
    {
        g_ota_config.state_callback(g_ota_state, OTA_ERROR_NONE);
    }

    return OTA_ERROR_NONE;
}

/**
 * @brief 回滚到上一版本
 */
int ota_rollback(void)
{
    if (!g_ota_initialized)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    OTA_LOGI("开始回滚");
    g_ota_state = OTA_STATE_ROLLBACK;

    if (g_ota_config.state_callback)
    {
        g_ota_config.state_callback(g_ota_state, OTA_ERROR_NONE);
    }

    return OTA_ERROR_NONE;
}

/**
 * @brief 获取当前状态
 */
ota_state_t ota_get_state(void)
{
    return g_ota_state;
}

/**
 * @brief 获取升级进度
 */
int ota_get_progress(uint32_t *downloaded, uint32_t *total)
{
    if (!g_ota_initialized || downloaded == NULL || total == NULL)
    {
        return OTA_ERROR_INVALID_PARAM;
    }

    *downloaded = g_downloaded;
    *total = g_total_size;

    return OTA_ERROR_NONE;
}

/**
 * @brief 反初始化OTA模块
 */
void ota_deinit(void)
{
    if (g_ota_initialized)
    {
        OTA_LOGI("OTA模块反初始化");
        g_ota_initialized = false;
        g_ota_state = OTA_STATE_IDLE;
        g_downloaded = 0;
        g_total_size = 0;
    }
}