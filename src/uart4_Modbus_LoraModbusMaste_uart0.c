#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "Nano100Series.h"

#include "sys.h"
#include "rtc.h"
#define _GLOBAL_H
#include	"global.h"

//modbus ͨѶЭ�������
#define cComandReadCoil               0x01
#define cComandReadDiStatus           0x02
#define cComandReadHoldRegister       0x03
#define cComandReadDiRegister         0x04

#define cComandWriteSingleCoil 0x05
#define cComandWriteMoreCoil   0x0f

#define cComandWriteSingleUint 0x06
#define cComandWriteMoreUint   0x10


//CONSIST
#define cRightStatus                 0x01
#define cCrcWrong                    0x02
#define cModbusOverTime              0x05 
//---------------------------

#define       cBufferLen_uart0     500
extern uint16_t    WritePiont_uart0;
extern uint16_t    ReadPiont_uart0;
extern unsigned char   ReceiveBuff_uart0[cBufferLen_uart0];
extern unsigned char   SendTempBufferBak_uart0[300];
uint8_t   uart0_LoopDeviceNo;
uint8_t   uart0_loop_stage=0;

uint8_t	  uart0_CloudCount;
uint8_t	  uart0_CloudComandLen; //�м����  ����������̵İ���
uint8_t	  uart0_CloudComandNo;
uint8_t	  uart0_CloudComandFunction;
uint8_t   uart0_CloudComandResondWaiting;
uint8_t   uart0_CloudComandResondLen;
uint8_t   uart0_Sendcomand[5][20];
uint8_t   uart0_Cloudcomand_RP;
uint8_t   uart0_Cloudcomand_WP;

uint8_t   uart0_LocalSendcomand[5][20];
uint8_t	  uart0_LocalCount;
uint8_t	  uart0_LocalComandLen;
uint8_t	  uart0_LocalComandNo;
uint8_t	  uart0_LocalComandFunction;//2022/2/4 12:28
uint8_t   uart0_LocalComandResondWaiting;
uint8_t   uart0_LocalComandResondLen;
uint8_t   uart0_LocalComandNo;
uint8_t	  uart0_LocalComandAddr;
uint8_t   uart0_Localcomand_RP;
uint8_t   uart0_Localcomand_WP;


uint8_t uart0_Bit_List[10];
uint8_t uart0_Bit_List_bak[20];
uint8_t uart0_Para_List[280];
uint8_t Bak_uart0_Bit_List[10];
uint8_t Bak_uart0_Para_List[280];

uint8_t uart0_Var_List[320];
uint8_t uart0_VolCurrentAlarm[10];

uint8_t uart0_OrigPara_List[cContorlRegLen*20];
uint8_t uart0_OrigBitList[70];




uint8_t DeviceInterval[20];
uint8_t  uart0_Para_MapTable[14]={51,50,15,16,17,17,47,47,12,12,31,31,32,32};
uint8_t  uart0_Var_MapTable[4]={0,0,1,1};
uint8_t  uart0_Bit_MapTable[7]={20,21,52,22,8,9,10};
uint8_t uart0_floatSeq[4]={3,2,1,0};

/*-----------------------------------------------------
ͨѶ��Э�����
--------------------------------------------------------*/
uint8_t uart0_GetBufferDataRead(uint8_t i);
void uart0_IncUartRead(uint8_t i);
void uart0_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart0_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart0_LoraMaster_DealWithCoil(uint8_t Device);
void uart0_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart0_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);
void  uart0_LoraMaster_DealWithCoilWreite(uint8_t Device);
void   Timer_uartInterval();
void  GetCommunicationStatus();
/*-----------------------------------------------------
��ѯָ��
--------------------------------------------------------*/
void uart0_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len);
void uart0_ModbusLoopInitla();
void  uart0_NextDevice(); 
void  uart0_ModbusLoop();

/*-----------------------------------------------------
�ƶ�ͨѶָ�����
--------------------------------------------------------*/
void UpdateChange();
void uart0_SendCloudComand();
void uart3_SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat);
void uart3_SendCoolDat();
void uart3_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len);
void uart0_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send);  
/*-----------------------------------------------------
����ͨѶָ�����
--------------------------------------------------------*/
void uart0_SendLocalComand() ;
void uart0_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send);     
void uart0_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send) ;  
void uart2_AddLocalBuffer_16(uint8_t FunCode,uint8_t *send);     
/*-----------------------------------------------------
״̬��ȡ
--------------------------------------------------------*/
uint8_t  GetBitStatus(uint8_t  bit);
void  SetStatus(uint8_t Device,uint8_t  bit);
void ClrStatus(uint8_t Device,uint8_t  bit);
uint8_t  GetConSensorStatus(uint8_t Device,uint8_t  bit);
uint8_t retrnPara(uint16_t i);
/*-----------------------------------------------------
���ϼ��
--------------------------------------------------------*/
uint8_t GetAllSensorError(uint8_t Device);
uint8_t GetAllDeviceError(uint8_t Device);	
uint8_t GetAllError(uint8_t Device);
void uart0_VolCurrentOverLoadAlarm();
/*-----------------------------------------------------
��ͣ����
--------------------------------------------------------*/
void uart0_OpenOrClose(uint8_t  no,uint8_t  Op);
/*---------------------------------------------------------------------
ʹ��˵����
1����ʼ�������е���uart0_ModbusLoopInitla()
2���������е���uart0_ModbusLoop()
���ܣ���uart������ѯ�¿����͵��һ���¿�����һ�����֧��10���¿�����10�����
�¿�������Ŵ�1-10������Ŵ�101-110
��ʼ��ź��¿��������ڼĴ����н������ã�Ĭ��Ϊ��1��ʼ����10�飬�˲�������Ҫ���е���
ʹ��ʹ�ø��¿����Ͷ�Ӧ�ĵ��ͨ�����������������
ÿ���¿�����8��״̬λ����2��32λ���¶ȱ�����������4��16λ������5��32λ����

����ѵ�¿����Ĺ����У�������ƶ˵Ŀ��������·����������һ����ѯ�����ִ���ƶ˵Ŀ�����������ƶ˵Ļظ�����
ÿ���¿���ʹ��6��16bit���������3��32λ������������ÿ�����ʹ��8��16bit�������ֱ�Ϊ3�������3���ѹ���ܹ���
ÿ����������NTC�¿���3ֻ��������ַ�����������֮��
----------------------------------------------------------------------*/

//---------------------------------------------------------------
//Function:  uint8_t uart0_GetBufferDataRead(uint8_t i)
//Input:    None
//Output:   None
//Description: ��ȡbuffer����
//----------------------------------------------------------------
uint8_t uart0_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart0[(ReadPiont_uart0+i)%cBufferLen_uart0]);
 
}
//---------------------------------------------------------------
//Function:  void uart0_IncUartRead(uint8_t i)
//Input:    None
//Output:   None
//Description: �ƶ�buffer����
//----------------------------------------------------------------	
void uart0_IncUartRead(uint8_t i)	
{
	ReadPiont_uart0=(ReadPiont_uart0+i)%cBufferLen_uart0;
}			


void   Timer_uartInterval()
{
	uint8_t  i;
	for(i=0;i<10;i++)
	{
       if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)==1)  //active
	       DeviceInterval[i]++;
			 else
				 DeviceInterval[i]=0; 
			 
			 if((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x02)==2)  //active
				 DeviceInterval[i+10]++;
			 else
				 DeviceInterval[i+10]=0;
} 
}

//---------------------------------------------------------------
//Function:  void uart0_Master_DealWith_ModbusRespond(uint8_t Device)
//Input:    None
//Output:   None
//Description: ����uart0�Ļظ�����
//---------------------------------------------------------------- 
void uart0_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,Error1,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                Error=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
//-----------------------------------------------------------------                                     
                              
                              if((uart0_CloudComandResondWaiting==1)||(uart0_LocalComandResondWaiting==1))
                                 {   if(uart0_CloudComandFunction==0x01)
                                       uart0_CloudComandLen=6;
                                    else
                                       uart0_CloudComandLen=7;    
                                     
                                     if(Error>=uart0_CloudComandLen)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==uart0_CloudComandNo)&&(k==uart0_CloudComandFunction))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         
			                                        //-------------------���� 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)|| (k==0x05)) //10������Ҫ�ȴ�
			                     	                        {  
			                     	                   	   if(Error1>=8)//buffer �㹻
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart0,6);
               	                                                             CRCAct=SendTempBufferBak_uart0[6]*256+SendTempBufferBak_uart0[7]; 
			                     	                             //----------��ȷ��crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart0_IncUartRead(8);
			                     	                                      uart0_CloudComandResondLen=8;
			                     	                                      uart0_LoraMaster_CheckModbusMaster(uart0_CloudComandNo);
			                     	                                   }
			                     	                              //---------����crc,��һ��byte     
			                     	                            else
			                     	                              uart0_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer����
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //��������03 04 
			                     	                  { 
			                     	                     //   �����㹻	
			                     	                  	if(Error>=uart0_GetBufferDataRead(2)+5)//buffer �㹻 01 03 20  crc
			                     	                        { for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {           uart0_CloudComandResondLen=uart0_GetBufferDataRead(2)+5;
			                     	                                             uart0_IncUartRead(uart0_CloudComandResondLen);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(uart0_CloudComandNo);
			                     	                         	
			                     	                                   }
			                     	                                          else //crc wrong
			                     	                                              uart0_IncUartRead(1); 
			                     	                                          return; 
									                                   }
								                                    else  // 04 03 buffer ����
								                                        return;
								                                     }
						   //--END  03 04-------  
					 	    } //end ��������enough ����
							else  //ʣ���buffer�����γ�һ���ĳ�֡,ֱ���˳�
			                     	     	   uart0_IncUartRead(1); 
			                     	 }  // end ��ȷ����
						
			                if(i==Error-1)
                                           uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte
			               }   
									 }
            										
			                         
			                         
//--------------------------------------------�¿���λ����---------------------				    
				  else if(uart0_loop_stage==0)    
				   { if(Error>=6)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x01))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                       if(i==Error-1)
                                                   uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte
			                  } 
			               }      
//--------------------------------------�¿������Ʊ���---------------------------------------------------------
 else if(uart0_loop_stage==1) 
                           { if(Error>=105)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x03))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                 
			                if(i==Error-1)
                                            uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte 
			               }
			             }          
//------------------------------------------------���-------------------------------------------------
 else if(uart0_loop_stage==2) 
                           { if(Error>=21)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte  
			               }
			         }  

//----------------------------------------�������豸-----------------------------------------------------
 else if(uart0_loop_stage==3) 
                           { if(Error>=15)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+199)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+199);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte  
			               }
			         }       							 
//------------------------------------------��ʪ��sensor------------------------------------------------------
           
  else if((uart0_loop_stage>3)&& (uart0_loop_stage<=11))
                           { if(Error>=9)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==11+uart0_loop_stage-4)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(11+uart0_loop_stage-4);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte  
			               }
			         }   

//-----------------------------------------����������ȱ��------------------------------------------------------							 
 else if(uart0_loop_stage==12) 
                           { if(Error>=7)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart0_GetBufferDataRead(i);
			                    k=uart0_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart0_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart0+cBufferLen_uart0-ReadPiont_uart0)%cBufferLen_uart0;
			                         if(Error1>=uart0_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart0_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart0[k]=uart0_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart0,uart0_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart0[uart0_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart0_IncUartRead(uart0_GetBufferDataRead(2)+5);
			                     	                                             uart0_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart0_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart0_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart0_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte  
			               }
			         }  
//-----------------------------------------------------------------------
							 
}

//--------------------------------------------------------
//Function:  uchar CheckModbusRespond()
//Input:    None
//Output:   None
//Description: �ȴ�modbus����Э�飬����д��Ԫ�Ͷ���Ԫ
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          ��ַ	������	��ַ��	��ַ��	������ ������	CRC�ĸ�λ	CRC�ĵ�λ
//----------------------------------------------------------
unsigned char uart0_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
	 if((SendTempBufferBak_uart0[0]>100)&&(SendTempBufferBak_uart0[0]<111))
	    DeviceInterval[SendTempBufferBak_uart0[0]-91]=0;
	else  if(SendTempBufferBak_uart0[0]<11)
		  DeviceInterval[SendTempBufferBak_uart0[0]-1]=0; 
    
           if(SendTempBufferBak_uart0[1]==cComandWriteSingleCoil)           //������coil
               	 	   { 
               	 	     uart0_LoraMaster_DealWithCoilWreite(Device);
               	 	     return(cRightStatus);
               	 	    }
                  if(SendTempBufferBak_uart0[1]==cComandReadCoil)           //������coil
               	 	   { 
               	 	     uart0_LoraMaster_DealWithCoil(Device);
               	 	     return(cRightStatus);
               	 	    }
             
//singleWrite  06-----------------------------------------------------                	 	
                else  if(SendTempBufferBak_uart0[1]==cComandWriteSingleUint)           //д�����Ĵ���06
               	 	   { 
               	 	     uart0_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart0[1]==cComandReadHoldRegister)    //�����ּĴ���03 
               	 	   {   if(uart0_CloudComandResondWaiting==1)
		                        {
    	                          uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	                          uart0_CloudComandResondWaiting=0;
    	                          uart0_CloudCount=0;
    	                          uart0_Cloudcomand_RP++;
    	                          if(uart0_Cloudcomand_RP>=5)
    	                                 uart0_Cloudcomand_RP=0;
																 uart0_CloudComandResondWaiting=0;
                                return(cRightStatus);			 
    	                          }  
    	                         
			                   else 
												 { //20231201
			                 if( uart0_LocalComandResondWaiting==1)
			                        uart0_LocalComandResondWaiting=0;
               	 	   	      uart0_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
										 } //20231201
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart0[1]==cComandReadDiRegister)      //�����ݼĴ���04 
               	 	   { uart0_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart0[1]==0x10)     
               	 	   { uart0_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	  
               	 	    	
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart0[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1_uart0(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    


//------------------
 
void   uart0_LoraMaster_DealWithCoilWreite(uint8_t Device)
{
   if(uart0_CloudComandResondWaiting==1)
    {  
		 uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0; 
     UptateFlag=1;			 
    	
		 }

}	
/* ---------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����01д����
//---------------------------------------------------------------------01----------------------------------------------------------------------------------*/
void uart0_LoraMaster_DealWithCoil(uint8_t Device)
{
  uint8_t  i=0;
	
	if((SendTempBufferBak_uart0[3+0/8]&(1<<(0%8)))!=0) //������״̬
	{ SetStatus(Device,0);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,0);
	        
	
            }
	
	if((SendTempBufferBak_uart0[3+1/8]&(1<<(1%8)))!=0) //��˪������״̬
	{ SetStatus(Device,1);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,1);
	        
	
}
	
	
  if((SendTempBufferBak_uart0[3+20/8]&(1<<(20%8)))!=0) //����״̬
	{ SetStatus(Device,5);
	    i|=1;  
	}	
	else
		 ClrStatus(Device,5);
		  
	
   if((SendTempBufferBak_uart0[3+21/8]&(1<<(21%8)))!=0) //���״̬
	 { i|=2; 
		 SetStatus(Device,6);
	 }
	else
    ClrStatus(Device,6);		
	 
  if((SendTempBufferBak_uart0[3+22/8]&(1<<(22%8)))!=0) //��˪״̬
	{ i|=4;
		SetStatus(Device,7);
	}
 else
    ClrStatus(Device,7);


 if((SendTempBufferBak_uart0[3+8/8]&(1<<(8%8)))!=0) //���¸澯
	{
		SetStatus(Device,2);
	
     i|=16;     
	}
 else
    ClrStatus(Device,2);

if((SendTempBufferBak_uart0[3+9/8]&(1<<(9%8)))!=0) //���¸澯
	{
		SetStatus(Device,3);
		i|=16;
	}
 else
    ClrStatus(Device,3);
 
if((SendTempBufferBak_uart0[3+10/8]&(1<<(10%8)))!=0) //�ⲿ�澯
	{
		SetStatus(Device,4);
		i|=16;
	}
 else
    ClrStatus(Device,4); 
 
if((SendTempBufferBak_uart0[3+50/8]&(1<<(50%8)))!=0) //ǿ�Ƴ�˪
	{
		SetStatus(Device,8);
	}
 else
    ClrStatus(Device,8);  
 
if((SendTempBufferBak_uart0[3+51/8]&(1<<(51%8)))!=0) //ϵͳ����
	{
		SetStatus(Device,9);
	}
 else
    ClrStatus(Device,9);  
  

 if((SendTempBufferBak_uart0[3+52/8]&(1<<(52%8)))!=0) //ϵͳ��˪״̬
	{
		SetStatus(Device,10);
	}
 else
    ClrStatus(Device,10);  
 
  uart0_Bit_List[Device-1]&=0xC8;     //ͨѶ״̬
   uart0_Bit_List[Device-1]|=i;	   
	

	uart0_Para_List[(Device-1)*28]=0x00;
	if((SendTempBufferBak_uart0[3+51/8]&(1<<(51%8)))!=0)
                uart0_Para_List[(Device-1)*28+1]=0x01;
	else
		 uart0_Para_List[(Device-1)*28+1]=0x00;
//add  20230328
 uart0_Para_List[(Device-1)*28+2]=0x00;
	if((SendTempBufferBak_uart0[3+50/8]&(1<<(50%8)))!=0)
                uart0_Para_List[(Device-1)*28+3]=0x01;
	else
		 uart0_Para_List[(Device-1)*28+3]=0x00;


//end 	
	
	
	for(i=0;i<7;i++)
	 	uart0_OrigBitList[(Device-1)*7+i]=SendTempBufferBak_uart0[3+i];

	
	 if(uart0_CloudComandResondWaiting==1)
    {  
		 uart3_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0;  
    	
		 }
}






/*---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����06д����
�ظ������ݣ��ƶ�ֱ�ӷ����¿���  4000����
            �ƶ�ͨ������buffer�����¿��� ���أ�0-279  �¿�����0-54 
            ���صĲ�������ֵ�л��������л��Ȳ���
�ظ�����������Ǳ��������еģ�����uint��float��ʽ�ֱ�д���Ӧ�ı�������
�ظ�����������Ǳ�����û�еģ���д������
���int��������0xff00�ģ���Ҫ����Ϊ1
���Ϊ�ƶ�д�����ݷ��أ�����Զ�������б��򷵻ؽ��յ��������ݣ�����������Ҫ���д������ϣ�
���Ϊ����д�����ݣ�������д���б�
//--------------------------------06------------------------------------------------------------------------------06-----------------------------------------------*/
void uart0_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {uint16_t  Addr,Dat;
	 uint8_t   sta;
	 union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;		 
	 
	 
	 
	 uart0_CloudCount=0;
#if  1	 
	 Addr=SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3];
	 Dat=SendTempBufferBak_uart0[4]*256+SendTempBufferBak_uart0[5];
 	 sta=SendTempBufferBak_uart0[0];
	 
	 
			for(i=0;i<14;i++)
			 { if( Addr==uart0_Para_MapTable[i])
				     break;
			 }
//---------------------- ���ص��ǲ������п�������int����			 
      if(i<4)
			{
				uart0_Para_List[(sta-1)*28+i*2]=SendTempBufferBak_uart0[4];
				uart0_Para_List[(sta-1)*28+i*2+1]=SendTempBufferBak_uart0[5];
				
				if((i==0)||(i==1))  //Openclose
				{
					if(SendTempBufferBak_uart0[4]==0xff)
					  {
					      uart0_Para_List[(sta-1)*28+i*2]=0x00;
					     uart0_Para_List[(sta-1)*28+i*2+1]=0x01;
				          }
				}
	#if  0		  
				else  if(i==1)  //Openclose
				{
					if(SendTempBufferBak_uart0[5]==1)
					  {
					      uart0_Para_List[(sta-1)*28+i*2]=0x00;
					     uart0_Para_List[(sta-1)*28+i*2+1]^=0x01;
				          }
				}
	#endif			
			}	
//--------------------------	 ���ص��ǲ������п�������float����		
		 else	if(i<14)
		 {
         tempT= SendTempBufferBak_uart0[3+uart0_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart0[4+uart0_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)Dat;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart0_Para_List[(sta-1)*28+(i-4)*4+8]=temp.vchar[uart0_floatSeq[0]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+9]=temp.vchar[uart0_floatSeq[1]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+10]=temp.vchar[uart0_floatSeq[2]];
    	uart0_Para_List[(sta-1)*28+(i-4)*4+11]=temp.vchar[uart0_floatSeq[3]];
		 }

//-----------------------------------------���ص��ǲ�������û�еĲ������ƶ˵��������ݣ������ص����ݷ��ظ��ƶˣ�			 
    if(uart0_CloudComandResondWaiting==1)
    {  
	//	 uart0_ReturnCloudData(SendTempBufferBak_uart0,uart0_CloudComandResondLen);
    	uart0_CloudComandResondWaiting=0;
    	uart0_CloudCount=0;
      uart0_Cloudcomand_RP++;
    	 if(uart0_Cloudcomand_RP>=5)
    	    uart0_Cloudcomand_RP=0;
    	 uart0_CloudComandResondWaiting=0;  
			 UptateFlag=1;	
    	
		 }
		
//----------------------------���ظ��ӵ�������
   if(uart0_LocalComandResondWaiting==1)
               	 	    {  uart0_LocalComandResondWaiting=0;
    	                       if(uart0_Localcomand_RP!=uart0_Localcomand_WP)
    	                      { uart0_Localcomand_RP++;
    	                        if(uart0_Localcomand_RP>=5)
    	                            uart0_Localcomand_RP=0;
    	                            uart0_LocalComandResondWaiting=0;  
    	                       }   
    	                       
    	                    }	
//----------------------------------------------ֻҪ��д��ɹ���״̬������Ҫ����һ�ο��ƵĲ���ˢ��--------
    	  for(i=0;i<8;i++)
       uart0_LocalSendcomand[uart0_Localcomand_WP][1+i]=SendTempBufferBak_uart0[i];
       if((SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3]==50)||(SendTempBufferBak_uart0[2]*256+SendTempBufferBak_uart0[3]==51))   
          {  uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x01; 
            uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x34;
          }   
        else
          {  uart0_LocalSendcomand[uart0_Localcomand_WP][2]=0x03;
              uart0_LocalSendcomand[uart0_Localcomand_WP][6]=0x30;  
          }
           uart0_LocalSendcomand[uart0_Localcomand_WP][3]=0; 
           uart0_LocalSendcomand[uart0_Localcomand_WP][4]=0; 
            uart0_LocalSendcomand[uart0_Localcomand_WP][5]=0;
					 uart0_LocalSendcomand[uart0_Localcomand_WP][1]=SendTempBufferBak_uart0[0];
					
            uart0_LocalSendcomand[uart0_Localcomand_WP][0] =8;
       tempT=CRC(&uart0_LocalSendcomand[uart0_Localcomand_WP][1],6);
  	uart0_LocalSendcomand[uart0_Localcomand_WP][7]=tempT/256;  //High crc
  	uart0_LocalSendcomand[uart0_Localcomand_WP][8]=tempT%256;  //Low crc
         uart0_Localcomand_WP++;
    	 if(uart0_Localcomand_WP>=5)
    	    uart0_Localcomand_WP=0;
    	  if(uart0_Localcomand_WP==uart0_Localcomand_RP)   
    	  {  uart0_Localcomand_RP++;
    	     if(uart0_Localcomand_RP>=5)
    	       uart0_Localcomand_RP=0;
        }
  
#endif	 
 
 }
 
uint8_t  Random10=0;
//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:�˿ں�0-4
            u8 Device:��ѯ���ն��豸��
//Output:   None
//Description:�����ն��豸�ظ�����Ϣ
//-----------------------------------����03��ȡ����������--------------------------------------------------03------------------------------------------------------*/
void uart0_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { union
     {float  vf;
      uint8_t vchar[4];
     }temp;
   
    uint16_t  tempT;	
    int16_t   tempInt,Tempint;
		 
		 
		  uint8_t  i,j; 

	  uint32_t  Tempint1;
 float  tempf; 
	 uint16_t  Tempuint,TempCrc;	


 if(Device>=101)
 {
	 
	 //---------------------����--------------------------
	 j=ParaList[cMetreType*2+1];
	 if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     { 
       //----------------------
			 if(j==0)  //����ֱ��ʽ
			 { 
	//20231114			 
		if(SendTempBufferBak_uart0[2]==0x08) 	
		{	 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[3+i];
  	      Tempint=(int16_t)(temp.vf*10);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempuint%256; //a���ѹ
				 
			    for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint=(int16_t)(temp.vf*100);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempuint%256; //a���ѹ	 
				 
	#if  0		 
			 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=SendTempBufferBak_uart0[9];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=SendTempBufferBak_uart0[10]; //a�����
			  
				 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //����
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //����	
#endif
        }
		else  if(SendTempBufferBak_uart0[2]==0x04) 	
   {
				 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint1=(uint32_t)(temp.vf*100);
  	      //Tempuint = (uint16_t)Tempint;
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=Tempint1/65535/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=Tempint1/65535%256;
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=(Tempint1%65535)/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=(Tempint1%65535)%256;
				 
				 
       }
	 //20231114	
		 }
	 }
 }	 
else
{	
    for(i=0;i<SendTempBufferBak_uart0[2];i++)
		 {
			  uart0_OrigPara_List[(Device-1)*cContorlRegLen*2+i]= SendTempBufferBak_uart0[3+i];
     }
		 
    tempT= SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4];
		Random10=0;
		 if(Random10>100)
			 Random10=0; 
		 tempT+=Random10; //20250212
		 
		 tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
		temp.vf/=10; 
    for(i=0;i<4;i++) //�ط��¶�
      uart0_Var_List[(Device-1)*12+i]=temp.vchar[uart0_floatSeq[i]];
   
		 tempT= SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6];  
     tempT+=Random10; //20250212
		 tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    temp.vf/=10; 
		 for(i=0;i<4;i++)  //��˪�¶� 
      uart0_Var_List[(Device-1)*12+8+i]=temp.vchar[uart0_floatSeq[i]];   
    
              //4��uint16 ����,ϵͳ���ƺͻ�˪�����Ѿ���λ�����н��д����ˣ�������������������Ĵ���
   for(i=2;i<4;i++) 
    { uart0_Para_List[(Device-1)*28+i*2]=SendTempBufferBak_uart0[3+uart0_Para_MapTable[i]*2];
      uart0_Para_List[(Device-1)*28+i*2+1]=SendTempBufferBak_uart0[3+uart0_Para_MapTable[i]*2+1];
		#if  0
			if(i==1)
			{
				uart0_Para_List[(Device-1)*28+i*2+1]^=1;  //20220216
			}
		#endif	
   }
             
  for(i=4;i<9;i++)   //5��uint32 ����
    { 
			tempT= SendTempBufferBak_uart0[3+uart0_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart0[4+uart0_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart0_Para_List[(Device-1)*28+(i-4)*4+8]=temp.vchar[uart0_floatSeq[0]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+9]=temp.vchar[uart0_floatSeq[1]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+10]=temp.vchar[uart0_floatSeq[2]];
    	uart0_Para_List[(Device-1)*28+(i-4)*4+11]=temp.vchar[uart0_floatSeq[3]];
    }
 }  
}
/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����04������
//----------------------------- ����04��ȡ������ֵ���---------------------------------------------------------------------------------------------04---------*/

void uart0_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i,j; 
 uint16_t  Tempint;
	  uint32_t  Tempint1;
 float  tempf; 
	 uint16_t  Tempuint,TempCrc;	
 union
     {float  vf;
      uint8_t vchar[4];
     }temp;   
          
  	    
     if(Device>=200)//������
		 {  i=3;
			  uart0_Var_List[(Device-200)*4+cTempPos*2+0+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+1+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //temp
	      i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+2+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;//256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+3+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //humi
			 i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+4+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+5+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //o2
	      i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+6+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+7+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //ch3oh
			  i++;
			 i++;
			 uart0_Var_List[(Device-200)*4+cTempPos*2+8+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])/256;
        uart0_Var_List[(Device-200)*4+cTempPos*2+9+20]=(SendTempBufferBak_uart0[i]*256+SendTempBufferBak_uart0[i+1])%256; //co2
	     
		 }
	 
	else if(Device>=101)
 {
	 
	 //---------------------����--------------------------
	 j=ParaList[cMetreType*2+1];
	 if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     { 
  #if  0    
			 //----------------------
			 if(j==0)  //����ֱ��ʽ
			 { 
	//20231114			 
		if(SendTempBufferBak_uart0[2]==0x08) 	
		{	 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[3+i];
  	      Tempint=(int16_t)(temp.vf*10);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempuint%256; //a���ѹ
				 
			    for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint=(int16_t)(temp.vf*100);
  	      Tempuint = (uint16_t)Tempint;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempuint/256;
          uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempuint%256; //a���ѹ	 
				 
	#if  0		 
			 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=SendTempBufferBak_uart0[9];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=SendTempBufferBak_uart0[10]; //a�����
			  
				 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //����
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //����	
#endif
        }
		else  if(SendTempBufferBak_uart0[2]==0x04) 	
   {
				 for(i=0;i<4;i++)
  	      temp.vchar[i]=SendTempBufferBak_uart0[7+i];
  	      Tempint1=(uint32_t)(temp.vf*100);
  	      //Tempuint = (uint16_t)Tempint;
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=Tempint1/65535/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=Tempint1/65535%256;
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=(Tempint1%65535)/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=(Tempint1%65535)%256;
				 
				 
       }
	 //20231114	
		 }
			 //-----------------------
      else   //���໥��ʽ
	#endif			
			
			{if(SendTempBufferBak_uart0[2]==0x3e)  //���س���Ϊ62byte����ȡ������ѹ��
        { uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=SendTempBufferBak_uart0[3];
         uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=SendTempBufferBak_uart0[4]; //a���ѹ
			   
					Tempint1=SendTempBufferBak_uart0[9]*256+SendTempBufferBak_uart0[10];
			 //  Tempint1= Tempint1/10;
				uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint1/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint1%256; //a����� 
					
					 uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //����
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //����
       }				
			
			
     }
		//--------------------------	
	 }
		 
		 //---------------------����-------------------------	 
   else if((ParaList[cMotorType*2+1]==0x03)||(ParaList[cMotorType*2+1]==0x04))
     { Tempint=0;
     //-----------ֱ��ʽ����
			 //---------------------��ѹ--------    
	if(j==0)	
	{	
if(SendTempBufferBak_uart0[2]==0x3e) 
{for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[3+i*2]*256+SendTempBufferBak_uart0[3+i*2+1];
     Tempint=Tempint/3;
    	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempint%256; //��ѹ
        
 //-----------------------------------����---------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[9+i*2]*256+SendTempBufferBak_uart0[9+i*2+1];
     	Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint%256; //����
//add  20240228  ��ȡ�����״̬
        
         uart0_Var_List[cPhaseDir*2+1]=SendTempBufferBak_uart0[17];					
		      uart0_Var_List[cPhase*2]=SendTempBufferBak_uart0[17];	
		//----------------------------------�ܹ���---------------------    
        
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[61];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[62]; //����
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[63];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[64]; //����
	}
else if(SendTempBufferBak_uart0[2]==0x02) 
{   uart0_Var_List[cPhaseLost*2+1]=SendTempBufferBak_uart0[4];	
     uart0_Var_List[cPhase*2+1]=SendTempBufferBak_uart0[4];
}	
}
else   //------------����ʽ����
{
   Tempint=0;
 //---------------------��ѹ--------    
	if(SendTempBufferBak_uart0[2]==0x3e)  //���س���Ϊ62byte����ȡ������ѹ��
	{for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart0[3+i*4+2]*256+SendTempBufferBak_uart0[3+i*4+3];
     Tempint/=3;
    	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+1]=Tempint%256; //��ѹ
        
 //-----------------------------------����---------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	 {
           Tempint1=(SendTempBufferBak_uart0[15+i*4]*256+SendTempBufferBak_uart0[16+i*4])*65536+SendTempBufferBak_uart0[17+i*4]*256+SendTempBufferBak_uart0[18+i*4];
			   Tempint1=Tempint1/10;
				 
				 
				 Tempint+=(uint16_t)Tempint1;
     	
				 
			 } 
				 Tempint=Tempint/3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+2]=Tempint/256;
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+3]=Tempint%256; //����

}	
else  if(SendTempBufferBak_uart0[2]==0x04)//���س���Ϊ4byte����ȡ�繦�ʣ�
   {
	    //----------------------------------�ܹ���---------------------    
        
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+4]=SendTempBufferBak_uart0[3];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+5]=SendTempBufferBak_uart0[4]; //����
			  uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+6]=SendTempBufferBak_uart0[5];
        uart0_Var_List[(Device-101)*cVarLenByte+cPowerMetre*2+7]=SendTempBufferBak_uart0[6]; //����
    }
  }
      
 }
}		 
//---------��ʪ��̽ͷ����-------
else
{
	#if  0
	      uart0_Var_List[(Device-11)*4+cTempPos*2]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+1]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])%256; //temp
	      uart0_Var_List[(Device-11)*4+cTempPos*2+2]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+3]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])%256; //humi
	#endif
	      uart0_Var_List[(Device-11)*4+cTempPos*2+2]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+3]=(SendTempBufferBak_uart0[3]*256+SendTempBufferBak_uart0[4])%256; //temp
	      uart0_Var_List[(Device-11)*4+cTempPos*2+0]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])/256;
        uart0_Var_List[(Device-11)*4+cTempPos*2+1]=(SendTempBufferBak_uart0[5]*256+SendTempBufferBak_uart0[6])%256; //humi
	      RequireTime[Device-11]=0;
}
//---------------------------------

}



//----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����0x10д����
//-----------------------------------10-----------------------------
void uart0_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
 
  
  
  
 }
