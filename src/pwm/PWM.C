/**************************************************************************//**
 * @file     PWM.c
 * @version  V1.00
 * $Revision: 14 $
 * $Date: 14/09/04 11:58a $
 * @brief    NANO100 series PWM driver source file
 *
 * @note
 * Copyright (C) 2013-2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "Nano100Series.h"

#include <stdio.h>
#include "Nano100Series.h"


#define cIdle   0 
#define cWait   1
#define cWaitInterval  2


struct pwm_Struct
   {  
   	 uint8_t AdcStatus;
  	 uint8_t AdcSampleInterval;
         uint8_t AdcIntervalBegin;
         uint8_t AdcIntervalTimeOvr;
         uint8_t AdcSampleIntervalSet;
         uint8_t BellOn;
         
   }; 
    
struct pwm_Struct   PWM;  


void Timer_PWM_Interval()
  {
    if(PWM.AdcIntervalBegin==1)
      {
      	 PWM.AdcSampleInterval++;
      	 if(PWM.AdcSampleInterval>=PWM.AdcSampleIntervalSet)
      	   { PWM.AdcIntervalBegin=0;
      	     PWM.AdcIntervalTimeOvr=1;
          }		
       }	
  }

/** @addtogroup NANO100_PWM_EXPORTED_FUNCTIONS PWM Exported Functions
  @{
*/

/**
 * @brief This function config PWM generator and get the nearest frequency in edge aligned auto-reload mode
 * @param[in] pwm The base address of PWM module
 * @param[in] u32ChannelNum PWM channel number. Valid values are between 0~5
 * @param[in] u32Frequency Target generator frequency
 * @param[in] u32DutyCycle Target generator duty cycle percentage. Valid range are between 0 ~ 100. 10 means 10%, 20 means 20%...
 * @return Nearest frequency clock in nano second
 * @note Since every two channels, (0 & 1), (2 & 3), (4 & 5), shares a prescaler. Call this API to configure PWM frequency may affect
 *       existing frequency of other channel.
 */
void  PWM_Initial( )
   {
     PWM_ConfigOutputChannel(PWM0,3,27000,50);
     PWM.AdcStatus=cIdle;
     PWM.BellOn=1;
   }
   

void PWM_StateMachine()
 {
   switch(PWM.AdcStatus)
      {   
      	case cIdle: 
                 if(PWM.BellOn==1)
                  {
                   PWM.AdcStatus=cWaitInterval;
                   PWM.AdcSampleInterval=0;
                   PWM.AdcIntervalBegin=1;
                   PWM.AdcIntervalTimeOvr=0;
                   PWM.AdcSampleIntervalSet=2;
                   PWM_Start(PWM0,8) ;
                  }       
                  break;
                  
         
         case cWaitInterval:
               if(PWM.AdcIntervalTimeOvr==1)
                { 
                   PWM_Stop(PWM0, 8) ;
									//PWM.BellOn=0;
                   PWM.AdcStatus=cIdle;
									 
                }  
               break;  
         default:   PWM.AdcStatus=cIdle;
                  break;    	       
        }         
   }
