#ifndef W25QXX_H
#define W25QXX_H
#if 0
#include "cpu.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#endif
#include "Nano100Series.h"
//#include "os_cfg_app.h"
//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17



#define TEST_NUMBER 100   /* page numbers */
#define TEST_LENGTH 256 /* length */

#define SPI_FLASH_PORT  SPI1
#define SPI_WRITE_TX    SPI_WRITE_TX0
#define SPI_READ_RX     SPI_READ_RX0




extern uint16_t W25QXX_TYPE;					//定义W25QXX芯片型号
#define __W25QXX_SPI_PTR__  SPI1
#define _W25QXX_SPI_PTR_    SPI1
#define W25QXX_SPI_PTR	    SPI1
#if  0
#define W25QXX_SPI_ReceiveBytes(pData,n) \
			{W25QXX_SPI_Receive(W25QXX_SPI_PTR, pData, n, CONVERT_MS_TO_OSTICK(2000));}
			
#define W25QXX_SPI_Receive( spi_ptr, pData, Size, Timeout_os_tick) \
			{HAL_SPI_Receive(spi_ptr, pData, Size, Timeout_os_tick);}
			
#define W25QXX_SPI_TransmitBytes(pData,n) \
			{W25QXX_SPI_Transmit(W25QXX_SPI_PTR, pData, n, CONVERT_MS_TO_OSTICK(2000));}
			
#define W25QXX_SPI_TransmitReceive( spi_ptr, pTxData, pRxData, Size, Timeout_os_tick) \
			{HAL_SPI_TransmitReceive(spi_ptr, pTxData, pRxData, Size, Timeout_os_tick);}		
			
#define W25QXX_SPI_TransmitReceiveBytes(pData,n) \
			{W25QXX_SPI_TransmitReceive(W25QXX_SPI_PTR, pData, pData, n, CONVERT_MS_TO_OSTICK(2000));}


			
//HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)			
			
			
#define W25QXX_SPI_Transmit( spi_ptr, pData, Size, Timeout_os_tick) \ SPI_WRITE_TX0(SPI0, u32TxData)		
#endif


			
			#define W25QXX_CS_SEL() SPI_SET_SS0_LOW(SPI1)
#define W25QXX_CS_DESEL() SPI_SET_SS0_HIGH(SPI1)
////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

uint64_t W25QXX_GET_USABLE_DENSITY(uint16_t TYPE);
uint16_t W25QXX_Init(void);
uint8_t W25QXX_Write_Check(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite,uint8_t NEED_CHECK);
uint8_t W25QXX_Write(const uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)  ;//写入flash
void W25QXX_Write_SR(uint8_t sr);  			//写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(uint32_t Dst_Addr);	//扇区擦除
uint8_t W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒
uint16_t  W25QXX_ReadID(void);  	    		//读取FLASH ID
uint8_t	W25QXX_ReadSR(void);        		//读取状态寄存器 

//这里定义的是硬件层的扇区和块的大小，是对应芯片手册的
#define W25QXX_HW_BYTE_PER_SECTOR 	4096 //每个扇区4KB
#define W25QXX_HW_ERASE_SIZE_ONCE 	W25QXX_HW_BYTE_PER_SECTOR //FLASH最小的可擦除单位:单位：字节
#define W25QXX_HW_SECTOR_PER_BLOCK  16  //每个BLOCK有16个扇区
#define W25QXX_HW_PAGE_SIZE 256 //W25QXX 一次性最多写256字节



#define W25Q128FV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q128FV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q128FV_DUMMY_CYCLES_READ           4
#define W25Q128FV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q128FV_BULK_ERASE_MAX_TIME         250000
#define W25Q128FV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME    800
#define W25Qx_TIMEOUT_VALUE 1000

/** 
  * @brief  W25Q128FV Commands  
  */  
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_DATA_CMD                        0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */


//#define W25Qx_Enable() 			HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
//#define W25Qx_Disable() 		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)

#define W25Qx_OK            ((uint8_t)0x00)
#define W25Qx_ERROR         ((uint8_t)0x01)
#define W25Qx_BUSY          ((uint8_t)0x02)
#define W25Qx_TIMEOUT		((uint8_t)0x03)


#endif
