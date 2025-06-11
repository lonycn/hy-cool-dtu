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



typedef void (*I2C_FUNC)(uint32_t u32Status);
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
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






 