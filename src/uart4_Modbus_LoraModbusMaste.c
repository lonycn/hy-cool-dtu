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

#define       cBufferLen_uart4     500
extern uint16_t    WritePiont_uart4;
extern uint16_t    ReadPiont_uart4;
extern unsigned char   ReceiveBuff_uart4[cBufferLen_uart4];
extern unsigned char   SendTempBufferBak_uart4[300];
uint8_t   uart4_LoopDeviceNo;
uint8_t   uart4_loop_stage=0;

uint8_t	  uart4_CloudCount;
uint8_t	  uart4_CloudComandLen; //�м����  ����������̵İ���
uint8_t	  uart4_CloudComandNo;
uint8_t	  uart4_CloudComandFunction;
uint8_t   uart4_CloudComandResondWaiting;
uint8_t   uart4_CloudComandResondLen;
uint8_t   uart4_Sendcomand[5][20];
uint8_t   uart4_Cloudcomand_RP;
uint8_t   uart4_Cloudcomand_WP;

uint8_t   uart4_LocalSendcomand[5][20];
uint8_t	  uart4_LocalCount;
uint8_t	  uart4_LocalComandLen;
uint8_t	  uart4_LocalComandNo;
uint8_t	  uart4_LocalComandFunction;//2022/2/4 12:28
uint8_t   uart4_LocalComandResondWaiting;
uint8_t   uart4_LocalComandResondLen;
uint8_t   uart4_LocalComandNo;
uint8_t	  uart4_LocalComandAddr;
uint8_t   uart4_Localcomand_RP;
uint8_t   uart4_Localcomand_WP;


uint8_t uart4_Bit_List[10];
uint8_t uart4_Bit_List_bak[20];
uint8_t uart4_Para_List[280];
uint8_t Bak_uart4_Bit_List[10];
uint8_t Bak_uart4_Para_List[280];

uint8_t uart4_Var_List[320];
uint8_t uart4_VolCurrentAlarm[10];

uint8_t uart4_OrigPara_List[cContorlRegLen*20];
uint8_t uart4_OrigBitList[70];




uint8_t DeviceInterval[20];
uint8_t  uart4_Para_MapTable[14]={51,22,15,16,17,17,47,47,12,12,31,31,32,32};
uint8_t  uart4_Var_MapTable[4]={0,0,1,1};
uint8_t  uart4_Bit_MapTable[7]={20,21,52,22,8,9,10};
uint8_t uart4_floatSeq[4]={3,2,1,0};

/*-----------------------------------------------------
ͨѶ��Э�����
--------------------------------------------------------*/
uint8_t uart4_GetBufferDataRead(uint8_t i);
void uart4_IncUartRead(uint8_t i);
void uart4_Master_DealWith_ModbusRespond(uint8_t Device);
unsigned char uart4_LoraMaster_CheckModbusMaster(uint8_t Device);
void uart4_LoraMaster_DealWithCoil(uint8_t Device);
void uart4_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device);
void uart4_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device);
void  uart4_LoraMaster_DealWithCoilWreite(uint8_t Device);
void   Timer_uartInterval();
void  GetCommunicationStatus();
/*-----------------------------------------------------
��ѯָ��
--------------------------------------------------------*/
void uart4_ModbusRequest(uint8_t Sta,uint8_t comand,uint16_t Addr,uint8_t Len);
void uart4_ModbusLoopInitla();
void  uart4_NextDevice(); 
void  uart4_ModbusLoop();

/*-----------------------------------------------------
�ƶ�ͨѶָ�����
--------------------------------------------------------*/
void UpdateChange();
void uart4_SendCloudComand();
void uart4_SendControlData(uint8_t Addr,uint8_t FunctionCode,uint16_t StartAddr,uint8_t Len,uint8_t *SendDat);
void uart4_SendCoolDat();
void uart4_ReturnCloudData(uint8_t *ReturnBuff,uint8_t Len);
void uart4_AddCloudBuffer(uint16_t  i,uint8_t FunCode,uint8_t *send);  
/*-----------------------------------------------------
����ͨѶָ�����
--------------------------------------------------------*/
void uart4_SendLocalComand() ;
void uart4_AddLocalBuffer_16(uint16_t  Temp,uint8_t FunCode,uint8_t *send);     
void uart4_AddLocalBuffer_32(uint16_t  Temp,uint8_t FunCode,uint8_t *send) ;  
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
void uart4_VolCurrentOverLoadAlarm();
/*-----------------------------------------------------
��ͣ����
--------------------------------------------------------*/
void uart4_OpenOrClose(uint8_t  no,uint8_t  Op);
/*---------------------------------------------------------------------
ʹ��˵����
1����ʼ�������е���uart4_ModbusLoopInitla()
2���������е���uart4_ModbusLoop()
���ܣ���uart������ѯ�¿����͵��һ���¿�����һ�����֧��10���¿�����10�����
�¿�������Ŵ�1-10������Ŵ�101-110
��ʼ��ź��¿��������ڼĴ����н������ã�Ĭ��Ϊ��1��ʼ����10�飬�˲�������Ҫ���е���
ʹ��ʹ�ø��¿����Ͷ�Ӧ�ĵ��ͨ�����������������
ÿ���¿�����8��״̬λ����2��32λ���¶ȱ�����������4��16λ������5��32λ����

����ѵ�¿����Ĺ����У�������ƶ˵Ŀ��������·����������һ����ѯ�����ִ���ƶ˵Ŀ�����������ƶ˵Ļظ�����
ÿ���¿���ʹ��6��16bit���������3��32λ������������ÿ�����ʹ��7��16bit�������ֱ�Ϊ3�������3���ѹ���ܹ���
ÿ����������NTC�¿���2ֻ��������ַ�����������֮��
----------------------------------------------------------------------*/

//---------------------------------------------------------------
//Function:  uint8_t uart4_GetBufferDataRead(uint8_t i)
//Input:    None
//Output:   None
//Description: ��ȡbuffer����
//----------------------------------------------------------------
uint8_t uart4_GetBufferDataRead(uint8_t i)
 {
  return(ReceiveBuff_uart4[(ReadPiont_uart4+i)%cBufferLen_uart4]);
 
}
//---------------------------------------------------------------
//Function:  void uart4_IncUartRead(uint8_t i)
//Input:    None
//Output:   None
//Description: �ƶ�buffer����
//----------------------------------------------------------------	
void uart4_IncUartRead(uint8_t i)	
{
	ReadPiont_uart4=(ReadPiont_uart4+i)%cBufferLen_uart4;
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
//Function:  void uart4_Master_DealWith_ModbusRespond(uint8_t Device)
//Input:    None
//Output:   None
//Description: ����uart4�Ļظ�����
//---------------------------------------------------------------- 
void uart4_Master_DealWith_ModbusRespond(uint8_t Device)
     {    unsigned int i,j,Error,Error1,k;
      unsigned int  CRCTemp ;
      unsigned int  CRCAct;
      
            
                              
                                     
                                Error=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
//-----------------------------------------------------------------                                     
                              
                              if((uart4_CloudComandResondWaiting==1)||(uart4_LocalComandResondWaiting==1))
                                 {   if(uart4_CloudComandFunction==0x01)
                                       uart4_CloudComandLen=6;
                                    else
                                       uart4_CloudComandLen=7;    
                                     
                                     if(Error>=uart4_CloudComandLen)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==uart4_CloudComandNo)&&(k==uart4_CloudComandFunction))
			                      {  uart4_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         
			                                        //-------------------���� 10 06-----	
			                                        	if((k==0x10)|| (k==0x06)|| (k==0x05)) //10������Ҫ�ȴ�
			                     	                        {  
			                     	                   	   if(Error1>=8)//buffer �㹻
			                     	                           { for(k=0;k<8;k++)
			                     	                              SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                             CRCTemp=CRC(SendTempBufferBak_uart4,6);
               	                                                             CRCAct=SendTempBufferBak_uart4[6]*256+SendTempBufferBak_uart4[7]; 
			                     	                             //----------��ȷ��crc
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                      uart4_IncUartRead(8);
			                     	                                      uart4_CloudComandResondLen=8;
			                     	                                      uart4_LoraMaster_CheckModbusMaster(uart4_CloudComandNo);
			                     	                                   }
			                     	                              //---------����crc,��һ��byte     
			                     	                            else
			                     	                              uart4_IncUartRead(1);    
			                     	                           
			                     	                           return;	
			                     	                           }	
			                     	             	          else   //06 buffer����
			                     	             	             return;
			                     	             	         }
			                     	             	 //-----------------03/04 
			                     	                else //��������03 04 
			                     	                  { 
			                     	                     //   �����㹻	
			                     	                  	if(Error>=uart4_GetBufferDataRead(2)+5)//buffer �㹻 01 03 20  crc
			                     	                        { for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {           uart4_CloudComandResondLen=uart4_GetBufferDataRead(2)+5;
			                     	                                             uart4_IncUartRead(uart4_CloudComandResondLen);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(uart4_CloudComandNo);
			                     	                         	
			                     	                                   }
			                     	                                          else //crc wrong
			                     	                                              uart4_IncUartRead(1); 
			                     	                                          return; 
									                                   }
								                                    else  // 04 03 buffer ����
								                                        return;
								                                     }
						   //--END  03 04-------  
					 	    } //end ��������enough ����
							else  //ʣ���buffer�����γ�һ���ĳ�֡,ֱ���˳�
			                     	     	   uart4_IncUartRead(1); 
			                     	 }  // end ��ȷ����
						
			                if(i==Error-1)
                                           uart4_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte
			               }   
									 }
            										
			                         
			                         
//-----------------------------------------------------------------				    
				  else if(uart4_loop_stage==0)    
				   { if(Error>=6)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x01))
			                      {  uart4_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                       if(i==Error-1)
                                                   uart4_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte
			                  } 
			               }      
//-----------------------------------------------------------------------------------------------
                        else if(uart4_loop_stage==1) 
                           { if(Error>=105)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device)&&(k==0x03))
			                      {  uart4_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                 
			                if(i==Error-1)
                                            uart4_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte 
			               }
			             }          
//-----------------------------------------------------------------------------------------------
                        else if(uart4_loop_stage==2) 
                           { if(Error>=21)   //������㹻��buffer����ȥ����
				       { for(i=0;i<Error-1;i++)
				       
			                 {  j=uart4_GetBufferDataRead(i);
			                    k=uart4_GetBufferDataRead(i+1);
			                    
			                    if((j==Device+100)&&(k==0x04))
			                      {  uart4_IncUartRead(i); //��λҪ����buffer���׵�ַ	  
			                         Error1=(WritePiont_uart4+cBufferLen_uart4-ReadPiont_uart4)%cBufferLen_uart4;
			                         if(Error1>=uart4_GetBufferDataRead(2)+5)
			                                                 {   for(k=0;k<uart4_GetBufferDataRead(2)+5;k++)
			                     	                             SendTempBufferBak_uart4[k]=uart4_GetBufferDataRead(k);	
			                     	                     
               	                                                              CRCTemp=CRC(SendTempBufferBak_uart4,uart4_GetBufferDataRead(2)+3);
               	                                                              CRCAct=SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+3]*256+SendTempBufferBak_uart4[uart4_GetBufferDataRead(2)+4];
               	                                                              if((CRCTemp==CRCAct)||(CRCAct==0x1234))
               	                                                                 {
			                     	                                             uart4_IncUartRead(uart4_GetBufferDataRead(2)+5);
			                     	                                             uart4_LoraMaster_CheckModbusMaster(Device+100);
			                     	                         	
			                     	                                   }
			                     	                                 else //crc wrong
			                     	                                     uart4_IncUartRead(1); 
			                     	                                   return; 
									 }
								       else  // 04 03 buffer ����
								          return;
						}
					     else
					         uart4_IncUartRead(1); 
			                      }
			                  
			                  if(i==Error-1)
                                              uart4_IncUartRead(Error-1); //���е����ݶ�����������ͷbyte  
			               }
			         }       
//------------------------------------------------------------------------------------------------
                
}

//--------------------------------------------------------
//Function:  uchar CheckModbusRespond()
//Input:    None
//Output:   None
//Description: �ȴ�modbus����Э�飬����д��Ԫ�Ͷ���Ԫ
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          ��ַ	������	��ַ��	��ַ��	������ ������	CRC�ĸ�λ	CRC�ĵ�λ
//----------------------------------------------------------
unsigned char uart4_LoraMaster_CheckModbusMaster(uint8_t Device)
 {
 
  unsigned char  SendTempBuff[10] ;	
  unsigned int  CRCTemp ;
  unsigned int  CRCAct;
 // WatchDog_feed();
	 if(SendTempBufferBak_uart4[0]>100)
	    DeviceInterval[SendTempBufferBak_uart4[0]-91]=0;
	 else
		  DeviceInterval[SendTempBufferBak_uart4[0]-1]=0; 
    
           if(SendTempBufferBak_uart4[1]==cComandWriteSingleCoil)           //������coil
               	 	   { 
               	 	     uart4_LoraMaster_DealWithCoilWreite(Device);
               	 	     return(cRightStatus);
               	 	    }
                  if(SendTempBufferBak_uart4[1]==cComandReadCoil)           //������coil
               	 	   { 
               	 	     uart4_LoraMaster_DealWithCoil(Device);
               	 	     return(cRightStatus);
               	 	    }
             
//singleWrite  06-----------------------------------------------------                	 	
                else  if(SendTempBufferBak_uart4[1]==cComandWriteSingleUint)           //д�����Ĵ���06
               	 	   { 
               	 	     uart4_LoraMaster_DealWithSingleRegWriteMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(SendTempBufferBak_uart4[1]==cComandReadHoldRegister)    //�����ּĴ���03 
               	 	   {   if(uart4_CloudComandResondWaiting==1)
		                        {
    	                          uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	                          uart4_CloudComandResondWaiting=0;
    	                          uart4_CloudCount=0;
    	                          uart4_Cloudcomand_RP++;
    	                          if(uart4_Cloudcomand_RP>=5)
    	                                 uart4_Cloudcomand_RP=0;
																 uart4_CloudComandResondWaiting=0;
                                return(cRightStatus);			 
    	                          }  
    	                         
			                   else 
			                 if( uart4_LocalComandResondWaiting==1)
			                        uart4_LocalComandResondWaiting=0;
               	 	   	      uart4_LoraMaster_DealWithMoreRegReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart4[1]==cComandReadDiRegister)      //�����ݼĴ���04 
               	 	   { uart4_LoraMaster_DealWithMoreDiReadMaster(Device);
               	 	     return(cRightStatus);
               	 	    }	 
               	 	    
//Read  Variable  10----------------------------------------------------                 	 	    
               	  else if(SendTempBufferBak_uart4[1]==0x10)     
               	 	   { uart4_LoraMaster_DealWithMoreRegWriteMaster(Device);
               	 	  
               	 	    	
               	 	     return(cRightStatus);
               	 	    }	                           	 	       
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 SendTempBuff[0]=SendTempBufferBak_uart4[1]+0x80; 
  	                  SendTempBuff[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1(SendTempBuff,2);
                    	 return(cCrcWrong);
                    }
                }
    


//------------------
 
void   uart4_LoraMaster_DealWithCoilWreite(uint8_t Device)
{
   if(uart4_CloudComandResondWaiting==1)
    {  
		 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0; 
     UptateFlag=1;			 
    	
		 }

}	
/* ---------------------------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����01д����
//---------------------------------------------------------------------01----------------------------------------------------------------------------------*/
void uart4_LoraMaster_DealWithCoil(uint8_t Device)
{
  uint8_t  i=0;
	
	if((SendTempBufferBak_uart4[3+0/8]&(1<<(0%8)))!=0) //������״̬
	{ SetStatus(Device,0);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,0);
	        
	
            }
	
	if((SendTempBufferBak_uart4[3+1/8]&(1<<(1%8)))!=0) //��˪������״̬
	{ SetStatus(Device,1);
	    i|=0x20;
      i|=16;  		
	}	
	else
	{	 ClrStatus(Device,1);
	        
	
}
	
	
  if((SendTempBufferBak_uart4[3+20/8]&(1<<(20%8)))!=0) //����״̬
	{ SetStatus(Device,5);
	    i|=1;  
	}	
	else
		 ClrStatus(Device,5);
		  
	
   if((SendTempBufferBak_uart4[3+21/8]&(1<<(21%8)))!=0) //���״̬
	 { i|=2; 
		 SetStatus(Device,6);
	 }
	else
    ClrStatus(Device,6);		
	 
  if((SendTempBufferBak_uart4[3+22/8]&(1<<(22%8)))!=0) //��˪״̬
	{ i|=4;
		SetStatus(Device,7);
	}
 else
    ClrStatus(Device,7);


 if((SendTempBufferBak_uart4[3+8/8]&(1<<(8%8)))!=0) //���¸澯
	{
		SetStatus(Device,2);
	
     i|=16;     
	}
 else
    ClrStatus(Device,2);

if((SendTempBufferBak_uart4[3+9/8]&(1<<(9%8)))!=0) //���¸澯
	{
		SetStatus(Device,3);
		i|=16;
	}
 else
    ClrStatus(Device,3);
 
if((SendTempBufferBak_uart4[3+10/8]&(1<<(10%8)))!=0) //�ⲿ�澯
	{
		SetStatus(Device,4);
		i|=16;
	}
 else
    ClrStatus(Device,4); 
 
if((SendTempBufferBak_uart4[3+50/8]&(1<<(50%8)))!=0) //ǿ�Ƴ�˪
	{
		SetStatus(Device,8);
	}
 else
    ClrStatus(Device,8);  
 
if((SendTempBufferBak_uart4[3+51/8]&(1<<(51%8)))!=0) //ϵͳ����
	{
		SetStatus(Device,9);
	}
 else
    ClrStatus(Device,9);  
  

 if((SendTempBufferBak_uart4[3+52/8]&(1<<(52%8)))!=0) //ϵͳ��˪״̬
	{
		SetStatus(Device,10);
	}
 else
    ClrStatus(Device,10);  
 
  uart4_Bit_List[Device-1]&=0xC8;     //ͨѶ״̬
   uart4_Bit_List[Device-1]|=i;	   
	

	uart4_Para_List[(Device-1)*28]=0x00;
	if((SendTempBufferBak_uart4[3+51/8]&(1<<(51%8)))!=0)
                uart4_Para_List[(Device-1)*28+1]=0x01;
	else
		 uart4_Para_List[(Device-1)*28+1]=0x00;
	
	for(i=0;i<7;i++)
	 	uart4_OrigBitList[(Device-1)*7+i]=SendTempBufferBak_uart4[3+i];

	
	 if(uart4_CloudComandResondWaiting==1)
    {  
		 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0;  
    	
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
void uart4_LoraMaster_DealWithSingleRegWriteMaster(uint8_t Device)
 {uint16_t  Addr,Dat;
	 uint8_t   sta;
	 union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;		 
	 
	 
	 
	 uart4_CloudCount=0;
#if  1	 
	 Addr=SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3];
	 Dat=SendTempBufferBak_uart4[4]*256+SendTempBufferBak_uart4[5];
 	 sta=SendTempBufferBak_uart4[0];
	 
	 
			for(i=0;i<14;i++)
			 { if( Addr==uart4_Para_MapTable[i])
				     break;
			 }
//---------------------- ���ص��ǲ������п�������int����			 
      if(i<4)
			{
				uart4_Para_List[(sta-1)*28+i*2]=SendTempBufferBak_uart4[4];
				uart4_Para_List[(sta-1)*28+i*2+1]=SendTempBufferBak_uart4[5];
				
				if(i==0)  //Openclose
				{
					if(SendTempBufferBak_uart4[4]==0xff)
					  {
					      uart4_Para_List[(sta-1)*28+i*2]=0x00;
					     uart4_Para_List[(sta-1)*28+i*2+1]=0x01;
				          }
				}
			    else  if(i==1)  //Openclose
				{
					if(SendTempBufferBak_uart4[5]==1)
					  {
					      uart4_Para_List[(sta-1)*28+i*2]=0x00;
					     uart4_Para_List[(sta-1)*28+i*2+1]^=0x01;
				          }
				}
				
			}	
//--------------------------	 ���ص��ǲ������п�������float����		
		 else	if(i<14)
		 {
         tempT= SendTempBufferBak_uart4[3+uart4_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart4[4+uart4_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)Dat;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart4_Para_List[(sta-1)*28+(i-4)*4+8]=temp.vchar[uart4_floatSeq[0]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+9]=temp.vchar[uart4_floatSeq[1]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+10]=temp.vchar[uart4_floatSeq[2]];
    	uart4_Para_List[(sta-1)*28+(i-4)*4+11]=temp.vchar[uart4_floatSeq[3]];
		 }

//-----------------------------------------���ص��ǲ�������û�еĲ������ƶ˵��������ݣ������ص����ݷ��ظ��ƶˣ�			 
    if(uart4_CloudComandResondWaiting==1)
    {  
	//	 uart4_ReturnCloudData(SendTempBufferBak_uart4,uart4_CloudComandResondLen);
    	uart4_CloudComandResondWaiting=0;
    	uart4_CloudCount=0;
      uart4_Cloudcomand_RP++;
    	 if(uart4_Cloudcomand_RP>=5)
    	    uart4_Cloudcomand_RP=0;
    	 uart4_CloudComandResondWaiting=0;  
			 UptateFlag=1;	
    	
		 }
		
//----------------------------���ظ��ӵ�������
   if(uart4_LocalComandResondWaiting==1)
               	 	    {  uart4_LocalComandResondWaiting=0;
    	                       if(uart4_Localcomand_RP!=uart4_Localcomand_WP)
    	                      { uart4_Localcomand_RP++;
    	                        if(uart4_Localcomand_RP>=5)
    	                            uart4_Localcomand_RP=0;
    	                            uart4_LocalComandResondWaiting=0;  
    	                       }   
    	                       
    	                    }	
//----------------------------------------------ֻҪ��д��ɹ���״̬������Ҫ����һ�ο��ƵĲ���ˢ��--------
    	  for(i=0;i<8;i++)
       uart4_LocalSendcomand[uart4_Localcomand_WP][1+i]=SendTempBufferBak_uart4[i];
       if((SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3]==50)||(SendTempBufferBak_uart4[2]*256+SendTempBufferBak_uart4[3]==51))   
          {  uart4_LocalSendcomand[uart4_Localcomand_WP][2]=0x01; 
            uart4_LocalSendcomand[uart4_Localcomand_WP][6]=0x34;
          }   
        else
          {  uart4_LocalSendcomand[uart4_Localcomand_WP][2]=0x03;
              uart4_LocalSendcomand[uart4_Localcomand_WP][6]=0x30;  
          }
           uart4_LocalSendcomand[uart4_Localcomand_WP][3]=0; 
           uart4_LocalSendcomand[uart4_Localcomand_WP][4]=0; 
            uart4_LocalSendcomand[uart4_Localcomand_WP][5]=0;
					 uart4_LocalSendcomand[uart4_Localcomand_WP][1]=SendTempBufferBak_uart4[0];
					
            uart4_LocalSendcomand[uart4_Localcomand_WP][0] =8;
       tempT=CRC(&uart4_LocalSendcomand[uart4_Localcomand_WP][1],6);
  	uart4_LocalSendcomand[uart4_Localcomand_WP][7]=tempT/256;  //High crc
  	uart4_LocalSendcomand[uart4_Localcomand_WP][8]=tempT%256;  //Low crc
         uart4_Localcomand_WP++;
    	 if(uart4_Localcomand_WP>=5)
    	    uart4_Localcomand_WP=0;
    	  if(uart4_Localcomand_WP==uart4_Localcomand_RP)   
    	  {  uart4_Localcomand_RP++;
    	     if(uart4_Localcomand_RP>=5)
    	       uart4_Localcomand_RP=0;
        }
  
#endif	 
 
 }
 

//---------------------------------------------------------------
/*Function:  void DealWithMoreRegReadMaster(u8 UartFlag,u8 Device)
//Input:    u8 UartFlag:�˿ں�0-4
            u8 Device:��ѯ���ն��豸��
//Output:   None
//Description:�����ն��豸�ظ�����Ϣ
//-----------------------------------����03��ȡ����������--------------------------------------------------03------------------------------------------------------*/
void uart4_LoraMaster_DealWithMoreRegReadMaster(uint8_t Device)
  { union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    uint8_t  i;
    uint16_t  tempT;	
    int16_t   tempInt;

    for(i=0;i<SendTempBufferBak_uart4[2];i++)
		 {
			  uart4_OrigPara_List[(Device-1)*cContorlRegLen*2+i]= SendTempBufferBak_uart4[3+i];
     }
		 
    tempT= SendTempBufferBak_uart4[3]*256+SendTempBufferBak_uart4[4];
		tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
		temp.vf/=10; 
    for(i=0;i<4;i++) //�ط��¶�
      uart4_Var_List[(Device-1)*12+i]=temp.vchar[uart4_floatSeq[i]];
   
		 tempT= SendTempBufferBak_uart4[5]*256+SendTempBufferBak_uart4[6];  
   tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    temp.vf/=10; 
		 for(i=0;i<4;i++)  //��˪�¶� 
      uart4_Var_List[(Device-1)*12+8+i]=temp.vchar[uart4_floatSeq[i]];   
    
              //4��uint16 ����
   for(i=1;i<4;i++) 
    { uart4_Para_List[(Device-1)*28+i*2]=SendTempBufferBak_uart4[3+uart4_Para_MapTable[i]*2];
      uart4_Para_List[(Device-1)*28+i*2+1]=SendTempBufferBak_uart4[3+uart4_Para_MapTable[i]*2+1];
			if(i==1)
			{
				uart4_Para_List[(Device-1)*28+i*2+1]^=1;  //20220216
			}
   }
             
  for(i=4;i<9;i++)   //5��uint32 ����
    { 
			tempT= SendTempBufferBak_uart4[3+uart4_Para_MapTable[i*2-4]*2]*256+SendTempBufferBak_uart4[4+uart4_Para_MapTable[i*2-4]*2];
    	tempInt=(int16_t)tempT;
		 temp.vf= (float)(tempInt);
    	temp.vf/=10; 
			uart4_Para_List[(Device-1)*28+(i-4)*4+8]=temp.vchar[uart4_floatSeq[0]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+9]=temp.vchar[uart4_floatSeq[1]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+10]=temp.vchar[uart4_floatSeq[2]];
    	uart4_Para_List[(Device-1)*28+(i-4)*4+11]=temp.vchar[uart4_floatSeq[3]];
    }
 }  

/*---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����04������
//----------------------------- ����04��ȡ������ֵ���---------------------------------------------------------------------------------------------04---------*/

void uart4_LoraMaster_DealWithMoreDiReadMaster(uint8_t Device)
 { uint8_t  i; 
 uint16_t  Tempint;
 float  tempf; 
   if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x02))
     {  uart4_Var_List[(Device-101)*cVarLenByte+120]=SendTempBufferBak_uart4[3];
        uart4_Var_List[(Device-101)*cVarLenByte+121]=SendTempBufferBak_uart4[4]; //a���ѹ
        uart4_Var_List[(Device-101)*cVarLenByte+122]=SendTempBufferBak_uart4[9];
        uart4_Var_List[(Device-101)*cVarLenByte+123]=SendTempBufferBak_uart4[10]; //a�����
        uart4_Var_List[(Device-101)*cVarLenByte+124]=SendTempBufferBak_uart4[63];
        uart4_Var_List[(Device-101)*cVarLenByte+125]=SendTempBufferBak_uart4[64]; //����
			  uart4_Var_List[(Device-101)*cVarLenByte+126]=SendTempBufferBak_uart4[65];
        uart4_Var_List[(Device-101)*cVarLenByte+127]=SendTempBufferBak_uart4[66]; //����
     }
   else if((ParaList[cMotorType*2+1]==0x03)||(ParaList[cMotorType*2+1]==0x04))
     { Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart4[3+i*2]*256+SendTempBufferBak_uart4[3+i*2+1];
     Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart4_Var_List[(Device-101)*cVarLenByte+120]=Tempint/256;
        uart4_Var_List[(Device-101)*cVarLenByte+121]=Tempint%256; //��ѹ
        
 //--------------------------------------------------------       
        Tempint=0;
     	for(i=0;i<3;i++)
     	  Tempint+=SendTempBufferBak_uart4[9+i*2]*256+SendTempBufferBak_uart4[9+i*2+1];
     	Tempint/=3;
     	tempf=(float)(Tempint)*1.73;
     	Tempint=(uint16_t)(tempf);
     	
     	uart4_Var_List[(Device-101)*cVarLenByte+122]=Tempint/256;
        uart4_Var_List[(Device-101)*cVarLenByte+123]=Tempint%256; //����
        
        
        uart4_Var_List[(Device-101)*cVarLenByte+124]=SendTempBufferBak_uart4[17];
        uart4_Var_List[(Device-101)*cVarLenByte+125]=SendTempBufferBak_uart4[18]; //����
     	
     }
      
}
 
//----------------------------------------------------------------

//----------------------------------10-----------------------------
//Function:  void DealWithSingleRegWrite(uint Addr,uchar CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����0x10д����
//-----------------------------------10-----------------------------
void uart4_LoraMaster_DealWithMoreRegWriteMaster(uint8_t Device)
 { 
  
 
  
  
  
 }
