#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"



#define  Lora_M0   PB15
#define  Lora_M1   PB12 
#define  Lora1_M1   PB6

void LoraNodeInital()
 {
   LoraNodeWork=1;
   LoraNodeConfig=1;
 }
void  Timer_Offline()
{uint8_t  i;
 uint16_t temp1;
 
 for(i=0;i<AddrLen;i++)
  {
     WirelessNodeDelay[i]++;
     if(WirelessNodeDelay[i]>=ParaList[cLoraNodeInteral*2]*256+ParaList[cLoraNodeInteral*2+1])
       {
       	         temp1=(uint16_t)(-cInValidTemp);	
	         VarList[cTempStartAddr*2+i*4]=temp1/256;   //13
                 VarList[cTempStartAddr*2+i*4+1]=temp1%256;
                 VarList[cTempStartAddr*2+i*4+2]=temp1/256;   //13
                 VarList[cTempStartAddr*2+i*4+3]=temp1%256;
                 WirelessNodeDelay[i]=0;
				          VarList[cNodePower*2+i*4+0]=0xff;  //20211221
				          VarList[cNodePower*2+i*4+1]=0xff;  //20211221
				          VarList[cNodePower*2+i*4+2]=0xff;  //20211221
				          VarList[cNodePower*2+i*4+3]=0xff;  //20211221
       }        
   } 
}
void  RequireLoraPara( unsigned char  No)
  { unsigned char  Require[3]={0xc1,0x00,0x09};
  if(No==1)	
	 SendDataToBus1_uart1(Require,3);
  else
		SendDataToBus1_uart3(Require,3);
   }	


void  SendTempHumi()
 {
 	
 	uint16_t  TempCrc;
 	unsigned char SendBuff[20],i=0; 
  	SendBuff[i++]=0xaa;
  	SendBuff[i++]=0xff;
  	SendBuff[i++]=DeviceNum;
  	SendBuff[i++]=VarList1[cTemp1*2];
  	SendBuff[i++]=VarList1[cTemp1*2+1];
  	SendBuff[i++]=VarList1[cHumi1*2];
  	SendBuff[i++]=VarList1[cHumi1*2+1];
  	 TempCrc=CRC(SendBuff,i);
  	 SendBuff[i++]=TempCrc/256;  //High crc
  	 SendBuff[i++]=TempCrc%256;  //Low crc
  	 SendDataToBus1_uart1(SendBuff,i);
  	
  }	

void ConfigLoraPara( unsigned char  No)
  {
      unsigned char SendBuff[20],i=0; 
  	SendBuff[i++]=0xc0;
  	SendBuff[i++]=0x00;
  	SendBuff[i++]=0x09;
  	SendBuff[i++]=ParaList[cLoraAddr*2];
  	SendBuff[i++]=ParaList[cLoraAddr*2+1];
  	SendBuff[i++]=ParaList[cNetId*2+1];
  	SendBuff[i++]=ParaList[cLoraBaudrate*2+1]*32+ParaList[cLoraParity*2+1]*8+ParaList[cAirSpeed*2+1];
  	SendBuff[i++]=ParaList[cPackageLen*2+1]*64+ParaList[cRssiEanble*2+1]*32+ParaList[cTansPower*2+1];
  	SendBuff[i++]=ParaList[cChannel*2+1];
  	SendBuff[i++]=ParaList[cIssiByteEnable*2+1]*128+ParaList[cTranslateMode*2+1]*64+ParaList[cRelayFun*2+1]*32+ParaList[cLBTFun*2+1]*16+ParaList[cWorMode*2+1]*8+ParaList[cWorPieod*2+1];
  	
  	SendBuff[i++]=ParaList[cCyrpt*2];
  	SendBuff[i++]=ParaList[cCyrpt*2+1];
		if(No==1)
        SendDataToBus1_uart1(SendBuff,i);
     else if(No==2)
			 SendDataToBus1_uart3(SendBuff,i);
			ParaList[cLoraAddr*2+i]=255; ParaList[cLoraAddr*2]=0;
  }	
#if  0  
void ConfigLoraPara1()
  {
      unsigned char SendBuff[20],i=0; 
  	SendBuff[i++]=0xc0;
  	SendBuff[i++]=0x00;
  	SendBuff[i++]=0x09;
  	SendBuff[i++]=0;
  	SendBuff[i++]=0;
  	SendBuff[i++]=0;
  	SendBuff[i++]=0x62;
  	SendBuff[i++]=0x00;
  	SendBuff[i++]=0x0d;
  	SendBuff[i++]=0x03;
  	
  	SendBuff[i++]=0;
  	SendBuff[i++]=0;
        SendDataToBus1_uart1(SendBuff,i);

  }	
 #endif 
void  DealPara(uint8_t *Para)
  {  uint8_t i=0,j;
     uint8_t Parameter[9];
     for(i=0;i<9;i++)
        Parameter[i]=Para[i];
  	for(i=0;i<16;i++)
  	 {
  	   ParaList[(cLoraAddr+i)*2]=0;
  	ParaList[(cLoraAddr+i)*2+1]=0;	
  	 }
  	 i=0;	
  	
  	ParaList[cLoraAddr*2]=Parameter[i++];  //0
  	ParaList[cLoraAddr*2+1]=Parameter[i++];  //1
  	ParaList[cNetId*2+1]=Parameter[i++];   //2
  	j=(Parameter[i]>>5)&0x07;
  	ParaList[cLoraBaudrate*2+1]=j;   //3
  	j=(Parameter[i]>>3)&0x03;
		 ParaList[cLoraParity*2+1]=j;
		j=(Parameter[i++])&0x07; 
  	ParaList[cAirSpeed*2+1]=j;
  	
  	j=(Parameter[i]>>6)&0x03; 
  	ParaList[cPackageLen*2+1]=j;  //4
		 j=(Parameter[i]>>5)&0x01;
  	ParaList[cRssiEanble*2+1]=j;
		 j=Parameter[i++]&0x03;
		ParaList[cTansPower*2+1]=j;
  	
  	ParaList[cChannel*2+1]=Parameter[i++];  //5
  	j=(Parameter[i]>>7)&0x01;
  	ParaList[cIssiByteEnable*2+1]=j;
		j=(Parameter[i]>>6)&0x01;
  	ParaList[cTranslateMode*2+1]=j;
		j=(Parameter[i]>>5)&0x01;
  	ParaList[cRelayFun*2+1]=j;
		j=(Parameter[i]>>4)&0x01;
  	ParaList[cLBTFun*2+1]=j;
		j=(Parameter[i]>>3)&0x01;
  	ParaList[cWorMode*2+1]=j;
		j=Parameter[i++]&0x07;
  	ParaList[cWorPieod*2+1]=j;   //6
  	
  	ParaList[cCyrpt*2]=Parameter[i++];   //7
  	
  	ParaList[cCyrpt*2+1]=Parameter[i++];     //8                     
  }	 


	
	


void  ReadOutLoraPara( unsigned char  No)
 {
 	SwtichMode(cConfigMode,No);
 	RequireLoraPara(No);
    
  }
 

	
	
void WriteInLoraPara( unsigned char  No)
  {
     SwtichMode(cConfigMode,No);
     ConfigLoraPara(No);
  } 	 		