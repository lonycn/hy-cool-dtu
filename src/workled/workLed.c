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
  	 u16 LEDSetDelayCount; //��λ����
  	 u8 LEDDelay;  //����ͳ�ƶ��ٸ���ʱ�ж�����һ�����㵥Ԫ
         u16 AskLEDOnCount; //���õ�on����
         u16 AskLEDOffCount; //���õ�off����
         u8 AskLEDTime;      //���õ��������
         u8 LEDOnDelayFlag;  //machine�д���on���ڱ�־
         u8 LEDOffDelayFlag;  //machine �д���off���ڱ�־
   	 u8 LEDtimes;
		 u8  Rstcount;
   	
   }; 
    
struct LED_Struct   LED;  

/*LED�����ʹ��˵��:
  ���÷�ʽһ:  ʹ���̵߳���
   step1:  ����ʼ����LEDInital()
   step2:  while��ѭ���е���StatusMachine_LEDLED()
   step3:  ��ʱ�жϳ����е���Timer_LEDControl()
   step4:  ��ҪLED�����ĵط�����LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
           Ondelaysʵ��ʱ��=LEDDelay*Ondelays*timer��ʱʱ��  
 ���÷�ʽ��:  ֱ����ʱ����
   step1:  ����ʼ����LEDInital()
   step2:  ��
   step3:  ��ʱ�жϳ����е���Timer_LEDControl()
   step4:  ��ҪLED�����ĵط�����LEDLEDLED(uchar Times,u16 DuringOn,u16 DuringOff)
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
Description:��ʼ��������LED PC4
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
Description:�رշ�����
------------------------------------------------*/  

	 void LEDOff()
 {
   PC8=0;
	
 } 
 
/*----------------------------------------------
Function:void  LEDOn()
Input:  
Output:
Description:����������
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
Input:    uchar Times:  �������������
           uchar DuringOn:ÿ������ʱ��  ��λ:ms
           uchar DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��
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
Input:    uchar Times:  �������������
           u16 DuringOn:ÿ������ʱ��  ��λ:ms
           u16 DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��,�ó��������һ������,�������������������е���״̬��ʵ��
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
Description:�ڶ�ʱ����ʵ��LED�Ĳ���,��Ҫ���ݶ�ʱ�������ڼ����趨ֵ
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
Description:�����������״̬���߳�
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