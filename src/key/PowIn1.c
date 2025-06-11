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
void Key_DI1_StateMachine();
void Key_DI2_StateMachine();
void Key_DI3_StateMachine();
void Key_DI4_StateMachine();
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

#ifdef  __flood
  #define DI1_PIN  PD14
#endif
#ifdef  __DoorMagic
  #define DI1_PIN  PD14
#endif
	 

  
#define DI2_PIN  PB9
#define DI3_PIN  PE5
#define DI4_PIN  PC2


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
   

#ifdef  __IOBoard    
  struct Key_Struct   Key_PowerIn,Key_DI1,Key_DI2,Key_DI3,Key_DI4,PumpRunTIme_1,PumpRunTIme_2,PumpRunTIme_3,PumpRunTIme_4;  
#else
  struct Key_Struct   Key_PowerIn;
#endif  
	 

/*--------------------------------------------------------------------------------------------
//Function: void   PowerOnOpreate()
//Input:  
//Output:   None 
//Description:外电恢复执行的操作
//--------------------------------------------------------------------------------------------*/
void   PowerOnOpreate()
     {
     	 SetBL( );
     }
/*--------------------------------------------------------------------------------------------
//Function:void PowerOffOperate()
//Input:  
//Output:   None 
//Description:掉电后执行的操作
//--------------------------------------------------------------------------------------------*/
void PowerOffOperate()
    {
    	ClrBL( );
    	
    }	


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
#ifdef  __IOBoard       
   Timer_KeyDI1Interval();
   Timer_KeyDI2Interval();
   Timer_KeyDI3Interval();
   Timer_KeyDI4Interval();	
   Timer_PumpRun_1();
   Timer_PumpRun_2();
   Timer_PumpRun_3();
   Timer_PumpRun_4();
#endif   
}
void Timer_PumpRun()
 {
 #ifdef  __IOBoard       
   	
   Timer_PumpRun_1();
   Timer_PumpRun_2();
   Timer_PumpRun_3();
   Timer_PumpRun_4();
#endif   	
 	
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
  }
#ifdef  __IOBoard    
void Timer_KeyDI4Interval()
  {
    if(Key_DI4.AdcIntervalBegin==1)
      {
      	 Key_DI4.AdcSampleInterval++;
      	 if(Key_DI4.AdcSampleInterval>=Key_DI4.AdcSampleIntervalSet)
      	   { Key_DI4.AdcIntervalBegin=0;
      	     Key_DI4.AdcIntervalTimeOvr=1;
          }		
       }	
  }

void Timer_KeyDI1Interval()
  {
    if(Key_DI1.AdcIntervalBegin==1)
      {
      	 Key_DI1.AdcSampleInterval++;
      	 if(Key_DI1.AdcSampleInterval>=Key_DI1.AdcSampleIntervalSet)
      	   { Key_DI1.AdcIntervalBegin=0;
      	     Key_DI1.AdcIntervalTimeOvr=1;
          }		
       }	
  }
  void Timer_KeyDI2Interval()
  {
    if(Key_DI2.AdcIntervalBegin==1)
      {
      	 Key_DI2.AdcSampleInterval++;
      	 if(Key_DI2.AdcSampleInterval>=Key_DI2.AdcSampleIntervalSet)
      	   { Key_DI2.AdcIntervalBegin=0;
      	     Key_DI2.AdcIntervalTimeOvr=1;
          }		
       }	
  }
  void Timer_KeyDI3Interval()
  {
    if(Key_DI3.AdcIntervalBegin==1)
      {
      	 Key_DI3.AdcSampleInterval++;
      	 if(Key_DI3.AdcSampleInterval>=Key_DI3.AdcSampleIntervalSet)
      	   { Key_DI3.AdcIntervalBegin=0;
      	     Key_DI3.AdcIntervalTimeOvr=1;
          }		
       }	
  }
#endif  
/*--------------------------------------------------------------------------------------------
//Function: void Key_PowerIn_Initial(void)
//Input:  
//Output:   None 
//Description:掉电监测单元初始化
//--------------------------------------------------------------------------------------------*/
void Key_PowerIn_Initial(void)
{  Key_PowerIn_Initial1();
#ifdef  __IOBoard    
   Key_DI1_Initial();
   Key_DI2_Initial();
   Key_DI3_Initial();
   Key_DI4_Initial();
#endif   
}



void Key_PowerIn_Initial1(void)
{
   GPIO_SetMode(PB, BIT14, GPIO_PMD_INPUT);
   Key_PowerIn.PowerInLast=0;
   Key_PowerIn.PowerIn=0;
   Key_PowerIn.AdcSampleIntervalSet=2;
   Key_PowerIn.AdcStatus=cIdle;
}

#ifdef  __IOBoard    
void Key_DI1_Initial(void)
{
   Key_DI1.PowerInLast=0xff;
   Key_DI1.PowerIn=0;
   Key_DI1.AdcSampleIntervalSet=2;
   Key_DI1.AdcStatus=cIdle;
}

void Key_DI2_Initial(void)
{
   Key_DI2.PowerInLast=0xff;
   Key_DI2.PowerIn=0;
   Key_DI2.AdcSampleIntervalSet=2;
   Key_DI2.AdcStatus=cIdle;
}

void Key_DI3_Initial(void)
{
   Key_DI3.PowerInLast=0xff;
   Key_DI3.PowerIn=0;
   Key_DI3.AdcSampleIntervalSet=2;
   Key_DI3.AdcStatus=cIdle;
}

void Key_DI4_Initial(void)
{
   Key_DI4.PowerInLast=0xff;
   Key_DI4.PowerIn=0;
   Key_DI4.AdcSampleIntervalSet=2;
   Key_DI4.AdcStatus=cIdle;
}
#endif
void Key_PowerIn_StateMachine()
 {
   Key_PowerIn_StateMachine1();	
   Key_DI1_StateMachine();
   Key_DI2_StateMachine();	
   Key_DI3_StateMachine();
   Key_DI4_StateMachine();	
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
                 if(PB14==0)
                  {
                    if(Key_PowerIn.PowerInLast==1)
                       Key_PowerIn.AdcStatus=cWait;
                  }
                 else
                  {  if(Key_PowerIn.PowerInLast==0)
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
                   
                  if(PB14==0)
                  {
                    if(Key_PowerIn.PowerInLast==1)
                        Key_PowerIn.PowerIn=0;
                    Key_PowerIn.AdcStatus=cIdle;
                       PowerOnOpreate();      
                  }
                 else
                  {  if(Key_PowerIn.PowerInLast==0)
                       Key_PowerIn.PowerIn=1;
                       PowerOffOperate();
                    Key_PowerIn.AdcStatus=cIdle;     
                  } 
                }        
                  break;
         default:    Key_PowerIn.AdcStatus=cIdle;
                  break;    	       
        }         
   }


/*--------------------------------------------------------------------------------------------
//Function: Key_PowerIn_StateMachine()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
#ifdef  __IOBoard    
void Key_DI1_StateMachine()
 {
   switch(Key_DI1.AdcStatus)
      {
      	case cIdle: 
                 if(DI1_PIN==0)
                  { Key_DI1.PowerIn=0;
                    if(Key_DI1.PowerInLast!=0)
                       Key_DI1.AdcStatus=cWait;
                  }
                 else
                  { Key_DI1.PowerIn=1; 
										if(Key_DI1.PowerInLast!=1)
                       Key_DI1.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key_DI1.AdcStatus=cWaitInterval;
                   Key_DI1.AdcSampleInterval=0;
                   Key_DI1.AdcIntervalBegin=1;
                   Key_DI1.AdcIntervalTimeOvr=0;
                   Key_DI1.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key_DI1.AdcIntervalTimeOvr==1)
                { 
                   
                  if(DI1_PIN==Key_DI1.PowerIn)
                  {
										if(Key_DI1.PowerIn==0)
										{
											Key_DI1.PowerInLast=0;
										
										       #ifdef  __flood
											 DI1_On_Opreate();
											  #endif 
										}
									 else
									 {
										 Key_DI1.PowerInLast=1; 
										 #ifdef  __flood
                         DI1_Off_Operate();
                         #endif
										 
									 }
								 }
								else
                   Key_DI1.AdcIntervalTimeOvr=0;
									 
                       Key_DI1.AdcStatus=cIdle;
                       Key_DI1.AdcIntervalBegin=0;
                       Key_DI1.AdcIntervalTimeOvr=0;
							}	
                 
              
                  break;
         default:    Key_DI1.AdcStatus=cIdle;
                  break;    	       
        }         
   }

/*--------------------------------------------------------------------------------------------
//Function: Key_PowerIn_StateMachine()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key_DI2_StateMachine()
 {
   switch(Key_DI2.AdcStatus)
      {
      	case cIdle: 
                 if(DI2_PIN==0)
                  {
                    if(Key_DI2.PowerInLast!=0)
                       Key_DI2.AdcStatus=cWait;
                  }
                 else
                  {  if(Key_DI2.PowerInLast==0)
                       Key_DI2.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key_DI2.AdcStatus=cWaitInterval;
                   Key_DI2.AdcSampleInterval=0;
                   Key_DI2.AdcIntervalBegin=1;
                   Key_DI2.AdcIntervalTimeOvr=0;
                   Key_DI2.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key_DI2.AdcIntervalTimeOvr==1)
                { 
                   
                  if(DI2_PIN==0)
                  {
                    if(Key_DI2.PowerInLast==1)
                        Key_DI2.PowerIn=0;
                    Key_DI2.AdcStatus=cIdle;
                       DI2_On_Opreate();
                       Key_DI2.PowerInLast=0;      
                  }
                 else
                  {  if(Key_DI2.PowerInLast==0)
                       Key_DI2.PowerIn=1;
                       DI2_Off_Operate();
                    Key_DI2.AdcStatus=cIdle;
                    Key_DI2.PowerInLast=1;     
                  } 
                }        
                  break;
         default:    Key_DI2.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: Key_PowerIn_StateMachine()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key_DI3_StateMachine()
 {
   switch(Key_DI3.AdcStatus)
      {
      	case cIdle: 
                 if(DI3_PIN==0)
                  {
                    if(Key_DI3.PowerInLast!=0)
                       Key_DI3.AdcStatus=cWait;
                  }
                 else
                  {  if(Key_DI3.PowerInLast==0)
                       Key_DI3.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key_DI3.AdcStatus=cWaitInterval;
                   Key_DI3.AdcSampleInterval=0;
                   Key_DI3.AdcIntervalBegin=1;
                   Key_DI3.AdcIntervalTimeOvr=0;
                   Key_DI3.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key_DI3.AdcIntervalTimeOvr==1)
                { 
                   
                  if(DI3_PIN==0)
                  {
                    if(Key_DI3.PowerInLast==1)
                        Key_DI3.PowerIn=0;
                    Key_DI3.AdcStatus=cIdle;
                       DI3_On_Opreate();
                       Key_DI3.PowerInLast=0;      
                  }
                 else
                  {  if(Key_DI3.PowerInLast==0)
                       Key_DI3.PowerIn=1;
                       DI3_Off_Operate();
                    Key_DI3.AdcStatus=cIdle;
                    Key_DI3.PowerInLast=1; 
               										
                  } 
                }        
                  break;
         default:    Key_DI3.AdcStatus=cIdle;
                  break;    	       
        }         
   }
/*--------------------------------------------------------------------------------------------
//Function: Key_PowerIn_StateMachine()
//Input:  
//Output:   None 
//Description:检查外部电源的状态 Key_PowerIn.PowerIn=0.有外电插入  =1:外部断电 
//--------------------------------------------------------------------------------------------*/
void Key_DI4_StateMachine()
 {
   switch(Key_DI4.AdcStatus)
      {
      	case cIdle: 
                 if(DI4_PIN==0)
                  {
                    if(Key_DI4.PowerInLast!=0)
                       Key_DI4.AdcStatus=cWait;
                  }
                 else
                  {  if(Key_DI4.PowerInLast==0)
                       Key_DI4.AdcStatus=cWait;  
                  }       
                  break;
                  
         case cWait:
               
                   Key_DI4.AdcStatus=cWaitInterval;
                   Key_DI4.AdcSampleInterval=0;
                   Key_DI4.AdcIntervalBegin=1;
                   Key_DI4.AdcIntervalTimeOvr=0;
                   Key_DI4.AdcSampleIntervalSet=2;
                
               break;
         case cWaitInterval:
               if(Key_DI4.AdcIntervalTimeOvr==1)
                { 
                   
                  if(DI4_PIN==0)
                  {
                    if(Key_DI4.PowerInLast==1)
                        Key_DI4.PowerIn=0;
                    Key_DI4.AdcStatus=cIdle;
                       DI4_On_Opreate(); 
                       Key_DI4.PowerInLast=0;     
                  }
                 else
                  {  if(Key_DI4.PowerInLast==0)
                       Key_DI4.PowerIn=1;
                       DI4_Off_Operate();
                    Key_DI4.AdcStatus=cIdle;  
                    Key_DI4.PowerInLast=1;   
                  } 
                }        
                  break;
         default:    Key_DI4.AdcStatus=cIdle;
                  break;    	       
        }         
   }



void  DI1_On_Opreate()
 {
  	                         if(ParaList[cDO1*2+1]==0)
  	                           {
  	                           	 DigitalOutput(1,1);	
  	                                 ParaList[cDO1*2+1]=1;
  	                                 ParaList[cAutoStatusDO1*2+1]=1;
  	                                 PumpBeginCount_1();
  	                            }
  	                          else
  	                             {
  	                           	 DigitalOutput(1,0);	
  	                                 ParaList[cDO1*2+1]=0;
  	                                 ParaList[cAutoStatusDO1*2+1]=1;
  	                            }
 }	
void DI1_Off_Operate()
 {
   
 }
 
void  DI2_On_Opreate()
 {
 if(ParaList[cDO2*2+1]==0)
  	                           {
  	                           	 DigitalOutput(2,1);	
  	                                 ParaList[cDO2*2+1]=1;
  	                                 ParaList[cAutoStatusDO2*2+1]=1;
  	                                 PumpBeginCount_2();
  	                            }
  	                          else
  	                             {
  	                           	 DigitalOutput(2,0);	
  	                                 ParaList[cDO2*2+1]=0;
  	                                 ParaList[cAutoStatusDO2*2+1]=1;
  	                            }	
 }	
void DI2_Off_Operate()
 {
 	
 } 	
 	
void  DI3_On_Opreate()
 {
 if(ParaList[cDO3*2+1]==0)
  	                           {
  	                           	 DigitalOutput(3,1);	
  	                                 ParaList[cDO3*2+1]=1;
  	                                 ParaList[cAutoStatusDO3*2+1]=1;
  	                                 PumpBeginCount_3();
  	                            }
  	                          else
  	                             {
  	                           	 DigitalOutput(3,0);	
  	                                 ParaList[cDO3*2+1]=0;
  	                                 ParaList[cAutoStatusDO3*2+1]=1;
  	                            }	
 }	
void DI3_Off_Operate()
 {
 	
 } 	
void  DI4_On_Opreate()
 {
 if(ParaList[cDO4*2+1]==0)
  	                           {
  	                           	 DigitalOutput(4,1);	
  	                                 ParaList[cDO4*2+1]=1;
  	                                 ParaList[cAutoStatusDO4*2+1]=1;
  	                                 PumpBeginCount_4();
  	                            }
  	                          else
  	                             {
  	                           	 DigitalOutput(4,0);	
  	                                 ParaList[cDO4*2+1]=0;
  	                                 ParaList[cAutoStatusDO4*2+1]=1;
  	                            }	
 }	
void DI4_Off_Operate()
 {
 	
 } 
 
void  PumpBeginCount(unsigned char port)
  {
     switch(port)
      {
      	  case 1: PumpBeginCount_1();break;
      	  case 2: PumpBeginCount_2();break;
      	  case 3: PumpBeginCount_3();break;
      	  case 4: PumpBeginCount_4();break;
      	  default:break;
      }		
  }	

void PumpBeginCount_1()
  {     PumpRunTIme_1.AdcSampleInterval=0;
  	PumpRunTIme_1.PowerInLast=0;
  	PumpRunTIme_1.PowerIn=0;
  	PumpRunTIme_1.AdcIntervalBegin=1;
  	PumpRunTIme_1.AdcIntervalTimeOvr=0;
  }
  
void PumpBeginCount_2()
  {     PumpRunTIme_2.AdcSampleInterval=0;
  	PumpRunTIme_2.PowerInLast=0;
  	PumpRunTIme_2.PowerIn=0;
  	PumpRunTIme_2.AdcIntervalBegin=1;
  	PumpRunTIme_2.AdcIntervalTimeOvr=0;
  }  
  
void PumpBeginCount_3()
  {     PumpRunTIme_3.AdcSampleInterval=0;
  	PumpRunTIme_3.PowerInLast=0;
  	PumpRunTIme_3.PowerIn=0;
  	PumpRunTIme_3.AdcIntervalBegin=1;
  	PumpRunTIme_3.AdcIntervalTimeOvr=0;
  }  
  
void PumpBeginCount_4()
  {     PumpRunTIme_4.AdcSampleInterval=0;
  	PumpRunTIme_4.PowerInLast=0;
  	PumpRunTIme_4.PowerIn=0;
  	PumpRunTIme_4.AdcIntervalBegin=1;
  	PumpRunTIme_4.AdcIntervalTimeOvr=0;
  }  
void  Timer_PumpRun_1()
 { if(PumpRunTIme_1.AdcIntervalBegin==1)
    {
       PumpRunTIme_1.AdcSampleInterval++;
       if(PumpRunTIme_1.AdcSampleInterval>=60)
          {
             PumpRunTIme_1.AdcSampleInterval=0;		
    	     PumpRunTIme_1.PowerInLast++;
    	     if(PumpRunTIme_1.PowerInLast>=60)
    	       {
    	       	   PumpRunTIme_1.PowerInLast=0;
    	       	   PumpRunTIme_1.PowerIn++;
    	       	}
    	   }
    	#ifdef __flood
    	   if(PumpRunTIme_1.PowerIn*60+PumpRunTIme_1.PowerInLast>= ParaList[cWorkTime1*2]*256+ParaList[cWorkTime1*2+1])
    	     {     PumpRunTIme_1.AdcIntervalBegin=0;
    	     	   PumpRunTIme_1.AdcIntervalTimeOvr=1;
    	     	   DigitalOutput(1,0);
    	     	   ParaList[cDO1*2+1]=1;
  	           ParaList[cAutoStatusDO1*2+1]=2;
    	     }
    	 #endif
    	 
    	#ifdef __DoorMagic
    	   if(PumpRunTIme_1.PowerInLast*60+PumpRunTIme_1.AdcSampleInterval>= ParaList[cWorkTime1*2]*256+ParaList[cWorkTime1*2+1])
    	     {  PumpRunTIme_1.AdcIntervalBegin=0;
    	     	PumpRunTIme_1.AdcIntervalTimeOvr=1;
    	     }
    	 #endif      
    }	     	
 }   	     	      	   
    	       	   
 
 

void  Timer_PumpRun_2()
 { if(PumpRunTIme_2.AdcIntervalBegin==1)
    {
       PumpRunTIme_2.AdcSampleInterval++;
       if(PumpRunTIme_2.AdcSampleInterval>=60)
          {
             PumpRunTIme_2.AdcSampleInterval=0;		
    	     PumpRunTIme_2.PowerInLast++;
    	     if(PumpRunTIme_2.PowerInLast>=60)
    	       {
    	       	   PumpRunTIme_2.PowerInLast=0;
    	       	   PumpRunTIme_2.PowerIn++;
    	       	}
    	   }
    	#ifdef __flood
    	   if(PumpRunTIme_2.PowerIn*60+PumpRunTIme_2.PowerInLast>= ParaList[cWorkTime2*2]*256+ParaList[cWorkTime2*2+1])
    	     {     PumpRunTIme_2.AdcIntervalBegin=0;
    	     	   PumpRunTIme_2.AdcIntervalTimeOvr=1;
    	     	   DigitalOutput(2,0);
    	     	   ParaList[cDO2*2+1]=1;
  	           ParaList[cAutoStatusDO2*2+1]=2;
    	     }
    	 #endif
    	 
    	#ifdef __DoorMagic
    	   if(PumpRunTIme_2.PowerInLast*60+PumpRunTIme_2.AdcSampleInterval>= ParaList[cWorkTime2*2]*256+ParaList[cWorkTime2*2+1])
    	     {  PumpRunTIme_2.AdcIntervalBegin=0;
    	     	PumpRunTIme_2.AdcIntervalTimeOvr=1;
    	     }
    	 #endif      
    }	     	
 }   	     	      
 
void  Timer_PumpRun_3()
 { if(PumpRunTIme_3.AdcIntervalBegin==1)
    {
       PumpRunTIme_3.AdcSampleInterval++;
       if(PumpRunTIme_3.AdcSampleInterval>=60)
          {
             PumpRunTIme_3.AdcSampleInterval=0;		
    	     PumpRunTIme_3.PowerInLast++;
    	     if(PumpRunTIme_3.PowerInLast>=60)
    	       {
    	       	   PumpRunTIme_3.PowerInLast=0;
    	       	   PumpRunTIme_3.PowerIn++;
    	       	}
    	   }
    	#ifdef __flood
    	   if(PumpRunTIme_3.PowerIn*60+PumpRunTIme_3.PowerInLast>= ParaList[cWorkTime3*2]*256+ParaList[cWorkTime3*2+1])
    	     {     PumpRunTIme_3.AdcIntervalBegin=0;
    	     	   PumpRunTIme_3.AdcIntervalTimeOvr=1;
    	     	   DigitalOutput(3,0);
    	     	   ParaList[cDO3*2+1]=1;
  	           ParaList[cAutoStatusDO3*2+1]=2;
    	     }
    	 #endif
    	 
    	#ifdef __DoorMagic
    	   if(PumpRunTIme_3.PowerInLast*60+PumpRunTIme_3.AdcSampleInterval>= ParaList[cWorkTime3*2]*256+ParaList[cWorkTime3*2+1])
    	     {  PumpRunTIme_3.AdcIntervalBegin=0;
    	     	PumpRunTIme_3.AdcIntervalTimeOvr=1;
    	     }
    	 #endif      
    }	     	
 }   	     	      
 
 
void  Timer_PumpRun_4()
 { if(PumpRunTIme_4.AdcIntervalBegin==1)
    {
       PumpRunTIme_4.AdcSampleInterval++;
       if(PumpRunTIme_4.AdcSampleInterval>=60)
          {
             PumpRunTIme_4.AdcSampleInterval=0;		
    	     PumpRunTIme_4.PowerInLast++;
    	     if(PumpRunTIme_4.PowerInLast>=60)
    	       {
    	       	   PumpRunTIme_4.PowerInLast=0;
    	       	   PumpRunTIme_4.PowerIn++;
    	       	}
    	   }
    	#ifdef __flood
    	   if(PumpRunTIme_4.PowerIn*60+PumpRunTIme_4.PowerInLast>= ParaList[cWorkTime4*2]*256+ParaList[cWorkTime4*2+1])
    	     {     PumpRunTIme_4.AdcIntervalBegin=0;
    	     	   PumpRunTIme_4.AdcIntervalTimeOvr=1;
    	     	   DigitalOutput(4,0);
    	     	   ParaList[cDO4*2+1]=1;
  	           ParaList[cAutoStatusDO4*2+1]=2;
    	     }
    	 #endif
    	 
    	#ifdef __DoorMagic
    	   if(PumpRunTIme_4.PowerInLast*60+PumpRunTIme_4.AdcSampleInterval>= ParaList[cWorkTime4*2]*256+ParaList[cWorkTime4*2+1])
    	     {  PumpRunTIme_4.AdcIntervalBegin=0;
    	     	PumpRunTIme_4.AdcIntervalTimeOvr=1;
    	     }
    	 #endif      
    }	     	
 }   	     	      	  	 
 
#endif 
 
 #define   RedLed  4
  #define  YellowLed  3
  #define  GreenLed  2
  #define  BellLed   1
void DealMagicLed()
{	
 if(PumpRunTIme_1.AdcIntervalTimeOvr==1) //超时报警到,红灯,声音
	   {
	     //PumpRunTIme_1.AdcIntervalTimeOvr=0;
	     DigitalOutput(RedLed,1);
	     DigitalOutput(BellLed,0);
	     DigitalOutput(YellowLed,0);
	     DigitalOutput(GreenLed,0);
	   }
	  else if(PumpRunTIme_2.AdcIntervalBegin==1) //延时中,黄灯
	   {
	     DigitalOutput(RedLed,0);
	     DigitalOutput(GreenLed,0);
	     DigitalOutput(YellowLed,1);
	     DigitalOutput(BellLed,0);	
	   	   	
	   }
	  else
	   {
	     DigitalOutput(RedLed,0);  //绿灯
	     DigitalOutput(GreenLed,1);
	     DigitalOutput(YellowLed,0);
	     DigitalOutput(BellLed,0);	
	   	
	   }	 
	 }

	 
void TestLamp()
{
       DigitalOutput(RedLed,0);
	     DigitalOutput(BellLed,0);
	     DigitalOutput(YellowLed,0);
	     DigitalOutput(GreenLed,0);
	
	     
        DigitalOutput(RedLed,1);
	      DigitalOutput(RedLed,0);
	      DigitalOutput(YellowLed,1);
	      DigitalOutput(YellowLed,0);
	      DigitalOutput(GreenLed,1);
	      DigitalOutput(GreenLed,0);
	      DigitalOutput(BellLed,1);
	     DigitalOutput(BellLed,0);
	      


}	
	 
/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/


