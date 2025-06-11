#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
unsigned char CheckModbusRespond_uart0(unsigned char   *pp);
void  Program();
void uart0_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer );
void uart0_DealwithPage(uint16_t page);
void uart0_DealwithPageRead_44_1(unsigned char  *SendTempBuffer ,unsigned char flag );
void uart0_DealwithPageRead_44(unsigned char  *SendTempBuffer );
void uart0_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer );
void uart0_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer );
void uart0_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer );
void uart0_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer );
 void uart0_DealWithSingleCoilWrite_05(unsigned char  *SendTempBuffer );
  void uart0_DealWithSingleCoilstatus_02(unsigned char  *SendTempBuffer );

extern  uint8_t uart4_Var_List[320];
extern  uint8_t uart4_Bit_List[10];
extern  uint8_t uart4_OrigPara_List[cContorlRegLen*20];
extern uint8_t uart4_OrigBitList[70];
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


unsigned char CheckModbusRespond_uart0(unsigned char   *pp)
 {unsigned char  SetT[6];
  WatchdogReset(); 
  
	               if(pp[1]==cComandReadCoil)           //������coil
               	 	   { uart0_DealWithSingleCoilRead_01(pp);
               	 	     return(cRightStatus);
										 }
               	 	 if(pp[1]==cComandReadDiStatus)           //������coil  02
               	 	   { uart0_DealWithSingleCoilstatus_02(pp);
               	 	     return(cRightStatus);     
               	 	    }
               	else  if(pp[1]==cComandWriteSingleCoil)           //д����coil 05
               	 	   { uart0_DealWithSingleCoilWrite_05(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//singleWrite  06-----------------------------------------------------                	 	
                 else if(pp[1]==cComandWriteSingleUint)           //д�����Ĵ���06
               	 	   { uart0_DealWithSingleRegWrite_06(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(pp[1]==cComandReadHoldRegister)    //�����ּĴ���03 
               	 	   { uart0_DealWithMoreRegRead_03(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(pp[1]==cComandReadDiRegister)      //�����ݼĴ���04 
               	 	   { uart0_DealWithMoreDiRead_04(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//Read  Variable  16-----------------------------------------------------                 	 	    
               	  else if(pp[1]==0x10)         //д������ݼĴ���10
               	 	   { uart0_DealWithMoreRegWrite_10(pp);
               	 	     return(cRightStatus);
               	 	    }	                  	 	    
//Read  History  0x41-----------------------------------------------------                 	 	    
               	  else if(pp[1]==0x41)     
               	 	   { DealWithHisRead(pp);
               	 	     return(cRightStatus);
               	 	    }	                  	 	    
//read page-------------------------------------------------------------
        else if(pp[1]==0x44)    //����ʷ����0x41
               	 	   { uart0_DealwithPageRead_44(pp);
               	 	     return(cRightStatus);
               	 	    }        	 	 
//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 pp[0]=pp[1]+0x80; 
  	                   pp[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1(pp,2);
                    	 return(cCrcWrong);
                    }
                }
      	     
          

    

void  Program()
          {      ProgramEepromByte();
  	         ReadEepromByte();
  	   }  



//---------------------------------------------------------------
//Function:   uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
//Input:    None
//Output:   None
//Description: �ظ�modbus����1д����
//-------------------------------------------------------01---------    
    
 void uart0_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
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
            if((	uart4_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc+i)/8]&(1<<((TempCrc+i)%8)))==0)
               SendTempBuffer1[j+i/8]&=((1<<(i%8))^0xff);  
           else
              SendTempBuffer1[j+i/8]|=(1<<(i%8)); 
             k= j+i/8; 
          }
    
  	 TempCrc=CRC(SendTempBuffer1,k+1);
  	 SendTempBuffer1[k+1]=TempCrc/256;  //High crc
  	 SendTempBuffer1[k+2]=TempCrc%256;  //Low crc
  	 SendDataToBus1(SendTempBuffer1,k+3);

 }
else
  uart4_AddCloudTranlate(SendTempBuffer) ;	
}

 void uart0_DealWithSingleCoilstatus_02(unsigned char  *SendTempBuffer )
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
            if((uart4_Bit_List[(SendTempBuffer[0]-1)*7+(TempCrc+i)/8]&(1<<((TempCrc+i)%8)))==0)
               SendTempBuffer1[j+i/8]&=(1<<(i%8))^0xff;  
           else
               SendTempBuffer1[j+i/8]|=1<<(i%8); 
             k= j+i/8; 
          }
    
  	 TempCrc=CRC(SendTempBuffer1,k+1);
  	 SendTempBuffer1[k+1]=TempCrc/256;  //High crc
  	 SendTempBuffer1[k+2]=TempCrc%256;  //Low crc
  	 SendDataToBus1(SendTempBuffer1,k+3);

 }


//---------------------------------------------------------------
//Function:   uart2_DealWithSingleCoilRead_01(unsigned char  *SendTempBuffer )
//Input:    None
//Output:   None
//Description: �ظ�modbus����05д����
//-------------------------------------------------------05---------    
    
 void uart0_DealWithSingleCoilWrite_05(unsigned char  *SendTempBuffer )
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
          { uart4_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]|=(1<<(TempCrc%8));
            
          }
         else if(SendTempBuffer[4]*256+SendTempBuffer[5]==0x0000)
         {  uart4_OrigBitList[(SendTempBuffer[0]-1)*7+(TempCrc/8)]&=(1<<(TempCrc%8)^0xff); 
         
        }
				SendDataToBus1(SendTempBuffer1,8); 
			  SendTempBuffer1[0]=TempCrc/56+1;
				SendTempBuffer1[2]=0;
				SendTempBuffer1[3]=TempCrc%56;
				 TempCrc=CRC(SendTempBuffer1,6);
  	      SendTempBuffer1[6]=TempCrc/256;  //High crc
  	      SendTempBuffer1[7]=TempCrc%256;  //Low crc
        uart2_AddLocalBuffer_16(0x05,SendTempBuffer1) ;
     
 
  }
else

   
	  uart4_AddCloudTranlate(SendTempBuffer) ;


}	





			 
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����06д����
//----------------------------------------------------------------
void uart0_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer )
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
   //add 2021-04-18 11:22
   	
   //end of add 2021-04-18 11:22 
    if(	(i==cTempOffset)||(i==cTempOffset+1))  
    {
	#if  0
   	 if((SendTempBuffer[0]>=AddrStart)
   	 &&(SendTempBuffer[0]<AddrStart+AddrLen)
   	 &&(  ParaList[(cLoraNodeAliveSet+SendTempBuffer[0]-AddrStart)*2+1]==1) 
   	
   	 )	
   	   {         ParaList[(i-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2]=SendTempBuffer[4];
  	             ParaList[(i-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2+1]=SendTempBuffer[5]; 
  	              Program();
   	   }
			#endif
   //	 #else 
   	              ParaList[i*2]=SendTempBuffer[4];
  	              ParaList[i*2+1]=SendTempBuffer[5]; 
		      Program();
   	//#endif   
}
  else  if(	(i==cInterTOffet)||(i==cInterHROffet))
   	{  
   			
   		
   	   if(i==cInterTOffet)
  	   { if(ParaList[cDoorVar*2+1]==78)
  	      {  j=SendTempBuffer[4]*256+SendTempBuffer[5];
  	      	 
  	      	 if((j<300)||(j>65236))
  	      	   {  ParaList[i*2]=SendTempBuffer[4];
  	              ParaList[i*2+1]=SendTempBuffer[5]; 
		      Program();
		   }   
  	     }
  	   }
  	else
  	   { if(ParaList[cDoorVar*2+1]==78)
  	      {  j=SendTempBuffer[4]*256+SendTempBuffer[5];
  	      	 
  	      	 if((j<200)||(j>65336))
  	      	   {  ParaList[i*2]=SendTempBuffer[4];
  	              ParaList[i*2+1]=SendTempBuffer[5]; 
		      Program();
		   }   
  	     }
  	   }
    
 	} 
 //-------------------lora  ��д	
 	else  if((i==cLoraReadConfigPara)&&((SendTempBuffer[5]==0x01)||(SendTempBuffer[5]==0x02)) ) //��ȡlora����
  	  	{     
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5]; 
  	    	   ReadOutLoraPara(SendTempBuffer[5]);
  	         }
		
  	    else if  ((i==cLoraReadConfigPara)&&(SendTempBuffer[5]==0x00))  //�˳���ȡ
  	  	{    
					    
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	   SwtichToWorkmode();
  	         }   
  	    else if((i==cLoraWriteConfigPara)&&((SendTempBuffer[5]==0x01)||(SendTempBuffer[5]==0x02)) )  //����lora����
  	  	 {  
   					 ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	   WriteInLoraPara(SendTempBuffer[5]);
  	         } 
  	   else if((i==cLoraWriteConfigPara)&&(SendTempBuffer[5]==0x00))  //�˳�����
  	  	 {     
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	  SwtichToWorkmode();
  	         }
  	         
  //-----------�޸��豸��ַ	         
  	 else  if(i==0)  //�޸��豸��ַ
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
    //--------------ͨ�����õ�ǰ��׼ֵ�������ڲ�ƫ�ʹ��ǰ����ʪ��Ϊ��ǰ���¶�-------
  	        else if(i==cOrigTS)  //t  offset  broadcast set   //2015-09-19 14:01
  	          {
  	             if(ParaList[cDoorVar*2+1]==78)
  	              { 
  	              TEmpInter1=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
  	              	 TEmpInter2=(int16_t)(SendTempBuffer[cTempError*2]*256+SendTempBuffer[cTempError*2+1]);
  	              	 TEmpInter2=TEmpInter2-TEmpInter1;
  	              	 TEmpInter1=(int16_t)(ParaList[cInterTOffet*2]*256+ParaList[cInterTOffet*2+1]);
  	              	 TEmpInter2=TEmpInter2+TEmpInter1;
  	              	 TempCrc=(uint16_t)TEmpInter2;
  	              	 
  	              	 ParaList[cInterTOffet*2]=TempCrc/256;
  	                 ParaList[cInterTOffet*2+1]=TempCrc%256;
		         ProgramEepromByte();
  	                 ReadEepromByte();
  	              }	
  	          }	
  	//-------------------------------------------------------------        
  	        else if(i==cOrigRHS)  //hr  offset  broadcast set   //2015-09-19 14:01
  	          {
  	             if(ParaList[cDoorVar*2+1]==78)
  	              { 
  	              	 TEmpInter1=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
  	              	 TEmpInter2=(int16_t)(SendTempBuffer[cHumiError*2]*256+SendTempBuffer[cHumiError*2+1]);
  	              	 TEmpInter2=TEmpInter2-TEmpInter1;
  	              	 TEmpInter1=(int16_t)(ParaList[cInterHROffet*2]*256+ParaList[cInterHROffet*2+1]);
  	              	 TEmpInter2=TEmpInter2+TEmpInter1;
  	              	 TempCrc=(uint16_t)TEmpInter2;
  	              	 
  	              	 ParaList[cInterHROffet*2]=TempCrc/256;
  	                 ParaList[cInterHROffet*2+1]=TempCrc%256;
		         ProgramEepromByte();
  	                 ReadEepromByte();
  	              }	
  	          	
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
  	  //---------------------------beep control--------------------------   
  	       else if(i==cRemoteControl)
  	   	       {  
			  #ifdef _Loramain 
				ParaList[cAlarmControlPos*2+(SendTempBuffer[0]-1)*2+1]=SendTempBuffer[5];   
  	   	          #else
  	   	               ParaList[cRemoteControl*2+1]=SendTempBuffer[5];    
  	   	           #endif
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
				uart4_AddCloudBuffer(i%cContorlRegLen,0x06,SendTempBuffer); 
         // uart4_AddLocalBuffer_16(i%cContorlRegLen,0x06,SendTempBuffer); 
		 }			
			
    }	
	else if(i>=5000) 
       
  	uart4_AddCloudTranlate(SendTempBuffer) ;			
		
		
		
 //-------------------------------------------------------------------  
   if(SendTempBuffer[0]!=0) 
  	
  	    SendDataToBus1(SendTempBuffer1,8);  
	 GetCoupleList();
	 for(i=0;i<10;i++)
	  UpdateTimerDelay(i);
}
//---------------------------------------------------------------
//Function:  void uart0_DealwithPage(uint16_t page)
//Input:    None
//Output:   None
//Description: �ظ�0x44 page read ����
//----------------------------------------------------------------
void uart0_DealwithPage(uint16_t page)
 {
   
}		
//------------------------------------------------------------------------------
void uart0_DealwithPageRead_44_1(unsigned char  *SendTempBuffer ,unsigned char flag )
 { 
}


void uart0_DealwithPageRead_44(unsigned char  *SendTempBuffer )
 { 
}

//---------------------------------------------------------------
//Function:  void DealWithMoreRegRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����03������
//----------------------------------------------------------------
void uart0_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer )
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
		    uart4_AddCloudTranlate(SendTempBuffer);  
     else	if(TempCrc>=4000)
        {
           TempCrc=TempCrc-4000;
					// uart4_AddCloudBuffer(TempCrc,0x03,SendTempBuffer); 
					   SendTempBuffer1[2]=SendTempBuffer1[5]*2; 
						for(k=TempCrc;k<TempCrc+SendTempBuffer1[2]/2;k++) 
  	          {
  	                  SendTempBuffer1[j++]= uart4_OrigPara_List[k*2];
  	                  SendTempBuffer1[j++]= uart4_OrigPara_List[k*2+1];
  	         }
            TempCrc=CRC(SendTempBuffer1,j);
  	 SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	 SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	 SendDataToBus1(SendTempBuffer1,j);
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
							
							
						}
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
  	 SendDataToBus1(SendTempBuffer1,j);
  	 
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
  	      SendDataToBus1(SendTempBuffer1,j);
            
           
           }   
  	  	
   		
   }
   
  } 	



//---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: �ظ�modbus����04������
//----------------------------------------------------------------
void uart0_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer )
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
  	        SendTempBuffer[j++]=uart4_Var_List[k*2];
  	        SendTempBuffer[j++]=uart4_Var_List[k*2+1];
  	      }  
          TempCrc=CRC(SendTempBuffer,j);
  	  SendTempBuffer[j++]=TempCrc/256;  //High crc
  	  SendTempBuffer[j++]=TempCrc%256;  //Low crc
  	  SendDataToBus1(SendTempBuffer,j);	
 	 }

  
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: �ظ�modbus����0x10д����
//----------------------------------------------------------------
void uart0_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer )
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
           uart4_AddLocalBuffer_16(TempCrc,0x06,SendTempBuffer);
  	   }
  	
//----------------------32bit ����  	 
     else  //32bit
  	   {uart4_AddLocalBuffer_32(TempCrc,0x06,SendTempBuffer);   
  	}    
//-------------------------------------------  	    
  	 TempCrc=CRC(SendTempBuffer,6);
  	 SendTempBuffer[6]=TempCrc/256;  //High crc
  	 SendTempBuffer[7]=TempCrc%256;  //Low crc
  	 if(SendTempBuffer[0]!=0) 
  	    
  	    SendDataToBus1(SendTempBuffer,8);  
  	
  	}




