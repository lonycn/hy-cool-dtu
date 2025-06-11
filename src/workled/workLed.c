#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"


#include "led.h"
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
		 u8  Rstcount;
   	
   }; 
    
struct LED_Struct   LED;  

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
void   FeedDogInitial()  //20240416
{
  GPIO_SetMode(PC, BIT15, GPIO_PMD_OUTPUT);
  PC15=0;
}	
	 
	 
	 
	 
	 
	 
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:初始化蜂鸣器LED PC4
------------------------------------------------*/  	
void  LEDInital()
  {
	
  	GPIO_SetMode(PC, BIT8, GPIO_PMD_OUTPUT);
	  GPIO_SetMode(PD, BIT7, GPIO_PMD_OUTPUT); 
		GPIO_SetMode(PD, BIT6, GPIO_PMD_OUTPUT);
		PD6=1;
		PD7=1;
	 LED.LEDStatus=cLEDIdle;
  	 LED.LEDFlag=0;
  	 LED.LEDSetDelayCount=0;
		
		FeedDogInitial();
		
   }	

/*----------------------------------------------
Function:void  LEDOff()
Input:  
Output:
Description:关闭蜂鸣器
------------------------------------------------*/  

	 void LEDOff()
 {
   PC8=0;
	
 } 
 
/*----------------------------------------------
Function:void  LEDOn()
Input:  
Output:
Description:开启蜂鸣器
------------------------------------------------*/   
void LEDOn()
 { LED.Rstcount++;
    PC8=1;
	 if(LED.Rstcount==1)
	   PC15=1;  //20240416
   else if(LED.Rstcount==2)
	 {  PC15=0;
	// else if(LED.Rstcount>=3)
		 LED.Rstcount=0;
	 }
 }


void LEDWaitDelay()
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
void LEDLEDLED(u8 Times,u16 DuringOn,u16 DuringOff)
 {
   
    LEDcontrol(Times, DuringOn,DuringOff );
    while(LED.LEDFlag==0)
     {
         LEDWaitDelay();
     }	
 
  } 	 


/*----------------------------------------------
Function:void LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
Input:    uchar Times:  蜂鸣器鸣响次数
           u16 DuringOn:每次鸣响时间  单位:ms
           u16 DuringOff:每次停止时间 单位:ms
Output:
Description:控制器蜂鸣器鸣响一段时间,该程序仅仅做一个设置,其他的流程在主程序中调用状态机实现
------------------------------------------------*/   
void LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
  {    LED.LEDFlag=1;
       
       LED.LEDDelay=0;
       LED.LEDSetDelayCount=0;
       LED.AskLEDOnCount=Ondelays;
       LED.AskLEDOffCount=Offdelays;
       LED.AskLEDTime=times;
       LED.LEDOnDelayFlag=0;
       LED.LEDOffDelayFlag=0;  
     
}
/*----------------------------------------------
Function:Timer_LEDControl()
Input:    None
Output:
Description:在定时器中实现LED的操作,需要根据定时器的周期计算设定值
------------------------------------------------*/
#define  cLEDDelay 10
void Timer_LEDControl()
 {
   if(LED.LEDFlag==1)	
     { 
 	  LED.LEDSetDelayCount++;
	 if(LED.LEDSetDelayCount>=LED.AskLEDOnCount)
	   LED.LEDOnDelayFlag=1;
	 if(LED.LEDSetDelayCount>=LED.AskLEDOffCount)
	    LED.LEDOffDelayFlag=1;  
 }  
 
} 

/*----------------------------------------------
Function:StatusMachine_LEDLED()
Input:    None
Output:
Description:处理蜂鸣器的状态机线程
------------------------------------------------*/   	
void StatusMachine_LEDLED()
 {
   switch(LED.LEDStatus)	
 	{
 		case cLEDIdle: if(LED.LEDFlag==1)
		             {     LED.LEDStatus=cLEDOn;
 		                   LED.LEDDelay=0;
 		                   LED.LEDSetDelayCount=0;
 		                   LED.LEDtimes=0;
				   LEDOn();
			     }
 		                 break;  
 	       case cLEDOn:     if(LED.LEDOnDelayFlag==1)
 	                           {
 	                             LED.LEDOnDelayFlag=0;
 	                             LED.LEDOffDelayFlag=0;
 	                              LEDOff();
 	                                          LED.LEDDelay=0;
 		                                  LED.LEDSetDelayCount=0;
 		                                  LED.LEDStatus=cLEDOff;
 		                    }        
				 break;
 	         case cLEDOff:  
 	         
 	                    if(LED.LEDOffDelayFlag==1)
 	                           {
 	                             LED.LEDOnDelayFlag=0;
 	                             LED.LEDOffDelayFlag=0;
 	                              
 	                                          LED.LEDDelay=0;
 		                                  LED.LEDSetDelayCount=0;
 		                                  
 		                   
 	                                 LED.LEDtimes++;
 	                                 if(LED.AskLEDTime!=255) 
 	                                  {
 	                                  if(LED.LEDtimes>=LED.AskLEDTime) 
 	                                    { LED.LEDFlag=0;
 	                                       LED.LEDStatus=cLEDIdle;
 	                                     }
 	                                   else
					    {
					    	LED.LEDStatus=cLEDOn;
					    	LEDOn();			
					    }														 
					}
 	                              else
 	                                 {
					    	LED.LEDStatus=cLEDOn;
					    	LEDOn();			
					    }	
 	                                }
 	                          break;
 	         default:break;
 	}
 }	                         


void  WorkledInital(void)
  {
  	GPIO_SetMode(PA, BIT1, GPIO_PMD_OUTPUT);

  }


void Test_LedIntial()
  {
  	LEDcontrol(255, 2,2);
  }
  
void  Test_Led() 
  {
  	StatusMachine_LEDLED();
  }	
  	
  	
void  StopWorkLed()
  {
    LED.LEDFlag=0;
    LED.LEDStatus=cLEDIdle;
    LEDOff();	
  }	  	