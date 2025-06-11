#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"


unsigned char CheckModbusRespond_uart2(unsigned char   *pp);
void  Program();
void uart2_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer );

void uart2_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer );
void uart2_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer );
void uart2_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer );
void uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer );
 void uart2_DealWithSingleCoilWrite_05(unsigned char  *SendTempBuffer );
  void uart2_DealWithSingleCoilstatus_02(unsigned char  *SendTempBuffer );



unsigned char CheckModbusRespond_uart2_Tp(unsigned char   *pp);
void uart2_DealWithSingleRegWrite_06_Tp(unsigned char  *SendTempBuffer );

void uart2_DealWithMoreRegRead_03_Tp(unsigned char  *SendTempBuffer );
void uart2_DealWithMoreDiRead_04_Tp(unsigned char  *SendTempBuffer );
void uart2_DealWithMoreRegWrite_10_Tp(unsigned char  *SendTempBuffer );
void uart2_DealWithSingleCoilRead_01_Tp(unsigned char  *SendTempBuffer );
 void uart2_DealWithSingleCoilWrite_05_Tp(unsigned char  *SendTempBuffer );
  void uart2_DealWithSingleCoilstatus_02_Tp(unsigned char  *SendTempBuffer );



extern  uint8_t uart0_Var_List[320];
extern  uint8_t uart0_Bit_List[10];
extern  uint8_t uart0_OrigPara_List[cContorlRegLen*20];
extern uint8_t uart0_OrigBitList[70];


//--------------------------------------------------------
//Function:  unsigned char CheckModbusRespond()
//Input:    None
//Output:   None
//Description: 等待modbus请求协议，包括写单元和读单元
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          地址	功能码	地址高	地址低	数量高 数量低	CRC的高位	CRC的低位
//----------------------------------------------------------
//CONSIST
#define cRightStatus                 0x01
#define cCrcWrong                    0x02
#define cModbusOverTime              0x05   //need modified 

//modbus 通讯协议控制字
#define cComandReadCoil               0x01
#define cComandReadDiStatus           0x02
#define cComandReadHoldRegister       0x03
#define cComandReadDiRegister         0x04

#define cComandWriteSingleCoil 0x05
#define cComandWriteMoreCoil   0x0f

#define cComandWriteSingleUint 0x06
#define cComandWriteMoreUint   0x10



  	
  	
  	
  	
  	
  	
unsigned char CheckModbusRespond_uart2_Tp(unsigned char   *pp)
 {unsigned char  SetT[6];
  WatchdogReset(); 
                 if(pp[1]==cComandReadCoil)           //读单个coil
               	 	   { uart2_DealWithSingleCoilRead_01_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }
               	else  if(pp[1]==cComandWriteSingleCoil)           //写单个coil 05
               	 	   { uart2_DealWithSingleCoilWrite_05_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//singleWrite  06-----------------------------------------------------                	 	
                 else if(pp[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { uart2_DealWithSingleRegWrite_06_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(pp[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   { uart2_DealWithMoreRegRead_03_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(pp[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart2_DealWithMoreDiRead_04_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//Read  Variable  16-----------------------------------------------------                 	 	    
               	  else if(pp[1]==0x10)     
               	 	   { uart2_DealWithMoreRegWrite_10_Tp(pp);
               	 	     return(cRightStatus);
               	 	    }	                  	 	    

//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 pp[0]=pp[1]+0x80; 
  	                   pp[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1_uart2(pp,2);
                    	 return(cCrcWrong);
                    }
                }
      	     
    
    
//---------------------------------------------------------------
//Function:   uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
//Input:    None
//Output:   None
//Description: 回复modbus请求1写数据
//-------------------------------------------------------01---------    
    
 void uart2_DealWithSingleCoilRead_01_Tp(unsigned char  *SendTempBuffer )
 {
  uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
   unsigned char   SendTempBuffer1[20];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
         TempCrc=SendTempBuffer[2]*256;
         TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
         SendTempBuffer1[2]=(SendTempBuffer[4]*256+SendTempBuffer[5]-1)/8+1; 
         for(i=0;i<SendTempBuffer[4]*256+SendTempBuffer[5];i++)
          {
            if((	uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc+i)/8]&(1<<((TempCrc+i)%8)))==0)
              SendTempBuffer1[j+i/8]&=(1<<((i%8))^0xff);  
           else
              SendTempBuffer1[j+i/8]|=1<<(i%8);					 
             k= j+i/8; 
          }
    
  	 TempCrc=CRC(SendTempBuffer1,k+1);
  	 SendTempBuffer1[k+1]=TempCrc/256;  //High crc
  	 SendTempBuffer1[k+2]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer1,k+3);

 }

//---------------------------------------------------------------
//Function:   uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
//Input:    None
//Output:   None
//Description: 回复modbus请求05写数据
//-------------------------------------------------------05---------    
    
 void uart2_DealWithSingleCoilWrite_05_Tp(unsigned char  *SendTempBuffer )
 {
 uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
   unsigned char   SendTempBuffer1[20];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
         TempCrc=SendTempBuffer[2]*256;
         TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
         if(SendTempBuffer[4]*256+SendTempBuffer[5]==0xff00)
          { uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]|=(1<<(TempCrc%8));
            
          }
         else if(SendTempBuffer[4]*256+SendTempBuffer[5]==0x0000)
         {  uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]&=(1<<(TempCrc%8)^0xff); 
         
        }
				 
			
     //   uart2_AddLocalBuffer_16(0x05,SendTempBuffer) ;
     SendDataToBus1_uart2(SendTempBuffer1,8);
     uart2_AddLocalBuffer_16(0x05,SendTempBuffer) ; 
 }
 

//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求06写数据
//----------------------------------------------------------------
void uart2_DealWithSingleRegWrite_06_Tp(unsigned char  *SendTempBuffer )
 {uint16_t   i,j;
   uint16_t  TempCrc=0,CRCTemp;
   int16_t   TEmpInter1,TEmpInter2;
	 int8_t SetT[7];
//   unsigned char   tempflash[2];
      for(i=0;i<8;i++)
  	  SendTempBuffer[i]=SendTempBuffer[i];
  	    
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
  	  i=TempCrc;
  	if(i==uart2_Tp_pc)
		{ ParaList[i*2]=SendTempBuffer[4];
  	  ParaList[i*2+1]=SendTempBuffer[5]; 
		}
	 else
		 uart2_AddLocalBuffer_16(0x06,SendTempBuffer) ;
}

//---------------------------------------------------------------
//Function:  void DealWithMoreRegRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求03读数据
//----------------------------------------------------------------
void uart2_DealWithMoreRegRead_03_Tp(unsigned char  *SendTempBuffer )
  {
   uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
	unsigned char   SendTempBuffer1[120];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
      TempCrc=SendTempBuffer[2]*256;
        TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
        
        SendTempBuffer1[2]=SendTempBuffer1[5]*2;   
  	i=(unsigned char)TempCrc;
  	         for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	          {
  	              
  	                   SendTempBuffer1[j++]= uart0_OrigPara_List[(SendTempBuffer[0]-1)*cContorlRegLen*2+k*2];
  	                   SendTempBuffer1[j++]= uart0_OrigPara_List[(SendTempBuffer[0]-1)*cContorlRegLen*2+k*2+1];
	
  	                  
  	                 }
  	       
  	 TempCrc=CRC(SendTempBuffer1,j);
  	 SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	 SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer1,j);
  	 
  	}
 
 
//---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据
//----------------------------------------------------------------
void uart2_DealWithMoreDiRead_04_Tp(unsigned char  *SendTempBuffer )
 {  
     uint16_t   TempCrc=0;
      unsigned char    i;
     unsigned char   k,j=3;
     WatchdogReset();
      
        TempCrc=SendTempBuffer[2]*256;
        TempCrc+=SendTempBuffer[3];  //addr
        SendTempBuffer[0]=SendTempBuffer[0];
        SendTempBuffer[1]=0x04; 
        SendTempBuffer[2]=SendTempBuffer[5]*2;   
  	
  	  i=(unsigned char)TempCrc;
  	      
  	     for(k=i;k<i+SendTempBuffer[2]/2;k++) 
  	      { WatchdogReset();
  	       if((k>=cNodePower)&&(k<cNodePower+16) )
					 { SendTempBuffer[j++]=VarList[(cNodePower+(k-cNodePower)*2)*2+1];
            SendTempBuffer[j++]=VarList[(cNodePower+(k-cNodePower)*2)*2+3];
					 }
					 else
						 
					 {	SendTempBuffer[j++]=VarList[k*2];
  	          SendTempBuffer[j++]=VarList[k*2+1];
  	      }  
				}	
		
					
          TempCrc=CRC(SendTempBuffer,j);
  	  SendTempBuffer[j++]=TempCrc/256;  //High crc
  	  SendTempBuffer[j++]=TempCrc%256;  //Low crc
  	  SendDataToBus1_uart2(SendTempBuffer,j);	
  }

  
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//----------------------------------------------------------------
void uart2_DealWithMoreRegWrite_10_Tp(unsigned char  *SendTempBuffer )
 {unsigned char i;
  unsigned char j,k;
  unsigned char  SetT[6];  //2015-10-21 5:41
   uint16_t  TempCrc=0;
  
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //base addr
  	 if( SendTempBuffer[5]<cReceivelength/2)  
  	  { i=(unsigned char)TempCrc;
  	    j=7;
  	    
  	   
  	         i=(unsigned char)TempCrc;
  	         for(k=i;k<i+SendTempBuffer[5];k++) 
  	            {
  	              ParaList[k*2]=SendTempBuffer[j++];	
  	              ParaList[k*2+1]=SendTempBuffer[j++];
  	             
  	           }
  	TempCrc=CRC(SendTempBuffer,6);
  	 SendTempBuffer[6]=TempCrc/256;  //High crc
  	 SendTempBuffer[7]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer,8);  
  	
  	}
}   	

