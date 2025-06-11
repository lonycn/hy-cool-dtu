/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 10 $
 * $Date: 15/06/16 7:23p $
 * @brief    Convert ADC channel 0 in Single mode and print conversion results.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

void   PowerOnOpreate();
void PowerOffOperate();
void  Key1_L_Operate();
void  Key2_L_Operate();
void  Key3_L_Operate();
void  Key4_L_Operate();
void  Key5_L_Operate();
void  Key6_L_Operate();
void  Key7_L_Operate();
void  Key1_H_Operate();
void  Key2_H_Operate();
void  Key3_H_Operate();
void  Key4_H_Operate();
void  Key5_H_Operate();
void  Key6_H_Operate();
void  Key7_H_Operate();


uint8_t GetPowerIn();
void Timer_KeyPowerinInterval();
void Timer_KeyPowerinInterval1();
void Timer_KeyDI4Interval();
void Timer_KeyDI1Interval();
void Timer_KeyDI2Interval();
void Timer_KeyDI3Interval();
void Key_PowerIn_Initial(void);
void Key_PowerIn_Initial1(void);
void Key_DI1_Initial(void);
void Key_DI2_Initial(void);
void Key_DI3_Initial(void);
void Key_DI4_Initial(void);
void Key_PowerIn_StateMachine();
void Key_PowerIn_StateMachine1();
void Key1_StateMachine1();
void Key2_StateMachine1();
void Key3_StateMachine1();
void Key4_StateMachine1();
void Key5_StateMachine1();
void Key6_StateMachine1();
void Key7_StateMachine1();


void  DI1_On_Opreate();
void DI1_Off_Operate();
void  DI2_On_Opreate();
void DI2_Off_Operate();
void  DI3_On_Opreate();
void DI3_Off_Operate();
void  DI4_On_Opreate();
void DI4_Off_Operate();
void  PumpBeginCount(unsigned char port);
void PumpBeginCount_1();
void PumpBeginCount_2();
void PumpBeginCount_3();
void PumpBeginCount_4();
void  Timer_PumpRun_1();
void  Timer_PumpRun_2();
void  Timer_PumpRun_3();
void  Timer_PumpRun_4();

#define cIdle   0 
#define cWait   1
#define cWaitInterval  2
extern uint8_t uart0_Var_List[320];
struct Key_Struct
   {  
   	 uint8_t AdcStatus;
  	 uint8_t AdcSampleInterval;
         uint8_t AdcIntervalBegin;
         uint8_t AdcIntervalTimeOvr;
         uint8_t AdcSampleIntervalSet;
         uint8_t PowerInLast;
         uint8_t PowerIn;
   }; 
   

  struct Key_Struct   Key_PowerIn,Key1,Key2,Key3,Key4,Key5,Key6,Key7;

#if   0	 
uint8_t Channel1_voice[]={"config,set,ttsp,7,0,\"断电报警\"\r\n"};
uint8_t Channel2_voice[]={"config,set,ttsp,7,0,\"机组一高压报警\"\r\n"};
uint8_t Channel3_voice[]={"config,set,ttsp,7,0,\"机组一低压报警\"\r\n"};
uint8_t Channel4_voice[]={"config,set,ttsp,7,0,\"机组二高压报警\"\r\n"};
uint8_t Channel5_voice[]={"config,set,ttsp,7,0,\"机组二低压报警\"\r\n"};
uint8_t Channel6_voice[]={"config,set,ttsp,7,0,\"相序故障报警\"\r\n"};
uint8_t Channel7_voice[]={"config,set,ttsp,7,0,\"机组一运行\"\r\n"};
uint8_t Channel8_voice[]={"config,set,ttsp,7,0,\"机组二运行\"\r\n"};


uint8_t Channel11_voice[]={"config,set,ttsp,7,0,\"断电恢复\"\r\n"};
uint8_t Channel12_voice[]={"config,set,ttsp,7,0,\"机组一高压报警恢复\"\r\n"};
uint8_t Channel13_voice[]={"config,set,ttsp,7,0,\"机组一低压报警恢复\"\r\n"};
uint8_t Channel14_voice[]={"config,set,ttsp,7,0,\"机组二高压报警恢复\"\r\n"};
uint8_t Channel15_voice[]={"config,set,ttsp,7,0,\"机组二低压报警恢复\"\r\n"};
uint8_t Channel16_voice[]={"config,set,ttsp,7,0,\"相序故障报警恢复\"\r\n"};
uint8_t Channel17_voice[]={"config,set,ttsp,7,0,\"机组一停机\"\r\n"};
uint8_t Channel18_voice[]={"config,set,ttsp,7,0,\"机组二停机\"\r\n"};


	 
void   PlayV(uint8_t *text)
{
//	SendString_uart3(text);
	
	
}


	 
void   PlayVoice(uint8_t  channal)
{
   switch(channal)
	 {
		 
		 case 1: PlayV(Channel1_voice);break;
		 case 2: PlayV(Channel2_voice);break;
     case 3: PlayV(Channel3_voice);break; 
     case 4: PlayV(Channel4_voice);break;
     case 5: PlayV(Channel5_voice);break;
		 case 6: PlayV(Channel6_voice);break;
     case 7: PlayV(Channel7_voice);break; 
     case 8: PlayV(Channel8_voice);break;
		 case 9: PlayV(Channel11_voice);break;
		 case 10: PlayV(Channel12_voice);break;
     case 11: PlayV(Channel13_voice);break; 
     case 12: PlayV(Channel14_voice);break;
     case 13: PlayV(Channel15_voice);break;
		 case 14: PlayV(Channel16_voice);break;
     case 15: PlayV(Channel17_voice);break; 
     case 16: PlayV(Channel18_voice);break;
		 
}	
	 
}	 
	 
#endif

void   PlayVoice(uint8_t  channal)
{}

/*--------------------------------------------------------------------------------------------
//Function: void   PowerOnOpreate()
//Input:  
//Output:   None 
	//Description:外电恢复执行的操作，level=0，normal，
//--------------------------------------------------------------------------------------------*/
void   PowerOnOpreate()
     { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	 uart0_Var_List[AlarmStartAddr*2+1]=0;
			         uart0_Var_List[AlarmStartAddr*2]=0;
		     }	 
	 else		 
		 
		     {
						 	 uart0_Var_List[AlarmStartAddr*2+1]=2;
			       uart0_Var_List[AlarmStartAddr*2]=0;
		     }	 	 
 
			}
void  Key1_L_Operate()
	
    { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	  uart0_Var_List[(AlarmStartAddr+7)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	 uart0_Var_List[(AlarmStartAddr+7)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
		     }	 	 
 
			}
		#if  0	//20250320

{
	  uart0_Var_List[(AlarmStartAddr+7)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
//	Bellcontrol(2, 100,100 );
}
#endif
void  Key2_L_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+4)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+4)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
		     }	 	 
 
			}
	 #if  0	 //20250320

{
	uart0_Var_List[(AlarmStartAddr+4)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
//	Bellcontrol(2, 100,100 );
}
			#endif
void  Key3_L_Operate()
	


 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+3)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+3)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
		     }	 	 
 
			}
 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+3)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
//	Bellcontrol(2, 100,100 );
}
			#endif
void  Key4_L_Operate()
	

 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						uart0_Var_List[(AlarmStartAddr+2)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	 uart0_Var_List[(AlarmStartAddr+2)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
		     }	 	 
 
			}
 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+2)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
	//Bellcontrol(2, 100,100 );
}
			#endif
void  Key5_L_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+1)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;
		     }	 
	 else		 
		 
		     {
						 uart0_Var_List[(AlarmStartAddr+1)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+1)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;
	//Bellcontrol(2, 100,100 );
}
			#endif
void  Key6_L_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	 uart0_Var_List[(AlarmStartAddr+8)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
		     }	 
	 else		 
		 
		     {
						  uart0_Var_List[(AlarmStartAddr+8)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	 uart0_Var_List[(AlarmStartAddr+8)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
	//Bellcontrol(2, 100,100 );
} 
			#endif
void  Key7_L_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 uart0_Var_List[(AlarmStartAddr+9)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+9)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+9)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
	//Bellcontrol(2, 100,100 );
}
			#endif
//----------------------------------------------------------------------
void PowerOffOperate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						   uart0_Var_List[(AlarmStartAddr)*2+1]=0;//20250328 modfied to 0
			uart0_Var_List[(AlarmStartAddr)*2]=0;
		     }	 
	 else		 
		 
		     {
						   uart0_Var_List[(AlarmStartAddr)*2+1]=2;
			uart0_Var_List[(AlarmStartAddr)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
    {int8_t SendBuf[200];
       uint8_t i=0,j;
      uart0_Var_List[(AlarmStartAddr)*2+1]=1;
			uart0_Var_List[(AlarmStartAddr)*2]=0;
	
  
	for(i=0;i<56;i++)
	 SendBuf[i]=uart0_Var_List[i+204];
uart3_SendControlData(1,4,0x103,28,SendBuf) ; 
for(i=0;i<56;i++)
	 SendBuf[i]=uart0_Var_List[i+204];
uart3_SendControlData(1,4,0x103,28,SendBuf) ; 
			
			for(i=0;i<56;i++)
	 SendBuf[i]=uart0_Var_List[i+204];
 uart3_SendControlData(1,4,0x103,28,SendBuf) ; 
			
    }	
			#endif
void  Key1_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	 uart0_Var_List[(AlarmStartAddr+7)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+7)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+7)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+7)*2]=0;
	
}
			#endif
void  Key2_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+4)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+4)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+4)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+4)*2]=0;
}
			#endif
void  Key3_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+3)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
		     }	 
	 else		 
		 
		     {
						 uart0_Var_List[(AlarmStartAddr+3)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+3)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+3)*2]=0;
}
			#endif
void  Key4_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	 uart0_Var_List[(AlarmStartAddr+2)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	 uart0_Var_List[(AlarmStartAddr+2)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+2)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+2)*2]=0;
}
			#endif
void  Key5_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 uart0_Var_List[(AlarmStartAddr+1)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;}
	 else		 
		 
		     {
						 uart0_Var_List[(AlarmStartAddr+1)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;
		     }	 	 
 
			}
	 #if  0	  //20250320
{
	uart0_Var_List[(AlarmStartAddr+1)*2+1]=1;
	uart0_Var_List[(AlarmStartAddr+1)*2]=0;
}
			#endif
void  Key6_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	uart0_Var_List[(AlarmStartAddr+8)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+8)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
		     }	 	 
 
			}
	#if  0	   //20250320
{
	uart0_Var_List[(AlarmStartAddr+8)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+8)*2]=0;
}
			#endif
void  Key7_H_Operate()
	 { 
		if(ParaList[cUseDi*2+1]==0x01)
		    {
						 	 	uart0_Var_List[(AlarmStartAddr+9)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
		     }	 
	 else		 
		 
		     {
						 	uart0_Var_List[(AlarmStartAddr+9)*2+1]=2;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
		     }	 	 
 
			}
#if  0	   //20250320
{
	uart0_Var_List[(AlarmStartAddr+9)*2+1]=0;
	uart0_Var_List[(AlarmStartAddr+9)*2]=0;
}
			#endif

/*--------------------------------------------------------------------------------------------
//Function: uint8_t GetPowerIn()
//Input:  
//Output:   
//Description:返回掉电状态  Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
uint8_t GetPowerIn()
	  {
			return(Key_PowerIn.PowerIn);
    }			
	 
/*--------------------------------------------------------------------------------------------
//Function: void Timer_KeyPowerinInterval()
//Input:  
//Output:   None 
//Description:掉电监测时间片定时处理,放在定时定时器中
//--------------------------------------------------------------------------------------------*/	 
void Timer_KeyPowerinInterval()
 {
   Timer_KeyPowerinInterval1();	

}


void Timer_KeyPowerinInterval1()
  {
    if(Key_PowerIn.AdcIntervalBegin==1)
      {
      	 Key_PowerIn.AdcSampleInterval++;
      	 if(Key_PowerIn.AdcSampleInterval>=Key_PowerIn.AdcSampleIntervalSet)
      	   { Key_PowerIn.AdcIntervalBegin=0;
      	     Key_PowerIn.AdcIntervalTimeOvr=1;
          }		
       }
//ADD 20230911

    if(Key1.AdcIntervalBegin==1)
      {
      	 Key1.AdcSampleInterval++;
      	 if(Key1.AdcSampleInterval>=Key1.AdcSampleIntervalSet)
      	   { Key1.AdcIntervalBegin=0;
      	     Key1.AdcIntervalTimeOvr=1;
          }		
       }
			
			 if(Key2.AdcIntervalBegin==1)
      {
      	 Key2.AdcSampleInterval++;
      	 if(Key2.AdcSampleInterval>=Key2.AdcSampleIntervalSet)
      	   { Key2.AdcIntervalBegin=0;
      	     Key2.AdcIntervalTimeOvr=1;
          }		
       }
			
			 
			 if(Key3.AdcIntervalBegin==1)
      {
      	 Key3.AdcSampleInterval++;
      	 if(Key3.AdcSampleInterval>=Key3.AdcSampleIntervalSet)
      	   { Key3.AdcIntervalBegin=0;
      	     Key3.AdcIntervalTimeOvr=1;
          }		
       }
			
			 if(Key4.AdcIntervalBegin==1)
      {
      	 Key4.AdcSampleInterval++;
      	 if(Key4.AdcSampleInterval>=Key4.AdcSampleIntervalSet)
      	   { Key4.AdcIntervalBegin=0;
      	     Key4.AdcIntervalTimeOvr=1;
          }		
       }
			
			 if(Key5.AdcIntervalBegin==1)
      {
      	 Key5.AdcSampleInterval++;
      	 if(Key5.AdcSampleInterval>=Key5.AdcSampleIntervalSet)
      	   { Key5.AdcIntervalBegin=0;
      	     Key5.AdcIntervalTimeOvr=1;
          }		
       }
			
			 
			 if(Key6.AdcIntervalBegin==1)
      {
      	 Key6.AdcSampleInterval++;
      	 if(Key6.AdcSampleInterval>=Key6.AdcSampleIntervalSet)
      	   { Key6.AdcIntervalBegin=0;
      	     Key6.AdcIntervalTimeOvr=1;
          }		
       }
			
			 if(Key7.AdcIntervalBegin==1)
      {
      	 Key7.AdcSampleInterval++;
      	 if(Key7.AdcSampleInterval>=Key7.AdcSampleIntervalSet)
      	   { Key7.AdcIntervalBegin=0;
      	     Key7.AdcIntervalTimeOvr=1;
          }		
       }







			 
  }

/*--------------------------------------------------------------------------------------------
//Function: void Key_PowerIn_Initial(void)
//Input:  
//Output:   None 
//Description:掉电监测单元初始化
//--------------------------------------------------------------------------------------------*/
void Key_PowerIn_Initial(void)
{  Key_PowerIn_Initial1();
   
}



void Key_PowerIn_Initial1(void)
{
   GPIO_SetMode(PB, BIT14, GPIO_PMD_INPUT);
	 GPIO_SetMode(PA, BIT15, GPIO_PMD_INPUT);  //20230416
   Key_PowerIn.PowerInLast=0xff;
   Key_PowerIn.PowerIn=0xff;
   Key_PowerIn.AdcSampleIntervalSet=2;
   Key_PowerIn.AdcStatus=cIdle;
//add  20230911	
	  GPIO_SetMode(PB, BIT13, GPIO_PMD_INPUT);
   Key1.PowerInLast=0xff;
   Key1.PowerIn=0xff;
   Key1.AdcSampleIntervalSet=2;
   Key1.AdcStatus=cIdle;
	
	 GPIO_SetMode(PA, BIT11, GPIO_PMD_INPUT);
   Key2.PowerInLast=0xff;
   Key2.PowerIn=0xff;
   Key2.AdcSampleIntervalSet=2;
   Key2.AdcStatus=cIdle;
	
	 GPIO_SetMode(PA, BIT10, GPIO_PMD_INPUT);
   Key3.PowerInLast=0xff;
   Key3.PowerIn=0xff;
   Key3.AdcSampleIntervalSet=2;
   Key3.AdcStatus=cIdle;
	 
	  GPIO_SetMode(PB, BIT6, GPIO_PMD_INPUT);
   Key4.PowerInLast=0xff;
   Key4.PowerIn=0xff;
   Key4.AdcSampleIntervalSet=2;
   Key4.AdcStatus=cIdle;
	 
	  GPIO_SetMode(PB, BIT7, GPIO_PMD_INPUT);
   Key5.PowerInLast=0xff;
   Key5.PowerIn=0xff;
   Key5.AdcSampleIntervalSet=2;
   Key5.AdcStatus=cIdle;
	 
	  GPIO_SetMode(PA, BIT5, GPIO_PMD_INPUT);
   Key6.PowerInLast=0xff;
   Key6.PowerIn=0xff;
   Key6.AdcSampleIntervalSet=2;
   Key6.AdcStatus=cIdle;
	 
	  GPIO_SetMode(PA, BIT4, GPIO_PMD_INPUT);
   Key7.PowerInLast=0xff;
   Key7.PowerIn=0xff;
   Key7.AdcSampleIntervalSet=2;
   Key7.AdcStatus=cIdle;
	
PA->PUEN |= 0x0c30;
	PB->PUEN |= 0x60C0;
}








void Key_PowerIn_StateMachine()
 {
   Key_PowerIn_StateMachine1();	
   Key1_StateMachine1();
  Key2_StateMachine1();	
  Key3_StateMachine1();
 Key4_StateMachine1();
   Key5_StateMachine1();	
 Key6_StateMachine1();
   Key7_StateMachine1();
	 
#if  0	 
 PowerOnOpreate();
  Key1_L_Operate();
  Key2_L_Operate();
  Key3_L_Operate();
  Key4_L_Operate();
 Key5_L_Operate();
  Key6_L_Operate();
 Key7_L_Operate();
#endif	 
 }

/*--------------------------------------------------------------------------------------------
//Function: Key_PowerIn_StateMachine()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key_PowerIn_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key_PowerIn.AdcStatus)
      {
      	case cIdle: 
                 if((PB14==0)||(PA15==0))
                  {
                    if(Key_PowerIn.PowerInLast!=0)
                       Key_PowerIn.AdcStatus=cWait;
                  }
                 else
                  {  if(Key_PowerIn.PowerInLast!=1)
                       Key_PowerIn.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key_PowerIn.AdcStatus=cWaitInterval;
                   Key_PowerIn.AdcSampleInterval=0;
                   Key_PowerIn.AdcIntervalBegin=1;
                   Key_PowerIn.AdcIntervalTimeOvr=0;
                   Key_PowerIn.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key_PowerIn.AdcIntervalTimeOvr==1)
                { 
                   
                   if((PB14==0)||(PA15==0))
                  {
                    if(Key_PowerIn.PowerInLast!=0)
										{  Key_PowerIn.PowerInLast=0;
											 Key_PowerIn.PowerIn=0;
											 PowerOnOpreate();
										}											
                    Key_PowerIn.AdcStatus=cIdle;
                         
                  }
                 else
                  {  if(Key_PowerIn.PowerInLast!=1)
                     {  Key_PowerIn.PowerInLast=1;
											 Key_PowerIn.PowerIn=1;
                       PowerOffOperate();
										 }
                    Key_PowerIn.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key_PowerIn.AdcStatus=cIdle;
                  break;    	       
        }         
   }


/*--------------------------------------------------------------------------------------------
//Function: void Key1_StateMachine1()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key1_StateMachine1()
{ unsigned  char  i;
	
	#if  1 
   switch(Key1.AdcStatus)
      {
      	case cIdle: 
                 if(PB13==0)
                  {
                    if(Key1.PowerInLast!=0)
                       Key1.AdcStatus=cWait;
                  }
                 else
                  {  if(Key1.PowerInLast!=1)
                       Key1.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key1.AdcStatus=cWaitInterval;
                   Key1.AdcSampleInterval=0;
                   Key1.AdcIntervalBegin=1;
                   Key1.AdcIntervalTimeOvr=0;
                   Key1.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key1.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PB13==0)
                  {
                    if(Key1.PowerInLast!=0)
										{  Key1.PowerInLast=0;
										      Key1_L_Operate(); 
											Key1.PowerIn=0;
										}											
                    Key1.AdcStatus=cIdle;
                       
                  }
                 else
                  {  if(Key1.PowerInLast!=1)
                      { Key1.PowerInLast=1;
                       Key1_H_Operate();
												Key1.PowerIn=1;
											}
                    Key1.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key1.AdcStatus=cIdle;
                  break;    	       
        } 
		#endif	
   }
/*--------------------------------------------------------------------------------------------
//Function: void Key2_StateMachine1()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key2_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key2.AdcStatus)
      {
      	case cIdle: 
                 if(PA11==0)
                  {
                    if(Key2.PowerInLast!=0)
                       Key2.AdcStatus=cWait;
                  }
                 else
                  {  if(Key2.PowerInLast!=1)
                       Key2.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key2.AdcStatus=cWaitInterval;
                   Key2.AdcSampleInterval=0;
                   Key2.AdcIntervalBegin=1;
                   Key2.AdcIntervalTimeOvr=0;
                   Key2.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key2.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PA11==0)
                  {
                    if(Key2.PowerInLast!=0)
										{   Key2.PowerInLast=0;
											 Key2_L_Operate(); 
											Key2.PowerIn=0;
										}
                    Key2.AdcStatus=cIdle;
                        
                  }
                 else
                  {  if(Key2.PowerInLast!=1)
                      { Key2.PowerInLast=1;
                        Key2_H_Operate();
                        Key2.PowerIn=1;												
											}												
                    Key2.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key2.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function:void Key3_StateMachine1()
//Input:  
//Output:   None 
//Description:
//--------------------------------------------------------------------------------------------*/
void Key3_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key3.AdcStatus)
      {
      	case cIdle: 
                 if(PA10==0)
                  {
                    if(Key3.PowerInLast!=0)
                       Key3.AdcStatus=cWait;
                  }
                 else
                  {  if(Key3.PowerInLast!=1)
                       Key3.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key3.AdcStatus=cWaitInterval;
                   Key3.AdcSampleInterval=0;
                   Key3.AdcIntervalBegin=1;
                   Key3.AdcIntervalTimeOvr=0;
                   Key3.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key3.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PA10==0)
                  {
                    if(Key3.PowerInLast!=0)
										{   Key3.PowerInLast=0;
										    Key3_L_Operate(); 
											  Key3.PowerIn=0;
										}
                    Key3.AdcStatus=cIdle;
                         
                  }
                 else
                  {  if(Key3.PowerInLast!=1)
                      { Key3.PowerInLast=1;
                        Key3_H_Operate();
                       Key3.PowerIn=1;												
											}
                    Key3.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key_PowerIn.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: void Key4_StateMachine1()
//Input:  
//Output:   None 
//Description:
//--------------------------------------------------------------------------------------------*/
void Key4_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key4.AdcStatus)
      {
      	case cIdle: 
                 if(PB6==0)
                  {
                    if(Key4.PowerInLast!=0)
                       Key4.AdcStatus=cWait;
                  }
                 else
                  {  if(Key4.PowerInLast!=1)
                       Key4.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key4.AdcStatus=cWaitInterval;
                   Key4.AdcSampleInterval=0;
                   Key4.AdcIntervalBegin=1;
                   Key4.AdcIntervalTimeOvr=0;
                   Key4.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key4.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PB6==0)
                  {
                    if(Key4.PowerInLast!=0)
										{   Key4.PowerInLast=0;
										 Key4_L_Operate();
											Key4.PowerIn=0;
										}
                    Key4.AdcStatus=cIdle;
                            
                  }
                 else
                  {  if(Key4.PowerInLast!=1)
                    {   Key4.PowerInLast=1;
                        Key4_H_Operate();
											Key4.PowerIn=1;
										}											
                    Key4.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key4.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: void Key5_StateMachine1()
//Input:  
//Output:   None 
//Description:
//--------------------------------------------------------------------------------------------*/
void Key5_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key5.AdcStatus)
      {
      	case cIdle: 
                 if(PB7==0)
                  {
                    if(Key5.PowerInLast!=0)
                       Key5.AdcStatus=cWait;
                  }
                 else
                  {  if(Key5.PowerInLast!=1)
                       Key5.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key5.AdcStatus=cWaitInterval;
                   Key5.AdcSampleInterval=0;
                   Key5.AdcIntervalBegin=1;
                   Key5.AdcIntervalTimeOvr=0;
                   Key5.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key5.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PB7==0)
                  {
                    if(Key5.PowerInLast!=0)
										{   Key5.PowerInLast=0;
										Key5_L_Operate(); 
											Key5.PowerIn=0;
										}
                    Key5.AdcStatus=cIdle;
                            
                  }
                 else
                  {  if(Key5.PowerInLast!=1)
                    {   Key5.PowerInLast=1;
                        Key5_H_Operate();
											Key5.PowerIn=1;
										}											
                    Key5.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key5.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: Key6_StateMachine1()
//Input:  
//Output:   None 
//Description:
//--------------------------------------------------------------------------------------------*/
void Key6_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key6.AdcStatus)
      {
      	case cIdle: 
                 if(PA5==0)
                  {
                    if(Key6.PowerInLast!=0)
                       Key6.AdcStatus=cWait;
                  }
                 else
                  {  if(Key6.PowerInLast!=1)
                       Key6.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key6.AdcStatus=cWaitInterval;
                   Key6.AdcSampleInterval=0;
                   Key6.AdcIntervalBegin=1;
                   Key6.AdcIntervalTimeOvr=0;
                   Key6.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key6.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PA5==0)
                  {
                    if(Key6.PowerInLast!=0)
										{  Key6.PowerInLast=0;
											Key6_L_Operate();
											Key6.PowerIn=0;
										}
                    Key6.AdcStatus=cIdle;
                            
                  }
                 else
                  {  if(Key6.PowerInLast!=1)
                      { Key6.PowerInLast=1;
                        Key6_H_Operate();  
													Key6.PowerIn=1;
											}
                    Key6.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key6.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: Key7_StateMachine1()
//Input:  
//Output:   None 
//Description:
//--------------------------------------------------------------------------------------------*/
void Key7_StateMachine1()
{ unsigned  char  i;
	  
   switch(Key7.AdcStatus)
      {
      	case cIdle: 
                 if(PA4==0)
                  {
                    if(Key7.PowerInLast!=0)
                       Key7.AdcStatus=cWait;
                  }
                 else
                  {  if(Key7.PowerInLast!=1)
                       Key7.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key7.AdcStatus=cWaitInterval;
                   Key7.AdcSampleInterval=0;
                   Key7.AdcIntervalBegin=1;
                   Key7.AdcIntervalTimeOvr=0;
                   Key7.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key7.AdcIntervalTimeOvr==1)
                { 
                   
                  if(PA4==0)
                  {
                    if(Key7.PowerInLast!=0)
										{  Key7.PowerInLast=0;
											Key7_L_Operate(); 
	                     Key7.PowerIn=0;											
                    
										}
                    Key7.AdcStatus=cIdle;     
                  }
                 else
                  {  if(Key7.PowerInLast!=1)
                      { Key7.PowerInLast=1;
                       Key7_H_Operate(); 
													Key7.PowerIn=1;
											}												
                    Key7.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key7.AdcStatus=cIdle;
                  break;    	       
        }         
   }

	 
	 
 

	 
/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/


