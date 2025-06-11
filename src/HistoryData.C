#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
//#define _GLOBAL_H
#include	"global.h"
extern void  StopBell();
void  DealAlarm_Bell();
//----------------------------------------------------
#if  0
struct Hisrory_Struct
   {  
   	 u8 AlarmLevel;
   	 u8 PowerVolume;
   	//-------------------- 
   	 u8 MiniterStartFlag[8];
   	 u8 MiniterDelay[8];
   	 u8 Alarm1Count[8];
   	 u8 AlarmCountOverFlag[8];
   	 u8 AlarmStatus1;
  	 u8 AlarmFlag;
  	 
   }; 
    
struct Hisrory_Struct   History;  

#endif


unsigned char   GetPower();
void ReSetAlarm(unsigned char i);
void   ReSet_All_Alarm_Para();
void Alarm_Initial();
void Timer_alarm_1s();
void DealAlarmCount();
void    DealAlarm1() ;
unsigned char  GetExtPowr();
void DealAlarm() ;
void  DealAlarm_Bell();
		
			
unsigned char   GetPower()
    {
      uint16_t  Volume;  	 
  	 Volume= GetAdc(0);
  	 History.PowerVolume=Volume*66/4096;
  	 if(History.PowerVolume>=37)
  	    History.PowerVolume=100;
  	 else if(History.PowerVolume<30)
  	     History.PowerVolume=0;  
  	 else
  	     History.PowerVolume=(37-History.PowerVolume)*100/7 ;    
  	  return(History.PowerVolume);
  	
    }


		
		
		
void ReSetAlarm(unsigned char i)
  {
         History.MiniterStartFlag[i]=0;
   	 History.MiniterDelay[i]=0;
   	 History.Alarm1Count[i]=0;
   	 History.AlarmCountOverFlag[i]=0;	
  	
   }

void   ReSet_All_Alarm_Para()
 {
    unsigned char  i;
    for(i=0;i<8;i++)
     ReSetAlarm(i) ;	
 	
  }
  
/*------------------------------------------------------------------
Function: void Alarm_Initial()
Input:
Output:
Description:  初始化报警的延时时间的标识,异常标识单元清零,报警标识单元清零
---------------------------------------------------------------------*/   	
void Alarm_Initial()
 {
    unsigned char  i;
    for(i=0;i<8;i++)
     ReSetAlarm(i) ;
    History.MiniterDelaySet[0]=ParaList[(cTempDownLimitAlarmDelay03D-cParaActual)*2+1];     //温度-报警下限 
    History.MiniterDelaySet[1]=ParaList[(cTempPreDownLimitAlarmDelay03D-cParaActual)*2+1] ; //温度-预警下限 
    History.MiniterDelaySet[2]=ParaList[(cTempUpLimitAlarmDelay03D -cParaActual)*2+1]  ;    //温度-报警上限 
    History.MiniterDelaySet[3]=ParaList[(cTempPreUpLimitAlarmDelay03D-cParaActual)*2+1];    //温度-预警上限 
    History.MiniterDelaySet[4]=ParaList[(cHumiDownLimitAlarmDelay03D-cParaActual)*2+1] ;    //湿度-报警下限 
    History.MiniterDelaySet[5]=ParaList[(cHumiPreDownLimitAlarmDelay03D-cParaActual)*2+1] ;  //湿度-预警下限   
    History.MiniterDelaySet[6]=ParaList[(cHumiUpLimitAlarmDelay03D-cParaActual)*2+1];        //湿度-报警上限 
    History.MiniterDelaySet[7]=ParaList[(cHumiPreUpLimitAlarmDelay03D-cParaActual)*2+1];      //湿度-预警上限 
    TNormal_level() ;
    RHNormal_level();
    TNormal_AlarmFlag();
    RHNormal_AlarmFlag(); 
 }    	
 	

/*------------------------------------------------------------------
Function: void Timer_alarm_1s()
Input:
Output:
Description:  1s定时中断,用来计算报警的延时时间,报警延时时间到,设置对应的报警位标识
              该程序在1s的定时器中自动运行,根据设置的启动计数开始进行数据累加
---------------------------------------------------------------------*/  
void Timer_alarm_1s()
 {
   unsigned char  i;
    for(i=0;i<8;i++)	
 	{
 	   if(History.MiniterStartFlag[i]==1)
 	     {  if(History.MiniterDelay[i]>=History.MiniterDelaySet[i])
 	              { 	History.AlarmCountOverFlag[i]=1;
 	               		History.MiniterStartFlag[i]=0;
 	               		switch(i)
 	               		 {case 0: TAlarmLow_AlarmFlag(1); break;
 	               		  case 1: TPreAlarmLow_AlarmFlag(1);break;
 	               		  case 2: TAlarmHi_AlarmFlag(1);  break;
 	               		  case 3: TPreAlarmHi_AlarmFlag(1); break;
 	               		  case 4: RHAlarmLow_AlarmFlag(1);break;
 	               		  case 5: RHPreAlarmLow_AlarmFlag(1);  break;
 	               		  case 6: RHAlarmHi_AlarmFlag(1); break;
 	               		  case 7: RHPreAlarmHi_AlarmFlag(1);break;
 	               		  default:  break;
 	               		}
 	               		
 	               }
 	        History.Alarm1Count[i]++;
 	        if(History.Alarm1Count[i]>=60)
 	          { History.Alarm1Count[i]=0;
 	            History.MiniterDelay[i]++;
 	            
 	            }   		
 		}
 	}	
  }	


/*------------------------------------------------------------------
Function: void DealAlarmCount()
Input:
Output:
Description:  无激活: 超温状态和报警状态都归零
                   超温状态=1:启动定时器开始计算延时 
                              定时器到期后,设置报警状态标识                                                            
---------------------------------------------------------------------*/   
void DealAlarmCount()
 { 
    //无激活
     if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))
         {
              ReSet_All_Alarm_Para();
              TNormal_level() ;
              RHNormal_level();
              TNormal_AlarmFlag();
  	      RHNormal_AlarmFlag();
  	  } 
   //有激活
     else
      {
 
      //温度无激活
        if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00) )
         {
         	TNormal_level() ;
         	TNormal_AlarmFlag();
  	 }	
       //温度激活
       else
       {  	
      //---------------------------------------------------------------          
  	  //报警下限未激活
  	 if( ParaList[(cTempDownLimitValid03D-cParaActual)*2]*256+ParaList[(cTempDownLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	    ReSetAlarm(0);  //不启动定时计算	
  	    TAlarmLow_AlarmFlag(0);
  	  }
  	//报警下限激活
  	else
  	  {  
  	    if ((History.AlarmLevel&0x10)!=0)
  	    {  
  	  	History.MiniterStartFlag[0]=1;
  	  	
  	     }
  	    else   
  	     { TAlarmLow_AlarmFlag(0);
  	       
  	     }   
  	}   
    
//---------------add  2015-12-05 17:07------------------------------------------------          
    //预警下限
     if( ParaList[(cTempPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreDownLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	   ReSetAlarm(1);  //不启动定时计算	
  	   TPreAlarmLow_AlarmFlag(0);
  	  }
  	else
  	  {  	
  	   if ((History.AlarmLevel&0x01)!=0)
  	      {History.MiniterStartFlag[1]=1;
  	    
  	       } 
  	     else
  	     { TPreAlarmLow_AlarmFlag(0);
  	     }
  	 }      
 
  	
//---------------报警上限-------------------------------------------
   if( ParaList[(cTempUpLimitValid03D-cParaActual)*2]*256+ParaList[(cTempUpLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	   	
  	   ReSetAlarm(2);  //不启动定时计算	
  	   TAlarmHi_AlarmFlag(0);
  	   OutputChange();
  	  }
  	else
  	  { 
            if( (History.AlarmLevel&0x20)!=0)
  	    {   History.MiniterStartFlag[2]=1;
  	     }
  	  
  	    else   
  	     { TAlarmHi_AlarmFlag(0);
  	     OutputChange();
  	     }   
  	  } 
  	
//----------------预警上限-----add  2015-12-05 17:11-------------------------------------  	
  	if( ParaList[(cTempPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreUpLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	    ReSetAlarm(3);  //不启动定时计算	
  	    TPreAlarmHi_AlarmFlag(0);	
  	 
  	  }
  	else
  	  { 
           if( (History.AlarmLevel&0x02)!=0)
  	  {   History.MiniterStartFlag[3]=1;
  	     }
  	    else   
  	     { 
  	     	TPreAlarmHi_AlarmFlag(0);
  	     	OutputChange();	
  	    }   
  	 } 
  	
   }	
   //温度激活处理完成，下面处理湿度激活 
  
  
   if(( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1])!=0x00)
         {
         	
         	RHNormal_level() ;
         	RHNormal_AlarmFlag();
  	 }	
      
       //湿度激活
       else
       {  	
  //-----------------湿度报警低-----------------------
       if( ParaList[(cHumiDownLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiDownLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	     ReSetAlarm(4);  //不启动定时计算	
  	     RHAlarmLow_AlarmFlag(0);	
  	    
  	  }
  	else
  	  {  
    
         if ((History.AlarmLevel&0x40)!=0)
  	     History.MiniterStartFlag[4]=1;
  	 else   
  	     RHAlarmLow_AlarmFlag(0);	     
  	     
  	  }
  	 
  	
//------------湿度预警低--------add 2015-12-05 17:14---------------------

if( ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	   	
  	     ReSetAlarm(5);  //不启动定时计算	
  	     RHPreAlarmLow_AlarmFlag(0);	
  	  }
  	else
  	  {  
            if((History.AlarmLevel&0x04)!=0)
               History.MiniterStartFlag[5]=1;
  	    else   
  	     RHPreAlarmLow_AlarmFlag(0);
  	  }      
  	 
//--------------湿度报警高------------------------------  	
  if( ParaList[(cHumiUpLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiUpLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	     ReSetAlarm(6);  //不启动定时计算	
  	     RHAlarmHi_AlarmFlag(0);
  	  }
  	else
  	  {  
           
            
            
  	if ((History.AlarmLevel&0x80)!=0)
  	  {   History.MiniterStartFlag[6]=1;
  	     } 
  	  
  	    else   
  	     { RHAlarmHi_AlarmFlag(0);
  	      
  	    }        
  	   }
  	   
  	  
//----------湿度预警高---------add 2015-12-05 17:14-------------------------  	
   if( ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2+1]!=0) 
  	  {
  	   	
  	     ReSetAlarm(7);  //不启动定时计算	
  	     RHPreAlarmHi_AlarmFlag(0);
  	  }
  	else
  	  {  	
  	
  	  if ((History.AlarmLevel&0x08)!=0)
  	    {  
  	    	History.MiniterStartFlag[7]=1;
  	    } 
  	  else   
  	     {
  	     	RHPreAlarmHi_AlarmFlag(0);
  	     }        
  	  }
//----------------------------------------        
   }
 }
} 
  	
/*------------------------------------------------------------------
Function: void    DealAlarm1()
Input:
Output:
Description:     1:将报警状态值存入到 VarList1[cRealDataAct1*2]
                 2: 待处理好所有报警后,将状态转移到VarList[cRealDataAct*2]=VarList1[cRealDataAct1*2];  
                 3: 根据报警状态,确定如何输出报警方式                                                     
---------------------------------------------------------------------*/     	
void    DealAlarm1() 
  {  
   	  History.AlarmFlag=6;
   //1   温湿度都没激活
    if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))
       {
       	
       	History.AlarmFlag=6;
       }
       //温湿度都激活的状态
    else  if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]==0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]==0x00))  
  	     	       		{ // AlarmOpen() ;
  	     	         		
  	     	          		if((History.AlarmStatus1&0x80)!=0x00) //有报警
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0x7f;  //设置报警标志
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x80; //清除预警标志
  	     	          		      
  	     	          		      
  	     	          		      
  	     	          		   }
  	     	          		if((History.AlarmStatus1&0x08)!=0x00)  //modified 2016-11-24 14:49
  	     	          		      { if(History.AlarmFlag==6) 
  	     	          		      	   History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0x7f;//设置预警标志
  	     	          		         VarList1[cRealDataAct1*2]|=0x80;  //清除报警标志
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x88)==0x00)  //modified 2016-11-24 14:49
  	     	          		     {   //AlarmFlag=6;
  	     	          		        VarList1[cRealDataAct1*2]|=0x80;    //清除报警标志
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x80;   //清除预警标志
  	     	          		       
  	     	          		     }
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x40)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xbf;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x40;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x04)!=0x00)
  	     	          		      { if(History.AlarmFlag==6)  
  	     	          		      	 History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0xbf;
  	     	          		         VarList1[cRealDataAct1*2]|=0x40;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x44)==0x00)
  	     	          		     {   //AlarmFlag=6; 
  	     	          		        VarList1[cRealDataAct1*2]|=0x40;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x40;
  	     	          		     }
  	     	          		
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x20)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xdf;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x20;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x02)!=0x00)
  	     	          		      {if(History.AlarmFlag==6)   
  	     	          		      	 History.AlarmFlag=7;
  	     	          		        VarList1[cRealDataAct1*2+1]&=0xdf;
  	     	          		        VarList1[cRealDataAct1*2]|=0x20;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x22)==0x00)
  	     	          		     {   // AlarmFlag=6;
  	     	          		        VarList1[cRealDataAct1*2]|=0x20;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x20;
  	     	          		     }	
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x10)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xef;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x10;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x01)!=0x00)
  	     	          		      { if(History.AlarmFlag==6) 
  	     	          		      	 History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0xef;
  	     	          		         VarList1[cRealDataAct1*2]|=0x10;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x11)==0x00)
  	     	          		     {   // AlarmFlag=6;
  	     	          		        VarList1[cRealDataAct1*2]|=0x10;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x10;
  	     	          		     }		
  	     	          	//-------------------------------------------------------	     
  	     	       
  	     	        }	     
  	     	          		     
  	     //------------------------------------ONLY HUMI--------------------------------------------  
      	      else  if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]==0x00))    //depend on temp
  	     	       		{ // AlarmOpen() ;
  	     	         		
  	     	          		if((History.AlarmStatus1&0x80)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0x7f;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x80;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x08)!=0x00)
  	     	          		      { if(History.AlarmFlag==6) 
  	     	          		      	   History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0x7f;
  	     	          		         VarList1[cRealDataAct1*2]|=0x80;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x88)==0x00)
  	     	          		     { //  AlarmFlag=6; 
  	     	          		        VarList1[cRealDataAct1*2]|=0x80;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x80;
  	     	          		       
  	     	          		     }
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x40)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xbf;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x40;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x04)!=0x00)
  	     	          		      { if(History.AlarmFlag==6)  
  	     	          		      	 History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0xbf;
  	     	          		         VarList1[cRealDataAct1*2]|=0x40;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x44)==0x00)
  	     	          		     {   //AlarmFlag=6; 
  	     	          		        VarList1[cRealDataAct1*2]|=0x40;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x40;
  	     	          		     }
  	     	          	  }	
  	     	          		
  	     	     //------------------------------------ONLY TEMP-------------------------------------------  
      	        else  if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]==0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))    //depend on temp    		
  	     	          	{	
  	     	          		
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x20)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xdf;
  	     	          		      VarList1[cRealDataAct1*2+1]|=0x20;
  	     	          		   }
  	     	          		if((History.AlarmStatus1&0x02)!=0x00)
  	     	          		      {if(History.AlarmFlag==6)   
  	     	          		      	 History.AlarmFlag=7;
  	     	          		        VarList1[cRealDataAct1*2+1]&=0xdf;
  	     	          		        VarList1[cRealDataAct1*2]|=0x20;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x22)==0x00)
  	     	          		     {  //  AlarmFlag=6;
  	     	          		        VarList1[cRealDataAct1*2]|=0x20;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x20;
  	     	          		     }	
  	     	          	//----------------------------------------------	     
  	     	          		if((History.AlarmStatus1&0x10)!=0x00)
  	     	          		   {  History.AlarmFlag=5;
  	     	          		      VarList1[cRealDataAct1*2]&=0xef;
  	     	          		       VarList1[cRealDataAct1*2+1]|=0x10;
  	     	          		   }
  	     	          		 if((History.AlarmStatus1&0x01)!=0x00)
  	     	          		      { if(History.AlarmFlag==6) 
  	     	          		      	 History.AlarmFlag=7;
  	     	          		         VarList1[cRealDataAct1*2+1]&=0xef;
  	     	          		         VarList1[cRealDataAct1*2]|=0x10;
  	     	          		      }
  	     	          		if((History.AlarmStatus1&0x11)==0x00)
  	     	          		     { // AlarmFlag=6; 
  	     	          		        VarList1[cRealDataAct1*2]|=0x10;
  	     	          		        VarList1[cRealDataAct1*2+1]|=0x10;
  	     	          		     }		
  	     	          	//-------------------------------------------------------	     
  	     	        }	     	          	
  	     	  //---------------------------------------------------------------------        	
  	
//-------------------------end of add 2015-12-05 18:03		
		  else
		           { //   AlarmOff() ;
		                History.AlarmFlag=6;
		                VarList1[cRealDataAct1*2]|=0xf0;  
		                VarList1[cRealDataAct1*2+1]|=0xf0;
		            } 
	//add 2019-03-28 13:45
	/**/
 
  	      
  	 
  	 //add 2016-5-21 13:53
  	    // EA=0;   2021-02-05 23:39
  	      VarList[cRealDataAct*2]=VarList1[cRealDataAct1*2];
  	      VarList[cRealDataAct*2+1]=VarList1[cRealDataAct1*2+1];
  	      VarList[cRealDataAct*2+2]=VarList1[cRealDataAct1*2+2];
  	      VarList[cRealDataAct*2+3]=VarList1[cRealDataAct1*2+3];
  	  #if  1   
  	      VarList[cTemp*2]=VarList1[cTemp1*2];
  	      VarList[cTemp*2+1]=VarList1[cTemp1*2+1];
  	      VarList[cHumi*2+0]=VarList1[cHumi1*2+0];
  	      VarList[cHumi*2+1]=VarList1[cHumi1*2+1];
  	  #endif    
  	  // EA=1;  2021-02-05 23:39    
  	   //end of add 2016-5-21 13:53 

   	     
 //add 2019-03-28 14:23
#if 0
 if(((j^VarList[cRealDataAct*2+1])&0x02)==0x02)
    VarList[cRealDataAct*2+2]&=0xfd;
	 #endif	
 } 
     

 //--------------------------------
 //Function: unsigned char  GetExtPowr()
 //
 //Description:获取外部供电状态
 //----------------------------------- 
unsigned char  GetExtPowr()
 {
 	return(GetPowerIn());
 }  
 

void Timer_Alarm_Delay_02D()
  {uint8_t  i;
   for(i=0;i<4;i++)
     { Alarm_Delay_02D_Count[i]++;
     if(Alarm_Delay_02D_Count[i]>=ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1])
     	Alarm_Delay_02D_OverFlag[i]=1;
     }	
  }   
  
 
 
 
void ClearAlarmFlag_02D(uint8_t  pos)
 {
   Alarm_Delay_02D_Count[pos]=0;
   Alarm_Delay_02D_OverFlag[pos]=0;	
 }	 
void  ClearAlarmFlag_02DAll()
  {
  	ClearAlarmFlag_02D(0) ; 
		        ClearAlarmFlag_02D(1) ;
		        ClearAlarmFlag_02D(2) ;
		        ClearAlarmFlag_02D(3) ;
   }
void DealAlarm_02D() 
  { unsigned char i,j,k; 
    int16_t temp,temp1;
	 if(BellOnOffStatus==1)   //开启状态
	   {   if(AlarmOutDepend_bak!=AlarmOutDepend)
	        {  
	        	 BellOnOffStatus=0;
	        	 AlarmOutDepend_bak=AlarmOutDepend;
	                 AlarmOpen();
	         }        
	       if(AlarmOuMode!=AlarmOuModeBak)
	         {
	                 BellOnOffStatus=0;
	        	  AlarmOuModeBak=AlarmOuMode;
	                 AlarmOpen();	
	         } 	
	    }
	          
	
	i=	 ParaList[9] ;
//----------------------------  	
  	if(i==1)  //local
  	 { 
  	 //计算温度	
  	    temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
  	    temp1=(int16_t)(ParaList[cAlarmUplimit*2]*256+ParaList[cAlarmUplimit*2+1]);
  	    if((temp>temp1)&((ParaList[cAlarmDepend*2+1]==0)||(ParaList[cAlarmDepend*2+1]==1)))
  	      History.AlarmLevel|=0x01; 
  	    else
  	     { History.AlarmLevel&=0xfe; 
  	       ClearAlarmFlag_02D(0) ;
  	     }  
  	    
  	    temp1=(int16_t)(ParaList[cAlarmDownlimit*2]*256+ParaList[cAlarmDownlimit*2+1]);   
  	     if((temp<temp1)&((ParaList[cAlarmDepend*2+1]==0)||(ParaList[cAlarmDepend*2+1]==1)))
  	      History.AlarmLevel|=0x02; 
  	    else
  	    {  History.AlarmLevel&=0xfd; 
  	      ClearAlarmFlag_02D(1) ; 
  	    }  
 //-----------计算湿度------------------ 	      
  	    temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
  	    temp1=(int16_t)(ParaList[cHumiUpLimit*2]*256+ParaList[cHumiUpLimit*2+1]);  
  	    if((temp>temp1)&((ParaList[cAlarmDepend*2+1]==0)||(ParaList[cAlarmDepend*2+1]==2)))
  	      History.AlarmLevel|=0x04; 
  	    else
  	     { History.AlarmLevel&=0xfb; 
  	       ClearAlarmFlag_02D(2) ;
  	     }
  	    temp1=(int16_t)(ParaList[cHumiDownLimit*2]*256+ParaList[cHumiDownLimit*2+1]);   
  	     if((temp<temp1)&((ParaList[cAlarmDepend*2+1]==0)||(ParaList[cAlarmDepend*2+1]==2)))
  	      History.AlarmLevel|=0x08; 
  	    else
  	     { History.AlarmLevel&=0xf7;    
  	       ClearAlarmFlag_02D(3) ;
  	      } 
//------------------------------------  	
  	  if(History.AlarmLevel!=0)
  	    {       
		    if(AlarmOutDepend==0)  //温湿度
  	     	      {
  	     	        if(Alarm_Delay_02D_OverFlag[0]+Alarm_Delay_02D_OverFlag[1]+Alarm_Delay_02D_OverFlag[2]+Alarm_Delay_02D_OverFlag[3]!=0)
  	     	             AlarmOpen() ;
  	     	       }  
  	//----------------------    
								 else if(AlarmOutDepend==1) //温度, 超高且延时到   或者超低且延时到
		      { ClearAlarmFlag_02D(2) ;
		        ClearAlarmFlag_02D(3) ;
		      	if((History.AlarmLevel&0x01)!=0)
		      	  {
		      	    if(Alarm_Delay_02D_OverFlag[0]==1)
		      	       AlarmOpen() ;
		      	  }
		      	if((History.AlarmLevel&0x02)!=0)
		      	  {
		      	    if(Alarm_Delay_02D_OverFlag[1]==1)
		      	       AlarmOpen() ;
		      	  } 
		      	 if(Alarm_Delay_02D_OverFlag[0]+Alarm_Delay_02D_OverFlag[1]==0)
		           AlarmOff() ; 
           }						 
//--------------------------------------		      		     } 	   
		  else if(AlarmOutDepend==2) //湿度, 超高且延时到   或者超低且延时到
		      { ClearAlarmFlag_02D(0) ;
		        ClearAlarmFlag_02D(1) ;
		      	if((History.AlarmLevel&0x04)!=0)
		        { if(Alarm_Delay_02D_OverFlag[2]==1)
		            AlarmOpen() ;
		         }
		      if((History.AlarmLevel&0x08)!=0)
		        { if(Alarm_Delay_02D_OverFlag[3]==1)
		            AlarmOpen() ;
		         }   
		      if(Alarm_Delay_02D_OverFlag[3]+Alarm_Delay_02D_OverFlag[2]==0)
		           AlarmOff() ;
		      }
//-------------------------------------		        	   
		   else  //不依赖
		      {  AlarmOff() ; 
		        ClearAlarmFlag_02DAll();
		      }
            }
  	   else
  	     { 
  	         AlarmOff() ;
  	         ClearAlarmFlag_02DAll();
  	     }
  	    
     }
//----------------------------------------------  	 
 else  	if(i==2)   //远程报警
  	 {
  	 
#ifdef _Loramain			 
			 j=0; 
			 for(k=0;k<AddrLen;k++)
  	    j+=ParaList[cAlarmControlPos*2+k*2+1];
  	   if(j!=0)
  	      AlarmOpen() ;
  	   else
  	    {  AlarmOff() ; 
  	    ClearAlarmFlag_02DAll();
  	    }  
 #else
	if(ParaList[cRemoteControl*2+1]==1)
  	      AlarmOpen() ;
  	   else
  	    {  AlarmOff() ;
  	       ClearAlarmFlag_02DAll();     
	    }		 
			 #endif 
			 
  	 }
//---------------- -----------------------------------  	     
  	  
  	  else      //不报警
	   {AlarmOff() ; 
	    ClearAlarmFlag_02DAll();
	}    
  	
  }            

 
/*--------------------------------
 //Function:  DealAlarm() 
 //Input:
   Output:
 //Description:处理
 //-----------------------------------*/

void DealAlarm_03D() 
  {  
#if   0		
		unsigned char i,j;
     SetAlarmLevel();  //检查是否有报警状态
     OutputChange();
		DealAlarmCount();	
		OutputChange();
     if(History.AlarmLevel==0)
        History.AlarmStatus1=0;
   //add 2019-03-28 15:53     
    DealAlarm1() ;
		 OutputChange();
    
//end of add 2019-03-28 15:57    
    
//delete 2015-05-14 12:43     VarList[18]|=(AlarmStatus1^0xf0);  //alarm flag
     VarList[cRealDataAct*2]&=0xfc; //sensor mode 
 
 if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)
 && ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))
   
   VarList[cRealDataAct*2]|=0x03;   //2019-04-26 21:10 all invlaid
else if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)
 && ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]==0x00))
   VarList[cRealDataAct*2]|=0x02;   //2019-04-26 21:10 only humi  
 else if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]==0x00)
 && ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))
   VarList[cRealDataAct*2]|=0x01;   //2019-04-26 21:10 only Temp 
 else ;
 //    VarList[cRealDataAct*2]|=ParaList[(cSensorMode03D-cParaActual)*2+1]; //sensor mode 


//-------------//start status
     if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0)	 
      VarList[cRealDataAct*2]&=0xf7;   
     else
       VarList[cRealDataAct*2]|=0x08;   


    OutputChange();
//------------     //power   low 
#if 0
  // VarList[cRealDataAct*2+2]&=0x0f;
   VarList[cRealDataAct*2+3]=GetPower();               //2015-12-05 21:25
  if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0)
    { //VarList[cRealDataAct*2+3]=GetPower();
    	if(GetPower()<ParaList[(cPowerAlarmValue03D-cParaActual)*2+1] )
         VarList[cRealDataAct*2+1]&=0xf7;
      else  
         VarList[cRealDataAct*2+1]|=0x08; 
     }
   else
       VarList[cRealDataAct*2+1]|=0x08; 
 #endif           
         CheckPowerLowAlarmValid();  //add 2019-03-29 9:52


OutputChange();
//-------------  蜂鸣器报警--------------
CheckBellAlarmValid();//add 2019-03-29 9:50

//-------------  extern power 检测断电情况--------------

      CheckPowerOffAlarm();//add 2019-03-29 9:33      	   
//--------------------  sensor error--------------- 
   CheckSensorWrongAlarmValid();//add 2019-03-29 9:39   
//-----------处理变化状态------------------------------  
    if(((VarList[cRealDataAct*2]&0xf8)!=(VarListBak[0]&0xf8))||(VarList[cRealDataAct*2+1]!=VarListBak[1]))
        {                                                //温度报警                                                  //温度预警
            if((((VarList[cRealDataAct*2]^VarListBak[0])&0x30)!=0x00)|| (((VarList[cRealDataAct*2+1]^VarListBak[1])&0x30)!=0))
                  //VarList[cRealDataAct*2+2]|=0x40;
                  VarList[cRealDataAct*2+2]&=0xbf;
            else
               // VarList[cRealDataAct*2+2]&=0xbf;
               VarList[cRealDataAct*2+2]|=0x40;   //modified 2016-5-22 22:28
        //----------------------------        
         
         
       //----------------------------              //湿度报警                           //湿度预警
           if((((VarList[cRealDataAct*2]^VarListBak[0])&0xc0)!=0x00)|| (((VarList[cRealDataAct*2+1]^VarListBak[1])&0xc0)!=0))
                  VarList[cRealDataAct*2+2]&=0x7f;   
            else
                 
                 VarList[cRealDataAct*2+2]|=0x80;
        
      //-------------------------断电变化---        
           if(((VarList[cRealDataAct*2+1]^VarListBak[1])&0x01)==0x01)
               {  //modified  2019-08-25 5:58 
                  VarList[cRealDataAct*2+2]&=0xfe; 
                  if(GetExtPowr()==0)  //from NO POWER  to PowerOn
                    LCD_init( );
                  //end of modified 2019-08-25 5:59
               }   
            else
                  VarList[cRealDataAct*2+2]|=0x01;      
       //------------------------声光变化----        
         #if 0 
           if(((VarList[cRealDataAct*2+1]^VarListBak[1])&0x02)==0x02)
                 
                  VarList[cRealDataAct*2+2]&=0xfd; 
            else
         #endif    
                  VarList[cRealDataAct*2+2]|=0x02;      
       //-----------------------传感器异常状态变化-----        
           if(((VarList[cRealDataAct*2+1]^VarListBak[1])&0x04)==0x04)
                  
                  VarList[cRealDataAct*2+2]&=0xfb;  
            else
                 VarList[cRealDataAct*2+2]|=0x04;     
       //-----------------------低电量状态变化-----        
           if(((VarList[cRealDataAct*2+1]^VarListBak[1])&0x08)==0x08)
                  
                  VarList[cRealDataAct*2+2]&=0xf7;    
            else
                  VarList[cRealDataAct*2+2]|=0x08;  
           
      //----------------------启动状态变化------          //start  status
      //modified 2016-5-22 22:35
      
         if((VarList[cRealDataAct*2]&0x08)!=(VarListBak[0]&0x08))
                 { //2019-04-29 17:33if(CheckStartStopStatus())
                          VarList[cRealDataAct*2+2]&=0xef;   
                 //  else
                 //        VarList[cRealDataAct*2+2]|=0x10;
                          
      //             ES0=0; //ADD 2016-5-22 21:48
            	History.AlarmRecordFlag=1;
            //add 2019-03-28 13:06 
         
         
      //add 2019-04-07 12:49         
           if(CheckStartStopStatus()==0)   //处于非启动状态   2021-05-26 11:44
             { if((VarList[cRealDataAct*2]&0xf0)!=0xf0)  //报警记录状态
           //end 2015-05-30 18:27
              {  j= (ParaList[(cAlarmHistoryRecInt-cParaActual)*2]*256+ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1])/60;
                 if(VarList[9]%j==0)
                   { if(VarList[9]!=RecordNextTime)
                        {   RecordNextTime=VarList[9];
                            History.AlarmRecordFlag=11;
                            storeARecord();
                        
          	           } 
			 else
          	   		    storeARecord();
          	   	//printf("Status  Record :1 \r\n");	    
          	   } 
          	}   
           else   //正常记录状态
             { 
          	 j= (ParaList[(cNormalHistoryRecInt-cParaActual)*2]*256+ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])/60;
          	if(VarList[9]%j==0)
                   { if(VarList[9]!=RecordNextTime)
                        {    RecordNextTime=VarList[9];
                             History.AlarmRecordFlag=11;
                             storeARecord();
                       }
					  else
					    storeARecord();
					   // printf("Status  Record :2 \r\n");
                    }
               }        
          	
            
            
            
      //end of add 2019-04-07 12:50       
                
            //end of add 2019-03-28 13:06  
               
               History.AlarmRecordFlag=0;
               
               
//               ES0=1;  //ADD 2016-5-22 21:48
           
           
          //--add 2019-04-29 10:44-------------------------------- 	

                 }
             }    
         else
           VarList[cRealDataAct*2+2]|=0x10;             
     //         
  OutputChange();         
#ifdef  _BoHuaiVer   
  //add 2019-04-27 11:22  在启动记录的情况下，所有的报警信息都记录一条数据
  //                      在不启动记录的情况下，如果启停状态发生了改变，也记录一条记录
           if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0)       //add 2016-5-18 15:14
            {  //ES0=0; //ADD 2016-5-22 21:48
            	History.AlarmRecordFlag=1;
            //add 2019-03-28 13:06 
         
         
      //add 2019-04-07 12:49         
               
              if((VarList[cRealDataAct*2]&0xf0)!=0xf0)
           //end 2015-05-30 18:27
              {  j= (ParaList[(cAlarmHistoryRecInt-cParaActual)*2]*256+ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1])/60;
                 if(VarList[9]%j==0)
                   { if(VarList[9]!=RecordNextTime)
                        {   RecordNextTime=VarList[9];
                            History.AlarmRecordFlag=11;
                            storeARecord();
                        
          	           } 
					 else
          	   		    storeARecord();
          	   		    //printf("Status  Record :3 \r\n");
          	   } 
          	}   
           else
             { 
          	 j= (ParaList[(cNormalHistoryRecInt-cParaActual)*2]*256+ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])/60;
          	if(VarList[9]%j==0)
                   { if(VarList[9]!=RecordNextTime)
                        {    RecordNextTime=VarList[9];
                             History.AlarmRecordFlag=11;
                             storeARecord();
                       }
					  else
					    storeARecord();
					 //   printf("Status  Record :4 \r\n");
                    }
               }        
          	
            
            
            
      //end of add 2019-04-07 12:50       
                
            //end of add 2019-03-28 13:06  
               
               History.AlarmRecordFlag=0;
               
               
//               ES0=1;  //ADD 2016-5-22 21:48
           
           
          //--add 2019-04-29 10:44-------------------------------- 	
  
           
            } 
	  
       
#endif            
            
            	
        }
      else
        VarList[cRealDataAct*2+2]=0xff;  
      VarListBak[0]=VarList[cRealDataAct*2];
      VarListBak[1]=VarList[cRealDataAct*2+1];
     DealAlarm_Bell();
     OutputChange(); 
  //3
  //-------------------------------
  // else   ;  
        //end of add 2019-04-29 10:44   
#endif    	
  }              


void DealAlarm()
{
	#ifdef  _BL02D
	  DealAlarm_02D();
	#endif
	
	#ifdef  _BL03D
	  DealAlarm_03D();
	#endif
}


/*--------------------------------
 //Function: DealAlarm_Bell()
 //Input:
   Output:
 //Description:处理蜂鸣器的输出状态,
 //-----------------------------------*/
void  DealAlarm_Bell()
   {  unsigned char  i;
   	 i= ParaList[(cAlarmOutControl03D-cParaActual)*2+1] ;
  	
  //-----------------------------------------------	
  	if(i==0)
  	  {
		if(History.AlarmStatus1==0)
				{ StopBell();
					History.AlarmFlagBak=6;
					AlarmLEDInital();
				}
			//------------------------
	if(BellAlarmValid() ==1)  //local  ,fixed
            { 
            	  if(History.AlarmFlag==6)
                   {    History.AlarmFlagBak=6;
		        StopBell();
                        AlarmLEDInital();								
	           } 
                else if((History.AlarmFlag==5)&&(History.AlarmFlagBak!=5))
                   {	 History.AlarmFlagBak=5;
	      	 
		               Bellcontrol(255,50,200);
	                 AlarmLEDcontrol(255,50,200);
	            }  
	           else if((History.AlarmFlag==7)&&(History.AlarmFlagBak!=7))
	               {  Bellcontrol(255,50,400 );
			   AlarmLEDcontrol(255,50,400);
			   History.AlarmFlagBak=7;
	      	      }	
	       		 
          } 
        else  //add 2021-03-22 9:49  
          {  StopBell();  //如果蜂鸣器不启用,停止蜂鸣器
	     AlarmLEDInital();
	     History.AlarmFlagBak=6;
           }
        }
					
  //----------------------------------------------  	 
  	else if((i==1)&&(BellAlarmValid() ==1))   //remote
  	 {
  	  
  	   //   Bellcontrol(255,1,1);
  	 if( History.AlarmFlagBak!=5)  
		 { Bellcontrol(255,100,200);
	         AlarmLEDcontrol(255,100,200);
			    History.AlarmFlagBak=5;
  	   }
  	   
		 }    
      else	    
  //	else if((i==2)||(BellAlarmValid() ==0))
  	  //add 2019-04-29 15:52
  	   {   StopBell();  
  	       AlarmLEDInital();
	       History.AlarmFlagBak=6;
  	      
  	    }
	} 			