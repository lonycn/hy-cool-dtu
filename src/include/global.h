#ifndef GLOBAL_H
#define GLOBAL_H

#include "Nano100Series.h"

// #define _UseTestBorad
// #define  Debuginfor
// #define    UseSim
#define cVarAlarmold 64
#define cVarAlarm 82

#define AlarmStartAddr 104

#define cSensorTye_OnlyRs485 0
#define cSensorTye_OnlyNtc 1
#define cSensorTye_Rs485_ntc 2
#define cSensorTye_Rs485_Qitiao 3
#define cSensorTye_ntc_Qitiao 4
#define cSensorTye_OnlyQitiao 5

#define TempCompesentStatus 127

// for 新增产品
#define cItemAddr 251
#define cCount 73
#define cReadTrig 74

struct Hisrory_Struct
{
    unsigned char AlarmLevel;
    unsigned char PowerVolume;
    //--------------------
    unsigned char MiniterStartFlag[8];
    unsigned char MiniterDelay[8];
    unsigned char Alarm1Count[8];
    uint16_t AlarmCountOverFlag[8];
    uint16_t MiniterDelaySet[8];
    unsigned char AlarmStatus1;
    unsigned char AlarmFlag;
    unsigned char TempCount;
    unsigned char AlarmRecordFlag;
    unsigned char AlarmFlagBak;
};

#define cConSensorError 0
#define cFrozeSensorError 1
#define cHighTempError 2
#define cLowTempError 3
#define cExtInputError 4
#define cMotorStatus 5
#define cFanStatus 6
#define cFrozenStatus 7
#define cRemoteFrozen 8
#define cRemoteOpen 9
#define cSysDisFrozen 10
#define cComError 11

#define cContorlRegLen 54
#define cVarLenByte 84
#define cPowerMetre 60
#define cDoorPos 64
#define cAlarmPos 65
#define cTempPos 66

#define TouchParaPannel 52 // touch pannel 读取配置数据长度，用于识别，如pannnel读取的para数据
#define TouchBitPannel 10

// 项目类型定义
#define LoraGateway

#ifdef LoraGateway
#define _BL02D
#define _Loramain
#define _LoraModule
#define __RestRfNode // use test RF signal
#define __NotuseLcd
#endif

//--------温湿度的无效值
#ifdef _BL02D
#ifdef _Loramain
#define cInValidTemp 3000
#define cInValidTempNouse 3000
#else
#define cInValidTemp 3000
#endif
#endif

// Version
#ifdef _BL02D
#ifdef _Loramain
#define cMainVersionRec 1
#define cSubVersionRec 1
#define cModifiedVersionRec 26 // OTA版本更新
#else
#define cMainVersionRec 10
#define cSubVersionRec 1
#define cModifiedVersionRec 0
#endif
#endif

// 关键寄存器地址定义
#define cMainVersion 10
#define cSubVersion 11
#define cModifiedVersion 12
#define cVer1 13
#define cVer2 14
#define cVer3 15

#define cRealDataAct 0
#define cTemp 1
#define cHumi 2
#define cRealDataAct1 3
#define cTemp1 4

#define cTempStartAddr 16
#define cNodePower 32

#define cTempOffset 78
#define cInterTOffet 79
#define cInterHROffet 80
#define cDoorVar 81

#define cLoraNodeAliveSet 90
#define cLoraNodeOffset 100

// Lora相关定义
#ifdef _Loramain
#define AddrStart 1
#define AddrLen 16
#endif

// 全局变量声明
extern unsigned char VarList[320];
extern unsigned char VarList1[320];
extern unsigned char VarListBak[320];
extern unsigned char ParaList[1024];
extern unsigned char DeviceNum;
extern unsigned char DisplayTurn;

// 外部函数声明
extern void SendByteAscii(unsigned char i);
extern void SendDataToBus(unsigned char *pp);
extern void SendDataToBus1(unsigned char *pp, unsigned char len);
extern void SendDataToBus_uart1(unsigned char *pp, unsigned char len);
extern void ProgramEepromByte(void);
extern void ReadEepromByte(void);
extern unsigned char GetInitialFlag(void);
extern void InitialPara(void);
extern void DealWithHisRead(unsigned char *pp);
extern uint16_t CRC(unsigned char *puchMsg, unsigned char usDataLen);
extern void WatchdogReset(void);

// LED相关函数
extern void led_on(void);
extern void led_off(void);
extern void led_toggle(void);
extern void led_blink(int times, int on_ms, int off_ms);

struct Hisrory_Struct History;

#endif // GLOBAL_H