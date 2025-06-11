/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 14/09/11 7:38p $
 * @brief    Use Polling mode to check WDT time-out state and reset WDT after time-out occurs.
 *
 * @note
 * Copyright (C) 2013-2014 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

void SetNormalWatch()
  {
     WatchDogTestFlag=0;	
  	
  }
void 	ResetNormalWatch()
{
	 WatchDogTestFlag=1;	
}


#ifndef _LoraNode 
void WDT_IRQHandler(void)
{
#if 1
    // Reload WWDT counter and clear WWDT interrupt flag
if(WatchDogTestFlag==0)
   { WWDT_RELOAD_COUNTER();
    WWDT_CLEAR_INT_FLAG();
  //  printf("WWDT counter reload\n");
   }
#endif
}

#endif
void watchdog_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Waiting for LIRC clock ready */
    CLK_WaitClockReady( CLK_CLKSTATUS_LIRC_STB_Msk);

    CLK_EnableModuleClock(WDT_MODULE);

    CLK_SetModuleClock(WDT_MODULE, 0, 0);
    //WDT_Open(WDT_TIMEOUT_2POW18, WDT_RESET_DELAY_1026CLK, TRUE, FALSE);
    WWDT_Open(WWDT_PRESCALER_768, 0x20, TRUE);
    NVIC_EnableIRQ(WDT_IRQn);
    SYS_LockReg();
    
   
}

void WatchdogReset()
{
	#if 0
  if(WDT_GET_TIMEOUT_INT_FLAG())
       
            WDT_CLEAR_TIMEOUT_INT_FLAG();
  #endif         
}	




//add  2021-04-30 12:26

__IO uint32_t  _Pin_Setting[11];    /* store Px_H_MFP and Px_L_MFP */
__IO uint32_t  _PullUp_Setting[6];  /* store GPIOx_PUEN */



/**
  * @brief  Store original setting of multi-function pin selection.
  * @param  None.
  * @return None.
  */
void SavePinSetting()
{
    /* Save Pin selection setting */
    _Pin_Setting[0] = SYS->PA_L_MFP;
    _Pin_Setting[1] = SYS->PA_H_MFP;
    _Pin_Setting[2] = SYS->PB_L_MFP;
    _Pin_Setting[3] = SYS->PB_H_MFP;
    _Pin_Setting[4] = SYS->PC_L_MFP;
    _Pin_Setting[5] = SYS->PC_H_MFP;
    _Pin_Setting[6] = SYS->PD_L_MFP;
    _Pin_Setting[7] = SYS->PD_H_MFP;
    _Pin_Setting[8] = SYS->PE_L_MFP;
    _Pin_Setting[9] = SYS->PE_H_MFP;
    _Pin_Setting[10] = SYS->PF_L_MFP;

    /* Save Pull-up setting */
    _PullUp_Setting[0] =  PA->PUEN;
    _PullUp_Setting[1] =  PB->PUEN;
    _PullUp_Setting[2] =  PC->PUEN;
    _PullUp_Setting[3] =  PD->PUEN;
    _PullUp_Setting[4] =  PE->PUEN;
    _PullUp_Setting[5] =  PF->PUEN;
}


/**
  * @brief  Restore original setting of multi-function pin selection.
  * @param  None.
  * @return None.
  */
void RestorePinSetting()
{
    /* Restore Pin selection setting */
    SYS->PA_L_MFP = _Pin_Setting[0];
    SYS->PA_H_MFP = _Pin_Setting[1];
    SYS->PB_L_MFP = _Pin_Setting[2];
    SYS->PB_H_MFP = _Pin_Setting[3];
    SYS->PC_L_MFP = _Pin_Setting[4];
    SYS->PC_H_MFP = _Pin_Setting[5];
    SYS->PD_L_MFP = _Pin_Setting[6];
    SYS->PD_H_MFP = _Pin_Setting[7];
    SYS->PE_L_MFP = _Pin_Setting[8];
    SYS->PE_H_MFP = _Pin_Setting[9];
    SYS->PF_L_MFP = _Pin_Setting[10];

    /* Restore Pull-up setting */
    PA->PUEN = _PullUp_Setting[0];
    PB->PUEN = _PullUp_Setting[1];
    PC->PUEN = _PullUp_Setting[2];
    PD->PUEN = _PullUp_Setting[3];
    PE->PUEN = _PullUp_Setting[4];
    PF->PUEN = _PullUp_Setting[5];
}


/**
  * @brief  Save multi-function pin setting and then go to power down.
  * @param  None.
  * @return None.
  */
#define  buz_Pull_Dis  ((0x0001<<6)^0xffff)
#define  led_Pull_Dis  ((0x0001<<2)^0xffff)



void  DealPull()
{
	PA->PUEN &= buz_Pull_Dis;
	PA->PUEN &=led_Pull_Dis;
//	GPIO_SetMode(PA, BIT2, GPIO_PMD_OUTPUT);
//	GPIO_SetMode(PA, BIT6, GPIO_PMD_OUTPUT);
}


void Enter_PowerDown()
{
    /* Back up original setting */
     
    SavePinSetting();

    /* Set function pin to GPIO mode */
    SYS->PA_L_MFP = 0;
    SYS->PA_H_MFP = 0;
    SYS->PB_L_MFP = 0;
    SYS->PB_H_MFP = 0;
    SYS->PC_L_MFP = 0;
    SYS->PC_H_MFP = 0;
    SYS->PD_L_MFP = 0;
    SYS->PD_H_MFP = 0;
    SYS->PE_L_MFP = 0;
    SYS->PE_H_MFP = 0;
    SYS->PF_L_MFP = 0x00007700;
  //   SYS->PF_L_MFP =0;
    /* Enable GPIO pull up */
    PA->PUEN = 0xFFFF;
    PB->PUEN = 0xFFFF;
    PC->PUEN = 0xFFFF;
    PD->PUEN = 0xFFFF;
    PE->PUEN = 0xFFFF;
    PF->PUEN = 0x0033;      /* exclude GPF2 and GPF3 which are HXT OUT/IN */
 //PF->PUEN = 0x00FF;
#if 0	 
    PA->PUEN = 0xFFBD;
    PB->PUEN = 0x6F7F;
#endif	 
     PA->PUEN = 0xFFFD;
#if 0
    /* Disable LCD clock */
    CLK->APBCLK &= ~CLK_APBCLK_LCD_EN; /* Disable LCD clock */
  
  CLK->PWRCTL &= ~CLK_PWRCTL_LIRC_EN_Msk; /* disable LXT - 32Khz */
#endif   
   DealPull();
   CLK_PowerDown();

}


/**
  * @brief  This function will not be called.
  * @param  None.
  * @return None.
  */
void Leave_PowerDown()
{

    /* Restore pin setting */
    RestorePinSetting();

    /* Set PF.0 and PF.1 to ICE Data and Clock */
    SYS->PF_L_MFP |= 0x00000077;

 #if  0 
    /* Enable LCD clock */
    CLK->PWRCTL |= CLK_PWRCTL_LXT_EN_Msk; /* enable LXT - 32Khz */
    CLK->APBCLK |= CLK_APBCLK_LCD_EN; /* enable LCD clock */
 #endif
}


