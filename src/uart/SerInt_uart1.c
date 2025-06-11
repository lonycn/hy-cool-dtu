#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

#define       cBufferLen_uart1     200
uint16_t    WritePiont_uart1;
uint16_t    ReadPiont_uart1;
unsigned char   ReceiveBuff_uart1[cBufferLen_uart1];
unsigned char   SendTempBufferBak_uart1[200];
unsigned char   DataLen_uart1;




#define       cBufferLen_uart0     200
extern uint16_t    WritePiont_uart0;
extern uint16_t    ReadPiont_uart0;
extern unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];

uint8_t   LoraConfigstatus;
uint8_t   LoraConfigok;
#define  cLoraIdle   0
#define cGetData     1

void DealWithBoardCast_uart1(unsigned char  *pp);
void DealUartBuff_uart1();
/*=======================================================================
//时钟：11.0592M
//baudrate:9600bps
//baudrate generate:timer1
//count :bandrate = 1÷((0x100-0xfd）×12*32÷11.0592)us＝1÷104us=9.6k
//======================================================================*/
//--------------------------------------------------------------------------------------------
//Function: void SendDataToBus(unsigned char *pp,unsigned char length)
//Input:    *pp,SendLength
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   :
//Description：PB4/PB5  RX/TX  与lora模块相连
//--------------------------------------------------------------------------------------------
void UART1_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg();
    CLK->APBCLK |= CLK_APBCLK_UART1_EN;
    SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB4_MFP_Msk | SYS_PB_L_MFP_PB5_MFP_Msk);
    SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB4_MFP_UART1_RX | SYS_PB_L_MFP_PB5_MFP_UART1_TX);
    /* Lock protected registers */
     
    SYS_LockReg();
    
    UART_Open(UART1, 9600);
    UART_SetTimeoutCnt(UART1, 40);
	   
  //  UART_SET_RX_FIFO_INTTRGLV(uart1, UART_TLCTL_RFITL_8BYTES); /*使能接收FIFO 阀值中断，和接收超时中断*/ 
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IE_Msk | UART_IER_RTO_IE_Msk)); 
    NVIC_EnableIRQ(UART1_IRQn);
   // uart1->CTL |= 0x40;     /* RX DMA enable */
  //  GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
  //  PB13=0;
  WritePiont_uart1=0;
	ReadPiont_uart1=0;
}


void UART1_Init1()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg();
    CLK->APBCLK |= CLK_APBCLK_UART1_EN;
    SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB4_MFP_Msk | SYS_PB_L_MFP_PB5_MFP_Msk);
    SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB4_MFP_UART1_RX | SYS_PB_L_MFP_PB5_MFP_UART1_TX);
    /* Lock protected registers */
     
    SYS_LockReg();
    
    UART_Open(UART1, 9600);
  #if 0 
    UART_SetTimeoutCnt(UART1, 40);
	   
  //  UART_SET_RX_FIFO_INTTRGLV(uart1, UART_TLCTL_RFITL_8BYTES); /*使能接收FIFO 阀值中断，和接收超时中断*/ 
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IE_Msk | UART_IER_RTO_IE_Msk)); 
    NVIC_EnableIRQ(UART1_IRQn);
   // uart1->CTL |= 0x40;     /* RX DMA enable */
  //  GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
  //  PB13=0;
  
  WritePiont_uart1=0;
	ReadPiont_uart1=0;
#endif	
}
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送字符串
//--------------------------------------------------------------------------------------------
void SendString_uart1(unsigned char *pp)
{
 // UART_Write(uart1, pp, Strlen(pp));
}



void SendByte_uart1 (unsigned char Dat)
  {
   
         WatchdogReset(); 
  
   }
void waitdelay_uart1() 
{
  uint16_t i;
  for(i=0;i<10000;i++)
 ;
  ;	
}	
//--------------------------------------------------------------------------------------------
//Function: void SendDataToBus1(unsigned char *pp,unsigned char length)
//Input:    *pp,SendLength
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   :
//Description：接受Rabbit core 模块的信息
//--------------------------------------------------------------------------------------------
void SendDataToBus_uart1(unsigned char *pp,uint16_t length)
  {  
     UART_Write(UART1, pp, length);
     waitdelay_uart1();
  
 } 
void SendDataToBus1_uart1(unsigned char *pp,uint16_t length)
 {
 	SendDataToBus_uart1(pp,length);
 	
}
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送一个数字的ascii表现形式
//--------------------------------------------------------------------------------------------
void SendDataToBus1Abyte_uart1(unsigned char dat)  
  {  
  	
 } 
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送换行符号
//--------------------------------------------------------------------------------------------
void SendDataToBus1LineOve_uart1()  
  {  
 }
 
	
/*UART1中断处理函数*/
/*------------------------------------------------------------------
Function:void UART1_IRQHandler(void) 
Input:
Output:
Description:  UART0中断处理函数
---------------------------------------------------------------------*/   
void UART1_IRQHandler(void) 
{
	 uint8_t u8InChar=0xFF; 
	 uint32_t u32IntSts= UART1->ISR; /*发生接收阀值中断或者接收超时中断*/ 
	 if(u32IntSts & (UART_ISR_RDA_IS_Msk| UART_ISR_RTO_IS_Msk)) 
	  { /* 读走接收FIFO中所有的数据，直到接收FIFO为空 */ 
	    while(UART_GET_RX_EMPTY(UART1)==0) 
	    { /* 从接收FIFO中读一个数据 */
	      u8InChar = UART_READ(UART1);
	     #ifdef  _UseTestBorad
	      ReceiveBuff_uart1[WritePiont_uart1]=u8InChar; 
	      WritePiont_uart1=(WritePiont_uart1+1)%cBufferLen_uart1;
	      if(WritePiont_uart1==ReadPiont_uart1)
	        {
	          ReadPiont_uart1++;
                if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;	
	      } 
	 #else

				ReceiveBuff_uart0[WritePiont_uart0]=u8InChar; 
	      WritePiont_uart0=(WritePiont_uart0+1)%cBufferLen_uart0;
	      if(WritePiont_uart0==ReadPiont_uart0)
	        {
	          ReadPiont_uart0++;
                if(ReadPiont_uart0>=cBufferLen_uart0)
              ReadPiont_uart0=0;	
	      } 
	#endif				
				
				
	   } 
	 }
}

#define  Lora_M0   PB15
#define  Lora_M1   PB12 
#define  Lora1_M1   PB6 
#define  cWorkMode     0
#define  cConfigMode   1
#define  cSleepMode    2



void SwtichToWorkmode(unsigned char No )
  {
  	if( No==1)
 	   {Lora_M0=0;
  	  Lora_M1=0;
	   }
 else if( No==2)
   	 Lora1_M1=0;
  	LoraConfigstatus=0; 
  
  }
  	
void SwtichToConfigMode(unsigned char No )
 {if( No==1)
 	 {Lora_M0=0;
  	Lora_M1=1;
		 LoraConfigstatus=1; 
	 }
 else if( No==2)
 { 	 Lora1_M1=1;
  	
     LoraConfigstatus=2; 
 } 
  }
  	
void SwtichToSleepMode(unsigned char No )
 { if( No==1)
	  {
 	  Lora_M0=1;
  	Lora_M1=1;
		}
	  else if( No==2)	
		 Lora1_M1=1;	
 }	





void SwtichMode(unsigned char mode,unsigned char No )
  {
    switch(mode)
      {
      	  case   cWorkMode  :  SwtichToWorkmode(No);break;
      	  case   cConfigMode  :SwtichToConfigMode(No);break;
      	  case   cSleepMode  : SwtichToSleepMode(No);break;
      	  default:             SwtichToWorkmode(No);break;
      }		
  	
  }
   


/*------------------------------------------------------------------
Function:void DealUartBuff_uart1()
Input:
Output:
Description:  处理lora模块正常工作数据
数据格式: aa  ff  deviceNo  th,tl,rhh,rhl,crch,crcl
---------------------------------------------------------------------*/   
/*------------------------------------------------------------------
Function:void DealUartBuff_uart1()
Input:
Output:
Description:  处理lora模块正常工作数据
数据格式: aa  ff  deviceNo  th,tl,rhh,rhl,crch,crcl
---------------------------------------------------------------------*/   
	
 #ifdef Bl02d
      void     DealUartBuff_uart1_02d() 
			{
				
				
			}
    #endif
//---------------------------
 #ifdef LoraGateway
			
	
#define   setlen    12		  //lora node 发送11byte，加上信号强度，gateway输出12byte到cpu
void DealUartBuff_uart1_LoraGateway()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	
	int16_t temp,temp2;
	uint16_t  temp1; 
	uint8_t ActnodeAddr;
    	
	unsigned char   TempBuf[8],i;
	
        while((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=setlen)
          {               i=ReceiveBuff_uart1[(ReadPiont_uart1+2)%cBufferLen_uart1];
                             // lora data
       	                          if((ReceiveBuff_uart1[ReadPiont_uart1]==0xaa)&&(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0xff)
       	                          &&(  ParaList[(cLoraNodeAliveSet+i-1)*2+1]==1)  //add 2021-04-23 16:27   检测激活
       	                          &&(i>=AddrStart)&&(i<AddrStart+AddrLen))
       	                            i=1;
				 else
                                    i=0; 
                                           
        if(i==0)
          { ReadPiont_uart1++;
            if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           
            
                if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=setlen)
                  {   for(i=0;i<setlen;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                          TempCrc=CRC(SendTempBufferBak_uart1,setlen-3); 
			                 TempCrcCount=SendTempBufferBak_uart1[setlen-3]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[setlen-2];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+8)%cBufferLen_uart1;
       //add  2021-04-14 12:56                                     
       ActnodeAddr=SendTempBufferBak_uart1[2]-AddrStart;
     temp2=(int16_t)(SendTempBufferBak_uart1[3]*256+SendTempBufferBak_uart1[4]); //node  orig
       if(temp2!=-cInValidTemp)
       { temp=(int16_t)(ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2]*256+ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+1]);  //gateway offset
         temp=temp+temp2;
        }
       else
         temp=temp2;  
       temp1=(uint16_t)temp;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2] =temp1/256;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+1] =temp1%256;  
      //humi 
       temp2=(int16_t)(SendTempBufferBak_uart1[5]*256+SendTempBufferBak_uart1[6]); //node  orig
        if(temp2!=-cInValidTemp)
          { temp=(int16_t)(ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+2]*256+ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+3]);  //gateway offset
            temp=temp+temp2;
          }
         else
           temp=temp2;      
       temp1=(uint16_t)temp;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+2] =temp1/256;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+3] =temp1%256; 	
				GetRfDataFlag=1;
        GetRfnode=ActnodeAddr+1;				 
				//add 20211212
				 VarList[(cNodePower+ActnodeAddr*2)*2] =0;
				 VarList[(cNodePower+ActnodeAddr*2)*2+1] =SendTempBufferBak_uart1[setlen-4]; 
				 VarList[(cNodePower+ActnodeAddr*2)*2+2] =0;
				 VarList[(cNodePower+ActnodeAddr*2)*2+3] =SendTempBufferBak_uart1[setlen-1];  
				 
       //end of add  2021-04-14 12:56                                   	
                                          	
                                               WirelessNodeDelay[ActnodeAddr]=0;
                                           }
                                         else
                                           {
                                           	 //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }         		   
              }    
         }
     }
  }	
}  	
    #endif
//------------------------	 
	 #ifdef BH_Bl03d
      void     DealUartBuff_uart1_03d()
			{

			}				
			   
    #endif
//-----------------------------------------	 
#ifdef LoraNode
    void DealUartBuff_uart1_LoraNode()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	
	int16_t temp,temp2;
	uint16_t  temp1; 
	uint8_t ActnodeAddr;
    	
	unsigned char   TempBuf[8],i;
	
        while((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=9)
          {               i=ReceiveBuff_uart1[(ReadPiont_uart1+2)%cBufferLen_uart1];
                             // lora data
       	                          if((ReceiveBuff_uart1[ReadPiont_uart1]==0xaa)&&(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0xff)
       	                          &&(  ParaList[(cLoraNodeAliveSet+i-1)*2+1]==1)  //add 2021-04-23 16:27   检测激活
       	                          &&(i>=AddrStart)&&(i<AddrStart+AddrLen))
       	                            i=1;
				 else
                                    i=0; 
                                           
        if(i==0)
          { ReadPiont_uart1++;
            if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           
            
                if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=9)
                  {   for(i=0;i<9;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                          TempCrc=CRC(SendTempBufferBak_uart1,7); 
			                 TempCrcCount=SendTempBufferBak_uart1[7]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[8];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+8)%cBufferLen_uart1;
       //add  2021-04-14 12:56                                     
       ActnodeAddr=SendTempBufferBak_uart1[2]-AddrStart;
     temp2=(int16_t)(SendTempBufferBak_uart1[3]*256+SendTempBufferBak_uart1[4]); //node  orig
       if(temp2!=-cInValidTemp)
       { temp=(int16_t)(ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2]*256+ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+1]);  //gateway offset
         temp=temp+temp2;
        }
       else
         temp=temp2;  
       temp1=(uint16_t)temp;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2] =temp1/256;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+1] =temp1%256;  
      //humi 
       temp2=(int16_t)(SendTempBufferBak_uart1[5]*256+SendTempBufferBak_uart1[6]); //node  orig
        if(temp2!=-cInValidTemp)
          { temp=(int16_t)(ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+2]*256+ParaList[(cLoraNodeOffset+ActnodeAddr*2)*2+3]);  //gateway offset
            temp=temp+temp2;
          }
         else
           temp=temp2;      
       temp1=(uint16_t)temp;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+2] =temp1/256;
       VarList[(cTempStartAddr+ActnodeAddr*2)*2+3] =temp1%256; 	
       //end of add  2021-04-14 12:56 
       
  
				 
			



				 
                                          	
                                               WirelessNodeDelay[ActnodeAddr]=0;
                                           }
                                         else
                                           {
                                           	 //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }         		   
              }    
         }
     }
  }	
}  	
    #endif
//---------------------------------------------------------	 
	 #ifdef LW_flood
            
void DealUartBuff_uart1_LW_flood()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
	
	    	
        while((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=8)
             {                              //广播修改 地址为0
       	                         if((ReceiveBuff_uart1[ReadPiont_uart1]==0xc1)&&(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x00)&&(ReceiveBuff_uart1[(ReadPiont_uart1+2)%cBufferLen_uart1]==0x09))
       	                               ReadOutLoraPara1();
       	                        
				   else if((ReceiveBuff_uart1[ReadPiont_uart1]==DeviceNum)&&((ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x03)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x04)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x06)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x10) ||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x41)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x44)))         
                                         i=1;
                                   else
                                         i=0; 
                                           
        if(i==0)
          { ReadPiont_uart1++;
            if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           //不为10的命令,是8个字节的命令长度
            if(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]!=0x10)
              {
              	
            
                if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=8)
                  {   for(i=0;i<8;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                          TempCrc=CRC(SendTempBufferBak_uart1,6); 
			                 TempCrcCount=SendTempBufferBak_uart1[6]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[7];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+8)%cBufferLen_uart1;
                                          	
                                                    CheckModbusRespond_uart1(SendTempBufferBak_uart1);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }         		   
              }    
         }
       else   //0x10
        {
            if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=7)	
              {
              	DataLen_uart1=ReceiveBuff_uart1[(ReadPiont_uart1+6)%cBufferLen_uart1];
              	if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=DataLen_uart1+9)
              	 { for(i=0;i<DataLen_uart1+9;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                            TempCrc=CRC(SendTempBufferBak_uart1,DataLen_uart1+7); 
			                 TempCrcCount=SendTempBufferBak_uart1[DataLen_uart1+7]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[DataLen_uart1+8];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+DataLen_uart1+9)%cBufferLen_uart1;
                                            CheckModbusRespond_uart1(SendTempBufferBak_uart1);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }      
              	  }	
             } 	  		
          }    	
      //------0x10        
   
     }  //right head
  }//while
 } //function

    #endif
//---------------------------------------------------------------	 
	 #ifdef LW_DoorMagic
           
 void DealUartBuff_uart1_LW_DoorMagic()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
	
	    	
        while((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=8)
             {                              //广播修改 地址为0
       	                         if((ReceiveBuff_uart1[ReadPiont_uart1]==0xc1)&&(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x00)&&(ReceiveBuff_uart1[(ReadPiont_uart1+2)%cBufferLen_uart1]==0x09))
       	                               ReadOutLoraPara1();
       	                        
				   else if((ReceiveBuff_uart1[ReadPiont_uart1]==DeviceNum)&&((ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x03)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x04)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x06)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x10) ||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x41)||(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x44)))         
                                         i=1;
                                   else
                                         i=0; 
                                           
        if(i==0)
          { ReadPiont_uart1++;
            if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           //不为10的命令,是8个字节的命令长度
            if(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]!=0x10)
              {
              	
            
                if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=8)
                  {   for(i=0;i<8;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                          TempCrc=CRC(SendTempBufferBak_uart1,6); 
			                 TempCrcCount=SendTempBufferBak_uart1[6]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[7];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+8)%cBufferLen_uart1;
                                          	
                                                    CheckModbusRespond_uart1(SendTempBufferBak_uart1);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }         		   
              }    
         }
       else   //0x10
        {
            if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=7)	
              {
              	DataLen_uart1=ReceiveBuff_uart1[(ReadPiont_uart1+6)%cBufferLen_uart1];
              	if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=DataLen_uart1+9)
              	 { for(i=0;i<DataLen_uart1+9;i++)
                            SendTempBufferBak_uart1[i]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                            TempCrc=CRC(SendTempBufferBak_uart1,DataLen_uart1+7); 
			                 TempCrcCount=SendTempBufferBak_uart1[DataLen_uart1+7]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart1[DataLen_uart1+8];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart1=(ReadPiont_uart1+DataLen_uart1+9)%cBufferLen_uart1;
                                            CheckModbusRespond_uart1(SendTempBufferBak_uart1);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart1++;
                                                if(ReadPiont_uart1>=cBufferLen_uart1)
                                                   ReadPiont_uart1=0; 
                                          }      
              	  }	
             } 	  		
          }    	
      //------0x10        
   
     }  //right head
  }//while
 } //function




    #endif
	 
	 #ifdef Thermo_Bl03d
     void      DealUartBuff_uart1_Thermo_Bl03d()
		 {

     }			 
   
    #endif	
	
	
	
	
	
	

		
/*------------------------------------------------------------------
Function:DealLora_Configdata()
Input:
Output:
Description:  处理lora模块配置状态下数据
数据格式: C1 00 09 data1,data2,data3,data4,data5,data6,data7,data8,data9
---------------------------------------------------------------------*/   

void DealLora_ConfigdataNo1( )
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
 
	 {  while((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=12)
          {
                             // lora data
       	                          if((ReceiveBuff_uart1[ReadPiont_uart1]==0xC1)&&(ReceiveBuff_uart1[(ReadPiont_uart1+1)%cBufferLen_uart1]==0x00)&&(ReceiveBuff_uart1[(ReadPiont_uart1+2)%cBufferLen_uart1]==0x09))
       	                            i=1;
				 else
                                    i=0; 
                                           
        if(i==0)
          { ReadPiont_uart1++;
            if(ReadPiont_uart1>=cBufferLen_uart1)
              ReadPiont_uart1=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           
            
                if((WritePiont_uart1+cBufferLen_uart1-ReadPiont_uart1)%cBufferLen_uart1>=11)
                  {   for(i=3;i<12;i++)
                          SendTempBufferBak_uart1[i-3]=ReceiveBuff_uart1[(ReadPiont_uart1+i)%cBufferLen_uart1];
                              DealPara(SendTempBufferBak_uart1);
	              	          LoraConfigok=1; 
                              ReadPiont_uart1=(ReadPiont_uart1+12)%cBufferLen_uart1;
                      
                                    
                 }
         }	
  }  	
  }
}	 


void DealLora_Configdata(uint8_t   No)
  { 
#if 0
    uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
   if(No==1)	
       DealLora_ConfigdataNo1( );
	 else  if(No==2)	
	    DealLora_ConfigdataNo2( );
 #endif
	} 
	 
	 
void Lora_IoInitialNo(uint8_t  No)
  { if(No==1)
		{
        GPIO_SetMode(PB, BIT15, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PB, BIT12, GPIO_PMD_OUTPUT);
  	
  	
	LoraConfigstatus=1;
	LoraConfigok=0;
  }
	else if(No==2)
	{
    GPIO_SetMode(PB, BIT6, GPIO_PMD_OUTPUT);
    LoraConfigstatus=2;
	  LoraConfigok=0;
	 }		
}	
	
void Lora_IoInitial()
{
	Lora_IoInitialNo(1);
	Lora_IoInitialNo(2);
}
/*--------------------------------
 //Function: void LoraInitial()
 //Input:
   Output:
 //Description:初始化lora模块
               1:设置模块管脚
               2:读取lora模块参数, 最多读3次,每次间隔1s
               3:读取到lora模块参数后,退出,进入工作模式
 //-----------------------------------*/
 void LoraInitialNo(uint8_t  No)
  { 	  
    uint8_t  i;
    Lora_IoInitial(No);
    	for(i=0;i<3;i++)
	 {  ReadOutLoraPara(No);
	    SetDelay1s(1);
	   while(1)
	     { if(GetDelay1sStatus())
     	        {  
 				         DealLora_Configdata(No);
     	        if(LoraConfigok==1)
	             {i=i+3;
	              break;
	              }
							 else
							 {
								 SetDelay1s(1);
								 ReadOutLoraPara(No);
							 }
	          }     
	    } 
    }			 
	Lora_IoInitial(No);
	LoraConfigstatus=0;    
	SwtichToWorkmode(No);
  }
  
  void LoraInitial() 
	{ LoraInitialNo(1);
	//	LoraInitialNo(2);
		
	}
/*--------------------------------
 //Function: void DealLoraData()
 //Input:LoraConfigstatus=1,在配置模式  =0:在正常工作模式
   Output:
 //Description:主程序中用来检测lora端口接收的数据,
                根据状态不同,分别处理配置程序或者无线探头程序
 //-----------------------------------*/  
void DealLoraData()
 {
   if((LoraConfigstatus==1)||(LoraConfigstatus==2))
       DealLora_Configdata(LoraConfigstatus);
	 
   else
    #ifdef Bl02d
           DealUartBuff_uart1_02d() ;
    #endif

    #ifdef LoraGateway
           DealUartBuff_uart1_LoraGateway() ;
	        
    #endif
	 
	 #ifdef BH_Bl03d
           DealUartBuff_uart1_03d() ;
    #endif
	 
	 #ifdef LoraNode
           DealUartBuff_uart1_LoraNode() ;
    #endif
	 
	 #ifdef LW_flood
           DealUartBuff_uart1_LW_flood() ;
    #endif
	 
	 #ifdef LW_DoorMagic
           DealUartBuff_uart1_LW_DoorMagic() ;
    #endif
	 
	 #ifdef Thermo_Bl03d
           DealUartBuff_uart1_Thermo_Bl03d() ;
    #endif
		

 	
 }
void ClearUart1()
  {
  	WritePiont_uart1=0;
        ReadPiont_uart1=0;
  	
  } 
 
void TranslateUart1() 
 {
 	
          
         if(ReadPiont_uart1!=WritePiont_uart1)
           {  SendDataToBus_uart0(&ReceiveBuff_uart1[ReadPiont_uart1], 1); 
              ReadPiont_uart1=(ReadPiont_uart1+1)%cBufferLen_uart1;
            }   
       
 } 		 