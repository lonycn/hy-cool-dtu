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

#define       cBufferLen_uart0     500
extern uint16_t    WritePiont_uart0;
extern uint16_t    ReadPiont_uart0;
extern unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];
extern unsigned char   SendTempBufferBak_uart0[300];
uint8_t   uart0_LoopDeviceNo;
uint8_t   uart0_loop_stage=0;

uint8_t	  uart0_CloudCount;
uint8_t	  uart0_CloudComandLen; //中间变量  用来计算最短的包长
uint8_t	  uart0_CloudComandNo;
uint8_t	  uart0_CloudComandFunction;
uint8_t   uart0_CloudComandResondWaiting;
uint8_t   uart0_CloudComandResondLen;
uint8_t   uart0_Sendcomand[5][20];
uint8_t   uart0_Cloudcomand_RP;
uint8_t   uart0_Cloudcomand_WP;

uint8_t   uart0_LocalSendcomand[5][20];
uint8_t	  uart0_LocalCount;
uint8_t	  uart0_LocalComandLen;
uint8_t	  uart0_LocalComandNo;
uint8_t	  uart0_LocalComandFunction;//2022/2/4 12:28
uint8_t   uart0_LocalComandResondWaiting;
uint8_t   uart0_LocalComandResondLen;
uint8_t   uart0_LocalComandNo;
uint8_t	  uart0_LocalComandAddr;
uint8_t   uart0_Localcomand_RP;
uint8_t   uart0_Localcomand_WP;


uint8_t uart0_Bit_List[10];
uint8_t uart0_Bit_List_bak[20];
uint8_t uart0_Para_List[280];
uint8_t Bak_uart0_Bit_List[10];
uint8_t Bak_uart0_Para_List[280];

uint8_t uart0_Var_List[320];
uint8_t uart0_VolCurrentAlarm[10];

uint8_t uart0_OrigPara_List[cContorlRegLen*20];
uint8_t uart0_OrigBitList[70];




uint8_t DeviceInterval[20];
uint8_t  uart0_Para_MapTable[14]={51,50,15,16,17,17,47,47,12,12,31,31,32,32};
uint8_t  uart0_Var_MapTable[4]={0,0,1,1};
uint8_t  uart0_Bit_MapTable[7]={20,21,52,22,8,9,10};
uint8_t uart0_floatSeq[4]={3,2,1,0};

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
每个温控器使用6个16bit变量，组成3个32位浮点数变量，每个电表使用8个16bit变量，分别为3相电流和3相电压及总功耗
每个电箱配套NTC温控器3只，变量地址紧跟电表数据之后
----------------------------------------------------------------------*/

//---------------------------------------------------------------
//Function:  uint8_t uart0_GetBufferDataRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 读取buffer数据
//----------------------------------------------------------------
uint8_t uart0_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart0[(ReadPiont_uart0+i)%cBufferLen_uart0]);
 
}
//---------------------------------------------------------------
//Function:  void uart0_IncUartRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 移动buffer数据
//----------------------------------------------------------------	
void uart0_IncUartRead(uint8_t i)	
{
	ReadPiont_uart0=(ReadPiont_uart0+i)%cBufferLen_uart0;
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
//Function:  void uart0_Master_DealWith_ModbusRespond(uint8_t Device)
//Input:    None
//Output:   None
//Description: 处理uart0的回复数据
//---------------------------------------------------------------- 
void uart0_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,Error1,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                Error=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
//-----------------------------------------------------------------                                     
                              
                              if((uart0_CloudComandResondWaiting==1)||(uart0_LocalComandResondWaiting==1))
                                 {   if(uart0_CloudComandFunction==0x01)
                                       uart0_CloudComandLen=6;
                                    else
                                       uart0_CloudComandLen=7;    
                                     
                                     if(Error>=uart0_CloudComandLen)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==uart0_CloudComandNo)&&(k==uart0_CloudComandFunction))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         
			                                        //-------------------命令 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)|| (k==0x05)) //10命令需要等待
			                     	                        {  
			                     	                   	   if(Error1>=8)//buffer 足够
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart0,6);
               	                                                             CRCAct=SendTempBufferBak_uart0[6]*256+SendTempBufferBak_uart0[7]; 
			                     	                             //----------正确的crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart0_IncUartRead(8);
			                     	                                      uart0_CloudComandResondLen=8;
			                     	                                      uart0_LoraMaster_CheckModbusMaster(uart0_CloudComandNo);
			                     	                                   }
			                     	                              //---------错误crc,下一个byte     
			                     	                            else
			                     	                              uart0_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer不够
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //其他命令03 04 
			                     	                  { 
			                     	                     //   长度足够	
			                     	                  	if(Error>=uart0_GetBufferDataRead(2)+5)//buffer 足够 01 03 20  crc
			                     	                        { for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {           uart0_CloudComandResondLen=uart0_GetBufferDataRead(2)+5;
			                     	                                             uart0_IncUartRead(uart0_CloudComandResondLen);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(uart0_CloudComandNo);
			                     	                         	
			                     	                                   }
			                     	                                          else //crc wrong
			                     	                                              uart0_IncUartRead(1); 
			                     	                                          return; 
									                                   }
								                                    else  // 04 03 buffer 不够
								                                        return;
								                                     }
						   //--END  03 04-------  
					 	    } //end 基本长度enough 处理
							else  //剩余的buffer不够形成一个寄出帧,直接退出
			                     	     	   uart0_IncUartRead(1); 
			                     	 }  // end 正确命令
						
			                if(i==Error-1)
                                           uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte
			               }   
									 }
            										
			                         
			                         
//--------------------------------------------温控器位变量---------------------				    
				  else if(uart0_loop_stage==0)    
				   { if(Error>=6)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x01))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                       if(i==Error-1)
                                                   uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte
			                  } 
			               }      
//--------------------------------------温控器控制变量---------------------------------------------------------
 else if(uart0_loop_stage==1) 
                           { if(Error>=105)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x03))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                 
			                if(i==Error-1)
                                            uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte 
			               }
			             }          
//------------------------------------------------电表-------------------------------------------------
 else if(uart0_loop_stage==2) 
                           { if(Error>=21)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
			               }
			         }  

//----------------------------------------气调库设备-----------------------------------------------------
 else if(uart0_loop_stage==3) 
                           { if(Error>=15)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+199)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+199);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
			               }
			         }       							 
//------------------------------------------温湿度sensor------------------------------------------------------
           
  else if((uart0_loop_stage>3)&& (uart0_loop_stage<=11))
                           { if(Error>=9)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==11+uart0_loop_stage-4)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(11+uart0_loop_stage-4);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
			               }
			         }   

//-----------------------------------------电表电量或者缺相------------------------------------------------------							 
 else if(uart0_loop_stage==12) 
                           { if(Error>=7)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
			               }
			         }  
//-----------------------------------------------------------------------
							 
}

//--------------------------------------------------------
//Function:  uchar CheckModbusRespond()
//Input:    None
//Output:   None
//Description: 等待modbus请求协议，包括写单元和读单元
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          地址	功能码	地址高	地址低	数量高 数量低	CRC的高位	CRC的低位
//----------------------------------------------------------
unsigned char uart0_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
	 if((SendTempBufferBak_uart0[0]>100)&&(SendTempBufferBak_uart0[0]<111))
	    DeviceInterval[SendTempBufferBak_uart0[0]-91]=0;
	else  if(SendTempBufferBak_uart0[0]<11)
		  DeviceInterval[SendTempBufferBak_uart0[0]-1]=0; 
    
           if(SendTempBufferBak_uart0[1]==cComandWriteSingleCoil)           //读单个coil
               	 	   { 
               	 	     uart0_LoraMaster_DealWithCoilWreite(Device);
               	 	     return(cRightStatus);
               	 	    }
                  if(SendTempBufferBak_uart0[1]==cComandReadCoil)           //读单个coil
               	 	   { 
               	 	     uart0_LoraMaster_DealWithCoil(Device);
               	 	     return(cRightStatus);
               	 	    }
             
//singleWrite  06-----------------------------------------------------                	 	
                else  if(SendTempBufferBak_uart0[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { 
               	 	     uart0_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart0[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   {   if(uart0_CloudComandResondWaiting==1)
		                        {
    	                          uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	                          uart0_CloudComandResondWaiting=0;
    	                          uart0_CloudCount=0;
    	                          uart0_Cloudcomand_RP++;
    	                          if(uart0_Cloudcomand_RP>=5)
    	                                 uart0_Cloudcomand_RP=0;
																 uart0_CloudComandResondWaiting=0;
                                return(cRightStatus);			 
    	                          }  
    	                         
			                   else 
												 { //20231201
			                 if( uart0_LocalComandResondWaiting==1)
			                        uart0_LocalComandResondWaiting=0;
               	 	   	      uart0_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
										 } //20231201
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart0[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart0_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart0[1]==0x10)     
               	 	   { uart0_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	  
               	 	    	
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart0[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1_uart0(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    


//------------------
 
void   uart0_LoraMaster_DealWithCoilWreite(uint8_t Device)
{
   if(uart0_CloudComandResondWaiting==1)
    {  
		 uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0; 
     UptateFlag=1;			 
    	
		 }

}	
/* ---------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求01写数据
//---------------------------------------------------------------------01----------------------------------------------------------------------------------*/
void uart0_LoraMaster_DealWithCoil(uint8_t Device)
{
  uint8_t  i=0;
	
	if((SendTempBufferBak_uart0[3+0/8]&(1<<(0%8)))!=0) //传感器状态
	{ SetStatus(Device,0);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,0);
	        
	
            }
	
	if((SendTempBufferBak_uart0[3+1/8]&(1<<(1%8)))!=0) //化霜传感器状态
	{ SetStatus(Device,1);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,1);
	        
	
}
	
	
  if((SendTempBufferBak_uart0[3+20/8]&(1<<(20%8)))!=0) //制冷状态
	{ SetStatus(Device,5);
	    i|=1;  
	}	
	else
		 ClrStatus(Device,5);
		  
	
   if((SendTempBufferBak_uart0[3+21/8]&(1<<(21%8)))!=0) //风机状态
	 { i|=2; 
		 SetStatus(Device,6);
	 }
	else
    ClrStatus(Device,6);		
	 
  if((SendTempBufferBak_uart0[3+22/8]&(1<<(22%8)))!=0) //除霜状态
	{ i|=4;
		SetStatus(Device,7);
	}
 else
    ClrStatus(Device,7);


 if((SendTempBufferBak_uart0[3+8/8]&(1<<(8%8)))!=0) //高温告警
	{
		SetStatus(Device,2);
	
     i|=16;     
	}
 else
    ClrStatus(Device,2);

if((SendTempBufferBak_uart0[3+9/8]&(1<<(9%8)))!=0) //低温告警
	{
		SetStatus(Device,3);
		i|=16;
	}
 else
    ClrStatus(Device,3);
 
if((SendTempBufferBak_uart0[3+10/8]&(1<<(10%8)))!=0) //外部告警
	{
		SetStatus(Device,4);
		i|=16;
	}
 else
    ClrStatus(Device,4); 
 
if((SendTempBufferBak_uart0[3+50/8]&(1<<(50%8)))!=0) //强制除霜
	{
		SetStatus(Device,8);
	}
 else
    ClrStatus(Device,8);  
 
if((SendTempBufferBak_uart0[3+51/8]&(1<<(51%8)))!=0) //系统开机
	{
		SetStatus(Device,9);
	}
 else
    ClrStatus(Device,9);  
  

 if((SendTempBufferBak_uart0[3+52/8]&(1<<(52%8)))!=0) //系统除霜状态
	{
		SetStatus(Device,10);
	}
 else
    ClrStatus(Device,10);  
 
  uart0_Bit_List[Device-1]&=0xC8;     //通讯状态
   uart0_Bit_List[Device-1]|=i;	   
	

	uart0_Para_List[(Device-1)*28]=0x00;
	if((SendTempBufferBak_uart0[3+51/8]&(1<<(51%8)))!=0)
                uart0_Para_List[(Device-1)*28+1]=0x01;
	else
		 uart0_Para_List[(Device-1)*28+1]=0x00;
//add  20230328
 uart0_Para_List[(Device-1)*28+2]=0x00;
	if((SendTempBufferBak_uart0[3+50/8]&(1<<(50%8)))!=0)
                uart0_Para_List[(Device-1)*28+3]=0x01;
	else
		 uart0_Para_List[(Device-1)*28+3]=0x00;


//end 	
	
	
	for(i=0;i<7;i++)
	 	uart0_OrigBitList[(Device-1)*7+i]=SendTempBufferBak_uart0[3+i];

	
	 if(uart0_CloudComandResondWaiting==1)
    {  
		 uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0;  
    	
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
void uart0_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {uint16_t  Addr,Dat;
	 uint8_t   sta;
	 union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;		 
	 
	 
	 
	 uart0_CloudCount=0;
#if  1	 
	 Addr=SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3];
	 Dat=SendTempBufferBak_uart0[4]*256+SendTempBufferBak_uart0[5];
 	 sta=SendTempBufferBak_uart0[0];
	 
	 
			for(i=0;i<14;i++)
			 { if( Addr==uart0_Para_MapTable[i])
				     break;
			 }
//---------------------- 返回的是参数表中控制器的int参数			 
      if(i<4)
			{
				uart0_Para_List[(sta-1)*28+i*2]=SendTempBufferBak_uart0[4];
				uart0_Para_List[(sta-1)*28+i*2+1]=SendTempBufferBak_uart0[5];
				
				if((i==0)||(i==1))  //Openclose
				{
					if(SendTempBufferBak_uart0[4]==0xff)
					  {
					      uart0_Para_List[(sta-1)*28+i*2]=0x00;
					     uart0_Para_List[(sta-1)*28+i*2+1]=0x01;
				          }
				}
	#if  0		  
				else  if(i==1)  //Openclose
				{
					if(SendTempBufferBak_uart0[5]==1)
					  {
					      uart0_Para_List[(sta-1)*28+i*2]=0x00;
					     uart0_Para_List[(sta-1)*28+i*2+1]^=0x01;
				          }
				}
	#endif			
			}	
//--------------------------	 返回的是参数表中控制器的float参数		
		 else	if(i<14)
		 {
         tempT= SendTempBufferBak_uart0[3+uart0_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart0[4+uart0_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)Dat;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart0_Para_List[(sta-1)*28+(i-4)*4+8]=temp.vchar[uart0_floatSeq[0]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+9]=temp.vchar[uart0_floatSeq[1]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+10]=temp.vchar[uart0_floatSeq[2]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+11]=temp.vchar[uart0_floatSeq[3]];
		 }

//-----------------------------------------返回的是参数表中没有的参数，云端的请求数据，将返回到数据返回给云端，			 
    if(uart0_CloudComandResondWaiting==1)
    {  
	//	 uart0_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0;  
			 UptateFlag=1;	
    	
		 }
		
//----------------------------本地附加到的命令
   if(uart0_LocalComandResondWaiting==1)
               	 	    {  uart0_LocalComandResondWaiting=0;
    	                       if(uart0_Localcomand_RP!=uart0_Localcomand_WP)
    	                      { uart0_Localcomand_RP++;
    	                        if(uart0_Localcomand_RP>=5)
    	                            uart0_Localcomand_RP=0;
    	                            uart0_LocalComandResondWaiting=0;  
    	                       }   
    	                       
    	                    }	
//----------------------------------------------只要有写入成功的状态，就需要进行一次控制的参数刷新--------
    	  for(i=0;i<8;i++)
       uart0_LocalSendcomand[uart0_Localcomand_WP][1+i]=SendTempBufferBak_uart0[i];
       if((SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3]==50)||(SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3]==51))   
          {  uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x01; 
            uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x34;
          }   
        else
          {  uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x03;
              uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x30;  
          }
           uart0_LocalSendcomand[uart0_Localcomand_WP][3]=0; 
           uart0_LocalSendcomand[uart0_Localcomand_WP][4]=0; 
            uart0_LocalSendcomand[uart0_Localcomand_WP][5]=0;
					 uart0_LocalSendcomand[uart0_Localcomand_WP][1]=SendTempBufferBak_uart0[0];
					
            uart0_LocalSendcomand[uart0_Localcomand_WP][0] =8;
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
  
#endif	 
 
 }
 
uint8_t  Random10=0;
//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:端口号0-4
            u8 Device:轮询的终端设备号
//Output:   None
//Description:处理终端设备回复的信息
//-----------------------------------处理03读取回来的数据--------------------------------------------------03------------------------------------------------------*/
void uart0_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { union
     {float  vf;
      uint8_t vchar[4];
     }temp;
   
    uint16_t  tempT;	
    int16_t   tempInt,Tempint;
		 
		 
		  uint8_t  i,j; 

	  uint32_t  Tempint1;
 float  tempf; 
	 uint16_t  Tempuint,TempCrc;	


 if(Device>=101)
 {
	 
	 //---------------------单相--------------------------
	 j=ParaList[cMetreType*2+1];
	 if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     { 
       //----------------------
			 if(j==0)  //单相直接式
			 { 
	//20231114			 
		if(SendTempBufferBak_uart0[2]==0x08) 	
		{	 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[3+i];
  	      Tempint=(int16_t)(temp.vf*10);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempuint%256; //a相电压
				 
			    for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint=(int16_t)(temp.vf*100);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempuint%256; //a相电压	 
				 
	#if  0		 
			 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=SendTempBufferBak_uart0[9];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=SendTempBufferBak_uart0[10]; //a相电流
			  
				 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //功率
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //功率	
#endif
        }
		else  if(SendTempBufferBak_uart0[2]==0x04) 	
   {
				 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint1=(uint32_t)(temp.vf*100);
  	      //Tempuint = (uint16_t)Tempint;
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=Tempint1/65535/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=Tempint1/65535%256;
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=(Tempint1%65535)/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=(Tempint1%65535)%256;
				 
				 
       }
	 //20231114	
		 }
	 }
 }	 
else
{	
    for(i=0;i<SendTempBufferBak_uart0[2];i++)
		 {
			  uart0_OrigPara_List[(Device-1)*cContorlRegLen*2+i]= SendTempBufferBak_uart0[3+i];
     }
		 
    tempT= SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4];
		Random10=0;
		 if(Random10>100)
			 Random10=0; 
		 tempT+=Random10; //20250212
		 
		 tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
		temp.vf/=10; 
    for(i=0;i<4;i++) //回风温度
      uart0_Var_List[(Device-1)*12+i]=temp.vchar[uart0_floatSeq[i]];
   
		 tempT= SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6];  
     tempT+=Random10; //20250212
		 tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    temp.vf/=10; 
		 for(i=0;i<4;i++)  //化霜温度 
      uart0_Var_List[(Device-1)*12+8+i]=temp.vchar[uart0_floatSeq[i]];   
    
              //4个uint16 参数,系统控制和化霜操作已经在位操作中进行处理了，这个忽略这两个参数的处理
   for(i=2;i<4;i++) 
    { uart0_Para_List[(Device-1)*28+i*2]=SendTempBufferBak_uart0[3+uart0_Para_MapTable[i]*2];
      uart0_Para_List[(Device-1)*28+i*2+1]=SendTempBufferBak_uart0[3+uart0_Para_MapTable[i]*2+1];
		#if  0
			if(i==1)
			{
				uart0_Para_List[(Device-1)*28+i*2+1]^=1;  //20220216
			}
		#endif	
   }
             
  for(i=4;i<9;i++)   //5个uint32 参数
    { 
			tempT= SendTempBufferBak_uart0[3+uart0_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart0[4+uart0_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart0_Para_List[(Device-1)*28+(i-4)*4+8]=temp.vchar[uart0_floatSeq[0]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+9]=temp.vchar[uart0_floatSeq[1]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+10]=temp.vchar[uart0_floatSeq[2]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+11]=temp.vchar[uart0_floatSeq[3]];
    }
 }  
}
/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据
//----------------------------- 处理04读取到数据值电表---------------------------------------------------------------------------------------------04---------*/

void uart0_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i,j; 
 uint16_t  Tempint;
	  uint32_t  Tempint1;
 float  tempf; 
	 uint16_t  Tempuint,TempCrc;	
 union
     {float  vf;
      uint8_t vchar[4];
     }temp;   
          
  	    
     if(Device>=200)//气调库
		 {  i=3;
			  uart0_Var_List[(Device-200)*4+cTempPos*2+0+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+1+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //temp
	      i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+2+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;//256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+3+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //humi
			 i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+4+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+5+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //o2
	      i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+6+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+7+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //ch3oh
			  i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+8+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+9+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //co2
	     
		 }
	 
	else if(Device>=101)
 {
	 
	 //---------------------单相--------------------------
	 j=ParaList[cMetreType*2+1];
	 if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     { 
  #if  0    
			 //----------------------
			 if(j==0)  //单相直接式
			 { 
	//20231114			 
		if(SendTempBufferBak_uart0[2]==0x08) 	
		{	 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[3+i];
  	      Tempint=(int16_t)(temp.vf*10);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempuint%256; //a相电压
				 
			    for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint=(int16_t)(temp.vf*100);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempuint%256; //a相电压	 
				 
	#if  0		 
			 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=SendTempBufferBak_uart0[9];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=SendTempBufferBak_uart0[10]; //a相电流
			  
				 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //功率
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //功率	
#endif
        }
		else  if(SendTempBufferBak_uart0[2]==0x04) 	
   {
				 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint1=(uint32_t)(temp.vf*100);
  	      //Tempuint = (uint16_t)Tempint;
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=Tempint1/65535/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=Tempint1/65535%256;
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=(Tempint1%65535)/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=(Tempint1%65535)%256;
				 
				 
       }
	 //20231114	
		 }
			 //-----------------------
      else   //单相互感式
	#endif			
			
			{if(SendTempBufferBak_uart0[2]==0x3e)  //返回长度为62byte，读取电流电压）
        { uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=SendTempBufferBak_uart0[3];
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=SendTempBufferBak_uart0[4]; //a相电压
			   
					Tempint1=SendTempBufferBak_uart0[9]*256+SendTempBufferBak_uart0[10];
			 //  Tempint1= Tempint1/10;
				uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint1/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint1%256; //a相电流 
					
					 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //功率
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //功率
       }				
			
			
     }
		//--------------------------	
	 }
		 
		 //---------------------三相-------------------------	 
   else if((ParaList[cMotorType*2+1]==0x03)||(ParaList[cMotorType*2+1]==0x04))
     { Tempint=0;
     //-----------直接式三相
			 //---------------------电压--------    
	if(j==0)	
	{	
if(SendTempBufferBak_uart0[2]==0x3e) 
{for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[3+i*2]*256+SendTempBufferBak_uart0[3+i*2+1];
     Tempint=Tempint/3;
    	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempint%256; //电压
        
 //-----------------------------------电流---------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[9+i*2]*256+SendTempBufferBak_uart0[9+i*2+1];
     	Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint%256; //电流
//add  20240228  读取相序的状态
        
         uart0_Var_List[cPhaseDir*2+1]=SendTempBufferBak_uart0[17];					
		      uart0_Var_List[cPhase*2]=SendTempBufferBak_uart0[17];	
		//----------------------------------总功率---------------------    
        
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //功率
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //功率
	}
else if(SendTempBufferBak_uart0[2]==0x02) 
{   uart0_Var_List[cPhaseLost*2+1]=SendTempBufferBak_uart0[4];	
     uart0_Var_List[cPhase*2+1]=SendTempBufferBak_uart0[4];
}	
}
else   //------------互感式三相
{
   Tempint=0;
 //---------------------电压--------    
	if(SendTempBufferBak_uart0[2]==0x3e)  //返回长度为62byte，读取电流电压）
	{for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[3+i*4+2]*256+SendTempBufferBak_uart0[3+i*4+3];
     Tempint/=3;
    	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempint%256; //电压
        
 //-----------------------------------电流---------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	 {
           Tempint1=(SendTempBufferBak_uart0[15+i*4]*256+SendTempBufferBak_uart0[16+i*4])*65536+SendTempBufferBak_uart0[17+i*4]*256+SendTempBufferBak_uart0[18+i*4];
			   Tempint1=Tempint1/10;
				 
				 
				 Tempint+=(uint16_t)Tempint1;
     	
				 
			 } 
				 Tempint=Tempint/3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint%256; //电流

}	
else  if(SendTempBufferBak_uart0[2]==0x04)//返回长度为4byte，读取电功率）
   {
	    //----------------------------------总功率---------------------    
        
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[3];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[4]; //功率
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[5];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[6]; //功率
    }
  }
      
 }
}		 
//---------温湿度探头数据-------
else
{
	#if  0
	      uart0_Var_List[(Device-11)*4+cTempPos*2]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+1]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])%256; //temp
	      uart0_Var_List[(Device-11)*4+cTempPos*2+2]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+3]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])%256; //humi
	#endif
	      uart0_Var_List[(Device-11)*4+cTempPos*2+2]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+3]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])%256; //temp
	      uart0_Var_List[(Device-11)*4+cTempPos*2+0]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+1]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])%256; //humi
	      RequireTime[Device-11]=0;
}
//---------------------------------

}



//----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//-----------------------------------10-----------------------------
void uart0_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
 
  
  
  
 }
