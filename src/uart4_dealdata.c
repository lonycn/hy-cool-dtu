#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "Nano100Series.h"

#include "sys.h"
#include "rtc.h"
#define _GLOBAL_H
#include	"global.h"

uint8_t     Sequel;  //20231114   

//modbus 通讯协议控制字
#define cComandReadCoil               0x01
#define cComandReadDiStatus           0x02
#define cComandReadHoldRegister       0x03
#define cComandReadDiRegister         0x04

#define cComandWriteSingleCoil 0x05
#define cComandWriteMoreCoil   0x0f

#define cComandWriteSingleUint 0x06
#define cComandWriteMoreUint   0x10


//CONSIST
#define cRightStatus                 0x01
#define cCrcWrong                    0x02
#define cModbusOverTime              0x05 
//---------------------------

#define       cBufferLen_uart0     500
extern uint16_t    WritePiont_uart0;
extern uint16_t    ReadPiont_uart0;
extern unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];
extern unsigned char   SendTempBufferBak_uart0[300];
extern  uint8_t   uart0_LoopDeviceNo;
extern  uint8_t   uart0_loop_stage;


#define       cBufferLen_uart3     200
extern uint16_t    WritePiont_uart3;
extern uint16_t    ReadPiont_uart3;
extern unsigned char   ReceiveBuff_uart3[cBufferLen_uart3];




extern  uint8_t	  uart0_CloudCount;
extern  uint8_t	  uart0_CloudComandLen; //中间变量  用来计算最短的包长
extern  uint8_t	  uart0_CloudComandNo;
extern  uint8_t	  uart0_CloudComandFunction;
extern  uint8_t   uart0_CloudComandResondWaiting;
extern  uint8_t   uart0_CloudComandResondLen;
extern  uint8_t   uart0_Sendcomand[5][20];
extern  uint8_t   uart0_Cloudcomand_RP;
extern  uint8_t   uart0_Cloudcomand_WP;

extern  uint8_t   uart0_LocalSendcomand[5][20];
extern  uint8_t	  uart0_LocalCount;
extern  uint8_t	  uart0_LocalComandLen;
extern  uint8_t	  uart0_LocalComandNo;
extern  uint8_t	  uart0_LocalComandFunction;//2022/2/4 12:28
extern  uint8_t   uart0_LocalComandResondWaiting;
extern  uint8_t   uart0_LocalComandResondLen;
extern  uint8_t   uart0_LocalComandNo;
extern  uint8_t	  uart0_LocalComandAddr;
extern  uint8_t   uart0_Localcomand_RP;
extern  uint8_t   uart0_Localcomand_WP;
 
extern  uint8_t uart0_Bit_List[10];
extern  uint8_t uart0_Bit_List_bak[20];
extern  uint8_t uart0_Para_List[280];
extern  uint8_t Bak_uart0_Bit_List[10];
extern  uint8_t Bak_uart0_Para_List[280];

extern  uint8_t uart0_Var_List[320];
extern  uint8_t uart0_VolCurrentAlarm[10];

extern  uint8_t uart0_OrigPara_List[cContorlRegLen*20];
extern  uint8_t uart0_OrigBitList[70];

extern  uint8_t DeviceInterval[20];
extern  uint8_t  uart0_Para_MapTable[14];
extern  uint8_t  uart0_Var_MapTable[4];
extern  uint8_t  uart0_Bit_MapTable[7];
extern  uint8_t uart0_floatSeq[4];
uint8_t Bak_uart0_var_List[66];
uint8_t Random;
uint8_t Random1;
/*-----------------------------------------------------
通讯及协议解析
--------------------------------------------------------*/
uint8_t uart0_GetBufferDataRead(uint8_t i);
void uart0_IncUartRead(uint8_t i);
void uart0_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart0_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart0_LoraMaster_DealWithCoil(uint8_t Device);
void uart0_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);
void  uart0_LoraMaster_DealWithCoilWreite(uint8_t Device);
void   Timer_uartInterval();
void  GetCommunicationStatus();
/*-----------------------------------------------------
轮询指令
--------------------------------------------------------*/
void uart0_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len);
void uart0_ModbusLoopInitla();
void  uart0_NextDevice(); 
void  uart0_ModbusLoop();

/*-----------------------------------------------------
云端通讯指令插入
--------------------------------------------------------*/
void UpdateChange();
void uart0_SendCloudComand();
void uart3_SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat);
void uart3_SendCoolDat();
void uart3_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len);
void uart0_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send);  
/*-----------------------------------------------------
本地通讯指令插入
--------------------------------------------------------*/
void uart0_SendLocalComand() ;
void uart0_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send);     
void uart0_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send) ;  
void uart2_AddLocalBuffer_16(uint8_t FunCode,uint8_t *send);     
/*-----------------------------------------------------
状态获取
--------------------------------------------------------*/
uint8_t  GetBitStatus(uint8_t  bit);
void  SetStatus(uint8_t Device,uint8_t  bit);
void ClrStatus(uint8_t Device,uint8_t  bit);
uint8_t  GetConSensorStatus(uint8_t Device,uint8_t  bit);
uint8_t retrnPara(uint16_t i);
/*-----------------------------------------------------
故障检测
--------------------------------------------------------*/
uint8_t GetAllSensorError(uint8_t Device);
uint8_t GetAllDeviceError(uint8_t Device);	
uint8_t GetAllError(uint8_t Device);
void uart0_VolCurrentOverLoadAlarm();
/*-----------------------------------------------------
启停操作
--------------------------------------------------------*/
void uart0_OpenOrClose(uint8_t  no,uint8_t  Op);
/*---------------------------------------------------------------------
使用说明：
1：初始化程序中调用uart0_ModbusLoopInitla()
2：主程序中调用uart0_ModbusLoop()
功能：在uart程序轮询温控器和电表，一个温控制配一个电表，支持10个温控器和10个电表
温控制器编号从1-10，电表编号从101-110
起始编号和温控器数量在寄存器中进行设置，默认为从1开始，共10组，此参数不需要进行调整
使用使用该温控器和对应的电表，通过激活控制器来设置
每个温控器有8个状态位，有2个32位的温度变量，参数有4个16位参数和5个32位参数

在轮训温控器的过程中，如果有云端的控制数据下发，则在完成一个查询命令后，执行云端的控制命令，返回云端的回复数据
每个温控器使用6个16bit变量，组成3个32位浮点数变量，每个电表使用7个16bit变量，分别为3相电流和3相电压及总功率
每个电箱配套NTC温控器2只，变量地址紧跟电表数据之后
----------------------------------------------------------------------*/
void InitialTemp()
{
  uint8_t i;
    int16_t  temp;
 		 uint16_t  temp1; 
    temp=-cInValidTemp;
   	     temp1=(uint16_t)temp;
     for(i=0;i<16;i++)
      {
      	
     	uart0_Var_List[(cTempPos+i)*2]=	temp1/256;
		  uart0_Var_List[(cTempPos+i)*2+1]=	temp1%256;
		 }  
  
}


 

void GetVolCurrentStatus()
{ uart0_Var_List[cDoorPos*2+1]&=0xf3;
	uart0_Var_List[cDoorPos*2+1]|=((uart0_VolCurrentAlarm[0]&0x03)<<2);
	
}
 
//for door magic
void  ClearDoorAlarmFlag()
{
	 uart0_Var_List[cDoorPos*2+1]&=0xfd;  
}

void  SetDoorAlarmFlag()
{
  uart0_Var_List[cDoorPos*2+1]|=0x02;	
	
}	


void  ClearDoorStatusFlag()
{
	 uart0_Var_List[cDoorPos*2+1]&=0xfe; 
}

void  SetDoorStatusFlag()
{
  uart0_Var_List[cDoorPos*2+1]|=0x01;
	
}


void   SetTempALarm(uint8_t  i)
{
  uart0_Var_List[cAlarmPos*2+1]|=(0x01<<i);	
}
   	 
void   ClearTempALarm(uint8_t  i)   	 
   	{  uart0_Var_List[cAlarmPos*2+1]&=((0x01<<i)^0xff);
   }

uint16_t GetAlarmStatus()
 {
 	return(uart0_Var_List[cAlarmPos*2+1]+uart0_Var_List[cAlarmPos*2]*256);
}


uint16_t  GetTempValue(uint8_t  i)
  {
		return(uart0_Var_List[(cTempPos+2*i)*2]*256+uart0_Var_List[(cTempPos+2*i)*2+1]);
	}

uint16_t  GetHumiValue(uint8_t  i)
  {
		return(uart0_Var_List[(cTempPos+2*i+1)*2]*256+uart0_Var_List[(cTempPos+2*i+1)*2+1]);
	}	
	
//end of 2022/8/13 18:45


void UpdateChange()
{uint16_t  i;
	int8_t SendBuf[140];
  uint8_t j,UptateFlag1=0;
  uint16_t CRCTemp;
#if  1
	if((uart0_Var_List[64*2+1]&0x04)==0)
	  uart0_Var_List[102*2+1]=0;
	else
		uart0_Var_List[102*2+1]=1;
	uart0_Var_List[102*2]=0;
	
	if((uart0_Var_List[64*2+1]&0x08)==0)
	  uart0_Var_List[103*2+1]=0;
	else
		uart0_Var_List[103*2+1]=1;
	uart0_Var_List[103*2]=0;
	
	for(i=0;i<26;i++)
	 {if(Bak_uart0_var_List[i]!=uart0_Var_List[i+102*2])
	   {  UptateFlag1=1;
	      break;
		 }
	 } 
	if(UptateFlag1==1) 
	{
    for(i=0;i<26;i++)
	      Bak_uart0_var_List[i]=uart0_Var_List[i+102*2]; 
	  for(i=0;i<56;i++)
	    SendBuf[i]=uart0_Var_List[i+204];
    uart3_SendControlData(1,4,0x103,28,SendBuf) ;
	}
#endif	 
	
 if(UptateFlag==1)
 {	 UptateFlag=0;
if(ReturnOpenDelay()>100)
{
 
	for(i=0;i<10;i++)	
	  { if(uart0_Bit_List[i]!=Bak_uart0_Bit_List[i])
			  break;
		}
   if(i<10)
	 {
      
		 for(i=0;i<10;i++)
        Bak_uart0_Bit_List[i]=uart0_Bit_List[i];
		for(i=0;i<10;i++)
     SendBuf[i]=uart0_Bit_List[i];
   uart3_SendControlData(1,2,1,80,SendBuf) ;
	 }	 
 //-----------------------
      for(i=0;i<140;i++)
		  if(Bak_uart0_Para_List[i]!=uart0_Para_List[i])
				  break;
			if(i<140)
			{
				
				 for(i=0;i<140;i++)
		    	  Bak_uart0_Para_List[i]=uart0_Para_List[i];
					for(i=0;i<140;i++)
             SendBuf[i]=uart0_Para_List[i];
        uart3_SendControlData(1,3,1,70,SendBuf) ;
				}		
					
			
//-----------------------------------------------			
		for(i=0;i<140;i++)
		  if(Bak_uart0_Para_List[i+140]!=uart0_Para_List[i+140])
				  break;
			if(i<140)
			{
				
				 for(i=0;i<140;i++)
		   
					Bak_uart0_Para_List[i+140]=uart0_Para_List[i+140];
					for(i=0;i<140;i++)
            SendBuf[i]=uart0_Para_List[i+140];
          uart3_SendControlData(1,3,0x71,70,SendBuf) ; 
					
					
				}
//------------------------------------------------------------
			}
	
}
}


#define  R25   10000.0
#define  ValueB  3950.0   //3470.0
#define  Rpull  10000.0
float Rntc,Tntc,t1,t2,t3,t4,t5,t6,t7;
	
float  GetNTCTemp(uint16_t AdcV)
 { 
 	 Rntc=(4096-AdcV)*Rpull/AdcV;
 //	Rntc=AdcV*Rpull/(4096-AdcV);
 // Tntc=298.15*(log(R25)-log(Rntc))/(log(R25)-log(Rntc)-ValueB*298.15)-273.15;
   t1=1.0/298.15;
   t5=Rntc/R25;
	 t2=log(t5);
	 
	t7=t2/ValueB;
	 t3=t1+t7;
        t4=1.0/t3;
   Tntc=t4-273.15;	
	 return(Tntc);
}







#define  cSensorTye_OnlyRs485     0
#define  cSensorTye_OnlyNtc      1
#define  cSensorTye_Rs485_ntc      2
#define  cSensorTye_Rs485_Qitiao      3
#define  cSensorTye_ntc_Qitiao      4
#define  cSensorTye_OnlyQitiao      5


/*-----------------------------------------------------------
//Function:  void   GetTemp(uint8_t dev)
//Input:   uint8_t dev:ntc number
//Output:   None
//Description: 获取ntc温度 类型为  cSensorTye_OnlyNtc或者cSensorTye_ntc_Qitiao?
#define  cSensorTye_OnlyRs485     0
#define  cSensorTye_OnlyNtc      1
#define  cSensorTye_Rs485_ntc      2
#define  cSensorTye_Rs485_Qitiao      3
#define  cSensorTye_ntc_Qitiao      4
#define  cSensorTye_OnlyQitiao      5

//--------------------------------------------------------------*/
void   GetTemp(uint8_t dev)
    {  int16_t  tempInt; 
      uint16_t  tempuint; 
      float  		tempFloat;	
			uint8_t  i;
#if 0			
			
			
////-------------------------------------------------------------	
			if((ParaList[cTempType*2+1]==cSensorTye_OnlyNtc)||(ParaList[cTempType*2+1]==cSensorTye_ntc_Qitiao))	
		{ tempFloat=GetNTCTemp(GetAdc(1)); 
			tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+cTempPos*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+cTempPos*2+1]=tempuint%256;
			
			
			tempFloat=GetNTCTemp(GetAdc(2)); 
		  tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+2)*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+2)*2+1]=tempuint%256;
			
		  
			tempFloat=GetNTCTemp(GetAdc(3)); 
		  tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+4)*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+4)*2+1]=tempuint%256;	
		
			 tempInt=-cInValidTemp;
   	   tempuint=(uint16_t)tempInt;
		  for(i=0;i<3;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
		 } 
			
			
        //----------------------------------------------------	
		


    if(ParaList[cTempType*2+1]==cSensorTye_OnlyNtc)	
		{
			 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
			for(i=0;i<3;i++)
      {
      	
     	 
				uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
				
			
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;	
		 } 
			
			
		  for(i=ParaList[cTempNum*2+1];i<8;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2+0]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
				uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
				
			uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;	
		 } 

		 
		}
       //---------------------------------------------------------		
	else
			{
			 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
		  for(i=ParaList[cTempNum*2+1];i<5;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
				uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
				
			 uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;		
		 }  
      uart0_Var_List[(cTempPos+7*2)*2+2]=	tempuint/256;     //Last
		   uart0_Var_List[(cTempPos+7*2)*2+3]=	tempuint%256;	
    uart0_Var_List[(cTempPos+7*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+7*2)*2+57]=	tempuint%256;			 
		}
			
    }	
		
////-------------------------------------------------		
  else if(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)	
	{  tempFloat=GetNTCTemp(GetAdc(1)); 
			tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+cTempPos*2+10*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+cTempPos*2+1+10*2]=tempuint%256;
			
			
			tempFloat=GetNTCTemp(GetAdc(2)); 
		  tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+2)*2+10*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+2)*2+1+10*2]=tempuint%256;
			
		  
			tempFloat=GetNTCTemp(GetAdc(3)); 
		  tempInt =(int16_t)(tempFloat*10);
			tempuint=(uint16_t)tempInt;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+4)*2+10*2]=tempuint/256;
			uart0_Var_List[(dev-101)*cVarLenByte+(cTempPos+4)*2+1+10*2]=tempuint%256;
		  
		
		
		 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
		  for(i=ParaList[cTempNum*2+1];i<5;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
			 uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
			  uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;			
				
		 } 
  	
		  for(i=5;i<8;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
				uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;		
		 } 

		 
}
  
////--------------------------------------------------------------------------
else if(ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)	
	{  
		 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
		  for(i=ParaList[cTempNum*2+1];i<8;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
			 uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;
uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;							
		 }   
}
////--------------------------------------------------------------------------
else if(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao)	
	{  
		 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
		  for(i=ParaList[cTempNum*2+1];i<5;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
			  uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;	
				uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;			
		 } 
       uart0_Var_List[(cTempPos+7*2)*2+2]=	tempuint/256;     //Last
		   uart0_Var_List[(cTempPos+7*2)*2+3]=	tempuint%256;		
  uart0_Var_List[(cTempPos+7*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+7*2)*2+57]=	tempuint%256;					 
}
////--------------------------------------------------------------------------
else if(ParaList[cTempType*2+1]==cSensorTye_OnlyQitiao)	
	{  
		 tempInt=-cInValidTempNouse;
   	   tempuint=(uint16_t)tempInt;
		  for(i=0;i<5;i++)
      {
      	
     	 uart0_Var_List[(cTempPos+i*2)*2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+1]=	tempuint%256;
			 uart0_Var_List[(cTempPos+i*2)*2+2]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+3]=	tempuint%256;	
				uart0_Var_List[(cTempPos+i*2)*2+40]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+41]=	tempuint%256;	
				
			uart0_Var_List[(cTempPos+i*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+i*2)*2+57]=	tempuint%256;		
		 }  
      uart0_Var_List[(cTempPos+7*2)*2+2]=	tempuint/256;     //Last
		   uart0_Var_List[(cTempPos+7*2)*2+3]=	tempuint%256;	
uart0_Var_List[(cTempPos+7*2)*2+56]=	tempuint/256;
		   uart0_Var_List[(cTempPos+7*2)*2+57]=	tempuint%256;		 
}
#endif
	}

uint8_t retrnPara(uint16_t i)
{
   return(uart0_Para_List[i]);	
}	




//------------------------------------------------------------
//Function:  uart0_ModbusLoopInitla()
//Input:    None
//Output:   None
//Description: uart0初始化
//--------------------------------------------------------------
void uart0_ModbusLoopInitla()
{
	 SetSecond1s_ModbusLoop_Intial(2);
	 uart0_LoopDeviceNo=AddrStart;
}

void CheckSensorStatus()
 {uint8_t  i;
	 uint16_t Temp;
	  Temp=(uint16_t)(-cInValidTemp);
    for(i=0;i<ParaList[cTempNum*2+1];i++)
 {
	  if(RequireTime[i]>5)
		{
		    uart0_Var_List[i*4+cTempPos*2+2]=Temp/256;
        uart0_Var_List[i*4+cTempPos*2+3]=Temp%256; //temp
	      uart0_Var_List[i*4+cTempPos*2+0]=Temp/256;
        uart0_Var_List[i*4+cTempPos*2+1]=Temp%256; //humi 
		}
 }
}
//---------------------------------------------------------------
//Function:  void uart0_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len)
//Input:    None
//Output:   None
//Description: uart0轮询命令
//----------------------------------------------------------------
void uart0_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len)
  {
     uint8_t  SendBuf[10],j=0;
     uint16_t	CRCTemp;
		
	 ReadPiont_uart0=0;
         WritePiont_uart0=0;
  	SendBuf[j++]=Sta;
  	SendBuf[j++]=comand;
  	SendBuf[j++]=Addr/256;
  	SendBuf[j++]=Addr%256;
  	SendBuf[j++]=Len/256;
  	SendBuf[j++]=Len%256;
  	CRCTemp=CRC(SendBuf,j);
  	SendBuf[j++]=CRCTemp/256;
  	SendBuf[j++]=CRCTemp%256;
  	SendDataToBus1_uart0(SendBuf,j);
  }	

//------------------------------------------------------------
//Function:  void  uart0_NextDevice() 
//Input:    None
//Output:   None
//Description: 切换到下一个电箱
//--------------------------------------------------------------
void  uart0_NextDevice() 
{uint8_t Loopcount=0;
   while(1)
      {	
	 uart0_LoopDeviceNo++;
	 if(AddrLen>=10)
	   AddrLen=10;
	 if(uart0_LoopDeviceNo>=AddrStart+AddrLen)
	 { uart0_LoopDeviceNo=AddrStart;
		 CheckSensorStatus();		
	 }
      SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]); 
   //add 2022/2/3 11:13
     if(ParaList[(cLoraNodeAliveSet+uart0_LoopDeviceNo-1)*2+1]!=0)  //如果使用到该控制器
         break;	
   Loopcount++;		 
     if(Loopcount>=10)
			 break;
			}
 }

 
 
/*-----------------------------------------------------------
//Function:  void  uart0_ModbusLoop()
//Input:    None
//Output:   None
//Description: void  uart0_轮询电箱及处理返回数据
 #define  cSensorTye_OnlyRs485     0
#define  cSensorTye_OnlyNtc      1
#define  cSensorTye_Rs485_ntc      2
#define  cSensorTye_Rs485_Qitiao      3
#define  cSensorTye_ntc_Qitiao      4
#define  cSensorTye_OnlyQitiao      5		
//--------------------------------------------------------------*/
void  uart0_ModbusLoop()
{uint8_t  Dis[6],j=0;   
	if(ParaList[cUrat0TranslateUart3*2+1]!=78)	
	{	
	if(GetSecond1s_ModbusLoopStatus()==1)
	{
		SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]);
//-------------------云端的查询命令，最多查询3次，3次没有返回数据，不需要返回云端数据	
	 if(uart0_Localcomand_RP==uart0_Localcomand_WP)
	 {
		 
		 uart0_LocalCount=0;
		 
		 
	 }
  if(uart0_Cloudcomand_RP==uart0_Cloudcomand_WP)
	{
		uart0_CloudCount=0;
		
	}
	 

		if(uart0_Localcomand_RP!=uart0_Localcomand_WP)
	   { 	
	     uart0_LocalCount++;
	     if(uart0_LocalCount>=3)
	     {
	      uart0_Localcomand_RP++;
	      if(uart0_Localcomand_RP>=5)
	         uart0_Localcomand_RP=0; 
				uart0_LocalComandResondWaiting=0;
				uart0_LocalCount=0;
				
	     }
	    else
	     uart0_SendLocalComand();
	     
	  }		
		
	
		
//-------------------云端的查询命令，最多查询3次，3次没有返回数据，放弃该条命令		
	 else if(uart0_Cloudcomand_RP!=uart0_Cloudcomand_WP)
	   { 	
	     uart0_CloudCount++;
	     if(uart0_CloudCount>=3)
	     {
	       uart0_Cloudcomand_RP++; 
	       if(uart0_Cloudcomand_RP>=5)
	        uart0_Cloudcomand_RP=0; 
        uart0_CloudComandResondWaiting=0;
         uart0_CloudCount=0;				 
	     }
	    else
	     uart0_SendCloudComand();
	     
	  }
//------------------正常轮询设备	  
	else  	
	  {	 if(uart0_loop_stage>12)
		  {uart0_NextDevice();
		   uart0_loop_stage=0;
		  } 
	         else
	       uart0_loop_stage++;
				//	 uart0_loop_stage=4;
	        switch(uart0_loop_stage)
	     {
	     	case 0: 
              if((ParaList[(cLoraNodeAliveSet+uart0_LoopDeviceNo-1)*2+1]&0x01)==1) 					
		                       uart0_ModbusRequest(uart0_LoopDeviceNo,1,0,52);
	           else
							    SetModDelay();
	                 break;  
	     	case 1:    if((ParaList[(cLoraNodeAliveSet+uart0_LoopDeviceNo-1)*2+1]&0x01)==1) 
					
				                	uart0_ModbusRequest(uart0_LoopDeviceNo,3,0,50);
				            else
											  SetModDelay();  
										break;
	     	case 2: if((ParaList[(cLoraNodeAliveSet+uart0_LoopDeviceNo-1)*2+1]&0x02)==0x02) 
				          { 
											 uart0_ModbusRequest(uart0_LoopDeviceNo+100,4,0,31);
										
									}
				        else
									 SetModDelay();   
				        break;
				
				case  3: 	 if((ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao)||(ParaList[cTempType*2+1]==cSensorTye_ntc_Qitiao)||(ParaList[cTempType*2+1]==cSensorTye_OnlyQitiao))
								    uart0_ModbusRequest(200,4,0,5);	
								 else
									  SetModDelay();   
								 break;
				case  4:  if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{  if(ParaList[ cTempNum*2+1]>=1)
					   uart0_ModbusRequest(11,4,0,2);
           RequireTime[0]++;
           if(RequireTime[0]>200)
              RequireTime[0]=200; 						 
				}	
								   else 
									  SetModDelay();   
				        break;
	
									 
				case  5:  if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{  if(ParaList[ cTempNum*2+1]>=2) 
					uart0_ModbusRequest(12,4,0,2);	
					RequireTime[1]++;
           if(RequireTime[1]>200)
              RequireTime[1]=200; 
				}
								   else
									  SetModDelay();   
				        break;					 
				case  6:  if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{  if(ParaList[ cTempNum*2+1]>=3)
					uart0_ModbusRequest(13,4,0,2);
RequireTime[2]++;
           if(RequireTime[2]>200)
              RequireTime[2]=200; 					
				}					
								   else
									  SetModDelay();   
				        break;		
				case  7:   if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{  if(ParaList[ cTempNum*2+1]>=4)
					uart0_ModbusRequest(14,4,0,2);	
					RequireTime[3]++;
           if(RequireTime[3]>200)
              RequireTime[3]=200; 
				}					
								   else
									  SetModDelay();   
				        break;							 
	    case  8:   if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
			{  if(ParaList[ cTempNum*2+1]>=5) 
				uart0_ModbusRequest(15,4,0,2);
				RequireTime[4]++;
           if(RequireTime[4]>200)
              RequireTime[4]=200; 
			}				
								   else
									  SetModDelay();   
				        break;
				case  9:   if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{  if(ParaList[ cTempNum*2+1]>=6)
					uart0_ModbusRequest(16,4,0,2);
					RequireTime[5]++;
           if(RequireTime[5]>200)
              RequireTime[5]=200; 
				}					
								   else
									  SetModDelay();   
				        break;					 
				case  10:   if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{ if(ParaList[ cTempNum*2+1]>=7)  
					uart0_ModbusRequest(17,4,0,2);
          RequireTime[6]++;
           if(RequireTime[6]>200)
              RequireTime[6]=200; 					
        }					
								   else
									  SetModDelay();   
				        break;		
				case  11:   if((ParaList[cTempType*2+1]==cSensorTye_OnlyRs485)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_ntc)||(ParaList[cTempType*2+1]==cSensorTye_Rs485_Qitiao))
				{if(ParaList[ cTempNum*2+1]>=8)  
					uart0_ModbusRequest(18,4,0,2);
RequireTime[7]++;
           if(RequireTime[7]>200)
              RequireTime[7]=200; 					
				}					
								   else
									  SetModDelay();   
				        break;
	//add 20230225  互感式电表读取电量								 
   case 12: if((ParaList[(cLoraNodeAliveSet+uart0_LoopDeviceNo-1)*2+1]&0x02)==0x02) 
				          {  if(ParaList[cMetreType*2+1]==1)
										    uart0_ModbusRequest(uart0_LoopDeviceNo+100,4,0x1d,2);
										 else
                      // SetModDelay();   											 
										     uart0_ModbusRequest(uart0_LoopDeviceNo+100,4,0x64,1);  //20240228
									}
				        else
									 SetModDelay();   
				        break;
//endif

									 
	     	default:break;
	      }
	 }
//-------------------------------	  
   }    	
	uart0_Master_DealWith_ModbusRespond(uart0_LoopDeviceNo);
 }	 

}




void uart0_SendCloudComand() 
{
 SendDataToBus1_uart0(&uart0_Sendcomand[uart0_Cloudcomand_RP][1],uart0_Sendcomand[uart0_Cloudcomand_RP][0]);
 uart0_CloudComandResondWaiting=1;  	
}	 
  	
void uart0_SendLocalComand() 
{
 SendDataToBus1_uart0(&uart0_LocalSendcomand[uart0_Localcomand_RP][1],uart0_LocalSendcomand[uart0_Localcomand_RP][0]);		
uart0_LocalComandResondWaiting=1;	
}


/*--------------------------------------------------------
//Function:  void  GetCommunicationStatus()
//Input:    None
//Output:   None
//Description: 获取温控器和电表的通讯状态
               在激活的情况下，如果连续15s没有数据回复，则认为通讯端口故障，将温度设置为-3000，
//----------------------------------------------------------*/
void  GetCommunicationStatus()
{ uint8_t  i,m,k,n[20],j[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

 union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    
    uint16_t  tempT;	
    int16_t   tempInt;
		 
 for(i=0;i<10;i++)
	{ if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)!=0)	
	   {  if(DeviceInterval[i]>=30)
				 {  j[i]=1;
					  #ifdef Debuginfor
						if(DeviceInterval[i]==30)	
						{	for(m=0;m<20;m++)
                 n[m]=0;
							m=0;
							SendString_uart0("Receive interval:");
							n[m++]=(i+1)+0x30;
							n[m++]=';';
							n[m++]=DeviceInterval[i]/100+0x30;
							n[m++]=DeviceInterval[i]%100/10+0x30;
							n[m++]=DeviceInterval[i]%10+0x30;
							SendString_uart0(n);
							SendString_uart0("\r\n\r\n");
						}
							#endif
					 if(DeviceInterval[i]>100)
						 DeviceInterval[i]=100;
				           tempT=62536;
		                          tempInt=(int16_t)tempT;
		                           temp.vf= (float)(tempInt);
		                          temp.vf/=10; 
                                          for(m=0;m<4;m++) //回风温度
                                               {    uart0_Var_List[i*12+m]=temp.vchar[uart0_floatSeq[m]];
                                                   uart0_Var_List[i*12+m+8]=temp.vchar[uart0_floatSeq[m]];
                                                } 
																		uart0_OrigPara_List[i*cContorlRegLen*2]= tempT/256; 
                                    uart0_OrigPara_List[i*cContorlRegLen*2+1]= tempT%256; 
																		uart0_OrigPara_List[i*cContorlRegLen*2+2]= tempT/256; 
                                    uart0_OrigPara_List[i*cContorlRegLen*2+3]= tempT%256; 						
           }
			 else
				 j[i]=0;
	   }
	if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x02)!=0)	
	   {  if(DeviceInterval[i+10]>=30)
				{ if(DeviceInterval[i+10]>100)
						 DeviceInterval[i+10]=100; 
					#ifdef Debuginfor
						if(DeviceInterval[i]==30)	
						{	for(m=0;m<20;m++)
                 n[m]=0;
							m=0;
							SendString_uart0("Receive interval:");
							n[m++]=(i+101)/100+0x30;
							n[m++]=(i+101)%100/10+0x30;
							n[m++]=(i+101)%10+0x30;
							n[m++]=';';
							n[m++]=DeviceInterval[i+10]/100+0x30;
							n[m++]=DeviceInterval[i+10]%100/10+0x30;
							n[m++]=DeviceInterval[i+10]%10+0x30;
							SendString_uart0(n);
							SendString_uart0("\r\n\r\n");
						}
							#endif
					
					j[i+10]=1;
				   tempT=62536;
				   uart0_Var_List[i*cVarLenByte+cPowerMetre*2]=tempT/256;
				   uart0_Var_List[i*cVarLenByte+cPowerMetre*2+1]=tempT%256;
				   uart0_Var_List[i*cVarLenByte+cPowerMetre*2+2]=tempT/256;
				   uart0_Var_List[i*cVarLenByte+cPowerMetre*2+3]=tempT%256;
				   uart0_Var_List[i*cVarLenByte+cPowerMetre*2+4]=0xff;
           uart0_Var_List[i*cVarLenByte+cPowerMetre*2+5]=0xff;
			     uart0_Var_List[i*cVarLenByte+cPowerMetre*2+6]=tempT/256;
           uart0_Var_List[i*cVarLenByte+cPowerMetre*2+7]=tempT%256;
					
			        }
			 else
				 j[i+10]=0;
	   }	 
}
k=0;
for(i=0;i<10;i++)
  { if(j[i]==1)
	   uart0_Bit_List[i]|=0x80;
	 else 	
	 uart0_Bit_List[i]&=0x7f;
	}	
for(i=0;i<10;i++)
  { if(j[i+10]==1)
	   uart0_Bit_List[i]|=0x40;
	 else 	
	 uart0_Bit_List[i]&=0xbf;
	}	
	//add 20220922
#if 0	
k=0;
for(i=0;i<10;i++)
  { if((uart0_Bit_List[i]&0xc0)!=0)	
       k++;
	}	
if(k!=0)
	uart0_Var_List[cDoorPos*2+1]|=0x10;
else
	uart0_Var_List[cDoorPos*2+1]&=0xef;

#endif
k=0;
if((uart0_Bit_List[0]&0xc0)!=0)	
       k++;

if(k!=0)
	uart0_Var_List[cDoorPos*2+1]|=0x10;
else
	uart0_Var_List[cDoorPos*2+1]&=0xef;


k=0;
if((uart0_Bit_List[1]&0xc0)!=0)	
       k++;

if(k!=0)
	uart0_Var_List[cDoorPos*2+1]|=0x20;
else
	uart0_Var_List[cDoorPos*2+1]&=0xdf;


}


uint8_t  GetBitStatus(uint8_t  bit)
 {
	  if((SendTempBufferBak_uart0[3+bit/8]&(1<<(bit%8)))!=0)
			  return(1);
		else
			 return(0); 
}
void  SetStatus(uint8_t Device,uint8_t  bit)
{
	uart0_Bit_List_bak[(Device-1)*2+bit/8]|=(1<<(bit%8));
}

void ClrStatus(uint8_t Device,uint8_t  bit)
{
	uart0_Bit_List_bak[(Device-1)*2+bit/8]&=~(1<<(bit%8));
}



uint8_t  GetConSensorStatus(uint8_t Device,uint8_t  bit)
  {
		if(((uart0_Bit_List_bak[(Device-1)*2+bit/8]&(1<<(bit%8)))!=0))
			 return(1);
		else
			 return(0);
	}

 
uint8_t GetAllSensorError(uint8_t Device)
 {
   uint8_t  i=0;
	 if(GetConSensorStatus(Device,cConSensorError)==1)
		  i++;
	 if(GetConSensorStatus(Device,cFrozeSensorError)==1)
		  i++;
	 
	 if(i==0)
		  return(0);
	 else
		 return(1); 
 }	 
uint8_t GetAllDeviceError(uint8_t Device)	
{
   uint8_t  i=0;
	if(GetConSensorStatus(Device,cHighTempError)==1)
		  i++;
	 if(GetConSensorStatus(Device,cExtInputError)==1)
		  i++;
	 if(GetConSensorStatus(Device,cLowTempError)==1)
		  i++;
   if(i==0)
		  return(0);
	 else
		 return(1); 
 }	 

//---------------------------------------------------------------
//Function:  uint8_t GetAllError(uint8_t Device)
//Input:    None
//Output:   None
//Description:双机轮值时候，故障检测判断条件，机组故障+sensor故障+通讯故障=故障转换  //汉字
//-----------------------------------------------------------------
uint8_t GetAllError(uint8_t Device)
 {
   uint8_t  k=0;
	 if(GetAllSensorError(Device)==1)  //sensor error
		 k++;
	 if(GetConSensorStatus(Device,cExtInputError)==1)   //input error
		  k++;
	 if((uart0_Bit_List[Device-1]&0x80)==0x80)    // comm eror
		 k++;
	  if(uart0_VolCurrentAlarm[Device-1]!=0)    //vol and current 
		 k++;
	//ADD 20250103
		#if 1
		if(uart0_Var_List[(AlarmStartAddr+1+(Device-1)*2)*2+1]==1)   //high press
			 k++; 
		if(uart0_Var_List[(AlarmStartAddr+2+(Device-1)*2)*2+1]==1)    //low PRESS
			 k++;
		#endif
  // END OF ADD  20250103		
	 if(k==0)
		  return(0);
	 else
		 return(1); 
 } 
	 //add 20221022

uint8_t  Getnet[]={"config,get,netstatus,1\r\n"};
uint8_t  Getgps[]={"config,get,gps\r\n"};
uint8_t  GetNo[]={"config,get,netchaninfo,1\r\n"};
uint8_t  Getccid[]={"config,get,iccid\r\n"};
uint8_t  Topic_SSID[]={"zs/dtudxhua/cloud/"};
uint8_t  Topic_ccid[]={"config,iccid,ok,"};
uint8_t  Topic_gps[]={"\r\nconfig,gps,ok,"};
uint8_t  Get_lbs[]={"config,get,lbsloc\r\n"};
uint8_t  Topic_lbs[]={"\r\nconfig,lbsloc,ok,"};
void SendGetNetComand()
{

 SendString_uart3(Getnet);

}
#define   useLBS
void  SendGetLbsComand();
void SendGetGpsComand()
{
	#ifdef  useLBS
	  SendGetLbsComand();
	#else
	  SendGetGpsComand1(); 
	#endif
}

//add 20240928
void SendGetLbsComand()
{ uint8_t *ResBegin=NULL,*ResEnd=NULL;
   uint8_t  GPS[27],i,k=0;
   
  	ClearUart3();
	  SendString_uart3(Get_lbs);
	  DelaySecond_1(200);
//gps
//config,get,gps r n应答WGS84 坐标系
//定位失败全是0 \r\nconfig,gps,ok,E,110.1234567,N,20.1234567\r\n
//LBS
//	config,get,lbsloc\r\n
//应答          \r\nconfig,lbsloc,ok,110.1234567，020.1234567\r\n 
	
	
	if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=38)//45
	   {
                       ResBegin=strstr(ReceiveBuff_uart3,Topic_lbs);
	   	          if(ResBegin != NULL)
			     {   ResBegin += strlen(Topic_lbs);        
						 ResEnd=strstr(ResBegin,"\r\n");
			                 
	   	                if(!((ResBegin == NULL) || (ResEnd == NULL) || (ResBegin >ResEnd)))
	   	                   {  //  ResBegin += strlen(Topic_gps);	
	   	                      k=ResEnd-ResBegin;
	   	                      if(ResEnd-ResBegin>=17)
	   	                       { 
	   	    	                 memcpy(GPS, ResBegin, k); 
														for(i=0;i<k;i++)
															 //ParaList[cGpsbuffer1*2+i]=GPS[i];
															  GPSSEND[i]=GPS[i];
	   	    	                 for(i=0;i<k/2;i++)
	   	    	                 { uart0_Var_List[cGpsbuffer*2+i*2+1]=GPS[i*2];
															 uart0_Var_List[cGpsbuffer*2+i*2]=GPS[i*2+1];
														 }
	   	    	                 uart0_Var_List[cGpsbuffer*2+k]=',';
														
														 for(i=k+1;i<27;i++)
	   	    	                    uart0_Var_List[cGpsbuffer*2+i]=0X30;
	   	    	                   
	   	    	         }
	   	    	   } 
    }
  }
}	 	











void SendGetGpsComand1()
{ uint8_t *ResBegin=NULL,*ResEnd=NULL;
   uint8_t  GPS[27],i,k=0;
   
  	ClearUart3();
	  SendString_uart3(Getgps);
	  DelaySecond_1(200);

//config,get,gps r n应答WGS84 坐标系
//定位失败全是0 \r\nconfig,gps,ok,E,110.1234567,N,20.1234567\r\n
 
	if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=38)//45
	   {
                       ResBegin=strstr(ReceiveBuff_uart3,Topic_gps);
	   	          if(ResBegin != NULL)
			     {   ResBegin += strlen(Topic_gps);        
						 ResEnd=strstr(ResBegin,"\r\n");
			                 
	   	                if(!((ResBegin == NULL) || (ResEnd == NULL) || (ResBegin >ResEnd)))
	   	                   {  //  ResBegin += strlen(Topic_gps);	
	   	                      k=ResEnd-ResBegin;
	   	                      if(ResEnd-ResBegin>=21)
	   	                       { 
	   	    	                 memcpy(GPS, ResBegin, k); 
														for(i=0;i<k;i++)
															 //ParaList[cGpsbuffer1*2+i]=GPS[i];
															  GPSSEND[i]=GPS[i];
	   	    	                 for(i=0;i<k/2;i++)
	   	    	                 { uart0_Var_List[cGpsbuffer*2+i*2+1]=GPS[i*2];
															 uart0_Var_List[cGpsbuffer*2+i*2]=GPS[i*2+1];
														 }
	   	    	                 uart0_Var_List[cGpsbuffer*2+k]=',';
														
														 for(i=k+1;i<27;i++)
	   	    	                    uart0_Var_List[cGpsbuffer*2+i]=0X30;
	   	    	                   
	   	    	         }
	   	    	   } 
    }
  }
}	 	


void  SendGetNoComand()
{ uint8_t *ResBegin=NULL,*ResEnd=NULL;
   uint8_t  No[20],i,k=0;
   uint64_t  NoLong;
  while(1)
     {  if(k>5)
			   {  GetCCidFlag=0;
           break;
				 }					 
	ClearUart3();
	SendString_uart3(GetNo);
	DelaySecond_1(200);
	k++;
	
//config,netchaninfo,ok,mqtt,uart,60,mqtt.coldwang.com,1883,,tancehao,tancehao123888,1,1,0,0,0,zs/dtudxhua/cloud/22980208012,zs/dtudxhua/data/22980208012,0,0,0,,,0,	
 
	if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=100)
	   {
                       ResBegin=strstr(ReceiveBuff_uart3,Topic_SSID);
	   	          if(ResBegin != NULL)
			    { ResEnd=strstr(ResBegin,",");
			   
	   	                if(!((ResBegin == NULL) || (ResEnd == NULL) || (ResBegin > ResEnd)))
	   	                   {    ResBegin += strlen(Topic_SSID);	
	   	                     
	   	                      if(ResEnd-ResBegin==11)
	   	                       { 
	   	    	                 memcpy(No, ResBegin, 11); 
	   	    	                 for(i=0;i<11;i++)
	   	    	                   No[i]=No[i]-0x30;
	   	    	                 NoLong=0;
															 i=0;
	   	    	                   ParaList[cid*2]=No[i++]*16+No[i++]; 
															 ParaList[cid*2+1]=No[i++]*16+No[i++];
															 ParaList[cid*2+2]=No[i++]*16+No[i++];
															 ParaList[cid*2+3]=No[i++]*16+No[i++];
															 ParaList[cid*2+4]=No[i++]*16+No[i++];
															 ParaList[cid*2+5]=No[i++]*16;
															  ProgramEepromByte();
															 GetCCidFlag=1;
															 
												  #if  0			 
															 for(i=0;i<11;i++)   
	   	    	                  NoLong=NoLong*10+No[i];
															 ParaList[cid*2]=0;   
	   	    	               ParaList[cid*2+1]=0;
	   	    	               ParaList[cid*2+2]=NoLong/65536/65536/256;   
	   	    	               ParaList[cid*2+3]=NoLong/65536/65536%256;    
	   	    	               ParaList[cid*2+4]=NoLong/65536%65536/256;  
	   	    	               ParaList[cid*2+5]=NoLong/65536%65536%256;
                           ParaList[cid*2+6]=NoLong%65536/256;  
	   	    	               ParaList[cid*2+7]=NoLong%65536%256;
												#endif	 
	   	    	               break;
	   	    	               }  
	   	    	                
	   	    	                   
	   	    	         }
	   	    	   } 
    }
  }
}	 	
void  SendGetccid()
{
   uint8_t *ResBegin=NULL,*ResEnd=NULL;
   uint8_t  No[20],i;
   uint64_t  NoLong;
	ClearUart3();
	SendString_uart3(Getccid);
	DelaySecond_1(200);
	
	
//config,netchaninfo,ok,mqtt,uart,60,mqtt.coldwang.com,1883,,tancehao,tancehao123888,1,1,0,0,0,zs/dtudxhua/cloud/22980208012,zs/dtudxhua/data/22980208012,0,0,0,,,0,	
	
	if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=20)
	   {
                       ResBegin=strstr(ReceiveBuff_uart3,Topic_ccid);
	   	          if(ResBegin != NULL)
			    { ResEnd=strstr(ResBegin,"\r\n");
			   
	   	                if(!((ResBegin == NULL) || (ResEnd == NULL) || (ResBegin > ResEnd)))
	   	                   {    ResBegin += strlen(Topic_ccid);	
	   	                     
	   	                      if(ResEnd-ResBegin==20)
	   	                       { 
	   	    	                 memcpy(No, ResBegin, 20); 
	   	    	                 for(i=0;i<20;i++)
	   	    	                  {
                                if(No[i]<=0x39)
																   No[i]=No[i]-0x30;
																else
																	 No[i]=No[i]-0x41+10;
															}
	   	    	                 NoLong=0;
															 
															 i=0;
	   	    	                   ParaList[cCCid*2]=No[i++]*16+No[i++]; 
															 ParaList[cCCid*2+1]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+2]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+3]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+4]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+5]=No[i++]*16+No[i++]; 
															 ParaList[cCCid*2+6]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+7]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+8]=No[i++]*16+No[i++];
															 ParaList[cCCid*2+9]=No[i++]*16+No[i++];
															  ProgramEepromByte();
															  GetCCidFlag=1;
													#if  0		 
	   	    	                for(i=0;i<10;i++)   
	   	    	                  NoLong=NoLong*10+No[i];
	   	    	               ParaList[cCCid*2]=NoLong/65536/65536/256;   
	   	    	               ParaList[cCCid*2+1]=NoLong/65536/65536%256;    
	   	    	              ParaList[cCCid*2+2]=NoLong/65536%65536/256;  
	   	    	               ParaList[cCCid*2+3]=NoLong/65536%65536%256;
	   	    	               ParaList[cCCid*2+4]=NoLong%65536/256;  
	   	    	               ParaList[cCCid*2+5]=NoLong%65536%256; 
	   	    	               
	   	    	               for(i=10;i<20;i++)
	   	    	                   No[i]=No[i]-0x30;
	   	    	                 NoLong=0;
	   	    	                for(i=10;i<20;i++)   
	   	    	                  NoLong=NoLong*10+No[i];
	   	    	              ParaList[cCCid*2+6]=NoLong/65536/65536/256;   
	   	    	               ParaList[cCCid*2+7]=NoLong/65536/65536%256;    
	   	    	              ParaList[cCCid*2+8]=NoLong/65536%65536/256;  
	   	    	               ParaList[cCCid*2+9]=NoLong/65536%65536%256;
	   	    	               ParaList[cCCid*2+10]=NoLong%65536/256;  
	   	    	               ParaList[cCCid*2+11]=NoLong%65536%256; 
	   	    	              #endif      
	   	    	               }  
	   	    	                
	   	    	                   
	   	    	         }
										else
                      GetCCidFlag=0;											
	   	    	   } 
					 	else
                      GetCCidFlag=0;		
    }
		 	else
                      GetCCidFlag=0;		
}

 
 
/*-----------------------------------------------------------------------------------------------------
 以上为uart0从温控器获取数据后的处理
 以下为uart0将读取的状态数据和命令返回数据返回到云端
  -------------------------------------------------------------------------------------------------------*/
 void uart3_SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat)
{
  uint8_t SendBuffer[220];
  uint8_t i=0,j;
  uint16_t CRCTemp;
	
  if(ParaList[cTempOffset*2]*256+ParaList[cTempOffset*2+1]!=1234)
 { 
  SendBuffer[i++] =0xaa;
  SendBuffer[i++] =0x55;	
  SendBuffer[i++] =0x00;
  if(FunctionCode==0x01)
    SendBuffer[i++] =0x01;
   else if(FunctionCode==0x02)
    SendBuffer[i++] =0x01;  	
 else if(FunctionCode==0x03)
    SendBuffer[i++] =0x04;  
 else if(FunctionCode==0x04)
    SendBuffer[i++] =0x03;  
SendBuffer[i++] =StartAddr/256; 
SendBuffer[i++] =StartAddr%256;

SendBuffer[i++] =Addr;
SendBuffer[i++] =FunctionCode;
if((FunctionCode==0x01)||(FunctionCode==0x02))
 {
  	
   SendBuffer[i++]=(Len-1)/8+1;
  
 }
else
 SendBuffer[i++] =Len*2;

 for(j=0;j< SendBuffer[8];j++)
   SendBuffer[i++]=SendDat[j];
CRCTemp=CRC(&SendBuffer[6],j+3);
SendBuffer[i++]=CRCTemp/256;
SendBuffer[i++]=CRCTemp%256;
SendDataToBus1_uart3(SendBuffer,i);
} 
}


//---------------------------------------------------------------
//Function:  void uart3_SendCoolDat()
//Input:    None
//Output:   None
//Description:定期发送数据数据到云端
//-----------------------------------------------------------------
void uart3_SendCoolDat()
{
int8_t SendBuf[200];
  uint8_t i=0,j;
  uint16_t CRCTemp;
#ifdef UseSim
	for(i=0;i<15;i++)
		uart0_Var_List[(114+i)*2+1]=Random+i;
	Random++;
	
Random1++;
if(Random1>=13)	
{	Random1=0;
    uart0_Var_List[(102+12)*2+1]^=0x01;
 }
else
	 uart0_Var_List[(102+Random1-1)*2+1]^=0x01;
  uart0_Var_List[(102+Random1)*2+1]^=0x01;
	
#endif	
	
	
for(i=0;i<10;i++)
   SendBuf[i]=uart0_Bit_List[i];
uart3_SendControlData(1,2,1,80,SendBuf) ;
  
for(i=0;i<120;i++)
   SendBuf[i]=uart0_Var_List[i];
uart3_SendControlData(1,4,1,60,SendBuf) ; 		

	for(i=0;i<140;i++)
   SendBuf[i]=uart0_Para_List[i];
uart3_SendControlData(1,3,1,70,SendBuf) ;  
  	
for(i=0;i<140;i++)
   SendBuf[i]=uart0_Para_List[i+140];
uart3_SendControlData(1,3,0x71,70,SendBuf) ; 
#if  1
for(i=0;i<cVarLenByte;i++)
   SendBuf[i]=uart0_Var_List[i+120];
uart3_SendControlData(1,4,0x61,cVarLenByte/2,SendBuf) ; 	
#endif
#if  1
for(i=0;i<56;i++)
   SendBuf[i]=uart0_Var_List[i+204];
uart3_SendControlData(1,4,0x103,28,SendBuf) ; 	
#endif
	for(i=0;i<22;i++)
	  //SendBuf[i]=ParaList[cGpsbuffer1*2+i]; 	    	                
		 SendBuf[i]=GPSSEND[i];
		uart3_SendControlData(1,4,0x131,11,SendBuf) ; 
}

//---------------------------------------------------------------
//Function:  void uart3_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len)
//Input:    None
//Output:   None
//Description:将终端返回的数据发送到云端
//-----------------------------------------------------------------
void uart3_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len)
 { 
 SendDataToBus1_uart3(ReturnBuff,Len);	
 	
 }	
 
//------------------------------------------------------------
//Function:  uart0_AddCloudBuffer(uint8_t  i)
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 云端对控制器的操作数据通过0x10，进行转发，云端操作控制器简易变量表，控制器通过转换控制对应的终端
//-------------------------------------------------------------- 
void uart0_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send)  
   {  
  
   uint16_t  TempCrc=0; 
		 
		 uart0_LocalComandNo=send[0];
      uart0_LocalComandFunction=FunCode;
  
       uart0_LocalSendcomand[uart0_Localcomand_WP][0]=8;
       uart0_LocalSendcomand[uart0_Localcomand_WP][1]=uart0_LocalComandNo;
       uart0_LocalSendcomand[uart0_Localcomand_WP][2]=FunCode;
       uart0_LocalSendcomand[uart0_Localcomand_WP][3]=(i%cContorlRegLen)/256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][4]=(i%cContorlRegLen)%256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][5]=send[4];
       uart0_LocalSendcomand[uart0_Localcomand_WP][6]=send[5];
       TempCrc=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	   uart0_LocalSendcomand[uart0_Localcomand_WP][7]=TempCrc/256;  //High crc
  	   uart0_LocalSendcomand[uart0_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Localcomand_WP++;
    	 if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
    	 }
   } 	
   
 
//------------------------------------------------------------
//Function:  void uart0_AddCloudTranlate(uint8_t *send)
//Input:    (uint8_t  i:访问的地址-5000
//Output:   None
//Description: 云端对终端的直接操作，映射在地址5100地址之上， 一号终端对应5100，二号终端对应5200，一次类推
//-------------------------------------------------------------- 
void uart0_AddCloudTranlate(uint8_t *send)  
   {  
  
   uint16_t  TempCrc=0;
   TempCrc=send[2]*256+send[3];		 
	if((	send[1]!=0x06)||((TempCrc%100)!=52))
	{ TempCrc=send[2]*256+send[3];
		 TempCrc-=5000;
		 uart0_CloudComandNo=TempCrc/100;
		 send[0]=uart0_CloudComandNo;
		 send[2]=0;
		  send[3]=TempCrc%100;
      uart0_CloudComandFunction=send[1];
    
       uart0_Sendcomand[uart0_Cloudcomand_WP][0]=8;
		  for(TempCrc=0;TempCrc<6;TempCrc++)
        uart0_Sendcomand[uart0_Cloudcomand_WP][TempCrc+1]=send[TempCrc];
		    
		     TempCrc=CRC(&uart0_Sendcomand[uart0_Cloudcomand_WP][1],6);
  	uart0_Sendcomand[uart0_Cloudcomand_WP][7]=TempCrc/256;  //High crc
  	uart0_Sendcomand[uart0_Cloudcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Cloudcomand_WP++;
    	 if(uart0_Cloudcomand_WP>=5)
    	    uart0_Cloudcomand_WP=0;
    	  if(uart0_Cloudcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Cloudcomand_RP++;
    	     if(uart0_Cloudcomand_RP>=5)
    	       uart0_Cloudcomand_RP=0;
    	 }
   } 	
	 
 } 
/*------------------------------------------------------------
//Function:  uart0_AddCloudBuffer(uint8_t  i)
//Input:    
//Output:   None
//Description: 将06写入命令转到对应的控制器访问命令
	  只有通过0x10的命令才是写cpu缓冲参数区，要配置控制器需要进行二次转换
		           启停的0001要转为ff00
//--------------------------------------------------------------*/ 
void uart0_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send)     
     {  
        uint16_t  TempCrc=0;   
       uart0_LocalComandNo=Temp/14+1;
  	       uart0_LocalComandFunction=FunCode; 
  	   	uart0_Para_List[Temp*2]=send[7];
  	   	uart0_Para_List[Temp*2+1]=send[8];
  	   	uart0_LocalSendcomand[uart0_Localcomand_WP][0]=8;
     
       uart0_LocalSendcomand[uart0_Localcomand_WP][1]=uart0_LocalComandNo;
       uart0_LocalSendcomand[uart0_Localcomand_WP][2]=FunCode;
       uart0_LocalSendcomand[uart0_Localcomand_WP][3]=(uart0_Para_MapTable[Temp%14])/256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][4]=(uart0_Para_MapTable[Temp%14])%256;
			#if 0
        if((Temp%14)==1) 
				{
             uart0_LocalSendcomand[uart0_Localcomand_WP][5]=send[7];
             uart0_LocalSendcomand[uart0_Localcomand_WP][6]=send[8]^0x01;
    					
				}
		
			 
			 else
				 	#endif	
				 if(((Temp%14)==0)||((Temp%14)==1))
			 {	 if(send[8]==0x01)
						 { uart0_LocalSendcomand[uart0_Localcomand_WP][5]=0xff;
               uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x00;
							}
					 
			     else
					 {
       uart0_LocalSendcomand[uart0_Localcomand_WP][5]=send[7];
       uart0_LocalSendcomand[uart0_Localcomand_WP][6]=send[8];
					 }
				 }
			 else
			 {
				 uart0_LocalSendcomand[uart0_Localcomand_WP][5]=send[7];
       uart0_LocalSendcomand[uart0_Localcomand_WP][6]=send[8];
			 }
       TempCrc=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	uart0_LocalSendcomand[uart0_Localcomand_WP][7]=TempCrc/256;  //High crc
  	uart0_LocalSendcomand[uart0_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Localcomand_WP++;
    	 if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
            }
    }   
    

/*------------------------------------------------------------
//Function:  void uart0_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send)    
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 将10写入命令float格式转换为控制终端需要的uint格式
//--------------------------------------------------------------*/ 
void uart0_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send)    
{  uint8_t  i;
	 int16_t  Tempint;
   uint16_t  Tempuint,TempCrc;	
 union
     {float  vf;
      uint8_t vchar[4];
     }temp;   
           uart0_LocalComandNo=Temp/14+1;
  	    uart0_LocalComandFunction=0x06; 
  	     for(i=0;i<4;i++)
  	      temp.vchar[i]=send[7+3-i];
  	    Tempint=(int16_t)(temp.vf*10);
  	    Tempuint = (uint16_t)Tempint;
  	    uart0_LocalSendcomand[uart0_Localcomand_WP][0]=8;
     
       uart0_LocalSendcomand[uart0_Localcomand_WP][1]=uart0_LocalComandNo;
       uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x06;
       uart0_LocalSendcomand[uart0_Localcomand_WP][3]=(uart0_Para_MapTable[Temp%14])/256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][4]=(uart0_Para_MapTable[Temp%14])%256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][5]=Tempuint/256;
       uart0_LocalSendcomand[uart0_Localcomand_WP][6]=Tempuint%256;
       TempCrc=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	uart0_LocalSendcomand[uart0_Localcomand_WP][7]=TempCrc/256;  //High crc
  	uart0_LocalSendcomand[uart0_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Localcomand_WP++;
    	 if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
  	   }
    }	   

		
		
		
/*------------------------------------------------------------
//Function: void uart0_OpenOrClose(uint8_t  no,uint8_t  Op)
//Input:   uint8_t  no：终端编号 从1 开始
          uint8_t  Op:开关
//Output:   None
//Description: 电流电压超限不执行开启动作，通讯异常不执行操作
//--------------------------------------------------------------*/ 		
		
void uart0_OpenOrClose(uint8_t  no,uint8_t  Op)
  {
    
        uint16_t  TempCrc=0;
if((uart0_Bit_List[no-1]&0x80)==0x00)
{
		
	if((Op==0)||((uart0_VolCurrentAlarm[no-1]&0x03)==0x00))
		
	 
	
	{ uart0_CloudComandNo=no;
      uart0_CloudComandFunction=0x06;
    
       uart0_Sendcomand[uart0_Cloudcomand_WP][0]=8;
       uart0_Sendcomand[uart0_Cloudcomand_WP][1]=uart0_CloudComandNo;
       uart0_Sendcomand[uart0_Cloudcomand_WP][2]=uart0_CloudComandFunction;
       uart0_Sendcomand[uart0_Cloudcomand_WP][3]=0;
       uart0_Sendcomand[uart0_Cloudcomand_WP][4]=51;
       uart0_Sendcomand[uart0_Cloudcomand_WP][5]=255*Op;
       uart0_Sendcomand[uart0_Cloudcomand_WP][6]=0;
       TempCrc=CRC(&uart0_Sendcomand[uart0_Cloudcomand_WP][1],6);
  	   uart0_Sendcomand[uart0_Cloudcomand_WP][7]=TempCrc/256;  //High crc
  	   uart0_Sendcomand[uart0_Cloudcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Cloudcomand_WP++;
    	 if(uart0_Cloudcomand_WP>=5)
    	    uart0_Cloudcomand_WP=0;
    	  if(uart0_Cloudcomand_WP==uart0_Cloudcomand_RP)   
    	  {  uart0_Cloudcomand_RP++;
    	     if(uart0_Cloudcomand_RP>=5)
    	       uart0_Cloudcomand_RP=0;
    	 }
   }
 }
}



/*------------------------------------------------------------
//Function: void uart0_OpenOrClose(uint8_t  no,uint8_t  Op)
//Input:   uint8_t  no：终端编号 从1 开始
          uint8_t  Op:开关
//Output:   None
//Description: 电流电压超限不执行开启动作，通讯异常不执行操作
//--------------------------------------------------------------*/ 		
void uart0_UpdateOpenOrClose(uint8_t  no)
  {
    uint16_t  tempT;	
		if(no==0)
			 no=1;
	   uart0_LocalSendcomand[uart0_Localcomand_WP][0] =8;
	   uart0_LocalSendcomand[uart0_Localcomand_WP][1] =no;
	   uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x01;
           uart0_LocalSendcomand[uart0_Localcomand_WP][3]=0; 
           uart0_LocalSendcomand[uart0_Localcomand_WP][4]=0; 
           uart0_LocalSendcomand[uart0_Localcomand_WP][5]=0;  
           uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x34;
           tempT=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	   uart0_LocalSendcomand[uart0_Localcomand_WP][7]=tempT/256;  //High crc
  	   uart0_LocalSendcomand[uart0_Localcomand_WP][8]=tempT%256;  //Low crc
           uart0_Localcomand_WP++;
    	   if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
        }
     }
/*------------------------------------------------------------
//Function:  GetVoltage(uint8_t  MeterNum)
//Input:    uint8_t  MeterNum;控制器编号 从0开始
//Output:   None
//Description: 读取对应控制器对应电表的电压值
//--------------------------------------------------------------*/ 
 int16_t GetVoltage(uint8_t  MeterNum)
  {
		 return((int16_t)(uart0_Var_List[(60+MeterNum*10)*2]*256+uart0_Var_List[(60+MeterNum*10)*2+1]));
		//20250311 return(3000);
	} 
 
/*------------------------------------------------------------
//Function:  uint16 GetCuurent(uint8_t  MeterNum)
//Input:    uint8_t  MeterNum;控制器编号 从0开始
//Output:   None
//Description: 读取对应控制器对应电表电流值
//--------------------------------------------------------------*/ 
int16_t GetCuurent(uint8_t  MeterNum)
  {
		return((int16_t)(uart0_Var_List[(60+MeterNum*10+1)*2]*256+uart0_Var_List[(60+MeterNum*10+1)*2+1]));
		 //20250311    return(0);
	} 

	
	
/*------------------------------------------------------------
//Function:  void uart0_VolCurrentOverLoadAlarm()
//Input:    uint8_t  MeterNum;控制器编号 从0开始
//Output:   None
//Description: 处理电压超限的处理及恢复
	如果为单机组，检测电表电压，在有电表的情况下，出现超限则关闭对应的机组，设置超限标识
	如果为双机组，则检测双机组中任何一个对应的电表，如果出现超限，则关闭对应的两个机组，设置超限标识
	如果为单机组，电表回复正常，取消超限标识，如果机组处于关闭状态，则开启对应的机组
	如果为双机组，电表回复正常，取消超限标识，只有两个机组都关闭的情况下，开启其中一个机组
	
//--------------------------------------------------------------*/ 	
void uart0_VolCurrentOverLoadAlarm()
 {
    uint8_t  i;
    uint16_t  Tempint;
	 int16_t     Tempint1;
 float  tempf,tempi; 
//----------------------single motor------------------    
	 
      
           for(i=0;i<10;i++)
           { 
              if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)==0x01)//active				 
							{   Tempint1=GetVoltage(i);
								tempf=(float)(Tempint1);
                  tempi=(float)(GetCuurent(i)); 
                if((tempf!=-3000)&&(tempi!=-3000))
							
							 { 
								 
								 
//---------------------------------------------single motor-------------------------------								 
								 if((ParaList[(cMotorType+i)*2+1]==0x01)||(ParaList[(cMotorType+i)*2+1]==0x03)) //single motor
						 
						     { 
						       if(((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x02)==0x02)&&((uart0_Bit_List[i]&0x40)==0)) //have  metrer  and comm ok
									 {	 
           	         if(ParaList[(cMotorType+i)*2+1]==0x01)  //single motor+220v
							             tempf=(float)(GetVoltage(i));
							        else                                  //single motor+380v 
								    tempf=(float)(GetVoltage(i));
              
									 Tempint=(uint16_t)(tempf);
                    Tempint/=10;
                  if(Tempint>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
							          {  if(GetConSensorStatus(i+1,cRemoteOpen)==1) 
							 	              uart0_OpenOrClose(i+1,0x00);
								         uart0_VolCurrentAlarm[i]|=0x01;
							       }
							     else
								    {									uart0_VolCurrentAlarm[i]&=0xfe; 
									          //  if(GetConSensorStatus(i+1,cRemoteOpen)==0) 
																  
									           //     uart0_OpenOrClose(i+1,0x01);
								   }
						
     							
 ////--------------------
           if(ParaList[(cMotorType+i)*2+1]==0x01)	
								tempf=(float)(GetCuurent(i));
							else
								tempf=(float)(GetCuurent(i));
                  Tempint=(uint16_t)(tempf);
                   Tempint/=100;
               if(Tempint>ParaList[cMaxVolt*2+2]*256+ParaList[cMaxVolt*2+3])
                    {  if(GetConSensorStatus(i+1,cRemoteOpen)==1) 
                    	uart0_OpenOrClose(i+1,0x00);
								  uart0_VolCurrentAlarm[i]|=0x02;
							 }
							  else
										uart0_VolCurrentAlarm[i]&=0xfd;
								//if(GetConSensorStatus(i+1,cRemoteOpen)==0) 
                //    uart0_OpenOrClose(i+1,0x01);								
               
            
////---------------------------------								 
							}//metre
									 } //single
									 }	//valid  value
								else
								{
	                   uart0_VolCurrentAlarm[i]&=0xfd;
                }									
				}	//alive	
				 }	 //for
//-----------------------------------双机--------------------------			
   for(i=0;i<10;i++)
       {				 
	if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)==0x01)//active	
	  {  
			            tempf=(float)(GetVoltage(Couple[i*2]-1));
                  tempi=(float)(GetCuurent(Couple[i*2]-1)); 
                if((tempf!=-3000)&&(tempi!=-3000))
							
							 { 
			
			if((ParaList[(cMotorType+i)*2+1]==0x02)|| (ParaList[(cMotorType+i)*2+1]==0x04))  //双机
           {   
      	     
						 if(((ParaList[(cLoraNodeAliveSet+Couple[i*2]-1)*2+1]&0x02)==0x02)&&((uart0_Bit_List[Couple[i*2]-1]&0x40)==0))  //有表 comm ok
      	       {
      	   	 //----------vol----------  
								 if(ParaList[(cMotorType+i)*2+1]==0x02) 
								 tempf=(float)(GetVoltage(Couple[i*2]-1));
						    else
									 tempf=(float)(GetVoltage(Couple[i*2]-1));
                     Tempint=(uint16_t)(tempf);
                      Tempint/=10;
                  if(Tempint>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
                   {  if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1) 
                   	uart0_OpenOrClose(Couple[i*2],0x00);
                    if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1) 
                    uart0_OpenOrClose(Couple[i*2+1],0x00);
										uart0_VolCurrentAlarm[Couple[i*2]-1]|=0x01;
									 uart0_VolCurrentAlarm[Couple[i*2+1]-1]|=0x01;
                   } 
									  else
											
								{ uart0_VolCurrentAlarm[Couple[i*2]-1]&=0xfe;	
									uart0_VolCurrentAlarm[Couple[i*2+1]-1]&=0xfe;	
								}  
                //----------current----------
							if(ParaList[(cMotorType+i)*2+1]==0x02) 			
                 tempf=(float)(GetCuurent(Couple[i*2]-1));
							 else
								  tempf=(float)(GetCuurent(Couple[i*2]-1));  
                  Tempint=(uint16_t)(tempf);
                   Tempint/=100;
               if(Tempint>ParaList[cMaxVolt*2+2]*256+ParaList[cMaxVolt*2+3])
                  {  if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1) 
                  	 uart0_OpenOrClose(Couple[i*2+1],0x00);
                   if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1) 
                    uart0_OpenOrClose(Couple[i*2],0x00);
										uart0_VolCurrentAlarm[Couple[i*2]-1]|=0x02;
									 uart0_VolCurrentAlarm[Couple[i*2+1]-1]|=0x02;
							    }
								else
								{ uart0_VolCurrentAlarm[Couple[i*2]-1]&=0xfd;	
									uart0_VolCurrentAlarm[Couple[i*2+1]-1]&=0xfd;	
								} 
							//---------------------------	
							} //有表 comm ok
           //-------------------------------------------------------------------------------------         	
      	   else  if(((ParaList[(cLoraNodeAliveSet+Couple[i*2+1])*2+1]&0x02)==0x02)&&((uart0_Bit_List[Couple[i*2+1]-1]&0x08)==0))//have meter ,and vol and current ok
      	       {
								
								 //----------vol---------- 
								 if(ParaList[(cMotorType+i)*2+1]==0x02) 
      	   	   tempf=(float)(GetVoltage(Couple[i*2+1]-1));
								 
								 else
									 tempf=(float)(GetVoltage(Couple[i*2+1]-1));
                     Tempint=(uint16_t)(tempf);
                      Tempint/=10;
                  if(Tempint>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
                   { if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1) 
                   	uart0_OpenOrClose(Couple[i*2],0x00);
                   if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1) 
                    uart0_OpenOrClose(Couple[i*2+1],0x00);
										 	uart0_VolCurrentAlarm[Couple[i*2]-1]|=0x01;
									 uart0_VolCurrentAlarm[Couple[i*2+1]-1]|=0x01;
                   } 
									  else
										
								{ uart0_VolCurrentAlarm[Couple[i*2]-1]&=0xfe;	
									uart0_VolCurrentAlarm[Couple[i*2+1]-1]&=0xfe;	
								}  
               ////--------------------current--------------
										
						  if(ParaList[(cMotorType+i)*2+1]==0x02) 				
               tempf=(float)(GetCuurent(Couple[i*2+1]-1));
							else
								 tempf=(float)(GetCuurent(Couple[i*2+1]-1));
                  Tempint=(uint16_t)(tempf);
                   Tempint/=100;
               if(Tempint>ParaList[cMaxVolt*2+2]*256+ParaList[cMaxVolt*2+3])
                  {    if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1) 
                  	uart0_OpenOrClose(Couple[i*2+1],0x00);
                  	if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1) 
                    uart0_OpenOrClose(Couple[i*2],0x00);
											uart0_VolCurrentAlarm[Couple[i*2]-1]|=0x02;
									 uart0_VolCurrentAlarm[Couple[i*2+1]-1]|=0x02;
							    }
							 else
									 
								{ uart0_VolCurrentAlarm[Couple[i*2]-1]&=0xfd;	
									uart0_VolCurrentAlarm[Couple[i*2+1]-1]&=0xfd;	
								} 
          ////--------------------------------------------------------------
								
                  }
               
               }  //double 
						 } //valid data		
           else
					 {
               uart0_VolCurrentAlarm[Couple[i*2]-1]&=0xfd;	
									uart0_VolCurrentAlarm[Couple[i*2+1]-1]&=0xfd;	
					 }						 
						 } //active    
      	 } //for
//-----------------------------------			 
#if 1
				 for(i=0;i<10;i++)
       {				 
             if((uart0_VolCurrentAlarm[i]&0x03)!=0)
							  uart0_Bit_List[i]|=0x08;	//alarm
						 else
							  uart0_Bit_List[i]&=0xf7;   //clear alarm
			 }		
#endif			 
 }	


 



/*------------------------------------------------------------
//Function:  uart0_AddCloudBuffer(uint8_t  i)
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 将06写入命令转到对应的控制器访问命令
	  只有通过0x10的命令才是写cpu缓冲参数区，要配置控制器需要进行二次转换
		           启停的0001要转为ff00
//--------------------------------------------------------------*/ 
void uart2_AddLocalBuffer_16(uint8_t FunCode,uint8_t *send)     
     {     uint8_t i;
            uint16_t  TempCrc=0;   
              uart0_LocalComandNo=send[0];
  	       uart0_LocalComandFunction=FunCode; 
  	   	
  	   	uart0_LocalSendcomand[uart0_Localcomand_WP][0]=8;
     
       for(i=0;i<8;i++)
       uart0_LocalSendcomand[uart0_Localcomand_WP][1+i]=send[i];
      uart0_LocalSendcomand[uart0_Localcomand_WP][2]=FunCode; 
      
       TempCrc=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	uart0_LocalSendcomand[uart0_Localcomand_WP][7]=TempCrc/256;  //High crc
  	uart0_LocalSendcomand[uart0_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart0_Localcomand_WP++;
    	 if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
            }
    }       



