#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include <math.h>
#include <lcd.h>

#define uchar unsigned char
#define uint  uint16_t
//定义Max7219端口
#define  Max7219_pinCLK  PA3
#define  Max7219_pinCS  PA4
#define  Max7219_pinDIN  PA2


void Delay_xms(uint x)
{
 uint i,j;
 for(i=0;i<x;i++)
  for(j=0;j<112;j++);
}






//--------------------------------------------
//功能：向MAX7219(U3)写入字节
//入口参数：DATA 
//出口参数：无
//说明：
void Write_Max7219_byte(uchar DATA)         
{
    	uchar i;    
		Max7219_pinCS=0;		
	    for(i=8;i>=1;i--)
          {		  
            Max7219_pinCLK=0;
						Delay_xms(2);
            Max7219_pinDIN=DATA&0x80;
            DATA=DATA<<1;
						Delay_xms(2);
            Max7219_pinCLK=1;
						Delay_xms(2);
           }                                 
}
//-------------------------------------------
//功能：向MAX7219写入数据
//入口参数：address、dat
//出口参数：无
//说明：
void Write_Max7219(uchar address,uchar dat)
{ 
     Max7219_pinCS=0;
	 Write_Max7219_byte(address);           //写入地址，即数码管编号
     Write_Max7219_byte(dat);               //写入数据，即数码管显示数字 
	 Max7219_pinCS=1;                        
}

void Init_MAX7219(void)
{
 Write_Max7219(0x09, 0xff);       //译码方式：BCD码
 Write_Max7219(0x0a, 0x03);       //亮度
 Write_Max7219(0x0b, 0x07);       //扫描界限；4个数码管显示
 Write_Max7219(0x0c, 0x01);       //掉电模式：0，普通模式：1
// Write_Max7219(0x0f, 0x01);       //显示测试：1；测试结束，正常显示：0
Write_Max7219(0x0f, 0x00);  
}
void SegLed_Test(void)
{

 Write_Max7219(0x0f, 0x00);       //显示测试：1；测试结束，正常显示：0
 Write_Max7219(1,8);
 Write_Max7219(2,7);
 Write_Max7219(3,6);
 Write_Max7219(4,5); 
 Write_Max7219(5,4);
 Write_Max7219(6,3);
 Write_Max7219(7,2);
 Write_Max7219(8,1);

}
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:初始化蜂鸣器LED PC4
------------------------------------------------*/  	
void  Test_SegLEDInital(void)
  {
	
  	
  	
  	GPIO_SetMode(PA, BIT2, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PA, BIT3, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PA, BIT4, GPIO_PMD_OUTPUT);
	Delay_xms(50);
        Init_MAX7219();
        Delay_xms(2000);
		
   }	

void Test_SegMentLedPin()
 {
    Max7219_pinCLK=0;
    Max7219_pinCLK=1;
    Max7219_pinCS=0;
    Max7219_pinCS=1;
    Max7219_pinDIN=0;
    Max7219_pinDIN=1;
 } 	   