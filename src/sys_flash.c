#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

uint8_t SIM_EEPROM_READ(uint32_t address);
void SIM_EEPROM_WRITE(uint32_t address, uint8_t data);
int32_t SetDataFlashBase(uint32_t u32DFBA);
int16_t Flash_eeprom_Initial();
void Restart();
void	ProgramFlash(uint16_t FlashAddr1,unsigned char *ptr,uint16_t ProgramNum);
void ReadEepromByte();
void	ProgramEepromByte();
void ReadPagePara();

void InitialPagePara();




#define DATA_FLASH_EEPROM_BASE      0xF000  /* Configure Data Flash base address for EEPROM */
#define DATA_FLASH_EEPROM_END       0xffff /* Configure Data Flash end address for EEPROM */
#define DATA_FLASH_EEPROM_SIZE      (DATA_FLASH_EEPROM_END - DATA_FLASH_EEPROM_BASE)    /* Data Flash size for EEPROM */
#define EEPROM_BASE                 (DATA_FLASH_EEPROM_BASE)  /* Set EEPROM base address */
#define EEPROM_SIZE                 (DATA_FLASH_EEPROM_SIZE)  /* Set EEPROM size */

//---------------------------------------------------------------
//Function:  SIM_EEPROM_READ(uint32_t address)
//Input:    uint32_t address
//Output:   None
//Description: 读一个指定地址的数据
//----------------------------------------------------------------
uint8_t SIM_EEPROM_READ(uint32_t address)
{
    uint32_t u32Data;

    u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
    return ((u32Data >> ((address % 4) * 8)) & 0xFF);
}

/*---------------------------------------------------------------
//Function:  SIM_EEPROM_WRITE(uint32_t address, uint8_t data)
//Input:   uint32_t address:地址
           uint8_t data:数据
//Output:   None
//Description: 写指定指定一个字节指定数据
//----------------------------------------------------------------*/
void SIM_EEPROM_WRITE(uint32_t address, uint8_t data)
{
    uint32_t i, u32Data;
    uint32_t u32Start, u32End, au32DataBuf[FMC_FLASH_PAGE_SIZE / 4], *pu32DataBuf;

    if ((SIM_EEPROM_READ(address) == 0xFF) || (SIM_EEPROM_READ(address) == data))
    {
        /* Original flash data is 0xFF, or data is matched. Do not perform flash page erase. */

        /* Read oroginal data */
        u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
        /* Assign 32-bit updated data */
        u32Data = ~(0xFF << ((address % 4) * 8));
        u32Data |= (data << ((address % 4) * 8));
        /* Write updated data */
        FMC_Write((((address / 4) * 4) + EEPROM_BASE), u32Data);
    }
    else
    {
        /* Need to update one page flash data. Perform flash page erase is mandatory before writing updated data. */

        /* Assign 32-bit updated data */
        u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
        u32Data &= ~(0xFF << ((address % 4) * 8));
        u32Data |= (data << ((address % 4) * 8));

        /* Dump target page data to SRAM buffer */
        u32Start = (EEPROM_BASE + ((address / FMC_FLASH_PAGE_SIZE) * FMC_FLASH_PAGE_SIZE)); // page-size alignment
        u32End   = (u32Start + FMC_FLASH_PAGE_SIZE);
        pu32DataBuf = (uint32_t *)au32DataBuf;

        for (i = u32Start; i < u32End; i += 4)
        {
            *pu32DataBuf++ = FMC_Read(i);
        }

        /* Erase target page */
        FMC_Erase(EEPROM_BASE + ((address / FMC_FLASH_PAGE_SIZE)*FMC_FLASH_PAGE_SIZE));

        /* Set updated data to SRAM buffer */
        au32DataBuf[((address % FMC_FLASH_PAGE_SIZE) / 4)] = u32Data;

        /* Write target flash data from SRAM buffer */
        pu32DataBuf = (uint32_t *)au32DataBuf;

        for (i = u32Start; i < u32End; i += 4)
        {
            FMC_Write(i, *pu32DataBuf++);
        }
    }
}

//---------------------------------------------------------------
//Function:  SetDataFlashBase(uint32_t u32DFBA)
//Input:    None
//Output:   None
//Description: 设置sim eeprom的地址
//----------------------------------------------------------------
int32_t SetDataFlashBase(uint32_t u32DFBA)
{
    uint32_t   au32Config[2];

    /* Read current User Configuration */
    if (FMC_ReadConfig(au32Config, 2) < 0)
    {
        //printf("\nRead User Config failed!\n");
        return -1;
    }

    /* Just return when Data Flash has been enabled */
    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
        return 0;

    /* Enable User Configuration Update */
    FMC_ENABLE_CFG_UPDATE();

    /* Write User Configuration to Enable Data Flash */
    au32Config[0] &= ~0x1;
    au32Config[1] = u32DFBA;

    if (FMC_WriteConfig(au32Config, 2) < 0)
        return -1;

    // Perform chip reset to make new User Config take effect
//2021-01-25 13:26    SYS->IPRSTC1 = SYS_IPRSTC1_CHIP_RST_Msk;
    return 0;

}

//---------------------------------------------------------------
//Function:  Flash_eeprom_Initial()
//Input:    None
//Output:   None
//Description: sim eeprom初始化
//----------------------------------------------------------------
int16_t Flash_eeprom_Initial()
{ 
    SYS_UnlockReg();
    FMC_Open();

    /* Set Data Flash base address */
    if (SetDataFlashBase(DATA_FLASH_EEPROM_BASE) < 0)
    {
        return(-1);
    }
  return(0);

}

void Restart()
 {
 
 
}	
/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/

/*---------------------------------------------------------------
//Function: ProgramFlash(uint16_t FlashAddr1,unsigned char *ptr,unsigned char ProgramNum)
//Input:    uint16_t FlashAddr1,
            unsigned char *ptr,
            unsigned char ProgramNum
//Output:   None
//Description: 指定地址写指定长度的数据
//----------------------------------------------------------------*/
void	ProgramFlash(uint16_t FlashAddr1,unsigned char *ptr,uint16_t ProgramNum)
{ uint16_t i;
	if(Flash_eeprom_Initial()==0)
	{  for(i=0;i<ProgramNum;i++)
	   SIM_EEPROM_WRITE(FlashAddr1+i, *ptr++);
	}
}

//---------------------------------------------------------------
//Function: ReadEepromByte()
//Input:    None
//Output:   None
//Description: 读参数,并确定是否需要初始化
//----------------------------------------------------------------
void ReadEepromByte()
{ uint16_t 	i=0,j=0;
            
       if(Flash_eeprom_Initial()==0)
			 {
             for(i=0;i<ParaNum;i++)
               { 
             	 ParaList[i]=SIM_EEPROM_READ(i);
                 WatchdogReset(); 
               }
             if(GetInitialFlag()==1)
               {    	
              BL03D_Base_Update();
              BL03D_Update();
              BL02D_Special_Update();
	      
               }
		          else
               {  ResetFlag=1;
                  InitialPara();
               }
        
 }    
}

//---------------------------------------------------------------
//Function:  void	ProgramEepromByte()
//Input:    None
//Output:   None
//Description: 将ParaList[i]的参数写入到eeprom中
//----------------------------------------------------------------
void	ProgramEepromByte()
{
  uint16_t 	i=0;	
       
						//保存中断状态
	if(Flash_eeprom_Initial()==0)
	{
	for(i=0;i<ParaNum;i++)
	 {  
//	if((i!=(cTestMode03D-cParaActual)*2)&&(i!=(cTestMode03D-cParaActual)*2+1))
	    
	     SIM_EEPROM_WRITE(i,ParaList[i]);    
 }          
              BL03D_Base_Update();
              BL02D_Special_Update();
              #if  0
              VarList[4]=ParaList[16];
              VarList[5]=ParaList[17];  //baojing 
              VarList[6]=ParaList[32];
              VarList[7]=ParaList[33];  //远程控制
              #endif
   }  
 }
                          