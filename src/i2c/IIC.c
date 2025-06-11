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
#define _GLOBAL_H
#include	"global.h"

void  UpdateTimeVar(uint8_t *temp) ;
typedef void (*I2C_FUNC)(uint32_t u32Status);
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
volatile I2C_FUNC s_I2CHandlerFn = NULL;


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
volatile I2C_FUNC s_I2C1HandlerFn = NULL;


unsigned char   TimeStsatus;

void I2C0_IRQHandler(void);
void I2C_MasterRx(uint32_t u32Status);
void I2C_MasterTx(uint32_t u32Status);
void IIC_Init(void);
int32_t Test_iic (void);

void I2C1_IRQHandler(void);
void I2C1_MasterTx(uint32_t u32Status);
void I2C1_MasterRx(uint32_t u32Status);
void I2C1_Init(void);
void  Write_iicData(uint8_t IIC_Num,uint8_t DeviceAd,uint8_t iic_Addr1,uint8_t iic_Addr2,uint8_t Addr_Len,uint8_t *iic_dat,uint8_t Dat_Len); 
void  Read_iicData(uint8_t IIC_Num,uint8_t DeviceAd,uint8_t iic_Addr1,uint8_t iic_Addr2,uint8_t Addr_Len,uint8_t *iic_dat,uint8_t Dat_Len);       
void  SetPCF8563(uint8_t *temp);
void  LoopForPCF8563(uint8_t *tempT);
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

void I2C0_Init(void)
{    SYS_UnlockReg(); 
	/* Select IP clock source */
    CLK_SetModuleClock(I2C0_MODULE, 0, 0);
        /* Enable IP clock */
    CLK_EnableModuleClock(I2C0_MODULE);
     /* Set multi function pin for I2C0/I2C1 */
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA13_MFP_I2C0_SCL | SYS_PA_H_MFP_PA12_MFP_I2C0_SDA );
    /* Open I2C0 and set clock to 100k */
     /* Lock protected registers */
    SYS_LockReg();
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C0 clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
}

void InitialIoIIC()
 {
 GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
 GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
}

#define  __useIoIIC
void IIC_Init(void)
{
  #ifdef __useIoIIC
      InitialIoIIC();
  #else
    I2C0_Init();
  #endif  
    I2C1_Init();
    
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
        if (g_u8MasterDataLen1 != g_u8MasterDataLenSet1)
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



void I2C1_Init(void)
{   SYS_UnlockReg();
     CLK_SetModuleClock(I2C1_MODULE, 0, 0);
    CLK_EnableModuleClock(I2C1_MODULE);
  /* Set multi function pin for I2C0/I2C1 */
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA11_MFP_I2C1_SCL | SYS_PA_H_MFP_PA10_MFP_I2C1_SDA);

    /* Lock protected registers */
    SYS_LockReg();
    /* Open I2C0 and set clock to 100k */
    I2C_Open(I2C1, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
}


void  Write_iicData(uint8_t IIC_Num,uint8_t DeviceAd,uint8_t iic_Addr1,uint8_t iic_Addr2,uint8_t Addr_Len,uint8_t *iic_dat,uint8_t Dat_Len) 
    { 
    	uint8_t  i,j=0;
    if(IIC_Num==0)
       {  
      for(i=0;i<Dat_Len;i++)
      { j=0;
    	
        if(Addr_Len==1) //1byte地址长度
          { g_au8MasterTxData[0] = iic_Addr1+i;
            j=1;
          }  
        else   //2byte地址长度
          {
             g_au8MasterTxData[1] = iic_Addr2+i;
             if(iic_Addr2+i>=256)
               g_au8MasterTxData[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData[0] = iic_Addr1 ; 
             j=2;  
          }     
        g_au8MasterTxData[j++]=iic_dat[i];
        g_u8MasterDataLenSet=j; //这个长度为地址长度+数据长度  1+1 或者2+1
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
    	 if(Addr_Len==1) //1byte地址长度
          { g_au8MasterTxData1[0] = iic_Addr1+i;
            j=1;
          }  
        else  //1byte地址长度
          {
             g_au8MasterTxData1[1] = iic_Addr2+i;
             if(iic_Addr2+i>=256)
               g_au8MasterTxData1[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData1[0] = iic_Addr1 ; 
             j=2;  
          }     
        g_au8MasterTxData1[j++]=iic_dat[i];
        g_u8MasterDataLenSet1=j;  //这个长度为地址长度+数据长度  1+1 或者2+1
        g_u8DeviceAddr1 =DeviceAd;
        g_u8MasterDataLen1 = 0;
        g_u8EndFlag1 = 0;
        s_I2C1HandlerFn = (I2C_FUNC)I2C1_MasterTx;
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
    	
        if(Addr_Len==1)  //1byte地址长度
          { g_au8MasterTxData[0] = iic_Addr1+i;
            j=1;
          }  
        else    //2byte 地址长度
          {
             g_au8MasterTxData[1] = iic_Addr2+i;
            if(iic_Addr2+i>=256)
               g_au8MasterTxData[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData[0] = iic_Addr1 ; 
             j=2;  
          } 
        g_au8MasterTxData[j++] =iic_dat[i];  
        g_u8MasterDataLenSet=j+1; //地址长度加上一个数据长度
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
    	
        if(Addr_Len==1)
          { g_au8MasterTxData1[0] = iic_Addr1+i;
            j=1;
          }  
        else
          {
             g_au8MasterTxData1[1] = iic_Addr2+i;
            if(iic_Addr2+i>=256)
               g_au8MasterTxData1[0] = iic_Addr1+1;  
             else
               g_au8MasterTxData1[0] = iic_Addr1 ; 
             j=2;  
          }     
        g_u8MasterDataLenSet1=j;  //读地址的长度
        g_u8DeviceAddr1 =DeviceAd;
        g_u8MasterDataLen1 = 0;
        g_u8EndFlag1 = 0;
        s_I2C1HandlerFn = (I2C_FUNC)I2C1_MasterRx;
      /* I2C as master sends START signal */
             I2C_SET_CONTROL_REG(I2C1, I2C_STA);
             /* Wait I2C Tx Finish */
             while (g_u8EndFlag1 == 0);
               g_u8EndFlag1 = 0;
          iic_dat[i]= g_u8MasterRxData1; 	
 }
}	
}
//---------------------------------------------------------------
//Function:  GetPcf8563Status()
//Input:    
//Output:   TimeSuccessFlag=1;正常   =0:故障
//Description: 返回时钟状态
//---------------------------------------------------------------- 	
uint8_t  GetPcf8563Status()
   {   
   	return(TimeStsatus);
  }
//---------------------------------------------------------------
//Function:  SetPCF8563(uint8_t *temp)
//Input:    *temp:时间参数  sec,min,hour,day,week,mon,year  十进制
//Output:   None
//Description: 设置时间到pcf8563中
//----------------------------------------------------------------
void  SetPCF8563DEC(uint8_t *temp)
   {  
#if  0		 
		 uint8_t SetT[7],j;
   	           SetT[0]=temp[5]/10*16+temp[5]%10;
  	           SetT[1]=temp[4]/10*16+temp[4]%10;
  	           SetT[2]=temp[3]/10*16+temp[3]%10;
  	           SetT[3]=temp[2]/10*16+temp[2]%10;
  	           SetT[5]=temp[2]/10*16+temp[1]%10;
  	           SetT[6]=temp[0]/10*16+temp[0]%10;  
  	      	   SetPCF8563(SetT);
  	      	   for(j=0;j<7;j++)
  	      	      SetT[j]=0xff;
  	      	   LoopForPCF8563(SetT);
  	      	   DisplayTime_Byte(SetT);  
 #endif
		 
    }      

//---------------------------------------------------------------
//Function:  SetPCF8563(uint8_t *temp)
//Input:    *temp:时间参数  sec,min,hour,day,week,mon,year BCD码
//Output:   None
//Description: 设置时间到pcf8563中
//----------------------------------------------------------------
void  SetPCF8563(uint8_t *temp)
   {uint8_t SetT[7],j;
          
           Write_iicData(1,0x51,0x02,0x00,1,temp,7) ;
              for(j=0;j<7;j++)
  	      	      SetT[j]=0xff;
  	      	   LoopForPCF8563(SetT);
  	      	   DisplayTime_Byte(SetT);
    }       




/*---------------------------------------------------------------
//Function:  void  LoopForPCF8563(uint8_t *tempT)
//Input:    *uint8_t *tempT:读取到的时间参数  sec,min,hour,day,week,mon,year
//Output:   None
//Description: 读取pcf8563中的时钟,从时钟中读出的数据是BCD码,转换为10进制输出
                读出数据格式:sec,min,hour,day,week,mon,year(BCD)
                输出格式:    year,mon,day,hour,min,sec
//---------------------------------------------------------------*/
 uint8_t  DayArray[13] ={0,31,28,31,30,31,30,31,31,30,31,30,31}; 

void  LoopForPCF8563(uint8_t *tempT)
 {	
   #ifdef _BL03D 
    
       uint8_t temp[7],temp1[7];
       uint8_t i,j;
       for(j=0;j<10;j++)
       {   for(i=0;i<7;i++) 
            { temp[i]=i+0x10;
              temp1[i]=i+0x20;
            }
           Read_iicData(1,0x51,0x02,0x00,1,temp,7); 
           Read_iicData(1,0x51,0x02,0x00,1,temp1,7);  	
       	   for(i=1;i<7;i++)
             { if(temp[i]!=temp1[i])
                 break;
             }
           if(i==7) 
              break;
        } 
       if(j==10)
          {
            temp[0]=1;  //2015-12-14 13:39
            temp[1]=1;
            temp[2]=1;
            temp[3]=1;
            temp[4]=1;
            temp[5]=1;
          }    
       else
         {    
            
            
          temp[0]&=0x7f;  //sec
          temp[1]&=0x7f;  //min
          temp[2]&=0x3f;  //hour
          temp[3]&=0x3f;  //day
           
          temp[5]&=0x1f;   //mon
          temp[4]=temp[1]/16*10+temp[1]%16;  //min   temp[0....5]=[year----sec] bcd码格式
          temp[1]=temp[5]/16*10+temp[5]%16;  //mon
          temp[5]=temp[0]/16*10+temp[0]%16;   //sec
          temp[0]=temp[6]/16*10+temp[6]%16;  //year
          temp[6]=temp[3]/16*10+temp[3]%16;   //
          temp[3]=temp[2]/16*10+temp[2]%16;   //hour
          temp[2]=temp[6];                   //day
          
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
                  { temp[0]=2;
                    temp[1]=2;
                    temp[2]=2;
                    temp[3]=2;
                    temp[4]=2;
                    temp[5]=2;
                  }
          	 }
          
        
         
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
         #if 1   
         if(i==1)
          TimeStsatus=0;
        else 
          TimeStsatus=1; 
         UpdateTimeVar(temp) ;  
         
        #endif  
      for(i=0;i<7;i++)  
        tempT[i]=temp[i];
 #endif
 }	
//---------------------------------------------------------------
//Function:  void  UpdateTimeVar(uint8_t *temp) 
//Input:    uint8_t *temp:10进制时间 6bytes
//Output:   None
//Description:将读取到时间放入指定的变量单元
//----------------------------------------------------------------
void  UpdateTimeVar(uint8_t *temp) 
    {
          VarList[0+4]=0;
          VarList[1+4]=temp[0];
          VarList[2+4]=0;
          VarList[3+4]=temp[1];
          VarList[4+4]=0;
          VarList[5+4]=temp[2];
          VarList[6+4]=0;
          VarList[7+4]=temp[3];
          VarList[8+4]=0;
          VarList[9+4]=temp[4];
          VarList[10+4]=0;
          VarList[11+4]=temp[5]; 
    }                           
         
void   CopyTimeVar(uint8_t *temp) 
   {
   	temp[0]=VarList[1+4];
   	temp[1]=VarList[3+4];
   	temp[2]=VarList[5+4];
   	temp[3]=VarList[7+4];
   	temp[4]=VarList[9+4];
   	temp[5]=VarList[11+4];
   	
   	       
    }     
          
#if  0

#define POLYNOMIAL  0x131 //P(x)=x^8+x^5+x^4+1 = 100110001

#define   TRIG_T_MEASUREMENT_HM    0xE3 // command trig. temp meas. hold master
#define   TRIG_RH_MEASUREMENT_HM   0xE5 // command trig. humidity meas. hold master
#define   TRIG_T_MEASUREMENT_POLL  0xF3 // command trig. temp meas. no hold master
#define   TRIG_RH_MEASUREMENT_POLL 0xF5 // command trig. humidity meas. no hold master
#define   USER_REG_W               0xE6 // command writing user register
#define   USER_REG_R               0xE7 // command reading user register
#define   SOFT_RESET               0xFE  // command soft reset



#define   SHT2x_RES_12_14BIT       0x00 // RH=12bit, T=14bit
#define   SHT2x_RES_8_12BIT        0x01 // RH= 8bit, T=12bit
#define   SHT2x_RES_10_13BIT       0x80 // RH=10bit, T=13bit
#define   SHT2x_RES_11_11BIT       0x81 // RH=11bit, T=11bit
#define   SHT2x_RES_MASK           0x85  // Mask for res. bits (7,0) in user reg.



#define   SHT2x_EOB_ON             0x40 // end of battery
#define   SHT2x_EOB_MASK           0x40 // Mask for EOB bit(6) in user reg.


#define   SHT2x_HEATER_ON          0x04 // heater on
#define   SHT2x_HEATER_OFF         0x00 // heater off
#define   SHT2x_HEATER_MASK        0x04 // Mask for Heater bit(2) in user reg.


// measurement signal selection
#define HUMIDITYsta    0
#define  TEMPsta       1



#define  I2C_ADR_W                 128   // sensor I2C address + write bit
#define  I2C_ADR_R                129    // sensor I2C address + read bit

#if 0
//==============================================================================
float SHT2x_CalcRH(uint16_t u16sRH)
//==============================================================================
{
  float humidityRH;              // variable for result
  u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --

  humidityRH = -6.0 + 125.0/65536 *( (float)u16sRH); // RH= -6 + 125 * SRH/2^16
  return humidityRH;
}

//==============================================================================
float SHT2x_CalcTemperatureC(uint16_t u16sT)
//==============================================================================
{
  float temperatureC;            // variable for result

  u16sT &= ~0x0003;           // clear bits [1..0] (status bits)

  //-- calculate temperature [癈] --
  temperatureC= -46.85 + 175.72/65536 *((float)u16sT); //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}
#endif


void  LoopForSth21_iic(void)
  {	
       uint8_t temp[7];
     
     temp[0]=0XFF;
     Write_iicData(0,I2C_ADR_W/2,SOFT_RESET,0x00,1,temp,0) ;
    
     temp[0] = (userRegister & (~SHT2x_RES_MASK)) |SHT2x_RES_12_14BIT;//SHT2x_RES_8_12BIT;//SHT2x_RES_10_13BIT;    //
     Write_iicData(1,I2C_ADR_W/2,USER_REG_R,0x00,1,temp,1);
     Read_iicData(0,I2C_ADR_W/2,TRIG_RH_MEASUREMENT_HM,0x00,1,temp,3);  //读取3个byte,2个数据+1checksum	
     SHT2x_CalcRH(temp[0]*256+temp[1]);
     Read_iicData(0,I2C_ADR_W/2,TRIG_T_MEASUREMENT_HM,0x00,1,temp,3);
     SHT2x_CalcTemperatureC(temp[0]*256+temp[1]);
     Read_iicData(0,I2C_ADR_W/2,USER_REG_R,0x00,1,temp,1) ;
   } 
 #endif
    
    


 