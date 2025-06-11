#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#define       cBufferLen_uart3     200
extern uint16_t    WritePiont_uart3;
extern uint16_t    ReadPiont_uart3;
extern unsigned char   ReceiveBuff_uart3[cBufferLen_uart3];

#define       cBufferLen_uart0     200
uint16_t    WritePiont_uart0;
uint16_t    ReadPiont_uart0;
unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];
unsigned char   SendTempBufferBak_uart0[200];
unsigned char   DataLen_uart0;


#define       cBufferLen_uart2     500
extern uint16_t    WritePiont_uart2;
extern uint16_t    ReadPiont_uart2;
extern unsigned char   ReceiveBuff_uart2[cBufferLen_uart2];


#define       cBufferLen_uart4     500
extern uint16_t    WritePiont_uart4;
extern uint16_t    ReadPiont_uart4;
extern unsigned char   ReceiveBuff_uart4[cBufferLen_uart4];

void DealWithBoardCast_uart0(unsigned char  *pp);
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
//Description：接受前面板或者打印机的信息
//--------------------------------------------------------------------------------------------
void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg();
    SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB0_MFP_Msk | SYS_PB_L_MFP_PB1_MFP_Msk);
    SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB0_MFP_UART0_RX | SYS_PB_L_MFP_PB1_MFP_UART0_TX);
    /* Lock protected registers */
     
    SYS_LockReg();
    
    UART_Open(UART0, 9600);
    UART_SetTimeoutCnt(UART0, 40);
	   
  //  UART_SET_RX_FIFO_INTTRGLV(UART0, UART_TLCTL_RFITL_8BYTES); /*使能接收FIFO 阀值中断，和接收超时中断*/ 
    UART_ENABLE_INT(UART0, (UART_IER_RDA_IE_Msk | UART_IER_RTO_IE_Msk)); 
    NVIC_EnableIRQ(UART0_IRQn);
   // UART0->CTL |= 0x40;     /* RX DMA enable */
 #if 1
	GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
	 PB2=0;
#endif
	WritePiont_uart0=0;
	ReadPiont_uart0=0;
	
}

//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送字符串
//--------------------------------------------------------------------------------------------
void SendString_uart0(unsigned char *pp)
{
	#if 1
	//PB13=1;
	 PB2=1;
	 //PA2=1;
  UART_Write(UART0, pp, strlen(pp));
	 PB2=0;
	// PA2=0;
	#endif
}



void SendByte_uart0 (unsigned char Dat)
  {
   
         WatchdogReset(); 
  
   }
void waitdelay_uart0() 
{
  uint16_t i;
  for(i=0;i<1500;i++)
  // PB13=1;
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
void SendDataToBus_uart0(unsigned char *pp,uint16_t length)
  {   PB2=1;
			UART_Write(UART0, pp, length);
     waitdelay_uart0();
        PB2=0;
			
 } 
void SendDataToBus1_uart0(unsigned char *pp,uint16_t length)
 {
	 

	 
 	SendDataToBus_uart0(pp,length);
//	 if(ParaList[cTempOffset*2]*256+ParaList[cTempOffset*2+1]!=1234)
// 	SendDataToBus_uart3(pp,length);
	 
}
//--------------------------------------------------------------------------------------------
//Function: void SendString(unsigned char *pp)
//Input:    *pp,
//Output:   None  ,send data to displayboard
//send data length :
//Receive length   : 
//Description：发送一个数字的ascii表现形式
//--------------------------------------------------------------------------------------------
void SendDataToBus1Abyte_uart0(unsigned char dat)  
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
void SendDataToBus1LineOve_uart0()  
  {  
 }
 

/*UART0中断处理函数*/
/*------------------------------------------------------------------
Function:void UART0_IRQHandler(void) 
Input:
Output:
Description:  UART0中断处理函数
---------------------------------------------------------------------*/   
void UART0_IRQHandler(void) 
{
	 uint8_t u8InChar=0xFF; 
	 uint32_t u32IntSts= UART0->ISR; /*发生接收阀值中断或者接收超时中断*/ 
	 if(u32IntSts & (UART_ISR_RDA_IS_Msk| UART_ISR_RTO_IS_Msk)) 
	  { /* 读走接收FIFO中所有的数据，直到接收FIFO为空 */ 
	    while(UART_GET_RX_EMPTY(UART0)==0) 
	    { /* 从接收FIFO中读一个数据 */
	      u8InChar = UART_READ(UART0);
				
	
				ReceiveBuff_uart0[WritePiont_uart0]=u8InChar; 
	      WritePiont_uart0=(WritePiont_uart0+1)%cBufferLen_uart0;
	      if(WritePiont_uart0==ReadPiont_uart0)
	        {
	          ReadPiont_uart0++;
                if(ReadPiont_uart0>=cBufferLen_uart0)
              ReadPiont_uart0=0;	
	      } 
		
				
	   } 
	 }
}


 	

void  Urat0TranslateUart3() 
 {    if(ParaList[cUrat0TranslateUart3*2+1]==78)
 	     {
          if(ReadPiont_uart0!=WritePiont_uart0)
           {  SendDataToBus_uart3(&ReceiveBuff_uart0[ReadPiont_uart0], 1); 
              ReadPiont_uart0=(ReadPiont_uart0+1)%cBufferLen_uart0;
            }
				 if(ReadPiont_uart3!=WritePiont_uart3)
           {  SendDataToBus_uart0(&ReceiveBuff_uart3[ReadPiont_uart3], 1); 
              ReadPiont_uart3=(ReadPiont_uart3+1)%cBufferLen_uart3;
            }
				}  
 }

