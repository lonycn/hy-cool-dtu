#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include "include/global.h"

// OTA升级模块头文件
#include "ota/include/ota_manager.h"
#include "ota/include/ota_config.h"
typedef unsigned char Byte;  // single byte   0xff
typedef uint16_t Word;       // two byte 		0xffff
typedef unsigned long Dword; // four byte		0xffffffff
typedef uint16_t uint;
typedef unsigned char uchar;
typedef unsigned char u8;
typedef uint16_t u16;
typedef signed long u32;
unsigned char BaudBak;

extern uint8_t uart0_Var_List[320];

void ChangeBaudrate(unsigned char mode)
{
  if (mode == 1)
  {
    if (BaudBak != ParaList[3])
    {
      if (ParaList[3] == 0x01)
      {
        UART_Close(UART0);
        UART_Open(UART0, 2400);
        BaudBak = ParaList[3];
      }
      else if (ParaList[3] == 0x02)
      {
        UART_Close(UART0);
        UART_Open(UART0, 4800);
        BaudBak = ParaList[3];
      }

      else if (ParaList[3] == 0x03)
      {
        UART_Close(UART0);
        UART_Open(UART0, 9600);
        BaudBak = ParaList[3];
      }
      else

      {
        UART_Close(UART0);
        UART_Open(UART0, 19200);
        BaudBak = ParaList[3];
      }
    }
  }
  else // mode=0,������ʼ��
  {
    if (ParaList[3] == 0x01)
    {
      UART_Open(UART0, 2400);
      BaudBak = ParaList[3];
    }
    else if (ParaList[3] == 0x02)
    {
      UART_Open(UART0, 4800);
      BaudBak = ParaList[3];
    }

    else if (ParaList[3] == 0x03)
    {
      UART_Open(UART0, 9600);
      BaudBak = ParaList[3];
    }
    else

    {
      UART_Open(UART0, 19200);
      BaudBak = ParaList[3];
    }
  }
}

void DisplayWelcome()
{
#if 0		
			unsigned char  No[3];
          LCD4_Clear();
          LCD_string(1,4,"Welcome");
          LCD_string(2,3,"Ver  .  .");
          No[0]=cMainVersionRec/10+0x30;
          No[1]=cMainVersionRec%10+0x30;
          No[2]=0;
          LCD_string(2,6,No);
          No[0]=cSubVersionRec/10+0x30;
          No[1]=cSubVersionRec%10+0x30;
          No[2]=0;
          LCD_string(2,9,No);
          No[0]=cModifiedVersionRec/10+0x30;
          No[1]=cModifiedVersionRec%10+0x30;
          No[2]=0;
          LCD_string(2,12,No);
#endif
}

void InitialVersion()
{
  VarList[cMainVersion * 2] = 0x00;
  VarList[cMainVersion * 2 + 1] = cMainVersionRec;
  VarList[cSubVersion * 2] = 0x00;
  VarList[cSubVersion * 2 + 1] = cSubVersionRec;
  VarList[cModifiedVersion * 2] = 0x00;
  VarList[cModifiedVersion * 2 + 1] = cModifiedVersionRec;

  ParaList[cVer1 * 2] = 0x00;
  ParaList[cVer1 * 2 + 1] = cMainVersionRec;
  ParaList[cVer2 * 2] = 0x00;
  ParaList[cVer2 * 2 + 1] = cSubVersionRec;
  ParaList[cVer3 * 2] = 0x00;
  ParaList[cVer3 * 2 + 1] = cModifiedVersionRec;
  ParaList[cVer1 * 2 + 6] = 0x00;
  ParaList[cVer1 * 2 + 1 + 6] = 1;
  ParaList[cVer2 * 2 + 6] = 0x00;
  ParaList[cVer2 * 2 + 1 + 6] = 0;
  ParaList[cVer3 * 2 + 6] = 0x00;
  ParaList[cVer3 * 2 + 1 + 6] = 0;
}

void InitialTempHumi_02D()
{
  uint16_t temp1;
  temp1 = (uint)(-cInValidTemp);

  VarList[0] = temp1 / 256; // 11
  VarList[1] = temp1 % 256;
  VarList[2] = temp1 / 256; // 12
  VarList[3] = temp1 % 256;
}

void InitialTempHumi_03D()
{
  uint16_t temp1;
  temp1 = (uint)(-cInValidTemp);

  VarList[cRealDataAct * 2 + 0] = 0xff;
  VarList[cRealDataAct * 2 + 1] = 0xff;
  VarList[cRealDataAct * 2 + 2] = 0xff;
  VarList[cRealDataAct * 2 + 3] = 0xff;
  VarList[cTemp * 2] = temp1 / 256; // 11
  VarList[cTemp * 2 + 1] = temp1 % 256;
  VarList[cHumi * 2] = temp1 / 256; // 12
  VarList[cHumi * 2 + 1] = temp1 % 256;

  VarList1[cRealDataAct1 * 2 + 0] = 0xff;
  VarList1[cRealDataAct1 * 2 + 1] = 0xff;
  VarList1[cRealDataAct1 * 2 + 2] = 0xff;
  VarList1[cRealDataAct1 * 2 + 3] = 0xff;
  VarList1[cTemp1 * 2] = temp1 / 256;
  VarList1[cTemp1 * 2 + 1] = temp1 % 256;
  VarList1[cTemp1 * 2 + 2] = temp1 / 256;
  VarList1[cTemp1 * 2 + 3] = temp1 % 256;

  VarListBak[0] = VarList[cRealDataAct * 2];     // add  2021-05-26 12:08
  VarListBak[1] = VarList[cRealDataAct * 2 + 1]; // add   2021-05-26 12:08
}

// #define  _TestVar
void InitialTempHumi_Lora()
{
  uint16_t temp1;
  uint8_t i;

  temp1 = (uint)(-3000);

  for (i = 0; i < 16; i++)
  {
#ifdef _TestVar
    temp1 = i * 50;
#endif
    VarList[cTempStartAddr * 2 + i * 4] = temp1 / 256; // 13
    VarList[cTempStartAddr * 2 + i * 4 + 1] = temp1 % 256;
    VarList[cTempStartAddr * 2 + i * 4 + 2] = temp1 / 256; // 13
    VarList[cTempStartAddr * 2 + i * 4 + 3] = temp1 % 256;
    // add 20211212
    temp1 = 65535;
    VarList[cNodePower * 2 + i * 4] = temp1 / 256; // 13
    VarList[cNodePower * 2 + i * 4 + 1] = temp1 % 256;
    VarList[cNodePower * 2 + i * 4 + 2] = temp1 / 256; // 13
    VarList[cNodePower * 2 + i * 4 + 3] = temp1 % 256;
  }

  DisplayTurn = AddrStart;
}
void InitialVar()
{
  uint16_t temp1;
  uchar i;
#ifdef _BL02D
  InitialTempHumi_02D();
#endif
#ifdef _BL03D
  InitialTempHumi_03D();
#endif
  InitialTempHumi_Lora();
  InitialVersion();
  WatchDogTestFlag = 0;
}

void nodePara()
{

  ReadEepromByte();

  if ((DeviceNum == 0x00) || (DeviceNum == 0xff))
  {
    ResetFlag = 1;
    WatchdogReset();
    InitialPara();
  }
}

void BellBell(uchar Ontime, uchar OffTime, uchar Times)
{
  uchar i;
  for (i = 0; i < Times; i++)
  {
    BellOn();
    DelaySecond_1(Ontime);
    BellOff();
    DelaySecond_1(OffTime);
  }
}

/*--------------------------------------------------------------------------------------------
//Function:void Main_loop_Initial()
//Input:
//Output:   None
//Description:������ĳ�ʼ������,�����Ҫ���ӹ���ģ��,��Ҫ���������ӳ�ʼ��ģ��
//--------------------------------------------------------------------------------------------*/
void Main_loop_Initial()
{
  static unsigned char j = 0, Temp[7];

  WatchdogReset();
  ReadEepromByte();
  if ((DeviceNum == 0x00) || (DeviceNum == 0xff))
  {
    ResetFlag = 1;
    WatchdogReset();
    InitialPara();
    Bellcontrol(2, 1, 1);
  }

  ChangeBaudrate(0); // ��һ��       ;
  WatchdogReset();
  WatchdogReset();
  DelaySecond(1);
  BellBell(10, 10, 2);
  BellBell(20, 20, 2);

  WatchdogReset();
  InitialVar();
  Alarm_Initial();

  WatchdogReset();

  SetDelay1s(2);
  watchdog_Init();
  Adc_Initial();

  Key_PowerIn_Initial();
  //   DI1_initial();  //2021-06-04 20:59

  LEDInital();
  LEDOff();
  LEDcontrol(255, 20, 200);

  uart0_ModbusLoopInitla();
  magicInitial();
  AlarmerInitial();
  Key_Alarm_inital();
  KeyDoor_Initial();
  UART1_Init();

  UART0_Init();
  UART2_Init();
  UART3_Init();
  UART4_Init();
  for (j = 0; j < 5; j++)
    SetCoolCompensateTimer(j, (ParaList[cCoolCompensate * 2] * 256 + ParaList[cCoolCompensate * 2 + 1]));
  // 20231015   SendGetNoComand();
  // 20231015   SendGetccid();

  // 🔧 OTA模块初始化
  ota_config_t ota_config = {
      .server_url = "https://ota.coolstorage.com",
      .device_id = "NANO_DTU_001",
      .transport_type = OTA_TRANSPORT_HTTP,
      .timeout_ms = 30000,
      .retry_count = 3,
      .chunk_size = 1024,
      .enable_signature_verify = true,
      .checksum_type = OTA_CHECKSUM_SHA256,
      .progress_callback = NULL,
      .state_callback = NULL};

  if (ota_init(&ota_config) == 0)
  {
    // OTA初始化成功，LED指示
    LEDcontrol(50, 50, 1); // 短闪烁表示OTA就绪
  }
}

void PowerDownFunction(void)
{
  /* To check if all the debug messages are finished */
  // UART_WAIT_TX_EMPTY(UART1);

  SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
  CLK->PWRCTL |= (CLK_PWRCTL_PD_EN_Msk | CLK_PWRCTL_WK_DLY_Msk);

  __WFI();
}

uint16_t tempVar;

/* ------------output debug-------------------------------*/

void OutputChange()
{
  uint8_t StringVar[30], i;
  // static  TempVar;

#if 0
   
   for(i=0;i<30;i++)
      StringVar[i]=0;
   if((VarList1[cRealDataAct1*2+1]&0x20)==0x20)
     { sprintf(StringVar,"Status:%d \r\n",VarList1[cRealDataAct1*2+1]);
        // SendDataToBus1_uart0(StringVar,strlen(StringVar)	); 
     } 
   if( tempVar!=VarList[22]*256+VarList[23])
     { if((VarList[23]&0x20)==0x20)
     	  {tempVar=VarList[22]*256+VarList[23];
     	   sprintf(StringVar,"Status:%d \r\n",tempVar);
       //  SendDataToBus1_uart0(StringVar,strlen(StringVar)	); 
     } 	   
   }
#endif
}
#if 0	     	      	 
void  SetWakeUpFunciton()
  {
    SetDelay1s(ParaList[cLoraNodeInteral*2]*256+ParaList[cLoraNodeInteral*2+1]) ;	
  }	    	 
/*--------------------------------------------------------------------------------------------
//Function: void Main_loop_while()
//Input:  
//Output:   None 
//Description:���������ѭ������,�����Ҫ����һ������ģ��,��Ҫ���Ӹ�ģ���״̬������ģ��
//--------------------------------------------------------------------------------------------*/
void GetAndSend()
 {
    SwtichMode(cWorkMode);
    LoopForSth21();
    SendTempHumi();
    SwtichMode(cSleepMode);	
  }

void TestLoraNode()
 {
    SetDelay1s(3) ;
while(1)
   {   
   if(PB8==0)
    { if(GetDelay1sStatus())
       	 {   SetDelay1s(3) ;
             GetAndSend();
         }    
  //    DealUartBuff_uart0();
      DealLoraData();   //����lora�˿ڵ�����
   }
  else
   { //if(GetDelay1sflag()==0)
      //     SetWakeUpFunciton();
     if(GetDelay1sStatus())
       	 {   SetWakeUpFunciton();
             GetAndSend();
		PowerDownFunction();
         }    
    
   }	     	
 	
}
}
#endif
#define _Normal // test si24r1 �Ƿ�ֻ����
// 20240926

extern uint8_t uart0_OrigPara_List[cContorlRegLen * 20];

// 🔧 OTA Modbus控制寄存器定义
#define OTA_REG_STATE 200       // OTA状态寄存器
#define OTA_REG_PROGRESS 201    // 升级进度寄存器
#define OTA_REG_CONTROL 202     // OTA控制寄存器
#define OTA_REG_VERSION_NEW 203 // 新版本号寄存器
#define OTA_REG_ERROR_CODE 204  // 错误代码寄存器

/**
 * @brief 处理OTA相关的Modbus寄存器读写
 * @param reg_addr 寄存器地址
 * @param value 写入值(读操作时忽略)
 * @param is_write true=写操作, false=读操作
 * @return 寄存器值(读操作) 或 操作结果(写操作)
 */
uint16_t ota_modbus_handler(uint16_t reg_addr, uint16_t value, bool is_write)
{
  static ota_info_t update_info = {0};

  switch (reg_addr)
  {
  case OTA_REG_STATE:
    if (!is_write)
    {
      return (uint16_t)ota_get_state();
    }
    break;

  case OTA_REG_PROGRESS:
    if (!is_write)
    {
      uint32_t downloaded, total;
      if (ota_get_progress(&downloaded, &total) == 0 && total > 0)
      {
        return (uint16_t)(downloaded * 100 / total);
      }
      return 0;
    }
    break;

  case OTA_REG_CONTROL:
    if (is_write)
    {
      switch (value)
      {
      case 1: // 检查更新
        ota_check_update(&update_info);
        LEDcontrol(100, 100, 2); // 双闪表示检查更新
        break;
      case 2: // 开始升级
        if (update_info.size > 0)
        {
          ota_start_download(&update_info, NULL, NULL);
          LEDcontrol(200, 50, 3); // 三闪表示开始升级
        }
        break;
      case 3: // 取消升级
        ota_cancel_update();
        LEDcontrol(50, 200, 1); // 长亮表示取消
        break;
      case 4: // 回滚
        ota_rollback();
        break;
      }
      return 0; // 写操作成功
    }
    break;

  case OTA_REG_ERROR_CODE:
    if (!is_write)
    {
      // 返回最后的错误代码 (这里简化为固定值)
      return 0;
    }
    break;
  }

  return 0xFFFF; // 无效操作
}
#define cActTempOrig 47
#define cActErrorOrig 12
void SelectMotorTemp()
{
  unsigned char SelType, SendDataToBus[8];
  unsigned int setTemp, setError, ActTemp, ActError;
  SelType = ParaList[(cSelectMenu - 1) * 2 + 1];

  if (SelType != 0)
  {
    setTemp = ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 1] + ParaList[cSelectMenu * 2 + (SelType - 1) * 4] * 256;
    setError = ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 3] + ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 2] * 256;

    ActTemp = uart0_OrigPara_List[cActTempOrig * 2 + 1] + uart0_OrigPara_List[cActTempOrig * 2] * 256;
    ActError = uart0_OrigPara_List[cActErrorOrig * 2 + 1] + uart0_OrigPara_List[cActErrorOrig * 2] * 256;

    if ((setTemp != ActTemp) || (setError != ActError))
    {
      SendDataToBus[0] = 0X01;
      SendDataToBus[1] = 0X06;
      SendDataToBus[2] = 0X00;
      SendDataToBus[3] = 47;
      SendDataToBus[4] = ParaList[cSelectMenu * 2 + (SelType - 1) * 4];
      SendDataToBus[5] = ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 1];
      uart2_AddLocalBuffer_16(0x06, SendDataToBus);

      SendDataToBus[0] = 0X01;
      SendDataToBus[1] = 0X06;
      SendDataToBus[2] = 0X00;
      SendDataToBus[3] = 12;
      SendDataToBus[4] = ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 2];
      SendDataToBus[5] = ParaList[cSelectMenu * 2 + (SelType - 1) * 4 + 3];
      uart2_AddLocalBuffer_16(0x06, SendDataToBus);
    }
  }
}
//----------------------Main_loop_while()--------------------------------------
// Function:  Main_loop_while()
// Input:
// Output:
// Description:
//--------------------------------------------------------------
uint16_t m = 0, idcount = 0, gpsconut = 0;
void Main_loop_while()
{
  static unsigned char j = 0, Temp[7], i, SendCount = 0;

  CountWatchdog();
  StatusMachine_BellLED();
#ifndef UseSim
  Key_PowerIn_StateMachine();
#endif
  StatusMachine_LEDLED();
  //  StatusMachine_AlarmLEDLED();
  //    DealHistoryData();
  //    DealBacklight();
  Adc_StateMachine();
  if (ParaList[cUrat0TranslateUart3 * 2 + 1] != 78)
  {
    DealUartBuff_uart3();
    // DealUartBuff_uart0();
    //    DealLoraData();   //����lora�˿ڵ���?
    // DealUartBuff_uart3(); //slave
    // Uart3_ModbusLoop();	   //master
    uart0_ModbusLoop(); // master
  }
  else
    Urat0TranslateUart3();

  DealUartBuff_uart2();
  OutputChange();

  //  StateMachine_Switch();  //add 2021-06-28 10:29
  DealWithDoor();        // ADD 2022/8/13 21:17
  GetVolCurrentStatus(); // add 2022 8 23
  DealDoorTempAlarm();   // ADD 2022/8/13 21:17

  // 🎯 OTA升级处理 (每10秒检查一次)
  static uint16_t ota_check_counter = 0;
  if (++ota_check_counter >= 5)
  { // 5*2秒 = 10秒
    ota_check_counter = 0;
    ota_process(); // 处理OTA状态和任务
  }

  // uart2_SendPowerSignal();  //add 2021/12/23 18:55

  UpdateChange();
  if (GetDelay1sStatus())
  {
    j++;
    idcount++;
    gpsconut++;
    if (gpsconut >= ((ParaList[cGpsInteral * 2 + 1] + ParaList[cGpsInteral * 2] * 256) / 2))
    {
      if (ParaList[cUseGps * 2 + 1] == 1)
        SendGetGpsComand();
      gpsconut = 0;
    }
    SetDelay1s(2);
    //	SendDataToBus_uart2("12345",5);
    GetCommunicationStatus();
    GetTemp(101);

    m++;
    if (SendCount > 5)
    {
      if (m >= (ParaList[cLoraNodeInteral * 2 + 1] + ParaList[cLoraNodeInteral * 2] * 256) / 2)
      {
        m = 0;
        uart3_SendCoolDat();
        //  CreatJsonPack(27);
      }
    }
    else
    {
      if (m >= 30)
      {
        m = 0;
        uart3_SendCoolDat();
        //		 CreatJsonPack(27);
        SendCount++;
      }
    }

    //		LoopForSth21();
    //     LoopForPCF8563(Temp);
    if ((j % 5) == 0)
    {
      j = 0;
      DealWithSwtich();
      //	SelectMotorTemp(); //20240926
    }
    if (idcount >= 3)
    {
      idcount = 0;
      if (GetCCidFlag == 0)
      {
        SendGetNoComand();
        SendGetccid();
      }
    }
  }
}

//-----------------------------------------
// main
//-----------------------------------------
void main_loop(void)
{
  Main_loop_Initial();
  GetCoupleList();

  SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval * 2 + 1]);
  // Main_loop_Test();
  SetDelay1s(2);
  PD7 = 0;
  while (1)

    Main_loop_while();

} // end of main