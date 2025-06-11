/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 14/09/11 5:32p $
 * @brief    An I2C master/slave demo by connecting I2C0 and I2C1 interface.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano100Series.h"

#define SLAVE_ADDRESS 0x16

typedef void (*I2C_FUNC)(uint32_t u32Status);
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;
uint32_t g_u32SlaveBuffAddr;
uint8_t g_au8MasterTxData[3];
uint8_t g_u8SlvData[256];
uint8_t g_u8MasterRxData;
uint8_t g_au8SlaveRxData[3];
volatile uint8_t g_u8MasterDataLen;
uint8_t g_u8SlaveDataLen;
volatile uint8_t g_u8EndFlag = 0;
uint8_t  g_u8MasterDataLenSet;


uint8_t g_u8DeviceAddr1;
uint32_t g_u32SlaveBuffAddr1;
uint8_t g_au8MasterTxData1[3];
uint8_t g_u8SlvData1[256];
uint8_t g_u8MasterRxData1;
uint8_t g_au8SlaveRxData1[3];
volatile uint8_t g_u8MasterDataLen1;
uint8_t g_u8SlaveDataLen1;
volatile uint8_t g_u8EndFlag1 = 0;
uint8_t g_u8MasterDataLenSet1;

volatile I2C_FUNC s_I2CHandlerFn = NULL;
volatile I2C_FUNC s_I2C1HandlerFn = NULL;
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    // clear interrupt flag
    I2C0->INTSTS = I2C_INTSTS_INTSTS_Msk;

    u32Status = I2C_GET_STATUS(I2C0);
    if (I2C_GET_TIMEOUT_FLAG(I2C0))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    }
    else
    {
        if (s_I2CHandlerFn != NULL)
            s_I2CHandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    // clear interrupt flag
    I2C1->INTSTS = I2C_INTSTS_INTSTS_Msk;

    u32Status = I2C_GET_STATUS(I2C1);
    if (I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if (s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1)); /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen != 2)
        {
            I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_SI);
        }
    }
    else if (u32Status == 0x10)                 /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1) | 0x01);  /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x40)                 /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x58)                 /* DATA has been received and NACK has been returned */
    {
        g_u8MasterRxData = I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
        g_u8EndFlag = 1;
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted */
    {
        I2C_SET_DATA(I2C1, g_u8DeviceAddr1 << 1);  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8MasterTxData1[g_u8MasterDataLen1++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen1 != 2)
        {
            I2C_SET_DATA(I2C1, g_au8MasterTxData1[g_u8MasterDataLen1++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);
            g_u8EndFlag1 = 1;
        }
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}




/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr1 << 1)); /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8MasterTxData1[g_u8MasterDataLen1++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen1 != g_u8MasterDataLenSet1)  //2021-01-16 19:17
        {
            I2C_SET_DATA(I2C1, g_au8MasterTxData1[g_u8MasterDataLen1++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_SI);
        }
    }
    else if (u32Status == 0x10)                 /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr1 << 1) | 0x01);  /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x40)                 /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x58)                 /* DATA has been received and NACK has been returned */
    {
        g_u8MasterRxData1 = I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);
        g_u8EndFlag1 = 1;
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted */
    {
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen != g_u8MasterDataLenSet)
        {
            I2C_SET_DATA(I2C0, g_au8MasterTxData[g_u8MasterDataLen++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
            g_u8EndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 TRx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_SlaveTRx(uint32_t u32Status)
{
    if (u32Status == 0x60)                      /* Own SLA+W has been receive; ACK has been return */
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        g_au8SlaveRxData[g_u8SlaveDataLen] = I2C_GET_DATA(I2C1);
        g_u8SlaveDataLen++;

        if (g_u8SlaveDataLen == 2)
        {
            g_u32SlaveBuffAddr = (g_au8SlaveRxData[0] << 8) + g_au8SlaveRxData[1];
        }
        if (g_u8SlaveDataLen == 3)
        {
            g_u8SlvData[g_u32SlaveBuffAddr] = g_au8SlaveRxData[2];
            g_u8SlaveDataLen = 0;
        }
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else if(u32Status == 0xA8)                  /* Own SLA+R has been receive; ACK has been return */
    {

        I2C_SET_DATA(I2C1, g_u8SlvData[g_u32SlaveBuffAddr]);
        g_u32SlaveBuffAddr++;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8SlaveDataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}
void I2C0_Init(void)
{
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C0 clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
}


void I2C1_Init(void)
{
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C1, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
}
#if 0
void I2C1_Init(void)
{
    /* Open I2C1 and set clock to 100k */
    I2C_Open(I2C1, 400000);

    /* Get I2C1 Bus Clock */
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    /* Set I2C1 2 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, SLAVE_ADDRESS, I2C_GCMODE_DISABLE);   /* Set Slave Address */

    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_SI | I2C_AA);
}

#endif
void IIC_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    
    /* Select IP clock source */
    CLK_SetModuleClock(I2C0_MODULE, 0, 0);
    CLK_SetModuleClock(I2C1_MODULE, 0, 0);

    /* Enable IP clock */
    CLK_EnableModuleClock(I2C0_MODULE);
    CLK_EnableModuleClock(I2C1_MODULE);

   /* Set multi function pin for I2C0/I2C1 */
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA13_MFP_I2C0_SCL | SYS_PA_H_MFP_PA12_MFP_I2C0_SDA | SYS_PA_H_MFP_PA11_MFP_I2C1_SCL | SYS_PA_H_MFP_PA10_MFP_I2C1_SDA);

    /* Lock protected registers */
    SYS_LockReg();
    I2C0_Init();
    I2C1_Init();
    
}






/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t Test_iic (void)
{
    uint8_t  i;

    for (i = 0; i < 0x100; i++)
    {
        g_u8SlvData[i] = 0;
    }

    /* I2C function to Slave receive/transmit data */
    s_I2C1HandlerFn = (I2C_FUNC)I2C_SlaveTRx;

    g_u8DeviceAddr = SLAVE_ADDRESS;

    printf("Test Loop =>");
    for (i = 0; i < 0x100; i++)
    {
        printf("%d..", i);
        g_au8MasterTxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8MasterTxData[1] = (uint8_t)(i & 0x00FF);
        g_au8MasterTxData[2] = (uint8_t)(g_au8MasterTxData[1] + 3);

        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;

        /* I2C function to write data to slave */
        s_I2CHandlerFn = (I2C_FUNC)I2C_MasterTx;

        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_STA);

        /* Wait I2C Tx Finish */
        while (g_u8EndFlag == 0);
        g_u8EndFlag = 0;

        
        /* I2C function to read data from slave */
        s_I2CHandlerFn = (I2C_FUNC)I2C_MasterRx;

        g_u8MasterDataLen = 0;
        g_u8DeviceAddr = SLAVE_ADDRESS;

        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C0, I2C_STA);

        /* Wait I2C Rx Finish */
        while (g_u8EndFlag == 0);

        /* Compare Tx and Rx data */
        if (g_u8MasterRxData != g_au8MasterTxData[2])
        {
            printf("I2C Byte Write/Read Failed, Data 0x%x\n", g_u8MasterRxData);
            return -1;
        }
        printf("[OK]\n");
    }

    printf("\nTest Completely !!\n");
    while(1);
}






#if 0          
          for (i = 2; i < 0x09; i++)
           {
              g_au8MasterTxData1[0] = i;
              g_au8MasterTxData1[1] =temp[i-2];
             g_u8MasterDataLenSet1=1;
              g_u8MasterDataLen1 = 0;
              g_u8EndFlag1 = 0;
              g_u8DeviceAddr1 =0x51;
             /* I2C function to write data to slave */
              s_I2C1HandlerFn = (I2C_FUNC)I2C1_MasterTx;

             /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
           }    
     }   
    

 uint8_t  DayArray[13] ={0,31,28,31,30,31,30,31,31,30,31,30,31}; 
void  LoopForPCF8563(uint8_t *tempT)
 {	
      uint8_t temp[7],temp1[7];
        //2015-12-14 12:43   //2016-7-20 13:04
       uint8_t i,j;
        /*   WriteRead8563_b=1;
           RW24XX(TimeTemp,7,0x02,0xa3);
           WriteRead8563_b=0;  */
       
       
       Write_iicData(uint8_t DeviceAd,0x51,0x51,1,uint8_t *iic_dat,7) 
          
     for(j=0;j<10;j++)
       {    
       	
       	    for (i = 2; i < 0x09; i++)
             {
              g_au8MasterTxData1[0] = i;
              g_u8MasterDataLen1 = 0;
              g_u8EndFlag1 = 0;
              g_u8DeviceAddr1 =0x51;
             /* I2C function to write data to slave */
              s_I2C1HandlerFn = (I2C_FUNC)I2C1_MasterRx;

             /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
             temp[i-2]= g_u8MasterRxData1;  
            }
          for (i = 2; i < 0x09; i++)
             {
              g_au8MasterTxData1[0] = i;
              g_u8MasterDataLen1 = 0;
              g_u8EndFlag1 = 0;

             /* I2C function to write data to slave */
              s_I2C1HandlerFn = (I2C_FUNC)I2C1_MasterRx;

             /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
             temp1[i-2]= g_u8MasterRxData1;  
            }  
           for(i=1;i<7;i++)
             { if(temp[i]!=temp1[i])
                 break;
             }
           if(i==7) 
              break;
        } 
       if(j==10)
          {
            temp[0]=0;  //2015-12-14 13:39
            temp[1]=1;
            temp[2]=1;
            temp[3]=1;
            temp[4]=0;
            temp[5]=0;
          }    
       else
         {    
            
            
          temp[0]&=0x7f;
          temp[1]&=0x7f;
          temp[2]&=0x3f;
          temp[3]&=0x3f;
          
          temp[5]&=0x1f;  
          temp[4]=temp[1]/16*10+temp[1]%16;
          temp[1]=temp[5]/16*10+temp[5]%16;
          temp[5]=temp[0]/16*10+temp[0]%16;
          temp[0]=temp[6]/16*10+temp[6]%16;
          temp[6]=temp[3]/16*10+temp[3]%16;
          temp[3]=temp[2]/16*10+temp[2]%16;
          temp[2]=temp[6];
          
          j=0;
           if((temp[1]==0)||(temp[1]>12) )  
                         j=1;
         
                 if(j==0) 
                   {   
                           if((temp[2]==0)||(temp[2]>DayArray[temp[1]]) )  
                                   j=1;
                                 if((temp[0]%4==0)&& (temp[1]==2))
                                  { if(temp[2]>29)
                                     j=1;
                                    else
                                      j=0; 
                                  }   
                   }              
                 if(j==0)
                  {              
                    if(temp[3]>23 )  
                     j=1; 
                   }
                  if(j==0)
                   {    
                     if(temp[4]>=60 )  
                      j=1;   
                   }    
                 if(j==0)
                   {            
                       if(temp[5]>=60 )  
                       j=1;
                   }
                 if(j==1)
                  { temp[0]=0;
                    temp[1]=1;
                    temp[2]=1;
                    temp[3]=0;
                    temp[4]=0;
                    temp[5]=0;
                  }
          	 }
         #if 0 
          if(j==0)   
             TimeSuccessFlag=1;   //2015-10-23 10:37
             
          #endif  
         
          //add 2019-04-07 11:59
          i=0;
          if((temp[0]<19)||(temp[0]>49))
            { i=1;
           }
        if((temp[1]<1)||(temp[1]>12))
            { i=1;
           }
         if((temp[2]<1)||(temp[2]>31))
            { i=1;
           } 
          if(temp[3]>23)
            { i=1;
           }  
          if((temp[4]>59)||(temp[5]>59))
            { i=1;
           }  
         #if 0   
         if(i==1)
          TimeStsatus=1;
        else 
          TimeStsatus=0; 
          
          VarList[0]=0;
          VarList[1]=temp[0];
          VarList[2]=0;
          VarList[3]=temp[1];
          VarList[4]=0;
          VarList[5]=temp[2];
          VarList[6]=0;
          VarList[7]=temp[3];
          VarList[8]=0;
          VarList[9]=temp[4];
          VarList[10]=0;
          VarList[11]=temp[5]; 
        #endif  
      for(i=0;i<7;i++)  
        tempT[i]=temp[i];
          
 }	
 
#endif 



void  Write_iicData(uint8_t IIC_Num,uint8_t DeviceAd,uint8_t iic_Addr1,uint8_t iic_Addr2,uint8_t Addr_Len,uint8_t *iic_dat,uint8_t Dat_Len) 
    { 
    	uint8_t  i,j=0;
    if(IIC_Num==0)
       {  
      for(i=0;i<Dat_Len;i++)
      { j=0;
    	g_u8MasterDataLenSet=Addr_Len;
        if(Addr_Len==1)
          { g_au8MasterTxData[0] = iic_Addr1+i;
            j=2;
          }  
        else
          {
             g_au8MasterTxData[1] = iic_Addr2+i;
             if(g_au8MasterTxData[1]==0)
               g_au8MasterTxData[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData[0] = iic_Addr1 ; 
             j=3;  
          }     
        g_au8MasterTxData[j++]=iic_dat[i];
        g_u8MasterDataLenSet=j-1;
        g_u8DeviceAddr =DeviceAd;
        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;
        s_I2CHandlerFn = (I2C_FUNC)I2C_MasterTx;
      /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C0, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag == 0);
               g_u8EndFlag = 0;
       }
    } 
  else
    {  for(i=0;i<Dat_Len;i++)
      { j=0;
    	g_u8MasterDataLenSet1=Addr_Len;
        if(Addr_Len==1)
          { g_au8MasterTxData1[0] = iic_Addr1+i;
            j=2;
          }  
        else
          {
             g_au8MasterTxData1[1] = iic_Addr2+i;
             if(g_au8MasterTxData1[1]==0)
               g_au8MasterTxData1[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData1[0] = iic_Addr1 ; 
             j=3;  
          }     
        g_au8MasterTxData1[j++]=iic_dat[i];
        g_u8MasterDataLenSet1=j-1;
        g_u8DeviceAddr1 =DeviceAd;
        g_u8MasterDataLen1 = 0;
        g_u8EndFlag1 = 0;
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;
      /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
       }
    	
    	
    }	
 }   	
    	   
    
    
    
    
void  Read_iicData(uint8_t IIC_Num,uint8_t DeviceAd,uint8_t iic_Addr1,uint8_t iic_Addr2,uint8_t Addr_Len,uint8_t *iic_dat,uint8_t Dat_Len)       
   { 
      uint8_t  i,j=0;
      
   if(IIC_Num==0)
     {   
      for(i=0;i<Dat_Len;i++)
      { j=0;
    	g_u8MasterDataLenSet=Addr_Len;
        if(Addr_Len==1)
          { g_au8MasterTxData[0] = iic_Addr1+i;
            j=2;
          }  
        else
          {
             g_au8MasterTxData[1] = iic_Addr2+i;
             if(g_au8MasterTxData[1]==0)
               g_au8MasterTxData[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData[0] = iic_Addr1 ; 
             j=3;  
          }     
        g_u8MasterDataLenSet=j-1;
        g_u8DeviceAddr =DeviceAd;
        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;
        s_I2CHandlerFn = (I2C_FUNC)I2C_MasterRx;
      /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C0, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag == 0);
               g_u8EndFlag = 0;
          iic_dat[i]= g_u8MasterRxData; 
   } 
 }   
else
 {
 for(i=0;i<Dat_Len;i++)
      { j=0;
    	g_u8MasterDataLenSet1=Addr_Len;
        if(Addr_Len==1)
          { g_au8MasterTxData1[0] = iic_Addr1+i;
            j=2;
          }  
        else
          {
             g_au8MasterTxData1[1] = iic_Addr2+i;
             if(g_au8MasterTxData1[1]==0)
               g_au8MasterTxData1[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData1[0] = iic_Addr1 ; 
             j=3;  
          }     
        g_u8MasterDataLenSet1=j-1;
        g_u8DeviceAddr1 =DeviceAd;
        g_u8MasterDataLen1 = 0;
        g_u8EndFlag1 = 0;
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterRx;
      /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
          iic_dat[i]= g_u8MasterRxData1; 	
 }
}	
} 	


void  SetPCF8563(uint8_t *temp)
   {
          
           Write_iicData(1,0x51,0x02,0x00,1,temp,7) ;
    }       

 uint8_t  DayArray[13] ={0,31,28,31,30,31,30,31,31,30,31,30,31}; 
void  LoopForPCF8563(uint8_t *tempT)
 {	
      uint8_t temp[7],temp1[7];
        //2015-12-14 12:43   //2016-7-20 13:04
       uint8_t i,j;
        /*   WriteRead8563_b=1;
           RW24XX(TimeTemp,7,0x02,0xa3);
           WriteRead8563_b=0;  */
       
       
       
          
     for(j=0;j<10;j++)
       {    
           Write_iicData(1,0x51,0x02,0x00,1,temp,7); 
           Write_iicData(1,0x51,0x02,0x00,1,temp1,7);  	
       	   for(i=1;i<7;i++)
             { if(temp[i]!=temp1[i])
                 break;
             }
           if(i==7) 
              break;
        } 
       if(j==10)
          {
            temp[0]=0;  //2015-12-14 13:39
            temp[1]=1;
            temp[2]=1;
            temp[3]=1;
            temp[4]=0;
            temp[5]=0;
          }    
       else
         {    
            
            
          temp[0]&=0x7f;
          temp[1]&=0x7f;
          temp[2]&=0x3f;
          temp[3]&=0x3f;
          
          temp[5]&=0x1f;  
          temp[4]=temp[1]/16*10+temp[1]%16;
          temp[1]=temp[5]/16*10+temp[5]%16;
          temp[5]=temp[0]/16*10+temp[0]%16;
          temp[0]=temp[6]/16*10+temp[6]%16;
          temp[6]=temp[3]/16*10+temp[3]%16;
          temp[3]=temp[2]/16*10+temp[2]%16;
          temp[2]=temp[6];
          
          j=0;
           if((temp[1]==0)||(temp[1]>12) )  
                         j=1;
         
                 if(j==0) 
                   {   
                           if((temp[2]==0)||(temp[2]>DayArray[temp[1]]) )  
                                   j=1;
                                 if((temp[0]%4==0)&& (temp[1]==2))
                                  { if(temp[2]>29)
                                     j=1;
                                    else
                                      j=0; 
                                  }   
                   }              
                 if(j==0)
                  {              
                    if(temp[3]>23 )  
                     j=1; 
                   }
                  if(j==0)
                   {    
                     if(temp[4]>=60 )  
                      j=1;   
                   }    
                 if(j==0)
                   {            
                       if(temp[5]>=60 )  
                       j=1;
                   }
                 if(j==1)
                  { temp[0]=0;
                    temp[1]=1;
                    temp[2]=1;
                    temp[3]=0;
                    temp[4]=0;
                    temp[5]=0;
                  }
          	 }
         #if 0 
          if(j==0)   
             TimeSuccessFlag=1;   //2015-10-23 10:37
             
          #endif  
         
          //add 2019-04-07 11:59
          i=0;
          if((temp[0]<19)||(temp[0]>49))
            { i=1;
           }
        if((temp[1]<1)||(temp[1]>12))
            { i=1;
           }
         if((temp[2]<1)||(temp[2]>31))
            { i=1;
           } 
          if(temp[3]>23)
            { i=1;
           }  
          if((temp[4]>59)||(temp[5]>59))
            { i=1;
           }  
         #if 0   
         if(i==1)
          TimeStsatus=1;
        else 
          TimeStsatus=0; 
          
          VarList[0]=0;
          VarList[1]=temp[0];
          VarList[2]=0;
          VarList[3]=temp[1];
          VarList[4]=0;
          VarList[5]=temp[2];
          VarList[6]=0;
          VarList[7]=temp[3];
          VarList[8]=0;
          VarList[9]=temp[4];
          VarList[10]=0;
          VarList[11]=temp[5]; 
        #endif  
      for(i=0;i<7;i++)  
        tempT[i]=temp[i];
     DisplayTime2(tempT)  ;   
 }	












 