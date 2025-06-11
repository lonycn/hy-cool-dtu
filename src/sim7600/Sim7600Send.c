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
Description:�����Ʊ����ı仯,���仯�Ŀ��Ʊ���ͨ��debug�ڴ�ӡ����
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
       	             sprintf(TempBuffer,"\r\n\r\�䶳���趨�¶�:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	              temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n����趨�¶�ֵ��:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    					
       	             sprintf(TempBuffer,"\r\n\r\n�Զ���˪���:%f����Ϊ %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;
       	               
       	 case 3:    sprintf(TempBuffer,"\r\n\r\n�ֶ���˪�������¶�:%f����Ϊ %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	                        
       	case 4:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪��ʽ����Ϊ:ͣ����˪��ʽ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪��ʽ����Ϊ:������˪��ʽ\r\n\r\n") ; 	        
       	                break;
       case 5:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ҹ��ܸ���Ϊ:��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ҹ��ܸ���Ϊ:��Ч\r\n\r\n") ; 	        
       	                break; 
       	                
      case 6:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �˳���˪��ʽ����Ϊ:����ʱ����¶��˳�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�˳���˪��ʽ����Ϊ:����ʱ���˳�\r\n\r\n") ; 	        
       	                break;  	                	             
       	          
     case 7:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �䶳����¶ȸ���Ϊ:-30��\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�䶳����¶ȸ���Ϊ:-5��\r\n\r\n") ; 	        
       	                break;  	                	             
     case 8:    
       	               	  sprintf(TempBuffer,"\r\n\r\n�Զ���˪ʱ��:%d����Ϊ %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;    	            	          
       	          
    case 9:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ���ȹ��ܸ���Ϊ:�ر�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n���ȹ��ܸ���Ϊ:����\r\n\r\n") ; 	        
       	                break;  	                	             
       	             
   case 10:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �ֶ���˪����:ֹͣ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�ֶ���˪����:�ֶ�����\r\n\r\n") ; 	        
       	                break;  	     	             
       	             
    case 11:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ϵͳ״̬����Ϊ:�ػ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nϵͳ״̬����Ϊ:����\r\n\r\n") ; 	        
       	                break;  	     	   	             
     case 12:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)(LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\�䶳�ҿ����²�:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;
    case 13:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\����ҿ����²�:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;   	             
    case 14:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪������ʱ���и���Ϊ:��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪������ʱ���и���Ϊ:��Ч\r\n\r\n") ; 	        
       	                break;  
    case 15:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ʱ������з�ʽ����Ϊ:�ر�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ʱ������з�ʽ����Ϊ:��������\r\n\r\n") ; 	        
       	                break;     	                
      case 16:
                sprintf(TempBuffer,"\r\n\r\nDUTYʱ��:%d����Ϊ %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;   
     case 17:    if(LocalConfig.ControlPara[i*2+1]==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��ѹ�������ܸ���Ϊ:��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��ѹ�������ܸ���Ϊ:��Ч\r\n\r\n") ; 	        
       	                break;  
       	         
       	                
    case 18:   if(LocalConfig.ControlPara[i*2+1]==0x00)
                  sprintf(TempBuffer,"\r\n\r\������ѡ�����ΪNEW ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x01)
                  sprintf(TempBuffer,"\r\n\r\������ѡ�����ΪOLD ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x02)
                  sprintf(TempBuffer,"\r\n\r\������ѡ�����ΪNEW(ASL) ,\r\n\r\n");     
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
       	             sprintf(TempBuffer,"\r\n\r\�䶳���趨�¶�:%f����ֵ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	              temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n����趨�¶�ֵ��:%f����ֵ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    					
       	             sprintf(TempBuffer,"\r\n\r\n�Զ���˪���:%d����ֵ: %f,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;
       	               
       	 case 3:    sprintf(TempBuffer,"\r\n\r\n�ֶ���˪�������¶�:%d����ֵ: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	                        
       	case 4:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪��ʽ����ֵ::ͣ����˪��ʽ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪��ʽ����ֵ::������˪��ʽ\r\n\r\n") ; 	        
       	                break;
       case 5:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ҹ�������ֵ::��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ҹ�������ֵ::��Ч\r\n\r\n") ; 	        
       	                break; 
       	                
      case 6:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �˳���˪��ʽ����ֵ::����ʱ����¶��˳�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�˳���˪��ʽ����ֵ::����ʱ���˳�\r\n\r\n") ; 	        
       	                break;  	                	             
       	          
     case 7:    
       	               	   if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �䶳����¶�����ֵ::-30��\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�䶳����¶�����ֵ::-5��\r\n\r\n") ; 	        
       	                break;  	                	             
     case 8:    
       	               	  sprintf(TempBuffer,"\r\n\r\n�Զ���˪ʱ��:%d����ֵ: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;    	            	          
       	          
    case 9:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ���ȹ�������ֵ::�ر�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n���ȹ�������ֵ::����\r\n\r\n") ; 	        
       	                break;  	                	             
       	             
   case 10:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �ֶ���˪����:ֹͣ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�ֶ���˪����:�ֶ�����\r\n\r\n") ; 	        
       	                break;  	     	             
       	             
    case 11:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ϵͳ״̬����ֵ::�ػ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nϵͳ״̬����ֵ::����\r\n\r\n") ; 	        
       	                break;  	     	   	             
     case 12:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)(LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\�䶳�ҿ����²�:%f����ֵ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;
    case 13:   inttemp=(int16_t)( LocalConfig.ControlParaCompare[i*2]*256+LocalConfig.ControlParaCompare[i*2+1]);
       	              temp1=(float)(inttemp); 
       	             temp1*=0.5;
       	             inttemp=(int16_t)( LocalConfig.ControlPara[i*2]*256+LocalConfig.ControlPara[i*2+1]);
       	             temp2=(float)(inttemp);            
				 temp2*=0.5;
       	             sprintf(TempBuffer,"\r\n\r\����ҿ����²�:%f����ֵ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ;	         
       	             break;   	             
    case 14:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪������ʱ��������ֵ::��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪������ʱ��������ֵ::��Ч\r\n\r\n") ; 	        
       	                break;  
    case 15:    if((LocalConfig.ControlPara[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ʱ������з�ʽ����ֵ::�ر�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ʱ������з�ʽ����ֵ::��������\r\n\r\n") ; 	        
       	                break;     	                
      case 16:
                sprintf(TempBuffer,"\r\n\r\nDUTYʱ��:%d����ֵ: %d,\r\n\r\n" ,LocalConfig.ControlParaCompare[i*2+1],LocalConfig.ControlPara[i*2+1]); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 	        
       	                break;   
     case 17:    if(LocalConfig.ControlPara[i*2+1]==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��ѹ������������ֵ::��Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��ѹ������������ֵ::��Ч\r\n\r\n") ; 	        
       	                break;  
       	         
       	                
    case 18:   if(LocalConfig.ControlPara[i*2+1]==0x00)
                  sprintf(TempBuffer,"\r\n\r\������ѡ������ֵ:NEW ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x01)
                  sprintf(TempBuffer,"\r\n\r\������ѡ������ֵ:OLD ,\r\n\r\n"); 
       	        else  if(LocalConfig.ControlPara[i*2+1]==0x02)
                  sprintf(TempBuffer,"\r\n\r\������ѡ������ֵ:NEW(ASL) ,\r\n\r\n");     
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
Description:���״̬�����ı仯,���仯��״̬����ͨ��debug�ڴ�ӡ����
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
       	             sprintf(TempBuffer,"\r\n\r\�䶳���趨�¶�:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	   temp2=(float)(inttemp);          
				 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n����趨�¶�ֵ��:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	              temp1=(float)(inttemp);
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	            temp2=(float)(inttemp);
										temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n�䶳�¶�ʵ��ֵ��:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	 case 3:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	              temp1=(float)(inttemp);
       	             temp1/=10;
       	            inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)(inttemp);
										 temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\n����¶�ʵ��ֵ��:%f����Ϊ %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;   
       	                        
       	case 4:    
       	            if(LocalConfig.varListCompare[i*2]!=LocalConfig.varList[i*2])
       	              
       	               {  char m=(char)LocalConfig.varListCompare[i*2];
       	                  char n=(char)LocalConfig.varList[i*2];
       	               
       	               	sprintf(TempBuffer,"\r\n\r\n�䶳�̹��¶�ʵ��ֵ��:%f����Ϊ %f,\r\n\r\n" ,m,n); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }  
       	            if(LocalConfig.varListCompare[i*2+1]!=LocalConfig.varList[i*2+1])
       	              { char m=(char)LocalConfig.varListCompare[i*2+1];
       	                  char n=(char)LocalConfig.varList[i*2+1];
       	              	
       	              	sprintf(TempBuffer,"\r\n\r\n�䶳�̹��¶�ʵ��ֵ��:%f����Ϊ %f,\r\n\r\n" ,m,n); 
       	                Uart_SendString(DebugPort,TempBuffer) ; 
       	              }
       	              break;    
       	          
       case 5:     //���� 
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����1����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����1����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����2����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����2����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����3����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����3����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����4����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����4����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }     
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����51����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����51����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }       
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����52����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����52����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	           if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����53����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����53����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }     
       	           if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����54����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����54����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }     
       	         //-------------------------------      
       	              if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����91����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����91����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��������Ϊ:92�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����92����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	           
       	           if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����97����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����97����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	          if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����98����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����98����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }     
       	           if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����99����Ϊ:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����99����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }       
       	        
       	              break; 
       	  case 6:     //����״̬
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ���������ȸ���Ϊ:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n���������ȸ���Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }
       	         if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �䶳�����������ȸ���Ϊ:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�䶳�����������ȸ���Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��������������ȸ���Ϊ:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��������������ȸ���Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪����Ϊ:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪����Ϊ:��Ч\r\n\r\n") ; 	        
       	             
       	               }     
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ѡ�����Ϊ:�����Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ѡ�����Ϊ:�����Ч\r\n\r\n") ; 	        
       	             
       	               }       
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����Ҹ���Ϊ:ֹͣ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����Ҹ���Ϊ:���ڹ���\r\n\r\n") ; 	        
       	             
       	               }   
       	          if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �������Ϊ:����������\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�������Ϊ:����������\r\n\r\n") ; 	        
       	             
       	               }     
       	         if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �������Ϊ:����������\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�������Ϊ:����������\r\n\r\n") ; 	        
       	             
       	               }                 
       	              
       	            break;
       	                            
       	  case  7:  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸״̬����Ϊ:�ػ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸״̬����Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸����Ϊ:ֹͣ��˪\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸����Ϊ:�ֶ���˪\r\n\r\n") ; 	        
       	             
       	               }   
       	            break;
       	     case  9:   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸�������Ϊ:�ⲿ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸�������Ϊ:��ع���\r\n\r\n") ;//��ع��� 	        
       	             
       	               }
       	               
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ�ø���Ϊ:����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ�ø���Ϊ:����\r\n\r\n") ; 	        
       	             
       	               }   
       	            if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ�ø���Ϊ:��γ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ�ø���Ϊ:��γ\r\n\r\n") ; 	        
       	             
       	               }      
       	             if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�Ÿ���Ϊ:�ر�״̬\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�Ÿ���Ϊ:����״̬\r\n\r\n") ; 	        
       	             
       	               }      
       	            if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x08)==0x08)
       	               {
       	               	   sprintf(TempBuffer,"\r\n\r\n��ص�ѹ��:%f����Ϊ: %f,\r\n\r\n" ,LocalConfig.varListCompare[i*2],LocalConfig.varList[i*2]); 
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
Description:�����е�״̬����һ����ȫ����ӡ����,�����Ƿ��б仯
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
       	             sprintf(TempBuffer,"\r\n\r\ǰһ�䶳�趨�¶�:%f  ��ǰֵΪ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break; 
       	  case 1:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\nǰһ����趨�¶�ֵ:%f    ��ǰֵΪ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	             
       	  case 2:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;  
       	             sprintf(TempBuffer,"\r\n\r\nǰһ�䶳�¶�ʵ��ֵ:%f    ��ǰֵΪ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;  
       	 case 3:    inttemp=(int16_t)( LocalConfig.varListCompare[i*2]*256+LocalConfig.varListCompare[i*2+1]);
       	             temp1=(float)inttemp;
       	             temp1/=10;
       	             inttemp=(int16_t)( LocalConfig.varList[i*2]*256+LocalConfig.varList[i*2+1]);
       	             temp2=(float)inttemp;
       	             temp2/=10;
       	             sprintf(TempBuffer,"\r\n\r\nǰһ����¶�ʵ��ֵ:%f    ��ǰֵΪ: %f,\r\n\r\n" ,temp1,temp2); 
       	             Uart_SendString(DebugPort,TempBuffer) ; 
       	             break;   
       	                        
       	case 4:    
       	          // if(LocalConfig.varListCompare[i*2]!=LocalConfig.varList[i*2])
       	              
       	               {  char m=(char)LocalConfig.varListCompare[i*2];
       	                  char n=(char)LocalConfig.varList[i*2];
       	               
       	               	sprintf(TempBuffer,"\r\n\r\n�䶳�̹��¶�ʵ��ֵ: %f,\r\n\r\n" ,n); 
       	                 Uart_SendString(DebugPort,TempBuffer) ; 
       	               }  
       	           // if(LocalConfig.varListCompare[i*2+1]!=LocalConfig.varList[i*2+1])
       	              { char m=(char)LocalConfig.varListCompare[i*2+1];
       	                  char n=(char)LocalConfig.varList[i*2+1];
       	              	
       	              	sprintf(TempBuffer,"\r\n\r\n����̹��¶�ʵ��ֵ:%f,\r\n\r\n" ,n); 
       	                Uart_SendString(DebugPort,TempBuffer) ; 
       	              }
       	              break;    
       	          
       	case 5:     //���� 
       	            // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����1�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����1:����\r\n\r\n") ; 	        
       	             
       	               }
       	           // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����2:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����2:����\r\n\r\n") ; 	        
       	             
       	               }   
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����3:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����3:����\r\n\r\n") ; 	        
       	             
       	               }      
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����4:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����4:����\r\n\r\n") ; 	        
       	             
       	               }     
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����51:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����51:����\r\n\r\n") ; 	        
       	             
       	               }       
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����52:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����52:����\r\n\r\n") ; 	        
       	             
       	               }   
       	         //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����53:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����53:����\r\n\r\n") ; 	        
       	             
       	               }     
       	         //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����54:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����54:����\r\n\r\n") ; 	        
       	             
       	               }     
       	         //-------------------------------      
       	          //    if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����91:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����91:����\r\n\r\n") ; 	        
       	             
       	               }
       	          //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����:92�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����92:����\r\n\r\n") ; 	        
       	             
       	               }   
       	           
       	         //   if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����97:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����97:����\r\n\r\n") ; 	        
       	             
       	               }   
       	         //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����98:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����98:����\r\n\r\n") ; 	        
       	             
       	               }     
       	       //    if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����99:�ָ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����99:����\r\n\r\n") ; 	        
       	             
       	               }       
       	        
       	              break; 
       	  case 6:     //����״̬
       	          //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����������:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����������:����\r\n\r\n") ; 	        
       	             
       	               }
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �䶳������������:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�䶳������������:����\r\n\r\n") ; 	        
       	             
       	               }   
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ���������������:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n���������������:����\r\n\r\n") ; 	        
       	             
       	               }      
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��˪:ֹͣ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��˪:��Ч\r\n\r\n") ; 	        
       	             
       	               }     
       	        //    if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x10)==0x10)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x10)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����ѡ��:�����Ч\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����ѡ��:�����Ч\r\n\r\n") ; 	        
       	             
       	               }       
       	         //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x20)==0x20)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x20)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n �����:ֹͣ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�����:���ڹ���\r\n\r\n") ; 	        
       	             
       	               }   
       	        //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x40)==0x40)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x40)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ����:����������\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����:����������\r\n\r\n") ; 	        
       	             
       	               }     
       	        //   if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x80)==0x80)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x80)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n ��������������\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n����:����������\r\n\r\n") ; 	        
       	             
       	               }                 
       	              
       	            break;
       	                            
       	  case  7:  // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸״̬:�ػ�\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸״̬:����\r\n\r\n") ; 	        
       	             
       	               }
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸:ֹͣ��˪\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸:�ֶ���˪\r\n\r\n") ; 	        
       	             
       	               }   
       	            break;
       	     case  9:  // if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x01)==0x01)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x01)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸����:�ⲿ����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n�豸����:��ع���\r\n\r\n") ; 	  //��ع���       
       	             
       	               }
       	               
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x02)==0x02)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x02)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ��:����\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ��:����\r\n\r\n") ; 	        
       	             
       	               }   
       	          //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x04)==0x04)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x04)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ��:��γ\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\nλ��:��γ\r\n\r\n") ; 	        
       	             
       	               }      
       	           //  if(((LocalConfig.varListCompare[i*2+1]^LocalConfig.varList[i*2+1])&0x08)==0x08)
       	               {
       	               	   if((LocalConfig.varList[i*2+1]&0x08)==0x00)
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��:�ر�״̬\r\n\r\n") ; 
       	               	  else
       	               	     Uart_SendString(DebugPort,"\r\n\r\n��:����״̬\r\n\r\n") ; 	        
       	             
       	               }      
       	          //  if(((LocalConfig.varListCompare[i*2]^LocalConfig.varList[i*2])&0x08)==0x08)
       	               {
       	               	   sprintf(TempBuffer,"\r\n\r\n��ص�ѹ: %f,\r\n\r\n", LocalConfig.varList[i*2]); 
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
    sprintf(TempBuffer,"\r\n\r\����: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ;   

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
    for(i=0;i<4;i++)
     f.var[i]=LocalConfig.varList[12*2+3-i];
    sprintf(TempBuffer,"\r\n\r\γ��: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
    for(i=0;i<4;i++)
     f.var[i]= LocalConfig.varList[14*2+3-i];
    sprintf(TempBuffer,"\r\n\r\�߶�: %f,\r\n\r\n" ,f.temp); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
   
    sprintf(TempBuffer,"\r\n\r\�ٶ�: %f,\r\n\r\n" ,LocalConfig.varList[16*2]*256+LocalConfig.varList[16*2+1]); 
    Uart_SendString(DebugPort,TempBuffer) ; 

//--------------------------   
    for(i=0;i<100;i++) 
      TempBuffer[i]=0;
   
    sprintf(TempBuffer,"\r\n\r\�Ƕ�: %f,\r\n\r\n" ,LocalConfig.varList[17*2]*256+LocalConfig.varList[17*2+1]); 
    Uart_SendString(DebugPort,TempBuffer) ; 


for(i=0;i<100;i++) 
      TempBuffer[i]=0;
 sprintf(TempBuffer,"\r\n\r\�ⲿ�¶�1: %f��,�ⲿʪ��1:%f% \r\n�ⲿ�¶�2: %f��,�ⲿʪ��2:%f% \r\n�ⲿ�¶�3: %f��,�ⲿʪ��3:%f%\r\n�ⲿ�¶�4: %f��,�ⲿʪ��4:%f% \r\n�ⲿ�¶�5: %f��,�ⲿʪ��5:%f%  \ \r\n\r\n" ,
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
Description:����״̬�������ݵ�������
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
Description:�����Ʊ仯���͵�������
------------------------------------------------*/  
#define cSendParaNumControl  19   //19�����Ʊ���
void PubControlMessageToServer()
 {
   unsigned char  i, Temp[160]="AA55040001010326";   //26=38
   
   uint16_t TempCrc;	
     Temp[16]=0X01;
     Temp[17]=0X03;
     Temp[18]=cSendParaNumControl*2;
      for(i=0;i<4;i++)
        LocalConfig.ControlPara[i]=LocalConfig.varList[i];  //�趨�¶�
      //���ػ�����/�ֶ���˪
         LocalConfig.ControlPara[20]=0;
         LocalConfig.ControlPara[21]=0; 
         LocalConfig.ControlPara[22]=0;
         LocalConfig.ControlPara[23]=0; 
      if((LocalConfig.varList[15]&0x01)==0x01)
         LocalConfig.ControlPara[23]=1; 
      if((LocalConfig.varList[15]&0x02)==0x02)
          LocalConfig.ControlPara[21]=1;
        
     for(i=0;i<cSendParaNumControl*2;i++)
      	Temp[19+i]=LocalConfig.ControlPara[i];  //����
     
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
Description:����ָ����ַ/ָ�����ȵĿ��Ʊ���,һ�������ظ�03����Ķ�ȡ����
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
      	Temp[19+i]=LocalConfig.ControlPara[i];  //����
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
Description:����ָ����ַ/ָ�����ȵ�״̬����,һ�������ظ�04����Ķ�ȡ����
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
      	Temp[19+i]=LocalConfig.varList[i];  //����
    
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
Output: 0:��ʱ  1-3:ƥ��Ľ��
Description: �ȴ�����buffer�е�ƥ����,���������֮����жϵȴ���
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
                            //ֻ��������gps���ݺ�mqtt�Ķ��İ�����
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
Description: �����ļ�/��ȡ����
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
                            //ֻ��������gps���ݺ�mqtt�Ķ��İ�����
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
Input:  mode:  0:�����ַ���  1;����len���ȵ�����    2:�����ƥ��,���ǽ��յ�len�ַ���,�˳�
Output:
Description: �������ݰ�,��ָ����ʱ��ȴ��ظ����,����յ��ظ�,����ȴ�,����ȴ�ʱ�䵽,����0
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
                            //ֻ��������gps���ݺ�mqtt�Ķ��İ�����
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
Input:  Clientid:ֻ����0��1
Output:
Description: ���Ҫ���͵�payload��Ϣ,������payloadҪ�ȵ��ظ�>���ٷ�
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
Input:  Clientid:ֻ����0��1
Output:
Description: ���Ҫ���͵�payload��Ϣ,������payloadҪ�ȵ��ظ�>���ٷ�
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
Description:  ��ѯgps,������gps�Զ�����
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
 
 
 													
