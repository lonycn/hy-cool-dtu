#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

#define       cBufferLen_uart4     500
uint16_t    WritePiont_uart4;
uint16_t    ReadPiont_uart4;
unsigned char   ReceiveBuff_uart4[cBufferLen_uart4];
unsigned char   SendTempBufferBak_uart4[300];
unsigned char   DataLen_uart4;

void DealWithBoardCast_uart4(unsigned char  *pp);
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
//Description：
//--------------------------------------------------------------------------------------------
void UART4_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg();
     /* Enable IP clock */
     CLK_EnableModuleClock(SC2_MODULE);
  /* Select IP clock source */
   // CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC_S_HXT, CLK_SC0_CLK_DIVIDER(1));
       /* Set PA.8 and PA.9 pin for SC UART mode */
    SYS->PB_H_MFP &= ~(SYS_PB_H_MFP_PB10_MFP_Msk | SYS_PB_H_MFP_PB11_MFP_Msk);
    SYS->PB_H_MFP |= (SYS_PB_H_MFP_PB10_MFP_SC2_CLK | SYS_PB_H_MFP_PB11_MFP_SC2_DAT);
    SYS_LockReg();	
    SCUART_Open(SC2,9600);
    SCUART_SetTimeoutCnt(SC2, 40);
    // Enable smartcard receive interrupt
    SCUART_ENABLE_INT(SC2, SC_IER_RDA_IE_Msk);
    NVIC_EnableIRQ(SC2_IRQn);   
   //  UART_SET_RX_FIFO_INTTRGLV(UART0, UART_TLCTL_RFITL_8BYTES); /*使能接收FIFO 阀值中断，和接收超时中断*/ 
   // UART_ENABLE_INT(uart4, (UART_IER_RDA_IE_Msk | UART_IER_RTO_IE_Msk)); 
   // NVIC_EnableIRQ(uart4_IRQn);
   // UART0->CTL |= 0x40;     /* RX DMA enable */
    GPIO_SetMode(PB, BIT9, GPIO_PMD_OUTPUT);
    PB9=0;
   WritePiont_uart4=0;
	ReadPiont_uart4=0;
}

//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送字符串
//--------------------------------------------------------------------------------------------
void SendString_uart4(unsigned char *pp)
{
 // UART_Write(uart4, pp, Strlen(pp));
}



void SendByte_uart4 (unsigned char Dat)
  {
   
         WatchdogReset(); 
  
   }
void waitdelay_uart4() 
{
  uint16_t i;
  for(i=0;i<10000;i++)
;//  PB13=1;
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
void SendDataToBus_uart4(unsigned char *pp,uint16_t length)
  { PB9=1;
     SCUART_Write(SC2, pp, length);
     waitdelay_uart4();
   PB9=0;
 } 
void SendDataToBus1_uart4(unsigned char *pp,uint16_t length)
 {
 	 SendDataToBus_uart4(pp,length);
	
 }
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送一个数字的ascii表现形式
//--------------------------------------------------------------------------------------------
void SendDataToBus1Abyte_uart4(unsigned char dat)  
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
void SendDataToBus1LineOve_uart4()  
  {  
 }
 

/*uart4中断处理函数*/
/*------------------------------------------------------------------
Function:void uart4_IRQHandler(void) 
Input:
Output:
Description:  uart4中断处理函数
---------------------------------------------------------------------*/   
void SC2_IRQHandler(void) 
{
	 uint8_t u8InChar=0xFF; 
	 uint32_t u32IntSts= SC2->ISR; /*发生接收阀值中断或者接收超时中断*/ 
	if(u32IntSts &SC_IER_RDA_IE_Msk) 
	//  { /* 读走接收FIFO中所有的数据，直到接收FIFO为空 */ 
	    while(SCUART_GET_RX_EMPTY(SC2)==0) 
	    { /* 从接收FIFO中读一个数据 */
	      u8InChar = SCUART_READ(SC2);
	      ReceiveBuff_uart4[WritePiont_uart4]=u8InChar; 
	      WritePiont_uart4=(WritePiont_uart4+1)%cBufferLen_uart4;
	      if(WritePiont_uart4==ReadPiont_uart4)
	        {
	          ReadPiont_uart4++;
                if(ReadPiont_uart4>=cBufferLen_uart4)
              ReadPiont_uart4=0;	
	      } 
	  } 
	 }


void CheckModbusRespond_uart4(unsigned char *SendTempBufferBak_uart4)
 {  
 	SendDataToBus1_uart4(SendTempBufferBak_uart4,8);
 	
 }
/*------------------------------------------------------------------
Function:DealUartBuff_uart4()
Input:
Output:
Description:  处理uart4的中断接收数据.主程序中调用
---------------------------------------------------------------------*/  
void DealUartBuff_uart4()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
	
        while((WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4>=8)
             {                              //广播修改 地址为0
       	                          if((ReceiveBuff_uart4[ReadPiont_uart4]==0x00)&&((ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x06)||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x10)))
				    	 i=1;
				    	 //255 03 100 xxx
				   else if((ReceiveBuff_uart4[ReadPiont_uart4]==0xff)&&(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x03)&&(ReceiveBuff_uart4[(ReadPiont_uart4+2)%cBufferLen_uart4]==0x00)&&(ReceiveBuff_uart4[(ReadPiont_uart4+3)%cBufferLen_uart4]==0x64))
					 i=1;
					 //255 06 100 xxx
				   else  if((ReceiveBuff_uart4[ReadPiont_uart4]==0xff)&&(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x06)&&(ReceiveBuff_uart4[(ReadPiont_uart4+2)%cBufferLen_uart4]==0x00)&&(ReceiveBuff_uart4[(ReadPiont_uart4+3)%cBufferLen_uart4]==0x64))
				         i=1;	 
				   else if((ReceiveBuff_uart4[ReadPiont_uart4]==DeviceNum)&&((ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x03)||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x04)||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x06)||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x10) ||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x41)||(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]==0x44)))         
                                         i=1;
                                   else
                                         i=0; 
                                           
        if(i==0)
          { ReadPiont_uart4++;
            if(ReadPiont_uart4>=cBufferLen_uart4)
              ReadPiont_uart4=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           //不为10的命令,是8个字节的命令长度
            if(ReceiveBuff_uart4[(ReadPiont_uart4+1)%cBufferLen_uart4]!=0x10)
              {
              	
            
                if((WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4>=8)
                  {   for(i=0;i<8;i++)
                            SendTempBufferBak_uart4[i]=ReceiveBuff_uart4[(ReadPiont_uart4+i)%cBufferLen_uart4];
                          TempCrc=CRC(SendTempBufferBak_uart4,6); 
			                 TempCrcCount=SendTempBufferBak_uart4[6]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart4[7];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart4=(ReadPiont_uart4+8)%cBufferLen_uart4;
                                          	if(SendTempBufferBak_uart4[0]==255)
                                                  { // DealWithBoardCast_uart4(SendTempBufferBak_uart4);
                                                   
                                                   }  
                                                else
                                                    CheckModbusRespond_uart4(SendTempBufferBak_uart4);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart4++;
                                                if(ReadPiont_uart4>=cBufferLen_uart4)
                                                   ReadPiont_uart4=0; 
                                          }         		   
              }    
         }
       else   //0x10
        {
            if((WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4>=7)	
              {
              	DataLen_uart4=ReceiveBuff_uart4[(ReadPiont_uart4+6)%cBufferLen_uart4];
              	if((WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4>=DataLen_uart4+9)
              	 { for(i=0;i<DataLen_uart4+9;i++)
                            SendTempBufferBak_uart4[i]=ReceiveBuff_uart4[(ReadPiont_uart4+i)%cBufferLen_uart4];
                            TempCrc=CRC(SendTempBufferBak_uart4,DataLen_uart4+7); 
			                 TempCrcCount=SendTempBufferBak_uart4[DataLen_uart4+7]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart4[DataLen_uart4+8];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart4=(ReadPiont_uart4+DataLen_uart4+9)%cBufferLen_uart4;
                                            CheckModbusRespond_uart4(SendTempBufferBak_uart4);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart4++;
                                                if(ReadPiont_uart4>=cBufferLen_uart4)
                                                   ReadPiont_uart4=0; 
                                          }      
              	  }	
             } 	  		
          }    	
      //------0x10        
   
     }  //right head
  }//while
 } //function


/*------------------------------------------------------------------
Function: void DealWithBoardCast_uart4(unsigned char  *pp)
Input:
Output:
Description:  处理广播测试命令
---------------------------------------------------------------------*/  
void DealWithBoardCast_uart4(unsigned char  *pp)
 {
    if(pp[5]==100)
	       	                     {  
	       	                     	
                                      }
                                   else	 if(pp[5]==101)
                                     {
                                     	InitialParaWithNo();
                                     	
                                     }	
                                //add 2015-08-29 23:37
                                  else	 if(pp[5]==102)   
                                     	{   
                                         ParaList[32]=0x00;
                                         ParaList[33]=0x00;
                                         VarList[6]=ParaList[32];
                                         VarList[7]=ParaList[33];
                                         ProgramEepromByte();
  	                                 ReadEepromByte();
                                         DealBacklight();
                                        } 
                                  else  if(pp[5]==103)     
                                        {
                                        
                                         ParaList[32]=0x00;
                                         ParaList[33]=0x01;
                                         VarList[6]=ParaList[32];
                                         VarList[7]=ParaList[33];
                                         ProgramEepromByte();
  	                                 ReadEepromByte();
                                         DealBacklight();
                                        } 
                                 //----------------------------       
                                   
                                        
                                        
                                   else  if(pp[5]==104)   //add  2015-09-10 16:19  
                                        {
                                        
                                         ParaList[cDoorVar*2]=0x00;
                                         ParaList[cDoorVar*2+1]=78;
                                         ProgramEepromByte();
  	                                     ReadEepromByte();
                                         
                                        } 	   
                                 else  if(pp[5]==105)   //add  2015-09-10 16:19  
                                        {
                                        
                                         ParaList[cDoorVar*2]=0x00;
                                         ParaList[cDoorVar*2+1]=0x00;
                                         ProgramEepromByte();
  	                                 ReadEepromByte();
                                         
                                        }
                                   else  if(pp[5]==106)   //add  2015-09-10 16:19  
                                        {
                                        
                                         SendByte_uart4(DeviceNum);
                                     	 SendByte_uart4(DeviceNum);
                                     	 SendByte_uart4(DeviceNum);
                                         
                                        }   
                                        
                                        
                                        
                                           	           
                                           //end of add 2015-08-30 0:46   
                                 //add 2017-3-24 6:17
                              #if  0   
                                 else  if(pp[5]==107)   //add  2015-09-10 16:19  
                                        {
                                          DeviceNum=DeviceNum%16;
                                          if(DeviceNum==0)
                                           DeviceNum=16;
                                           ParaList[0]=0;
                                           ParaList[1]=DeviceNum;
                                           ProgramEepromByte();
                                         
                                         
                                        }   
                                #endif        
                                        
                                 else  if(pp[5]==107)   //add  2015-09-10 16:19  开启记录
                                        {
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2]=0x00;
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2+1]=0x00;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        }      	  
                                  else  if(pp[5]==108)   //add  2015-09-10 16:19   关闭记录
                                        {
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2]=0x00;
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2+1]=0x01;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        } 
                                   else  if(pp[5]==109)   //add  2015-09-10 16:19   
                                        {
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2]=0x00;
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2+1]=0x00;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        }       
                                   else  if(pp[5]==110)   //add  2015-09-10 16:19  
                                        {
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2]=0x00;
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2+1]=0x01;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        }         
                                  else  if(pp[5]==111)   //add  2015-09-10 16:19  
                                        {
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2]=0x00;
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2+1]=0x02;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         }        
                                   else  if(pp[5]==112)   //BroadCast Enter Test Mode
                                        {
                                          EnterTestMode();	
                                         }          
                                   else  if(pp[5]==113)   //BroadCast  Quit Test Mode
                                        {
                                          QuitTestMode();	
                                         }  
                                         
                                  else  if(pp[5]==114)   //BroadCast  Set Current Time and Display
                                        {
                                         SetTestTime();
                                         }  
                                  else  if(pp[5]==115)   //BroadCast  Quit DisplayTime
                                        {
                                          QuitDisplayTestTime();	
                                         }  
                                  else  if(pp[5]==116)   //BroadCast  enter Test Mem
                                        {  EnterTestSpiFlash();
                                        
                                         }  
                                                               
                                  else  if(pp[5]==117)   //Quit mem test
                                        { QuitTestSpiFlash();
                                         //Second30s_b=0;	
                                         }  
                                   else  if(pp[5]==118)   //Only DisplayTime
                                        {
                                        // command100Flag=1;
                                         SetOnlyDisplayTime();
  	      	                         LCD4_Clear();	
                                         }
                            //add 2015-11-11 15:25                             
                                   else if(pp[5]==119) 
                                         {  //BeepControlFun(1,5);
                                            DisplayWaitWdt();
                                            while(1);
                                             	
                                        	
                                         }	              
                             //end of add 2015-11-11 15:25           
                             //----------------------------------           
                                  else    if(pp[5]==120)    
                                         {  //ES0=0;
                                            //ScanTime=0;
                                            //ScanFlag=1;
                                           
                                        } 
                             //----------------------end of add  2015-12-10 17:30              	
                                 
                                 
                              //----------------------------------   add 2015-12-30 11:50        
                                  else    if(pp[5]==121)    
                                         {  
                                           // ES0=1;
                                           // ScanTime=0;
                                           // ScanFlag=0;
                                           
                                        } 
                                        
                                  else    if(pp[5]==122)    
                                         {  
                                           
                                            //TestFlag=1;
                                           
                                        }        
                                        
                                    else    if(pp[5]==123)    
                                         { 
                                         #if 0	 
                                            TestFlag=0;
                                            ParaList[cDoorVar*2+1]=0x00;
                                            ProgramEepromByte(); 
                                         #endif     
                                             
                                        }            
                                 
                                 
                                 
                                 
                                 
                                 //end of add 2017-3-24 6:17
                                   #if  0  
                                     else  if(pp[5]==110)
                                       { ParaList[16]=0x00;
                                         ParaList[17]=0x01;
                                         VarList[4]=ParaList[16];
                                         VarList[5]=ParaList[17];
                                         DealAlarm();	
                                      }       
                                    else  if(pp[5]==111)
                                       { ParaList[16]=0x00;
                                         ParaList[17]=0x00;
                                         VarList[4]=ParaList[16];
                                         VarList[5]=ParaList[17];
                                         DealAlarm();	
                                       }   
                                    #endif
                                           
                                        	
 	
 	
 }	

   

