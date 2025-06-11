#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

#define       cBufferLen_uart2     500
extern uint16_t    WritePiont_uart2;
extern uint16_t    ReadPiont_uart2;
extern unsigned char   ReceiveBuff_uart2[cBufferLen_uart2];
extern unsigned char   SendTempBufferBak_uart2[300];
uint8_t   uart2_LoopDeviceNo;
uint8_t   uart2_loop_stage=0;

uint8_t	  uart2_CloudCount;
uint8_t	  uart2_CloudComandLen; //中间变量  用来计算最短的包长
uint8_t	  uart2_CloudComandNo;
uint8_t	  uart2_CloudComandFunction;
uint8_t   uart2_CloudComandResondWaiting;
uint8_t   uart2_CloudComandResondLen;
uint8_t   uart2_Sendcomand[5][20];
uint8_t   uart2_Cloudcomand_RP;
uint8_t   uart2_Cloudcomand_WP;

uint8_t   uart2_LocalSendcomand[5][20];
uint8_t	  uart2_LocalCount;
uint8_t	  uart2_LocalComandLen;
uint8_t	  uart2_LocalComandNo;
uint8_t	  uart2_LocalComandFunction;//2022/2/4 12:28
uint8_t   uart2_LocalComandResondWaiting;
uint8_t   uart2_LocalComandResondLen;
uint8_t   uart2_LocalComandNo;
uint8_t	  uart2_LocalComandAddr;
uint8_t   uart2_Localcomand_RP;
uint8_t   uart2_Localcomand_WP;


uint8_t Bit_List[10];
uint8_t Para_List[280];
uint8_t Var_List[300];
uint8_t VolCurrentAlarm[10];
uint8_t  Para_MapTable[14]={51,22,15,16,17,17,47,47,12,12,31,31,32,32};
uint8_t  Var_MapTable[4]={0,0,1,1};
uint8_t  Bit_MapTable[7]={20,21,52,22,8,9,10};
uint8_t floatSeq[4]={0,1,2,3};
void uart2_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart2_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart2_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart2_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart2_LoraMaster_SendModbusPackage(uint8_t Device);
void uart2_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart2_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);
void uart2_LoraMaster_DealWithCoil(uint8_t Device);



void SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat);
void SendCoolDat();
void ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len);
/*---------------------------------------------------------------------
使用说明：
1：初始化程序中调用uart2_ModbusLoopInitla()
2：主程序中调用uart2_ModbusLoop()
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
//Function:  uint8_t uart2_GetBufferDataRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 读取buffer数据
//----------------------------------------------------------------
uint8_t uart2_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart2[(ReadPiont_uart2+i)%cBufferLen_uart2]);
 
}
//---------------------------------------------------------------
//Function:  void uart2_IncUartRead(uint8_t i)
//Input:    None
//Output:   None
//Description: 移动buffer数据
//----------------------------------------------------------------	
void uart2_IncUartRead(uint8_t i)	
{
	ReadPiont_uart2=(ReadPiont_uart2+i)%cBufferLen_uart2;
}			

//---------------------------------------------------------------
//Function:  void uart2_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len)
//Input:    None
//Output:   None
//Description: uart2轮询命令
//----------------------------------------------------------------
void uart2_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len)
  {
     uint8_t  SendBuf[10],j=0;
     uint16_t	CRCTemp;
		
	 ReadPiont_uart2=0;
         WritePiont_uart2=0;
  	SendBuf[j++]=Sta;
  	SendBuf[j++]=comand;
  	SendBuf[j++]=Addr/256;
  	SendBuf[j++]=Addr%256;
  	SendBuf[j++]=Len/256;
  	SendBuf[j++]=Len%256;
  	CRCTemp=CRC(SendBuf,j);
  	SendBuf[j++]=CRCTemp/256;
  	SendBuf[j++]=CRCTemp%256;
  	SendDataToBus1_uart2(SendBuf,j);
  }	


  
void SendCloudComand() 
{
 SendDataToBus1_uart2(&uart2_Sendcomand[uart2_Cloudcomand_RP][1],uart2_Sendcomand[uart2_Cloudcomand_RP][0]);
 uart2_CloudComandResondWaiting=1;  	
}	 
  	
void SendLocalComand() 
{
 SendDataToBus1_uart2(&uart2_LocalSendcomand[uart2_Localcomand_RP][1],uart2_LocalSendcomand[uart2_Localcomand_RP][0]);		
uart2_LocalComandResondWaiting=1;	
}
//---------------------------------------------------------------
//Function:  void uart2_Master_DealWith_ModbusRespond(uint8_t Device)
//Input:    None
//Output:   None
//Description: 处理uart2的回复数据
//---------------------------------------------------------------- 
void uart2_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,Error1,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                Error=(WritePiont_uart2+cBufferLen_uart2-ReadPiont_uart2)%cBufferLen_uart2;
//-----------------------------------------------------------------                                     
                              
                              if((uart2_CloudComandResondWaiting==1)||(uart2_LocalComandResondWaiting==1))
                                 {   if(uart2_CloudComandFunction==0x05)
                                       uart2_CloudComandLen=6;
                                    else
                                       uart2_CloudComandLen=7;    
                                     
                                     if(Error>=uart2_CloudComandLen)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart2_GetBufferDataRead(i);
			                    k=uart2_GetBufferDataRead(i+1);
			                    
			                    if((j==uart2_CloudComandNo)&&(k==uart2_CloudComandFunction))
			                      {  uart2_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart2+cBufferLen_uart2-ReadPiont_uart2)%cBufferLen_uart2;
			                         
			                                        //-------------------命令 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)|| (k==0x05)) //10命令需要等待
			                     	                        {  
			                     	                   	   if(Error1>=8)//buffer 足够
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart2[k]=uart2_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart2,6);
               	                                                             CRCAct=SendTempBufferBak_uart2[6]*256+SendTempBufferBak_uart2[7]; 
			                     	                             //----------正确的crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart2_IncUartRead(8);
			                     	                                      uart2_CloudComandResondLen=8;
			                     	                                      uart2_LoraMaster_CheckModbusMaster(uart2_CloudComandNo);
			                     	                                   }
			                     	                              //---------错误crc,下一个byte     
			                     	                            else
			                     	                              uart2_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer不够
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //其他命令03 04 
			                     	                  { 
			                     	                     //   长度足够	
			                     	                  	if(Error>=uart2_GetBufferDataRead(2)+5)//buffer 足够 01 03 20  crc
			                     	                        { for(k=0;k<uart2_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart2[k]=uart2_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart2,uart2_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {           uart2_CloudComandResondLen=uart2_GetBufferDataRead(2)+5;
			                     	                                             uart2_IncUartRead(uart2_CloudComandResondLen);
			                     	                                             uart2_LoraMaster_CheckModbusMaster(uart2_CloudComandNo);
			                     	                         	
			                     	                                   }
			                     	                                          else //crc wrong
			                     	                                              uart2_IncUartRead(1); 
			                     	                                          return; 
									                                   }
								                                    else  // 04 03 buffer 不够
								                                        return;
								                                     }
						   //--END  03 04-------  
					 	    } //end 基本长度enough 处理
							else  //剩余的buffer不够形成一个寄出帧,直接退出
			                     	     	   uart2_IncUartRead(1); 
			                     	 }  // end 正确命令
						
			                if(i==Error)
                                           uart2_IncUartRead(Error); //所有的数据都不包含两个头byte
			               }   
									 }
            										
			                         
			                         
//-----------------------------------------------------------------				    
				  else if(uart2_loop_stage==0)    
				   { if(Error>=6)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart2_GetBufferDataRead(i);
			                    k=uart2_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x01))
			                      {  uart2_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart2+cBufferLen_uart2-ReadPiont_uart2)%cBufferLen_uart2;
			                         if(Error1>=uart2_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart2_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart2[k]=uart2_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart2,uart2_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart2_IncUartRead(uart2_GetBufferDataRead(2)+5);
			                     	                                             uart2_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart2_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart2_IncUartRead(1); 
			                      }
			                       if(i==Error)
                                                   uart2_IncUartRead(Error-1); //所有的数据都不包含两个头byte
			                  } 
			               }      
//-----------------------------------------------------------------------------------------------
                        else if(uart2_loop_stage==1) 
                           { if(Error>=145)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart2_GetBufferDataRead(i);
			                    k=uart2_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x03))
			                      {  uart2_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart2+cBufferLen_uart2-ReadPiont_uart2)%cBufferLen_uart2;
			                         if(Error1>=uart2_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart2_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart2[k]=uart2_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart2,uart2_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart2_IncUartRead(uart2_GetBufferDataRead(2)+5);
			                     	                                             uart2_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart2_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart2_IncUartRead(1); 
			                      }
			                 
			                if(i==Error)
                                            uart2_IncUartRead(Error-1); //所有的数据都不包含两个头byte 
			               }
			             }          
//-----------------------------------------------------------------------------------------------
                        else if(uart2_loop_stage==2) 
                           { if(Error>=21)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart2_GetBufferDataRead(i);
			                    k=uart2_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart2_IncUartRead(i); //定位要处理buffer的首地址	  
			                         Error1=(WritePiont_uart2+cBufferLen_uart2-ReadPiont_uart2)%cBufferLen_uart2;
			                         if(Error1>=uart2_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart2_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart2[k]=uart2_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart2,uart2_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart2[uart2_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart2_IncUartRead(uart2_GetBufferDataRead(2)+5);
			                     	                                             uart2_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart2_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
						}
					     else
					         uart2_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error)
                                              uart2_IncUartRead(Error-1); //所有的数据都不包含两个头byte  
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
unsigned char uart2_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
    if(uart2_CloudComandResondWaiting==1)
    {
    	ReturnCloudData(SendTempBufferBak_uart2,uart2_CloudComandResondLen);
    	uart2_CloudComandResondWaiting=0;
    	uart2_CloudCount=0;
    	if(uart2_Cloudcomand_RP!=uart2_Cloudcomand_WP)
    	{ uart2_Cloudcomand_RP++;
    	 if(uart2_Cloudcomand_RP>=5)
    	    uart2_Cloudcomand_RP=0;
    	 uart2_CloudComandResondWaiting=0;  
    	 }   
        return(cRightStatus);
    }

    
  else
    { 
                 
                  if(SendTempBufferBak_uart2[1]==cComandReadCoil)           //写单个寄存器06
               	 	   { 
               	 	     uart2_LoraMaster_DealWithCoil(Device);
               	 	     return(cRightStatus);
               	 	    }
             
//singleWrite  06-----------------------------------------------------                	 	
                  if(SendTempBufferBak_uart2[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { 
               	 	     uart2_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart2[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   { 
               	 	   	uart2_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart2[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart2_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart2[1]==0x10)     
               	 	   { uart2_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	   if(uart2_LocalComandResondWaiting==1)
               	 	    {  uart2_LocalComandResondWaiting=0;
    	                       if(uart2_Localcomand_RP!=uart2_Localcomand_WP)
    	                      { uart2_Localcomand_RP++;
    	                        if(uart2_Localcomand_RP>=5)
    	                            uart2_Localcomand_RP=0;
    	                            uart2_LocalComandResondWaiting=0;  
    	                       }   
    	                       
    	                    }	
               	 	    	
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart2[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    
}
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求01写数据
//----------------------------------------------------------------
void uart2_LoraMaster_DealWithCoil(uint8_t Device)
{
  uint8_t  i=0;
  if(SendTempBufferBak_uart2[3+20/8]&(1<<(20%8))!=0)
    i+=1;
   if(SendTempBufferBak_uart2[3+21/8]&(1<<(21%8))!=0)
    i+=2;   
  if(SendTempBufferBak_uart2[3+22/8]&(1<<(22%8))!=0)
    i+=4;
  if((SendTempBufferBak_uart2[4]&0x07)!=0) 
    i+=16;     
    
    Bit_List[Device-1]&=0x80;
   Bit_List[Device-1]|=i;	
}


//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求06写数据
//----------------------------------------------------------------
void uart2_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {
 	
  	   	
 }
 

//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:端口号0-4
            u8 Device:轮询的终端设备号
//Output:   None
//Description:处理终端设备回复的信息
//-----------------------------------处理03读取回来的数据-----------------------------*/
void uart2_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i; 
    temp.vf= (float)(SendTempBufferBak_uart2[3]*256+SendTempBufferBak_uart2[4]);
    for(i=0;i<4;i++) //回风温度
      Var_List[(Device-1)*12+i]=temp.vchar[floatSeq[i]];
     
   temp.vf= (float)(SendTempBufferBak_uart2[4]*256+SendTempBufferBak_uart2[5]);
    for(i=0;i<4;i++)  //化霜温度 
      Var_List[(Device-1)*12+8+i]=temp.vchar[floatSeq[i]];   
    
              //4个uint16 参数
   for(i=0;i<4;i++) 
    { Para_List[(Device-1)*28+i*2]=SendTempBufferBak_uart2[3+Para_MapTable[i]*2];
      Para_List[(Device-1)*28+i*2+1]=SendTempBufferBak_uart2[3+Para_MapTable[i]*2+1];
   }
             
  for(i=4;i<9;i++)   //5个uint32 参数
    { 
    	temp.vf= (float)(SendTempBufferBak_uart2[3+Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart2[4+Para_MapTable[i*2-4]*2]);
    	Para_List[(Device-1)*28+(i-4)*4]=temp.vchar[floatSeq[0]];
    	Para_List[(Device-1)*28+(i-4)*4+1]=temp.vchar[floatSeq[1]];
    	Para_List[(Device-1)*28+(i-4)*4+2]=temp.vchar[floatSeq[2]];
    	Para_List[(Device-1)*28+(i-4)*4+3]=temp.vchar[floatSeq[3]];
    }
 }  

/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据
//----------------------------- 处理04读取到数据值-----------------------------------*/

void uart2_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i; 
   for(i=0;i<12;i++)
    Var_List[(Device-101)*18+120+i]=SendTempBufferBak_uart2[3+i];
  
  Var_List[(Device-101)*18+120+12]=SendTempBufferBak_uart2[3+14];   
  Var_List[(Device-101)*18+120+13]=SendTempBufferBak_uart2[3+15];      
}
 
//----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//-----------------------------------10-----------------------------
void uart2_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
 
  
  
  
 }
//------------------------------------------------------------
//Function:  uart2_ModbusLoopInitla()
//Input:    None
//Output:   None
//Description: uart2初始化
//--------------------------------------------------------------
void uart2_ModbusLoopInitla()
{
	 SetSecond1s_ModbusLoop_Intial(2);
	 uart2_LoopDeviceNo=AddrStart;
}



 //------------------------------------------------------------
//Function:  void  uart2_NextDevice() 
//Input:    None
//Output:   None
//Description: 切换到下一个电箱
//--------------------------------------------------------------
void  uart2_NextDevice() 
{
   while(1)
      {	
	 uart2_LoopDeviceNo++;
	 if(AddrLen>=10)
	   AddrLen=10;
	 if(uart2_LoopDeviceNo>=AddrStart+AddrLen)
	 { uart2_LoopDeviceNo=AddrStart;
				
	 }
      SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]); 
   //add 2022/2/3 11:13
     if(ParaList[cLoraNodeAliveSet*2+1]==1)  //如果使用到该控制器
         break;	 
   }
 }



//------------------------------------------------------------
//Function:  void  uart2_ModbusLoop()
//Input:    None
//Output:   None
//Description: void  uart2_轮询电箱及处理返回数据
//--------------------------------------------------------------
void  uart2_ModbusLoop()
{uint8_t  Dis[6],j=0;   
		
	if(GetSecond1s_ModbusLoopStatus()==1)
	{
		
//-------------------云端的查询命令，最多查询3次，3次没有返回数据，不需要返回云端数据	
	 if(uart2_Localcomand_RP!=uart2_Localcomand_WP)
	   { 	
	     uart2_LocalCount++;
	     if(uart2_LocalCount>=3)
	     {
	      uart2_Localcomand_RP++;
	      if(uart2_Localcomand_RP>=5)
	         uart2_Localcomand_RP=0; 	
	     }
	    else
	     SendLocalComand();
	     
	  }		
		
	
		
//-------------------云端的查询命令，最多查询3次，3次没有返回数据，放弃该条命令		
	 if(uart2_Cloudcomand_RP!=uart2_Cloudcomand_WP)
	   { 	
	     uart2_CloudCount++;
	     if(uart2_CloudCount>=3)
	     {
	       uart2_Cloudcomand_RP++; 
	       if(uart2_Cloudcomand_RP>=5)
	        uart2_Cloudcomand_RP=0; 	
	     }
	    else
	     SendCloudComand();
	     
	  }
//------------------正常轮询设备	  
	else  	
	  {	 if(uart2_loop_stage>=3)
		  {uart2_NextDevice();
		   uart2_loop_stage=0;
		  } 
	         else
	       uart2_loop_stage++;
	        switch(uart2_loop_stage)
	     {
	     	case 0:  uart2_ModbusRequest(uart2_LoopDeviceNo,1,0,53);break;  
	     	case 1:  uart2_ModbusRequest(uart2_LoopDeviceNo,3,0,50);break;
	     	case 2:  uart2_ModbusRequest(uart2_LoopDeviceNo+100,4,0,8);break;
	     	
	     	default:break;
	      }
	 }
//-------------------------------	  
   }    	
	uart2_Master_DealWith_ModbusRespond(uart2_LoopDeviceNo);
 }	 



/*-----------------------------------------------------------------------------------------------------
 以上为uart2从温控器获取数据后的处理
 以下为uart0将读取的状态数据和命令返回数据返回到云端
  -------------------------------------------------------------------------------------------------------*/
 void SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat)
{
  uint8_t SendBuffer[160];
  uint8_t i=0,j;
  uint16_t CRCTemp;
  
  SendBuffer[i++] =0xaa;
  SendBuffer[i++] =0x55;	
  SendBuffer[i++] =0x00;
  if(FunctionCode==0x01)
    SendBuffer[i++] =0x01;
   else if(FunctionCode==0x02)
    SendBuffer[i++] =0x02;  	
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
  	
   SendBuffer[i]=Len/8;
   if(Len%8!=0)
     SendBuffer[i]++;
   i++;
 }
else
 SendBuffer[i++] =Len*2;

 for(j=0;j< SendBuffer[8];j++)
   SendBuffer[i++]=SendDat[j];
CRCTemp=CRC(SendBuffer,j);
SendBuffer[i++]=CRCTemp/256;
SendBuffer[i++]=CRCTemp%256;
SendDataToBus1(SendBuffer,i);
} 

void SendCoolDat()
{
int8_t SendBuf[140];
  uint8_t i=0,j;
  uint16_t CRCTemp;

for(i=0;i<10;i++)
   SendBuf[i]=Bit_List[i];
SendControlData(1,2,1,80,SendBuf) ;
  
for(i=0;i<140;i++)
   SendBuf[i]=Para_List[i];
SendControlData(1,3,1,70,SendBuf) ;  
  	
for(i=0;i<140;i++)
   SendBuf[i]=Para_List[i+140];
SendControlData(1,3,71,70,SendBuf) ; 

	
for(i=0;i<120;i++)
   SendBuf[i]=Var_List[i];
SendControlData(1,4,1,60,SendBuf) ; 	
	
}


void ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len)
 {
 SendDataToBus1(ReturnBuff,Len);	
 	
 }	
 
//------------------------------------------------------------
//Function:  uart2_AddCloudBuffer(uint8_t  i)
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 将06写入命令转到对应的控制器访问命令
//-------------------------------------------------------------- 
void uart2_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send)  
   {  
  
   uint16_t  TempCrc=0; 
		 
		 uart2_CloudComandNo=i/54+1;
      uart2_CloudComandFunction=0x06;
    
       uart2_Sendcomand[uart2_Cloudcomand_WP][0]=8;
       uart2_Sendcomand[uart2_Cloudcomand_WP][1]=uart2_CloudComandNo;
       uart2_Sendcomand[uart2_Cloudcomand_WP][2]=FunCode;
       uart2_Sendcomand[uart2_Cloudcomand_WP][3]=(i%54)/256;
       uart2_Sendcomand[uart2_Cloudcomand_WP][4]=(i%54)%256;
       uart2_Sendcomand[uart2_Cloudcomand_WP][5]=send[4];
       uart2_Sendcomand[uart2_Cloudcomand_WP][6]=send[5];
       TempCrc=CRC(uart2_Sendcomand[uart2_Cloudcomand_WP],6);
  	   uart2_Sendcomand[uart2_Cloudcomand_WP][7]=TempCrc/256;  //High crc
  	   uart2_Sendcomand[uart2_Cloudcomand_WP][8]=TempCrc%256;  //Low crc
         uart2_Cloudcomand_WP++;
    	 if(uart2_Cloudcomand_WP>=5)
    	    uart2_Cloudcomand_WP=0;
    	  if(uart2_Cloudcomand_WP==uart2_Cloudcomand_RP)   
    	  {  uart2_Cloudcomand_RP++;
    	     if(uart2_Cloudcomand_RP>=5)
    	       uart2_Cloudcomand_RP=0;
    	 }
   } 	
   
   
   
//------------------------------------------------------------
//Function:  uart2_AddCloudBuffer(uint8_t  i)
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 将06写入命令转到对应的控制器访问命令
//-------------------------------------------------------------- 
void uart2_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send)     
     {  
        uint16_t  TempCrc=0;   
       uart2_LocalComandNo=Temp/14+1;
  	       uart2_LocalComandFunction=FunCode; 
  	   	Para_List[TempCrc*2]=send[7];
  	   	Para_List[TempCrc*2+1]=send[8];
  	   	uart2_LocalSendcomand[uart2_Localcomand_WP][0]=8;
     
       uart2_LocalSendcomand[uart2_Localcomand_WP][1]=uart2_LocalComandNo;
       uart2_LocalSendcomand[uart2_Localcomand_WP][2]=FunCode;
       uart2_LocalSendcomand[uart2_Localcomand_WP][3]=(Para_MapTable[Temp%14])/256;
       uart2_LocalSendcomand[uart2_Localcomand_WP][4]=(Para_MapTable[TempCrc%14])%256;
       uart2_LocalSendcomand[uart2_Localcomand_WP][5]=send[7];
       uart2_LocalSendcomand[uart2_Localcomand_WP][6]=send[8];
       TempCrc=CRC(uart2_LocalSendcomand[uart2_Localcomand_WP],6);
  	uart2_LocalSendcomand[uart2_Localcomand_WP][7]=TempCrc/256;  //High crc
  	uart2_LocalSendcomand[uart2_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart2_Localcomand_WP++;
    	 if(uart2_Localcomand_WP>=5)
    	    uart2_Localcomand_WP=0;
    	  if(uart2_Localcomand_WP==uart2_Localcomand_RP)   
    	  {  uart2_Localcomand_RP++;
    	     if(uart2_Localcomand_RP>=5)
    	       uart2_Localcomand_RP=0;
            }
    }   
    

//------------------------------------------------------------
//Function:  uart2_AddCloudBuffer(uint8_t  i)
//Input:    (uint8_t  i:访问的地址-4000
//Output:   None
//Description: 将06写入命令转到对应的控制器访问命令
//-------------------------------------------------------------- 
void uart2_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send)    
{  uint8_t  i;
	 int16_t  Tempint;
   uint16_t  Tempuint,TempCrc;	
 union
     {float  vf;
      uint8_t vchar[4];
     }temp;   
           uart2_LocalComandNo=Temp/14+1;
  	    uart2_LocalComandFunction=0x06; 
  	     for(i=0;i<4;i++)
  	      temp.vchar[i]=send[7+i];
  	    Tempint=(int16_t)temp.vf;
  	    Tempuint = (uint16_t)Tempint;
  	    uart2_LocalSendcomand[uart2_Localcomand_WP][0]=8;
     
       uart2_LocalSendcomand[uart2_Localcomand_WP][1]=uart2_LocalComandNo;
       uart2_LocalSendcomand[uart2_Localcomand_WP][2]=0x06;
       uart2_LocalSendcomand[uart2_Localcomand_WP][3]=(Para_MapTable[Temp%14])/256;
       uart2_LocalSendcomand[uart2_Localcomand_WP][4]=(Para_MapTable[Temp%14])%256;
       uart2_LocalSendcomand[uart2_Localcomand_WP][5]=Tempuint/256;
       uart2_LocalSendcomand[uart2_Localcomand_WP][6]=Tempuint%256;
       TempCrc=CRC(uart2_LocalSendcomand[uart2_Localcomand_WP],6);
  	uart2_LocalSendcomand[uart2_Localcomand_WP][7]=TempCrc/256;  //High crc
  	uart2_LocalSendcomand[uart2_Localcomand_WP][8]=TempCrc%256;  //Low crc
         uart2_Localcomand_WP++;
    	 if(uart2_Localcomand_WP>=5)
    	    uart2_Localcomand_WP=0;
    	  if(uart2_Localcomand_WP==uart2_Localcomand_RP)   
    	  {  uart2_Localcomand_RP++;
    	     if(uart2_Localcomand_RP>=5)
    	       uart2_Localcomand_RP=0;
  	   }
    }	   

		
		
		
		
		
void OpenOrClose(uint8_t  no,uint8_t  Op)
  {
    
        uint16_t  TempCrc=0; 
		 
		 uart2_CloudComandNo=no;
      uart2_CloudComandFunction=0x06;
    
       uart2_Sendcomand[uart2_Cloudcomand_WP][0]=8;
       uart2_Sendcomand[uart2_Cloudcomand_WP][1]=uart2_CloudComandNo;
       uart2_Sendcomand[uart2_Cloudcomand_WP][2]=uart2_CloudComandFunction;
       uart2_Sendcomand[uart2_Cloudcomand_WP][3]=0;
       uart2_Sendcomand[uart2_Cloudcomand_WP][4]=51;
       uart2_Sendcomand[uart2_Cloudcomand_WP][5]=255*Op;
       uart2_Sendcomand[uart2_Cloudcomand_WP][6]=0;
       TempCrc=CRC(uart2_Sendcomand[uart2_Cloudcomand_WP],6);
  	   uart2_Sendcomand[uart2_Cloudcomand_WP][7]=TempCrc/256;  //High crc
  	   uart2_Sendcomand[uart2_Cloudcomand_WP][8]=TempCrc%256;  //Low crc
         uart2_Cloudcomand_WP++;
    	 if(uart2_Cloudcomand_WP>=5)
    	    uart2_Cloudcomand_WP=0;
    	  if(uart2_Cloudcomand_WP==uart2_Cloudcomand_RP)   
    	  {  uart2_Cloudcomand_RP++;
    	     if(uart2_Cloudcomand_RP>=5)
    	       uart2_Cloudcomand_RP=0;
    	 }
   }


void VolCurrentOverLoadAlarm()
 {
    uint8_t  i;
   for(i=0;i<10;i++)
    {
       if(ParaList[(cLoraNodeOffset+i*3)*2+1]==1)  //active
         {
           if(Var_List[(60+i*2)*2]*256+Var_List[(60+i*2)*2+1]*256>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
               VolCurrentAlarm[i]|=0x01;
           else
              VolCurrentAlarm[i]&=0xfe;
          
          if(Var_List[(61+i*2)*2]*256+Var_List[(61+i*2)*2+1]*256>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
               VolCurrentAlarm[i]|=0x02;
           else
              VolCurrentAlarm[i]&=0xfd; 
              
           if(Var_List[(62+i*2)*2]*256+Var_List[(62+i*2)*2+1]*256>ParaList[cMaxVolt*2]*256+ParaList[cMaxVolt*2+1])
               VolCurrentAlarm[i]|=0x04;
           else
              VolCurrentAlarm[i]&=0xfb; 
              
          if(Var_List[(63+i*2)*2]*256+Var_List[(62+i*2)*2+1]*256>ParaList[(cMaxVolt+1)*2]*256+ParaList[(cMaxVolt+1)*2+1])
               VolCurrentAlarm[i]|=0x08;
           else
              VolCurrentAlarm[i]&=0xf7;     
          
          if(Var_List[(63+i*2)*2]*256+Var_List[(63+i*2)*2+1]*256>ParaList[(cMaxVolt+1)*2]*256+ParaList[(cMaxVolt+1)*2+1])
               VolCurrentAlarm[i]|=0x10;
           else
              VolCurrentAlarm[i]&=0xef;   
              
         if(Var_List[(65+i*2)*2]*256+Var_List[(65+i*2)*2+1]*256>ParaList[(cMaxVolt+1)*2]*256+ParaList[(cMaxVolt+1)*2+1])
               VolCurrentAlarm[i]|=0x20;
           else
              VolCurrentAlarm[i]&=0xdf;               
         }         
     }
    for(i=0;i<10;i++)
    {
       if(ParaList[(cLoraNodeOffset+i*3)*2+1]==1)  //active
         if( (VolCurrentAlarm[i]&0x3f)!=0x00)
           OpenOrClose(i+1,0x00);
   }  			
 }		 