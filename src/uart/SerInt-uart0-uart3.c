#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

#define       cBufferLen_uart3     200
uint16_t    WritePiont_uart3;
uint16_t    ReadPiont_uart3;
unsigned char   ReceiveBuff_uart3[cBufferLen_uart3];
unsigned char   SendTempBufferBak_uart3[200];
unsigned char   DataLen_uart3;
unsigned char   CommandStatus;

#define       cBufferLen_uart2     500
extern uint16_t    WritePiont_uart2;
extern uint16_t    ReadPiont_uart2;
extern unsigned char   ReceiveBuff_uart2[cBufferLen_uart2];


#define       cBufferLen_uart4     500
extern uint16_t    WritePiont_uart4;
extern uint16_t    ReadPiont_uart4;
extern unsigned char   ReceiveBuff_uart4[cBufferLen_uart4];

void DealWithBoardCast_uart3(unsigned char  *pp);

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
void UART3_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg();
     /* Enable IP clock */
     CLK_EnableModuleClock(SC1_MODULE);
  /* Select IP clock source */
   // CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC_S_HXT, CLK_SC0_CLK_DIVIDER(1));
       /* Set PA.8 and PA.9 pin for SC UART mode */
    SYS->PC_L_MFP &= ~(SYS_PC_L_MFP_PC0_MFP_Msk | SYS_PC_L_MFP_PC1_MFP_Msk);
    SYS->PC_L_MFP |= (SYS_PC_L_MFP_PC0_MFP_SC1_CLK | SYS_PC_L_MFP_PC1_MFP_SC1_DAT);
    SYS_LockReg();	
    SCUART_Open(SC1,9600);
    SCUART_SetTimeoutCnt(SC1, 40);
    // Enable smartcard receive interrupt
    SCUART_ENABLE_INT(SC1, SC_IER_RDA_IE_Msk);
    NVIC_EnableIRQ(SC1_IRQn);   
   //  UART_SET_RX_FIFO_INTTRGLV(UART0, UART_TLCTL_RFITL_8BYTES); /*使能接收FIFO 阀值中断，和接收超时中断*/ 
   // UART_ENABLE_INT(uart3, (UART_IER_RDA_IE_Msk | UART_IER_RTO_IE_Msk)); 
   // NVIC_EnableIRQ(uart3_IRQn);
   // UART0->CTL |= 0x40;     /* RX DMA enable */
   // GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
   // PB13=0;
   WritePiont_uart3=0;
	 ReadPiont_uart3=0;
}

//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送字符串
//--------------------------------------------------------------------------------------------



void SendByte_uart3 (unsigned char Dat)
  {
   
         WatchdogReset(); 
  
   }
void waitdelay_uart3() 
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
void SendDataToBus_uart3(unsigned char *pp,uint16_t length)
  {  
     SCUART_Write(SC1, pp, length);
     waitdelay_uart3();
    
 }

 
void SendDataToBus1_uart3(unsigned char *pp,uint16_t length)
 {
 	SendDataToBus_uart3(pp,length);
 	
}
 
void SendString_uart3(unsigned char *pp)
{
  SendDataToBus_uart3( pp, strlen(pp));
}

//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送一个数字的ascii表现形式
//--------------------------------------------------------------------------------------------
void SendDataToBus1Abyte_uart3(unsigned char dat)  
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
void SendDataToBus1LineOve_uart3()  
  {  
 }
 

/*uart3中断处理函数*/
/*------------------------------------------------------------------
Function:void uart3_IRQHandler(void) 
Input:
Output:
Description:  uart3中断处理函数
---------------------------------------------------------------------*/   
void SC1_IRQHandler(void) 
{
	 uint8_t u8InChar=0xFF; 
	 uint32_t u32IntSts= SC1->ISR; /*发生接收阀值中断或者接收超时中断*/ 
	if(u32IntSts &SC_IER_RDA_IE_Msk) 
	//  { /* 读走接收FIFO中所有的数据，直到接收FIFO为空 */ 
	    while(SCUART_GET_RX_EMPTY(SC1)==0) 
	    { /* 从接收FIFO中读一个数据 */
	      u8InChar = SCUART_READ(SC1);
	      ReceiveBuff_uart3[WritePiont_uart3]=u8InChar; 
	      WritePiont_uart3=(WritePiont_uart3+1)%cBufferLen_uart3;
	      if(WritePiont_uart3==ReadPiont_uart3)
	        {
	          ReadPiont_uart3++;
                if(ReadPiont_uart3>=cBufferLen_uart3)
              ReadPiont_uart3=0;	
	      } 
	  } 
	 }

 /*------------------------------------------------------------------
Function:void DealUartBuff_uart3_Lora() 
Input:
Output:
Description:  UART3接收数据处理
---------------------------------------------------------------------*/   	

void DealUartBuff_uart3_Lora()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
	
	
        while((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=8)
             {                              //广播修改 地址为0
       	  if((ReceiveBuff_uart3[ReadPiont_uart3]==0xff)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x03)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+3)%cBufferLen_uart3]==0x64))
					 i=1;
					if((ReceiveBuff_uart3[ReadPiont_uart3]==0xff)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x03)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+3)%cBufferLen_uart3]==0x66))
					 i=1;
					 //255 06 100 xxx
				   else  if((ReceiveBuff_uart3[ReadPiont_uart3]==0xff)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x06)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+3)%cBufferLen_uart3]==0x64))
				         i=1;	 
				   else if((ReceiveBuff_uart3[ReadPiont_uart3]==DeviceNum)&&
				         ((ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x03)
				           ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x04)
				           ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x06)
				          ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x10) 
				          ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x02)
				          ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x01)
				           ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x05)
				           )
				          )         
                                         i=1;
                                   
                                   else
                                         i=0; 
                                           
        if(i==0)
          { ReadPiont_uart3++;
            if(ReadPiont_uart3>=cBufferLen_uart3)
              ReadPiont_uart3=0;
          }
         else
         { //找到匹配的字符头,计算crc
            
           //不为10的命令,是8个字节的命令长度
            if(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]!=0x10)
              {
              	
            
                if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=8)
                  {   for(i=0;i<8;i++)
                            SendTempBufferBak_uart3[i]=ReceiveBuff_uart3[(ReadPiont_uart3+i)%cBufferLen_uart3];
                          TempCrc=CRC(SendTempBufferBak_uart3,6); 
			                 TempCrcCount=SendTempBufferBak_uart3[6]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart3[7];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart3=(ReadPiont_uart3+8)%cBufferLen_uart3;
                                          	if(SendTempBufferBak_uart3[0]==255)
                                                  {  //DealWithBoardCast_uart3(SendTempBufferBak_uart3);
                                                   
                                                   }  
                                                else
                                                    CheckModbusRespond_uart3(SendTempBufferBak_uart3);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart3++;
                                                if(ReadPiont_uart3>=cBufferLen_uart3)
                                                   ReadPiont_uart3=0; 
                                          }         		   
              }    
         }
       else   //0x10
        {
            if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=7)	
              {
              	DataLen_uart3=ReceiveBuff_uart3[(ReadPiont_uart3+6)%cBufferLen_uart3];
              	if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=DataLen_uart3+9)
              	 { for(i=0;i<DataLen_uart3+9;i++)
                            SendTempBufferBak_uart3[i]=ReceiveBuff_uart3[(ReadPiont_uart3+i)%cBufferLen_uart3];
                            TempCrc=CRC(SendTempBufferBak_uart3,DataLen_uart3+7); 
			                 TempCrcCount=SendTempBufferBak_uart3[DataLen_uart3+7]<<8;
                                         TempCrcCount+=SendTempBufferBak_uart3[DataLen_uart3+8];
                                        if((TempCrc==TempCrcCount)||(TempCrcCount==0x1234))
                                          { ReadPiont_uart3=(ReadPiont_uart3+DataLen_uart3+9)%cBufferLen_uart3;
                                            CheckModbusRespond_uart3(SendTempBufferBak_uart3);    
                                          }
                                         else  //校验不对,说明是其他的数据,不是完整数据包
                                          {
                                          	ReadPiont_uart3++;
                                                if(ReadPiont_uart3>=cBufferLen_uart3)
                                                   ReadPiont_uart3=0; 
                                          }      
              	  }	
             } 	  		
          }    	
      //------0x10        
   
     }  //right head
  }//while
 } //function




void  EnterTestMode()
 {
   
    ParaList[(cTestMode03D-cParaActual)*2]=0x00;
    ParaList[(cTestMode03D-cParaActual)*2+1]=0x01; 
 }
void QuitTestMode()
 {
    ParaList[(cTestMode03D-cParaActual)*2]=0x00;
    ParaList[(cTestMode03D-cParaActual)*2+1]=0x00; 
 
 }

void  SetTestTime()
{uint8_t TimeTempVar[6];
	#if  0
      
                                         TimeTempVar[0]=0x01;
  	                                 TimeTempVar[1]=0x02;
  	                                 TimeTempVar[2]=0x03;
  	                                 TimeTempVar[3]=0x04;
  	                                 TimeTempVar[5]=0x05;
  	                                 TimeTempVar[6]=0x06;
  	      	                         SetPCF8563(TimeTempVar);
  	      	                         LCD4_Clear();	
 	      	                         
 #endif 	      	                         
  }            
  	      	                         
void EnterTestSpiFlash()
{       
 #if   0 
 	uint8_t  TemBuffer[4]; //每8页开始更换一次页和记录的位置每次换位置的时候,进行一次参数存储
 	uint8_t  TimeTempVari[4],i;
 	             W25QXX_Read(TemBuffer,0,4);
 	        for(i=0;i<4;i++)
 	             TimeTempVari[i]=i+100;   
 	       W25QXX_Write(TimeTempVari,0,4);
               W25QXX_Read(TimeTempVari,0,4); 
               for(i=0;i<4;i++)
                {if(TimeTempVari[i]!=i+100)
                   break;
                }   
               if(i==4)               
                   { LCD4_Clear();
                     LCD_string(1,2,"Mem Test Ok");
                  }
                 else
                  {
                 	       LCD4_Clear();
                               LCD_string(1,2,"Mem Test Fail");	
                  }	
             	 W25QXX_Write(TemBuffer,0,4); 								
 #endif
}
      
void      QuitTestSpiFlash()
  {
  
  
  }
  
void SetOnlyDisplayTime()
  {
    ParaList[(cTestMode03D-cParaActual)*2]=0x00;
    ParaList[(cTestMode03D-cParaActual)*2+1]=0x02;
  }
      
void  DisplayWaitWdt()
 {
   LCD4_Clear();
   LCD_string(1,2," Wait Watchdog  ");
   WatchDogTestFlag=1;
 
 }	
 
void InitialParaWithOutNo()
{    
  	
	
	       	                        ResetFlag=0;
	       	                        InitialPara();
	       	                        HardWaitBell(1,10,10); 
																	AlarmLEDcontrol(1,1,1); 
 
  }	       	                         
 
void  InitialParaWithNo()
{  
  
                                    	  ResetFlag=1;
	       	                           InitialPara();
					  HardWaitBell(3,10,10);
					  
																			AlarmLEDcontrol(3,1,1);
    }
    
void QuitDisplayTestTime()
 {
 	ParaList[(cTestMode03D-cParaActual)*2]=0x00;
       ParaList[(cTestMode03D-cParaActual)*2+1]=0x00;
 }	  
 

  
/*------------------------------------------------------------------
Function: void DealWithBoardCast_uart3(unsigned char  *pp)
Input:
Output:
Description:  处理广播测试命令
---------------------------------------------------------------------*/  
void DealWithBoardCast_uart3(unsigned char  *pp)
 {               if(pp[3]==101)
                    {
                         
  	               if((pp[4]==0)&&((pp[5]!=0)&&(pp[5]!=255)) )  //2016-01-18 21:19
  	                { ParaList[cDeviceNum*2]=pp[4];
  	                  ParaList[cDeviceNum*2+1]=pp[5];   
     	          	  ProgramEepromByte();
  	                   ReadEepromByte();
  	               } 
  	              else if(pp[5]==255)
  	               { 
  	               	 // GenerateSimData(pp[4]);
  	               }	
                    }
                  								
                  else if(pp[3]==100)  	
                     {	     
                              if(pp[5]==100)
	       	                     {  if(CommandStatus==0)
                                     	 {      CommandStatus=1;
                                     	 	InitialParaWithOutNo();
                                     	 	CommandStatus=0;
                                     	 }
	       	                     	}
                                   else	 if(pp[5]==101)
                                     {  if(CommandStatus==0)
                                     	 {      CommandStatus=1;
                                     	 	InitialParaWithNo();
                                     	 	CommandStatus=0;
                                     	 }
                                     }	
                                //add 2015-08-29 23:37
                                  else	 if(pp[5]==102)    //backlight 0ff
                                     	{   
                                         ParaList[cBackLightControl*2]=0x00;
                                         ParaList[cBackLightControl*2+1]=0x00;
                                         ProgramEepromByte();
  	                                 ReadEepromByte();
                                         DealBacklight();
                                        } 
                                  else  if(pp[5]==103)       //backlight 0n
                                        {
                                        
                                         ParaList[cBackLightControl*2]=0x00;
                                         ParaList[cBackLightControl*2+1]=0x01;
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
                                        
                                         SendByte_uart3(DeviceNum);
                                     	 SendByte_uart3(DeviceNum);
                                     	 SendByte_uart3(DeviceNum);
                                         
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
                                          ParaList[cRemoteControl*2]=0x00;
                                          ParaList[cRemoteControl*2+1]=0x01;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        }         
                                  else  if(pp[5]==111)   //add  2015-09-10 16:19  
                                        {
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2]=0x00;
                                          ParaList[(cAlarmOutControl03D-cParaActual)*2+1]=0x02;
                                          ParaList[cRemoteControl*2]=0x00;
                                          ParaList[cRemoteControl*2+1]=0x00;
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
                                        	
 	
 	
 }	

   


void SendAbyte(unsigned char dat)  
  {          
  	        WatchdogReset(); 
  
 } 


 
 /*------------------------------------------------------------------
Function:DealUartBuff_uart3()
Input:
Output:
Description:  处理uart3的中断接收数据.主程序中调用
---------------------------------------------------------------------*/  
void DealUartBuff_uart3()
 {
     DealUartBuff_uart3_Lora( ); 
         
}	



 /*------------------------------------------------------------------
Function:DealUartBuff_uart3()
Input:
Output:
Description:  处理uart3的中断接收数据.主程序中调用
---------------------------------------------------------------------*/  
void ClearUart3()
  {
    	
    WritePiont_uart3=0;
    ReadPiont_uart3=0;	
  }
void TranslateUart3() 
 {
 	
          if(ReadPiont_uart3!=WritePiont_uart3)
           {  SendDataToBus_uart1(&ReceiveBuff_uart3[ReadPiont_uart3], 1); 
              ReadPiont_uart3=(ReadPiont_uart3+1)%cBufferLen_uart3;
            }
        
     
 }
 

