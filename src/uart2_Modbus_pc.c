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
//Description: �ȴ�modbus����Э�飬����д��Ԫ�Ͷ���Ԫ
//format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          ��ַ	������	��ַ��	��ַ��	������ ������	CRC�ĸ�λ	CRC�ĵ�λ
//----------------------------------------------------------
//CONSIST
#define cRightStatus                 0x01
#define cCrcWrong                    0x02
#define cModbusOverTime              0x05   //need modified 

//modbus ͨѶЭ�������
#define cComandReadCoil               0x01
#define cComandReadDiStatus           0x02
#define cComandReadHoldRegister       0x03
#define cComandReadDiRegister         0x04

#define cComandWriteSingleCoil 0x05
#define cComandWriteMoreCoil   0x0f

#define cComandWriteSingleUint 0x06
#define cComandWriteMoreUint   0x10


unsigned char CheckModbusRespond_uart2(unsigned char   *pp)
 {unsigned char  SetT[6];
  WatchdogReset(); 
  
	               if(pp[1]==cComandReadCoil)           //������coil
               	 	   { uart2_DealWithSingleCoilRead_01(pp);
               	 	     return(cRightStatus);
										 }
               	 	 if(pp[1]==cComandReadDiStatus)           //������coil  02
               	 	   { uart2_DealWithSingleCoilstatus_02(pp);
               	 	     return(cRightStatus);     
               	 	    }
               	else  if(pp[1]==cComandWriteSingleCoil)           //д����coil 05
               	 	   { uart2_DealWithSingleCoilWrite_05(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//singleWrite  06-----------------------------------------------------                	 	
                 else if(pp[1]==cComandWriteSingleUint)           //д�����Ĵ���06
               	 	   { uart2_DealWithSingleRegWrite_06(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(pp[1]==cComandReadHoldRegister)    //�����ּĴ���03 
               	 	   { uart2_DealWithMoreRegRead_03(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(pp[1]==cComandReadDiRegister)      //�����ݼĴ���04 
               	 	   { uart2_DealWithMoreDiRead_04(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//Read  Variable  16-----------------------------------------------------                 	 	    
               	  else if(pp[1]==0x10)         //д������ݼĴ���10
               	 	   { uart2_DealWithMoreRegWrite_10(pp);
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
//Description: �ظ�modbus����1д����
//-------------------------------------------------------01---------    
    
 void uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
 {
  uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
   unsigned char   SendTempBuffer1[20];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
      
       TempCrc=SendTempBuffer[2]*256;
         TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
  if( TempCrc<5000)   
  { SendTempBuffer1[2]=(SendTempBuffer[4]*256+SendTempBuffer[5]-1)/8+1; 
         for(i=0;i<SendTempBuffer[4]*256+SendTempBuffer[5];i++)
          {
            if((	uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc+i)/8]&(1<<((TempCrc+i)%8)))==0)
               SendTempBuffer1[j+i/8]&=((1<<(i%8))^0xff);  
           else
              SendTempBuffer1[j+i/8]|=(1<<(i%8)); 
             k= j+i/8; 
          }
    
  	 TempCrc=CRC(SendTempBuffer1,k+1);
  	 SendTempBuffer1[k+1]=TempCrc/256;  //High crc
  	 SendTempBuffer1[k+2]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer1,k+3);

 }
else
  uart0_AddCloudTranlate(SendTempBuffer) ;	
}

 
 
void uart2_DealWithSingleCoilstatus_02(unsigned char  *SendTempBuffer )
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
            if((uart0_Bit_List[(SendTempBuffer[0]-1)*7+(TempCrc+i)/8]&(1<<((TempCrc+i)%8)))==0)
               SendTempBuffer1[j+i/8]&=(1<<(i%8))^0xff;  
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
//Description: �ظ�modbus����05д����
//-------------------------------------------------------05---------    
    
 void uart2_DealWithSingleCoilWrite_05(unsigned char  *SendTempBuffer )
 {
 uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
   unsigned char   SendTempBuffer1[20];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
         TempCrc=SendTempBuffer[2]*256;
         TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
  if( TempCrc<5000)         
	{ if(SendTempBuffer[4]*256+SendTempBuffer[5]==0xff00)
          { uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]|=(1<<(TempCrc%8));
            
          }
         else if(SendTempBuffer[4]*256+SendTempBuffer[5]==0x0000)
         {  uart0_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]&=(1<<(TempCrc%8)^0xff); 
         
        }
				SendDataToBus1_uart2(SendTempBuffer1,8); 
			  SendTempBuffer1[0]=TempCrc/56+1;
				SendTempBuffer1[2]=0;
				SendTempBuffer1[3]=TempCrc%56;
				 TempCrc=CRC(SendTempBuffer1,6);
  	      SendTempBuffer1[6]=TempCrc/256;  //High crc
  	      SendTempBuffer1[7]=TempCrc%256;  //Low crc
        uart2_AddLocalBuffer_16(0x05,SendTempBuffer1) ;
     
 
  }
else

   
	  uart0_AddCloudTranlate(SendTempBuffer) ;


}	





			 
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����06д����
//----------------------------------------------------------------
void uart2_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer )
 {uint16_t   i,j;
   uint16_t  TempCrc=0;
   int16_t   TEmpInter1,TEmpInter2;
	 int8_t SetT[7];
//   unsigned char   tempflash[2];
	 unsigned char   SendTempBuffer1[8];
      for(i=0;i<8;i++)
  	  SendTempBuffer1[i]=SendTempBuffer[i];
  	    
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
  	  i=TempCrc;
  	  
  	  
/*-------------ƫ��ֵ�趨������������ڲ���ƫ��ֵ����Ҫ���ű���=78������²���Ч�� 
               �¶�ƫ������ķ�ΧΪ����300��30�ȣ���ʪ��Ϊ����200��20%��
*/ 	 	
//add 2016-5-1 13:48

 if( i<2010)
   {
  
  	         
  //-----------�޸��豸��ַ	         
  	  if(i==0)  //�޸��豸��ַ
  	   	  {
  	   	  	 
  	   	   ParaList[cDeviceNum*2]=SendTempBuffer[4];
  	           ParaList[cDeviceNum*2+1]=SendTempBuffer[5]; 
  	          if((SendTempBuffer[4]==0)&&((SendTempBuffer[5]!=0)&&(SendTempBuffer[5]!=255)) )  //2016-01-18 21:19
  	              {   
     	          	ProgramEepromByte();
  	                 ReadEepromByte();
  	              } 
  	              
  	           }    
  //--------------------���г�ʼ������-------------------------
  	        else if(((i==26)||(i==cInitial))&&(SendTempBuffer[4]==0x00)&&(SendTempBuffer[5]==26))//initial
  	         { ResetFlag=0;  //2021-02-21 17:33
  	           InitialPara();
  	           BellOn();
  	           DelayMicroSeconds (1000);
  	           BellOff();
  	
		 }
    
  	      //---------------------------�޸�ʱ��--------------------------    
  	        else if((i==cTimeTrig)&&(SendTempBuffer[4]==0x00)&&(SendTempBuffer[5]==78))//initial   
  	          {
  	           SetT[6]=ParaList[cYear*2+1]/10*16+ParaList[cYear*2+1]%10;
  	           SetT[5]=ParaList[cMonth*2+1]/10*16+ParaList[cMonth*2+1]%10;
  	           SetT[3]=ParaList[cDay*2+1]/10*16+ParaList[cDay*2+1]%10;
  	           SetT[2]=ParaList[cHour*2+1]/10*16+ParaList[cHour*2+1]%10;
  	           SetT[1]=ParaList[cMiniter*2+1]/10*16+ParaList[cMiniter*2+1]%10;
  	           SetT[0]=ParaList[cSecond*2+1]/10*16+ParaList[cSecond*2+1]%10;  
  	      	   SetPCF8563(SetT);
  	      	  }	         
  	  
  	   	       
  	//------------------------special adjust------10��error adjust------------------------------   	            
  	       else if(( i>=cAllowHumi)&&(i<cHumiOff1+10))
  	          {
  	          	 if(ParaList[23]==78)
  	          	  {   ParaList[i*2]=SendTempBuffer[4];
  	                      ParaList[i*2+1]=SendTempBuffer[5];
  	                      ProgramEepromByte();
  	                      ReadEepromByte();
  	          	  }
  	          }
  	 //------------------------------------------spi flash  initial------------------------------          
  	       else if(i==cSpiFlashInitial)
  	         {  LCD4_Clear();
                    LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"                ");
  	            W25QXX_Erase_Chip( );
                    SpiFlash_WaitReady();	
  	            LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"    Over        ");	
                      
  	         } 	 
  	   else
  	   { 
  	      if((i!=cReordNumIndex)&& (i!=cCurrentReordNum)&& (i!=cCurrentReordPage)&&(i!=cPagePos))  //2015-09-19 13:59
  	   	              { ParaList[i*2]=SendTempBuffer[4];
  	                      ParaList[i*2+1]=SendTempBuffer[5]; 
  	                    ProgramEepromByte();
  	                    ReadEepromByte();	
  	               }
  	     }
  	 }       
  	               
/*���õ���ʪ��10��ƫ��У׼ֵ,�趨�ĵ�ַ��ʼֵ,��ַ����(���16���ڵ�),lora���� 3000���ϵ�ַ�Ĵ���*/
  	//3000---3019
else  if((i>=cTempOff)&&(i<4000))
  	        {  
  	          if(ParaList[23]==78)
  	           { 
  	             ParaList[(i-c02D_ParaActual)*2]=SendTempBuffer[4];
  	             ParaList[(i-c02D_ParaActual)*2+1]=SendTempBuffer[5];	
  	             ProgramEepromByte();
  	             ReadEepromByte();	
  	           }	
	       }
 //----------- 03D para------------------ 	
 else if(( i>=2010) &&( i<3000))  
  	      {	
  	    	
  	      	          ParaList[(i-cParaActual)*2]=SendTempBuffer[4];
  	                  ParaList[(i-cParaActual)*2+1]=SendTempBuffer[5]; 
  	            if(StartBak!=ParaList[(cSaveDataFlag03D-cParaActual)*2+1])
         	     {
         	       StartBak=ParaList[(cSaveDataFlag03D-cParaActual)*2+1];
		       if(StartBak==0)  //����--->ֹͣ
         	        { 
                           VarList[cRealDataAct*2+1]=0x0ff;
                           VarList[cRealDataAct*2]=0x0ff; 
		         } 
		        else   //ֹͣ--->����
		         History.AlarmStatus1=0x00; 
		     }
         	
  //-----------����Ϊ���������ı��,��ʱ������ض�Ӧ�Ĳ���---------------------- 	 
                if(i==cAlarmOutControl03D)
		 {
	             DealAlarm();
	             ProgramEepromByte();  
                 }	 
//-------------------------------------------------//	 record status	 
	      else if(i==cBackLightControl)	 
		 {     
                       DealBacklight();
		  }
	     else if(i!=cTestMode03D)
	      {  
	        ProgramEepromByte();  
  	        ReadEepromByte();
  	      }
  	  }
 //-----------------------------------------
 else if((i>=4000)&&(i<5000) ) 
   {
      i=i-4000;
	   
		 if((i%cContorlRegLen)!=52) 
		 {
 
        SendTempBuffer[0]=i/cContorlRegLen+1;
				SendTempBuffer[3]=i%cContorlRegLen;
				 TempCrc=CRC(SendTempBuffer,6);
  	      SendTempBuffer[6]=TempCrc/256;  //High crc
  	      SendTempBuffer[7]=TempCrc%256;  //Low crc
				uart0_AddCloudBuffer(i%cContorlRegLen,0x06,SendTempBuffer); 
         // uart0_AddLocalBuffer_16(i%cContorlRegLen,0x06,SendTempBuffer); 
		 }			
			
    }	
	else if(i>=5000) 
       
  	uart0_AddCloudTranlate(SendTempBuffer) ;			
		
		
		
 //-------------------------------------------------------------------  
   if(SendTempBuffer[0]!=0) 
  	
  	    SendDataToBus1_uart2(SendTempBuffer1,8);  
	 GetCoupleList();
	 for(i=0;i<10;i++)
	  UpdateTimerDelay(i);
}

//---------------------------------------------------------------
//Function:  void DealWithMoreRegRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����03������
//----------------------------------------------------------------
void uart2_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer )
  {
   uint16_t    TempCrc=0;
   uint16_t   i;
   uint16_t   k,j=3;
	unsigned char   SendTempBuffer1[300];	
       for(i=0;i<8;i++)
  	       SendTempBuffer1[i]=SendTempBuffer[i];
  
  	  
  if(SendTempBuffer1[0]==DeviceNum )     
    {    TempCrc=SendTempBuffer[2]*256;
       TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
//-------------------------------      
		if(TempCrc>=5000)
		    uart0_AddCloudTranlate(SendTempBuffer);  
     else	if(TempCrc>=4000)
        {
           TempCrc=TempCrc-4000;
					// uart0_AddCloudBuffer(TempCrc,0x03,SendTempBuffer); 
					   SendTempBuffer1[2]=SendTempBuffer1[5]*2; 
						for(k=TempCrc;k<TempCrc+SendTempBuffer1[2]/2;k++) 
  	          {
  	                  SendTempBuffer1[j++]= uart0_OrigPara_List[k*2];
  	                  SendTempBuffer1[j++]= uart0_OrigPara_List[k*2+1];
  	         }
            TempCrc=CRC(SendTempBuffer1,j);
  	 SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	 SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer1,j);
         }
//-------------------------				
      else
       {    
       
       
        SendTempBuffer1[0]=DeviceNum;
        SendTempBuffer1[1]=0x03; 
        SendTempBuffer1[2]=SendTempBuffer1[5]*2; 
				 
  	if( SendTempBuffer1[5]<=ParaNum/2)  
  	  { 
  	    
  	    
//------------------<2010  	    
  	    if(TempCrc<cSaveDataFlag03D)
  	      { i=TempCrc;
  	        if(SendTempBuffer1[2]/2==14)
						{
							for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	          {
  	             
  	                   SendTempBuffer1[j++]=retrnPara(k*2);	
  	                   SendTempBuffer1[j++]=retrnPara(k*2+1);	
  	               
  	          }
							
							
//add   2022/8/13 23:14 						
							}
		
//add 2022/8/13 23:16						
						
						
          else
					 {						
						 for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	           {
  	              if(k==0)
  	                 {
  	                   SendTempBuffer1[j++]=ParaList[cDeviceNum*2];	
  	                   SendTempBuffer1[j++]=ParaList[cDeviceNum*2+1];	
  	     	
  	                 }
  	           
  	               else
  	                 { 
  	                   SendTempBuffer1[j++]=ParaList[k*2];	
  	                   SendTempBuffer1[j++]=ParaList[k*2+1];	
  	                 }
  	          }
  	       } 
				 }         	
//----------------------->=3000  	                 	
  	   else  if(TempCrc>=cTempOff)
  	       {
  	       	 TempCrc-=c02D_ParaActual;
  	         i=(unsigned char)TempCrc;
  	        for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	       	{  
  	           	  SendTempBuffer1[j++]=ParaList[k*2];	
  	                  SendTempBuffer1[j++]=ParaList[k*2+1];
  	           }
  	        }
 //-------------------------------- 	  
  	else  if(TempCrc>=cSaveDataFlag03D)
  	   {  
  	        TempCrc-=cParaActual;
  	         i=(unsigned char)TempCrc;
  	        for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	           {  
  	           	  SendTempBuffer1[j++]=ParaList[k*2];	
  	                  SendTempBuffer1[j++]=ParaList[k*2+1];
  	           }
  	    }
 //-------------------------------------- 	    
  	  
         TempCrc=CRC(SendTempBuffer1,j);
  	 SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	 SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart2(SendTempBuffer1,j);
  	 
  	}
	}
  }
else      //for read  offset para for lora node
  {
      if((SendTempBuffer[0]>=AddrStart)
   	 &&(SendTempBuffer[0]<AddrStart+AddrLen)
   	 &&(  ParaList[(cLoraNodeAliveSet+SendTempBuffer[0]-AddrStart)*2+1]==1) 
   	)
   	  { 
   	     TempCrc=SendTempBuffer[2]*256;
             TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
             SendTempBuffer1[0]=SendTempBuffer[0];
             SendTempBuffer1[1]=0x03; 
             SendTempBuffer1[2]=SendTempBuffer1[5]*2;
				    i=(unsigned char)TempCrc;
             for(k=i;k<i+SendTempBuffer1[2]/2;k++) 
  	       	{  
  	           	  SendTempBuffer1[j++]=ParaList[(k-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2];// ��ʪ��λ��+�豸��Ӧλ��+ƫ��baseλ��
  	                  SendTempBuffer1[j++]=ParaList[(k-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2+1];
  	         }
              TempCrc=CRC(SendTempBuffer1,j);
  	      SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	      SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	      SendDataToBus1_uart2(SendTempBuffer1,j);
            
           
           }   
  	  	
   		
   }
   
  } 	



//---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����04������
//----------------------------------------------------------------
void uart2_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer )
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
  	        SendTempBuffer[j++]=uart0_Var_List[k*2];
  	        SendTempBuffer[j++]=uart0_Var_List[k*2+1];
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
//Description: �ظ�modbus����0x10д����
//----------------------------------------------------------------
void uart2_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer )
 { uint8_t  i;
  unsigned char j,k;
  unsigned char  SetT[6];  //2015-10-21 5:41
   uint16_t  TempCrc=0;
   union
     {float  vf;
      uint8_t vchar[4];
     }temp;
    
 
  	    
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //base addr
  	  
  //---------------16bit ����	
  	  if((TempCrc%14)<4)
  	   {
				  if((TempCrc%14)==0)
						 if(SendTempBuffer[8]==0x01)
						 {
							 SendTempBuffer[7]=0xff;
							 SendTempBuffer[8]=0x00;
						 }
           uart0_AddLocalBuffer_16(TempCrc,0x06,SendTempBuffer);
  	   }
  	
//----------------------32bit ����  	 
     else  //32bit
  	   {uart0_AddLocalBuffer_32(TempCrc,0x06,SendTempBuffer);   
  	}    
//-------------------------------------------  	    
  	 TempCrc=CRC(SendTempBuffer,6);
  	 SendTempBuffer[6]=TempCrc/256;  //High crc
  	 SendTempBuffer[7]=TempCrc%256;  //Low crc
  	 if(SendTempBuffer[0]!=0) 
  	    
  	    SendDataToBus1_uart2(SendTempBuffer,8);  
  	
  	}
  	
  	
  	
  	
  	
  	
  	
  	
  	
