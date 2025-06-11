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
extern uint16_t   LoraConfigok;
unsigned char   ReceiveBuff_uart3[cBufferLen_uart3];
unsigned char   SendTempBufferBak_uart3[200];
unsigned char   DataLen_uart3;




#define       cBufferLen_uart0     200
extern uint16_t    WritePiont_uart0;
extern uint16_t    ReadPiont_uart0;
extern unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];


void DealWithBoardCast_uart3(unsigned char  *pp);
/*=======================================================================
//ʱ�ӣ�11.0592M
//baudrate:9600bps
//baudrate generate:timer1
//count :bandrate = 1��((0x100-0xfd����12*32��11.0592)us��1��104us=9.6k
//======================================================================*/
//--------------------------------------------------------------------------------------------
//Function: void SendDataToBus(unsigned char *pp,unsigned char length)
//Input:    *pp,SendLength
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   :
//Description��
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
   //  UART_SET_RX_FIFO_INTTRGLV(UART0, UART_TLCTL_RFITL_8BYTES); /*ʹ�ܽ���FIFO ��ֵ�жϣ��ͽ��ճ�ʱ�ж�*/ 
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
//Description�������ַ���
//--------------------------------------------------------------------------------------------
void SendString_uart3(unsigned char *pp)
{
 // UART_Write(uart3, pp, Strlen(pp));
}



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
//Description������Rabbit core ģ�����Ϣ
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
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description������һ�����ֵ�ascii������ʽ
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
//Description�����ͻ��з���
//--------------------------------------------------------------------------------------------
void SendDataToBus1LineOve_uart3()  
  {  
 }
 

/*uart3�жϴ�����*/
/*------------------------------------------------------------------
Function:void uart3_IRQHandler(void) 
Input:
Output:
Description:  uart3�жϴ�����
---------------------------------------------------------------------*/   
void SC1_IRQHandler(void) 
{
	 uint8_t u8InChar=0xFF; 
	 uint32_t u32IntSts= SC1->ISR; /*�������շ�ֵ�жϻ��߽��ճ�ʱ�ж�*/ 
	if(u32IntSts &SC_IER_RDA_IE_Msk) 
	//  { /* ���߽���FIFO�����е����ݣ�ֱ������FIFOΪ�� */ 
	    while(SCUART_GET_RX_EMPTY(SC1)==0) 
	    { /* �ӽ���FIFO�ж�һ������ */
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
Function:DealUartBuff_uart3()
Input:
Output:
Description:  ����uart3���жϽ�������.�������е���
---------------------------------------------------------------------*/  
void DealUartBuff_uart3()
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
	
        while((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=8)
             {                              //�㲥�޸� ��ַΪ0
       	                          if((ReceiveBuff_uart3[ReadPiont_uart3]==0x00)&&((ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x06)||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x10)))
				    	 i=1;
				    	 //255 03 100 xxx
				   else if((ReceiveBuff_uart3[ReadPiont_uart3]==0xff)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x03)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+3)%cBufferLen_uart3]==0x64))
					 i=1;
					 //255 06 100 xxx
				   else  if((ReceiveBuff_uart3[ReadPiont_uart3]==0xff)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x06)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+3)%cBufferLen_uart3]==0x64))
				         i=1;	 
				   else if((ReceiveBuff_uart3[ReadPiont_uart3]==DeviceNum)&&((ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x03)||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x04)||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x06)||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x10) ||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x41)||(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x44)))         
                                         i=1;
                                   else
                                         i=0; 
                                           
        if(i==0)
          { ReadPiont_uart3++;
            if(ReadPiont_uart3>=cBufferLen_uart3)
              ReadPiont_uart3=0;
          }
         else
         { //�ҵ�ƥ����ַ�ͷ,����crc
            
           //��Ϊ10������,��8���ֽڵ������
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
                                                  {  DealWithBoardCast_uart3(SendTempBufferBak_uart3);
                                                   
                                                   }  
                                                else
                                                    CheckModbusRespond_uart3(SendTempBufferBak_uart3);    
                                          }
                                         else  //У�鲻��,˵��������������,�����������ݰ�
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
                                         else  //У�鲻��,˵��������������,�����������ݰ�
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


/*------------------------------------------------------------------
Function: void DealWithBoardCast_uart3(unsigned char  *pp)
Input:
Output:
Description:  ����㲥��������
---------------------------------------------------------------------*/  
void DealWithBoardCast_uart3(unsigned char  *pp)
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
                                        
                                 else  if(pp[5]==107)   //add  2015-09-10 16:19  ������¼
                                        {
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2]=0x00;
                                          ParaList[(cSaveDataFlag03D-cParaActual)*2+1]=0x00;
                                          ProgramEepromByte();
  	                                  ReadEepromByte();
                                         
                                         
                                        }      	  
                                  else  if(pp[5]==108)   //add  2015-09-10 16:19   �رռ�¼
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

   
/*------------------------------------------------------------------
Function:DealLora_Configdata()
Input:
Output:
Description:  ����loraģ������״̬������
���ݸ�ʽ: C1 00 09 data1,data2,data3,data4,data5,data6,data7,data8,data9
---------------------------------------------------------------------*/   

void DealLora_ConfigdataNo2( )
  {     uint16_t    TempCrc;
	uint16_t    TempCrcCount;
	unsigned char   TempBuf[8],i;
 {  while((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=12)
          {
                             // lora data
       	                          if((ReceiveBuff_uart3[ReadPiont_uart3]==0xC1)&&(ReceiveBuff_uart3[(ReadPiont_uart3+1)%cBufferLen_uart3]==0x00)&&(ReceiveBuff_uart3[(ReadPiont_uart3+2)%cBufferLen_uart3]==0x09))
       	                            i=1;
				 else
                                    i=0; 
                                           
        if(i==0)
          { ReadPiont_uart3++;
            if(ReadPiont_uart3>=cBufferLen_uart3)
              ReadPiont_uart3=0;
          }
         else
         { //�ҵ�ƥ����ַ�ͷ,����crc
            
           
            
                if((WritePiont_uart3+cBufferLen_uart3-ReadPiont_uart3)%cBufferLen_uart3>=11)
                  {   for(i=3;i<12;i++)
                          SendTempBufferBak_uart3[i-3]=ReceiveBuff_uart3[(ReadPiont_uart3+i)%cBufferLen_uart3];
                              DealPara(SendTempBufferBak_uart3);
	              	          LoraConfigok=1; 
                              ReadPiont_uart3=(ReadPiont_uart3+12)%cBufferLen_uart3;
                      
                                    
                 }
         }	
  }  	
  }
//--------	
}
void ClearUart3()
  {
  	WritePiont_uart3=0;
        ReadPiont_uart3=0;
  	
  } 
