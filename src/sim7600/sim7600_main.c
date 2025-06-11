#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
//#define _GLOBAL_H
#include	"global.h"


#define  u8     unsigned char
#define  u16    uint16_t
	

void  Mqtt_main();

void CanToDevice();
void  DealSensor();
void  SetDelayS(uint16_t  Delay);
uint8_t CheckDelaySecond();
void Delay_Sec_Timer_500ms();
void  Mqtt_SetDelayS(uint16_t  Delay);
uint8_t Mqtt_CheckDelaySecond();
void Mqtt_Delay_Sec_Timer_250ms() ;  //����250ms���ж���
void  CanSend_SetDelaymS(uint16_t  Delay);
uint8_t CanSend_CheckDelaySecond();
void CanSend_Delay_Sec_Timer_1ms();
void  HalfSecond_SetDelayS(uint16_t  Delay);
uint8_t HalfSecond_CheckDelaySecond();
void HalfSecond_Delay_Sec_Timer();
void GetControlRunstatus();
void GetControlParastatus(u8 pos);
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/  
void  Mqtt_main()
 { 
}



/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/     
void CanToDevice()
 {      if(LocalConfig.CanSendOK==0)
         {
            LocalConfig.CanSendTime=0;
         }
      //-------------�յ����ô����Ӧ��,���·���һ��,���·��ʹ���>5,��ֹ����   
       else 
        {
         if(LocalConfig.CanSendOK==2) 
          { if(LocalConfig.CanSendTime<5)
            {
             LocalConfig.CanSendTime++;
             Can_Send_Msg(LocalConfig.BakConfigUint,8);   	
             CanSend_SetDelaymS(50);	
           }
          else
             LocalConfig.CanSendOK=0;
         }  
          else
           {
             
      //-------------------û���յ����õ�Ӧ��,��ʱ,���·���    	
 	if(CanSend_CheckDelaySecond()==1)
 	  {
 	  	if(LocalConfig.CanSendTime<5)
           {
             LocalConfig.CanSendTime++;
             Can_Send_Msg(LocalConfig.BakConfigUint,8);   	
             CanSend_SetDelaymS(50);	
           }
          else
           {
             LocalConfig.CanSendOK=0;
           	
           }	 	  	
          }
       //--------------------------------------------   	
   }
 }
} 
void SendSetCommand()
 {
   Can_Send_Msg(LocalConfig.ConfigUint,8);  	
   CanSend_SetDelaymS(50);
   LocalConfig.CanSendTime=0;
   LocalConfig.CanSendOK=1;
   	
 }	 
 
void GetControlRunstatus()
 { u8 Sendarray[8],i;
      for(i=0;i<8;i++)
       Sendarray[i]=0;
     Can_Send_Msg(Sendarray,8);   
   	 
 }
void GetControlParastatus(u8 pos)
 { u8 Sendarray[8],i;
       Sendarray[0]=0x10;
       Sendarray[4]=pos;
     Can_Send_Msg(Sendarray,8);   
   	 
 } 
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/   	   
void  DealSensor()
  {   u16  TEmpIN;
          TEmpIN=(u16)(-9999);
         LocalConfig.SensorNumber++;
     	 
     	 ModbusRequest(0,LocalConfig.SensorNumber,0x04,0,2);
     	 delay_ms(50);
     	 if(DealWith_ModbusRespond(0,LocalConfig.SensorNumber)==0)  //û�����ݻظ�
     	  {  LocalConfig.LoopCount[LocalConfig.SensorNumber]++;
     	     if(LocalConfig.LoopCount[LocalConfig.SensorNumber]>3)
     	      {
     	        LocalConfig.varList[cTemperaturePos*2+(LocalConfig.SensorNumber)*4+0]=TEmpIN/256;
     	        LocalConfig.varList[cTemperaturePos*2+(LocalConfig.SensorNumber)*4+1]=TEmpIN%256;
     	        LocalConfig.varList[cTemperaturePos*2+(LocalConfig.SensorNumber)*4+2]=TEmpIN/256;
     	        LocalConfig.varList[cTemperaturePos*2+(LocalConfig.SensorNumber)*4+3]=TEmpIN%256;
     	      }
     	  }    
     	 else
     	   {
     	   	 LocalConfig.LoopCount[LocalConfig.SensorNumber]=0;
     	   }		
     	 if(LocalConfig.SensorNumber>5)
     	   LocalConfig.SensorNumber=0; 
     	 
  } 

/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/  
void  SetDelayS(uint16_t  Delay)
  {
  	 DelaySecond.DelayCount=Delay;
  	 DelaySecond.DelaysOver=0;
  	 DelaySecond.Delayms1=0;
  }	

uint8_t CheckDelaySecond()
 {
	return(DelaySecond.DelaysOver);
 }

void Delay_Sec_Timer_500ms()
   { DelaySecond.Delayms1++;
     if(DelaySecond.Delayms1>=2)
        {DelaySecond.Delayms1=0;
   	    DelaySecond.DelayCount--;
   	    if(DelaySecond.DelayCount==0)
  	       DelaySecond.DelaysOver=1;
  	 }
   }
   
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/    
void  Mqtt_SetDelayS(uint16_t  Delay)
  {
  	 MqttSendDelay.DelayCount=Delay;
  	 MqttSendDelay.DelaysOver=0;
  	 MqttSendDelay.Delayms1=0;
  }	

uint8_t Mqtt_CheckDelaySecond()
 {
	return(MqttSendDelay.DelaysOver);
 }
void Mqtt_Delay_Sec_Timer_250ms()   //����250ms���ж���
   { 
  	MqttSendDelay.Delayms1++;
     if(MqttSendDelay.Delayms1>=4)
        {MqttSendDelay.Delayms1=0;
   	    MqttSendDelay.DelayCount--;
   	    if(MqttSendDelay.DelayCount==0)
  	       MqttSendDelay.DelaysOver=1;
  	 } 
   }
      
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/  
void  CanSend_SetDelaymS(uint16_t  Delay)
  {
  	 CanSendDelay.DelayCount=Delay;
  	 CanSendDelay.DelaysOver=0;
  	 CanSendDelay.Delayms1=0;
  }	

uint8_t CanSend_CheckDelaySecond()
 {
	return(CanSendDelay.DelaysOver);
 }
void CanSend_Delay_Sec_Timer_1ms()
   { 
  	CanSendDelay.Delayms1++;
     if(CanSendDelay.Delayms1>=4)
        {CanSendDelay.Delayms1=0;
   	    CanSendDelay.DelayCount--;
   	    if(CanSendDelay.DelayCount==0)
  	       CanSendDelay.DelaysOver=1;
  	 } 
   }  
  
  
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:��ʼ��
             2:ÿ3s����һ��
             3:������յ���gps�źźͶ����ź�
------------------------------------------------*/  
void  HalfSecond_SetDelayS(uint16_t  Delay)
  {
  	 DelayHalfSecond.DelayCount=Delay;
  	 DelayHalfSecond.DelaysOver=0;
  	 DelayHalfSecond.Delayms1=0;
  }	

uint8_t HalfSecond_CheckDelaySecond()
 {
	return(DelayHalfSecond.DelaysOver);
 }
void HalfSecond_Delay_Sec_Timer()
   { 
  	DelayHalfSecond.Delayms1++;
     if(DelayHalfSecond.Delayms1>=4)
        {DelayHalfSecond.Delayms1=0;
   	    DelayHalfSecond.DelayCount--;
   	    if(DelayHalfSecond.DelayCount==0)
  	       DelayHalfSecond.DelaysOver=1;
  	 } 
   }    
#include "md5.h"   
   #include "cJson.h"
void MqttUpdate()
{
    int16_t start = 0;
    char content[100];
    char recvBuf[1024+100];
    while(1)
    {
        
       SetSubTopic(0, "random");//��֤Ψһ
       SetPubTopic(0, "LwUpdateClient");
     //   SetPubPayDownload(start, 1024, "V3.1",content);
     //   PubMessage(0,strlen(content),content);
        
        
        //DealWithMqttPoroctol();//���޸�
        
        //recdMsg ���յ���mqtt��Ϣ����
        char* recdMsg= "{\"DeviceId\":\"123\",\"Version\":\"V3.1\",\"MD5\":\"24fc8a41c0afdfdcc38ec86b5b015c1b\",\"Size\":38""\0""CMAKE_MINIMUM_REQUIRED(VERSION 3.10)";
        cJSON* dst = cJSON_Parse(recdMsg);
        char* md5 = cJSON_GetObjectItem(dst, "MD5")->valuestring;
        char* payload = recdMsg+strlen(recdMsg);
        if(cJSON_GetObjectItem(dst, "Size") != NULL) {cJSON_Delete(dst);return;}
        int16_t payLoadLen= cJSON_GetObjectItem(dst, "Size")->valueint;//->valuedouble
        if(cJSON_GetObjectItem(dst, "Start")->valueint != start) {/*����*/};
        MD5_CTX mdContext;
        MD5Init (&mdContext);
        MD5Update (&mdContext, payload, payLoadLen);
        MD5Final (&mdContext);
        char md5_check[34] = {0};
        for(int16_t i = 0; i < 16; i++)
        {
            sprintf(&md5_check[i * 2], "%02x", mdContext.digest[i]);
        }
        //������ROM(payload, start/*ƫ��*/, size/*��С*/);
        if(payLoadLen < 1024 || memcmp(md5_check, md5, 32) !=0) {cJSON_Delete(dst);return;}
        start += payLoadLen;
    }
}


  
   
      