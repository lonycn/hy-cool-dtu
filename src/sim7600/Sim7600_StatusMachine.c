#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include "sim7600.h"
//#define _GLOBAL_H
#include	"global.h"

#define  u8     unsigned char
#define  u16    uint16_t

unsigned char  Sim7600Initial();
void  RequestToInitial();
unsigned char  Sim7600CanbeUsed();
void  sim7600_SetDelayS(uint16_t  Delay);
uint8_t sim7600_CheckDelaySecond();
void sim7600_Delay_Sec_Timer();
void Sim7600_StatusMachine();
unsigned char  GetCsq();

#define cInitial                  0
#define cWaitPowerOn              1
#define cWait7600PowerOn          2
#define cWaitCpin                 3 
#define cWaitSMS                  4
#define cWaitPB                   5
#define cWaitAT                   6
#define cWaitFun                  7
#define cWaitGD                   8
#define cWaitMode                 9
#define cWaitNet                  10
#define cWaitCCID                 11 
#define cWaitCSQ                  12 
#define cWaitREG                  13 
#define cWaitMQTTstart            14 
#define cWaitClient0              15           
#define cWaitClient1              16          
#define cWaitConnetServer0        17                 
#define cWaitConnetServer1        18                 
#define cWaitSubTopic0            19             
#define cWaitSubTopicContent0     20
#define cWaitSubTopic1            21             
#define cWaitSubTopicContent1     22
#define cWaitGpsStart             23
#define cWaitGpsAuto              24
#define cWaitInitialOk            25




unsigned char sim7600_at[]="AT\r\n";
unsigned char sim7600_FUN[]="AT+CFUN=1\r\n";
unsigned char sim7600_gd[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
unsigned char sim7600_mode[]="AT+CIPMODE=1\r\n";
unsigned char sim7600_net[]="AT+NETOPEN\r\n";
unsigned char sim7600_ccid[]="AT+CICCID\r\n";
unsigned char sim7600_CSQ[]="AT+CSQ\r\n";
unsigned char sim7600_reg[]="AT+CREG?\r\n";
unsigned char Mqtt_start[]="AT+CMQTTSTART\r\n";
unsigned char Mqtt_OK[]="+CMQTTSTART: 0";


struct sim7600mqtt // 位域
{
	unsigned char  status;
	unsigned char  ret;
	unsigned char  Count;
	
};  

struct DelayInfor_7600
  {    
   uint16_t         DelayCount;
   unsigned char 	 DelaysOver;
   unsigned char 	 Delayms1;
  
   
  } ;   

struct sim7600mqtt       Sim7600_Status;
struct  DelayInfor_7600  sim7600IntialDelay;


void SendOutputStatus(unsigned char m)
 {
   unsigned char  TempChar[8],j;
   for(j=0;j<8;j++)   //quit positon
      TempChar[j] =0; 
     Uart5SendString("Inital sim7600 quit Positon:",strlen("Inital sim7600 quit Positon:"));
     TempChar[0]=m/10+0x30;
	   TempChar[1]=m%10+0x30;
	   TempChar[2]=0;
     strcat(TempChar,"\r\n");
 Uart5SendString(TempChar,strlen(TempChar)); 
}
/*----------------------------------------------
Function: unsigned char  Sim7600CanbeUsed()
          
Input:  None
Output:   1:可以使用7600功能   2: 7600正在初始化
Description:  外部程序需要使用7600功能的时候,需要检查这个状态,以免冲突
CALL: 
------------------------------------------------*/  
unsigned char  Sim7600Initial()
 {
     Sim7600_Status.status=cInitial;
      sim7600IntialDelay.DelaysOver=0;
      sim7600IntialDelay.Delayms1=0;   	
 }
/*----------------------------------------------
Function: void  RequestToInitial()
          
Input:  None
Output:  None
Description:  外部程序需要使用7600初始化时调用
CALL: 
------------------------------------------------*/    
void  RequestToInitial()
 {
    Sim7600_Status.status=cInitial;
  	
 }



/*----------------------------------------------
Function: unsigned char  Sim7600CanbeUsed()
          
Input:  None
Output:   1:可以使用7600功能   2: 7600正在初始化
Description:  外部程序需要使用7600功能的时候,需要检查这个状态,以免冲突
CALL: 
------------------------------------------------*/   
unsigned char  Sim7600CanbeUsed()
 {
    if(Sim7600_Status.status==cWaitInitialOk)
      	return(1);
    else
       return(0);  	
 }

/*----------------------------------------------
Function: sim7600_SetDelayS(uint16_t  Delay)
          sim7600_CheckDelaySecond()
          sim7600_Delay_Sec_Timer()
Input:  None
Output:   
Description: 独立的定时处理程序
             需要设置定时:sim7600_SetDelayS(uint16_t  Delay)  单位:秒
             sim7600_Delay_Sec_Timer() 放到定时中断250ms中
             检查延时时间是否到:sim7600_CheckDelaySecond()=1,时间到
           如果在使用过程中,碰到错误的情况,或者需要重启的情况,只需要将Sim7600_Status.status设置为cInitial即可
------------------------------------------------*/  
void  sim7600_SetDelayS(uint16_t  Delay)
  {
  	 sim7600IntialDelay.DelayCount=Delay;
  	 sim7600IntialDelay.DelaysOver=0;
  	 sim7600IntialDelay.Delayms1=0;
  }	

uint8_t sim7600_CheckDelaySecond()
 {
	return(sim7600IntialDelay.DelaysOver);
 }
void sim7600_Delay_Sec_Timer()
   { 
  	sim7600IntialDelay.Delayms1++;
     if(sim7600IntialDelay.Delayms1>=4)
        {sim7600IntialDelay.Delayms1=0;
   	    sim7600IntialDelay.DelayCount--;
   	    if(sim7600IntialDelay.DelayCount==0)
  	       sim7600IntialDelay.DelaysOver=1;
  	 } 
   }    
   
/*----------------------------------------------
Function: void Sim7600_StatusMachine()
Input:  None
Output:   Sim7600_Status.status=cWaitInitialOk;时候,才能继续7600的mqtt功能的使用,
Description: 初始化7600及mqtt内容
           将本程序放置在while主循环中,作为一个线程使用
           如果在使用过程中,碰到错误的情况,或者需要重启的情况,只需要将Sim7600_Statuch.status设置为cInitial即可
------------------------------------------------*/  
void Sim7600_StatusMachine()
 {unsigned char  TempChar[100],i;
    switch(Sim7600_Status.status)
     {
        case  cInitial:  G4PowerDisable();
                         sim7600_SetDelayS(1);
                         Sim7600_Status.status=cWaitPowerOn;
                         SendOutputStatus(Sim7600_Status.status);
                         break;
                         
        case cWaitPowerOn:
                         if(sim7600_CheckDelaySecond()==1)                 
                          { Sim7600_Status.status=cWait7600PowerOn;
                          SendOutputStatus(Sim7600_Status.status);
                            G4PowerEnable();
                            ClearUart2Buff();
                            sim7600_SetDelayS(5);
                          }
                         WatchDog_feed();  
                         break;
        case  cWait7600PowerOn:
                          if(sim7600_CheckDelaySecond()==1)                 
                          {   if(WaitReceiveResult(cMqttWaitTime*3,"RDY","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               { 
                               	 sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitCpin;
                                  SendOutputStatus(Sim7600_Status.status);
                                }  
                          }
                          WatchDog_feed();                    
                        break;
              
        case  cWaitCpin:
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(WaitReceiveResult(cMqttWaitTime,"+CPIN: READY","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               { 
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitSMS;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               }
                           } 
                           WatchDog_feed();                   
                        break;
        case  cWaitSMS:
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(WaitReceiveResult(cMqttWaitTime,"SMS DONE","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitPB;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               } 
                          } 
                          WatchDog_feed();                   
                        break;                
         case  cWaitPB:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(WaitReceiveResult(cMqttWaitTime,"PB DONE","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitAT;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               }
                          } 
                          WatchDog_feed();                     
                        break;
                                        
       case  cWaitAT:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_at,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitFun;
                                //  Sim7600_Status.status=cWaitGD;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               }
                          }
                          WatchDog_feed();                     
                        break;
                        
     case  cWaitFun:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_FUN,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitGD;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               } 
                           }
                           WatchDog_feed();                   
                        break;
  
   case  cWaitGD:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_gd,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitMode;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               } 
                          }
                          WatchDog_feed();                    
                        break;
  
 case  cWaitMode:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_mode,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitNet;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               }
                           } 
                           WatchDog_feed();                   
                        break; 
 case  cWaitNet:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_net,cMqttWaitTime,"OK","Network is already opened","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitCCID;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               } 
                           }
                           WatchDog_feed();                   
                        break;  
                        
case  cWaitCCID:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive(sim7600_ccid,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitCSQ;
                                  Sim7600_Status.Count=0;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               }
                           }
                           WatchDog_feed();                    
                        break;                                                

case  cWaitCSQ:    
                        if(sim7600_CheckDelaySecond()==1)                 
                          {   Sim7600_Status.ret=MqttSendAndReceive(sim7600_CSQ,cMqttWaitTime*3,"+CSQ: 99,99","+CSQ: ","IGNORE",0,0);
                              if(Sim7600_Status.ret==1)
                                {       Sim7600_Status.Count++;
                                        if(Sim7600_Status.Count>10)
                                           Sim7600_Status.status=cInitial;
                                        else
                                            sim7600_SetDelayS(1);
                                  } 
                              else if(Sim7600_Status.ret==0)
                                { Sim7600_Status.Count++;
                                   if(Sim7600_Status.Count>3)
                                           Sim7600_Status.status=cInitial;
                                    else
                                            sim7600_SetDelayS(1);       
                                 }             
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                               	  Sim7600_Status.Count=0;
                                  Sim7600_Status.status=cWaitREG;
                                  SendOutputStatus(Sim7600_Status.status);
                                
                               } 
                          } 
                          WatchDog_feed();                   
                        break; 
                        
 case  cWaitREG:
                     if(sim7600_CheckDelaySecond()==1)                 
                          {   Sim7600_Status.ret=MqttSendAndReceive(sim7600_reg,cMqttWaitTime*50,"+CREG: 0,1","+CREG: 0,5","+CREG: 0,2",0,0);
                              if((Sim7600_Status.ret==1)||(Sim7600_Status.ret==2))
                                {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitMQTTstart;
                                  SendOutputStatus(Sim7600_Status.status);
                                  Sim7600_Status.Count=0;
                               }               
                              else
                                {       Sim7600_Status.Count++;
                                        if(Sim7600_Status.Count>5)
                                           Sim7600_Status.status=cInitial;
                                        else
                                            sim7600_SetDelayS(1);
                                 } 
                           }
                           WatchDog_feed();   
                        break;                 
 //------------------Begin  MQTT config-------------
case  cWaitMQTTstart:
                     if(sim7600_CheckDelaySecond()==1)                 
                          {   Sim7600_Status.ret=MqttSendAndReceive(Mqtt_start,cMqttWaitTime,Mqtt_OK,"IGNORE","IGNORE",0,0);
                              if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitClient0;
                                   SendOutputStatus(Sim7600_Status.status);
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial;
                                        else
                                          { 
                                	    Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,Mqtt_OK,"IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              { ClearUart2Buff();
                               	   		sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitClient0;
                                   		SendOutputStatus(Sim7600_Status.status);
                                              }
                                            else
                                            sim7600_SetDelayS(1);
                                          } 
                            }
                         } 
                         WatchDog_feed();     
                        break;   
 //client  0                       
case  cWaitClient0:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {     for(i=0;i<100;i++)
                                  TempChar[i] =0; 	 
	                        CombineClient(0,TempChar);
                          	Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
                              if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitClient1;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                          Sim7600_Status.status=cInitial;    
                                        else
                                          sim7600_SetDelayS(1);    
                                 }          
                                    
                         } 
                         WatchDog_feed();     
                        break;    
  //client1    
 case  cWaitClient1:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {     for(i=0;i<100;i++)
                                  TempChar[i] =0; 	 
	                        CombineClient(1,TempChar);
                          	Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
                              if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitConnetServer0;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           {  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitConnetServer0;
                                  
                               			}   
                                        else
                                          sim7600_SetDelayS(1);     
                                 }          
                                    
                         } 
                         WatchDog_feed();     
                        break;      
   
  //server 0
case  cWaitConnetServer0:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {      for(i=0;i<100;i++)
                                   TempChar[i] =0; 
                                  CombineSeverInfor(0,TempChar);
                              Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitConnetServer1;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial; 
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitConnetServer1;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                     }          
                                    
                             }
                         } 
                         WatchDog_feed();          
                        break;      
      


 //server 1
case  cWaitConnetServer1:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {      for(i=0;i<100;i++)
                                   TempChar[i] =0; 
                                  CombineSeverInfor(1,TempChar);
                              Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"+CMQTTCONNECT: 1,0","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitSubTopic0;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           {  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitSubTopic0;
                                  
                               		   }   
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,"+CMQTTCONNECT: 1,0","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitSubTopic0;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                 }          
                                    
                            } 
                        } 
                        WatchDog_feed();         
                        break;      


   

  //subtopic 0
case  cWaitSubTopic0:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {      
               
                          	
                          	for(i=0;i<100;i++)
                                   TempChar[i] =0; 
                                  SetSubTopic(0,TempChar);
                              Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,">","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitSubTopicContent0;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial; 
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitSubTopicContent0;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                       }          
                                    
                         }  
                      } 
                     WatchDog_feed();  
                   
                        break;      
 //topic content0     
case cWaitSubTopicContent0:
                if(sim7600_CheckDelaySecond()==1)                 
                          {      
                          	  for(i=0;i<100;i++)
      					TempChar[i] =0; 
     				 strcat(TempChar,&LocalConfig.ParaList_Slave[SubTopic0]);
       	                         strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
	                        Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"CMQTTSUB: 0,0","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                 {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitSubTopic1;
                                  
                                }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial; 
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,"CMQTTSUB: 0,0","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitSubTopic1;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                 }          
                                    
                           }     

                }
                WatchDog_feed(); 
             break;
               
  //subtopic1
case  cWaitSubTopic1:                        
               if(sim7600_CheckDelaySecond()==1)                 
                          {      
               
                          	
                          	for(i=0;i<100;i++)
                                   TempChar[i] =0; 
                                  SetSubTopic(1,TempChar);
                              Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,">","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitSubTopicContent1;
                                  
                               }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial; 
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitSubTopicContent1;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                       }          
                                    
                         }  
                      } 
                      
                   WatchDog_feed(); 
                        break;      
 //topic content0     
case cWaitSubTopicContent1:
                if(sim7600_CheckDelaySecond()==1)                 
                          {      
                          	  for(i=0;i<100;i++)
      					TempChar[i] =0; 
     				 strcat(TempChar,&LocalConfig.ParaList_Slave[SubTopic1]);
       	                         strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
       	                         strcat(TempChar,"1");
	                        Sim7600_Status.ret=MqttSendAndReceive(TempChar,cMqttWaitTime,"CMQTTSUB: 1,0","IGNORE","IGNORE",0,0);
                          	 if(Sim7600_Status.ret==1)
                                 {  ClearUart2Buff();
                               	   sim7600_SetDelayS(1);
                               	   Sim7600_Status.Count=0;
                                   Sim7600_Status.status=cWaitGpsStart;
                                  
                                }               
                              else
                                {       
                                	
                                	Sim7600_Status.Count++;
                                	if(Sim7600_Status.Count>2)
                                           Sim7600_Status.status=cInitial; 
                                        else
                                         {  Sim7600_Status.ret=WaitReceiveResult(cMqttWaitTime,"CMQTTSUB: 1,0","IGNORE","IGNORE",0,0);
                                            if(Sim7600_Status.ret==1)
                                              {
                                              	  ClearUart2Buff();
                               	  		 sim7600_SetDelayS(1);
                               	   		Sim7600_Status.Count=0;
                                   		Sim7600_Status.status=cWaitGpsStart;
                                              	
                                               }
                                            else  	
                                              sim7600_SetDelayS(1);     
                                 }          
                                    
                           }     

                }
                WatchDog_feed(); 
             break;


case cWaitGpsStart:
              if(sim7600_CheckDelaySecond()==1)                 
                          {   if(MqttSendAndReceive("AT+CGPS=1,1\r\n",cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                Sim7600_Status.status=cInitial;
                             else
                               {  ClearUart2Buff();
                               	  sim7600_SetDelayS(1);
                                  Sim7600_Status.status=cWaitGpsAuto;
                                
                               }
                           }  
                           WatchDog_feed();                  
                        break;
                        
case cWaitGpsAuto:
              if(sim7600_CheckDelaySecond()==1)                 
                          {  
                          	for(i=0;i<100;i++)
      					TempChar[i] =0; 
     				if(LocalConfig.ParaList_Slave[sGpsAutoSendTime]!=0) 
     					 {     CombinGpsAutoSend(LocalConfig.ParaList_Slave[sGpsAutoSendTime],TempChar) ;  
         					if(MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0)!=1)
                                                  Sim7600_Status.status=cInitial;
                                               else
                                                {  ClearUart2Buff();
                               	                   sim7600_SetDelayS(1);
                                                 Sim7600_Status.status=cWaitInitialOk;
                                    
                                              }
                                         } 
                                      else
                                       Sim7600_Status.status=cWaitInitialOk;    
                              }
                              WatchDog_feed();                              
                        break;                        
                        
  case cWaitInitialOk:break;
 } //end of case 
 
}//end of fun 


unsigned char  GetCsq()
 { unsigned char i; 
    i= MqttSendAndReceive(sim7600_CSQ,cMqttWaitTime,"+CSQ: ","IGNORE","IGNORE",0,0);   
    if(i==1)
        i=DealWithError("+CSQ: ");
     return(i);	
 }