
//#define   _UseNrf51822
#define   __Nano100

#ifdef  __Nano100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#endif



#ifdef  _UseNrf51822

#include "config.h"
#include "nrf_delay.h"
#include "radio_config.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_temp.h"
#include "simple_uart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "misc.h"
#include "uart.h"
#include "atomic.h"
#include "timer.h"
#endif




typedef struct _TXPACKET_FORMAT // λ��
{  
     
	uint64_t tag_id;//��ǩID
//	uint8_t tag[8];//��ǩID
   
	int16_t  temp;//�¶�
	int16_t  humi;//ʪ��
	uint64_t bat_level:3;//��ǩ����
	uint64_t rec_on:1;//�Ƿ����ڼ�¼����
	uint64_t rec_cnt:15;//��ǰ��¼������	
	uint64_t version:8;//��ǩ�汾
	uint64_t CONTENT:2;//������������ ֻ���¶Ȼ����¶Ⱥ�ʪ��
	uint64_t NOT_USE:8;
	uint64_t :0;//����
}TXPACKET_FORMAT;

TXPACKET_FORMAT tx_packet,*ptx_packet;

#define TX_ADR_WIDTH   3  				// 5�ֽڿ�ȵķ���/���յ�ַ
#define TX_PLOAD_WIDTH     17 //sizeof(TXPACKET_FORMAT)  				// ����ͨ����Ч���ݿ��
//********************************************************************************************************************//
// SPI(SI24R1) commands
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

//********************************************************************************************************************//
// SPI(SI24R1) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define RSSI            0x09  // 'Received Signal Strength Indecator' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

//********************************************************************************************************************//
// STATUS Register 
#define RX_DR						0x40  /**/
#define TX_DS						0x20
#define MAX_RT					0x10

//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
//SI24R1 API Functions
void SI24R1_Init(void); //SI24R1 Pin Init
unsigned char SI24R1_Write_Reg(unsigned char reg, unsigned char value); 
unsigned char SI24R1_Write_Buf(unsigned char reg, const unsigned char *pBuf, unsigned char bytes);
unsigned char SI24R1_Read_Reg(unsigned char reg);
unsigned char SI24R1_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);

void SI24R1_RX_Mode(void);
void SI24R1_TX_Mode(void);
unsigned char SI24R1_RxPacket(unsigned char *rxbuf);
unsigned char SI24R1_TxPacket(unsigned char *txbuf);

//unsigned char  TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // ����һ����̬���͵�ַ

//unsigned char  TX_ADDRESS[TX_ADR_WIDTH] = {0x55  , 0x57, 0x4A ,0x55, 0x4d};
                        
unsigned char  TX_ADDRESS[TX_ADR_WIDTH] = {0x66  , 0x66, 0x66 };
#ifdef  __Nano100
//SI24R1 PIN DEFINITION

#define	MOSI      PC2            // Master Out, Slave In pin (output)
#define	MISO      PC3             // Master In, Slave Out pin (input)
#define	SCK       PE5            // Serial Clock pin, (output)
#define	CSN       PB9             // Slave Select pin, (output to CSN)
#define	CE        PA14             // Chip Enable pin signal (output)
#define	IRQ       PD15          // Interrupt signal, from nRF24L01 (input)


#if  0
#define	MOSI      PA3             // Master Out, Slave In pin (output)
#define	MISO      PC14            // Master In, Slave Out pin (input)
#define	SCK       PC15            // Serial Clock pin, (output)
#define	CSN       PA2             // Slave Select pin, (output to CSN)
#define	CE        PA4             // Chip Enable pin signal (output)
#define	IRQ       PC7           // Interrupt signal, from nRF24L01 (input)
#endif

/********************************************************
�������ܣ�SI24R1���ų�ʼ��                
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_Init(void)
{    GPIO_SetMode(PC, BIT2, GPIO_PMD_OUTPUT);
     GPIO_SetMode(PE, BIT5, GPIO_PMD_OUTPUT);
     GPIO_SetMode(PB, BIT9, GPIO_PMD_OUTPUT);
     GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
     GPIO_SetMode(PC, BIT3, GPIO_PMD_INPUT);
     GPIO_SetMode(PD, BIT15, GPIO_PMD_INPUT);	
	SCK=0; 													//SPIʱ��������
	CSN=1;				
	CE=0;				
	//IRQ=1;
	SI24R1_RX_Mode();
}
#endif

#ifdef _UseNrf51822

#define	MOSI_PIN      11            // SCL2
#define	MISO_PIN      15            // SDA1
#define	SCK_PIN       12             // SDA3
#define	CSN_PIN       14             // SCL1
#define	CE_PIN        16             // SDA2
#define	IRQ_PIN       21             // CONFIG




#define	MOSI=1;    nrf_gpio_pin_set(MOSI_PIN); 
#define	MOSI=0;    nrf_gpio_pin_clear(MOSI_PIN);
#define	SCK=1;     nrf_gpio_pin_set(SCK_PIN); 
#define	SCK=0;     nrf_gpio_pin_clear(SCK_PIN);
#define	CSN=1;     nrf_gpio_pin_set(CSN_PIN);  
#define	CSN=0;     nrf_gpio_pin_clear(CSN_PIN);
#define	CE=1;      nrf_gpio_pin_set(CE_PIN); 
#define	CE=0;      nrf_gpio_pin_clear(CE_PIN);
#define	IRQ=1;     nrf_gpio_pin_set(IRQ_PIN); 
#define	IRQ        nrf_gpio_pin_read(IRQ)
#define	MISO       nrf_gpio_pin_read(MISO)


/********************************************************
�������ܣ�SI24R1���ų�ʼ��                
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_Init(void)
{      
	 leds_init_output( MOSI_PIN,MOSI_PIN );
         leds_init_output( SCK_PIN,SCK_PIN);
         leds_init_output( CSN_PIN,CSN_PIN );
         leds_init_output( CE_PIN,CE_PIN);
         nrf_gpio_cfg_input(MISO_PIN, NRF_GPIO_PIN_PULLUP);
         nrf_gpio_cfg_input(IRQ_PIN, NRF_GPIO_PIN_PULLUP);
	 SCK=0; 													//SPIʱ��������
	 CSN=1;				
	 CE=0;				
	//IRQ=1;
	SI24R1_RX_Mode();
}

#endif




static unsigned char SPI_RW(unsigned char byte)
{
	unsigned char bit_ctr;
	for(bit_ctr=0; bit_ctr<8; bit_ctr++)
	{
		if(byte & 0x80)
			MOSI=1;
		else
			MOSI=0;																 
		byte = (byte << 1);                      
		SCK=1;                                   
		byte |= MISO;                             
		SCK=0;                                
	}
	return(byte);                              
}





/********************************************************
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_REG��reg��
					value:�Ĵ�����ֵ
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
unsigned char SI24R1_Write_Reg(unsigned char reg, unsigned char value)
{
	unsigned char status;

	CSN=0;                 
	status = SPI_RW(reg);				
	SPI_RW(value);
	CSN=1;  
	
	return(status);
}


/********************************************************
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��WRITE_REG��reg��
					pBuf:д�����׵�ַ
					bytes:д�����ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
unsigned char SI24R1_Write_Buf(unsigned char reg, const unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;

  CSN=0;                                  			
  status = SPI_RW(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI_RW(*pBuf++);
  CSN=1;                                      	

  return(status);       
}							  					   


/********************************************************
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��READ_REG��reg��
����  ֵ���Ĵ���ֵ
*********************************************************/
unsigned char SI24R1_Read_Reg(unsigned char reg)
{
 	unsigned char value;

	CSN=0;    
	SPI_RW(reg);			
	value = SPI_RW(0);
	CSN=1;              

	return(value);
}


/********************************************************
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ��READ_REG��reg��
					pBuf:���ջ��������׵�ַ
					bytes:��ȡ�ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
unsigned char SI24R1_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;

  CSN=0;                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //��ȡ���ݣ����ֽ���ǰ
  CSN=1;                                        

  return(status);    
}


/********************************************************
�������ܣ�SI24R1����ģʽ��ʼ��                      
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE=0;
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x00);               						// ʹ�ܽ���ͨ��0�Զ�Ӧ��
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);           						// ʹ�ܽ���ͨ��0
	SI24R1_Write_Reg(WRITE_REG + RF_CH, 9);                 						// ѡ����Ƶͨ��0x40
	SI24R1_Write_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  						// ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x27);//250k            						// ���ݴ�����2Mbps�����书��7dBm
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);              						// CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
	SI24R1_Write_Reg(WRITE_REG + STATUS, 0xff);  												//������е��жϱ�־λ
	CE=1;                                            									// ����CE���������豸
}						


/********************************************************
�������ܣ�SI24R1����ģʽ��ʼ��                      
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE=0;
	SI24R1_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // д�뷢�͵�ַ
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ

	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x00);    //01--00   											// ʹ�ܽ���ͨ��0�Զ�Ӧ��
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);   											// ʹ�ܽ���ͨ��0
	SI24R1_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a);  											// �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
	SI24R1_Write_Reg(WRITE_REG + RF_CH, 9);         											// ѡ����Ƶͨ��0x40
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x27);//0f---47    											// ���ݴ�����2Mbps�����书��7dBm
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e); //0e-06     											// CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	//CE = 1;
}


/********************************************************
�������ܣ���ȡ��������                       
��ڲ�����rxbuf:�������ݴ���׵�ַ
����  ֵ��0:���յ�����
          1:û�н��յ�����
*********************************************************/
unsigned char SI24R1_RxPacket(unsigned char *rxbuf)
{
	unsigned char state;
	state = SI24R1_Read_Reg(STATUS);  			                 //��ȡ״̬�Ĵ�����ֵ    	  
	SI24R1_Write_Reg(WRITE_REG+STATUS,state);               //���RX_DS�жϱ�־

	if(state & RX_DR)								                           //���յ�����
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //��ȡ����
		SI24R1_Write_Reg(FLUSH_RX,0xff);					              //���RX FIFO�Ĵ���
		return 0; 
	}	   
	return 1;                                                   //û�յ��κ�����
}


/********************************************************
�������ܣ�����һ�����ݰ�                      
��ڲ�����txbuf:Ҫ���͵�����
����  ֵ��0x10:�ﵽ����ط�����������ʧ�� 
          0x20:���ͳɹ�            
          0xff:����ʧ��                  
*********************************************************/
unsigned char SI24R1_TxPacket(unsigned char *txbuf)
{
	unsigned char state;
	CE=0;																										  //CE���ͣ�ʹ��SI24R1����
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //д���ݵ�TX FIFO,32���ֽ�
 	CE=1;																										  //CE�øߣ�ʹ�ܷ���	   
	
	while(IRQ==1);																				  //�ȴ��������
	state = SI24R1_Read_Reg(STATUS);  											  //��ȡ״̬�Ĵ�����ֵ	   
	SI24R1_Write_Reg(WRITE_REG+STATUS, state); 								//���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_RT)																			    //�ﵽ����ط�����
	{
		SI24R1_Write_Reg(FLUSH_TX,0xff);										    //���TX FIFO�Ĵ��� 
		return MAX_RT; 
	}
	if(state&TX_DS)																			      //�������
	{
		return TX_DS;
	}
	return 0XFF;																						  //����ʧ��
}

unsigned char buf[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30},BufCount=0;

void Si24R1_mainInital(void)
  { unsigned char  i;
     SI24R1_Init();
     SI24R1_RX_Mode();	
     for(BufCount=0;BufCount<32;BufCount++)
       buf[BufCount]=0;
     
  }

void Si24R1Tx_mainMachine(void)
{        
 static  unsigned char TxStatus=0,i;
	#if 0
	   TxStatus++;
	   for(i=0;i<32;i++)
	     buf[i] = i+ TxStatus;
	   if(TxStatus>=10)
	#endif 		    TxStatus=0;
	 		    
				SI24R1_TX_Mode();
				SI24R1_TxPacket(buf);
	                        SI24R1_RX_Mode();
 }	
		
	
void Si24R1Rx_mainMachine(void)
{        
 unsigned char i;
		if(!SI24R1_RxPacket(buf))
		{     	
		 #ifdef   __Nano100
		   SendString_uart0(buf);
		 #endif
		 
		 #ifdef   _UseNrf51822
		 uart_printf("%s \r\n",buf);
		  #endif 
		}
	
}


uint64_t id= 0x12345;


void set_tx_packet(TXPACKET_FORMAT* packet,int16_t temperature,int16_t humidity,int8_t bat_level,uint8_t rec_on,uint16_t cnt)
{
	//uint64_t id;
	id++;
	packet->CONTENT = 1;
	packet->version = 1;
	
	packet->tag_id =id;
	packet->bat_level = bat_level;
	packet->humi = humidity;
	packet->temp = temperature;
	packet->rec_on = rec_on;
	packet->rec_cnt = cnt;
	
}

void SetDoorMagicPacakage(uint8_t status)
 {uint8_t *ptr,i,j;
   if(status==2)  //open
      set_tx_packet(&tx_packet,0x12,0x34,7,0,1000);//�������ݰ�
   else if(status==1)
    set_tx_packet(&tx_packet,-100,-100,7,0,1000);//�������ݰ�
 else 
    set_tx_packet(&tx_packet,0x67,0x89,7,1,1000);//�������ݰ�   
  ptx_packet=&tx_packet;
    
 ptr =(uint8_t *) ptx_packet;
   j=sizeof(TXPACKET_FORMAT);
    for(i=0;i<20;i++)
      buf[i]=*ptr++;
     
 }
 
void  MagicAlarmAtOnce()
 {
  if(BakAlarm!=ReturnAlarmStatus())
  {  BakAlarm=ReturnAlarmStatus();
     SetDoorMagicPacakage(ReturnAlarmStatus()) ;						 
     Si24R1Tx_mainMachine( ); 
   }
  }    