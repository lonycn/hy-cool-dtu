

#ifdef  __Nano100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#endif





/********************************************************
�������ܣ�switch_Init(void)    �����ʼ��            
��ڲ�������
����  ֵ����
*********************************************************/
void switch_Init(void)
{   
#ifdef __DoorMagic	
     GPIO_SetMode(PA, BIT2, GPIO_PMD_INPUT);
     GPIO_SetMode(PA, BIT3, GPIO_PMD_INPUT);
     GPIO_SetMode(PA, BIT4, GPIO_PMD_INPUT);
     GPIO_SetMode(PC, BIT7, GPIO_PMD_INPUT);
#endif     
    
}
/********************************************************
�������ܣ�void  StateMachine_Switch()  ����״̬����������������         
��ڲ�������
����  ֵ����
*********************************************************/
void  StateMachine_Switch()
 {
   uint8_t  temp;	
//���±���������ť,�ر���������,����30����,  	
#ifdef __DoorMagic
        if(PB8==0)
        {    while(PB8==0);
           if(Key_DI1.Alarm==2) //������
             { Key_Delay1.HideSound=0;  
	       DigitalOutput(BellLed,0);
	        Key_Delay1.Key_IntervalTimeOvr=0;
	        if(ParaList[cRuntimeDO1Ratio*2+1]==0)
	          Key_Delay1.Key_Interval=10;   //�����0��,Ϊ���Ե�,����10s
	        else
	          Key_Delay1.Key_Interval=1800;   //30����
      	        Key_Delay1.Key_IntervalBegin=1;
	     }  	
	}
       if((Key_DI1.Alarm==2)&&(Key_Delay1.Key_IntervalTimeOvr==1))
        { Key_Delay1.Key_IntervalTimeOvr=0;
          Key_Delay1.HideSound=1;  
	  DigitalOutput(BellLed,1);
        }
//����ǵ�ǰ���õ�ʱ��
temp=0;
 if(PA3==0)
   temp+=1;
 if(PA4==0)
   temp+=2; 
 if(PC7=0)
   temp+=4;  
 if(PA2=0)
   temp+=8; 
   ParaList[cRuntimeDO1Ratio*2+1]=temp; 
 #endif          
 }	