#ifndef __GLOBAL_VARS_H__
#define __GLOBAL_VARS_H__

/**
 * @file global_vars.h
 * @brief 全局变量定义
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  全局变量声明
 *----------------------------------------------------------------------------*/

// 看门狗和复位相关
extern unsigned char WatchDogTestFlag; /*!< 看门狗测试标志 */
extern unsigned char ResetFlag;        /*!< 复位标志 */

// 时间间隔相关
extern unsigned char Interval; /*!< 时间间隔 */

// 设备参数
extern unsigned char DeviceNum;   /*!< 设备号 */
extern unsigned char ParaNum;     /*!< 参数数量 */
extern unsigned short ParaList[]; /*!< 参数列表 */
extern unsigned short VarList[];  /*!< 变量列表 */

// 启动相关
extern unsigned char StartBak; /*!< 启动备份 */

// 常量定义 (避免magic numbers)
#define cCoolCompensate 50  /*!< 冷却补偿参数索引 */
#define cLoraNodeInteral 60 /*!< LoRa节点间隔参数索引 */
#define cWorkMode 1         /*!< 工作模式 */
#define cSleepMode 0        /*!< 睡眠模式 */
#define cRealDataAct1 100   /*!< 实时数据1索引 */

#ifdef __cplusplus
}
#endif

#endif /* __GLOBAL_VARS_H__ */