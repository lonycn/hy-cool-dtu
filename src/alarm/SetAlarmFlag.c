#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#define    u8  unsigned char
#define    u16  uint16_t
//--------------����-----�����¶ȱ���λ
void  TAlarmHi_Only(u8 i);
void  TAlarmLow_Only(u8 i);
void  TPreAlarmHi_Only(u8 i);
void  TPreAlarmLow_Only(u8 i);
//-------------���������-------------------  
void  TAlarmHi( );
void  TAlarmLow( );
void  TPreAlarmHi( );
void  TPreAlarmLow( );
//-------------��Ըߵ���λ--------��λʪ��-��-Ԥ��-����λ---------------       
void TNormalHi();
void TNormalLow();
void TNormal_level(); 
//=============����==========����ʪ��Ԥ��-����λ=================
void  RHAlarmHi_Only(u8 i);
void  RHAlarmLow_Only(u8 i);
void  RHPreAlarmHi_Only(u8 i);
void  RHPreAlarmLow_Only(u8 i);
 //-------------��Ըߵ���λ--------��λʪ��-��-Ԥ��-����λ---------------       
void RHNormalHi();
void RHNormalLow();
void  RHNormal_level();
//-------------���������-------------------     
void  RHAlarmHi();
void  RHAlarmLow();
void  RHPreAlarmHi();
void  RHPreAlarmLow();

/*-----------------����Ϊ����������״̬--------     
 1:����ʹ�ܼ���,
 2:������ʱʱ�䵽,���ö�Ӧ��״̬    
-------------------------------------------------*/     
//----------����------�¶ȸߵ�Ԥ����--����
void  TAlarmHi_AlarmFlag(u8 i);
void  TAlarmLow_AlarmFlag(u8 i);
void  TPreAlarmHi_AlarmFlag(u8 i);
void  TPreAlarmLow_AlarmFlag(u8 i);
 //----------��Ըߵ�------�¶ȸ�Ԥ����--��λ    
void TNormalHi_AlarmFlag();
void TNormalLow_AlarmFlag();
void    TNormal_AlarmFlag();
//-------����-----===ʪ�ȸߵ�Ԥ��������====-------------------
void  RHAlarmHi_AlarmFlag(u8 i);
void  RHAlarmLow_AlarmFlag(u8 i);
void  RHPreAlarmHi_AlarmFlag(u8 i);
void  RHPreAlarmLow_AlarmFlag(u8 i);
//----------��Ըߵ�------ʪ��-��Ԥ������λ-----------------------      
void RHNormalHi_AlarmFlag();
void RHNormalLow_AlarmFlag();
void RHNormal_AlarmFlag();

//���¸����¶ȱȽ�,�ж��Ƿ�������,�������Ƿ�����ʹ��

//-------------------�����¶ȱ���λ
void  TAlarmHi_Only(u8 i)
      { if(i==1)
      	 History.AlarmLevel|=0x20; 
      	else
				{ History.AlarmLevel&=0xdf;
				  History.MiniterDelay[2]=0;//add 20210322
				}	
      }	  
void  TAlarmLow_Only(u8 i)
      {	if(i==1)
      	 History.AlarmLevel|=0x10; 
      	else
				{  History.AlarmLevel&=0xef;
				History.MiniterDelay[0]=0;//add 20210322
      }	 
		}
void  TPreAlarmHi_Only(u8 i)
      { 
      if(i==1)
      	 History.AlarmLevel|=0x02; 
      	else
				{ History.AlarmLevel&=0xfd;
				History.MiniterDelay[3]=0;//add 20210322
      }	
      }	  
void  TPreAlarmLow_Only(u8 i)
      {  if(i==1)
      	 History.AlarmLevel|=0x01; 
      	else
      	{  History.AlarmLevel&=0xfe;
				History.MiniterDelay[1]=0;//add 20210322
      }	
      }	

//---------------------------��λ�¶�-��-����-Ԥ��λ

void  TAlarmHi( )
      { TAlarmHi_Only(1);
         TPreAlarmHi_Only(0);
         TAlarmLow_Only(0);
         TPreAlarmLow_Only(0);
      }	  
void  TAlarmLow( )
      {	TAlarmHi_Only(0);
         TPreAlarmHi_Only(0);
         TAlarmLow_Only(1);
         TPreAlarmLow_Only(0);
      }	 

void  TPreAlarmHi( )
      { 
      TAlarmHi_Only(0);
         TPreAlarmHi_Only(1);
         TAlarmLow_Only(0);
         TPreAlarmLow_Only(0);
      }	  
void  TPreAlarmLow( )
      {  TAlarmHi_Only(0);
         TPreAlarmHi_Only(0);
         TAlarmLow_Only(0);
         TPreAlarmLow_Only(1);
      }	      	
      	 
   
      
void TNormalHi()
     {
        TAlarmHi_Only(0);
        TPreAlarmHi_Only(0);
     }


//---------------------------��λ�¶�-��-����-Ԥ��λ     
void TNormalLow()
     {  TAlarmLow_Only(0);
        TPreAlarmLow_Only(0);
     }     
       
void TNormal_level() 
   {   TNormalHi();
       TNormalLow() ;      
    }   
//=======================����ʪ��Ԥ��-����λ=================
void  RHAlarmHi_Only(u8 i)
      {   if(i==1)
      	 History.AlarmLevel|=0x80; 
      	else
				{ History.AlarmLevel&=0x7f;
				History.MiniterDelay[6]=0;//add 20210322
      }	
      }	  
void  RHAlarmLow_Only(u8 i)
      {   if(i==1)
      	 History.AlarmLevel|=0x40; 
      	else
				{ History.AlarmLevel&=0xbf;
				History.MiniterDelay[4]=0;//add 20210322
      }	
      }	 

void  RHPreAlarmHi_Only(u8 i)
      {  if(i==1)
      	 History.AlarmLevel|=0x08; 
      	else
				{ History.AlarmLevel&=0xf7;
				History.MiniterDelay[7]=0;//add 20210322
      }	
      }	  
void  RHPreAlarmLow_Only(u8 i)
      {  if(i==1)
      	 History.AlarmLevel|=0x04; 
      	else
				{ History.AlarmLevel&=0xfb;
					History.MiniterDelay[5]=0;//add 20210322
      }	
      }	 

      
//---------------------��λʪ��-��-Ԥ��-����λ---------------       
void RHNormalHi()
     {   RHAlarmHi_Only(0);
         RHPreAlarmHi_Only(0);
     }
//---------------------��λʪ��-��-Ԥ��-����λ--------------     
void RHNormalLow()
     {  RHAlarmLow_Only(0);
        RHPreAlarmLow_Only(0);
     }     
 
void  RHNormal_level()
     {  RHNormalHi();
     	RHNormalLow();
     }	
 
 
 
     
     
void  RHAlarmHi()
      {   RHAlarmHi_Only(1);
          RHPreAlarmHi_Only(0);
          RHAlarmLow_Only(0);
          RHPreAlarmLow_Only(0);
      }	  
void  RHAlarmLow()
      {   RHAlarmHi_Only(0);
          RHPreAlarmHi_Only(0);
          RHAlarmLow_Only(1);
          RHPreAlarmLow_Only(0);
      }	 

void  RHPreAlarmHi()
      {  RHAlarmHi_Only(0);
          RHPreAlarmHi_Only(1);
          RHAlarmLow_Only(0);
          RHPreAlarmLow_Only(0);
      }	  
void  RHPreAlarmLow()
      {  RHAlarmHi_Only(0);
          RHPreAlarmHi_Only(0);
          RHAlarmLow_Only(0);
          RHPreAlarmLow_Only(1);
      }	 
     
/*-----------------����Ϊ����������״̬--------     
 1:����ʹ�ܼ���,
 2:������ʱʱ�䵽,���ö�Ӧ��״̬    
-------------------------------------------------*/     
//----------------�¶ȸߵ�Ԥ����--����
void  TAlarmHi_AlarmFlag(u8 i)
      {if(i==1)  
       	History.AlarmStatus1|=0x20;
       else
         History.AlarmStatus1&=0xdf;  
      }	  
void  TAlarmLow_AlarmFlag(u8 i)
      { if(i==1)  
       	History.AlarmStatus1|=0x10;
       else
         History.AlarmStatus1&=0xef;  
      }	 

void  TPreAlarmHi_AlarmFlag(u8 i)
      {  if(i==1)  
       	History.AlarmStatus1|=0x02;
       else
         History.AlarmStatus1&=0xfd;  
      }	  
void  TPreAlarmLow_AlarmFlag(u8 i)
      {  if(i==1)  
       	History.AlarmStatus1|=0x01;
       else
         History.AlarmStatus1&=0xfe;  
      }	

 //----------------�¶ȸ�Ԥ����--��λ    
void TNormalHi_AlarmFlag()
     {
         TAlarmHi_AlarmFlag(0);
         TPreAlarmHi_AlarmFlag(0);
     }
 //----------------�¶ȵ�Ԥ����--��λ     
void TNormalLow_AlarmFlag()
     {
         TAlarmLow_AlarmFlag(0);
         TPreAlarmLow_AlarmFlag(0);
     }     
void    TNormal_AlarmFlag()
    {
    	TNormalHi_AlarmFlag();
    	TNormalLow_AlarmFlag();
    	
    }	
       
//----------===ʪ�ȸߵ�Ԥ��������====-------------------
void  RHAlarmHi_AlarmFlag(u8 i)
      {  if(i==1)  
       	History.AlarmStatus1|=0x80;
       else
         History.AlarmStatus1&=0x7f;  
      }	  
void  RHAlarmLow_AlarmFlag(u8 i)
      {  if(i==1)  
       	History.AlarmStatus1|=0x40;
       else
         History.AlarmStatus1&=0xbf;   
      }	 

void  RHPreAlarmHi_AlarmFlag(u8 i)
      {  if(i==1)  
       	History.AlarmStatus1|=0x08;
       else
         History.AlarmStatus1&=0xf7;  
      }	  
void  RHPreAlarmLow_AlarmFlag(u8 i)
      { if(i==1)  
       	History.AlarmStatus1|=0x04;
       else
         History.AlarmStatus1&=0xfb;  
      }	 

//-------------ʪ��-��Ԥ������λ-----------------------      
void RHNormalHi_AlarmFlag()
     {   RHAlarmHi_AlarmFlag(0);
        RHPreAlarmHi_AlarmFlag(0);
        
     }
//-------------ʪ��-��Ԥ������λ-----------------------     
void RHNormalLow_AlarmFlag()
     {   RHAlarmLow_AlarmFlag(0);
         RHPreAlarmLow_AlarmFlag(0);
       
     }
     
void RHNormal_AlarmFlag()
  {
     RHNormalHi_AlarmFlag();
     RHNormalLow_AlarmFlag();
  }    	
  	               