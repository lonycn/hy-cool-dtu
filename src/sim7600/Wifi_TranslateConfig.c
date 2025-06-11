#include "sys.h"
#include "stm32f10x_flash.h"
#include "device.h"
#include "string.h"
#include<stdlib.h>
#include<stdio.h>
#define CommonH
#include "Var.h"
#include "math.h"
//定义常量 (以STM32F051R8为例)
#include "global.h"
void  Mqtt_main();
unsigned char  MqttInitial() ;
void  CombinGpsAutoSend(unsigned char Interval,unsigned char  *SubTopic);
unsigned char MqttSendAndReceive(unsigned char *Sendbuffer,unsigned char TimeOut,unsigned char * Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len);
unsigned char PubMessage(unsigned char Clientid,unsigned char len,unsigned char *Sendbuffer);
void SetPubPayload(unsigned char Clientid,unsigned char  len,unsigned char  *SubTopic);
void SetPubTopic(unsigned char Clientid,unsigned char  *SubTopic);
void SetSubTopic(unsigned char Clientid,unsigned char  *SubTopic);
void CombineClient(unsigned char Clientid,unsigned char  *ClientInfor);
void  CombineSeverInfor(unsigned char Clientid,unsigned char  *ServerInfor);
void DealWithMqttPoroctol();
void DealWithGpsPoroctol();
void  DealSim7600Data();
void GetGpsData();
void DealWithMqttAndGpsReceiveData();
void ConverStatusRegisterToConfigUnit();
void  ConvertConfigDatToStatusRegister();
unsigned char DealWithError(unsigned char *Machstring);
unsigned char WaitReceiveResult(unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len);
unsigned char GetGps[]="AT+CGPSINFO";
unsigned char Topic_GPS[]="+CGPSINFO: ";
unsigned char  Mqtt_sub_Start[]="+CMQTTRXSTART:";  //接收信息
unsigned char  Mqtt_sub_Topic[]="+CMQTTRXTOPIC:";  //接受到topic信息
                                                               //topic 内容
unsigned char  Mqtt_sub_Payload[]="+CMQTTRXPAYLOAD: ";  //接受到到payload信息
                                                               //接受到到payload内容    
unsigned char  Mqtt_sub_End[]="+CMQTTRXEND:";  //接收完成
#define GpsUsePort   1
/*----------------------------------------------
Function:Mqtt_main()
Input:  None
Output: None
Description: 1:初始化
             2:每3s发布一次
             3:处理接收到的gps信号和订阅信号
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
   
//--------------------mqtt---------------   
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
void Mqtt_Delay_Sec_Timer_250ms()   //放在250ms的中断中
   { 
  	MqttSendDelay.Delayms1++;
     if(MqttSendDelay.Delayms1>=4)
        {MqttSendDelay.Delayms1=0;
   	    MqttSendDelay.DelayCount--;
   	    if(MqttSendDelay.DelayCount==0)
  	       MqttSendDelay.DelaysOver=1;
  	 } 
   }
      
//-------------can send delay----------------------   
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
  
  
//-------------half second----------------------   
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
   

  
   
   
#define  cSendParaNum  34   

void PubMessageToServer()
 {
   unsigned char  i, Temp[160]="AA55030001010442";
   
   uint16_t TempCrc;	
     Temp[16]=0X01;
     Temp[17]=0X04;
     Temp[18]=cSendParaNum*2;
     for(i=0;i<cSendParaNum*2;i++)
      	Temp[19+i]=LocalConfig.varList[i];
      TempCrc=CRCcount(&Temp[16],cSendParaNum*2+3); 
     
     Temp[14]=(cSendParaNum*2)/16+0x30;
     if(Temp[14]>0x39)
        Temp[14]=Temp[14]+'A'-0X3A;
     
     Temp[15]=(cSendParaNum*2)%16+0x30;
     if(Temp[15]>0x39)
        Temp[15]=Temp[15]+'A'-0X3A;   
     
     for(i=0;i<cSendParaNum*2;i++)
       {	
       	          Temp[16+i*2]=LocalConfig.varList[i]/16+0X30;
       	          if(Temp[16+i*2]>0x39)
       	             Temp[16+i*2]=Temp[16+i*2]+'A'-0X3A;
       	           
                Temp[16+i*2+1]=LocalConfig.varList[i]%16+0X30;
                if(Temp[16+i*2+1]>0x39)
       	             Temp[16+i*2+1]=Temp[16+i*2+1]+'A'-0X3A;
        }  
       Temp[16+cSendParaNum*4]=TempCrc/256/16+0X30;
       if(Temp[16+cSendParaNum*4]>0x39)
       	             Temp[16+cSendParaNum*4]=Temp[16+cSendParaNum*4]+'A'-0X3A;
     	Temp[16+cSendParaNum*4+1]=TempCrc/256%16+0X30;
     	if(Temp[16+cSendParaNum*4+1]>0x39)
       	             Temp[16+cSendParaNum*4+1]=Temp[16+cSendParaNum*4+1]+'A'-0X3A;
     	Temp[16+cSendParaNum*4+2]=(TempCrc%256)/16+0X30;
     	if(Temp[16+cSendParaNum*4+2]>0x39)
       	             Temp[16+cSendParaNum*4+2]=Temp[16+cSendParaNum*4+2]+'A'-0X3A;
     	Temp[16+cSendParaNum*4+3]=(TempCrc%256)%16+0X30;
     	if(Temp[16+cSendParaNum*4+3]>0x39)
       	             Temp[16+cSendParaNum*4+3]=Temp[16+cSendParaNum*4+3]+'A'-0X3A;
     	PubMessage(0,(cSendParaNum*2+10)*2,Temp);
  }




void  PowerCheck()
 {         
 	   if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0) //外部供电
     	      LocalConfig.varList[cGpsPosition-1]&=0xfe;
     	     else
     	       LocalConfig.varList[cGpsPosition-1]|=0x01;
     	    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0) //ACC ON  预留
     	      LocalConfig.varList[cGpsPosition-1]&=0XEF;   //BIT4
     	     else
     	       LocalConfig.varList[cGpsPosition-1]|=0x10;  //ACC OFF 
 	
 	
 }	


void MagicDoorCheck()
   {
   	
   	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0) //ALARM 1 门磁使用
     	      LocalConfig.varList[cGpsPosition-1]&=0xf7;    //BIT3
     	     else
     	       LocalConfig.varList[cGpsPosition-1]|=0x08;
     	    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==0) //ALARM2  bit5  预留
     	      LocalConfig.varList[cGpsPosition-1]&=0XDF;   //BIT4
     	     else
     	       LocalConfig.varList[cGpsPosition-1]|=0x20;  // 
   	

   } 

   
void CanToDevice()
 {    if(CanSend_CheckDelaySecond()==1)
 	  {
 	  	CanSend_SetDelaymS(200);
 		LocalConfig.ConfigUint[4]=0x03;
                Can_Send_Msg(LocalConfig.ConfigUint,8);   	
          }	
 }	   
void  DealSensor()
  {
         LocalConfig.SensorNumber++;
     	 
     	 ModbusRequest(0,LocalConfig.SensorNumber,0x04,0,2);
     	 delay_ms(50);
     	 DealWith_ModbusRespond(0,LocalConfig.SensorNumber);
     	 if(LocalConfig.SensorNumber>5)
     	   LocalConfig.SensorNumber=0; 
     	 
  } 
   
void  Mqtt_main()
 {  
    MqttInitial();
    Mqtt_SetDelayS(3);	 
    LocalConfig.SensorNumber=0;
    CanSend_SetDelaymS(200);
    ConverStatusRegisterToConfigUnit(); //add 2020-09-09 12:13
    while(1)
     {   
         
 	 DealWithMqttAndGpsReceiveData();
  	 CanToDevice();
     	 DealSensor();
     	if(Mqtt_CheckDelaySecond()==1)
     	  {
     	     GetAdc();
     	     Mqtt_SetDelayS(3);
     	     PubMessageToServer();
     	     ReverseWorkLed();
     	    PowerCheck();
     	    MagicDoorCheck();
     	  } 
     	TranslateData(1,4);    
     	
     }	
}

#define cMqttWaitTime  50
unsigned char sim7600_at[]="AT\r\n";
unsigned char sim7600_FUN[]="AT+CFUN=1\r\n";
unsigned char sim7600_gd[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";
unsigned char sim7600_mode[]="AT+CIPMODE=1\r\n";
unsigned char sim7600_net[]="AT+NETOPEN\r\n";
unsigned char sim7600_ccid[]="AT+CICCID\r\n";
unsigned char sim7600_CSQ[]="AT+CSQ\r\n";
unsigned char sim7600_reg[]="AT+CREG?\r\n";


/*----------------------------------------------
Function: unsigned char MqttSendAndReceive(unsigned char *Sendbuffer,unsigned char TimeOut,unsigned char Matchstring1,unsigned char Matchstring2,unsigned char Matchstring3,unsigned char mode,unsigned char len)
Input:  unsigned char *Sendbuffer:发送的字符串
        unsigned char TimeOut:等待的时间 单位s
        unsigned char Matchstring1:返回数据的匹配字符串1
        unsigned char Matchstring2:返回数据的匹配字符串2
        unsigned char Matchstring3:返回数据的匹配字符串2
        unsigned char mode: 1:字符串  2:长度
        unsigned char len:  模式为2时设置的长度
Output: 0:超时  1-3:匹配的结果
Description: 发送消息,在指定的时间内等待返回匹配情况
------------------------------------------------*/  
unsigned char  SIM7600CEInitial() 
 {// unsigned char i,j,m=0;
	 unsigned char  TempChar[100],i,j,m=0;
  //start

  i=WaitReceiveResult(cMqttWaitTime,"RDY","IGNORE","IGNORE",0,0);
  
  if(i==1)
   {
    m++;
    i=WaitReceiveResult(cMqttWaitTime,"+CPIN: READY","IGNORE","IGNORE",0,0);
   }
  if(i==1)
   {
    m++;    
   i=WaitReceiveResult(cMqttWaitTime,"SMS DONE","IGNORE","IGNORE",0,0);
  }
  if(i==1)
  { m++;    
    i=WaitReceiveResult(cMqttWaitTime,"PB DONE","IGNORE","IGNORE",0,0);
  }
  if(i==1) //at
  {m++;
    TranslateData(1,4);   
    ClearUart2Buff();
   i=MqttSendAndReceive(sim7600_at,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
  }
   if(i==1)   //at+fun
  {m++;
   TranslateData(1,4);   
   ClearUart2Buff(); 
   i= MqttSendAndReceive(sim7600_FUN,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0); 
  }   
   if(i==1)  //at+dg
  {m++;
     TranslateData(1,4);   
      ClearUart2Buff();   
   i= MqttSendAndReceive(sim7600_gd,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);    
  } 
 if(i==1)  //at+ipmode
  {m++;
       TranslateData(1,4);   
        ClearUart2Buff();
   i= MqttSendAndReceive(sim7600_mode,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);    
  } 
 if(i==1)  //at+opennet
  {m++;
  TranslateData(1,4);   
  ClearUart2Buff();
   i= MqttSendAndReceive(sim7600_net,cMqttWaitTime,"OK","Network is already opened","IGNORE",0,0);   
   if(i==0)
      i=WaitReceiveResult(cMqttWaitTime,"+NETOPEN: 0","Network is already opened","IGNORE",0,0);
   } 
//-----------------
   if(i!=0)  //at_ccid
    {
     m++;
    TranslateData(1,4);   
     ClearUart2Buff();   
     i= MqttSendAndReceive(sim7600_ccid,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);    
    }
//--------------------     
   if(i!=0)  //at+csq
    {  m++;
 	for(j=0;j<10;j++)
  	{    TranslateData(1,4);   
  	     ClearUart2Buff();
  	     i= MqttSendAndReceive(sim7600_CSQ,cMqttWaitTime,"+CSQ: 99,99","+CSQ: ","IGNORE",0,0);   
            if(i==1)
              delay_ms(1000);
            else if(i==2) 
              break;  
        } 
      if(j==10)
          i=0;    
     }   
//-----------------  
if(i!=0)    //at+reg
    {  m++;
      for(j=0;j<10;j++) 
       {  
 	TranslateData(1,4);   
 	ClearUart2Buff();
 	i=MqttSendAndReceive(sim7600_reg,cMqttWaitTime,"+CREG: 0,1","+CREG: 0,5","+CREG: 0,2",0,0); 
        if((i==1)||(i==2))
          break;
        else if(i==3)
          delay_ms(1000); 
      } 
    if(j==10)
      i=0;   
   } 
//-------------------------------------------   
 for(j=0;j<100;j++)   //quit positon
      TempChar[j] =0; 
     strcat(TempChar,"Inital sim7600 quit Positon:"); 
     TempChar[90]=m+0x30;  
     strcat(TempChar,&TempChar[90]);
     strcat(TempChar,"\r\n");
 Uart5SendString(TempChar,strlen(TempChar));   
   
 return(i);   
}    
                


unsigned char Mqtt_start[]="AT+CMQTTSTART\r\n";
unsigned char Mqtt_OK[]="+CMQTTSTART: 0";
/*----------------------------------------------
Function: unsigned char  MqttInitial() 
Input:  
Output:
Description: 初始化mqtt环境,从配置参数中获取配置数据
------------------------------------------------*/  
unsigned char  MqttInitial() 
 { unsigned char  TempChar[100],i,j,m=0;
  //start
    ClearUart2Buff();
    j=MqttSendAndReceive(Mqtt_start,cMqttWaitTime,Mqtt_OK,"IGNORE","IGNORE",0,0);
    
			
			if(j!=1)
        {
           j=WaitReceiveResult(cMqttWaitTime,Mqtt_OK,"IGNORE","IGNORE",0,0);
        }
//-------------------1---------		
  //client 0
    TranslateData(1,4);   
    ClearUart2Buff();
    if(j==1)
      { m++;
        for(i=0;i<100;i++)
          TempChar[i] =0; 	 
	 CombineClient(0,TempChar);
        j=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
      }
      
 //----------------2--------------     
 //client 1
 TranslateData(1,4);  
  ClearUart2Buff();
  if(j==1)  
    {  m++;
       for(i=0;i<100;i++)
      TempChar[i] =0; 
      CombineClient(1,TempChar);
       j=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
    }  
    
 //----------------3------------    
// client0 to server
TranslateData(1,4);  	
ClearUart2Buff();
   if(j==1)  
    {	m++;
	  for(i=0;i<100;i++)
      TempChar[i] =0; 
     CombineSeverInfor(0,TempChar);
     //Uart2SendString(TempChar,strlen(TempChar));Mqtt_OK,"IGNORE","IGNORE"
      j=MqttSendAndReceive(TempChar,cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
      if(j!=1)
        {
           j=WaitReceiveResult(cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
        }
    }  
//----------------------4---------------------      
//client 1 to server
TranslateData(1,4);  	
ClearUart2Buff();
    if(j==1)  
    {	  m++;  
	  for(i=0;i<100;i++)
      TempChar[i] =0; 
     CombineSeverInfor(1,TempChar);
     Uart2SendString(TempChar,strlen(TempChar));
     j=MqttSendAndReceive(TempChar,cMqttWaitTime,"+CMQTTCONNECT: 1,0","IGNORE","IGNORE",0,0);
      if(j!=1)
        {
           j=WaitReceiveResult(cMqttWaitTime,"+CMQTTCONNECT: 1,0","IGNORE","IGNORE",0,0);
        }
     }  
 //----------------------5------------------     
//set sub 0
TranslateData(1,4);  	
    ClearUart2Buff();
    if(j==1)
     {  m++;
	  for(i=0;i<100;i++)
      TempChar[i] =0; 
      SetSubTopic(0,TempChar);
       j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","IGNORE","IGNORE",0,0);
       if(j!=1)
         {
           j=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
         }
      }  
//------------------------6-------------------          	 
//subtopic0 
TranslateData(1,4);  	
ClearUart2Buff();
     if(j==1)
       { m++;
	for(i=0;i<100;i++)
         TempChar[i] =0; 
	strcat(TempChar,&LocalConfig.ParaList_Slave[SubTopic0]);
       	 strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
	
	j=MqttSendAndReceive(TempChar,cMqttWaitTime,"CMQTTSUB: 0,0","IGNORE","IGNORE",0,0);
	if(j!=1)
	 {
	    j=WaitReceiveResult(cMqttWaitTime,"CMQTTSUB: 0,0","IGNORE","IGNORE",0,0);
         }  	
    }	
#if 1
//----------------------7-----------------------------
//set sub 1
TranslateData(1,4);  	
ClearUart2Buff();
     if(j==1)
       { m++;
	 for(i=0;i<100;i++)
           TempChar[i] =0; 
        SetSubTopic(1,TempChar);
       j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","IGNORE","IGNORE",0,0); 
       if(j!=1)
	 {
	    j=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
         }
      }  
 //----------------------8------------------------       	
//subtopic1
TranslateData(1,4);  	
ClearUart2Buff();
   if(j==1)
       { m++;
	for(i=0;i<100;i++)
      TempChar[i] =0; 
	strcat(TempChar,&LocalConfig.ParaList_Slave[SubTopic1]);
       	strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
	j=MqttSendAndReceive(TempChar,cMqttWaitTime,"CMQTTSUB: 1,0","IGNORE","IGNORE",0,0); 
	if(j!=1)
	 {
	    j=WaitReceiveResult(cMqttWaitTime,"CMQTTSUB: 1,0","IGNORE","IGNORE",0,0);
         }
    }    
#endif 
//----------------------------9-------------------
 //gps start 
 if(j==1)
       { m++;     
        j=MqttSendAndReceive("AT+CGPS=1,1\r\n",cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
       }
       
       
//------------------------------10--------------------       
//gps auto send
#if 1
    if(j==1)
     { m++;
     for(i=0;i<100;i++)
      TempChar[i] =0; 
     if(LocalConfig.ParaList_Slave[sGpsAutoSendTime]!=0) 
      {  CombinGpsAutoSend(LocalConfig.ParaList_Slave[sGpsAutoSendTime],TempChar) ;  
         j=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
           
       } 
     } 
        
#endif 
//--------------------PRINT----------------- 
 for(i=0;i<100;i++)
      TempChar[i] =0; 
     strcat(TempChar,"Initial MQTT quit Positon:"); 
     TempChar[90]=m+0x30;  
     strcat(TempChar,&TempChar[90]);
     strcat(TempChar,"\r\n");
 Uart5SendString(TempChar,strlen(TempChar));

return(j);
      
}

/*----------------------------------------------
Function:void  CombinGpsAutoSend(unsigned char Interval,unsigned char  *SubTopic)
Input:  
Output:
Description: 配置gps自动发送数据的间隔时间,单位:秒
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
Function: unsigned char WaitReceiveResult(unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len)
Input:  
Output: 0:超时  1-3:匹配的结果
Description: 等待接收buffer中的匹配结果,用在命令发送之后的判断等待中
------------------------------------------------*/  
unsigned char WaitReceiveResult(unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len)
  {
	char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000];
        uint16_t i,Error;
  	
        SetDelayS(TimeOut);
	while(CheckDelaySecond()==0)
	  { 
	   if(LocalConfig.UART_RespondRead[1]!=LocalConfig.UART_Write[1])	
 	     {  TranslateData(1,4);   
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_RespondRead[1])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_RespondRead[1]+i)%UART_Bufferlength];
                            //只检测回来的gps数据和mqtt的订阅包数据
                ResBegin=strstr(TempBuffer,Matchstring1);
                ResEnd=strstr(TempBuffer,Matchstring2);
                ResEnd2=strstr(TempBuffer,Matchstring3);
	   	if(ResBegin != NULL)
			    {   TranslateData(1,4); 
			    	return(1);
	   	    	     }
	   	else if(ResEnd != NULL)
			    {   TranslateData(1,4); 
			    	return(2);
	   	    	     }    	     
	   	else if(ResEnd2 != NULL)
			    {   TranslateData(1,4); 
			    	return(3);
	   	    	     }        	     
	    }
	}
     TranslateData(1,4); 
     return(0); 
 }            

void StorePara()
 {
 	
 	
 }
 
void  SendOutPara()
 {
 	
 	 
 }


/*----------------------------------------------
Function: void  WaitSystemConfig()
Input:  
Output:
Description: 配置文件/读取配置
------------------------------------------------*/  
void  WaitSystemConfig()
 {
 
char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000];
        uint16_t i,Error;
  	if(LocalConfig.UART_RespondRead[4]!=LocalConfig.UART_Write[4])	
 	     {    
 	   	Error=(LocalConfig.UART_Write[4]+UART_Bufferlength-LocalConfig.UART_RespondRead[4])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[4][(LocalConfig.UART_RespondRead[4]+i)%UART_Bufferlength];
                            //只检测回来的gps数据和mqtt的订阅包数据
                ResBegin=strstr(TempBuffer,"Write EnterConfig:");
                ResEnd=strstr(ResBegin,"\r\n");
                ResEnd2=strstr(TempBuffer,"Read EnterConfig\r\n");
	   	if(ResBegin != NULL)
			    {   if(ResEnd != NULL)
			    	  {  StorePara();
			    	     Mqtt_SetDelayS(1); 
			    	  } 
	   	    	     }
	   	else if(ResEnd2 != NULL)
			    {     SendOutPara(); 
			    	  Mqtt_SetDelayS(1); 
	   	    	     }    	     
	   	 	     
	    }
	
 }	
        
 
 
/*----------------------------------------------
Function: MqttSendAndReceive(unsigned char *Sendbuffer,unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len)
Input:  mode:  0:发送字符串  1;发送len长度的数据    2:如果不匹配,但是接收到len字符串,退出
Output:
Description: 发送数据包,在指定的时间等待回复情况,如果收到回复,提出等待,否则等待时间到,返回0
------------------------------------------------*/  
unsigned char MqttSendAndReceive(unsigned char *Sendbuffer,unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len)
  {
	char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[1000],j;
        uint16_t i,Error;
        TranslateData(1,4); 
        LocalConfig.UART_RespondRead[1] =LocalConfig.UART_Write[1];
  	//ClearUart2Buff();
     	if(mode==0)
			{ i=strlen(Sendbuffer);
				Uart2SendString(Sendbuffer,i);//strlen(Sendbuffer));
			}
	else
      	              Uart2SendString(Sendbuffer,len);
        
        SetDelayS(TimeOut);
	while(CheckDelaySecond()==0)
	  { 
	   if(LocalConfig.UART_RespondRead[1]!=LocalConfig.UART_Write[1])	
 	     {  TranslateData(1,4);   
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_RespondRead[1])%UART_Bufferlength;
                for(i=0;i<1000;i++)
                              TempBuffer[i]=0;
                for(i=0;i<Error;i++)
                               TempBuffer[i]=LocalConfig.UART_BUF[1][(LocalConfig.UART_RespondRead[1]+i)%UART_Bufferlength];
                            //只检测回来的gps数据和mqtt的订阅包数据
                ResBegin=strstr(TempBuffer,Matchstring1);
                ResEnd=strstr(TempBuffer,Matchstring2);
                ResEnd2=strstr(TempBuffer,Matchstring3);
	   	if(ResBegin != NULL)
			    {   TranslateData(1,4); 
			    	return(1);
	   	    	     }
	   	else if(ResEnd != NULL)
			    {   TranslateData(1,4); 
			    	return(2);
	   	    	     }    	     
	   	else if(ResEnd2 != NULL)
			    {   TranslateData(1,4); 
			    	return(3);
	   	    	     }
	   	else 
	   	 {  if(mode==2)
	   	     {if(Error>len)     
	   	        {   
	   	         return(16); 	 
	   	        }
	   	     }                	     
	    }
	}
	}
     TranslateData(1,4); 
 	
	return(0); 
 }  
 
 
 
unsigned char  GetCsq()
 { unsigned char i; 
    i= MqttSendAndReceive(sim7600_CSQ,cMqttWaitTime,"+CSQ: ","IGNORE","IGNORE",0,0);   
    if(i==1)
        i=DealWithError("+CSQ: ");
     return(i);	
 }
           
/*----------------------------------------------
Function: void SetPayload(unsigned char Clientid,unsigned char Len)
Input:  Clientid:只能是0和1
Output:
Description: 组合要发送的payload信息,真正的payload要等到回复>后再发
------------------------------------------------*/  
unsigned char PubMessage(unsigned char Clientid,unsigned char len,unsigned char *Sendbuffer)
  {
     unsigned char  TempChar[100],i,j,m=0;
     for(i=0;i<100;i++)
       	TempChar[i]=0;
     SetPubTopic( Clientid,TempChar);
     
     j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","+CMQTTTOPIC: 0,14","+CMQTTTOPIC: 0,",0,0);
    if(j==0)
       { for(i=0;i<10;i++)
          { j=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
       	     if(j==1)
       	       break;
       	   }    
       	 if(i==10)
       	    {
       	       j=0;	    
       	    }
       }
     else if(j==2)
      {      delay_ms(1000);
             if(GetCsq()>10)
              {
	     G4PowerDisable();
	     delay_ms(1000);
	     G4PowerEnable();
	     delay_ms(5000);
	     SIM7600CEInitial() ;
	     MqttInitial();
	   }
          }
   else  if(j==3)
       {
         DealWithError("+CMQTTTOPIC: 0,");
         j=0; 
        } 	     
//-----------------   pub message  
    if(j==1)
      { m++;  	     
       for(i=0;i<100;i++)
       	TempChar[i]=0;
      if(Clientid==0) 	
       {	strcat(TempChar,&LocalConfig.ParaList_Slave[PubTopic0]);
       	        strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
       }
     else
      {	strcat(TempChar,&LocalConfig.ParaList_Slave[PubTopic1]);
       	        strcat(TempChar,&LocalConfig.ParaList_Slave[cIP]);
       }
      
       j=MqttSendAndReceive(TempChar,cMqttWaitTime,"OK","IGNORE","IGNORE",0,0);
      
      }
 //---------------pub content
   if(j==1)
     { m++;    
      for(i=0;i<100;i++)
       	   TempChar[i]=0;
         SetPubPayload(Clientid,len,TempChar);  
         j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","IGNORE","IGNORE",0,0);
         if(j==0)
          {
             for(i=0;i<10;i++)
              { j=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
       	         if(j==1)
       	            break;
       	       }    
       	     if(i==10)
       	       {
       	       j=0;	    
       	      }
          }  
      }
  if(j==1)
     {  m++;
     	        
         j=MqttSendAndReceive(Sendbuffer,cMqttWaitTime,"OK","IGNORE","IGNORE",1,len);
          if(Clientid==0)  
                	j=MqttSendAndReceive("AT+CMQTTPUB=0,1,180\r\n",cMqttWaitTime,"OK","IGNORE","IGNORE",0,len);
            else
                     j=MqttSendAndReceive("AT+CMQTTPUB=1,1,180\r\n",cMqttWaitTime,"OK","IGNORE","IGNORE",0,len);
     }
     for(i=0;i<100;i++)
        TempChar[i] =0; 
     strcat(TempChar,"publish quit Positon:"); 
     TempChar[90]=m+0x30;  
     strcat(TempChar,&TempChar[90]);
     strcat(TempChar,"\r\n");
     Uart5SendString(TempChar,strlen(TempChar));

return(j);
  }

/*----------------------------------------------
Function: void SetPayload(unsigned char Clientid,unsigned char Len)
Input:  Clientid:只能是0和1
Output:
Description: 组合要发送的payload信息,真正的payload要等到回复>后再发
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




/*----------------------------------------------
Function: void SetPubTopic(unsigned char Clientid,unsigned char  *SubTopic)
Input:  Clientid:只能是0和1
Output:
Description: 组合PUBtopic设置的内容,设置pub的topic
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
Input:  Clientid:只能是0和1
Output:
Description: 组合subtopic设置的内容
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
Input:  Clientid:只能是0和1
Output:
Description: 组合client信息
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
Input:   *wifiInfor:at命令buffer
Output:
Description:合成设置wifi 进入透传的AT命令
调用: DisplayWifiComand(unsigned char AT)
      SendWifiCommand(unsigned char AT)文件内调用
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

void DealPayload(unsigned char client,unsigned char len,unsigned char *Payload)
 {   unsigned char  TempChar[200],i;
     uint16_t Addr,Value,TempCrc;
            for(i=0;i<200;i++)
                  TempChar[i]=0;
                  strcat(TempChar,"MqttSub Payload:"); 
                  strcat(TempChar,Payload); 
 	 strcat(TempChar,"\r\n"); 
	  Uart5SendString(TempChar,strlen(TempChar));
 	// PubMessage(client,len,Payload);
 	// 转换为modbus hex格式
 	 for(i=0;i<len/2;i++)
 	   TempChar[i] =( Payload[i*2]/16>0x39?(Payload[i*2]/16-'A')*16:(Payload[i*2]/16-'0')*16)+( Payload[i*2+1]%16>0x39?(Payload[i*2+1]%16-'A'):(Payload[i*2+1]%16-'0'));
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
 	  	       	 case cFroozeTemp :  Value=Value+300;
 	  	       	                     LocalConfig.ConfigUint[0]=Value/10;
 	  	       	                     break;
 	  	       	case cCoolingTemp:                       
 	  	       	                      LocalConfig.ConfigUint[1]=Value/10;
 	  	       	                     break;
 	  	       	                     
 	  	       	case cDefrosterInterval:   
 	  	       	                      LocalConfig.ConfigUint[2]&=0x1f;
 	  	       	                      Value*=32;  //bit5-7
 	  	       	                      LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break;
 	  	       	case cDefrosterTemp:                       
 	  	       	                       LocalConfig.ConfigUint[2]&=0xf3;
 	  	       	                      Value*=4;  //bit2-4
 	  	       	                      LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break; 
 	  	       	                      	
 	  	       	case cDefrosterMode:             //bit1          
 	  	       	                       LocalConfig.ConfigUint[2]&=0xfd;
 	  	       	                       LocalConfig.ConfigUint[2]+= (unsigned char)Value*2;
 	  	       	                      break; 
 	  	       	                      
 	  	       	case cUseCoolRom: //bit 0  
 	  	       	                                    
 	  	       	                       LocalConfig.ConfigUint[2]&=0xfe;
 	  	       	                       LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break; 
 	  	       	                      
 	  	        case cExitDefrosterMode:  //bit7
 	  	                             	LocalConfig.ConfigUint[3]&=0x7f;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value;
 	  	       	                      break;
 	  	       	case cFroozeMinTemp:
 	  	       	                      // bit 5 -6
 	  	       	                        LocalConfig.ConfigUint[3]&=0x9f;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*32;                    
 	  	       	                        break;
 	  	       	case cAutoDefroozeTime:
 	  	       	                      // bit 3 -4
 	  	       	                        LocalConfig.ConfigUint[3]&=0xe7;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*8;                    
 	  	       	                        break;    
 	  	       	                        
 	  	       	case cAutoHotFunction:
 	  	       	                      // bit 2
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfb;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*4;                    
 	  	       	                        break;     
 	  	       	case cManualFroozeMode:
 	  	       	                      // bit 1
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfd;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*2;                    
 	  	       	                        break; 
 	  	       	case cOpenClose:        // bit 0                                              
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfe;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value;                    
 	  	       	                        break;
 	  	       	 default:break;
 	  	       	                                                                                                                     
 	  	       } 	
 	         }
              }
        	
 	//----------------------------------------------
 	if((TempChar[0]==0x01)&&(TempChar[1]==0x10))
 	  {      
 	  	 TempCrc=CRCcount(TempChar,TempChar[6]+7); 
 	  	 Addr=TempChar[6]+7;
 	  	 if(TempCrc==TempChar[Addr]*256+TempChar[Addr+1])
 	  	  {  PubMessage(client,16,Payload);
 	  	  	
 	  	      Addr=TempChar[2]*256+TempChar[3];
 	  	      Value=TempChar[4]*256+TempChar[5];
 	  	      for(i=0;i<Value;i++)
 	  	        {
 	  	       switch(Addr)
 	  	       {
 	  	       	 case cFroozeTemp :  Value=Value+300;
 	  	       	                     LocalConfig.ConfigUint[0]=Value/10;
 	  	       	                     break;
 	  	       	case cCoolingTemp:                       
 	  	       	                      LocalConfig.ConfigUint[1]=Value/10;
 	  	       	                     break;
 	  	       	                     
 	  	       	case cDefrosterInterval:   
 	  	       	                      LocalConfig.ConfigUint[2]&=0x1f;
 	  	       	                      Value*=32;  //bit5-7
 	  	       	                      LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break;
 	  	       	case cDefrosterTemp:                       
 	  	       	                       LocalConfig.ConfigUint[2]&=0xf3;
 	  	       	                      Value*=4;  //bit2-4
 	  	       	                      LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break; 
 	  	       	                      	
 	  	       	case cDefrosterMode:             //bit1          
 	  	       	                       LocalConfig.ConfigUint[2]&=0xfd;
 	  	       	                       LocalConfig.ConfigUint[2]+= (unsigned char)Value*2;
 	  	       	                      break; 
 	  	       	                      
 	  	       	case cUseCoolRom: //bit 0  
 	  	       	                                    
 	  	       	                       LocalConfig.ConfigUint[2]&=0xfe;
 	  	       	                       LocalConfig.ConfigUint[2]+= (unsigned char)Value;
 	  	       	                      break; 
 	  	       	                      
 	  	        case cExitDefrosterMode:  //bit7
 	  	                             	LocalConfig.ConfigUint[3]&=0x7f;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value;
 	  	       	                      break;
 	  	       	case cFroozeMinTemp:
 	  	       	                      // bit 5 -6
 	  	       	                        LocalConfig.ConfigUint[3]&=0x9f;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*32;                    
 	  	       	                        break;
 	  	       	case cAutoDefroozeTime:
 	  	       	                      // bit 3 -4
 	  	       	                        LocalConfig.ConfigUint[3]&=0xe7;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*8;                    
 	  	       	                        break;    
 	  	       	                        
 	  	       	case cAutoHotFunction:
 	  	       	                      // bit 2
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfb;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*4;                    
 	  	       	                        break;     
 	  	       	case cManualFroozeMode:
 	  	       	                      // bit 1
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfd;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value*2;                    
 	  	       	                        break; 
 	  	       	case cOpenClose:        // bit 0                                              
 	  	       	                        LocalConfig.ConfigUint[3]&=0xfe;
 	  	       	                        LocalConfig.ConfigUint[3]+= (unsigned char)Value;                    
 	  	       	                        break;
 	  	       	 default:break;
 	  	       	                                                                                                                     
 	  	       } 	
 	         }  
              }
      }
//ConvertConfigDatToStatusRegister();		
		}



void ConverStatusRegisterToConfigUnit()
 {
       LocalConfig.ConfigUint[3]=LocalConfig.varList[14]; 	
       LocalConfig.ConfigUint[2]= LocalConfig.varList[15];  	
       LocalConfig.ConfigUint[0]=((LocalConfig.varList[0]*256+LocalConfig.varList[0])+300)/10;
       LocalConfig.ConfigUint[1]=(LocalConfig.varList[0]*256+LocalConfig.varList[0])/10;
 }


void  ConvertConfigDatToStatusRegister()
 {   unsigned char   i=0;
 	;
 	LocalConfig.varList[i++]= (LocalConfig.ConfigUint[0]*10-300)/256; 	
        LocalConfig.varList[i++]= (LocalConfig.ConfigUint[0]*10-300)/256; 
 	LocalConfig.varList[i++]= LocalConfig.ConfigUint[1]*10/256; 	
        LocalConfig.varList[i++]= LocalConfig.ConfigUint[1]*10/256; 
 	
 	
 	LocalConfig.varList[14]=LocalConfig.ConfigUint[3]; 	
        LocalConfig.varList[15]=LocalConfig.ConfigUint[2];
        if((LocalConfig.ConfigUint[3]&0x01)==0x01)
         {
            LocalConfig.varList[13]|=0x03;	
         }
       else
        
            LocalConfig.varList[13]&=0xfc;	
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
	   	    	                 LocalConfig.UART_Read[GpsUsePort]=(LocalConfig.UART_Read[GpsUsePort]+ResEnd-TempBuffer)%UART_Bufferlength;
	   	                      } 
	   	                   }  
	   	                 }     
	   	               else  //没有找到结尾的信息,如果长度超出200byte,该数据帧为问题帧,跳过帧头
	   	                     {        if(strlen(TempBuffer)>200)
	   	                                  LocalConfig.UART_Read[GpsUsePort]=(LocalConfig.UART_Read[GpsUsePort]+strlen(Mqtt_sub_Start))%UART_Bufferlength;     
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
	   	                             LocalConfig.UART_Read[GpsUsePort]=(LocalConfig.UART_Read[GpsUsePort]+strlen(Topic_GPS))%UART_Bufferlength;     
                                  }

}

}




/*----------------------------------------------
Function:  void GetGpsData()
Input:     none
Output:
Description:  查询gps,或者让gps自动发送
------------------------------------------------*/
void GetGpsData()
 { unsigned char i,j;
         i=MqttSendAndReceive(GetGps,cMqttWaitTime,Topic_GPS,Mqtt_sub_Start,Mqtt_sub_End,0,0);
 }                 
 	


/*----------------------------------------------
Function:  void GetGpsData()
Input:     none
Output:
Description:  查询gps,或者让gps自动发送
------------------------------------------------*/ 	
void Reconnection()
 {
    unsigned char  TempChar[100],i,j;
  
 //----------------3------------    
// client0 to server
TranslateData(1,4);  	
ClearUart2Buff();
   for(i=0;i<100;i++)
      TempChar[i] =0; 
     CombineSeverInfor(0,TempChar);
      j=MqttSendAndReceive(TempChar,cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
      if(j!=1)
        {
           j=WaitReceiveResult(cMqttWaitTime,"+CMQTTCONNECT: 0,0","IGNORE","IGNORE",0,0);
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
 
 
          