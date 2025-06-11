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
  	 u16 LEDSetDelayCount; //��λ����
  	 u8 LEDDelay;  //����ͳ�ƶ��ٸ���ʱ�ж�����һ�����㵥Ԫ
         u16 AskLEDOnCount; //���õ�on����
         u16 AskLEDOffCount; //���õ�off����
         u8 AskLEDTime;      //���õ��������
         u8 LEDOnDelayFlag;  //machine�д���on���ڱ�־
         u8 LEDOffDelayFlag;  //machine �д���off���ڱ�־
   	 u8 LEDtimes;
   	
   }; 
    
struct LED_Struct   AlarmLED;  

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
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:��ʼ��������LED PC4
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
Description:�رշ�����
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
Description:����������
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
Input:    uchar Times:  �������������
           uchar DuringOn:ÿ������ʱ��  ��λ:ms
           uchar DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��
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
Input:    uchar Times:  �������������
           u16 DuringOn:ÿ������ʱ��  ��λ:ms
           u16 DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��,�ó��������һ������,�������������������е���״̬��ʵ��
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
Description:�ڶ�ʱ����ʵ��LED�Ĳ���,��Ҫ���ݶ�ʱ�������ڼ����趨ֵ
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
Description:�����������״̬���߳�
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
  	
