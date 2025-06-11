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

#define       cBufferLen_uart3     200
extern uint16_t    WritePiont_uart3;
extern uint16_t    ReadPiont_uart3;
extern unsigned char   ReceiveBuff_uart3[cBufferLen_uart3];
extern unsigned char   SendTempBufferBak_uart3[200];
uint8_t   uart3_LoopDeviceNo;
#define  uart3_cModbusLen    10
#define  uart3_cPackageLen    101
uint8_t   uart3_ModbusWritePointer;
uint8_t   uart3_ModbusBuffer[uart3_cModbusLen][uart3_cPackageLen];
uint8_t   uart3_ModbusReadPointer;


void uart3_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart3_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart3_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart3_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart3_LoraMaster_SendModbusPackage(uint8_t Device);
void uart3_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart3_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);




uint8_t uart3_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart3[(ReadPiont_uart3+i)%cBufferLen_uart3]);
 
}	
void uart3_IncUartRead(uint8_t i)	
{
	ReadPiont_uart3=(ReadPiont_uart3+i)%cBufferLen_uart3;
}			







void uart3_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len)
  {
     uint8_t  SendBuf[10],j=0;
     uint16_t	CRCTemp;
		
	 ReadPiont_uart3=0;
  WritePiont_uart3=0;
  	SendBuf[j++]=Sta;
  	SendBuf[j++]=comand;
  	SendBuf[j++]=Addr/256;
  	SendBuf[j++]=Addr%256;
  	SendBuf[j++]=Len/256;
  	SendBuf[j++]=Len%256;
  	CRCTemp=CRC(SendBuf,j);
  	SendBuf[j++]=CRCTemp/256;
  	SendBuf[j++]=CRCTemp%256;
  	SendDataToBus1_uart3(SendBuf,j);
  }	


  
void uart3_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                     Error=(WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3;
				    if(Error>=7)   //如果有足够的buffer可以去处理
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart3_GetBufferDataRead(i);
			                    k=uart3_GetBufferDataRead(i+1);
			                    if((j==Device)&&((k==0x03)||(k==0x04)||(k==0x06)||(k==0x10))) 
			                      {
			                                      uart3_IncUartRead(i); //定位要处理buffer的首地址	  
			                                       Error=(WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3;
			                                       if(Error>=7)
			                                        { 
			                                        
			                                        //-------------------命令 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)) //10命令需要等待
			                     	                        {  
			                     	                   	   if(Error>=8)//buffer 足够
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart3[k]=uart3_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart3,6);
               	                                                             CRCAct=SendTempBufferBak_uart3[6]*256+SendTempBufferBak_uart3[7]; 
			                     	                             //----------正确的crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart3_IncUartRead(8);
			                     	                                      uart3_LoraMaster_CheckModbusMaster(Device);
			                     	                                   }
			                     	                              //---------错误crc,下一个byte     
			                     	                            else
			                     	                              uart3_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer不够
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //其他命令03 04 
			                     	                  { 
			                     	                     //   长度足够	
			                     	                  	if(Error>=uart3_GetBufferDataRead(2)+5)//buffer 足够 01 03 20  crc
			                     	                        { for(k=0;k<uart3_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart3[k]=uart3_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart3,uart3_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart3[uart3_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart3[uart3_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart3_IncUartRead(uart3_GetBufferDataRead(2)+5);
			                     	                                             uart3_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                            }
			                     	                                          else //crc wrong
			                     	                                              uart3_IncUartRead(1); 
			                     	                                          return; 
									 }
								       else  // 04 03 buffer 不够
								          return;
								   }
						   //--END  03 04-------  
					 	    } //end 基本长度enough 处理
							else  //剩余的buffer不够形成一个寄出帧,直接退出
			                     	     	   return;
			                     	 }  // end 正确命令
						 else  //没有找到device+command的组合
			                     	     {  	  	
			                     	      //  IncUartRead(1,UartFlag); //定位到后一个字节
			                     	        
			                     	     }   
			                       
			                    }  //END for  
			                if(i==Error)
                                           uart3_IncUartRead(Error); //所有的数据都不包含两个头byte
			               }   
			                    
			           
			            
			  					
		   }        
		                         
	



		    
//--------------------------------------------------------
//Function:  uchar CheckModbusRespond()
//Input:    None
//Output:   None
//Description: 等待modbus请求协议，包括写单元和读单元
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          地址	功能码	地址高	地址低	数量高 数量低	CRC的高位	CRC的低位
//----------------------------------------------------------
unsigned char uart3_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
     
             
//singleWrite  06-----------------------------------------------------                	 	
                  if(SendTempBufferBak_uart3[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { 
               	 	     uart3_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart3[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   { 
               	 	   	uart3_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart3[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart3_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart3[1]==0x10)     
               	 	   { uart3_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart3[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1_uart0(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    
    


//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求06写数据
//----------------------------------------------------------------
void uart3_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {
 	
  	   	
 }
 

//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:端口号0-4
            u8 Device:轮询的终端设备号
//Output:   None
//Description:处理终端设备回复的信息
//-----------------------------------处理03读取回来的数据-----------------------------*/
void uart3_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { 
     if(Device==0x01)   //co2  ppm*10
   {
   	 VarList[6]=(SendTempBufferBak_uart3[3]*256+SendTempBufferBak_uart3[4])*10/256;
   	 VarList[7]=(SendTempBufferBak_uart3[3]*256+SendTempBufferBak_uart3[4])*10%256;	
   }
 if(Device==0x04)  //c2h4 ppm*10
  {
  	 VarList[8]=SendTempBufferBak_uart3[3];
   	 VarList[9]=SendTempBufferBak_uart3[4];
  }	
  }
/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据

从二级lora读取上来的数据进行接析
根据接收到的数据得到node数据的个数
然后每按照16个单元进行接析
将每个node的数据存入到Modbuff中(ModBuffPointer++)
如果有禁用的端口,将禁用端口的modbuff空间进行借用,待用 
DisableTab[5]:
获取设备的最大值
//----------------------------- 处理04读取到数据值-----------------------------------*/

void uart3_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i,Dis[6];; 
   for(i=0;i<SendTempBufferBak_uart3[2]+5;i++)
	  
	 uart3_ModbusBuffer[uart3_ModbusWritePointer][i]=SendTempBufferBak_uart3[i];
	   uart3_ModbusWritePointer++;
	   if(uart3_ModbusWritePointer>uart3_cModbusLen)
		     uart3_ModbusWritePointer=0;
		 
		if(uart3_ModbusWritePointer==uart3_ModbusReadPointer)
		{
			uart3_ModbusReadPointer++;
			if(uart3_ModbusReadPointer>uart3_cModbusLen)
		  uart3_ModbusReadPointer=0;
		}
		LCD_string(1,1,"Receive:        ");
 
 	  Dis[0]=SendTempBufferBak_uart3[2]%100/10+0x30;
 	  Dis[1]=SendTempBufferBak_uart3[2]%10+0X30;
 	  Dis[2]=0;
    LCD_string(1,6,Dis);
}
 //----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//-----------------------------------10-----------------------------
void uart3_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
  
 }
 
 
 
void Uart3_ModbusLoopInitla()
{
	 SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]);
	 uart3_LoopDeviceNo=AddrStart;
}

void  Uart3_NextDevice() 
{
	 uart3_LoopDeviceNo++;
	 if(uart3_LoopDeviceNo>AddrStart+AddrLen)
	 { uart3_LoopDeviceNo=AddrStart;
		 if(ParaList[cNodeLoopInterval*2+1]*AddrLen<=ParaList[cLoraNodeInteral*2+1]+ParaList[cLoraNodeInteral*2]*256)
		 {
			 SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]);
		 } 
		else
    	SetSecond1s_ModbusLoop_Intial(ParaList[cLoraNodeInteral*2+1]+ParaList[cLoraNodeInteral*2]*256-ParaList[cNodeLoopInterval*2+1]*AddrLen);		
	 }

}

void  Uart3_ModbusLoop()
{uint8_t   Dis[6];   
	if(GetSecond1s_ModbusLoopStatus()==1)
	{
		SetSecond1s_ModbusLoop_Intial(ParaList[cNodeLoopInterval*2+1]);
		uart3_ModbusRequest(uart3_LoopDeviceNo,4,27,48);
		LCD_string(1,1,"Loop:");
 
 	  Dis[0]=uart3_LoopDeviceNo%100/10+0x30;
 	  Dis[1]=uart3_LoopDeviceNo%10+0X30;
 	  Dis[2]=0;
    LCD_string(1,6,Dis);
    Uart3_NextDevice();
		
	}	
	uart3_Master_DealWith_ModbusRespond(uart3_LoopDeviceNo);
}	 
 
 void uart3_SendPowerSignal()
	{
   uint8_t SendBuffer[120],i=0;
		
  if(	uart3_ModbusReadPointer!=	uart3_ModbusWritePointer)
	{for(i=0;i<uart3_cPackageLen;i++)
       SendBuffer[i]=uart3_ModbusBuffer[uart3_ModbusWritePointer][i];
       SendDataToBus1_uart3(SendBuffer,i);
		   uart3_ModbusWritePointer++;
		   if(uart3_ModbusReadPointer>uart3_cModbusLen)
		     uart3_ModbusReadPointer=0;
		 }	
		
	}		
