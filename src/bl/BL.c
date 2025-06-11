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
  	 u16 LEDSetDelayCount; //��λ����
  	 u8 LEDDelay;  //����ͳ�ƶ��ٸ���ʱ�ж�����һ�����㵥Ԫ
         u16 AskLEDOnCount; //���õ�on����
         u16 AskLEDOffCount; //���õ�off����
         u8 AskLEDTime;      //���õ��������
         u8 LEDOnDelayFlag;  //machine�д���on���ڱ�־
         u8 LEDOffDelayFlag;  //machine �д���off���ڱ�־
   	 u8 LEDtimes;
   	
   }; 
    
struct Bell_Struct   Bell;  

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
Function:void  LEDOff()
Input:  
Output:
Description:�رշ�����
------------------------------------------------*/  
void BellOff()
 {
	 PWM_Stop(PWM0, PWM_CH_3_MASK);
 } 
 
/*----------------------------------------------
Function:void  LEDOn()
Input:  
Output:
Description:����������
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
Description:��ʼ��������LED PC4
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
Input:    uchar Times:  �������������
           u16 DuringOn:ÿ������ʱ��  ��λ:ms
           u16 DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��,�ó��������һ������,�������������������е���״̬��ʵ��
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
Input:    uchar Times:  �������������
           uchar DuringOn:ÿ������ʱ��  ��λ:ms
           uchar DuringOff:ÿ��ֹͣʱ�� ��λ:ms
Output:
Description:����������������һ��ʱ��
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
Description:�ڶ�ʱ����ʵ��LED�Ĳ���,��Ҫ���ݶ�ʱ�������ڼ����趨ֵ
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
Description:�����������״̬���߳�
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
