#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
//extern  Hisrory_Struct   History;

extern  uint8_t uart0_Var_List[320];

#define    u8  unsigned char
#define    u16  uint16_t


struct Key_Alarm
   {  
  	 uint8_t LevelH;
  	 uint8_t LevelL;
  	 uint8_t LevelHBak;
  	 uint8_t LevelLBak;
  	 uint8_t LevelH_Alarm;
  	 uint8_t LevelL_Alarm;
  	 uint16_t Key_Interval[16];
  	 uint8_t Key_IntervalBegin[16];
         uint8_t Key_IntervalTimeOvr[16];
         uint16_t Key_SampleIntervalSet[16];
  	 
    };    

 struct Key_Alarm   Key_Alarm1;
   
void    Key_Alarm_inital()
   {
     uint8_t i;
    int16_t  temp;
 		 uint16_t  temp1; 
     Key_Alarm1.LevelH=0;
     Key_Alarm1.LevelL=0;
     Key_Alarm1.LevelHBak=0;
     Key_Alarm1.LevelLBak=0;
     Key_Alarm1.LevelH_Alarm=0;
     Key_Alarm1.LevelL_Alarm=0;
		 InitialTemp();
     for(i=0;i<16;i++)
      {
      	Key_Alarm1.Key_Interval[16]=0;
  	Key_Alarm1.Key_IntervalBegin[i]=0;
        Key_Alarm1.Key_IntervalTimeOvr[i]=0;
        Key_Alarm1.Key_SampleIntervalSet[i]=(ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])*60;
      
      
		 }  
  GPIO_SetMode(PC, BIT10, GPIO_PMD_OUTPUT);
    PC10=0; 
 } 


	
void Key_Alarm_AlarmHDelayStart(uint8_t i) 
{
    Key_Alarm1.Key_Interval[i]=0;
   Key_Alarm1.Key_IntervalBegin[i]=1;
   Key_Alarm1.Key_IntervalTimeOvr[i]=0;
   Key_Alarm1.Key_SampleIntervalSet[i]=(ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])*60;
   	
} 
void Key_Alarm_AlarmHDelayStop(uint8_t i) 
{
    Key_Alarm1.Key_Interval[i]=0;
   Key_Alarm1.Key_IntervalBegin[i]=0;
   Key_Alarm1.Key_IntervalTimeOvr[i]=0;
   Key_Alarm1.Key_SampleIntervalSet[i]=(ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])*60;
   	
} 

void Key_Alarm_AlarmLDelayStart(uint8_t i) 
{
    Key_Alarm1.Key_Interval[i+8]=0;
   Key_Alarm1.Key_IntervalBegin[i+8]=1;
   Key_Alarm1.Key_IntervalTimeOvr[i+8]=0;
   Key_Alarm1.Key_SampleIntervalSet[i+8]=(ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])*60;
   	
} 
void Key_Alarm_AlarmLDelayStop(uint8_t i) 
{
    Key_Alarm1.Key_Interval[i+8]=0;
   Key_Alarm1.Key_IntervalBegin[i+8]=0;
   Key_Alarm1.Key_IntervalTimeOvr[i+8]=0;
   Key_Alarm1.Key_SampleIntervalSet[i+8]=(ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])*60;
   	
}    
   
void Timer_Key_Alarm()  
 {
  uint8_t i;
  for(i=0;i<16;i++)
   {
    if(Key_Alarm1.Key_IntervalBegin[i]==1)
		{ Key_Alarm1.Key_Interval[i]++;
     if( Key_Alarm1.Key_Interval[i]> Key_Alarm1.Key_SampleIntervalSet[i])
      {
      	 Key_Alarm1.Key_IntervalTimeOvr[i]=1;
      	 Key_Alarm1.Key_IntervalBegin[i]=0;  		
     } 
   }
}	 
}

/*------------------------------------------------------------------
Function: u8  DealWith_Temp_UpLimit()
Input:
Output:
Description:                                  
---------------------------------------------------------------------*/  	 
u8  DealWith_TempHumi_Alarm()
    { int16_t temp,temp1,temp2;
    	uint8_t  i; 
 if(ParaList[cTempType*2+1]!=1)   	
 {		for(i=0;i<ParaList[cTempNum*2+1];i++)
    	   {
    	      temp=(int16_t)(GetTempValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;	
			 temp2=-cInValidTempNouse;			 
            if((temp!=temp1)&&(temp!=temp2))//2015-05-30 8:36
             { 
  	      temp1=(int16_t)(ParaList[cAlarmUplimit*2]*256+ParaList[cAlarmUplimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cAlarmDownlimit*2]*256+ParaList[cAlarmDownlimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2))  
  	       {  
  	         Key_Alarm1.LevelH|=(1<<i);
  	         if(Key_Alarm1.LevelH!= Key_Alarm1.LevelHBak)
  	          {  Key_Alarm1.LevelHBak= Key_Alarm1.LevelH;
  	               {
  	                  Key_Alarm_AlarmHDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);
  	       }
  	       
  	     }     
  	  //----------------------------- 
  	  temp=(int16_t)(GetHumiValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;
 temp2=-cInValidTempNouse;			 
            if((temp!=temp1)&&(temp!=temp2))				 
         
  	     {
    	     //---------------temp  hi         
	      temp1=(int16_t)(ParaList[cHumiUpLimit*2]*256+ParaList[cHumiUpLimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cHumiDownLimit*2]*256+ParaList[cHumiDownLimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2) ) 
  	       {  
  	         Key_Alarm1.LevelL|=(1<<i);
  	         if(Key_Alarm1.LevelL!= Key_Alarm1.LevelLBak)
  	          {  Key_Alarm1.LevelLBak= Key_Alarm1.LevelL;
  	               {
  	                  Key_Alarm_AlarmLDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelL&=((1<<i)^0xff);
            Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
  	       }   
				 }
  	 }
//----------------------------------------------------				 
	 if(ParaList[cTempType*2+1]==2)
	 { for(i=5;i<8;i++)
    	   {
    	      temp=(int16_t)(GetTempValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;	
        temp2=-cInValidTempNouse;			 
            if((temp!=temp1)&&(temp!=temp2))
             { 
  	      temp1=(int16_t)(ParaList[cAlarmUplimit*2]*256+ParaList[cAlarmUplimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cAlarmDownlimit*2]*256+ParaList[cAlarmDownlimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2))  
  	       {  
  	         Key_Alarm1.LevelH|=(1<<i);
  	         if(Key_Alarm1.LevelH!= Key_Alarm1.LevelHBak)
  	          {  Key_Alarm1.LevelHBak= Key_Alarm1.LevelH;
  	               {
  	                  Key_Alarm_AlarmHDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);
  	       }
  	       
  	     }     
  	  //----------------------------- 
  	  temp=(int16_t)(GetHumiValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;	
          temp2=-cInValidTempNouse;			 
            if((temp!=temp1)&&(temp!=temp2))
  	     {
    	     //---------------temp  hi         
	      temp1=(int16_t)(ParaList[cHumiUpLimit*2]*256+ParaList[cHumiUpLimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cHumiDownLimit*2]*256+ParaList[cHumiDownLimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2) ) 
  	       {  
  	         Key_Alarm1.LevelL|=(1<<i);
  	         if(Key_Alarm1.LevelL!= Key_Alarm1.LevelLBak)
  	          {  Key_Alarm1.LevelLBak= Key_Alarm1.LevelL;
  	               {
  	                  Key_Alarm_AlarmLDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelL&=((1<<i)^0xff);
            Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
  	       }   
				 }
  	       
  	     
  	    //-----------------------
  	     
   }
for(i=ParaList[cTempNum*2+1];i<5;i++)	 
	       {    Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);	
		         Key_Alarm1.LevelL&=((1<<i)^0xff);
             Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
				 }				 
	 
	 
 }
else
{ for(i=ParaList[cTempNum*2+1];i<8;i++)	 
	       {    Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);	
		         Key_Alarm1.LevelL&=((1<<i)^0xff);
             Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
				 }				 
}	
//---------------------------
 }
 
 
 //---------------------
 else //ntc
{
	 for(i=0;i<3;i++)
    	   {
    	      temp=(int16_t)(GetTempValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36
             { 
  	      temp1=(int16_t)(ParaList[cAlarmUplimit*2]*256+ParaList[cAlarmUplimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cAlarmDownlimit*2]*256+ParaList[cAlarmDownlimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2))  
  	       {  
  	         Key_Alarm1.LevelH|=(1<<i);
  	         if(Key_Alarm1.LevelH!= Key_Alarm1.LevelHBak)
  	          {  Key_Alarm1.LevelHBak= Key_Alarm1.LevelH;
  	               {
  	                  Key_Alarm_AlarmHDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);
  	       }
  	       
  	     }     
  	  //----------------------------- 
  	  temp=(int16_t)(GetHumiValue(i)); 
    	     //---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36  
  	     {
    	     //---------------temp  hi         
	      temp1=(int16_t)(ParaList[cHumiUpLimit*2]*256+ParaList[cHumiUpLimit*2+1]); 
  	      temp2=(int16_t)(ParaList[cHumiDownLimit*2]*256+ParaList[cHumiDownLimit*2+1]); 
  	    //----check over alarm
  	     if((temp>temp1)||(temp<temp2) ) 
  	       {  
  	         Key_Alarm1.LevelL|=(1<<i);
  	         if(Key_Alarm1.LevelL!= Key_Alarm1.LevelLBak)
  	          {  Key_Alarm1.LevelLBak= Key_Alarm1.LevelL;
  	               {
  	                  Key_Alarm_AlarmLDelayStart(i);
  	               }   
  	        }
  	      }
  	     else
  	       { Key_Alarm1.LevelL&=((1<<i)^0xff);
            Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
  	       }   
				 }
  	}
			for(i=3;i<8;i++)	 
	       {    Key_Alarm1.LevelH&=((1<<i)^0xff);
             Key_Alarm1.LevelH_Alarm&=((1<<i)^0xff);						 
  	         Key_Alarm_AlarmHDelayStop(i);	
		         Key_Alarm1.LevelL&=((1<<i)^0xff);
             Key_Alarm1.LevelL_Alarm&=((1<<i)^0xff);					 
  	         Key_Alarm_AlarmLDelayStop(i);
				 }
}
}
//------------------------end if ntc	


void DealWith_T_Alarm()
   {uint8_t i;
   	DealWith_TempHumi_Alarm();
   	for(i=0;i<16;i++)
   	 {
   	    if(Key_Alarm1.Key_IntervalTimeOvr[i]==1)
   	      {
   	      	Key_Alarm1.Key_IntervalTimeOvr[i]=0;
   	      	if(i<8)
   	      	  Key_Alarm1.LevelH_Alarm|=(1<<i);
   	        else
   	          Key_Alarm1.LevelL_Alarm|=(1<<(i-8));	  
   	      }	
   	 }
		uart0_Var_List[cAlarmPos*2]=Key_Alarm1.LevelH_Alarm; 
		uart0_Var_List[cAlarmPos*2+1]=Key_Alarm1.LevelL_Alarm ;
#if 0		 
   	for(i=0;i<8;i++) 
   	{   if(((Key_Alarm1.LevelH_Alarm&(1<<i))!=0)||(Key_Alarm1.LevelL_Alarm&(1<<i)!=0))
   		     SetTempALarm(i);
   	    else
   	        ClearTempALarm(i); 	
   	}
#endif		
   }

void BoxAlarmOn()
 {
  if(uart0_Var_List[AlarmStartAddr*2+1]!=1)
	 PC10=1;
  else
   PC10=0;		
}
void  BoxAlarmOff()
{
   PC10=0;
}
 
void  DealDoorTempAlarm()  
 {  DealWith_T_Alarm();
   if(GetAlarmStatus()!=0)
     BoxAlarmOn();
   else
      BoxAlarmOff();   
 }            