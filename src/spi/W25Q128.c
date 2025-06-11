#include "w25qxx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"


#define SIZE_1M (1024*1024)
uint16_t W25QXX_TYPE = W25Q128;	//默认是W25Q128
//4Kbytes为一个Sector
//16个扇区为1个Block:64
//W25Q128
//容量为128(Mbit)/8=16(MB)共有256个Block,4096个Sector
struct
{
    uint16_t W25QXX_TYPE;//flash型号
    uint32_t DENSITY; //单位B:总容量
    uint32_t RESERVED;//保留的容量，不给FatFS使用的，单位B;等于0就是说整个FLASH都是用FatFS管理
} W25QXX_TABLE[] =
  {
    {W25Q80,  80 * SIZE_1M / 8, 0},
    {W25Q16,  16 * SIZE_1M / 8, 0},
    {W25Q32,  32 * SIZE_1M / 8, 0},
    {W25Q64,  64 * SIZE_1M / 8, 0},
    {W25Q128, 128 * SIZE_1M / 8, 7 * SIZE_1M},
  };
 #if  0 
uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[6], u8IDCnt = 0;

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x90, Read Manufacturer/Device ID
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x90);

    // send 24-bit '0', dummy
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // receive 16-bit
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
        u8RxData[u8IDCnt ++] = SPI_READ_RX(SPI_FLASH_PORT);

    return ( (u8RxData[4]<<8) | u8RxData[5] );
}  
  
  
uint8_t SpiFlash_ReadStatusReg(void)
{
    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    SPI_READ_RX(SPI_FLASH_PORT);

    return (SPI_READ_RX(SPI_FLASH_PORT) & 0xff);
}  
/*--------------------------------------------------------------------------------------------
//Function: void SpiFlash_WaitReady(void)
//Input:  
//Output:   None 
//Description：等待flash操作完毕
//--------------------------------------------------------------------------------------------*/  
void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue;

    do
    {
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;
    }
    while(ReturnValue!=0);   // check the BUSY bit
}  
/*--------------------------------------------------------------------------------------------
//Function: void SPI_Init(void)
//Input:  
//Output:   None 
//Description：初始化spi
//--------------------------------------------------------------------------------------------*/
void SPI_Init(void)
{


	 uint16_t u16ID;
    /* Unlock protected registers */
    SYS_UnlockReg();
  /* Select IP clock source */
   
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL2_SPI1_S_HCLK, 0);

    /* Enable IP clock */
    CLK_EnableModuleClock(SPI1_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    SYS->PC_H_MFP = (SYS_PC_H_MFP_PC8_MFP_SPI1_SS0 | SYS_PC_H_MFP_PC9_MFP_SPI1_SCLK | SYS_PC_H_MFP_PC10_MFP_SPI1_MISO0 | SYS_PC_H_MFP_PC11_MFP_SPI1_MOSI0);
    /* Lock protected registers */
    GPIO_SetMode(PA, BIT15, GPIO_PMD_OUTPUT);
    PA15=1;
    SYS_LockReg();
    
    
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, 2000000);
 /* Enable the automatic hardware slave select function. Select the SS0 pin and configure as low-active. */
    SPI_EnableAutoSS(SPI_FLASH_PORT, SPI_SS0, SPI_SS0_ACTIVE_LOW);
    SPI_EnableFIFO(SPI_FLASH_PORT, 1, 1);
  /* Wait ready */
    SpiFlash_WaitReady();
    #if 1
		u16ID = SpiFlash_ReadMidDid();
   if(u16ID  != 0xEF17)  //w25q128
    {
        printf("Wrong ID, 0x%x\n", u16ID);
      
 
    }
   #endif
}

#endif



void  DELAY_ms(uint16_t microsecond)
 {
   	
 	
 }
 #if  0
void W25QXX_SPI_ReceiveBytes(uint8_t *pData,uint16_t n)
   {
   	
		W25QXX_SPI_Receive(W25QXX_SPI_PTR, pData, n);
    }			
void W25QXX_SPI_Receive(SPI_T spi_ptr,uint8_t  *pData, uint16_t Size)
  {
  	
			HAL_SPI_Receive(spi_ptr, pData, Size);
   }
//----------------------------------------------   			
void W25QXX_SPI_TransmitBytes(uint8_t *pData,uint16_t n)
   {
   			W25QXX_SPI_Transmit(W25QXX_SPI_PTR, pData, uint16_tn);
   }			
void W25QXX_SPI_Transmit(SPI_T spi_ptr, uint8_t *pData, uint16_t Size)
 {  uint16_t i;
        for(i=0;i<Size;i++)
 	  SPI_WRITE_TX0(spi_ptr,*pData++) ;
 }
//--------------------------------- 


void W25QXX_SPI_TransmitReceive(SPI_T *spi_ptr, uint8_t *pTxData,uint8_t  *pRxData,uint16_t Size)
   {
	uint16_t i;
		  SPI_SET_SS0_LOW(spi_ptr);
	for(i=0;i<Size;i++)
	 {    SPI_WRITE_TX0(spi_ptr,*pTxData++) ;
	      while(SPI_IS_BUSY(spi_ptr));
		pRxData[i] = SPI_READ_RX0(spi_ptr);
	 } 
	while(SPI_IS_BUSY(spi_ptr));  
    SPI_SET_SS0_HIGH(spi_ptr);
   }			


void W25QXX_SPI_TransmitReceiveBytes(uint8_t *pData,uint8_t *pRxData,uint16_t n)
  {   uint16_t i;
		  SPI_SET_SS0_LOW(W25QXX_SPI_PTR);
	        for(i=0;i<n;i++)   
  	           SPI_WRITE_TX0(W25QXX_SPI_PTR,*pData++) ;
  	       while(SPI_IS_BUSY(W25QXX_SPI_PTR));
  	    SPI_SET_SS0_HIGH(W25QXX_SPI_PTR);       
	 
  }


  #endif
 
 
 
 
static int16_t _ctr = 0 ;
int16_t inc_ctr(void)
{

    _ctr++;

    return _ctr ;
}

static int16_t FLASH_RW_LOCK(void)
{
    int16_t ctr = 1 ;

    return ctr ;
}

static int16_t FLASH_RW_UNLOCK(void)
{
    int16_t ctr = 0 ;

    return ctr ;
}

//void W25QXX_CS_SEL(void)
//{
//    //FLASH_RW_LOCK();
//    _W25QXX_CS_SEL();

//}
//void W25QXX_CS_DESEL(void)
//{
//    //FLASH_RW_UNLOCK();
//    _W25QXX_CS_DESEL();
//}



//#define W25QXX_CS_SEL() _W25QXX_CS_SEL()
//#define W25QXX_CS_DESEL() _W25QXX_CS_DESEL()
#define INIT_FLASH_OS_PARAM()


//FATFS W25QXX_FS;	
/*
*根据FLASH型号返回对应的可用容量
返回值res > 0 : 可用容量为 resM-bit
<0,错误没有该型号参数
*/
uint64_t W25QXX_GET_USABLE_DENSITY(uint16_t TYPE)
{int16_t i;
    uint64_t res = -1;//W25QXX_TABLE[i].DENSITY 是32位无符号整形，减法后可能小于0
    for(i = 0; i < sizeof(W25QXX_TABLE); i++)
    {
        if(TYPE == W25QXX_TABLE[i].W25QXX_TYPE)
        {
            res = (W25QXX_TABLE[i].DENSITY - W25QXX_TABLE[i].RESERVED) ;
            res = (res > 0) ? res : 0 ;
            break;
        }
    }
    return res;
}


static uint8_t W25QXX_ReadWriteByte(uint8_t Data)
{
    uint8_t RxData;
            //  SPI_SET_SS0_LOW(W25QXX_SPI_PTR);
	      SPI_WRITE_TX0(W25QXX_SPI_PTR,Data) ;
	      while(SPI_IS_BUSY(W25QXX_SPI_PTR));
	      RxData = SPI_READ_RX0(W25QXX_SPI_PTR);
	      while(SPI_IS_BUSY(W25QXX_SPI_PTR));
	   //   SPI_SET_SS0_HIGH(W25QXX_SPI_PTR);
              return RxData;
}

//W25QXX_SPI_TransmitReceive(SPI_FLASH_PORT,&res,&RxData, 1);
//初始化SPI FLASH的IO口
uint16_t W25QXX_Init(void)
{
    
    INIT_FLASH_OS_PARAM();
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();			//SPI FLASH不选中
    W25QXX_ReadWriteByte(RESET_ENABLE_CMD);
    W25QXX_ReadWriteByte(RESET_MEMORY_CMD);
    W25QXX_CS_DESEL();			//SPI FLASH不选中
    //	//设置为42M时钟,高速模式 ,初始化函数中已设置为36MHz
    W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.
    FLASH_RW_UNLOCK();
    return W25QXX_TYPE;
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t W25QXX_ReadSR(void)
{
    uint8_t byte = 0;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                           //使能器件
    W25QXX_ReadWriteByte(READ_STATUS_REG1_CMD);    //发送读取状态寄存器命令
    byte = W25QXX_ReadWriteByte(0Xff);           //读取一个字节
    W25QXX_CS_DESEL();                            //取消片选
    FLASH_RW_UNLOCK();
    return byte;
}
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
static void W25QXX_Write_SR(uint8_t sr)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(WRITE_STATUS_REG1_CMD);   //发送写取状态寄存器命令
    W25QXX_ReadWriteByte(sr);               //写入一个字节
    W25QXX_CS_DESEL();                            //取消片选
    FLASH_RW_UNLOCK();
}
//W25QXX写使能
//将WEL置位
static void W25QXX_Write_Enable(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();;                            //使能器件
    W25QXX_ReadWriteByte(WRITE_ENABLE_CMD);      //发送写使能
    W25QXX_CS_DESEL();;                            //取消片选
    FLASH_RW_UNLOCK();
}
//W25QXX写禁止
//将WEL清零
static void W25QXX_Write_Disable(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();;                            //使能器件
    W25QXX_ReadWriteByte(WRITE_DISABLE_CMD);     //发送写禁止指令
    W25QXX_CS_DESEL();;                            //取消片选
    FLASH_RW_UNLOCK();
}
//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
//static 
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;
     uint8_t id[100],k=0 ;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();
    
    W25QXX_ReadWriteByte(READ_ID_CMD);//发送读取ID命令
    W25QXX_ReadWriteByte(0x00);
    W25QXX_ReadWriteByte(0x00);
    W25QXX_ReadWriteByte(0x00);
	
    Temp |= W25QXX_ReadWriteByte(0x00) << 8;
    Temp |= W25QXX_ReadWriteByte(0x00);
    FLASH_RW_UNLOCK();

    return Temp;
}



//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
    {SpiFlash_NormalRead(ReadAddr, pBuffer,NumByteToRead);
    //SpiFlash_WaitReady();
}

void W25QXX_Read__(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint16_t i;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(READ_DATA_CMD);         //发送读取命令
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); //发送24bit地址
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)ReadAddr);
    for(i = 0; i < NumByteToRead; i++)
    {     WatchdogReset();
        pBuffer[i] = W25QXX_ReadWriteByte(0XFF); //循环读数
      
    }
   
    W25QXX_CS_DESEL();;
    FLASH_RW_UNLOCK();
}
//SPI在一页(0~65535)内写入少于256(W25QXX_PAGE_SIZE)个字节的数据
//在指定地址开始写入最大256(W25QXX_PAGE_SIZE)字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256(W25QXX_PAGE_SIZE)),该数不应该超过该页的剩余字节数!!!
static void W25QXX_Write_Page__(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    FLASH_RW_LOCK();
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(PAGE_PROG_CMD);      //发送写页命令
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); //发送24bit地址
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)WriteAddr);
    for(i = 0; i < NumByteToWrite; i++)
        W25QXX_ReadWriteByte(pBuffer[i]);//循环写数
    W25QXX_CS_DESEL();                            //取消片选
    W25QXX_Wait_Busy();					   //等待写入结束
    FLASH_RW_UNLOCK();
}

static void W25QXX_Write_Page(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
   { 
   	SpiFlash_NormalPageProgram(WriteAddr,pBuffer,NumByteToWrite);
   	SpiFlash_WaitReady();
   }
//读取SPI FLASH
//在指定地址开始读取指定长度的数据,并与pBuffer的内容比较
//pBuffer:数据对比区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要对比的字节数(最大65535)
//返回值:
//等于0:数据一致
//小于等于0:数据不一致，数值的绝对值大小为不一致的偏移
static uint32_t W25QXX_ReadCheck__1(const uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToCompare)
{
    uint16_t i;
    uint32_t res = 0;
	  uint8_t k;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(W25X_ReadData);         //发送读取命令
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); //发送24bit地址
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)ReadAddr);
   for(i = 0; i < NumByteToCompare; i++)
    {
       if( pBuffer[i]!= W25QXX_ReadWriteByte(0XFF)) //循环读数
	 {
            res = -i;
            break;
        }
    }
    W25QXX_CS_DESEL();
    FLASH_RW_UNLOCK();
    return res;
}

static uint32_t W25QXX_ReadCheck(const uint8_t* u8DataBuffer, uint32_t StartAddress, uint16_t n)
{   
	 uint16_t i;
         uint32_t res = 0;
	
	SPI_SET_SS0_LOW(SPI_FLASH_PORT);
  
    // send Command: 0x03, Read data
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x03);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);

    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    // clear RX buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    for(i=0; i<n; i++)
	 {
        SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
        while(SPI_IS_BUSY(SPI_FLASH_PORT));
        if(u8DataBuffer[i] != SPI_READ_RX(SPI_FLASH_PORT))
         {
            res = -i;
            break;
        }
			}
    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));
        
    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);
    return(res);
}

//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//NEED_CHECK:是否需要校验
//返回0:写入正确
// -1:写入失败
#if  0
static uint8_t W25QXX_Write_Check(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite, uint8_t NEED_CHECK)
{
    uint16_t pageremain;
    uint8_t res = 0;
    FLASH_RW_LOCK();
    pageremain = W25QXX_HW_PAGE_SIZE - WriteAddr % W25QXX_HW_PAGE_SIZE ; //单页剩余的字节数
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //不大于256(W25QXX_PAGE_SIZE)个字节

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
			 if(NEED_CHECK)
          {
            res = W25QXX_ReadCheck(pBuffer, WriteAddr, NumByteToWrite);
           if(res < 0 )
            goto FAIL ;
          if(NumByteToWrite == pageremain)
             break;//写入结束了
          else //NumByteToWrite>pageremain
          {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;			 //减去已经写入了的字节数
            if(NumByteToWrite > W25QXX_HW_PAGE_SIZE)
                pageremain = W25QXX_HW_PAGE_SIZE; //一次可以写入256(W25QXX_PAGE_SIZE)个字节
            else
                pageremain = NumByteToWrite; 	 //不够256(W25QXX_PAGE_SIZE)个字节了
          }
				}
			}		

SUCCESS:
    FLASH_RW_UNLOCK();
    return 0 ;
FAIL:
    FLASH_RW_UNLOCK();
    return -1;
    
}
#endif
static uint8_t W25QXX_Write_Check(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite, uint8_t NEED_CHECK)
{
    uint16_t pageremain;
    const uint8_t  *Temp_pBuffer;
    uint8_t res = 0;
    uint16_t Temp_NumByteToWrite;
	   uint32_t Temp_WriteAddr; 
    Temp_pBuffer=pBuffer;
	   Temp_WriteAddr=WriteAddr;
    Temp_NumByteToWrite=NumByteToWrite;
    FLASH_RW_LOCK();
    pageremain = W25QXX_HW_PAGE_SIZE - WriteAddr % W25QXX_HW_PAGE_SIZE ; //单页剩余的字节数
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //不大于256(W25QXX_PAGE_SIZE)个字节

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if(NumByteToWrite == pageremain)
            break;//写入结束了
        else //NumByteToWrite>pageremain
        {
           pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;			 //减去已经写入了的字节数
            if(NumByteToWrite > W25QXX_HW_PAGE_SIZE)
                pageremain = W25QXX_HW_PAGE_SIZE; //一次可以写入256(W25QXX_PAGE_SIZE)个字节
            else
                pageremain = NumByteToWrite; 	 //不够256(W25QXX_PAGE_SIZE)个字节了
        }
    };
    if(NEED_CHECK)
    {   
        res = W25QXX_ReadCheck(Temp_pBuffer, Temp_WriteAddr, Temp_NumByteToWrite);
        if(res < 0 )
            //return -1;
            goto FAIL ;
        else
            //return 0;
            goto SUCCESS ;
    }
    else
    {
        //return 0;
        goto SUCCESS ;
    }
SUCCESS:
    FLASH_RW_UNLOCK();
    return 0 ;
FAIL:
    FLASH_RW_UNLOCK();
    return -1;
    
}

//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//返回值表示，写入成功:0
// 失败: -1
static  uint8_t W25QXX_BUFFER[W25QXX_HW_ERASE_SIZE_ONCE];
uint8_t W25QXX_Write(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t * W25QXX_BUF;
    FLASH_RW_LOCK();
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / W25QXX_HW_ERASE_SIZE_ONCE; //扇区地址  这里的1扇区=1个最小可擦除大小
    secoff = WriteAddr % W25QXX_HW_ERASE_SIZE_ONCE; //在扇区内的偏移
    secremain = W25QXX_HW_ERASE_SIZE_ONCE - secoff; //扇区剩余空间大小
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
    if(NumByteToWrite <= secremain)  //是否跨扇区
        secremain = NumByteToWrite; //在当前扇区内
    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE); //读出整个扇区的内容
        for(i = 0; i < secremain; i++) //校验数据
        {
            if(W25QXX_BUF[secoff + i] != 0XFF) //剩下的扇区内容不为空,需要擦除
                break;//需要擦除
        }
        if( i < secremain )//需要擦除
        {
            W25QXX_Erase_Sector(secpos);//擦除这个扇区 1扇区=1个最小可擦除大小
            for(i = 0; i < secremain; i++)	 //复制
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0) //写入整个扇区  这里的1扇区=1个最小可擦除大小
            {  if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0)
                { if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0)
                     goto FAIL;
                }
            }

        }
        else  //不需要擦除
        {
            if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0) //写已经擦除了的,直接写入扇区剩余区间.
            {  if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0)  
                {if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0)
                //return -1;
                goto FAIL;
                }
            }
        }

        if(NumByteToWrite == secremain)
            break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff = 0; //偏移位置为0

            pBuffer += secremain; //指针偏移
            WriteAddr += secremain; //写地址偏移

            NumByteToWrite -= secremain;				//字节数递减

            if(NumByteToWrite > W25QXX_HW_ERASE_SIZE_ONCE)
                secremain = W25QXX_HW_ERASE_SIZE_ONCE;	//下一个扇区还是写不完
            else
                secremain = NumByteToWrite;			//下一个扇区可以写完了
        }
    };
    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    FLASH_RW_UNLOCK();
    return 0;
FAIL:
    FLASH_RW_UNLOCK();
    return -1 ;
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
    FLASH_RW_LOCK();
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_SEL();                             //使能器件
    W25QXX_ReadWriteByte(CHIP_ERASE_CMD);        //发送片擦除命令
    W25QXX_CS_DESEL();                             //取消片选
    W25QXX_Wait_Busy();   				   //等待芯片擦除结束
    FLASH_RW_UNLOCK();
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
static void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    FLASH_RW_LOCK();
    //监视falsh擦除情况,测试用
// 	printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= W25QXX_HW_ERASE_SIZE_ONCE;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(SECTOR_ERASE_CMD);      //发送扇区擦除指令
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr) >> 16)); //发送24bit地址
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)Dst_Addr);
    W25QXX_CS_DESEL();                            //取消片选
    W25QXX_Wait_Busy();   				   //等待擦除完成
    FLASH_RW_UNLOCK();
}
//等待空闲
uint8_t W25QXX_Wait_Busy(void)
{
    //time_t time_now, time_start = 0;
  //  gettime(&time_start, NULL);
    FLASH_RW_LOCK();
    while((W25QXX_ReadSR() & 0x01) == 0x01) // 等待BUSY位清空
    {
      //  gettime(&time_now, NULL);
      //  if( difftime( time_now , time_start ) > 10 )
      //  {
            return W25Qx_TIMEOUT;
      //  }
    }
    FLASH_RW_UNLOCK();
    return W25Qx_OK;
}
//进入掉电模式
void W25QXX_PowerDown(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                             //使能器件
    W25QXX_ReadWriteByte(W25X_PowerDown);        //发送掉电命令
    W25QXX_CS_DESEL();                           //取消片选
    //delay_us(3);                               //等待TPD
    DELAY_ms(2);//别人写的是3us，我这里为了简单，就延迟1ms,write by mjw
    FLASH_RW_UNLOCK();


}
//唤醒
void W25QXX_WAKEUP(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //使能器件
    W25QXX_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
    W25QXX_CS_DESEL();                            //取消片选
    DELAY_ms(2);//别人写的是3us，我这里为了简单，就延迟1ms
    FLASH_RW_UNLOCK();
}

#define   len 250
void TestSpi()
 {
    uint8_t W25QXX_BUFFER1[len];
    uint8_t W25QXX_BUFFER2[len];
      uint16_t i,j,m,n;	
      static uint8_t k=0;
      printf("Erasing Chip ...\r\n");
      SpiFlash_ChipErase();
      SpiFlash_WaitReady();
      
	for(m=0;m<10;m++) 
	{ for(i=0;i<10;i++)
	   {   k++;
	   	 for(n=0;n<len;n++)
               W25QXX_BUFFER1[n]=(n+k)%256;
		while(1)	 
		{   W25QXX_Write(W25QXX_BUFFER1, i*300, len);
	      // W25QXX_Write_Page(W25QXX_BUFFER1, i*256, len);
		    W25QXX_Read(W25QXX_BUFFER2, i*300,len);	 
		    for(j=0;j<len;j++)
                     {            if(W25QXX_BUFFER1[j]!=W25QXX_BUFFER2[j])
			           {  printf("Check wrong:   loop:  %d,  turn: %d, Pos:%d ,   write data :%d,  read data:%d,  Addr: %d ,  \r\n",m,i,j,W25QXX_BUFFER1[j],W25QXX_BUFFER2[j],i*300+j);
			              break;
			           }
			    }
	   	if(j==len)
		    {	   printf("Check OK   :    loop:  %d  turn: %d Pos:%d  start data :%d   \r\n",m,i,j,W25QXX_BUFFER1[0]);
	        break;
		     }
		}
	}
     }		
	#if  0	 
	 DelaySecond(2);
	  for(i=0;i<4;i++)
         for(j=0;i<256;j++)
            W25QXX_BUFFER2[i][j]=0xff;
	W25QXX_Read(W25QXX_BUFFER2[0], 0x0001000,256);
	W25QXX_Read(W25QXX_BUFFER2[1], 0x0001100,256);
	W25QXX_Read(W25QXX_BUFFER2[2], 0x0001200,256); 
	W25QXX_Read(W25QXX_BUFFER2[3], 0x0001300,256); 
     W25QXX_Write(W25QXX_BUFFER1, 0x0001100, len);
     while(SPI_IS_BUSY(SPI_FLASH_PORT));
DelaySecond(2);	 
       for(i=0;i<4;i++)
         for(j=0;i<256;j++)
            W25QXX_BUFFER2[i][j]=0xff;
      
      W25QXX_Read(W25QXX_BUFFER2[0], 0x0001000,256);
	W25QXX_Read(W25QXX_BUFFER2[1], 0x0001100,256);
	W25QXX_Read(W25QXX_BUFFER2[2], 0x0001200,256); 
	W25QXX_Read(W25QXX_BUFFER2[3], 0x0001300,256); 

 
	  W25QXX_Write(W25QXX_BUFFER1, 0x0001200, len);
	  while(SPI_IS_BUSY(SPI_FLASH_PORT));
		DelaySecond(2);
		W25QXX_Read(W25QXX_BUFFER2[0], 0x0001000,256);
	 for(i=0;i<4;i++)
         for(j=0;i<256;j++)
            W25QXX_BUFFER2[i][j]=0xff;
	W25QXX_Read(W25QXX_BUFFER2[1], 0x0001100,256);
	W25QXX_Read(W25QXX_BUFFER2[2], 0x0001200,256); 
	W25QXX_Read(W25QXX_BUFFER2[3], 0x0001300,256); 
	  W25QXX_Write(W25QXX_BUFFER1, 0x0001300, len); 
	  while(SPI_IS_BUSY(SPI_FLASH_PORT)); 
DelaySecond(2);
 for(i=0;i<4;i++)
         for(j=0;i<256;j++)
            W25QXX_BUFFER2[i][j]=0xff; 
W25QXX_Read(W25QXX_BUFFER2[0], 0x0001000,256);
	W25QXX_Read(W25QXX_BUFFER2[1], 0x0001100,256);
	W25QXX_Read(W25QXX_BUFFER2[2], 0x0001200,256); 
	W25QXX_Read(W25QXX_BUFFER2[3], 0x0001300,256); 
 	  
 	for(i=0;i<len;i++)
       W25QXX_BUFFER1[i]=0xff;
			 
			 #endif
   }























