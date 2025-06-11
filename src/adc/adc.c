/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 10 $
 * $Date: 15/06/16 7:23p $
 * @brief    Convert ADC channel 0 in Single mode and print conversion results.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano100Series.h"

volatile uint8_t u8ADF;
#define cIdle   0 
#define cWait   1
#define cWaitInterval  2


struct ADC_Struct
   {  
   	 uint8_t AdcStatus;
  	 uint16_t u32Result_0[10];
   	 uint16_t u32Result_1[10];
   	 uint16_t u32Result_2[10];
   	 uint16_t u32Result_000;
   	 uint16_t u32Result_100;
   	 uint16_t u32Result_200;
   	 uint8_t AdcSampleInterval;
         uint8_t AdcIntervalBegin;
         uint8_t AdcIntervalTimeOvr;
         uint8_t AdcSampleIntervalSet;
         uint8_t AdcCount;
   }; 
    
struct ADC_Struct   ADC_BatVol; 

/*--------------------------------------------------------------------------------------------
//Function: GetAdc(uint8_t channel)	 
//Input:  
//Output:   None 
//Description:外部通过调用这个函数获取相应通道的值
//--------------------------------------------------------------------------------------------*/
uint32_t  GetAdc(uint8_t channel)	 
	  { 
	     switch(channel)
			 {
				                                case   3:  
				                                return(ADC_BatVol.u32Result_000);break;
				 				 case   2: return(ADC_BatVol.u32Result_100);break;
				 				 case   1: return(ADC_BatVol.u32Result_200);break;
				 default:break;
			 }				 
    }

		
void Timer_AdcInterval()
  {
    if(ADC_BatVol.AdcIntervalBegin==1)
      {
      	 ADC_BatVol.AdcSampleInterval++;
      	 if(ADC_BatVol.AdcSampleInterval>=ADC_BatVol.AdcSampleIntervalSet)
      	   { ADC_BatVol.AdcIntervalBegin=0;
      	     ADC_BatVol.AdcIntervalTimeOvr=1;
          }		
       }	
  }



void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT)
        u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#if  0
void Adc_Initial(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
     /* Enable ADC clock */
    CLK_EnableModuleClock(ADC_MODULE);
/* Lock protected registers */
    SYS_LockReg();
    
    
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE, ADC_CH_0_MASK|ADC_CH_4_MASK|ADC_CH_5_MASK);
// Set reference voltage to AVDD
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);
   // Power on ADC
    ADC_POWER_ON(ADC);
    // Enable ADC ADC_IF interrupt
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);

  ADC_BatVol.AdcCount=0;
  ADC_BatVol.AdcStatus=cIdle;
}
#endif
void Adc_Initial(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
     /* Enable ADC clock */
    CLK_EnableModuleClock(ADC_MODULE);
	  CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_ADC_CLK_DIVIDER(10));	
/* Lock protected registers */
       GPIO_SetMode(PA, BIT1, GPIO_PMD_INPUT);
	     GPIO_SetMode(PA ,BIT2, GPIO_PMD_INPUT); 
		   GPIO_SetMode(PA, BIT3, GPIO_PMD_INPUT);     
	 
	        SYS->PA_L_MFP &= ~(SYS_PA_L_MFP_PA1_MFP_Msk);
	        SYS->PA_L_MFP &= ~(SYS_PA_L_MFP_PA2_MFP_Msk);
	        SYS->PA_L_MFP &= ~(SYS_PA_L_MFP_PA3_MFP_Msk);
	   
          SYS->PA_L_MFP |= (SYS_PA_L_MFP_PA1_MFP_ADC_CH1|SYS_PA_L_MFP_PA2_MFP_ADC_CH2|SYS_PA_L_MFP_PA3_MFP_ADC_CH3);
          PA->OFFD|=((1<<1)<<GP_OFFD_OFFD_Pos);	
	        PA->OFFD|=((1<<2)<<GP_OFFD_OFFD_Pos);	
	        PA->OFFD|=((1<<3)<<GP_OFFD_OFFD_Pos);	
	       // SYS_EnableTempCtl(); // Enable Temperature Sensor
         // SYS_EnableIntVRefBGP(1); // Enable Vref is BGP
         ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);
      	// ADC_SET_REF_VOLTAGE(ADC, adcRef);					
					ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE_CYCLE, ADC_CH_1_MASK|ADC_CH_2_MASK|ADC_CH_3_MASK);
	      //  ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE_CYCLE, ADC_CH_0_MASK);

        //  ADC_SetVrefVoltage(1); // Set Vref voltage

    

	  ADC_POWER_ON(ADC);
	 // Enable ADC ADC_IF interrupt
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);			 
   
     
		 SYS_LockReg();
  ADC_BatVol.AdcCount=0;
  ADC_BatVol.AdcStatus=cIdle;
}



void Adc_StateMachine()
 { uint8_t   j;
   uint32_t u32Result_00,u32Result_10,u32Result_20;
   switch(ADC_BatVol.AdcStatus)
      {
      	case cIdle: 
    
    	         u8ADF = 0;
                  ADC_START_CONV(ADC);
                  ADC_BatVol.AdcStatus=cWait;
                  break;
                  
         case cWait:
               if(u8ADF == 1)
                 {
                   ADC_BatVol.u32Result_0[(ADC_BatVol.AdcCount)%10] = ADC_GET_CONVERSION_DATA(ADC, 1);	         
                   ADC_BatVol.u32Result_1[(ADC_BatVol.AdcCount)%10] = ADC_GET_CONVERSION_DATA(ADC, 2);
                   ADC_BatVol.u32Result_2[(ADC_BatVol.AdcCount++)%10] = ADC_GET_CONVERSION_DATA(ADC, 3);
                   ADC_BatVol.AdcStatus=cWaitInterval;
                   ADC_BatVol.AdcSampleInterval=0;
                   ADC_BatVol.AdcIntervalBegin=1;
                   ADC_BatVol.AdcIntervalTimeOvr=0;
                   ADC_BatVol.AdcSampleIntervalSet=2;
                   u32Result_00 = 0;	         
                   u32Result_10 = 0;
                   u32Result_20 = 0;
                   for(j=0;j<10;j++)
                   {
                    u32Result_00+= ADC_BatVol.u32Result_0[j]	;         
                    u32Result_10+= ADC_BatVol.u32Result_1[j]	;   
                    u32Result_20+=  ADC_BatVol.u32Result_2[j]	;   
                   }
                    u32Result_00/= 10;         
                    u32Result_10/= 10;  
                    u32Result_20/= 10;
                    ADC_BatVol.u32Result_000 = u32Result_00;         
                    ADC_BatVol.u32Result_100 = u32Result_10;  
                    ADC_BatVol.u32Result_200=  u32Result_20;
								 }									 
               break;
         case cWaitInterval:
               if(ADC_BatVol.AdcIntervalTimeOvr==1)
                { 
                   ADC_BatVol.AdcStatus=cIdle; 	
                }
               break; 
         default:    ADC_BatVol.AdcStatus=cIdle;
                  break;    	       
        }         
   }






uint8_t WakeUp_CheckPowerVolume()
 { uint8_t   j;
   uint32_t u32Result_00,u32Result_10,u32Result_20;
   
    	         u8ADF = 0;
                  ADC_START_CONV(ADC);
                 while(u8ADF == 0);
                 
                   ADC_BatVol.u32Result_000 = ADC_GET_CONVERSION_DATA(ADC, 0);	         
                   ADC_BatVol.u32Result_100 = ADC_GET_CONVERSION_DATA(ADC, 1);
                   ADC_BatVol.u32Result_200 = ADC_GET_CONVERSION_DATA(ADC, 2);
                  return(GetPower(0));
            
   }
/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/


