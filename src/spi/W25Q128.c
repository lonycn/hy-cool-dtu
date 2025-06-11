#include "w25qxx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"


#define SIZE_1M (1024*1024)
uint16_t W25QXX_TYPE = W25Q128;	//Ĭ����W25Q128
//4KbytesΪһ��Sector
//16������Ϊ1��Block:64
//W25Q128
//����Ϊ128(Mbit)/8=16(MB)����256��Block,4096��Sector
struct
{
    uint16_t W25QXX_TYPE;//flash�ͺ�
    uint32_t DENSITY; //��λB:������
    uint32_t RESERVED;//����������������FatFSʹ�õģ���λB;����0����˵����FLASH������FatFS����
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
//Description���ȴ�flash�������
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
//Description����ʼ��spi
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
*����FLASH�ͺŷ��ض�Ӧ�Ŀ�������
����ֵres > 0 : ��������Ϊ resM-bit
<0,����û�и��ͺŲ���
*/
uint64_t W25QXX_GET_USABLE_DENSITY(uint16_t TYPE)
{int16_t i;
    uint64_t res = -1;//W25QXX_TABLE[i].DENSITY ��32λ�޷������Σ����������С��0
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
//��ʼ��SPI FLASH��IO��
uint16_t W25QXX_Init(void)
{
    
    INIT_FLASH_OS_PARAM();
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();			//SPI FLASH��ѡ��
    W25QXX_ReadWriteByte(RESET_ENABLE_CMD);
    W25QXX_ReadWriteByte(RESET_MEMORY_CMD);
    W25QXX_CS_DESEL();			//SPI FLASH��ѡ��
    //	//����Ϊ42Mʱ��,����ģʽ ,��ʼ��������������Ϊ36MHz
    W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID.
    FLASH_RW_UNLOCK();
    return W25QXX_TYPE;
}

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t W25QXX_ReadSR(void)
{
    uint8_t byte = 0;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                           //ʹ������
    W25QXX_ReadWriteByte(READ_STATUS_REG1_CMD);    //���Ͷ�ȡ״̬�Ĵ�������
    byte = W25QXX_ReadWriteByte(0Xff);           //��ȡһ���ֽ�
    W25QXX_CS_DESEL();                            //ȡ��Ƭѡ
    FLASH_RW_UNLOCK();
    return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
static void W25QXX_Write_SR(uint8_t sr)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(WRITE_STATUS_REG1_CMD);   //����дȡ״̬�Ĵ�������
    W25QXX_ReadWriteByte(sr);               //д��һ���ֽ�
    W25QXX_CS_DESEL();                            //ȡ��Ƭѡ
    FLASH_RW_UNLOCK();
}
//W25QXXдʹ��
//��WEL��λ
static void W25QXX_Write_Enable(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();;                            //ʹ������
    W25QXX_ReadWriteByte(WRITE_ENABLE_CMD);      //����дʹ��
    W25QXX_CS_DESEL();;                            //ȡ��Ƭѡ
    FLASH_RW_UNLOCK();
}
//W25QXXд��ֹ
//��WEL����
static void W25QXX_Write_Disable(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();;                            //ʹ������
    W25QXX_ReadWriteByte(WRITE_DISABLE_CMD);     //����д��ָֹ��
    W25QXX_CS_DESEL();;                            //ȡ��Ƭѡ
    FLASH_RW_UNLOCK();
}
//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
//static 
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;
     uint8_t id[100],k=0 ;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();
    
    W25QXX_ReadWriteByte(READ_ID_CMD);//���Ͷ�ȡID����
    W25QXX_ReadWriteByte(0x00);
    W25QXX_ReadWriteByte(0x00);
    W25QXX_ReadWriteByte(0x00);
	
    Temp |= W25QXX_ReadWriteByte(0x00) << 8;
    Temp |= W25QXX_ReadWriteByte(0x00);
    FLASH_RW_UNLOCK();

    return Temp;
}



//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
    {SpiFlash_NormalRead(ReadAddr, pBuffer,NumByteToRead);
    //SpiFlash_WaitReady();
}

void W25QXX_Read__(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint16_t i;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(READ_DATA_CMD);         //���Ͷ�ȡ����
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); //����24bit��ַ
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)ReadAddr);
    for(i = 0; i < NumByteToRead; i++)
    {     WatchdogReset();
        pBuffer[i] = W25QXX_ReadWriteByte(0XFF); //ѭ������
      
    }
   
    W25QXX_CS_DESEL();;
    FLASH_RW_UNLOCK();
}
//SPI��һҳ(0~65535)��д������256(W25QXX_PAGE_SIZE)���ֽڵ�����
//��ָ����ַ��ʼд�����256(W25QXX_PAGE_SIZE)�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256(W25QXX_PAGE_SIZE)),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
static void W25QXX_Write_Page__(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    FLASH_RW_LOCK();
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(PAGE_PROG_CMD);      //����дҳ����
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); //����24bit��ַ
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)WriteAddr);
    for(i = 0; i < NumByteToWrite; i++)
        W25QXX_ReadWriteByte(pBuffer[i]);//ѭ��д��
    W25QXX_CS_DESEL();                            //ȡ��Ƭѡ
    W25QXX_Wait_Busy();					   //�ȴ�д�����
    FLASH_RW_UNLOCK();
}

static void W25QXX_Write_Page(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
   { 
   	SpiFlash_NormalPageProgram(WriteAddr,pBuffer,NumByteToWrite);
   	SpiFlash_WaitReady();
   }
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����,����pBuffer�����ݱȽ�
//pBuffer:���ݶԱ���
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ�Աȵ��ֽ���(���65535)
//����ֵ:
//����0:����һ��
//С�ڵ���0:���ݲ�һ�£���ֵ�ľ���ֵ��СΪ��һ�µ�ƫ��
static uint32_t W25QXX_ReadCheck__1(const uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToCompare)
{
    uint16_t i;
    uint32_t res = 0;
	  uint8_t k;
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); //����24bit��ַ
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)ReadAddr);
   for(i = 0; i < NumByteToCompare; i++)
    {
       if( pBuffer[i]!= W25QXX_ReadWriteByte(0XFF)) //ѭ������
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

//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//NEED_CHECK:�Ƿ���ҪУ��
//����0:д����ȷ
// -1:д��ʧ��
#if  0
static uint8_t W25QXX_Write_Check(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite, uint8_t NEED_CHECK)
{
    uint16_t pageremain;
    uint8_t res = 0;
    FLASH_RW_LOCK();
    pageremain = W25QXX_HW_PAGE_SIZE - WriteAddr % W25QXX_HW_PAGE_SIZE ; //��ҳʣ����ֽ���
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //������256(W25QXX_PAGE_SIZE)���ֽ�

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
			 if(NEED_CHECK)
          {
            res = W25QXX_ReadCheck(pBuffer, WriteAddr, NumByteToWrite);
           if(res < 0 )
            goto FAIL ;
          if(NumByteToWrite == pageremain)
             break;//д�������
          else //NumByteToWrite>pageremain
          {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;			 //��ȥ�Ѿ�д���˵��ֽ���
            if(NumByteToWrite > W25QXX_HW_PAGE_SIZE)
                pageremain = W25QXX_HW_PAGE_SIZE; //һ�ο���д��256(W25QXX_PAGE_SIZE)���ֽ�
            else
                pageremain = NumByteToWrite; 	 //����256(W25QXX_PAGE_SIZE)���ֽ���
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
    pageremain = W25QXX_HW_PAGE_SIZE - WriteAddr % W25QXX_HW_PAGE_SIZE ; //��ҳʣ����ֽ���
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //������256(W25QXX_PAGE_SIZE)���ֽ�

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if(NumByteToWrite == pageremain)
            break;//д�������
        else //NumByteToWrite>pageremain
        {
           pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;			 //��ȥ�Ѿ�д���˵��ֽ���
            if(NumByteToWrite > W25QXX_HW_PAGE_SIZE)
                pageremain = W25QXX_HW_PAGE_SIZE; //һ�ο���д��256(W25QXX_PAGE_SIZE)���ֽ�
            else
                pageremain = NumByteToWrite; 	 //����256(W25QXX_PAGE_SIZE)���ֽ���
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

//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//����ֵ��ʾ��д��ɹ�:0
// ʧ��: -1
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
    secpos = WriteAddr / W25QXX_HW_ERASE_SIZE_ONCE; //������ַ  �����1����=1����С�ɲ�����С
    secoff = WriteAddr % W25QXX_HW_ERASE_SIZE_ONCE; //�������ڵ�ƫ��
    secremain = W25QXX_HW_ERASE_SIZE_ONCE - secoff; //����ʣ��ռ��С
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if(NumByteToWrite <= secremain)  //�Ƿ������
        secremain = NumByteToWrite; //�ڵ�ǰ������
    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE); //������������������
        for(i = 0; i < secremain; i++) //У������
        {
            if(W25QXX_BUF[secoff + i] != 0XFF) //ʣ�µ��������ݲ�Ϊ��,��Ҫ����
                break;//��Ҫ����
        }
        if( i < secremain )//��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);//����������� 1����=1����С�ɲ�����С
            for(i = 0; i < secremain; i++)	 //����
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0) //д����������  �����1����=1����С�ɲ�����С
            {  if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0)
                { if(W25QXX_Write_Check(W25QXX_BUF, secpos * W25QXX_HW_ERASE_SIZE_ONCE, W25QXX_HW_ERASE_SIZE_ONCE, 1) < 0)
                     goto FAIL;
                }
            }

        }
        else  //����Ҫ����
        {
            if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0) //д�Ѿ������˵�,ֱ��д������ʣ������.
            {  if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0)  
                {if(W25QXX_Write_Check(pBuffer, WriteAddr, secremain, 1) < 0)
                //return -1;
                goto FAIL;
                }
            }
        }

        if(NumByteToWrite == secremain)
            break;//д�������
        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff = 0; //ƫ��λ��Ϊ0

            pBuffer += secremain; //ָ��ƫ��
            WriteAddr += secremain; //д��ַƫ��

            NumByteToWrite -= secremain;				//�ֽ����ݼ�

            if(NumByteToWrite > W25QXX_HW_ERASE_SIZE_ONCE)
                secremain = W25QXX_HW_ERASE_SIZE_ONCE;	//��һ����������д����
            else
                secremain = NumByteToWrite;			//��һ����������д����
        }
    };
    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    FLASH_RW_UNLOCK();
    return 0;
FAIL:
    FLASH_RW_UNLOCK();
    return -1 ;
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    FLASH_RW_LOCK();
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_SEL();                             //ʹ������
    W25QXX_ReadWriteByte(CHIP_ERASE_CMD);        //����Ƭ��������
    W25QXX_CS_DESEL();                             //ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
    FLASH_RW_UNLOCK();
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
static void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    FLASH_RW_LOCK();
    //����falsh�������,������
// 	printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= W25QXX_HW_ERASE_SIZE_ONCE;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(SECTOR_ERASE_CMD);      //������������ָ��
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr) >> 16)); //����24bit��ַ
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));
    W25QXX_ReadWriteByte((uint8_t)Dst_Addr);
    W25QXX_CS_DESEL();                            //ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				   //�ȴ��������
    FLASH_RW_UNLOCK();
}
//�ȴ�����
uint8_t W25QXX_Wait_Busy(void)
{
    //time_t time_now, time_start = 0;
  //  gettime(&time_start, NULL);
    FLASH_RW_LOCK();
    while((W25QXX_ReadSR() & 0x01) == 0x01) // �ȴ�BUSYλ���
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
//�������ģʽ
void W25QXX_PowerDown(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                             //ʹ������
    W25QXX_ReadWriteByte(W25X_PowerDown);        //���͵�������
    W25QXX_CS_DESEL();                           //ȡ��Ƭѡ
    //delay_us(3);                               //�ȴ�TPD
    DELAY_ms(2);//����д����3us��������Ϊ�˼򵥣����ӳ�1ms,write by mjw
    FLASH_RW_UNLOCK();


}
//����
void W25QXX_WAKEUP(void)
{
    FLASH_RW_LOCK();
    W25QXX_CS_SEL();                            //ʹ������
    W25QXX_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
    W25QXX_CS_DESEL();                            //ȡ��Ƭѡ
    DELAY_ms(2);//����д����3us��������Ϊ�˼򵥣����ӳ�1ms
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























