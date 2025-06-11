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

#define       cBufferLen_uart4     500
extern uint16_t    WritePiont_uart4;
extern uint16_t    ReadPiont_uart4;
extern unsigned char   ReceiveBuff_uart4[cBufferLen_uart4];
extern unsigned char   SendTempBufferBak_uart4[300];
uint8_t   uart4_LoopDeviceNo;
uint8_t   uart4_loop_stage=0;

uint8_t	  uart4_CloudCount;
uint8_t	  uart4_CloudComandLen; //中间变量  用来计算最短的包长
uint8_t	  uart4_CloudComandNo;
uint8_t	  uart4_CloudComandFunction;
uint8_t   uart4_CloudComandResondWaiting;
uint8_t   uart4_CloudComandResondLen;
uint8_t   uart4_Sendcomand[5][20];
uint8_t   uart4_Cloudcomand_RP;
uint8_t   uart4_Cloudcomand_WP;

uint8_t   uart4_LocalSendcomand[5][20];
uint8_t	  uart4_LocalCount;
uint8_t	  uart4_LocalComandLen;
uint8_t	  uart4_LocalComandNo;
uint8_t	  uart4_LocalComandFunction;//2022/2/4 12:28
uint8_t   uart4_LocalComandResondWaiting;
uint8_t   uart4_LocalComandResondLen;
uint8_t   uart4_LocalComandNo;
uint8_t	  uart4_LocalComandAddr;
uint8_t   uart4_Localcomand_RP;
uint8_t   uart4_Localcomand_WP;


uint8_t uart4_Bit_List[10];
uint8_t uart4_Bit_List_bak[20];
uint8_t uart4_Para_List[280];
uint8_t Bak_uart4_Bit_List[10];
uint8_t Bak_uart4_Para_List[280];

uint8_t uart4_Var_List[320];
uint8_t uart4_VolCurrentAlarm[10];

uint8_t uart4_OrigPara_List[cContorlRegLen*20];
uint8_t uart4_OrigBitList[70];




uint8_t DeviceInterval[20];
uint8_t  uart4_Para_MapTable[14]={51,22,15,16,17,17,47,47,12,12,31,31,32,32};
uint8_t  uart4_Var_MapTable[4]={0,0,1,1};
uint8_t  uart4_Bit_MapTable[7]={20,21,52,22,8,9,10};
uint8_t uart4_floatSeq[4]={3,2,1,0};

/*-----------------------------------------------------
通讯及协议解析
--------------------------------------------------------*/
uint8_t uart4_GetBufferDataRead(uint8_t i);
void uart4_IncUartRead(uint8_t i);
void uart4_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart4_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart4_LoraMaster_DealWithCoil(uint8_t Device);
void uart4_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);
void  uart4_LoraMaster_DealWithCoilWreite(uint8_t Device);
void   Timer_uartInterval();
void  GetCommunicationStatus();
/*-----------------------------------------------------
轮询指令
--------------------------------------------------------*/
void uart4_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len);
void uart4_ModbusLoopInitla();
void  uart4_NextDevice(); 
void  uart4_ModbusLoop();

/*-----------------------------------------------------
云端通讯指令插入
--------------------------------------------------------*/
void UpdateChange();
void uart4_SendCloudComand();
void uart4_SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat);
void uart4_SendCoolDat();
void uart4_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len);
void uart4_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send);  
/*-----------------------------------------------------
本地通讯指令插入
--------------------------------------------------------*/
void uart4_SendLocalComand() ;
void uart4_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send);     
void uart4_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send) ;  
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
void uart4_VolCurrentOverLoadAlarm();
/*-----------------------------------------------------
启停操作
--------------------------------------------------------*/
void uart4_OpenOrClose(uint8_t  no,uint8_t  Op);
/*---------------------------------------------------------------------
使用说明：
1：初始化程序中调用uart4_ModbusLoopInitla()
2：主程序中调用uart4_ModbusLoop()
功能：在uart程序轮询温控器和电表，一个温控制配一个电表，支持10个温控器和10个电表
温控制器编号从1-10，电表编号从101-110
起始编号和温控器数量在寄存器中进行设置，默认为从1开始，共10组，此参数不需要进行调整
使用使用该温控器和对应的电表，通过激活控制器来设置
每个温控器有8个状态位，有2个32位的温度变量，参数有4个16位参数和5个32位参数

在轮训温控器的过程中，如果有云端的控制数据下发，则在完成一个查询命令后，执行云端的控制命令，返回云端的回复数据
每个温控器使用6个16bit变量，组成3个32位浮点数变量，每个电表使用7个16bit变量，分别为3相电流和3相电压及总功率
每个电箱配套NTC温控器2只，变量地址紧跟电表数据之后
----------------------------------------------------------------------*/

//---------------------------------------------------------------
//Function:  uint8_t uart4_GetBufferDataRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 读取buffer数据
//----------------------------------------------------------------
uint8_t uart4_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart4[(ReadPiont_uart4+i)%cBufferLen_uart4]);
 
}
//---------------------------------------------------------------
//Function:  void uart4_IncUartRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 移动buffer数据
//----------------------------------------------------------------	
void uart4_IncUartRead(uint8_t i)	
{
	ReadPiont_uart4=(ReadPiont_uart4+i)%cBufferLen_uart4;
}			


void   Timer_uartInterval()
{
	uint8_t  i;
	for(i=0;i<10;i++)
	{
       if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)==1)  //active
	       DeviceInterval[i]++;
			 else
				 DeviceInterval[i]=0; 
			 
			 if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x02)==2)  //active
				 DeviceInterval[i+10]++;
			 else
				 DeviceInterval[i+10]=0;
} 
}

//---------------------------------------------------------------
//Function:  void uart4_Master_DealWith_ModbusRespond(uint8_t Device)
//Input:    None
//Output:   None
//Description: 处理uart4的回复数据
//---------------------------------------------------------------- 
void uart4_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,Error1,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                Error=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
//-----------------------------------------------------------------                                     
                              
                              if((uart4_CloudComandResondWaiting==1)||(uart4_LocalComandResondWaiting==1))
                                 {   if(uart4_CloudComandFunction==0x01)
                                       uart4_CloudComandLen=6;
                                    else
                                       uart4_CloudComandLen=7;    
                                     
                                     if(Error>=uart4_CloudComandLen)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==uart4_CloudComandNo)&&(k==uart4_CloudComandFunction))
			                      {  uart4_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         
			                                        //-------------------命令 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)|| (k==0x05)) //10命令需要等待
			                     	                        {  
			                     	                   	   if(Error1>=8)//buffer 足够
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart4,6);
               	                                                             CRCAct=SendTempBufferBak_uart4[6]*256+SendTempBufferBak_uart4[7]; 
			                     	                             //----------正确的crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart4_IncUartRead(8);
			                     	                                      uart4_CloudComandResondLen=8;
			                     	                                      uart4_LoraMaster_CheckModbusMaster(uart4_CloudComandNo);
			                     	                                   }
			                     	                              //---------错误crc,下一个byte     
			                     	                            else
			                     	                              uart4_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer不够
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //其他命令03 04 
			                     	                  { 
			                     	                     //   长度足够	
			                     	                  	if(Error>=uart4_GetBufferDataRead(2)+5)//buffer 足够 01 03 20  crc
			                     	                        { for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {           uart4_CloudComandResondLen=uart4_GetBufferDataRead(2)+5;
			                     	                                             uart4_IncUartRead(uart4_CloudComandResondLen);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(uart4_CloudComandNo);
			                     	                         	
			                     	                                   }
			                     	                                          else //crc wrong
			                     	                                              uart4_IncUartRead(1); 
			                     	                                          return; 
									                                   }
								                                    else  // 04 03 buffer 不够
								                                        return;
								                                     }
						   //--END  03 04-------  
					 	    } //end 基本长度enough 处理
							else  //剩余的buffer不够形成一个寄出帧,直接退出
			                     	     	   uart4_IncUartRead(1); 
			                     	 }  // end 正确命令
						
			                if(i==Error-1)
                                           uart4_IncUartRead(Error-1); //所有的数据都不包含两个头byte
			               }   
									 }
            										
			                         
			                         
//-----------------------------------------------------------------				    
				  else if(uart4_loop_stage==0)    
				   { if(Error>=6)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x01))
			                      {  uart4_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                       if(i==Error-1)
                                                   uart4_IncUartRead(Error-1); //所有的数据都不包含两个头byte
			                  } 
			               }      
//-----------------------------------------------------------------------------------------------
                        else if(uart4_loop_stage==1) 
                           { if(Error>=105)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x03))
			                      {  uart4_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                 
			                if(i==Error-1)
                                            uart4_IncUartRead(Error-1); //所有的数据都不包含两个头byte 
			               }
			             }          
//-----------------------------------------------------------------------------------------------
                        else if(uart4_loop_stage==2) 
                           { if(Error>=21)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart4_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart4_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
			               }
			         }       
//------------------------------------------------------------------------------------------------
                
}

//--------------------------------------------------------
//Function:  uchar CheckModbusRespond()
//Input:    None
//Output:   None
//Description: 等待modbus请求协议，包括写单元和读单元
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          地址	功能码	地址高	地址低	数量高 数量低	CRC的高位	CRC的低位
//----------------------------------------------------------
unsigned char uart4_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
	 if(SendTempBufferBak_uart4[0]>100)
	    DeviceInterval[SendTempBufferBak_uart4[0]-91]=0;
	 else
		  DeviceInterval[SendTempBufferBak_uart4[0]-1]=0; 
    
           if(SendTempBufferBak_uart4[1]==cComandWriteSingleCoil)           //读单个coil
               	 	   { 
               	 	     uart4_LoraMaster_DealWithCoilWreite(Device);
               	 	     return(cRightStatus);
               	 	    }
                  if(SendTempBufferBak_uart4[1]==cComandReadCoil)           //读单个coil
               	 	   { 
               	 	     uart4_LoraMaster_DealWithCoil(Device);
               	 	     return(cRightStatus);
               	 	    }
             
//singleWrite  06-----------------------------------------------------                	 	
                else  if(SendTempBufferBak_uart4[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { 
               	 	     uart4_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart4[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   {   if(uart4_CloudComandResondWaiting==1)
		                        {
    	                          uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	                          uart4_CloudComandResondWaiting=0;
    	                          uart4_CloudCount=0;
    	                          uart4_Cloudcomand_RP++;
    	                          if(uart4_Cloudcomand_RP>=5)
    	                                 uart4_Cloudcomand_RP=0;
																 uart4_CloudComandResondWaiting=0;
                                return(cRightStatus);			 
    	                          }  
    	                         
			                   else 
			                 if( uart4_LocalComandResondWaiting==1)
			                        uart4_LocalComandResondWaiting=0;
               	 	   	      uart4_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart4[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart4_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart4[1]==0x10)     
               	 	   { uart4_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	  
               	 	    	
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart4[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    


//------------------
 
void   uart4_LoraMaster_DealWithCoilWreite(uint8_t Device)
{
   if(uart4_CloudComandResondWaiting==1)
    {  
		 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0; 
     UptateFlag=1;			 
    	
		 }

}	
/* ---------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求01写数据
//---------------------------------------------------------------------01----------------------------------------------------------------------------------*/
void uart4_LoraMaster_DealWithCoil(uint8_t Device)
{
  uint8_t  i=0;
	
	if((SendTempBufferBak_uart4[3+0/8]&(1<<(0%8)))!=0) //传感器状态
	{ SetStatus(Device,0);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,0);
	        
	
            }
	
	if((SendTempBufferBak_uart4[3+1/8]&(1<<(1%8)))!=0) //化霜传感器状态
	{ SetStatus(Device,1);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,1);
	        
	
}
	
	
  if((SendTempBufferBak_uart4[3+20/8]&(1<<(20%8)))!=0) //制冷状态
	{ SetStatus(Device,5);
	    i|=1;  
	}	
	else
		 ClrStatus(Device,5);
		  
	
   if((SendTempBufferBak_uart4[3+21/8]&(1<<(21%8)))!=0) //风机状态
	 { i|=2; 
		 SetStatus(Device,6);
	 }
	else
    ClrStatus(Device,6);		
	 
  if((SendTempBufferBak_uart4[3+22/8]&(1<<(22%8)))!=0) //除霜状态
	{ i|=4;
		SetStatus(Device,7);
	}
 else
    ClrStatus(Device,7);


 if((SendTempBufferBak_uart4[3+8/8]&(1<<(8%8)))!=0) //高温告警
	{
		SetStatus(Device,2);
	
     i|=16;     
	}
 else
    ClrStatus(Device,2);

if((SendTempBufferBak_uart4[3+9/8]&(1<<(9%8)))!=0) //低温告警
	{
		SetStatus(Device,3);
		i|=16;
	}
 else
    ClrStatus(Device,3);
 
if((SendTempBufferBak_uart4[3+10/8]&(1<<(10%8)))!=0) //外部告警
	{
		SetStatus(Device,4);
		i|=16;
	}
 else
    ClrStatus(Device,4); 
 
if((SendTempBufferBak_uart4[3+50/8]&(1<<(50%8)))!=0) //强制除霜
	{
		SetStatus(Device,8);
	}
 else
    ClrStatus(Device,8);  
 
if((SendTempBufferBak_uart4[3+51/8]&(1<<(51%8)))!=0) //系统开机
	{
		SetStatus(Device,9);
	}
 else
    ClrStatus(Device,9);  
  

 if((SendTempBufferBak_uart4[3+52/8]&(1<<(52%8)))!=0) //系统除霜状态
	{
		SetStatus(Device,10);
	}
 else
    ClrStatus(Device,10);  
 
  uart4_Bit_List[Device-1]&=0xC8;     //通讯状态
   uart4_Bit_List[Device-1]|=i;	   
	

	uart4_Para_List[(Device-1)*28]=0x00;
	if((SendTempBufferBak_uart4[3+51/8]&(1<<(51%8)))!=0)
                uart4_Para_List[(Device-1)*28+1]=0x01;
	else
		 uart4_Para_List[(Device-1)*28+1]=0x00;
	
	for(i=0;i<7;i++)
	 	uart4_OrigBitList[(Device-1)*7+i]=SendTempBufferBak_uart4[3+i];

	
	 if(uart4_CloudComandResondWaiting==1)
    {  
		 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0;  
    	
		 }
}






/*---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求06写数据
回复的数据：云端直接访问温控器  4000以上
            云端通过本地buffer访问温控器 本地：0-279  温控器：0-54 
            本地的操作：轮值切换，故障切换等操作
回复的数据如果是变量表中有的，则按照uint和float格式分别写入对应的变量表中
回复的数据如果是变量中没有的，不写入数据
如果int变量中有0xff00的，需要调整为1
如果为云端写入数据返回，处理远端数据列表，则返回接收到到的数据，但是数据需要进行处理整合，
如果为本地写入数据，处理本地写入列表
//--------------------------------06------------------------------------------------------------------------------06-----------------------------------------------*/
void uart4_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {uint16_t  Addr,Dat;
	 uint8_t   sta;
	 union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;		 
	 
	 
	 
	 uart4_CloudCount=0;
#if  1	 
	 Addr=SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3];
	 Dat=SendTempBufferBak_uart4[4]*256+SendTempBufferBak_uart4[5];
 	 sta=SendTempBufferBak_uart4[0];
	 
	 
			for(i=0;i<14;i++)
			 { if( Addr==uart4_Para_MapTable[i])
				     break;
			 }
//---------------------- 返回的是参数表中控制器的int参数			 
      if(i<4)
			{
				uart4_Para_List[(sta-1)*28+i*2]=SendTempBufferBak_uart4[4];
				uart4_Para_List[(sta-1)*28+i*2+1]=SendTempBufferBak_uart4[5];
				
				if(i==0)  //Openclose
				{
					if(SendTempBufferBak_uart4[4]==0xff)
					  {
					      uart4_Para_List[(sta-1)*28+i*2]=0x00;
					     uart4_Para_List[(sta-1)*28+i*2+1]=0x01;
				          }
				}
			    else  if(i==1)  //Openclose
				{
					if(SendTempBufferBak_uart4[5]==1)
					  {
					      uart4_Para_List[(sta-1)*28+i*2]=0x00;
					     uart4_Para_List[(sta-1)*28+i*2+1]^=0x01;
				          }
				}
				
			}	
//--------------------------	 返回的是参数表中控制器的float参数		
		 else	if(i<14)
		 {
         tempT= SendTempBufferBak_uart4[3+uart4_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart4[4+uart4_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)Dat;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart4_Para_List[(sta-1)*28+(i-4)*4+8]=temp.vchar[uart4_floatSeq[0]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+9]=temp.vchar[uart4_floatSeq[1]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+10]=temp.vchar[uart4_floatSeq[2]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+11]=temp.vchar[uart4_floatSeq[3]];
		 }

//-----------------------------------------返回的是参数表中没有的参数，云端的请求数据，将返回到数据返回给云端，			 
    if(uart4_CloudComandResondWaiting==1)
    {  
	//	 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0;  
			 UptateFlag=1;	
    	
		 }
		
//----------------------------本地附加到的命令
   if(uart4_LocalComandResondWaiting==1)
               	 	    {  uart4_LocalComandResondWaiting=0;
    	                       if(uart4_Localcomand_RP!=uart4_Localcomand_WP)
    	                      { uart4_Localcomand_RP++;
    	                        if(uart4_Localcomand_RP>=5)
    	                            uart4_Localcomand_RP=0;
    	                            uart4_LocalComandResondWaiting=0;  
    	                       }   
    	                       
    	                    }	
//----------------------------------------------只要有写入成功的状态，就需要进行一次控制的参数刷新--------
    	  for(i=0;i<8;i++)
       uart4_LocalSendcomand[uart4_Localcomand_WP][1+i]=SendTempBufferBak_uart4[i];
       if((SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3]==50)||(SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3]==51))   
          {  uart4_LocalSendcomand[uart4_Localcomand_WP][2]=0x01; 
            uart4_LocalSendcomand[uart4_Localcomand_WP][6]=0x34;
          }   
        else
          {  uart4_LocalSendcomand[uart4_Localcomand_WP][2]=0x03;
              uart4_LocalSendcomand[uart4_Localcomand_WP][6]=0x30;  
          }
           uart4_LocalSendcomand[uart4_Localcomand_WP][3]=0; 
           uart4_LocalSendcomand[uart4_Localcomand_WP][4]=0; 
            uart4_LocalSendcomand[uart4_Localcomand_WP][5]=0;
					 uart4_LocalSendcomand[uart4_Localcomand_WP][1]=SendTempBufferBak_uart4[0];
					
            uart4_LocalSendcomand[uart4_Localcomand_WP][0] =8;
       tempT=CRC(&uart4_LocalSendcomand[uart4_Localcomand_WP][1],6);
  	uart4_LocalSendcomand[uart4_Localcomand_WP][7]=tempT/256;  //High crc
  	uart4_LocalSendcomand[uart4_Localcomand_WP][8]=tempT%256;  //Low crc
         uart4_Localcomand_WP++;
    	 if(uart4_Localcomand_WP>=5)
    	    uart4_Localcomand_WP=0;
    	  if(uart4_Localcomand_WP==uart4_Localcomand_RP)   
    	  {  uart4_Localcomand_RP++;
    	     if(uart4_Localcomand_RP>=5)
    	       uart4_Localcomand_RP=0;
        }
  
#endif	 
 
 }
 

//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:端口号0-4
            u8 Device:轮询的终端设备号
//Output:   None
//Description:处理终端设备回复的信息
//-----------------------------------处理03读取回来的数据--------------------------------------------------03------------------------------------------------------*/
void uart4_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;

    for(i=0;i<SendTempBufferBak_uart4[2];i++)
		 {
			  uart4_OrigPara_List[(Device-1)*cContorlRegLen*2+i]= SendTempBufferBak_uart4[3+i];
     }
		 
    tempT= SendTempBufferBak_uart4[3]*256+SendTempBufferBak_uart4[4];
		tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
		temp.vf/=10; 
    for(i=0;i<4;i++) //回风温度
      uart4_Var_List[(Device-1)*12+i]=temp.vchar[uart4_floatSeq[i]];
   
		 tempT= SendTempBufferBak_uart4[5]*256+SendTempBufferBak_uart4[6];  
   tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    temp.vf/=10; 
		 for(i=0;i<4;i++)  //化霜温度 
      uart4_Var_List[(Device-1)*12+8+i]=temp.vchar[uart4_floatSeq[i]];   
    
              //4个uint16 参数
   for(i=1;i<4;i++) 
    { uart4_Para_List[(Device-1)*28+i*2]=SendTempBufferBak_uart4[3+uart4_Para_MapTable[i]*2];
      uart4_Para_List[(Device-1)*28+i*2+1]=SendTempBufferBak_uart4[3+uart4_Para_MapTable[i]*2+1];
			if(i==1)
			{
				uart4_Para_List[(Device-1)*28+i*2+1]^=1;  //20220216
			}
   }
             
  for(i=4;i<9;i++)   //5个uint32 参数
    { 
			tempT= SendTempBufferBak_uart4[3+uart4_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart4[4+uart4_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart4_Para_List[(Device-1)*28+(i-4)*4+8]=temp.vchar[uart4_floatSeq[0]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+9]=temp.vchar[uart4_floatSeq[1]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+10]=temp.vchar[uart4_floatSeq[2]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+11]=temp.vchar[uart4_floatSeq[3]];
    }
 }  

/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据
//----------------------------- 处理04读取到数据值电表---------------------------------------------------------------------------------------------04---------*/

void uart4_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i; 
 uint16_t  Tempint;
 float  tempf; 
   if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     {  uart4_Var_List[(Device-101)*cVarLenByte+120]=SendTempBufferBak_uart4[3];
        uart4_Var_List[(Device-101)*cVarLenByte+121]=SendTempBufferBak_uart4[4]; //a相电压
        uart4_Var_List[(Device-101)*cVarLenByte+122]=SendTempBufferBak_uart4[9];
        uart4_Var_List[(Device-101)*cVarLenByte+123]=SendTempBufferBak_uart4[10]; //a相电流
        uart4_Var_List[(Device-101)*cVarLenByte+124]=SendTempBufferBak_uart4[63];
        uart4_Var_List[(Device-101)*cVarLenByte+125]=SendTempBufferBak_uart4[64]; //功率
			  uart4_Var_List[(Device-101)*cVarLenByte+126]=SendTempBufferBak_uart4[65];
        uart4_Var_List[(Device-101)*cVarLenByte+127]=SendTempBufferBak_uart4[66]; //功率
     }
   else if((ParaList[cMotorType*2+1]==0x03)||(ParaList[cMotorType*2+1]==0x04))
     { Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart4[3+i*2]*256+SendTempBufferBak_uart4[3+i*2+1];
     Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart4_Var_List[(Device-101)*cVarLenByte+120]=Tempint/256;
        uart4_Var_List[(Device-101)*cVarLenByte+121]=Tempint%256; //电压
        
 //--------------------------------------------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart4[9+i*2]*256+SendTempBufferBak_uart4[9+i*2+1];
     	Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart4_Var_List[(Device-101)*cVarLenByte+122]=Tempint/256;
        uart4_Var_List[(Device-101)*cVarLenByte+123]=Tempint%256; //电流
        
        
        uart4_Var_List[(Device-101)*cVarLenByte+124]=SendTempBufferBak_uart4[17];
        uart4_Var_List[(Device-101)*cVarLenByte+125]=SendTempBufferBak_uart4[18]; //功耗
     	
     }
      
}
 
//----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//-----------------------------------10-----------------------------
void uart4_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
 
  
  
  
 }
