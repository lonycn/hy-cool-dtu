##include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
//#define _GLOBAL_H
#include	"global.h"

#define  u8     unsigned char
#define  u16    uint16_t
void  CombinGpsAutoSend(unsigned char Interval,unsigned char  *SubTopic);
void SetPubPayload(unsigned char Clientid,unsigned char  len,unsigned char  *SubTopic);
void SetPubTopic(unsigned char Clientid,unsigned char  *SubTopic);
void SetSubTopic(unsigned char Clientid,unsigned char  *SubTopic);
void CombineClient(unsigned char Clientid,unsigned char  *ClientInfor);
void  CombineSeverInfor(unsigned char Clientid,unsigned char  *ServerInfor);
/*----------------------------------------------
Function:void  CombinGpsAutoSend(unsigned char Interval,unsigned char  *SubTopic)
Input:  
Output:
Description: ����gps�Զ��������ݵļ��ʱ��,��λ:��
------------------------------------------------*/  
void  CombinGpsAutoSend(unsigned char Interval,unsigned char  *SubTopic)
  {
     unsigned char wifiComb1[]="AT+CGPSINFO=";
     
     unsigned char  TempChar[100],i;	
     
           strcat(SubTopic,wifiComb1);      
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", Interval);
            strcat(SubTopic,TempChar);
            strcat(SubTopic,"\r\n");
 }

/*----------------------------------------------
Function: void SetPayload(unsigned char Clientid,unsigned char Len)
Input:  Clientid:ֻ����0��1
Output:
Description: ���Ҫ���͵�payload��Ϣ,������payloadҪ�ȵ��ظ�>���ٷ�
------------------------------------------------*/  
void SetPubPayload(unsigned char Clientid,unsigned char  len,unsigned char  *SubTopic)
 {
   unsigned char wifiComb1[]="AT+CMQTTPAYLOAD=0,";
     unsigned char wifiComb2[]="AT+CMQTTPAYLOAD=1,";
     unsigned char  TempChar[100],i;	
      if(Clientid==0)
          strcat(SubTopic,wifiComb1);
      else
          strcat(SubTopic,wifiComb2);      
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", len);
            strcat(SubTopic,TempChar);
            strcat(SubTopic,"\r\n");
 }

void SetPubPayloadGetVersion(unsigned char  len,unsigned char  *SubTopic)
{
u8  GetVersion1[]="{\"DeviceId\":\"";
u8  GetVersion2[]="\",\"type\":\"QueryVersion\",\"Topic\":\"";
u8  GetVersion3[]="\"}"; //��ѯ�汾��Ϣ

strcat(SubTopic,GetVersion1);
strcat(SubTopic,&LocalConfig.ParaList_Slave[cIP]); //id
strcat(SubTopic,GetVersion2);
strcat(SubTopic,&LocalConfig.ParaList_Slave[PubTopic0]);
strcat(SubTopic,&LocalConfig.ParaList_Slave[cIP]); 
strcat(SubTopic,GetVersion3);
}


void SetPubPayDownload(uint16_t start,uint16_t  len,unsigned char  *GetVersion,unsigned char  *SubTopic)
{
u8  GetVersion1[]="{\"DeviceId\":\"";
u8  GetVersion2[]="\",\"type\":\"Download\",\"Start\":";
u8  GetVersion3[]=" ,\"Size\":";
u8  GetVersion4[]=" ,\"Topic\":\"";
u8  GetVersion5[]=",\"Version \":";
u8  GetVersion6[]="\"}"; //��ѯ�汾��Ϣ
unsigned char  TempChar[100],i;	
       
strcat(SubTopic,GetVersion1);
strcat(SubTopic,&LocalConfig.ParaList_Slave[cIP]); //���
strcat(SubTopic,GetVersion2);  //start
for(i=0;i<100;i++)
                  TempChar[i]=0;
sprintf(TempChar, "%d",start);
strcat(SubTopic,TempChar); 
strcat(SubTopic,GetVersion3);   //len
for(i=0;i<100;i++)
                  TempChar[i]=0;
sprintf(TempChar, "%d",len);
strcat(SubTopic,TempChar);
strcat(SubTopic,GetVersion4);   //topic
strcat(SubTopic,&LocalConfig.ParaList_Slave[PubTopic0]);
strcat(SubTopic,&LocalConfig.ParaList_Slave[cIP]); //���            strcat(SubTopic,"\r\n");
strcat(SubTopic,GetVersion5);  //version
strcat(SubTopic,GetVersion);
strcat(SubTopic,GetVersion6);

}
//"{\"DeviceId\":\"123\",\"type\":\"Download\",\"Start\":0,\"Size\":1024,\"Topic\": "random \",\"Version \":\"V3.1\"}"
//�����ļ�

//{"DeviceId":"123","type":"Download","Start":0,"Size":1024,"Topic": "random ","Version ":"V3.1"}"
//�����ļ�

/*----------------------------------------------
Function: void SetPubTopic(unsigned char Clientid,unsigned char  *SubTopic)
Input:  Clientid:ֻ����0��1
Output:
Description: ���PUBtopic���õ�����,����pub��topic
------------------------------------------------*/  
void SetPubTopic(unsigned char Clientid,unsigned char  *SubTopic)
 {
   unsigned char wifiComb1[]="AT+CMQTTTOPIC=0,";
     unsigned char wifiComb2[]="AT+CMQTTTOPIC=1,";
     unsigned char  TempChar[100],i;	
      if(Clientid==0)
         {   strcat(SubTopic,wifiComb1);
            
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d",strlen(&LocalConfig.ParaList_Slave[PubTopic0])+strlen(&LocalConfig.ParaList_Slave[cIP]));
            strcat(SubTopic,TempChar);
            strcat(SubTopic,"\r\n");
           	
        }
         else
         {   strcat(SubTopic,wifiComb2);
            
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", strlen(&LocalConfig.ParaList_Slave[PubTopic1])+strlen(&LocalConfig.ParaList_Slave[cIP]));
            strcat(SubTopic,TempChar);
            strcat(SubTopic,"\r\n");
           	
        }
 	
 }





/*----------------------------------------------
Function: void SetSubTopic(unsigned char Clientid,unsigned char  *SubTopic)
Input:  Clientid:ֻ����0��1
Output:
Description: ���subtopic���õ�����
------------------------------------------------*/  
void SetSubTopic(unsigned char Clientid,unsigned char  *SubTopic)
 {
   unsigned char wifiComb1[]="AT+CMQTTSUB=0,";
     unsigned char wifiComb2[]="AT+CMQTTSUB=1,";
     unsigned char  TempChar[100],i;	
      if(Clientid==0)
         {   strcat(SubTopic,wifiComb1);
            
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", strlen(&LocalConfig.ParaList_Slave[SubTopic0])+strlen(&LocalConfig.ParaList_Slave[cIP]));
            strcat(SubTopic,TempChar);
            strcat(SubTopic,",1\r\n");
           	
        }
         else
         {   strcat(SubTopic,wifiComb2);
            
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", strlen(&LocalConfig.ParaList_Slave[SubTopic1])+strlen(&LocalConfig.ParaList_Slave[cIP]));
            strcat(SubTopic,TempChar);
            strcat(SubTopic,",1\r\n");
           	
        }
 	
 }


/*----------------------------------------------
Function: CombineClient(unsigned char Clientid)
Input:  Clientid:ֻ����0��1
Output:
Description: ���client��Ϣ
------------------------------------------------*/  
void CombineClient(unsigned char Clientid,unsigned char  *ClientInfor)
{
     unsigned char wifiComb1[]="AT+CMQTTACCQ=0,\"";
     unsigned char wifiComb2[]="AT+CMQTTACCQ=1,\"";
     
      if(Clientid==0)
         {   strcat(ClientInfor,wifiComb1);
            strcat(ClientInfor,&LocalConfig.ParaList_Slave[cIP]);	
        }
         else
         {   strcat(ClientInfor,wifiComb2);
            strcat(ClientInfor,&LocalConfig.ParaList_Slave[cIP]);
            strcat(ClientInfor,"1");	
        }
       strcat(ClientInfor,"\"\r\n");
 }	
 	


			
/*----------------------------------------------
Function: void  CombineSeverInfor(unsigned char  *ServerInfor)
Input:   *wifiInfor:at����buffer
Output:
Description:�ϳ�����wifi ����͸����AT����
����: DisplayWifiComand(unsigned char AT)
      SendWifiCommand(unsigned char AT)�ļ��ڵ���
------------------------------------------------*/  
void  CombineSeverInfor(unsigned char Clientid,unsigned char  *ServerInfor)
  {
     unsigned char wifiComb1[]="AT+CMQTTCONNECT=0,\"tcp://";
     unsigned char wifiComb2[]="AT+CMQTTCONNECT=1,\"tcp://";
     unsigned char  TempChar[100],i;	
     
    
     if(Clientid==0)
            strcat(ServerInfor,wifiComb1);	
         else
            strcat(ServerInfor,wifiComb2);
            if(LocalConfig.ParaList_Slave[cUseIpOffset]==1)
               strcat(ServerInfor,&LocalConfig.ParaList_Slave[cDominOffset]);
             else
               strcat(ServerInfor,&LocalConfig.ParaList_Slave[cIP]);
             strcat(ServerInfor,":");
            //port 
             for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%s", &LocalConfig.ParaList_Slave[cPortOffset]);
            strcat(ServerInfor,TempChar);
            strcat(ServerInfor,"\",");
           //keep alive time 
            for(i=0;i<100;i++)
                  TempChar[i]=0;
            sprintf(TempChar, "%d", LocalConfig.ParaList_Slave[KeepAliveTime+1]*256+LocalConfig.ParaList_Slave[KeepAliveTime]);
            strcat(ServerInfor,TempChar);
            strcat(ServerInfor,",");
           //clean status
            if(LocalConfig.ParaList_Slave[CleanMqtt]==0)
             strcat(ServerInfor,"0,\"");
           else
             strcat(ServerInfor,"1,\""); 
          // user   
             strcat(ServerInfor,&LocalConfig.ParaList_Slave[User]);
             strcat(ServerInfor,"\",\""); 
           //pass
               strcat(ServerInfor,&LocalConfig.ParaList_Slave[Pass]);
             strcat(ServerInfor,"\"\r\n");
 }
