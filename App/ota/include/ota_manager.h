#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // OTA状态枚举
    typedef enum
    {
        OTA_STATE_IDLE = 0,
        OTA_STATE_CHECKING,
        OTA_STATE_DOWNLOADING,
        OTA_STATE_VERIFYING,
        OTA_STATE_INSTALLING,
        OTA_STATE_COMPLETED,
        OTA_STATE_FAILED,
        OTA_STATE_ROLLBACK
    } ota_state_t;

    // 传输类型
    typedef enum
    {
        OTA_TRANSPORT_HTTP = 0,
        OTA_TRANSPORT_MQTT,
        OTA_TRANSPORT_LORA
    } ota_transport_type_t;

    // 校验类型
    typedef enum
    {
        OTA_CHECKSUM_MD5 = 0,
        OTA_CHECKSUM_SHA256,
        OTA_CHECKSUM_CRC32
    } ota_checksum_type_t;

    // 升级信息结构
    typedef struct
    {
        char version[16];
        uint32_t size;
        char url[256];
        char md5[33];
        uint32_t timestamp;
    } ota_info_t;

    // 进度回调函数类型
    typedef void (*ota_progress_cb_t)(uint32_t downloaded, uint32_t total);

    // 状态回调函数类型
    typedef void (*ota_state_cb_t)(ota_state_t state, int error_code);

    // OTA配置结构
    typedef struct
    {
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

    // API函数声明

    /**
     * @brief 初始化OTA模块
     * @param config 配置参数
     * @return 0: 成功, <0: 失败
     */
    int ota_init(const ota_config_t *config);

    /**
     * @brief 检查固件更新
     * @param info 返回的更新信息
     * @return 0: 有更新, 1: 无更新, <0: 错误
     */
    int ota_check_update(ota_info_t *info);

    /**
     * @brief 开始固件下载
     * @param info 固件信息
     * @param progress_cb 进度回调
     * @param state_cb 状态回调
     * @return 0: 成功, <0: 失败
     */
    int ota_start_download(const ota_info_t *info,
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
     * @return 当前OTA状态
     */
    ota_state_t ota_get_state(void);

    /**
     * @brief 获取升级进度
     * @param downloaded 已下载字节数
     * @param total 总字节数
     * @return 0: 成功, <0: 失败
     */
    int ota_get_progress(uint32_t *downloaded, uint32_t *total);

    /**
     * @brief 反初始化OTA模块
     */
    void ota_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // OTA_MANAGER_H