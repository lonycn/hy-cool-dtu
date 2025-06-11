#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#define    u8  unsigned char
#define    u16  uint16_t
//--------------单独-----设置温度报警位
void  TAlarmHi_Only(u8 i);
void  TAlarmLow_Only(u8 i);
void  TPreAlarmHi_Only(u8 i);
void  TPreAlarmLow_Only(u8 i);
//-------------针对排他性-------------------  
void  TAlarmHi( );
void  TAlarmLow( );
void  TPreAlarmHi( );
void  TPreAlarmLow( );
//-------------针对高低限位--------复位湿度-高-预警-报警位---------------       
void TNormalHi();
void TNormalLow();
void TNormal_level(); 
//=============单独==========设置湿度预警-报警位=================
void  RHAlarmHi_Only(u8 i);
void  RHAlarmLow_Only(u8 i);
void  RHPreAlarmHi_Only(u8 i);
void  RHPreAlarmLow_Only(u8 i);
 //-------------针对高低限位--------复位湿度-高-预警-报警位---------------       
void RHNormalHi();
void RHNormalLow();
void  RHNormal_level();
//-------------针对排他性-------------------     
void  RHAlarmHi();
void  RHAlarmLow();
void  RHPreAlarmHi();
void  RHPreAlarmLow();

/*-----------------以下为设置允许报警状态--------     
 1:报警使能激活,
 2:报警延时时间到,设置对应的状态    
-------------------------------------------------*/     
//----------单独------温度高低预报警--设置
void  TAlarmHi_AlarmFlag(u8 i);
void  TAlarmLow_AlarmFlag(u8 i);
void  TPreAlarmHi_AlarmFlag(u8 i);
void  TPreAlarmLow_AlarmFlag(u8 i);
 //----------针对高低------温度高预报警--复位    
void TNormalHi_AlarmFlag();
void TNormalLow_AlarmFlag();
void    TNormal_AlarmFlag();
//-------单独-----===湿度高低预报警设置====-------------------
void  RHAlarmHi_AlarmFlag(u8 i);
void  RHAlarmLow_AlarmFlag(u8 i);
void  RHPreAlarmHi_AlarmFlag(u8 i);
void  RHPreAlarmLow_AlarmFlag(u8 i);
//----------针对高低------湿度-高预报警复位-----------------------      
void RHNormalHi_AlarmFlag();
void RHNormalLow_AlarmFlag();
void RHNormal_AlarmFlag();

//以下根据温度比较,判断是否发生超温,不考虑是否启动使能

//-------------------设置温度报警位
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

//---------------------------复位温度-高-报警-预警位

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


//---------------------------复位温度-低-报警-预警位     
void TNormalLow()
     {  TAlarmLow_Only(0);
        TPreAlarmLow_Only(0);
     }     
       
void TNormal_level() 
   {   TNormalHi();
       TNormalLow() ;      
    }   
//=======================设置湿度预警-报警位=================
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

      
//---------------------复位湿度-高-预警-报警位---------------       
void RHNormalHi()
     {   RHAlarmHi_Only(0);
         RHPreAlarmHi_Only(0);
     }
//---------------------复位湿度-低-预警-报警位--------------     
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
     
/*-----------------以下为设置允许报警状态--------     
 1:报警使能激活,
 2:报警延时时间到,设置对应的状态    
-------------------------------------------------*/     
//----------------温度高低预报警--设置
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

 //----------------温度高预报警--复位    
void TNormalHi_AlarmFlag()
     {
         TAlarmHi_AlarmFlag(0);
         TPreAlarmHi_AlarmFlag(0);
     }
 //----------------温度低预报警--复位     
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
       
//----------===湿度高低预报警设置====-------------------
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

//-------------湿度-高预报警复位-----------------------      
void RHNormalHi_AlarmFlag()
     {   RHAlarmHi_AlarmFlag(0);
        RHPreAlarmHi_AlarmFlag(0);
        
     }
//-------------湿度-低预报警复位-----------------------     
void RHNormalLow_AlarmFlag()
     {   RHAlarmLow_AlarmFlag(0);
         RHPreAlarmLow_AlarmFlag(0);
       
     }
     
void RHNormal_AlarmFlag()
  {
     RHNormalHi_AlarmFlag();
     RHNormalLow_AlarmFlag();
  }    	
  	               