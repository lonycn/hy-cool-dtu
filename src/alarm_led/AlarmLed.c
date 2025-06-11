#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"
#include "alarm.h"
#if  1
 struct LED_Struct
   {  
   	 u8 LEDStatus;
  	 u8 LEDFlag;
  	 u16 LEDSetDelayCount; //单位计数
  	 u8 LEDDelay;  //用来统计多少个定时中断算作一个计算单元
         u16 AskLEDOnCount; //设置的on周期
         u16 AskLEDOffCount; //设置的off周期
         u8 AskLEDTime;      //设置的鸣响次数
         u8 LEDOnDelayFlag;  //machine中处于on周期标志
         u8 LEDOffDelayFlag;  //machine 中处于off周期标志
   	 u8 LEDtimes;
   	
   }; 
    
struct LED_Struct   AlarmLED;  

/*LED程序的使用说明:
  调用方式一:  使用线程调用
   step1:  程序开始调用LEDInital()
   step2:  while主循环中调用StatusMachine_LEDLED()
   step3:  定时中断程序中调用Timer_LEDControl()
   step4:  需要LED操作的地方调用LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
           Ondelays实际时间=LEDDelay*Ondelays*timer定时时间  
 调用方式二:  直接延时调用
   step1:  程序开始调用LEDInital()
   step2:  无
   step3:  定时中断程序中调用Timer_LEDControl()
   step4:  需要LED操作的地方调用LEDLEDLED(uchar Times,u16 DuringOn,u16 DuringOff)
 */
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:初始化蜂鸣器LED PC4
------------------------------------------------*/  	
void AlarmLEDInital()
  {
     #ifdef _LoraNode
       GPIO_SetMode(PB, BIT8, GPIO_PMD_INPUT);
     #else
        #ifdef __DoorMagic
           GPIO_SetMode(PB, BIT8, GPIO_PMD_INPUT);
        #else
        GPIO_SetMode(PB, BIT8, GPIO_PMD_OUTPUT);
      #endif
    #endif 
    
	 AlarmLED.LEDStatus=cLEDIdle;
  	 AlarmLED.LEDFlag=0;
  	 AlarmLED.LEDSetDelayCount=0;
     #ifndef  _LoraNode	
  	#ifndef __DoorMagic
  	 AlarmLEDOff();
  	#endif  
     #endif	
   }	

/*----------------------------------------------
Function:void  LEDOff()
Input:  
Output:
Description:关闭蜂鸣器
------------------------------------------------*/  
void AlarmLEDOff()
 { 
#ifndef  _LoraNode	
   PB8=0;
   #endif
 } 
 
/*----------------------------------------------
Function:void  LEDOn()
Input:  
Output:
Description:开启蜂鸣器
------------------------------------------------*/   
void AlarmLEDOn()
 { 
 	 #ifndef  _LoraNode	
    PB8=1;
 #endif    
 }


void AlarmLEDWaitDelay()
 {
 
}	
 

/*----------------------------------------------
Function:void LEDLEDLED(uchar Times,uchar DuringOn,uchar DuringOff)
Input:    uchar Times:  蜂鸣器鸣响次数
           uchar DuringOn:每次鸣响时间  单位:ms
           uchar DuringOff:每次停止时间 单位:ms
Output:
Description:控制器蜂鸣器鸣响一段时间
------------------------------------------------*/   
void AlarmLEDLEDLED(u8 Times,u16 DuringOn,u16 DuringOff)
 {
    #ifndef  _LoraNode	
    AlarmLEDcontrol(Times, DuringOn,DuringOff );
    while(AlarmLED.LEDFlag==0)
     {
         AlarmLEDWaitDelay();
     }	
 #endif
  } 	 


/*----------------------------------------------
Function:void LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
Input:    uchar Times:  蜂鸣器鸣响次数
           u16 DuringOn:每次鸣响时间  单位:ms
           u16 DuringOff:每次停止时间 单位:ms
Output:
Description:控制器蜂鸣器鸣响一段时间,该程序仅仅做一个设置,其他的流程在主程序中调用状态机实现
------------------------------------------------*/   
void AlarmLEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
  {   
		#ifndef  _LoraNode	
  	  AlarmLED.LEDFlag=1;
       
       AlarmLED.LEDDelay=0;
       AlarmLED.LEDSetDelayCount=0;
       AlarmLED.AskLEDOnCount=Ondelays;
       AlarmLED.AskLEDOffCount=Offdelays;
       AlarmLED.AskLEDTime=times;
       AlarmLED.LEDOnDelayFlag=0;
       AlarmLED.LEDOffDelayFlag=0;  
 #endif    
}
/*----------------------------------------------
Function:Timer_LEDControl()
Input:    None
Output:
Description:在定时器中实现LED的操作,需要根据定时器的周期计算设定值
------------------------------------------------*/
#define  cLEDDelay 10
void Timer_AlarmLEDControl()
 { 
	 #ifndef  _LoraNode	
   if(AlarmLED.LEDFlag==1)	
     { 
 	  AlarmLED.LEDSetDelayCount++;
	 if(AlarmLED.LEDSetDelayCount>=AlarmLED.AskLEDOnCount)
	   AlarmLED.LEDOnDelayFlag=1;
	 if(AlarmLED.LEDSetDelayCount>=AlarmLED.AskLEDOffCount)
	    AlarmLED.LEDOffDelayFlag=1;  
 }  
 #endif
} 

/*----------------------------------------------
Function:StatusMachine_LEDLED()
Input:    None
Output:
Description:处理蜂鸣器的状态机线程
------------------------------------------------*/   	
void StatusMachine_AlarmLEDLED()
 { 
	 #ifndef  _LoraNode	
   switch(AlarmLED.LEDStatus)	
 	{
 		case cLEDIdle: if(AlarmLED.LEDFlag==1)
		             {     AlarmLED.LEDStatus=cLEDOn;
 		                   AlarmLED.LEDDelay=0;
 		                   AlarmLED.LEDSetDelayCount=0;
 		                   AlarmLED.LEDtimes=0;
				   AlarmLEDOn();
			     }
 		                 break;  
 	       case cLEDOn:     if(AlarmLED.LEDOnDelayFlag==1)
 	                           {
 	                             AlarmLED.LEDOnDelayFlag=0;
 	                             AlarmLED.LEDOffDelayFlag=0;
 	                              AlarmLEDOff();
 	                                          AlarmLED.LEDDelay=0;
 		                                  AlarmLED.LEDSetDelayCount=0;
 		                                  AlarmLED.LEDStatus=cLEDOff;
 		                    }        
				 break;
 	         case cLEDOff:  
 	         
 	                    if(AlarmLED.LEDOffDelayFlag==1)
 	                           {
 	                             AlarmLED.LEDOnDelayFlag=0;
 	                             AlarmLED.LEDOffDelayFlag=0;
 	                              
 	                                          AlarmLED.LEDDelay=0;
 		                                  AlarmLED.LEDSetDelayCount=0;
 		                                  
 		                   
 	                                 AlarmLED.LEDtimes++;
 	                                 if(AlarmLED.AskLEDTime!=255) 
 	                                  {
 	                                  if(AlarmLED.LEDtimes>=AlarmLED.AskLEDTime) 
 	                                    { AlarmLED.LEDFlag=0;
 	                                       AlarmLED.LEDStatus=cLEDIdle;
 	                                     }
 	                                   else
					    {
					    	AlarmLED.LEDStatus=cLEDOn;
					    	AlarmLEDOn();			
					    }														 
					}
 	                              else
 	                                 {
					    	AlarmLED.LEDStatus=cLEDOn;
					    	AlarmLEDOn();			
					    }	
 	                                }
 	                          break;
 	         default:break;
 	}
 #endif	
 }	                         


void  AlarmWorkledInital(void)
  {
  		#ifndef  _LoraNode	
  	GPIO_SetMode(PB, BIT8, GPIO_PMD_OUTPUT);
    #endif	
  }


void Test_AlarmLedIntial()
  {
  	AlarmLEDcontrol(255, 2,2);
  }
  
void  Test_AlarmLed() 
  {
  	StatusMachine_AlarmLEDLED();
  }
	
 #endif 	
  	
