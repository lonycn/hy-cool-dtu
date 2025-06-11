//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  I2C_HAL.c
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  I2C Hardware abstraction layer
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include "shx.h"


void NopCount(unsigned char i)
 { unsigned char j,m;
   for(j=0;j<i;j++)
     m++;
 }
     
void delay(unsigned char DelayCount)
{
while(DelayCount--);
}
//==============================================================================
void I2c_Init ()
//==============================================================================
{
  SDA=LOW;                // Set port as output for configuration
 NopCount(5);
  SCL=LOW;                // Set port as output for configuration
   NopCount(5);
  //SDA_CONF=LOW;           // Set SDA level as low for output mode
  //SCL_CONF=LOW;           // Set SCL level as low for output mode

  SDA=HIGH;  
   NopCount(5);             // I2C-bus idle mode SDA released (input)
  SCL=HIGH;  
   NopCount(5);             // I2C-bus idle mode SCL released (input)
}

//==============================================================================
void I2c_StartCondition ()
//==============================================================================
{ WatchdogReset();
   SDA=1; 
  // SCK=0;                   //Initial state 
  // _nop_(); 
   SCL=1; 
   NopCount(1);
    NopCount(5);
   SDA=0; 
   
    NopCount(5); 
   SCL=0;   
    NopCount(5);
  // SCL=1; 
  // _nop_(); 
  // SDA=1;        
  // _nop_(); 
  // SCL=0;     
}

//==============================================================================
void I2c_StopCondition ()
//==============================================================================
{ WatchdogReset();
  SDA=LOW;
   NopCount(5);
  SCL=LOW;
   NopCount(5);
  SCL=HIGH;
   NopCount(5);
  DelayMicroSeconds(40);  // set-up time stop condition (t_SU;STO)
  SDA=HIGH;
  DelayMicroSeconds(40);
}

//==============================================================================
unsigned char  I2c_WriteByte (unsigned char txByte)
//==============================================================================
{
  unsigned char mask,error=0;
  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
  { WatchdogReset();
  	if ((mask & txByte) == 0) 
         SDA=LOW;//masking txByte, write bit to SDA-Line
    else 
          SDA=HIGH;
    DelayMicroSeconds(4);             //data set-up time (t_SU;DAT)
    SCL=HIGH;                         //generate clock pulse on SCL
    DelayMicroSeconds(20);             //SCL high time (t_HIGH)
    SCL=LOW;
    DelayMicroSeconds(4);             //data hold time(t_HD;DAT)
  }
  SDA=HIGH;                           //release SDA-line
   NopCount(5);
  SCL=HIGH;                           //clk #9 for ack
  DelayMicroSeconds(4);               //data set-up time (t_SU;DAT)
  if(SDA==HIGH) 
     error=ACK_ERROR; //check ack from i2c slave
  SCL=LOW;
  DelayMicroSeconds(80);              //wait time to see byte package on scope
  return error;                       //return error code
}

//==============================================================================
unsigned char I2c_ReadByte (unsigned char ack)
//==============================================================================
{
  unsigned char mask,rxByte=0;
  SDA=HIGH;                           //release SDA-line
  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
  { WatchdogReset();
  	SCL=HIGH;                         //start clock on SCL-line
    DelayMicroSeconds(4);             //data set-up time (t_SU;DAT)
    DelayMicroSeconds(12);             //SCL high time (t_HIGH)
    if (SDA==1) 
       rxByte=(rxByte | mask); //read bit
    SCL=LOW;
    DelayMicroSeconds(12);             //data hold time(t_HD;DAT)
  }
  SDA=ack;          //2011-7-12 22:56                  //send acknowledge if necessary
  DelayMicroSeconds(4);               //data set-up time (t_SU;DAT)
  SCL=HIGH;                           //clk #9 for ack
  DelayMicroSeconds(20);               //SCL high time (t_HIGH)
  SCL=LOW;
   NopCount(5);
  SDA=HIGH;                           //release SDA-line
  DelayMicroSeconds(80);              //wait time to see byte package on scope
  return rxByte;                      //return error code
}






/*****************以下是对IIC总线的操作子程序***/
/*****************启动总线**********************/

void IICStart(void)
   {  
            SDA=1;
             NopCount(5);
            SCL=1;

            delay(4);
           NopCount(5);
            SCL=1; 
             NopCount(5);
            SDA=0;
             NopCount(5);
            delay(4);

     
  }
/*****************停止IIC总线****************/
void IICStop(void)
{ 
             SCL=0;
              NopCount(5);
             SDA=0;
               
             delay(4);
              NopCount(5);
             SCL=1;
              NopCount(5);
             SDA=1;
             NopCount(5);
             delay(4);
  
}

/**************检查应答位*******************/
unsigned char IICRecAck(void)
{ unsigned char CY;
             SCL=0;
              NopCount(5);
             SDA=1;

             delay(4);

            // P2CR  &= Bin(11110111); //set SDA into input 
            GPIO_SetMode(PA, BIT12, GPIO_PMD_INPUT);
           
           // P2CR  = Bin(11110101);
             delay(4);
              NopCount(5);
             SCL=1;
             delay(4);

             CY=SDA;                

             SCL=0;
             //delay(1);
            
             NopCount(5);
             GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
            // P2CR  |= Bin(00001000);//set SDA into output
             SDA=1;    
             return(CY);
    
}
/***************对IIC总线产生应答*******************/
void IICACK(void)
{       
             SDA=0;
              NopCount(5);
             SCL=0;

             delay(4);
              NopCount(5);
             //SDA=0;
             SCL=1;

             delay(4);
              NopCount(5);
             SCL=0;
              NopCount(5);
   
  }
/*****************不对IIC总线产生应答***************/
/*void IICNoAck(void)
  { 
SCL=0; 
    SDA=1;
   
delay(1);
    SCL=1;
SDA=1;
delay(1);
SCL=0; 
    SDA=1;
 
}*/
/*******************向IIC总线写数据*********************/
void IICSendByte(unsigned char sendbyte)           /*先发高位*/
{
   unsigned char  j=8;
   for(;j>0;j--)
        {

           SCL=0;

           delay(4);
            NopCount(5);
           if((sendbyte&0x80)==0)
             SDA=0;
           else 
             SDA=1;  
           sendbyte<<=1;        
        
           delay(4);
            NopCount(5);
           SCL=1;

           delay(4);
            NopCount(5);

       }
    SCL=0;
     NopCount(5);
    SDA=1;
     NopCount(5);
    delay(4);

 //SCL=1;
//delay(1); 
// SCL=0;
}
/**********************从IIC总线上读数据子程序**********/
unsigned char IICReceiveByte(void)
{
unsigned char 	receivebyte,i=8;

    SCL=0;
    SDA=1;
    delay(4);
   //P2CR  = Bin(11110101);
	
    //P2CR  &= Bin(11110111); // set SDA int16_t input
    
    //SCL=0;
    //delay(1);

    while(i--)
   	{
    	SCL=1;
        delay(4);

   		receivebyte=(receivebyte<<1)|SDA;

        SCL=0;
        delay(4);

   	}
  // P2CR  = Bin(11111101);	
    NopCount(5);
  // P2CR|= Bin(00001000);// set SDA int16_t output
SDA=1;
   //P2CR  &= Bin(11110111);	
	return(receivebyte);
  
}
/***************一个简单延时程序************************/






