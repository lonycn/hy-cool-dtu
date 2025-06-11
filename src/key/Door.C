#include <stdio.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

extern  uint8_t uart0_Var_List[320];
struct Key_Door
   {  
   	 
  	 uint16_t Key_Interval;
  	 uint16_t Key_DoorBak;
         uint8_t Key_IntervalBegin;
         uint8_t Key_IntervalTimeOvr;
         uint16_t Key_SampleIntervalSet;
    };    

 struct Key_Door   Key_Door1;   







/*--------------------------------------------------------------------------------------------
//Function:void Key_DI1_Initial(void)
//Input:  
//Output:   None 
//Description:di1的初始化部分,
//--------------------------------------------------------------------------------------------*/
void KeyDoor_Initial(void)  //debounce
{
   Key_Door1.Key_Interval=0;
   Key_Door1.Key_IntervalBegin=0;
   Key_Door1.Key_IntervalTimeOvr=0;
   Key_Door1.Key_SampleIntervalSet=(ParaList[DoorDelay*2]*256+ParaList[DoorDelay*2+1])*60;
   Key_Door1.Key_DoorBak=1;
	GPIO_SetMode(PA, BIT0, GPIO_PMD_INPUT);
	PA->PUEN |= 0x0001;
}   

void KeyDoor_OpenStart()
{
    Key_Door1.Key_Interval=0;
   Key_Door1.Key_IntervalBegin=1;
   Key_Door1.Key_IntervalTimeOvr=0;
   Key_Door1.Key_SampleIntervalSet=(ParaList[DoorDelay*2]*256+ParaList[DoorDelay*2+1])*60;
   	
} 
void KeyDoor_CloseStart()
{
    Key_Door1.Key_Interval=0;
   Key_Door1.Key_IntervalBegin=0;
   Key_Door1.Key_IntervalTimeOvr=0;
   Key_Door1.Key_SampleIntervalSet=(ParaList[DoorDelay*2]*256+ParaList[DoorDelay*2+1])*60;
   	
} 
/*--------------------------------------------------------------------------------------------
//Function:void Timer_KeyDI1Interval()
//Input:  
//Output:   None 
//Description:DI1的debounce延时处理定时
//--------------------------------------------------------------------------------------------*/
void Timer_KeyDoor_Interval()
  {
    if(Key_Door1.Key_IntervalBegin==1)
      {
      	 Key_Door1.Key_Interval++;
      	 if(Key_Door1.Key_Interval>=Key_Door1.Key_SampleIntervalSet)
      	   {// Key_Door1.Key_IntervalBegin=0;
      	     Key_Door1.Key_IntervalTimeOvr=1;
          }		
       }	
  }

/*--------------------------------------------------------------------------------------------
//Function:void  DealWithDoor()
//Input:  
//Output:   None 
//Description:main 主程序中调用，门磁关=0，相应报警为关，门磁开=1，开始延时，时间到，设置报警位
//--------------------------------------------------------------------------------------------*/
void  DealWithDoor()
 {
 uint8_t  i;
	 uint16_t temp1;
if(ParaList[uart2_Tp_pc*2+1]==1)
{	 if(PA0==1)
	{ 
		SetDoorStatusFlag();
	if(Key_Door1.Key_IntervalBegin==0)
	   { KeyDoor_OpenStart() ;
	      Key_Door1.Key_DoorBak=1;
	   }
	}    
	    
     else
	{ ClearDoorStatusFlag();
	 if(Key_Door1.Key_IntervalBegin==1)
	   { KeyDoor_CloseStart();
	      Key_Door1.Key_DoorBak=0;
	   }
	  ClearDoorAlarmFlag(); 
	  
       }		
   if(Key_Door1.Key_IntervalTimeOvr==1)
	 { SetDoorAlarmFlag();
		 Key_Door1.Key_IntervalTimeOvr=0;
	 }
   

}
else
{
	ClearDoorAlarmFlag();
	
}
//----------------------------------将报警位进行映射------
i=0;
if((uart0_Var_List[cVarAlarmold*2+1]&0x01)==0x01)
   uart0_Var_List[cVarAlarm*2+1]=1;
else
   uart0_Var_List[cVarAlarm*2+1]=0;	
if(ParaList[uart2_Tp_pc*2+1]==0)
{               temp1=(uint8_t)(-cInValidTempNouse);
                uart0_Var_List[cVarAlarm*2]=temp1/256;   //11
                uart0_Var_List[cVarAlarm*2+1]=temp1%256;
}
i++;
if((uart0_Var_List[cVarAlarmold*2+1]&0x02)==0x02)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;	
if(ParaList[uart2_Tp_pc*2+1]==0)
{               temp1=(uint8_t)(-cInValidTempNouse);
                uart0_Var_List[cVarAlarm*2]=temp1/256;   //11
                uart0_Var_List[cVarAlarm*2+1]=temp1%256;
}


i++;
if((uart0_Var_List[cVarAlarmold*2+1]&0x04)==0x04)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;	
i++;
if((uart0_Var_List[cVarAlarmold*2+1]&0x08)==0x08)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;	

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x01)==0x01)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;	

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x02)==0x02)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x04)==0x04)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x08)==0x08)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;


i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x10)==0x10)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x20)==0x20)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x20)==0x40)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+2]&0x20)==0x80)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;
//-------------------------------
i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x01)==0x01)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;	

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x02)==0x02)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x04)==0x04)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x08)==0x08)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;


i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x10)==0x10)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x20)==0x20)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x20)==0x40)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;

i++;
if((uart0_Var_List[cVarAlarmold*2+1+1]&0x20)==0x80)
   uart0_Var_List[cVarAlarm*2+1+2*i]=1;
else
   uart0_Var_List[cVarAlarm*2+1+2*i]=0;



}
