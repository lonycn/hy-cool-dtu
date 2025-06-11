

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
函数功能：switch_Init(void)    拨码初始化            
入口参数：无
返回  值：无
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
函数功能：void  StateMachine_Switch()  拨码状态监测和消音按键处理         
入口参数：无
返回  值：无
*********************************************************/
void  StateMachine_Switch()
 {
   uint8_t  temp;	
//按下报警静音按钮,关闭声音报警,消音30分钟,  	
#ifdef __DoorMagic
        if(PB8==0)
        {    while(PB8==0);
           if(Key_DI1.Alarm==2) //报警中
             { Key_Delay1.HideSound=0;  
	       DigitalOutput(BellLed,0);
	        Key_Delay1.Key_IntervalTimeOvr=0;
	        if(ParaList[cRuntimeDO1Ratio*2+1]==0)
	          Key_Delay1.Key_Interval=10;   //如果在0档,为测试档,消音10s
	        else
	          Key_Delay1.Key_Interval=1800;   //30分钟
      	        Key_Delay1.Key_IntervalBegin=1;
	     }  	
	}
       if((Key_DI1.Alarm==2)&&(Key_Delay1.Key_IntervalTimeOvr==1))
        { Key_Delay1.Key_IntervalTimeOvr=0;
          Key_Delay1.HideSound=1;  
	  DigitalOutput(BellLed,1);
        }
//检查是当前设置的时间
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