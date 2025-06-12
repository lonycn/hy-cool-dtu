#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

/**
 * @file constants.h
 * @brief 项目中缺失的常量定义
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Modbus相关常量定义
 *----------------------------------------------------------------------------*/

// 时间设置相关
#define cModifiedTime 2000 /*!< 时间修改标志地址 */

// 数据保存标志相关
#define cSaveDataFlag03D 3000 /*!< 数据保存标志起始地址 */
#define cParaActual 3000      /*!< 参数实际起始地址 */
#define cParaEndPos 3100      /*!< 参数结束位置 */

// 温度偏移相关
#define cTempOff 3000        /*!< 温度偏移地址 */
#define c02D_ParaActual 3000 /*!< 02D参数实际地址 */
#define cTempOffset 2000     /*!< 温度偏移值 */

// LoRa节点相关
#define cLoraNodeAliveSet 1000 /*!< LoRa节点活跃设置地址 */
#define cLoraNodeOffset 1500   /*!< LoRa节点偏移地址 */

// 节点功率相关
#define cNodePower 500 /*!< 节点功率地址 */

// 接收长度相关
#define cReceivelength 200 /*!< 接收长度限制 */

// 设备号相关
#define cDeviceNum 0 /*!< 设备号地址 */

// 背光控制相关
#define cBackLightControl 1200 /*!< 背光控制地址 */
#define cTestMode03D 1300      /*!< 03D测试模式地址 */

// 门变量和内部偏移相关
#define cDoorVar 100      /*!< 门变量地址 */
#define cInterTOffet 200  /*!< 内部温度偏移地址 */
#define cInterHROffet 250 /*!< 内部湿度偏移地址 */

/*----------------------------------------------------------------------------
  全局变量声明 (使用extern避免未使用警告)
 *----------------------------------------------------------------------------*/

// 这些变量在其他文件中定义
extern unsigned char StartBak;    /*!< 启动状态备份 */
extern unsigned char DeviceNum;   /*!< 设备号 */
extern unsigned char ParaNum;     /*!< 参数数量 */
extern unsigned short ParaList[]; /*!< 参数列表 */
extern unsigned short VarList[];  /*!< 变量列表 */

/*----------------------------------------------------------------------------
  函数声明
 *----------------------------------------------------------------------------*/

// UART通信函数
extern void SendDataToBus_uart0(unsigned char *data, unsigned char len);
extern void SendDataToBus_uart1(unsigned char *data, unsigned char len);
extern void SendDataToBus_uart2(unsigned char *data, unsigned char len);
extern void SendDataToBus_uart3(unsigned char *data, unsigned char len);
extern void SendDataToBus1(unsigned char *data, unsigned char len);

// CRC计算函数
extern unsigned short CRC(unsigned char *data, unsigned char len);

// 看门狗重置函数
extern void WatchdogReset(void);

// 历史数据处理函数
extern void DealWithHisRead(unsigned char *buffer);

// EEPROM操作函数
extern void ProgramEepromByte(void);
extern void ReadEepromByte(void);

// 报警处理函数
extern void DealAlarm(void);

// RTC设置函数
extern void SetPCF8563(unsigned char *time_data);

/*----------------------------------------------------------------------------
  IO 类型定义 (解决__IO未定义问题)
 *----------------------------------------------------------------------------*/
#ifndef __IO
#define __IO volatile /*!< Defines 'read / write' permissions */
#endif

#ifndef __O
#define __O volatile /*!< Defines 'write only' permissions */
#endif

#ifndef __I
#define __I volatile const /*!< Defines 'read only' permissions */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CONSTANTS_H__ */