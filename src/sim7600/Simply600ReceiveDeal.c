 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
//#define _GLOBAL_H
#include	"global.h"

#define  u8     unsigned char
#define  u16    uint16_t

unsigned char Topic_GPS[]="+CGPSINFO: ";
unsigned char  Mqtt_sub_Start[]="+CMQTTRXSTART:";  //接收信息
unsigned char  Mqtt_sub_Topic[]="+CMQTTRXTOPIC:";  //接受到topic信息
                                                               //topic 内容
unsigned char  Mqtt_sub_Payload[]="+CMQTTRXPAYLOAD: ";  //接受到到payload信息
                                                               //接受到到payload内容    
unsigned char  Mqtt_sub_End[]="+CMQTTRXEND:";  //接收完成

void StorePara();
void  SendOutPara();
void DealPayload(unsigned char client,unsigned char len,unsigned char *Payload);
void ConverStatusRegisterToConfigUnit();
void  ConvertConfigDatToStatusRegister();
void DealWithMqttPoroctol();
void DealWithGpsPoroctol();
void DealWithMqttAndGpsReceiveData();
void DealWithErrorNumber(unsigned char   ErrorNum);
unsigned char  DealWithError(unsigned char *Matchstring);
/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void StorePara()
 {
 	
 	
 }
/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	 
void  SendOutPara()
 {
 	
 	 
 }

unsigned char      cPosTab[19]={0,0,5,20,4,11,7,1,6,13,0,0,2,3,8,9,10,14,12}; //寄存器位置对应的参数编号
//unsigned char  cPosControl[22]={0,7,12,13,4,2,8,6,14,15,16,5, 18, 9,17,0, 0, 0, 0, 0,3};
//                                  1,2,  3,4,5,6,7,8, 9, 10,11,12,13,14,15,16,17,18,19,20   

unsigned char   RequreUpdate[]={"Update App Version"};                                      
/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void DealPayload(unsigned char client,unsigned char len,unsigned char *Payload)
 {   unsigned char  TempChar[200],i;
     u16 Addr,Value,TempCrc;
            for(i=0;i<200;i++)
                  TempChar[i]=0;
                  strcat(TempChar,"Mqtt Receive Payload:"); 
                  strcat(TempChar,Payload); 
 	     strcat(TempChar,"\r\n"); 
	    Uart5SendString(TempChar,strlen(TempChar));
	  
 	// PubMessage(client,len,Payload);
 	// 转换为modbus hex格式
 	//add 2020-11-08 17:54
 	  if(strstr(Payload,RequreUpdate)!=NULL)
 	   {
 	   	MqttUpdate(Payload);
 	
 	   }
 	else
 	  {
 	 for(i=0;i<len/2;i++)
 	   TempChar[i] =( Payload[i*2]>0x39?(Payload[i*2]-'A'+10)*16:(Payload[i*2]-'0')*16)+( Payload[i*2+1]>0x39?(Payload[i*2+1]-'A'+10):(Payload[i*2+1]-'0'));
      //----------------------------------------------
 	if((TempChar[0]==0x01)&&(TempChar[1]==0x06))
 	  {     
 	  	 TempCrc=CRCcount(TempChar,6); 
 	  	 if(TempCrc==TempChar[6]*256+TempChar[7])
 	  	  {  PubMessage(client,len,Payload);
 	  	  	
 	  	      Addr=TempChar[2]*256+TempChar[3];
 	  	      Value=TempChar[4]*256+TempChar[5];
 	  	      switch(Addr)
 	  	       {
 	  	       	 case cFroozeTemp :  Value=Value+400;
 	  	       	                     LocalConfig.ConfigUint[1]=Value/10;
 	  	       	                     LocalConfig.ConfigUint[0]=0x01;
 	  	       	                     for(i=0;i<8;i++)
 	  	       	                        LocalConfig.BakConfigUint[i]=LocalConfig.ConfigUint[i];
 	  	       	                     SendSetCommand();
 	  	       	                     break;
 	  	       	case cCoolingTemp:    Value=Value+400;                    
 	  	       	                      LocalConfig.ConfigUint[2]=Value/10;
 	  	       	                      LocalConfig.ConfigUint[0]=0x01;
 	  	       	                      for(i=0;i<8;i++)
 	  	       	                        LocalConfig.BakConfigUint[i]=LocalConfig.ConfigUint[i];
 	  	     	                      SendSetCommand();
 	  	       	                     break;
 	  	       	                  
 	  	       	case cManualFroozeMode:
 	  	       	                      
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfd;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*2;
 	  	       	                         LocalConfig.ConfigUint[0]=0x01;
 	  	       	                         for(i=0;i<8;i++)
 	  	       	                        LocalConfig.BakConfigUint[i]=LocalConfig.ConfigUint[i];
 	  	       	                        SendSetCommand();                    
 	  	       	                        break; 
 	  	       	case cOpenClose:        // bit 0                                              
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfe;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value;
 	  	       	                        LocalConfig.ConfigUint[0]=0x01;
 	  	       	                        for(i=0;i<8;i++)
 	  	       	                        LocalConfig.BakConfigUint[i]=LocalConfig.ConfigUint[i];
 	  	       	                        SendSetCommand();                       
 	  	       	                        break;                  
 	  	       	                  
 	  	       	case cFroozeMinTemp:        //冷冻温度下限  P1  
 	  	       	case cDefrosterMode:       //除霜模式 P4                   
 	  	       	case cDefrosterInterval:   //除霜间隔 P5
 	  	       	case cAutoDefroozeTime:     //自动除霜时间 P6
 	  	       	case cExitDefrosterMode:    //退出除霜模式  P7
 	  	       	case cUseCoolRom:           //冷藏室功能   P11
 	  	       	case cHotFunction:          //制热功能 P13
 	  	       	case cDefrosterTemp:        //手动除霜蒸发器温度 P20
 	  	       	
 	  	       	case cFroozeRoomTempError:  //p2 冷冻室控制温差
 	  	       	case cCoolRoomTempError:  //p3 冷冻室控制温差
 	  	       	case cEvaporateFanRunAfterDefroze: //p8 除霜结束后蒸发风机延时运行
 	  	       	case cEvaporateFanRunWhenIdle: //p9 待机时候蒸发风机运行方式
 	  	       	case cDuty: //冷藏室分配的时间  p10
 	  	       	case cTempSelect:  //温度传感器选择 p12
 	  	       	case cHorLVoltProtect: //高低压保护功能 P14
 	  	       	                      LocalConfig.ConfigUint[4]=cPosTab[(unsigned char)Addr];
 	  	       	                      LocalConfig.ConfigUint[5]=(unsigned char)Value;
 	  	       	                      LocalConfig.ConfigUint[0]=0x11;
 	  	       	                      for(i=0;i<8;i++)
 	  	       	                        LocalConfig.BakConfigUint[i]=LocalConfig.ConfigUint[i];
 	  	       	                      SendSetCommand();
 	  	       	                      break;
 	  	       	                    
 	  	       	 default:break;
 	  	       	                                                                                                                     
 	  	       } 
 	  	      
 	  //---------------------模拟状态,假定设置的值都正确接收,反馈在状态上,正式需要拿掉	       
 	  	#if  0   
 	  	   switch(Addr)
 	  	       {
 	  	       	 case cFroozeTemp :  LocalConfig.varList[0]=TempChar[4];
 	  	       	                     LocalConfig.varList[1]=TempChar[5];
 	  	       	                     break;
 	  	       	case cCoolingTemp:   LocalConfig.varList[2]=TempChar[4];
 	  	       	                     LocalConfig.varList[3]=TempChar[5];
 	  	       	                     break;
 	  	       	                  
 	  	       	case cManualFroozeMode:
 	  	       	                      LocalConfig.varList[15]&=0xfd;
 	  	       	                      if(TempChar[5]==1)
 	  	       	                        LocalConfig.varList[15]|=0x02;
 	  	       	                                     
 	  	       	                        break; 
 	  	       	case cOpenClose:        // bit 0                                              
 	  	       	                       LocalConfig.varList[15]&=0xfe;
 	  	       	                      if(TempChar[5]==1)
 	  	       	                        LocalConfig.varList[15]|=0x01;                    
 	  	       	                        break;                  
 	  	       
 	  	       	 default:break;
 	  	       	                                                                                                                     
 	  	       } 	    
 	  	   #endif
						 
						 
 	         }
              }
        	
 	//-----------------------0x10-----------------------
 	else if((TempChar[0]==0x01)&&(TempChar[1]==0x10))
 	  {      
 	  	 TempCrc=CRCcount(TempChar,TempChar[6]+7); 
 	  	 Addr=TempChar[6]+7;
 	  	 if(TempCrc==TempChar[Addr]*256+TempChar[Addr+1])
 	  	  {  PubMessage(client,16,Payload);
 	  	  	
 	  	      Addr=TempChar[2]*256+TempChar[3];
 	  	      Value=TempChar[4]*256+TempChar[5];
 	  	    
                  }
          }
        //-----------------------0x03-----------------------
 	else if((TempChar[0]==0x01)&&(TempChar[1]==0x03))
 	  {      
 	  	 TempCrc=CRCcount(TempChar,6); 
 	  	 //Addr=TempChar[6]+7;
 	  	 if(TempCrc==TempChar[6]*256+TempChar[7])
 	  	  { // PubMessage(client,16,Payload);
 	  	  	
 	  	      Addr=TempChar[2]*256+TempChar[3];
 	  	      Value=TempChar[4]*256+TempChar[5];
 	  	      PubLoopControlMessageToServer(Addr,Value);
                  }
          } 
        //-----------------------0x04-----------------------
 	else if((TempChar[0]==0x01)&&(TempChar[1]==0x04))
 	  {      
 	  	 TempCrc=CRCcount(TempChar,6); 
 	  	// Addr=TempChar[6]+7;
 	  	 if(TempCrc==TempChar[6]*256+TempChar[7])
 	  	  {  //PubMessage(client,16,Payload);
 	  	  	
 	  	      Addr=TempChar[2]*256+TempChar[3];
 	  	      Value=TempChar[4]*256+TempChar[5];
 	  	      PubLoopRunMessageToServer(Addr,Value);
 
 	  	    
                  }
          }    
          
 }
}

/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void ConverStatusRegisterToConfigUnit()
 {
       	
       LocalConfig.ConfigUint[1]=((LocalConfig.varList[0]*256+LocalConfig.varList[1])+400)/10;
       LocalConfig.ConfigUint[2]=((LocalConfig.varList[2]*256+LocalConfig.varList[3])+400)/10;
 }

/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void  ConvertConfigDatToStatusRegister()
 {   unsigned char   i=0;
 	
 	LocalConfig.varList[i++]= (LocalConfig.ConfigUint[1]*10-400)/256; 	
        LocalConfig.varList[i++]= (LocalConfig.ConfigUint[1]*10-400)%256; 
 	LocalConfig.varList[i++]= (LocalConfig.ConfigUint[2]*10-400)/256;  	
        LocalConfig.varList[i++]= (LocalConfig.ConfigUint[2]*10-400)%256;  
 	
 	
 }

/*----------------------------------------------
Function: void DealWithMqttPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出200byte长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void DealWithMqttPoroctol()
{ //unsigned char  TempBuffer1[1000];
   
  char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000];
        uint16_t i,Error;
  	if(LocalConfig.UART_Read[1]!=LocalConfig.UART_Write[1])	
 	     {    
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_Read[1])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_Read[1]+i)%UART_Bufferlength];
                           
                               ResBegin=strstr(TempBuffer,Mqtt_sub_Start); 
                               ResEnd=strstr(TempBuffer,Mqtt_sub_End);  //完整的mqtt信息
			        if(ResEnd != NULL)
			          {    
			           
			              ResBegin=strstr(TempBuffer,Mqtt_sub_Payload);
			              if(ResBegin!= NULL)
			              {   ResBegin += strlen(Mqtt_sub_Payload); //开始解析payload
			                 //---------client 
			                  ResEnd2=strstr(ResBegin,","); 
			                 if(ResBegin!=ResEnd2)
	   	                             MqttReceiveInfor.MqttTopicNum=TempBuffer[ResEnd2-1-TempBuffer]-0x30;
	   	    	             //---------payload len 
	   	    	              ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,"\r\n"); 
			             
			             for(i=0;i<6;i++)
			               MqttReceiveInfor.MqttLen[i]=0; 	
	   	                     if(ResBegin!=ResEnd2)
	   	                       {  memcpy(MqttReceiveInfor.MqttLen, ResBegin, ResEnd2-ResBegin); 
	   	    	                  MqttReceiveInfor.MqttLength=atoi(MqttReceiveInfor.MqttLen);
	   	    	               //--always get len  
	   	    	                 ResBegin=ResEnd2+2;  //begin of payload 
	   	    	                 for(i=0;i<200;i++)
	   	    	                   MqttReceiveInfor.MqttPayload[i]=0;
	   	    	                 for(i=0;i<MqttReceiveInfor.MqttLength;i++)
	   	    	                    MqttReceiveInfor.MqttPayload[i]=TempBuffer[ResBegin-TempBuffer+i];
	   	    	                 //---------------解析接收到的payload数据---------- 
	   	    	                 DealPayload(MqttReceiveInfor.MqttTopicNum,MqttReceiveInfor.MqttLength,MqttReceiveInfor.MqttPayload);  
	   	    	                 LocalConfig.UART_Read[1]=(LocalConfig.UART_Read[1]+ResEnd-TempBuffer)%UART_Bufferlength;
	   	                      } 
	   	                   }  
	   	                 }     
	   	               else  //没有找到结尾的信息,如果长度超出200byte,该数据帧为问题帧,跳过帧头
	   	                     {        if(strlen(TempBuffer)>200)
	   	                                  LocalConfig.UART_Read[1]=(LocalConfig.UART_Read[1]+strlen(Mqtt_sub_Start))%UART_Bufferlength;     
                                     }

                             }
													 }
/*----------------------------------------------
Function: void DealWithGpsPoroctol(unsigned char *TempBuffer);
Input:     unsigned char :*TempBuffer  需要进行判断的字符串,
Output:
Description:  根据,来区分各个数据帧,如果帧头有,数据长度超出常规的长度而解析不到数据帧尾,放弃帧头
------------------------------------------------*/	
void DealWithGpsPoroctol()
{ //unsigned char  TempBuffer1[1000];
  // uint16_t i=0;
  
   char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000],j;
        uint16_t i,Error;
        
	 union uu{ float x; 
         char p[4]}jingwei;
  	if(LocalConfig.UART_Read[1]!=LocalConfig.UART_Write[1])	
 	     {    
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_Read[1])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_Read[1]+i)%UART_Bufferlength];
               
                               ResBegin=strstr(TempBuffer,Topic_GPS);
                               ResEnd=strstr(ResBegin,"\r\n");  //完整的gps信息
			        if(ResEnd != NULL)
			          {    
			           //维度
			             ResBegin += strlen(Topic_GPS); //开始解析经纬度
			             ResEnd2=strstr(ResBegin,","); 
			             for(i=0;i<13;i++)
			               GpsInfor.Gps_lat[i]=0; 	
	   	                     if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_lat, ResBegin, ResEnd2-ResBegin); 
	   	    	            //---------南北纬  
	   	    	                 ResBegin=ResEnd2+1;
	   	    	                 ResEnd2=strstr(ResBegin,","); 
			                       GpsInfor.Gps_NS=0; 	
	   	                     if(ResBegin!=ResEnd2)
	   	                        memcpy(&GpsInfor.Gps_NS, ResEnd2-1,1); 
					 								 GpsInfor.Gps_NS=TempBuffer[ResEnd2-1-TempBuffer];
	   	    	             //---------经度 
	   	    	                ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,","); 
			             for(i=0;i<13;i++)
			               GpsInfor.Gps_longi[i]=0; 	
	   	                     if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_longi, ResBegin, ResEnd2-ResBegin); 
	   	    	              //---------东西经  
	   	    	                 ResBegin=ResEnd2+1;
	   	    	                 ResEnd2=strstr(ResBegin,","); 
			                      GpsInfor.Gps_EW=0; 	
	   	                     if(ResBegin!=ResEnd2)
					  memcpy(&GpsInfor.Gps_EW, ResEnd2-1,1); 								 { i=ResEnd2-1-TempBuffer;
	   	    	                 
													 }
	   	    	              //---------年月日
	   	    	                  ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,","); 
			             for(i=0;i<15;i++)
			               GpsInfor.Gps_gpsTime[i]=0; 	
	   	                     if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_gpsTime, ResBegin, ResEnd2-ResBegin);   
	   	    	              //------------时分秒
	   	    	              ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,","); 
			             if(ResBegin!=ResEnd2)
	   	                        memcpy(&GpsInfor.Gps_gpsTime[6], ResBegin, ResEnd2-ResBegin);  
	   	    	             //------------高度
	   	    	              ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,",");
	   	    	             for(i=0;i<6;i++)
			               GpsInfor.Gps_gpsHigh[i]=0;     
			             if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_gpsHigh, ResBegin, ResEnd2-ResBegin);      
	   	    	             //-------------速度   
	   	    	                ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,",");
	   	    	             for(i=0;i<6;i++)
			               GpsInfor.Gps_gpsSpeed[i]=0;     
			             if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_gpsSpeed, ResBegin, ResEnd2-ResBegin);      
	   	                   //-------------方向  
	   	    	                ResBegin=ResEnd2+1;  
	   	    	                ResEnd2=strstr(ResBegin,"\r\n");
	   	    	             for(i=0;i<6;i++)
			               GpsInfor.Gps_gpsDirect[i]=0;     
			             if(ResBegin!=ResEnd2)
	   	                        memcpy(GpsInfor.Gps_gpsDirect, ResBegin, ResEnd2-ResBegin);           
	   	                  //-------------转换完毕,更新指针位置
	   	                       GpsInfor.Gps_latitude=atof(GpsInfor.Gps_lat)/100;
	   	                       GpsInfor.Gps_longitude=atof(GpsInfor.Gps_longi)/100; 
	   	                       GpsInfor.Gps_altitude=atof(GpsInfor.Gps_gpsHigh);
	   	                       GpsInfor.Gps_Speed=atof(GpsInfor.Gps_gpsSpeed)*1.852; 
	   	                       GpsInfor.Gps_Direct=atof(GpsInfor.Gps_gpsDirect); 
	   	                       LocalConfig.UART_Read[1]=(LocalConfig.UART_Read[1]+ResEnd-TempBuffer)%UART_Bufferlength;
	   	                  
    
      
  if((GpsInfor.Gps_NS=='N')||(GpsInfor.Gps_NS=='S'))      
	{    if(GpsInfor.Gps_NS=='N')
             LocalConfig.varList[cGpsPosition-1]|=0x04;
				else
					   LocalConfig.varList[cGpsPosition-1]&=0xfb;
        if(GpsInfor.Gps_EW=='W')
             LocalConfig.varList[cGpsPosition-1]|=0x02; 
				 else
					   LocalConfig.varList[cGpsPosition-1]|=0xfd; 
	//经纬度
        jingwei.x=GpsInfor.Gps_longitude;
        for(j=0;j<4;j++)  
            LocalConfig.varList[cGpsPosition+j]=jingwei.p[3-j];
        
	jingwei.x=GpsInfor.Gps_latitude;
        for(j=0;j<4;j++)  
            LocalConfig.varList[cGpsPosition+4+j]=jingwei.p[3-j];
			 
         jingwei.x=GpsInfor.Gps_altitude;
        for(j=0;j<4;j++)  
            LocalConfig.varList[cGpsPosition+8+j]=jingwei.p[3-j];
        
  LocalConfig.varList[cGpsPosition+12]=(uint16_t)(GpsInfor.Gps_Speed*10)/256;    
	LocalConfig.varList[cGpsPosition+13]=(uint16_t)(GpsInfor.Gps_Speed*10)%256; 
	
	LocalConfig.varList[cGpsPosition+14]=(uint16_t)(GpsInfor.Gps_Direct*10)/256;    
	LocalConfig.varList[cGpsPosition+15]=(uint16_t)(GpsInfor.Gps_Direct*10)%256; 
	
	
	
	j=0;
	LocalConfig.varList[cTimeYear+0]=0;
	LocalConfig.varList[cTimeYear+2]=0;
	LocalConfig.varList[cTimeYear+4]=0;
	LocalConfig.varList[cTimeYear+6]=0;
	LocalConfig.varList[cTimeYear+8]=0;
	LocalConfig.varList[cTimeYear+10]=0;
	LocalConfig.varList[cTimeYear+5]=(GpsInfor.Gps_gpsTime[0]-0x30)*10+(GpsInfor.Gps_gpsTime[1]-0x30);
	LocalConfig.varList[cTimeYear+3]=(GpsInfor.Gps_gpsTime[2]-0x30)*10+(GpsInfor.Gps_gpsTime[3]-0x30);
	LocalConfig.varList[cTimeYear+1]=(GpsInfor.Gps_gpsTime[4]-0x30)*10+(GpsInfor.Gps_gpsTime[5]-0x30);
	LocalConfig.varList[cTimeYear+7]=(GpsInfor.Gps_gpsTime[6]-0x30)*10+(GpsInfor.Gps_gpsTime[7]-0x30);
	LocalConfig.varList[cTimeYear+9]=(GpsInfor.Gps_gpsTime[8]-0x30)*10+(GpsInfor.Gps_gpsTime[9]-0x30);
	LocalConfig.varList[cTimeYear+11]=(GpsInfor.Gps_gpsTime[10]-0x30)*10+(GpsInfor.Gps_gpsTime[11]-0x30);
	  } 
	}
	   	               else  //没有找到结尾的信息,如果长度超出70byte,该数据帧为问题帧,跳过帧头
	   	                  {        if(strlen(TempBuffer)>70)
	   	                             LocalConfig.UART_Read[1]=(LocalConfig.UART_Read[1]+strlen(Topic_GPS))%UART_Bufferlength;     
                                  }

}

}


												
/*----------------------------------------------
Function:  void DealWithMqttAndGpsReceiveData()
Input:     none
Output:
Description:  1:查看是否有gps头或者mqtt sub头
              2:如果没有,将read指针移动write指针前面gps头与mqtt sub头长度值大值的位置,
              预防有帧头正好处于半接收状态
              3:如果只有一个帧头,处理对应的帧头
              4:如果有两个帧头,判断先后,处理前面的帧头后退出
              5:在退出该程序后,将command命令反馈帧的read设置为当前的write位置,后面如果有
              命令,则在处理命令的时候,从当前read开始判断
------------------------------------------------*/
void DealWithMqttAndGpsReceiveData()
 {

   char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000];
        uint16_t i,Error,Error1;
  	if(LocalConfig.UART_Read[1]!=LocalConfig.UART_Write[1])	
 	     {    
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_Read[1])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_Read[1]+i)%UART_Bufferlength];
                               ResEnd2=strstr(TempBuffer,"+CMQTTCONNLOST: 0,1");
                               ResBegin=strstr(TempBuffer,Topic_GPS);
                               ResEnd=strstr(TempBuffer,Mqtt_sub_Start);
                               if(ResEnd2!=NULL)
                                 {
                                    Reconnection();	
                                 } 	
                              else if((ResBegin!=NULL)&&(ResEnd!=NULL))
                                {
                                   if(ResBegin>ResEnd) //MQTT 在前面
                                      DealWithMqttPoroctol();
                                    else
                                     DealWithGpsPoroctol();	
                                }	
                              else if((ResBegin==NULL)&&(ResEnd!=NULL))
                                {
                                   
                                      DealWithMqttPoroctol();
                                   
                                }
                            else if((ResBegin!=NULL)&&(ResEnd==NULL))
                                {
                                   
                                      DealWithGpsPoroctol();
                                }      	  	
                            else
                               {  Error1=strlen(Topic_GPS)>strlen(Mqtt_sub_Start)?strlen(Topic_GPS): strlen(Mqtt_sub_Start);
                               	  if(Error>Error1)
                               	     LocalConfig.UART_Read[1]=(LocalConfig.UART_Read[1]+Error-Error1)%UART_Bufferlength;
                               }
                	      	                 
          }          
if(Mqtt_CheckDelaySecond()==1)
    LocalConfig.UART_RespondRead[1] =LocalConfig.UART_Write[1];
 }   



void DealWithErrorNumber(unsigned char   ErrorNum)
 {
   switch(ErrorNum)
    {
    	case 7:  //net open fail
    		 
        case 8:  //network close fail
        
        case 9:   break;//network not opened
        
        case 11:Reconnection(); break;//no connection
        
        case 14: break;//topic is busy
         
        default:break;
    }
}	
/*----------------------------------------------
Function: unsigned char DealWithError()
Input:  
Output: 0:超时  1-3:匹配的结果
Description: 接收到命令错误的处理结果
------------------------------------------------*/  
unsigned char  DealWithError(unsigned char *Matchstring)
 
  {
	char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[200],Temp[4],ErrorNum;
        uint16_t i,Error;
  	
           delay_ms(200);
	   if(LocalConfig.UART_RespondRead[1]!=LocalConfig.UART_Write[1])	
 	     {  TranslateData(1,4);   
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_RespondRead[1])%UART_Bufferlength;
                 for(i=0;i<200;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_RespondRead[1]+i)%UART_Bufferlength];
                           
                ResBegin=strstr(TempBuffer,Matchstring);
                ResEnd=strstr(ResBegin,"\r\n");
                if((ResEnd!=NULL)&&(ResBegin!=NULL))
                  {
                     ResBegin+=strlen(Matchstring);
                     for(i=0;i<4;i++)
                      Temp[i]=0;
                      memcpy(Temp, ResBegin,ResEnd-ResBegin); 
	   	       ErrorNum =atoi(Temp);
	   	       return(ErrorNum);
	   	      // if(ErrorNum==11)
	   	      //    Reconnection(); 
                  }   	
		} 
     }  
 

 
void strstrLen(unsigned char *buff,unsigned char *Matchstring,uint16_t bufflen,uint16_t Matchlen)
 {
    	
 	
 } 	
          