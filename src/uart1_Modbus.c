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


unsigned char CheckModbusRespond_uart1(unsigned char   *pp);
void uart1_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer );
void uart1_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer );
void uart1_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer );
void uart1_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer );


unsigned char CheckModbusRespond_uart3(unsigned char   *pp);
void uart3_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer );
void uart3_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer );
void uart3_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer );
void uart3_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer );
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


//add   2021-05-31 13:38
unsigned char CheckModbusRespond_uart1(unsigned char   *pp)
 {unsigned char  SetT[6];
  WatchdogReset(); 
  
//singleWrite  06-----------------------------------------------------                	 	
                  if(pp[1]==cComandWriteSingleUint)           //写单个寄存器06
               	 	   { uart1_DealWithSingleRegWrite_06(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Register  03-----------------------------------------------------                 
               	  else if(pp[1]==cComandReadHoldRegister)    //读保持寄存器03 
               	 	   { uart1_DealWithMoreRegRead_03(pp);
               	 	     return(cRightStatus);
               	 	    }
//Read  Variable  04-----------------------------------------------------                 	 	    
               	  else if(pp[1]==cComandReadDiRegister)      //读数据寄存器04 
               	 	   { uart1_DealWithMoreDiRead_04(pp);
               	 	     return(cRightStatus);
               	 	    }	    
//Read  Variable  16-----------------------------------------------------                 	 	    
               	  else if(pp[1]==0x10)     
               	 	   { uart1_DealWithMoreRegWrite_10(pp);
               	 	     return(cRightStatus);
               	 	    }	                  	 	    

//FunctionCode Wrong------------------------------------------------------
                 else
                    {
                    	 pp[0]=pp[1]+0x80; 
  	                   pp[1]=0x01;   //Functioncode wrong
  	    	             SendDataToBus1_uart0(pp,2);
                    	 return(cCrcWrong);
                    }
                }
      	     
          

//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求06写数据
//----------------------------------------------------------------
void uart1_DealWithSingleRegWrite_06(unsigned char  *SendTempBuffer )
 {uint16_t   i,j;
   uint16_t  TempCrc=0;
   int16_t   TEmpInter1,TEmpInter2;
	 int8_t SetT[7];
//   unsigned char   tempflash[2];
      for(i=0;i<8;i++)
  	  SendTempBuffer[i]=SendTempBuffer[i];
  	    
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //modified 2014-02-21 10:29
  	  i=TempCrc;
  	  
  	  
/*-------------偏差值设定，如果是设置内部的偏差值，需要在门变量=78的情况下才有效， 
               温度偏差允许的范围为正负300（30度），湿度为正负200（20%）
*/ 	 	
//add 2016-5-1 13:48

 if( i<2010)
   {
   //add 2021-04-18 11:22
   	
   //end of add 2021-04-18 11:22 
    if(	(i==cTempOffset)||(i==cTempOffset+1))  
    {
	#ifdef _Loramain	
   	 if((SendTempBuffer[0]>=AddrStart)
   	 &&(SendTempBuffer[0]<AddrStart+AddrLen)
   	 &&(  ParaList[(cLoraNodeAliveSet+SendTempBuffer[0]-AddrStart)*2+1]==1) 
   	
   	 )	
   	   {         ParaList[(i-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2]=SendTempBuffer[4];
  	             ParaList[(i-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2+1]=SendTempBuffer[5]; 
  	              Program();
   	   }
   	 #else 
   	              ParaList[i*2]=SendTempBuffer[4];
  	              ParaList[i*2+1]=SendTempBuffer[5]; 
		      Program();
   	#endif   
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
 //-------------------lora  读写	
 	else  if((i==cLoraReadConfigPara)&&(SendTempBuffer[5]==0x01))  //读取lora参数
  	  	{     
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5]; 
  	    	   ReadOutLoraPara();
  	         }
  	    else if  ((i==cLoraReadConfigPara)&&(SendTempBuffer[5]==0x00))  //退出读取
  	  	{    
					    
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	   SwtichToWorkmode();
  	         }   
  	    else if((i==cLoraWriteConfigPara)&&(SendTempBuffer[5]==0x01))  //设置lora参数
  	  	 {  
   					 ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	   WriteInLoraPara();
  	         } 
  	   else if((i==cLoraWriteConfigPara)&&(SendTempBuffer[5]==0x00))  //退出设置
  	  	 {     
					   ParaList[i*2]=SendTempBuffer[4];
  	         ParaList[i*2+1]=SendTempBuffer[5];
  	    	  SwtichToWorkmode();
  	         }
  	         
  //-----------修改设备地址	         
  	 else  if(i==0)  //修改设备地址
  	   	  {
  	   	  	
  	   	  	#ifndef  __flood 
  	   	   ParaList[cDeviceNum*2]=SendTempBuffer[4];
  	           ParaList[cDeviceNum*2+1]=SendTempBuffer[5]; 
  	          if((SendTempBuffer[4]==0)&&((SendTempBuffer[5]!=0)&&(SendTempBuffer[5]!=255)) )  //2016-01-18 21:19
  	              {   
     	          	ProgramEepromByte();
  	                 ReadEepromByte();
  	              }
  	            #endif   
  	           }    
  //--------------------进行初始化操作-------------------------
  	        else if(((i==26)||(i==cInitial))&&(SendTempBuffer[4]==0x00)&&(SendTempBuffer[5]==26))//initial
  	         { ResetFlag=0;  //2021-02-21 17:33
  	           InitialPara();
  	           BellOn();
  	           DelayMicroSeconds (1000);
  	           BellOff();
  	
		 }
    //--------------通过设置当前标准值，修正内部偏差，使当前的温湿度为当前的温度-------
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
  	      //---------------------------修改时间--------------------------    
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
  	   	       
  	//------------------------special adjust------10级error adjust------------------------------   	            
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
  	         {  
							#if   0 
							 LCD4_Clear();
                    LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"                ");
  	            W25QXX_Erase_Chip( );
                    SpiFlash_WaitReady();	
  	            LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"    Over        ");	
              #endif        
  	         } 	 
  	   else
  	   { 
  	      if((i!=cReordNumIndex)&& (i!=cCurrentReordNum)&& (i!=cCurrentReordPage)&&(i!=cPagePos))  //2015-09-19 13:59
  	   	      
  	   	               { ParaList[i*2]=SendTempBuffer[4];
  	                         ParaList[i*2+1]=SendTempBuffer[5]; 
  	                  #if  0     
												 if((i<cDO1)&& (i>cDI4))
  	                         {
  	                          ProgramEepromByte();
  	                          ReadEepromByte();
  	                         }
  	                        else
  	                         {
  	                           if((i>=cDO1)&& (i<=cDO4))	
  	                              { 
  	                              	#ifdef  __flood    
  	                              	 DigitalOutput(i-cDO1+1,SendTempBuffer[5]);	
  	                         	 ParaList[(cAutoStatusDO1+i-cDO1)*2]=0;
  	                                 ParaList[(cAutoStatusDO1+i-cDO1)*2+1]=0;
  	                                 ParaList[cRuntimeDO1*2]=0;
      	      	                         ParaList[cRuntimeDO1*2+1]=0;
  	                                 if(SendTempBuffer[5]==1)
  	                                    PumpBeginCount(i-cDO1+1);
																		 SendFloodStatus();
  	                                #endif    
  	                              }	
  	                        }
												#endif		 
  	     }
  	 }       
	 }               
/*设置的温湿度10级偏差校准值,设定的地址起始值,地址长度(最长是16个节点),lora参数 3000以上地址的处理*/
  	//3000---3019
else  if(i>=cTempOff)
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
		       if(StartBak==0)  //启动--->停止
         	        { 
                           VarList[cRealDataAct*2+1]=0x0ff;
                           VarList[cRealDataAct*2]=0x0ff; 
		         } 
		        else   //停止--->启动
		         History.AlarmStatus1=0x00; 
		     }
         	
  //-----------以下为参数发生改变后,即时处理相关对应的操作---------------------- 	 
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
   
    
	 #ifdef  __flood    
  	                         
					 SendFloodStatus();
	 #else
	   if(SendTempBuffer[0]!=0) 
  	
  	    SendDataToBus1_uart1(SendTempBuffer,8);
  	                                #endif    
	 
	 
}

//---------------------------------------------------------------
//Function:  void DealWithMoreRegRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求03读数据
//----------------------------------------------------------------
void uart1_DealWithMoreRegRead_03(unsigned char  *SendTempBuffer )
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
        SendTempBuffer1[0]=DeviceNum;
        SendTempBuffer1[1]=0x03; 
        SendTempBuffer1[2]=SendTempBuffer1[5]*2;   
  	if( SendTempBuffer1[5]<=ParaNum/2)  
  	  { 
  	    
  	    
//------------------<2010  	    
  	    if(TempCrc<cSaveDataFlag03D)
  	      { i=(unsigned char)TempCrc;
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
	
         TempCrc=CRC(SendTempBuffer1,j);
  	 SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	 SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart1(SendTempBuffer1,j);
  	 
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
  	           	  SendTempBuffer1[j++]=ParaList[(k-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2];// 温湿度位置+设备对应位置+偏差base位置
  	                  SendTempBuffer1[j++]=ParaList[(k-cTempOffset)*2+(SendTempBuffer[0]-AddrStart)*4+cLoraNodeOffset*2+1];
  	         }
              TempCrc=CRC(SendTempBuffer1,j);
  	      SendTempBuffer1[j++]=TempCrc/256;  //High crc
  	      SendTempBuffer1[j++]=TempCrc%256;  //Low crc
  	      SendDataToBus1_uart1(SendTempBuffer1,j);
            
           
           }   
  	  	
   		
   }
   
  } 	



//---------------------------------------------------------------
//Function:  void DealWithMoreDiRead()
//Input:    None
//Output:   None
//Description: 回复modbus请求04读数据
//----------------------------------------------------------------
void uart1_DealWithMoreDiRead_04(unsigned char  *SendTempBuffer )
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
       
        	 
  	 if(TempCrc<cModifiedTime) 
  	   { if((SendTempBuffer[0]!=DeviceNum)&&((SendTempBuffer[5]==3)||(SendTempBuffer[5]==2))
  	   &&(  ParaList[(cLoraNodeAliveSet+SendTempBuffer[0]-AddrStart)*2+1]==1) )  //read var len
  	        TempCrc+=cTempStartAddr+(SendTempBuffer[0]-AddrStart)*2;  //lora gateway
  	      else   
  	       TempCrc+=cTemp;
  	    }  
  	 else if(TempCrc>=cRealData)  //03d   rht
  	        {   TempCrc=TempCrc-cRealData+cRealDataAct;
  	           
  	        }  
  	 else if((TempCrc>=cModifiedTime)&&(TempCrc<cTempStartAddr))
  	        {   TempCrc-=cModifiedTime;  //03d time
  	           TempCrc+=2;
  	         }  
  	  i=(unsigned char)TempCrc;
  	      
  	     for(k=i;k<i+SendTempBuffer[2]/2;k++) 
  	      { WatchdogReset();
  	        SendTempBuffer[j++]=VarList[k*2];
  	        SendTempBuffer[j++]=VarList[k*2+1];
  	      }  
          TempCrc=CRC(SendTempBuffer,j);
  	  SendTempBuffer[j++]=TempCrc/256;  //High crc
  	  SendTempBuffer[j++]=TempCrc%256;  //Low crc
  	  SendDataToBus1_uart1(SendTempBuffer,j);	
 	 }
 
  
//---------------------------------------------------------------
//Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
//Input:    None
//Output:   None
//Description: 回复modbus请求0x10写数据
//----------------------------------------------------------------
void uart1_DealWithMoreRegWrite_10(unsigned char  *SendTempBuffer )
 {unsigned char i;
  unsigned char j,k;
  unsigned char  SetT[6];  //2015-10-21 5:41
   uint16_t  TempCrc=0;
    //  for(i=0;i<40;i++)
  //	  SendTempBuffer[i]=SendTempBufferBak[i];
  	    
  	  TempCrc=SendTempBuffer[2]*256;
          TempCrc+=SendTempBuffer[3];  //base addr
  	 if( SendTempBuffer[5]<cReceivelength/2)  
  	  { i=(unsigned char)TempCrc;
  	    j=7;
  	    
  	    if(TempCrc==cModifiedTime)
  	      
  	      {  
  	      	   SetT[0]=SendTempBuffer[j+5*2+1]/10*16+SendTempBuffer[j+5*2+1]%10;
  	           SetT[1]=SendTempBuffer[j+4*2+1]/10*16+SendTempBuffer[j+4*2+1]%10;
  	           SetT[2]=SendTempBuffer[j+3*2+1]/10*16+SendTempBuffer[j+3*2+1]%10;
  	           SetT[3]=SendTempBuffer[j+2*2+1]/10*16+SendTempBuffer[j+2*2+1]%10;
  	           SetT[5]=SendTempBuffer[j+1*2+1]/10*16+SendTempBuffer[j+1*2+1]%10;
  	           SetT[6]=SendTempBuffer[j+1]/10*16+SendTempBuffer[j+1]%10;  
  	      	   SetPCF8563(SetT);
  	      	  //add  2020-06-02 8:51 
  	      	 #if 0 
  	      	   RTC_Second=SendTempBuffer[j+5*2+1];
                   RTC_Minite=SendTempBuffer[j+4*2+1];
                   RTC_Hour=SendTempBuffer[j+3*2+1];
                   RTC_Day=SendTempBuffer[j+2*2+1];
                   RTC_Month=SendTempBuffer[j+1*2+1];
                   RTC_Year=SendTempBuffer[j+1];
                 #endif  
                   
                   //end of add  2020-06-02 8:52
  	      	 //end of add 2015-10-21 5:41   
  	      	 
  	     }
  	    else  if(TempCrc>=cSaveDataFlag03D)
  	      {  TempCrc-=cParaActual;
  	         i=(unsigned char)TempCrc;
  	         if(i+SendTempBuffer[5]<=cParaEndPos) //add 2016-5-1 15:49
  	          { for(k=i;k<i+SendTempBuffer[5];k++) 
  	            {
  	              ParaList[k*2]=SendTempBuffer[j++];	
  	              ParaList[k*2+1]=SendTempBuffer[j++];
  	             
  	           }
  	        //if start status change,we should clear the count of all time
		 if(StartBak!=ParaList[(cSaveDataFlag03D-cParaActual)*2+1])
         	 {
         	    StartBak=ParaList[(cSaveDataFlag03D-cParaActual)*2+1];
			
	         }
			//-----------------------------------    
	          DealAlarm(); 
                  ProgramEepromByte();
  	          ReadEepromByte();
  	        
  	      } 
  	     }   
  	
  	           
         TempCrc=CRC(SendTempBuffer,6);
  	 SendTempBuffer[6]=TempCrc/256;  //High crc
  	 SendTempBuffer[7]=TempCrc%256;  //Low crc
  	 if(SendTempBuffer[0]!=0) 
  	    
  	    SendDataToBus1_uart1(SendTempBuffer,8);  
  	
  	}
} 



