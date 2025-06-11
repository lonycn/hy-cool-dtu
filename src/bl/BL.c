#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"


#define  u8     unsigned char
#define  u16    uint16_t

#define  cLEDIdle   0
#define  cLEDOn     1
#define  cLEDOff    2
 struct Bell_Struct
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
    
struct Bell_Struct   Bell;  

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
Function:void  LEDOff()
Input:  
Output:
Description:关闭蜂鸣器
------------------------------------------------*/  
void BellOff()
 {
	 PWM_Stop(PWM0, PWM_CH_3_MASK);
 } 
 
/*----------------------------------------------
Function:void  LEDOn()
Input:  
Output:
Description:开启蜂鸣器
------------------------------------------------*/   
void BellOn()
 {

	 //PWM_EnableOutput(PWM0, PWM_CH_3_MASK);
	// PWM_Start(PWM0, PWM_CH_3_MASK);
	 PWM_init ();
	 
   // BellOff(); 
 }
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:初始化蜂鸣器LED PC4
------------------------------------------------*/
int32_t PWM_init (void)
{
SYS_UnlockReg();
CLK_EnableModuleClock(PWM0_CH23_MODULE);
CLK_SetModuleClock(PWM0_CH23_MODULE, CLK_CLKSEL1_PWM0_CH23_S_HCLK, 0);
 SYS->PA_L_MFP = (SYS->PA_L_MFP &(~SYS_PA_L_MFP_PA6_MFP_Msk ))| SYS_PA_L_MFP_PA6_MFP_PWM0_CH3; 
 /* PWM0 frequency is 100Hz, duty 30% */
 PWM_ConfigOutputChannel(PWM0, 3, 2700, 30);
 PWM_EnableOutput(PWM0, PWM_CH_3_MASK);
  SYS_LockReg();
	PWM_Start(PWM0, PWM_CH_3_MASK);
	
}
 

void BellInital()
  {  
		PWM_init();
	  BellOff();
	 Bell.LEDStatus=cLEDIdle;
  	 Bell.LEDFlag=0;
  	 Bell.LEDSetDelayCount=0;
	
  

 }
	



void BellWaitDelay()
 {
 
}	




/*----------------------------------------------
Function:void LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
Input:    uchar Times:  蜂鸣器鸣响次数
           u16 DuringOn:每次鸣响时间  单位:ms
           u16 DuringOff:每次停止时间 单位:ms
Output:
Description:控制器蜂鸣器鸣响一段时间,该程序仅仅做一个设置,其他的流程在主程序中调用状态机实现
------------------------------------------------*/   
void Bellcontrol(u8  times, u16 Ondelays,u16 Offdelays )
  {    Bell.LEDFlag=1;
       
       Bell.LEDDelay=0;
       Bell.LEDSetDelayCount=0;
       Bell.AskLEDOnCount=Ondelays;
       Bell.AskLEDOffCount=Offdelays;
       Bell.AskLEDTime=times;
       Bell.LEDOnDelayFlag=0;
       Bell.LEDOffDelayFlag=0;  
     
}


/*----------------------------------------------
Function:void LEDLEDLED(uchar Times,uchar DuringOn,uchar DuringOff)
Input:    uchar Times:  蜂鸣器鸣响次数
           uchar DuringOn:每次鸣响时间  单位:ms
           uchar DuringOff:每次停止时间 单位:ms
Output:
Description:控制器蜂鸣器鸣响一段时间
------------------------------------------------*/   
void BellLEDLED(u8 Times,u16 DuringOn,u16 DuringOff)
 {
   
    Bellcontrol(Times, DuringOn,DuringOff );
    while(Bell.LEDFlag==0)
     {
         BellWaitDelay();
     }	
 
  } 	 

/*----------------------------------------------
Function:Timer_LEDControl()
Input:    None
Output:
Description:在定时器中实现LED的操作,需要根据定时器的周期计算设定值
------------------------------------------------*/
#define  cLEDDelay 10
void Timer_BellControl()
 {
   if(Bell.LEDFlag==1)	
     { 
 	  Bell.LEDSetDelayCount++;
	 if(Bell.LEDSetDelayCount>=Bell.AskLEDOnCount)
	   Bell.LEDOnDelayFlag=1;
	 if(Bell.LEDSetDelayCount>=Bell.AskLEDOffCount)
	    Bell.LEDOffDelayFlag=1;  
 }  
 
} 

/*----------------------------------------------
Function:StatusMachine_LEDLED()
Input:    None
Output:
Description:处理蜂鸣器的状态机线程
------------------------------------------------*/   	
void StatusMachine_BellLED()
 {
   switch(Bell.LEDStatus)	
 	{
 		case cLEDIdle: if(Bell.LEDFlag==1)
		             {     Bell.LEDStatus=cLEDOn;
 		                   Bell.LEDDelay=0;
 		                   Bell.LEDSetDelayCount=0;
 		                   Bell.LEDtimes=0;
				   BellOn();
			     }
 		                 break;  
 	       case cLEDOn:     if(Bell.LEDOnDelayFlag==1)
 	                           {
 	                             Bell.LEDOnDelayFlag=0;
 	                             Bell.LEDOffDelayFlag=0;
 	                              BellOff();
 	                                          Bell.LEDDelay=0;
 		                                  Bell.LEDSetDelayCount=0;
 		                                  Bell.LEDStatus=cLEDOff;
 		                    }        
				 break;
 	         case cLEDOff:  
 	         
 	                    if(Bell.LEDOffDelayFlag==1)
 	                           {
 	                             Bell.LEDOnDelayFlag=0;
 	                             Bell.LEDOffDelayFlag=0;
 	                              
 	                                          Bell.LEDDelay=0;
 		                                  Bell.LEDSetDelayCount=0;
 		                                  
 		                   
 	                                 Bell.LEDtimes++;
 	                                 if(Bell.AskLEDTime!=255) 
 	                                  {
 	                                  if(Bell.LEDtimes>=Bell.AskLEDTime) 
 	                                    { Bell.LEDFlag=0;
 	                                       Bell.LEDStatus=cLEDIdle;
 	                                     }
 	                                   else
					    {
					    	Bell.LEDStatus=cLEDOn;
					    	BellOn();			
					    }														 
					}
 	                              else
 	                                 {
					    	Bell.LEDStatus=cLEDOn;
					    	BellOn();			
					    }	
 	                                }
 	                          break;
 	         default:break;
 	}
 }	                         




void Test_BellIntial()
  {
  	Bellcontrol(255, 2,2);
  }
  
void  Test_Bell() 
  {
  	StatusMachine_BellLED();
  }
	
  	
void  StopBell()
  {
    Bell.LEDFlag=0;
    Bell.LEDStatus=cLEDIdle;
    BellOff();	
  }	  	
void HardWaitBell(u8  times, u16 Ondelays,u16 Offdelays )
 {
    Bellcontrol(times,Ondelays,Offdelays );
    while(1)
     {  WatchdogReset();
     	StatusMachine_BellLED();
     	if(Bell.LEDFlag==0)
     	  break;
     }	
     		
 }	
