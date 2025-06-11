#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

unsigned char    SelectMenuBak;  //20240926
unsigned char      DisplayMode;  //add 2016-11-8 12:04
     
unsigned char GPSSEND[30];


unsigned char     TestFlag;
unsigned char     RecordNextTime;
unsigned char   AlarmFlag;  //add 2019-04-28 8:04

unsigned char      InitialStatus[2];
unsigned char     StartBak;
unsigned char     GetsensorWrong;
unsigned char     Interval;
unsigned char     ResetFlag;
unsigned char          CurrentReordNum ;
uint16_t        CurrentWritePage ;
unsigned char          CurrentReordNum_bak ;
uint16_t           CurrentWritePage_bak ;
unsigned char          Loop_bak;
unsigned char          Loop;
unsigned char          AlarmOutDepend;
unsigned char          AlarmOutDepend_bak;
unsigned char          AlarmDelay;
unsigned char         AlarmOuMode;
unsigned char         AlarmOuModeBak;
unsigned char     DeviceNum; 
unsigned char     SendTempBuffer[cSendLength];

//--------modbus para list----------
unsigned char     VarList[VarNum];
unsigned char     VarList1[8];//add 2016-5-21 13:44
unsigned char   ParaList[ParaNum];
unsigned char   ParaListBak[ParaNum];  //add 2016-01-15 11:10

unsigned char    VarListBak[2];
unsigned char    BellOnOffStatus	;//add 2019-03-29


unsigned char    AddrStart	;//add 2019-03-29
unsigned char    AddrLen	;//add 2019-03-29
unsigned char   DisplayTurn;
uint16_t   WirelessNodeDelay[16];
uint16_t   NodeInterval;
unsigned char   WatchDogTestFlag;  //用来测试watchdog 是否有效
uint8_t    Alarm_Delay_02D_Count[4];
uint8_t    Alarm_Delay_02D_OverFlag[4];

uint8_t    LoraNodeConfigPin;  //管脚状态
uint8_t    LoraNodeWork;       //切换到工作模式,需要先配置
uint8_t    LoraNodeConfig;
uint8_t   LoraNodeStatus;

uint8_t   BakAlarm;  //保存门磁状态的上一状态


uint8_t  GetRfDataFlag;
uint8_t          GetRfnode;	
uint8_t          Couple[10];	

uint8_t  UptateFlag;
 
uint8_t uart2ReceiveMode;
uint8_t RequireTime[8];

uint8_t GetCCidFlag;//20231015