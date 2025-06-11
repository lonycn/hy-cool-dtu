#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include <math.h>
#include <lcd.h>

#define uchar unsigned char
#define uint  uint16_t
//����Max7219�˿�
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
//���ܣ���MAX7219(U3)д���ֽ�
//��ڲ�����DATA 
//���ڲ�������
//˵����
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
//���ܣ���MAX7219д������
//��ڲ�����address��dat
//���ڲ�������
//˵����
void Write_Max7219(uchar address,uchar dat)
{ 
     Max7219_pinCS=0;
	 Write_Max7219_byte(address);           //д���ַ��������ܱ��
     Write_Max7219_byte(dat);               //д�����ݣ����������ʾ���� 
	 Max7219_pinCS=1;                        
}

void Init_MAX7219(void)
{
 Write_Max7219(0x09, 0xff);       //���뷽ʽ��BCD��
 Write_Max7219(0x0a, 0x03);       //����
 Write_Max7219(0x0b, 0x07);       //ɨ����ޣ�4���������ʾ
 Write_Max7219(0x0c, 0x01);       //����ģʽ��0����ͨģʽ��1
// Write_Max7219(0x0f, 0x01);       //��ʾ���ԣ�1�����Խ�����������ʾ��0
Write_Max7219(0x0f, 0x00);  
}
void SegLed_Test(void)
{

 Write_Max7219(0x0f, 0x00);       //��ʾ���ԣ�1�����Խ�����������ʾ��0
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
Description:��ʼ��������LED PC4
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