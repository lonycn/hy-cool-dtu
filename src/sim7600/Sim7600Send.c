#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
//#define _GLOBAL_H
#include	"global.h"


#define  u8     unsigned char
#define  u16    uint16_t

#define  cSendParaNum  34  

void PubMessageToServer();
unsigned char WaitReceiveResult(unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len);
void  WaitSystemConfig();
unsigned char MqttSendAndReceive(unsigned char *Sendbuffer,unsigned char TimeOut,unsigned char *Matchstring1,unsigned char *Matchstring2,unsigned char *Matchstring3,unsigned char mode,unsigned char len);
unsigned char PubMessage(unsigned char Clientid,unsigned char len,unsigned char *Sendbuffer);
void GetGpsData();
void Reconnection();

void CompareControlChange();
void SendControlChange(u8 change);
void  SendChangeMessage(u8 change);
void CompareControlChange1()
 {
 	
}


/*----------------------------------------------
Function: void CompareControlChange()
Input:  
Output: 
Description:监测控制变量的变化,将变化的控制变量通过debug口打印出来
------------------------------------------------*/  
void CompareControlChange()
 { u8 i;
    for(i=0;i<19*2;i++)
     {
       if(LocalConfig.ControlPara[i]!=LocalConfig.ControlParaCompare[i])
         SendControlChange(i);
     }    		
    for(i=0;i<19*2;i++)	
      LocalConfig.ControlParaCompare[i]=LocalConfig.ControlPara[i];
 }      
 
void SendControlChange(u8 change)
 {float temp1,temp2;
     int16_t  inttemp;
    u8 i=change/2,j;
    u8   TempBuffer[100];
      for(j=0;j<100;j++)
        TempBuffer[j]=0;
     switch(i)
       {
       	  case  0:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\冷冻室设定温度:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	              temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n冷藏设定温度值由:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    					
       	             sprintf(TempBuffer,"\r\n\r\n自动除霜间隔:%f更改为 %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;
       	               
       	 case 3:    sprintf(TempBuffer,"\r\n\r\n手动除霜蒸发器温度:%f更改为 %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	                        
       	case 4:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜方式更改为:停机除霜方式\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜方式更改为:热气除霜方式\r\n\r\n") ; 	        
       	                break;
       case 5:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室功能更改为:无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室功能更改为:有效\r\n\r\n") ; 	        
       	                break; 
       	                
      case 6:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 退出除霜方式更改为:根据时间或温度退出\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n退出除霜方式更改为:根据时间退出\r\n\r\n") ; 	        
       	                break;  	                	             
       	          
     case 7:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷冻最低温度更改为:-30度\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷冻最低温度更改为:-5度\r\n\r\n") ; 	        
       	                break;  	                	             
     case 8:    
       	               	  sprintf(TempBuffer,"\r\n\r\n自动除霜时间:%d更改为 %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;    	            	          
       	          
    case 9:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 制热功能更改为:关闭\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n制热功能更改为:启用\r\n\r\n") ; 	        
       	                break;  	                	             
       	             
   case 10:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 手动除霜控制:停止控制\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n手动除霜控制:手动控制\r\n\r\n") ; 	        
       	                break;  	     	             
       	             
    case 11:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 系统状态更改为:关机\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n系统状态更改为:开机\r\n\r\n") ; 	        
       	                break;  	     	   	             
     case 12:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)(LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\冷冻室控制温差:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;
    case 13:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\冷藏室控制温差:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;   	             
    case 14:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜后风机延时运行更改为:无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜后风机延时运行更改为:有效\r\n\r\n") ; 	        
       	                break;  
    case 15:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 待机时风机运行方式更改为:关闭\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n待机时风机运行方式更改为:继续运行\r\n\r\n") ; 	        
       	                break;     	                
      case 16:
                sprintf(TempBuffer,"\r\n\r\nDUTY时间:%d更改为 %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;   
     case 17:    if(LocalConfig.ControlPara[i*2+1]==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 高压保护功能更改为:无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n高压保护功能更改为:有效\r\n\r\n") ; 	        
       	                break;  
       	         
       	                
    case 18:   if(LocalConfig.ControlPara[i*2+1]==0x00)
                  sprintf(TempBuffer,"\r\n\r\传感器选择更改为NEW ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x01)
                  sprintf(TempBuffer,"\r\n\r\传感器选择更改为OLD ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x02)
                  sprintf(TempBuffer,"\r\n\r\传感器选择更改为NEW(ASL) ,\r\n\r\n");     
       	     Uart_SendString(DebugPort,TempBuffer) ;
       	     break;
    default:break;   	             	                
   }    	                 	                  	                	     	   	             
}       	       



void SendControlMessage()
 {float temp1,temp2;
     int16_t  inttemp;
    u8 i,j;
    u8   TempBuffer[100];
    
    for(i=0;i<19;i++)
     {  WatchDog_feed();
      for(j=0;j<100;j++)
        TempBuffer[j]=0;
     switch(i)
       {
       	  case  0:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\冷冻室设定温度:%f最新值: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	              temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n冷藏设定温度值由:%f最新值: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    					
       	             sprintf(TempBuffer,"\r\n\r\n自动除霜间隔:%d最新值: %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;
       	               
       	 case 3:    sprintf(TempBuffer,"\r\n\r\n手动除霜蒸发器温度:%d最新值: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	                        
       	case 4:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜方式最新值::停机除霜方式\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜方式最新值::热气除霜方式\r\n\r\n") ; 	        
       	                break;
       case 5:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室功能最新值::无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室功能最新值::有效\r\n\r\n") ; 	        
       	                break; 
       	                
      case 6:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 退出除霜方式最新值::根据时间或温度退出\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n退出除霜方式最新值::根据时间退出\r\n\r\n") ; 	        
       	                break;  	                	             
       	          
     case 7:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷冻最低温度最新值::-30度\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷冻最低温度最新值::-5度\r\n\r\n") ; 	        
       	                break;  	                	             
     case 8:    
       	               	  sprintf(TempBuffer,"\r\n\r\n自动除霜时间:%d最新值: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;    	            	          
       	          
    case 9:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 制热功能最新值::关闭\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n制热功能最新值::启用\r\n\r\n") ; 	        
       	                break;  	                	             
       	             
   case 10:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 手动除霜控制:停止控制\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n手动除霜控制:手动控制\r\n\r\n") ; 	        
       	                break;  	     	             
       	             
    case 11:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 系统状态最新值::关机\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n系统状态最新值::开机\r\n\r\n") ; 	        
       	                break;  	     	   	             
     case 12:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)(LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\冷冻室控制温差:%f最新值: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;
    case 13:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\冷藏室控制温差:%f最新值: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;   	             
    case 14:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜后风机延时运行最新值::无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜后风机延时运行最新值::有效\r\n\r\n") ; 	        
       	                break;  
    case 15:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 待机时风机运行方式最新值::关闭\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n待机时风机运行方式最新值::继续运行\r\n\r\n") ; 	        
       	                break;     	                
      case 16:
                sprintf(TempBuffer,"\r\n\r\nDUTY时间:%d最新值: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;   
     case 17:    if(LocalConfig.ControlPara[i*2+1]==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 高压保护功能最新值::无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n高压保护功能最新值::有效\r\n\r\n") ; 	        
       	                break;  
       	         
       	                
    case 18:   if(LocalConfig.ControlPara[i*2+1]==0x00)
                  sprintf(TempBuffer,"\r\n\r\传感器选择最新值:NEW ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x01)
                  sprintf(TempBuffer,"\r\n\r\传感器选择最新值:OLD ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x02)
                  sprintf(TempBuffer,"\r\n\r\传感器选择最新值:NEW(ASL) ,\r\n\r\n");     
       	     Uart_SendString(DebugPort,TempBuffer) ;
       	     break;
    default:break;   	             	                
   }    	                 	                  	                	     	   	             
}       	       
}
/*----------------------------------------------
Function:void  SendChangeMessage(u8 change)
void CompareChange()
Input:  
Output: 
Description:监测状态变量的变化,将变化的状态变量通过debug口打印出来
------------------------------------------------*/
void CompareChange()
 { u8 i;
    for(i=0;i<cSendParaNum*2;i++)
     {
       if(LocalConfig.varList[i]!=LocalConfig.varListCompare[i])
         SendChangeMessage(i);
     }    		
    for(i=0;i<cSendParaNum*2;i++)	
      LocalConfig.varListCompare[i]=LocalConfig.varList[i];
 }      		  
void  SendChangeMessage(u8 change)
  { float temp1,temp2;
     int16_t  inttemp;
    u8 i=change/2,j;
    u8   TempBuffer[100];
      for(j=0;j<100;j++)
        TempBuffer[j]=0;
     switch(i)
       {
       	  case  0:   inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	 temp2=(float)(inttemp);            
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\冷冻室设定温度:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	   temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n冷藏设定温度值由:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	              temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	            temp2=(float)(inttemp);
										temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n冷冻温度实际值由:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	 case 3:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	              temp1=(float)(inttemp);
       	             temp1/=10;
       	            inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)(inttemp);
										 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n冷藏温度实际值由:%f更改为 %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;   
       	                        
       	case 4:    
       	            if(LocalConfig.varListCompare[i*2]!=LocalConfig.varList[i*2])
       	              
       	               {  char m=(char)LocalConfig.varListCompare[i*2];
       	                  char n=(char)LocalConfig.varList[i*2];
       	               
       	               	sprintf(TempBuffer,"\r\n\r\n冷冻盘管温度实际值由:%f更改为 %f,\r\n\r\n" ,m,n); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }  
       	            if(LocalConfig.varListCompare[i*2+1]!=LocalConfig.varList[i*2+1])
       	              { char m=(char)LocalConfig.varListCompare[i*2+1];
       	                  char n=(char)LocalConfig.varList[i*2+1];
       	              	
       	              	sprintf(TempBuffer,"\r\n\r\n冷冻盘管温度实际值由:%f更改为 %f,\r\n\r\n" ,m,n); 
       	                Uart_SendString(DebugPort,TempBuffer) ; 
       	              }
       	              break;    
       	          
       case 5:     //报警 
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警1更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警1更改为:报警\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警2更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警2更改为:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警3更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警3更改为:报警\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警4更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警4更改为:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警51更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警51更改为:报警\r\n\r\n") ; 	        
       	             
       	               }       
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警52更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警52更改为:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	           if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警53更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警53更改为:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	           if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警54更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警54更改为:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	         //-------------------------------      
       	              if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警91更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警91更改为:报警\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警更改为:92恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警92更改为:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	           
       	           if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警97更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警97更改为:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	          if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警98更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警98更改为:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	           if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警99更改为:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警99更改为:报警\r\n\r\n") ; 	        
       	             
       	               }       
       	        
       	              break; 
       	  case 6:     //工作状态
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷凝器风扇更改为:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷凝器风扇更改为:开启\r\n\r\n") ; 	        
       	             
       	               }
       	         if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷冻室蒸发器风扇更改为:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷冻室蒸发器风扇更改为:开启\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室蒸发器风扇更改为:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室蒸发器风扇更改为:开启\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜更改为:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜更改为:有效\r\n\r\n") ; 	        
       	             
       	               }     
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 温区选择更改为:冷藏无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n温区选择更改为:冷藏有效\r\n\r\n") ; 	        
       	             
       	               }       
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室更改为:停止工作\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室更改为:正在工作\r\n\r\n") ; 	        
       	             
       	               }   
       	          if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 机组更改为:无制冷请求\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n机组更改为:有制冷请求\r\n\r\n") ; 	        
       	             
       	               }     
       	         if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 机组更改为:无制热请求\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n机组更改为:有制热请求\r\n\r\n") ; 	        
       	             
       	               }                 
       	              
       	            break;
       	                            
       	  case  7:  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备状态更改为:关机\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备状态更改为:开机\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备更改为:停止除霜\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备更改为:手动除霜\r\n\r\n") ; 	        
       	             
       	               }   
       	            break;
       	     case  9:   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备供电更改为:外部供电\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备供电更改为:电池供电\r\n\r\n") ;//电池供电 	        
       	             
       	               }
       	               
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置更改为:东经\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置更改为:西经\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置更改为:南纬\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置更改为:北纬\r\n\r\n") ; 	        
       	             
       	               }      
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n门更改为:关闭状态\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n门更改为:开启状态\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x08)==0x08)
       	               {
       	               	   sprintf(TempBuffer,"\r\n\r\n电池电压由:%f更改为: %f,\r\n\r\n" ,LocalConfig.varListCompare[i*2],LocalConfig.varList[i*2]); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }         
       	            break; 
       	  default:break;
      } 	                  
 }


/*----------------------------------------------
Function:void  SendCurrentStatusMessage()
Input:  
Output: 
Description:将所有的状态变量一次性全部打印出来,不管是否有变化
------------------------------------------------*/
void  SendCurrentStatusMessage()
  { float temp1,temp2;
    u8 i,j;
    u8   TempBuffer[250];
    int16_t  inttemp;
    union  
     {  float  temp;
     	char    var[4];
      }f;	
    for(i=0;i<10;i++)
    {  WatchDog_feed();
      for(j=0;j<100;j++)
        TempBuffer[j]=0;
     switch(i)
       {
       	  case  0:   inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\前一冷冻设定温度:%f  当前值为: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n前一冷藏设定温度值:%f    当前值为: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;  
       	             sprintf(TempBuffer,"\r\n\r\n前一冷冻温度实际值:%f    当前值为: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	 case 3:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n前一冷藏温度实际值:%f    当前值为: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;   
       	                        
       	case 4:    
       	          // if(LocalConfig.varListCompare[i*2]!=LocalConfig.varList[i*2])
       	              
       	               {  char m=(char)LocalConfig.varListCompare[i*2];
       	                  char n=(char)LocalConfig.varList[i*2];
       	               
       	               	sprintf(TempBuffer,"\r\n\r\n冷冻盘管温度实际值: %f,\r\n\r\n" ,n); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }  
       	           // if(LocalConfig.varListCompare[i*2+1]!=LocalConfig.varList[i*2+1])
       	              { char m=(char)LocalConfig.varListCompare[i*2+1];
       	                  char n=(char)LocalConfig.varList[i*2+1];
       	              	
       	              	sprintf(TempBuffer,"\r\n\r\n冷藏盘管温度实际值:%f,\r\n\r\n" ,n); 
       	                Uart_SendString(DebugPort,TempBuffer) ; 
       	              }
       	              break;    
       	          
       	case 5:     //报警 
       	            // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警1恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警1:报警\r\n\r\n") ; 	        
       	             
       	               }
       	           // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警2:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警2:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警3:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警3:报警\r\n\r\n") ; 	        
       	             
       	               }      
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警4:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警4:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警51:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警51:报警\r\n\r\n") ; 	        
       	             
       	               }       
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警52:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警52:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	         //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警53:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警53:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	         //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警54:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警54:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	         //-------------------------------      
       	          //    if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警91:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警91:报警\r\n\r\n") ; 	        
       	             
       	               }
       	          //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警:92恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警92:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	           
       	         //   if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警97:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警97:报警\r\n\r\n") ; 	        
       	             
       	               }   
       	         //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警98:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警98:报警\r\n\r\n") ; 	        
       	             
       	               }     
       	       //    if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 报警99:恢复\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n报警99:报警\r\n\r\n") ; 	        
       	             
       	               }       
       	        
       	              break; 
       	  case 6:     //工作状态
       	          //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷凝器风扇:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷凝器风扇:开启\r\n\r\n") ; 	        
       	             
       	               }
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷冻室蒸发器风扇:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷冻室蒸发器风扇:开启\r\n\r\n") ; 	        
       	             
       	               }   
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室蒸发器风扇:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室蒸发器风扇:开启\r\n\r\n") ; 	        
       	             
       	               }      
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 除霜:停止\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n除霜:有效\r\n\r\n") ; 	        
       	             
       	               }     
       	        //    if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 温区选择:冷藏无效\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n温区选择:冷藏有效\r\n\r\n") ; 	        
       	             
       	               }       
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 冷藏室:停止工作\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n冷藏室:正在工作\r\n\r\n") ; 	        
       	             
       	               }   
       	        //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 机组:无制冷请求\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n机组:有制冷请求\r\n\r\n") ; 	        
       	             
       	               }     
       	        //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n 机组无制热请求\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n机组:有制热请求\r\n\r\n") ; 	        
       	             
       	               }                 
       	              
       	            break;
       	                            
       	  case  7:  // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备状态:关机\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备状态:开机\r\n\r\n") ; 	        
       	             
       	               }
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备:停止除霜\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备:手动除霜\r\n\r\n") ; 	        
       	             
       	               }   
       	            break;
       	     case  9:  // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备供电:外部供电\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n设备供电:电池供电\r\n\r\n") ; 	  //电池供电       
       	             
       	               }
       	               
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置:东经\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置:西经\r\n\r\n") ; 	        
       	             
       	               }   
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置:南纬\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n位置:北纬\r\n\r\n") ; 	        
       	             
       	               }      
       	           //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n门:关闭状态\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n门:开启状态\r\n\r\n") ; 	        
       	             
       	               }      
       	          //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x08)==0x08)
       	               {
       	               	   sprintf(TempBuffer,"\r\n\r\n电池电压: %f,\r\n\r\n", LocalConfig.varList[i*2]); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }         
       	            break; 
       	  default:break;
      } 
  

   
   
   
      	                  
 }
//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
    for(i=0;i<4;i++)
     f.var[i]= LocalConfig.varList[10*2+3-i];
    sprintf(TempBuffer,"\r\n\r\经度: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ;   

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
    for(i=0;i<4;i++)
     f.var[i]=LocalConfig.varList[12*2+3-i];
    sprintf(TempBuffer,"\r\n\r\纬度: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
    for(i=0;i<4;i++)
     f.var[i]= LocalConfig.varList[14*2+3-i];
    sprintf(TempBuffer,"\r\n\r\高度: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
   
    sprintf(TempBuffer,"\r\n\r\速度: %f,\r\n\r\n" ,LocalConfig.varList[16*2]*256+LocalConfig.varList[16*2+1]); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
   
    sprintf(TempBuffer,"\r\n\r\角度: %f,\r\n\r\n" ,LocalConfig.varList[17*2]*256+LocalConfig.varList[17*2+1]); 
    Uart_SendString(DebugPort,TempBuffer) ; 


for(i=0;i<100;i++) 
      TempBuffer[i]=0;
 sprintf(TempBuffer,"\r\n\r\外部温度1: %f度,外部湿度1:%f% \r\n外部温度2: %f度,外部湿度2:%f% \r\n外部温度3: %f度,外部湿度3:%f%\r\n外部温度4: %f度,外部湿度4:%f% \r\n外部温度5: %f度,外部湿度5:%f%  \ \r\n\r\n" ,
  ((float)((int16_t)(LocalConfig.varList[18*2]*256+LocalConfig.varList[18*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[19*2]*256+LocalConfig.varList[19*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[20*2]*256+LocalConfig.varList[20*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[21*2]*256+LocalConfig.varList[21*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[22*2]*256+LocalConfig.varList[22*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[23*2]*256+LocalConfig.varList[23*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[24*2]*256+LocalConfig.varList[24*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[25*2]*256+LocalConfig.varList[25*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[26*2]*256+LocalConfig.varList[26*2+1])))/10,
  ((float)((int16_t)(LocalConfig.varList[27*2]*256+LocalConfig.varList[27*2+1])))/10); 
    Uart_SendString(DebugPort,TempBuffer) ;
}


 
 	
/*----------------------------------------------
Function: PubMessageToServer()
Input:  
Output: 
Description:发送状态变量数据到服务器
------------------------------------------------*/  
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


/*----------------------------------------------
Function: void PubControlMessageToServer()
Input:  
Output: 
Description:将控制变化发送到服务器
------------------------------------------------*/  
#define cSendParaNumControl  19   //19个控制变量
void PubControlMessageToServer()
 {
   unsigned char  i, Temp[160]="AA55040001010326";   //26=38
   
   uint16_t TempCrc;	
     Temp[16]=0X01;
     Temp[17]=0X03;
     Temp[18]=cSendParaNumControl*2;
      for(i=0;i<4;i++)
        LocalConfig.ControlPara[i]=LocalConfig.varList[i];  //设定温度
      //开关机控制/手动除霜
         LocalConfig.ControlPara[20]=0;
         LocalConfig.ControlPara[21]=0; 
         LocalConfig.ControlPara[22]=0;
         LocalConfig.ControlPara[23]=0; 
      if((LocalConfig.varList[15]&0x01)==0x01)
         LocalConfig.ControlPara[23]=1; 
      if((LocalConfig.varList[15]&0x02)==0x02)
          LocalConfig.ControlPara[21]=1;
        
     for(i=0;i<cSendParaNumControl*2;i++)
      	Temp[19+i]=LocalConfig.ControlPara[i];  //参数
     
      TempCrc=CRCcount(&Temp[16],cSendParaNumControl*2+3); 
    //len 
     Temp[14]=(cSendParaNumControl*2)/16+0x30;
     if(Temp[14]>0x39)
        Temp[14]=Temp[14]+'A'-0X3A;
     
     Temp[15]=(cSendParaNumControl*2)%16+0x30;
     if(Temp[15]>0x39)
        Temp[15]=Temp[15]+'A'-0X3A;   
    //data 
     
     for(i=0;i<cSendParaNumControl*2;i++)    
        {	
       	          Temp[16+i*2]=LocalConfig.ControlPara[i]/16+0X30;
       	          if(Temp[16+i*2]>0x39)
       	             Temp[16+i*2]=Temp[16+i*2]+'A'-0X3A;
       	           
                Temp[16+i*2+1]=LocalConfig.ControlPara[i]%16+0X30;
                if(Temp[16+i*2+1]>0x39)
       	             Temp[16+i*2+1]=Temp[16+i*2+1]+'A'-0X3A;
        } 
       //crc  
       Temp[16+cSendParaNumControl*4]=TempCrc/256/16+0X30;
       if(Temp[16+cSendParaNumControl*4]>0x39)
       	             Temp[16+cSendParaNumControl*4]=Temp[16+cSendParaNum*4]+'A'-0X3A;
     	Temp[16+cSendParaNumControl*4+1]=TempCrc/256%16+0X30;
     	if(Temp[16+cSendParaNumControl*4+1]>0x39)
       	             Temp[16+cSendParaNumControl*4+1]=Temp[16+cSendParaNumControl*4+1]+'A'-0X3A;
     	Temp[16+cSendParaNumControl*4+2]=(TempCrc%256)/16+0X30;
     	if(Temp[16+cSendParaNumControl*4+2]>0x39)
       	             Temp[16+cSendParaNumControl*4+2]=Temp[16+cSendParaNumControl*4+2]+'A'-0X3A;
     	Temp[16+cSendParaNumControl*4+3]=(TempCrc%256)%16+0X30;
     	if(Temp[16+cSendParaNumControl*4+3]>0x39)
       	             Temp[16+cSendParaNumControl*4+3]=Temp[16+cSendParaNumControl*4+3]+'A'-0X3A;
     	
     	PubMessage(0,(cSendParaNumControl*2+10)*2,Temp);
  }


/*----------------------------------------------
Function: void PubLoopControlMessageToServer(u16 Addr,u16 len)
Input:  
Output: 
Description:发送指定地址/指定长度的控制变量,一般用来回复03命令的读取命令
------------------------------------------------*/  
void PubLoopControlMessageToServer(u16 Addr,u16 len)
 {
   unsigned char  i, Temp[160]="010326";   //26=38
   
   uint16_t TempCrc;	
     Temp[16]=0X01;
     Temp[17]=0X03;
     Temp[18]=len*2;
     
     for(i=0;i<4;i++)
        LocalConfig.ControlPara[i]=LocalConfig.varList[i];
         LocalConfig.ControlPara[20]=0;
         LocalConfig.ControlPara[21]=0; 
         LocalConfig.ControlPara[22]=0;
         LocalConfig.ControlPara[23]=0; 
      if((LocalConfig.varList[15]&0x01)==0x01)
         LocalConfig.ControlPara[23]=1; 
      if((LocalConfig.varList[15]&0x02)==0x02)
          LocalConfig.ControlPara[21]=1;
     for(i=Addr*2;i<Addr*2+len*2;i++)
      	Temp[19+i]=LocalConfig.ControlPara[i];  //参数
    //crc 
     TempCrc=CRCcount(&Temp[16],len*2+3); 
   //len    
     Temp[4]=(len*2)/16+0x30;
     if(Temp[4]>0x39)
        Temp[4]=Temp[4]+'A'-0X3A;
     
     Temp[5]=(len*2)%16+0x30;
     if(Temp[5]>0x39)
        Temp[5]=Temp[5]+'A'-0X3A;   
     
    
     for(i=0;i<len*2;i++)    
        {	
       	          Temp[6+i*2]=LocalConfig.ControlPara[Addr*2+i]/16+0X30;
       	          if(Temp[6+i*2]>0x39)
       	             Temp[6+i*2]=Temp[6+i*2]+'A'-0X3A;
       	           
                Temp[6+i*2+1]=LocalConfig.ControlPara[Addr*2+i]%16+0X30;
                if(Temp[6+i*2+1]>0x39)
       	             Temp[6+i*2+1]=Temp[6+i*2+1]+'A'-0X3A;
        } 
       //crc  
       Temp[6+len*4]=TempCrc/256/16+0X30;
       if(Temp[6+len*4]>0x39)
       	             Temp[6+len*4]=Temp[6+len*4]+'A'-0X3A;
     	
     	Temp[6+len*4+1]=TempCrc/256%16+0X30;
     	if(Temp[6+len*4+1]>0x39)
       	             Temp[6+len*4+1]=Temp[6+len*4+1]+'A'-0X3A;
     	
     	Temp[6+len*4+2]=(TempCrc%256)/16+0X30;
     	if(Temp[6+len*4+2]>0x39)
       	             Temp[6+len*4+2]=Temp[6+len*4+2]+'A'-0X3A;
     	
     	Temp[6+len*4+3]=(TempCrc%256)%16+0X30;
     	if(Temp[6+len*4+3]>0x39)
       	             Temp[6+len*4+3]=Temp[6+len*4+3]+'A'-0X3A;
     	PubMessage(0,(len*2+5)*2,Temp);
  }
/*----------------------------------------------
Function:void PubLoopRunMessageToServer(u16 Addr,u16 len)
Input:  
Output: 
Description:发送指定地址/指定长度的状态变量,一般用来回复04命令的读取命令
------------------------------------------------*/    
void PubLoopRunMessageToServer(u16 Addr,u16 len)
 {
   unsigned char  i, Temp[160]="010426";   //26=38
   
   uint16_t TempCrc;	
     Temp[16]=0X01;
     Temp[17]=0X04;
     Temp[18]=len*2;
     
     for(i=0;i<4;i++)
        LocalConfig.ControlPara[i]=LocalConfig.varList[i];
    
     for(i=Addr*2;i<Addr*2+len*2;i++)
      	Temp[19+i]=LocalConfig.varList[i];  //参数
    
     TempCrc=CRCcount(&Temp[16],len*2+3); 
 //len    
     Temp[4]=(len*2)/16+0x30;
     if(Temp[4]>0x39)
        Temp[4]=Temp[4]+'A'-0X3A;
     
     Temp[5]=(len*2)%16+0x30;
     if(Temp[5]>0x39)
        Temp[5]=Temp[5]+'A'-0X3A;   
     
    
     for(i=0;i<len*2;i++)    
        {	
       	          Temp[6+i*2]=LocalConfig.varList[Addr*2+i]/16+0X30;
       	          if(Temp[6+i*2]>0x39)
       	             Temp[6+i*2]=Temp[6+i*2]+'A'-0X3A;
       	           
                Temp[6+i*2+1]=LocalConfig.varList[Addr*2+i]%16+0X30;
                if(Temp[6+i*2+1]>0x39)
       	             Temp[6+i*2+1]=Temp[6+i*2+1]+'A'-0X3A;
        } 
         
       Temp[6+len*4]=TempCrc/256/16+0X30;
       if(Temp[6+len*4]>0x39)
       	             Temp[6+len*4]=Temp[6+len*4]+'A'-0X3A;
     	Temp[6+len*4+1]=TempCrc/256%16+0X30;
     	if(Temp[6+len*4+1]>0x39)
       	             Temp[6+len*4+1]=Temp[6+len*4+1]+'A'-0X3A;
     	Temp[6+len*4+2]=(TempCrc%256)/16+0X30;
     	if(Temp[6+len*4+2]>0x39)
       	             Temp[6+len*4+2]=Temp[6+len*4+2]+'A'-0X3A;
     	Temp[6+len*4+3]=(TempCrc%256)%16+0X30;
     	if(Temp[6+len*4+3]>0x39)
       	             Temp[6+len*4+3]=Temp[6+len*4+3]+'A'-0X3A;
     	PubMessage(0,(len*2+5)*2,Temp);
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
	  { WatchDog_feed();
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
	char *ResBegin=NULL,*ResEnd=NULL,*ResEnd2=NULL,TempBuffer[300],j;
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
	  {  WatchDog_feed();
	   if(LocalConfig.UART_RespondRead[1]!=LocalConfig.UART_Write[1])	
 	     {  TranslateData(1,4);   
 	   	Error=(LocalConfig.UART_Write[1]+UART_Bufferlength-LocalConfig.UART_RespondRead[1])%UART_Bufferlength;
                for(i=0;i<300;i++)
                              TempBuffer[i]=0;
                if(Error>300)
                   Error=290;
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
   j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","+CMQTTTOPIC: 0,14","ERROR",0,0);
		if(j==0)
		 {
      for(m=0;m<3;m++) 
         { 
            j=WaitReceiveResult(cMqttWaitTime,">","IGNORE","IGNORE",0,0);
       	       if(j==1)
       	         break;
       j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","+CMQTTTOPIC: 0,14","ERROR",0,0);	
      	if((j==1)||(j==2)||(j==3))
       	         break;	
						 }	
					 }	 
 //------------------------------ 
	 if(j==2)
      {     
	   RequestToInitial();
				j=0;
      }
    if(j==3)
       {
         RequestToInitial();
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
   //add  2020-10-23 23:05 
     Uart_SendStringLen(4,"SendToServer:",strlen("SendToServer:"));
     Uart_SendStringLen(4,Sendbuffer,strlen(Sendbuffer));
   //end of add  2020-10-23 23:05
     
return(j);
  }


/*----------------------------------------------
Function: void SetPayload(unsigned char Clientid,unsigned char Len)
Input:  Clientid:只能是0和1
Output:
Description: 组合要发送的payload信息,真正的payload要等到回复>后再发
------------------------------------------------*/  
unsigned char PubMessageSel(unsigned char Clientid,unsigned char len,unsigned char *Sendbuffer)
  {
     unsigned char  TempChar[100],i,j,m=0;
     for(i=0;i<100;i++)
       	TempChar[i]=0;
     SetPubTopic( Clientid,TempChar);
     
     j=MqttSendAndReceive(TempChar,cMqttWaitTime,">","+CMQTTTOPIC: 0,14","ERROR",0,0);
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
      {     
	   RequestToInitial();
      }
   else  if(j==3)
       {
         RequestToInitial();
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
 
 
 													
