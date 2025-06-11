#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

extern  void AlarmLEDInital( );
void  DelayhalfSecond(unsigned char n)
  {
 	 Interval=0;
      	  while(Interval<n)
      	  WatchdogReset(); 
  }   
 
unsigned char  JudgePara()
  {
unsigned char 	i=1;
unsigned char 	m;
  
      
         if((ParaList[cDeviceNum*2+1]==0x00)||(ParaList[cDeviceNum*2+1]==0xff))
           i=0;
         if(ParaList[(cSaveDataFlag03D-cParaActual)*2]*256+ParaList[(cSaveDataFlag03D-cParaActual)*2+1]>1)
           i=0;
     //modified 2016-7-17 7:27   
         if(((ParaList[(cNormalHistoryRecInt-cParaActual)*2]*256+ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])%60)!=0)
           i=0;
         m= (ParaList[(cNormalHistoryRecInt-cParaActual)*2]*256+ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])/60 ;
      //modified 2016-12-21 10:14 
         if(m==0)
          i=0;
         if(m>60)
          i=0;  
     
      //   if((60%m)!=0)
       //    i=0;
        //end of modified 2016-12-21 10:14 
       
       //modified 2016-7-17 7:27   
         if(((ParaList[(cAlarmHistoryRecInt-cParaActual)*2]*256+ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1])%60)!=0)
           i=0;
         m= (ParaList[(cAlarmHistoryRecInt-cParaActual)*2]*256+ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1])/60 ;
      //modified 2016-12-21 10:14 
         if(m==0)
          i=0;
         if(m>60)
          i=0;  
     //end of add 2016-12-21 10:15
       
     //delete 2016-12-21 10:16
     //   if(ParaList[(cAlarmHistoryRecInt-cParaActual)*2]!=ParaList[(cNormalHistoryRecInt-cParaActual)*2])
    //       i=0;
     //   if(ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1]!=ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])
     //      i=0;
     //end of delet3 
        if((ParaList[(cTempError03D-cParaActual)*2]*256+ParaList[(cTempError03D-cParaActual)*2+1]>1000)&&(ParaList[(cTempError03D-cParaActual)*2]*256+ParaList[(cTempError03D-cParaActual)*2+1]<64536)) 
		  i=1;
                                
        if((ParaList[(cHumiError03D-cParaActual)*2]*256+ParaList[(cHumiError03D-cParaActual)*2+1]>1000)&&(ParaList[(cHumiError03D-cParaActual)*2]*256+ParaList[(cHumiError03D-cParaActual)*2+1]<64536))             
           i=0; 
    //   if(( ParaList[(cSensorMode03D-cParaActual)*2]*256+ParaList[(cSensorMode03D-cParaActual)*2+1]>3))
     //      i=0;
       if((ParaList[cInterTOffet*2]*256+ParaList[cInterTOffet*2+1]>200)&&(ParaList[cInterTOffet*2]*256+ParaList[cInterTOffet*2+1]<65336))                           
            i=0;
       if((ParaList[cInterHROffet*2]*256+ParaList[cInterHROffet*2+1]>300)&&(ParaList[cInterHROffet*2]*256+ParaList[cInterHROffet*2+1]<65236))
             i=0;
         if(( ParaList[(cAlarmOutControl03D-cParaActual)*2]*256+ParaList[(cAlarmOutControl03D-cParaActual)*2+1]>=3))
           i=0;
      //modified 2016-7-17 7:52          
        if(ParaList[cDoorVar*2] !=0)
          i=0;
      //   if(!((ParaList[cDoorVar*2+1]==0)||(ParaList[cDoorVar*2+1]==78)||( ParaList[cDoorVar*2+1]==100)))
      //     i=0; 
      //end  
       #if  0				
         if((CurrentReordNum>cPageRecordNum)&&(CurrentReordNum!=255))
          i=0;
         if(CurrentWritePage>cMaxStorePage) 
          i=0;
				 
       if(ParaList[cReordNumIndex*2]!=0)
          i=0;
			 #endif
     /*  if(i==0)
         BeepOn();
	    delay(5000);
	  BeepOff();    */        
      return(i);
    }







void  DealBacklight()
 {  

#ifdef _BL03D  	
    if(ParaList[cBackLightControl*2+1]==1)  //remote lcd on
      {	SetBL();
      }     
    else if(ParaList[cBackLightControl*2+1]==0)  //remote lcd off
      {ClrBL();
      }  
  #endif
  
  #ifdef _BL02D 
   if(ParaList[cBackLight*2+1]==0)
	 ClrBL();
     else
	SetBL();
  #endif	 			
  }	  	       
 	
 
 
 
 
 void AlarmOff(void)
{ uint8_t  i; 
	
	BellOnOffStatus=0;  //add 2019-03-29 9:55
	AlarmLEDInital();
	StopBell();
	
	for(i=0;i<4;i++)
	 { if((History.AlarmLevel&(0x01<<i))==0)
		{ Alarm_Delay_02D_Count[i]=0;
                 Alarm_Delay_02D_OverFlag[i]=0;	
  	      }
  	  }
  	
} 

 
 //--------------------------------------------
void AlarmOpen(void) 
 {  if(BellOnOffStatus==0)
     {  BellOnOffStatus=1; //add 2021-04-09 20:50
     	//AlarmOutDepend_bak=AlarmOutDepend;
 //--------------------    		
if (ParaList[15]==0)  //声光
{  

   Bellcontrol(255,50,200);
  //AlarmLEDcontrol(255,50,200);
}
//--------------------------
else if (ParaList[15]==1)  //光
{
	StopBell();  
//	AlarmLEDcontrol(255,50,200);
	
}
//------------------------------
else if (ParaList[15]==2)   //声
  {    Bellcontrol(255,50,200);
        AlarmLEDInital( );
      
    }
  else   //不报警
     AlarmOff( ) ; 
    
  }
 } 
//-------------------------------------------------  

  	     
  	     
  	     



  
//add 2019-03-29 8:55
//-------------------------------------------------------- 
unsigned char AlarmAlwaysValid()
  {
     if(ParaList[(cSystemAlarmMode-cParaActual)*2+1]==0)
        return(1);
     else
        return(0);   
  }
//-----------------------------------------  	
unsigned char   PowerOffAlarmValid()
  {
  	
      if( ParaList[(cSensorWrongAlarmValid-cParaActual)*2+1]==0)
           return(1);
      else
           return(0);  
  }
  
//---------------------------------------  
unsigned char   SensorWrongAlarmValid()
  {
  	
      if( ParaList[(cSensorWrongAlarmValid-cParaActual)*2+1]==0)
           return(1);
      else
           return(0);  
  }           
//--------------------------------------------------           
unsigned char   PowerLowAlarmValid()
  {
  	
      if( ParaList[(cPowerLowAlarmValid-cParaActual)*2+1]==0)
           return(1);
      else
           return(0);  
  }            
//----------------------------------------------
unsigned char   BellAlarmValid()                        
              
      {
  	
      if( ParaList[(cBellAlarmValid-cParaActual)*2+1]==0)
           return(1);
      else
           return(0);  
  }                   
//-------------------------------------------------             
unsigned char   StartStopAlarmValid()                        
              
      {
  	
      if( ParaList[(cStartStopAlarmValid-cParaActual)*2+1]==0)
           return(1);
      else
           return(0);  
  }            

//--------------------------------------------------
unsigned char  CheckStartStopStatus()
  {
  	if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0)
  	      return(1);
  	else
  	      return(0);      
  
   }	
  	

 
 /*---------------------------------------------------
 Function：CheckPowerOffAlarm()
 Input：
 Output:
 Description:
 
//------------------------------------------------------*/ 
void  SetPowerOffAlarmValid()
        {
          VarList[cRealDataAct*2+1]&=0xfe; 	
        }	


void   SetPowerOffAlarmInValid()
       {
       	VarList[cRealDataAct*2+1]|=0x01; 
       	
       }
       
void   CheckPowerOffAlarm()
     {
     	 unsigned char  testbit,testbit1; 
     	  	testbit=AlarmAlwaysValid();
                testbit1=CheckStartStopStatus();
     	  	
     	     if((testbit==1)||(testbit1==1))	// 报警一直有效，或者启动后有效且处于启动状态
     	  	{if(GetExtPowr()==1)   //1:no power    0:power ok
                  {  SetPowerOffAlarmValid(); 
                    if(PowerOffAlarmValid()==1)  //断电报警是激活的
     	            {  
     	            
  	     	          if(History.AlarmFlagBak!=5) 
  	     	            {History.AlarmFlagBak=5;
  	     	              History.AlarmFlag=5;
  	     	           }
     	            }
     	          
     	          }  	
                 else
                   SetPowerOffAlarmInValid();
                }
              
             else 
                 SetPowerOffAlarmInValid();   
          }
           	
 
/*---------------------------------------------------
 Function：CheckPowerOffAlarm()
 Input：
 Output:
 Description: 检测sensor故障
 
//------------------------------------------------------*/ 
void  SetSensorWrongAlarmValid()
        {
          VarList[cRealDataAct*2+1]&=0xfb; 	
        }	


void   SetSensorWrongAlarmInValid()
       {
       	VarList[cRealDataAct*2+1]|=0x04; 
       	
       }

void   CheckSensorWrongAlarmValid()
     { unsigned char  testbit,testbit1; 
     	
     	     testbit=AlarmAlwaysValid();
                testbit1=CheckStartStopStatus();
     	  	
     	     if((testbit==1)||(testbit1==1))	
     	  	{  if((GetSensorStatus() ==0) ||(GetPcf8563Status()==0))  //0:sensor error     1:sensor ok  TimeStsatus o;wrong 1:ok    
                     {
                       SetSensorWrongAlarmValid() ; 
                       if(SensorWrongAlarmValid()==1)  //sensor 故障激活
                        {
                            if(History.AlarmFlagBak!=5) 
  	     	            {History.AlarmFlagBak=5;
  	     	              History.AlarmFlag=5;
  	     	           }	
                        }
                      } 
                    else
                      SetSensorWrongAlarmInValid();
                }
            else
             SetSensorWrongAlarmInValid();    
     	  
     	 
     }
//---------------------------------------------
/*---------------------------------------------------
 Function：CheckPowerLowAlarmValid()
 Input：
 Output:
 Description: 检测低电压报警
 
//------------------------------------------------------*/ 
void  SetSPowerLowAlarmValid()
        {
          VarList[cRealDataAct*2+1]&=0xf7; 	
        }	


void   SetPowerLowAlarmInValid()
       {
       	VarList[cRealDataAct*2+1]|=0x08; 
       	
       }


void   CheckPowerLowAlarmValid()
     {   unsigned char  testbit,testbit1; 
     	//delete 2019-09-12 10:15
     	//VarList[cRealDataAct*2+3]=GetPower(); 
     	//end of 2019-09-12 10:15
     	
     	//add 2019-09-12 10:16
     	if(GetExtPowr()==1)  //供电过程中
     	 {
     	   if(	GetPower()<VarList[cRealDataAct*2+3])
     	       VarList[cRealDataAct*2+3]=GetPower();
		   
		      
     	
     	 }
     	
     	else  //充电过程中
     	 {
     	   if(	GetPower()>VarList[cRealDataAct*2+3])
     	     {  	if(	VarList[cRealDataAct*2+3]<100)
			      VarList[cRealDataAct*2+3]=GetPower();
     		  }
			else
			  { if(	VarList[cRealDataAct*2+3]>=100)     
     	 	      VarList[cRealDataAct*2+3]=100;  
     	 	}
     	 }
     	 
     	
     	//end of add 2019-09-12 10:16
     	
     	
     	     testbit=AlarmAlwaysValid();
                testbit1=CheckStartStopStatus();
     	  	
     	     if((testbit==1)||(testbit1==1)) 
     	      { if(VarList[cRealDataAct*2+3]<ParaList[(cPowerAlarmValue03D-cParaActual)*2+1] )	
     	  	{ SetSPowerLowAlarmValid();
     	  	 if(PowerLowAlarmValid()==1)
     	          {
     	          	 if(History.AlarmFlagBak!=5) 
  	     	            {History.AlarmFlagBak=5;
  	     	              History.AlarmFlag=5;
  	     	           }
     	          } 
     	        }  	
     	       else
     	         SetPowerLowAlarmInValid();
     	       }
     	    else
     	    SetPowerLowAlarmInValid();
         
  }
/*---------------------------------------------------
 Function：CheckPowerLowAlarmValid()
 Input：
 Output:
 Description: 检测蜂鸣器报警状态
 
//------------------------------------------------------*/ 
void  SetBellAlarmValid()
        {
          VarList[cRealDataAct*2+1]&=0xfd; 	
        }	


void   SetBellAlarmInValid()
       {
       	VarList[cRealDataAct*2+1]|=0x02; 
       	
       }


void   CheckBellAlarmValid() 
     {
     	 unsigned char  testbit,testbit1; 
     	
     	        testbit=AlarmAlwaysValid();
                testbit1=CheckStartStopStatus();
     	  	
     	     if((testbit==1)||(testbit1==1))		
     	  	 {  
     	  	 	if(BellOnOffStatus==1)
     	  	 	    SetBellAlarmValid();
     	  	 	else
     	  	 	    SetBellAlarmInValid();
     	  	 	        
     	         }
     	       else
     	         SetBellAlarmInValid();
     	
    		 	  
    }


