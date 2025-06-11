#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include <math.h>
#define _GLOBAL_H
#include	"global.h"


typedef	unsigned char	Byte;       //single byte   0xff
typedef	uint16_t	Word; 		//two byte 		0xffff
typedef	unsigned long	Dword;		//four byte		0xffffffff
typedef	uint16_t	uint;
typedef	unsigned char	uchar;
typedef unsigned char     u8	;
typedef uint16_t      u16	;
typedef signed   long     u32	;


/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:³õÊ¼»¯·äÃùÆ÷LED PC4
------------------------------------------------*/  	
void  DIDO_Initial(void)
  {
   	
  
  #ifdef  __flood
        GPIO_SetMode(PA, BIT14, GPIO_PMD_INPUT);  //DI1
        GPIO_SetMode(PB, BIT9,  GPIO_PMD_INPUT);  //DI2
	      GPIO_SetMode(PC, BIT2,  GPIO_PMD_INPUT);  //DI3
	      GPIO_SetMode(PE, BIT5,  GPIO_PMD_INPUT);  //DI4
   #endif	
	GPIO_SetMode(PD, BIT6, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT7, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PB, BIT3, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PB, BIT7, GPIO_PMD_OUTPUT);
	
	GPIO_SetMode(PB, BIT7, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT14, GPIO_PMD_INPUT);  //MAGIC	
	SetBL();
	
		
   }	
	
	 
void magicInitial()
{

GPIO_SetMode(PD, BIT14, GPIO_PMD_INPUT); 

}
	
	
void AlarmerInitial()
{

 GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
}	


	 
	 
	 
	 

void DigitalOutput(unsigned char port,unsigned char Value)
 {
   switch(port)
    {
    	case 1:  if(Value==1)
    	            PD7=1;
    	         else
    	            PD7=0;
    	         break;
    	         
    	case 2:  if(Value==1)
    	            PD6=1;
    	         else
    	            PD6=0;
    	         break;         
    	case 3:  if(Value==1)
    	            PB3=1;
    	         else
    	            PB3=0;
    	         break;         
      case 4:  if(Value==1)
    	            PB2=1;
    	         else
    	            PB2=0;
    	         break;	          	
 	default:break;
   }	
} 	 
 
