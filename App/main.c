// 基本类型定义
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned char bool;
#define true 1
#define false 0

// NANO100B 头文件
#include "Nano100Series.h"

// OTA 模块头文件
#ifdef OTA_ENABLE
#include "ota/include/ota_manager.h"
#include "ota/include/ota_config.h"
#endif

// 系统配置
#define SYSTEM_CLOCK_FREQ 48000000 // 48MHz
#define UART_BAUDRATE 115200
#define OTA_CHECK_INTERVAL 10000 // 10秒检查一次OTA

// 全局变量
static uint32_t system_tick = 0;
static uint32_t ota_check_timer = 0;

// 函数声明
void System_Init(void);
void UART_Init(void);
void Timer_Init(void);
void Main_Loop(void);

#ifdef OTA_ENABLE
void OTA_Init(void);
void OTA_Process(void);
static void ota_progress_callback(uint32_t downloaded, uint32_t total);
static void ota_state_callback(ota_state_t state, int error_code);
#endif

/**
 * @brief 系统初始化
 */
void System_Init(void)
{
    // 解锁寄存器保护
    SYS_UnlockReg();

    // 配置系统时钟
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_HCLK_CLK_DIVIDER(1));

    // 启用模块时钟
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    // 锁定寄存器保护
    SYS_LockReg();

    printf("\n=== 冷库DTU系统启动 ===\n");
    printf("版本: %s\n", "3.4.2(3283)");
    printf("芯片: NANO100BE\n");
    printf("时钟: %d MHz\n", SystemCoreClock / 1000000);

#ifdef OTA_ENABLE
    printf("OTA功能: 已启用\n");
#else
    printf("OTA功能: 已禁用\n");
#endif
}

/**
 * @brief UART初始化
 */
void UART_Init(void)
{
    // 配置UART0引脚
    SYS->PB_L_MFP = (SYS->PB_L_MFP & ~SYS_PB_L_MFP_PB0_MFP_Msk) | SYS_PB_L_MFP_PB0_MFP_UART0_TX;
    SYS->PB_L_MFP = (SYS->PB_L_MFP & ~SYS_PB_L_MFP_PB1_MFP_Msk) | SYS_PB_L_MFP_PB1_MFP_UART0_RX;

    // 配置UART0
    UART_Open(UART0, UART_BAUDRATE);

    printf("UART0 初始化完成，波特率: %d\n", UART_BAUDRATE);
}

/**
 * @brief 定时器初始化 (1ms中断)
 */
void Timer_Init(void)
{
    // 配置Timer0为1ms中断
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    printf("定时器初始化完成，1ms中断\n");
}

#ifdef OTA_ENABLE
/**
 * @brief OTA模块初始化
 */
void OTA_Init(void)
{
    printf("初始化OTA模块...\n");

    ota_config_t config = {
        .server_url = "https://ota.example.com",
        .server_port = 443,
        .device_id = "DTU_001",
        .api_key = "",
        .transport_type = OTA_TRANSPORT_HTTP,
        .timeout_ms = 30000,
        .retry_count = 3,
        .chunk_size = 1024,
        .enable_signature_verify = true,
        .enable_encryption = false,
        .checksum_type = OTA_CHECKSUM_SHA256,
        .app_partition_size = 0x38000,
        .backup_partition_addr = 0x08040000,
        .progress_callback = ota_progress_callback,
        .state_callback = ota_state_callback};

    if (ota_init(&config) == 0)
    {
        printf("✅ OTA模块初始化成功\n");
    }
    else
    {
        printf("❌ OTA模块初始化失败\n");
    }
}

/**
 * @brief OTA处理函数
 */
void OTA_Process(void)
{
    // 定期检查OTA更新
    if (++ota_check_timer >= OTA_CHECK_INTERVAL)
    {
        ota_check_timer = 0;

        printf("🔍 检查OTA更新...\n");

        ota_info_t info;
        int result = ota_check_update(&info);

        if (result == 0)
        {
            printf("📦 发现新版本: %s\n", info.version);
            printf("📏 固件大小: %d bytes\n", info.size);

            // 开始下载更新
            if (ota_start_download(&info, ota_progress_callback, ota_state_callback) == 0)
            {
                printf("⬇️  开始下载固件...\n");
            }
            else
            {
                printf("❌ 启动下载失败\n");
            }
        }
        else if (result == 1)
        {
            printf("ℹ️  当前已是最新版本\n");
        }
        else
        {
            printf("❌ 检查更新失败\n");
        }
    }

    // 处理OTA状态机
    // ota_state_machine_process();
}

/**
 * @brief OTA进度回调
 */
static void ota_progress_callback(uint32_t downloaded, uint32_t total)
{
    uint32_t progress = total > 0 ? (downloaded * 100 / total) : 0;
    printf("📊 下载进度: %d%% (%d/%d bytes)\n", progress, downloaded, total);
}

/**
 * @brief OTA状态回调
 */
static void ota_state_callback(ota_state_t state, int error_code)
{
    switch (state)
    {
    case OTA_STATE_IDLE:
        printf("🔄 OTA状态: 空闲\n");
        break;
    case OTA_STATE_CHECKING:
        printf("🔍 OTA状态: 检查更新中\n");
        break;
    case OTA_STATE_DOWNLOADING:
        printf("⬇️  OTA状态: 下载中\n");
        break;
    case OTA_STATE_VERIFYING:
        printf("🔐 OTA状态: 校验中\n");
        break;
    case OTA_STATE_INSTALLING:
        printf("⚙️  OTA状态: 安装中\n");
        break;
    case OTA_STATE_COMPLETED:
        printf("✅ OTA状态: 完成\n");
        printf("🔄 系统将在5秒后重启...\n");
        // 延迟重启
        for (int i = 5; i > 0; i--)
        {
            printf("%d... ", i);
            // 简单延迟
            for (volatile int j = 0; j < 1000000; j++)
                ;
        }
        printf("\n🔄 重启系统\n");
        NVIC_SystemReset();
        break;
    case OTA_STATE_FAILED:
        printf("❌ OTA状态: 失败 (错误代码: %d)\n", error_code);
        break;
    case OTA_STATE_ROLLBACK:
        printf("🔙 OTA状态: 回滚\n");
        break;
    default:
        printf("❓ OTA状态: 未知 (%d)\n", state);
        break;
    }
}
#endif

/**
 * @brief 主循环
 */
void Main_Loop(void)
{
    printf("🚀 进入主循环\n");

    while (1)
    {
        // 原有业务逻辑处理
        // ... 温湿度采集、设备控制、数据上传等 ...

#ifdef OTA_ENABLE
        // OTA处理
        OTA_Process();
#endif

        // 喂狗
        // WDT_RESET_COUNTER();

        // 简单延迟
        for (volatile int i = 0; i < 100000; i++)
            ;
    }
}

/**
 * @brief Timer0中断服务程序
 */
void TMR0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0))
    {
        TIMER_ClearIntFlag(TIMER0);
        system_tick++;
    }
}

/**
 * @brief 主函数
 */
int main(void)
{
    // 系统初始化
    System_Init();

    // 外设初始化
    UART_Init();
    Timer_Init();

#ifdef OTA_ENABLE
    // OTA模块初始化
    OTA_Init();
#endif

    printf("🎯 系统初始化完成\n");
    printf("⏰ 系统时间: %d ms\n", system_tick);

    // 进入主循环
    Main_Loop();

    return 0;
}