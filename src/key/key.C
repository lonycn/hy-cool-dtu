#include <stdio.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#if 0
#define cIdle   0 
#define cWait   1
#define cWaitInterval  2
#define cWaitDelay     3
#define cWaitLoose     4
#define DI1_PIN  PD14
#define DI2_PIN  PB9
#define DI3_PIN  PE5
#define DI4_PIN  PC2

void  DI1_On_Opreate();
void DI1_Off_Operate();

struct Key_Struct1
   {  
   	 uint8_t Key_Status;
  	 uint16_t Key_Interval;
         uint8_t Key_IntervalBegin;
         uint8_t Key_IntervalTimeOvr;
         uint16_t Key_SampleIntervalSet;
         uint8_t Key_Code;   //1: press  2:loose  3:delay
         uint8_t Key_OnOFF;  //上一个电平状态
         uint8_t Key_Sample;
         uint8_t Key_Ratio;  //比例系数
         uint8_t Alarm;
         uint8_t HideSound;
         
   };    

 struct Key_Struct1   Key_DI1,Key_Delay1,PumpRunTIme_1;

/*--------------------------------------------------------------------------------------------
//Function:void Key_DI1_Initial(void)
//Input:  
//Output:   None 
//Description:di1的初始化部分,
//--------------------------------------------------------------------------------------------*/
void Key_DI1_Initial(void)  //debounce
{
   Key_DI1.Key_OnOFF=0xff;
   Key_DI1.Key_Code=0xff;
   Key_DI1.Key_SampleIntervalSet=1;
   Key_DI1.Key_Status=cIdle;
   #ifdef  __flood
        PumpRunTIme_1.Key_Interval=0;
  	PumpRunTIme_1.Key_Ratio=0;
  	PumpRunTIme_1.Key_IntervalBegin=0;
  	PumpRunTIme_1.Key_IntervalTimeOvr=0;
  	DigitalOutput(1,0);
    	ParaList[cDO1*2+1]=0;
  	ParaList[cAutoStatusDO1*2+1]=2;
  	ParaList[cRuntimeDO1*2]=0;
      	ParaList[cRuntimeDO1*2+1]=0;
  #endif	
  	
}

/*--------------------------------------------------------------------------------------------
//Function:void Key_Delay1_Initial(void)  PumpBeginCount_1()
//Input:  
//Output:   None 
//Description:根据不同的宏定义,确定是否初始化magic或者flood
//--------------------------------------------------------------------------------------------*/
#ifdef  __DoorMagic
void Key_Delay1_Initial(void)  //magic
{
   Key_Delay1.Key_OnOFF=0x00;
   Key_Delay1.Key_Code=0xff;
   Key_Delay1.Key_SampleIntervalSet=0;
   Key_Delay1.Key_Status=cIdle;
   Key_Delay1.HideSound=1;
}
#endif

#ifdef  __flood
void PumpBeginCount_1()
  {     PumpRunTIme_1.Key_Interval=0;
  	PumpRunTIme_1.Key_Ratio=0;
  	PumpRunTIme_1.Key_IntervalBegin=1;
  	PumpRunTIme_1.Key_IntervalTimeOvr=0;
  }
#endif

/*--------------------------------------------------------------------------------------------
//Function:void Timer_KeyDI1Interval()
//Input:  
//Output:   None 
//Description:DI1的debounce延时处理定时
//--------------------------------------------------------------------------------------------*/
void Timer_KeyDI1Interval()
  {
    if(Key_DI1.Key_IntervalBegin==1)
      {
      	 Key_DI1.Key_Interval++;
      	 if(Key_DI1.Key_Interval>=Key_DI1.Key_SampleIntervalSet)
      	   { Key_DI1.Key_IntervalBegin=0;
      	     Key_DI1.Key_IntervalTimeOvr=1;
          }		
       }	
  }

/*--------------------------------------------------------------------------------------------
//Function:Timer_Key_Delay1Interval()
//Input:  
//Output:   None 
//Description:DI1的门磁延时处理程序
//--------------------------------------------------------------------------------------------*/
#ifdef  __DoorMagic
void Timer_Key_Delay1Interval()
  {
    if(Key_Delay1.Key_IntervalBegin==1)
      {  
      	 Key_Delay1.Key_Interval--;
      	 ParaList[cRuntimeDO1*2]=Key_Delay1.Key_Interval/256;  //距离报警还剩下的秒数
      	 ParaList[cRuntimeDO1*2+1]=Key_Delay1.Key_Interval%256;
      	 if(Key_Delay1.Key_Interval==0)
      	   { Key_Delay1.Key_IntervalBegin=0;
      	     Key_Delay1.Key_IntervalTimeOvr=1;
          }		
       }	
  }
#endif

/*--------------------------------------------------------------------------------------------
//Function:Timer_Key_Delay1Interval()
//Input:  
//Output:   None 
//Description:DI1的水泵延时处理程序
//--------------------------------------------------------------------------------------------*/
#ifdef __flood
void  Timer_PumpRun_1()
 { 
    if(PumpRunTIme_1.Key_IntervalBegin==1)
      {
      	 
      	
      	 if(PumpRunTIme_1.Key_Ratio++)
      	  { if(PumpRunTIme_1.Key_Ratio>=60)
      	      {
      	      	PumpRunTIme_1.Key_Ratio=0;
      	      	PumpRunTIme_1.Key_Interval++;
      	      	ParaList[cRuntimeDO1*2]=PumpRunTIme_1.Key_Interval/256;
      	      	ParaList[cRuntimeDO1*2+1]=PumpRunTIme_1.Key_Interval%256;
      	      	
      	       if(PumpRunTIme_1.Key_Interval>= ParaList[cWorkTime1*2]*256+ParaList[cWorkTime1*2+1])
    	        {     PumpRunTIme_1.Key_IntervalBegin=0;
    	     	   PumpRunTIme_1.Key_IntervalTimeOvr=1;
    	     	   DigitalOutput(1,0);
    	     	   ParaList[cDO1*2+1]=0;
  	           ParaList[cAutoStatusDO1*2+1]=2;
  	           ParaList[cRuntimeDO1*2]=0;
      	      	   ParaList[cRuntimeDO1*2+1]=0;
  	   	}
    	 
             }	     	
        }
     } 
 }
 
void TimeOut()
{	
 if(PumpRunTIme_1.Key_IntervalTimeOvr==1)
 { PumpRunTIme_1.Key_IntervalTimeOvr=0;
	 SendFloodStatus();
 }
 }
#endif 
 

 
 
#define   RedLed  4
#define  YellowLed  3
#define  GreenLed  2
#define  BellLed   1

void DI1_initial()
 {
    Key_DI1_Initial();	
 #ifdef  __DoorMagic	
    Key_Delay1_Initial( );
    
 #endif
}

void Timer_DI1()
 {
  
   #ifdef __flood
     Timer_PumpRun_1();
   #endif  	
 	
  #ifdef  __DoorMagic	
    Timer_Key_Delay1Interval();
  #endif	
 }
 
 


#ifdef  __flood
  
void PumpBeginCount_2()
  {     
  }  
  
void PumpBeginCount_3()
  {     
  }  
  
void PumpBeginCount_4()
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
#endif  	



#define  WaitLoose
#ifdef  __flood
     
     #define  ValidLevel    0
#else 
	    #ifdef  __DoorMagic
         #define  ValidLevel    1
	    #else
	       #define  ValidLevel    1
	    #endif
  
void Key_DI1_StateMachine()
 {
  #if  0   
	 switch(Key_DI1.Key_Status)
      {
      	case cIdle: 
                 if(DI1_PIN==ValidLevel)
                  { 
                   
                       Key_DI1.Key_Status=cWait;
                  }
                 
                 break;
                  
         case cWait:
                 
                   Key_DI1.Key_Status=cWaitInterval;
                   Key_DI1.Key_Interval=0;
                   Key_DI1.Key_IntervalBegin=1;
                   Key_DI1.Key_IntervalTimeOvr=0;
                   Key_DI1.Key_SampleIntervalSet=1;
                
               break;
         case cWaitInterval:
               if(Key_DI1.Key_IntervalTimeOvr==1)
                {   
                  if(DI1_PIN==ValidLevel)//抖动处理后正确
                  {
			  Key_DI1.Key_Code=1; //press
			  ParaList[cDI1*2]=0;
      	      	          ParaList[cDI1*2+1]=1;
			  #ifdef WaitLoose
			    Key_DI1.Key_Status=cWaitLoose;
			  #else  
			    Key_DI1.Key_Status=cIdle;
			  #endif  
                           Key_DI1.Key_Interval=0;
                           Key_DI1.Key_IntervalBegin=0;
                           Key_DI1.Key_IntervalTimeOvr=0;
		  }
		 else
		  {
		           Key_DI1.Key_Status=cIdle;
			   Key_DI1.Key_Interval=0;
                           Key_DI1.Key_IntervalBegin=0;
                           Key_DI1.Key_IntervalTimeOvr=0;	
		  	
		  }
		}  
		break;  
	case  cWaitLoose:
	   
	    	if(DI1_PIN!=ValidLevel)	
	    	  {        ParaList[cDI1*2]=0;
      	      	           ParaList[cDI1*2+1]=0;
	    	           Key_DI1.Key_Status=cIdle;
			   Key_DI1.Key_Interval=0;
                           Key_DI1.Key_IntervalBegin=0;
                           Key_DI1.Key_IntervalTimeOvr=0;
                           Key_DI1.Key_Code=0;
	    	  }
	    	break;
	    }
 #endif
 }	       	  	
			  
			  
			
     
void  DI1_On_Opreate() 
 {
   #ifdef  __flood	
                    if(Key_DI1.Key_Code==1)
                     {
                     	 Key_DI1.Key_Code=0;   
                              if(ParaList[cDO1*2+1]==0)
  	                           {
  	                           	 DigitalOutput(1,1);	 //开启水泵
  	                                 ParaList[cDO1*2+1]=1;  //启停控制单元为启动状态
  	                                 ParaList[cAutoStatusDO1*2+1]=1; //为手动状态
  	                                 PumpBeginCount_1();  //开始定时
  	                            }
  	                          else
  	                             {
  	                           	 DigitalOutput(1,0);	//关闭水泵
  	                                 ParaList[cDO1*2+1]=0;  //启停控制单元为关闭状态
  	                                 ParaList[cAutoStatusDO1*2+1]=1; //为手动状态
  	                                 PumpRunTIme_1.Key_IntervalBegin=0; //开启及时
  	                                 ParaList[cRuntimeDO1*2]=0;
      	      	                         ParaList[cRuntimeDO1*2+1]=0;
  	                            }
			SendFloodStatus();
  	              }              
    #endif	                            
  	                            
   #ifdef  __DoorMagic	 
//ParaList[cRuntimeDO1Ratio*2+1]=3;//FOR TEST									
  	 if(ParaList[cRuntimeDO1Ratio*2+1]==0)
  	  {
  	  	
  	                  DigitalOutput(RedLed,0);
	                    DigitalOutput(GreenLed,0);
	                    DigitalOutput(YellowLed,0);
	                    DigitalOutput(BellLed,0);	
  	  	
  	  	
  	  	
  	  }
  	else
  	  {  	  
  	   
  	    if(Key_DI1.Key_Code==1)                         
  	      {  if(Key_DI1.Key_OnOFF==0)
  	            {
  	              Key_Delay1_Initial();
  	              Key_DI1.Key_OnOFF=1;
  	              Key_Delay1.Key_IntervalTimeOvr=0;
									if(ParaList[cRuntimeDO1Ratio*2+1]==9)
										 Key_Delay1.Key_Interval=3;
									else
  	          //       Key_Delay1.Key_Interval= (ParaList[cWorkTime1*2]*256+ParaList[cWorkTime1*2+1])*ParaList[cRuntimeDO1Ratio*2+1];
      	             Key_Delay1.Key_Interval=ParaList[cRuntimeDO1Ratio*2+1]*ParaList[cRuntimeDO1Ratio*2+1]*60;
      	              Key_Delay1.Key_IntervalBegin=1;
      	              Key_Delay1.HideSound=1; 
      	    
                    }			
  	           else
  	           {
  	             if((Key_Delay1.Key_IntervalTimeOvr==1)&&(Key_DI1.Alarm==1)) //延时到,预警状态
  	                 {
  	           	    
  	           	  //超时报警到,红灯,声音
	                    DigitalOutput(RedLed,1);
	                    if((Key_Delay1.HideSound==1)||(ParaList[cRuntimeDO1Ratio*2+1]==0))
	                      DigitalOutput(BellLed,1);
	                    DigitalOutput(YellowLed,0);
	                    DigitalOutput(GreenLed,0);
	                    Key_DI1.Alarm=2;
	                  }
	                 else  //延时中,黄灯  press
	                  {
											if(Key_DI1.Alarm==0)   // 正常--预警
											{
	                    DigitalOutput(RedLed,0);
	                    DigitalOutput(GreenLed,0);
	                    DigitalOutput(YellowLed,1);
	                    DigitalOutput(BellLed,0);
	                    Key_DI1.Alarm=1;
											}
										else   
										  {											
	                    if(Key_Delay1.Key_Interval==3)
	                      Bellcontrol(3,200,200);	
										  }
	                  	   	
	                  }
									}
	           } 
	   
	  else
	    {
	    	 DigitalOutput(RedLed,0);  //绿灯
	         DigitalOutput(GreenLed,1);
	         DigitalOutput(YellowLed,0);
	         DigitalOutput(BellLed,0);
	         Key_DI1.Key_OnOFF=0;
	         ParaList[cRuntimeDO1*2]=ParaList[cRuntimeDO1Ratio*2+1]*ParaList[cRuntimeDO1Ratio*2+1]*60/256;
      	      	ParaList[cRuntimeDO1*2+1]=ParaList[cRuntimeDO1Ratio*2+1]*ParaList[cRuntimeDO1Ratio*2+1]*60%256;
	        Key_DI1.Alarm=0;
	   	
	   }	 	                    
  	                           
 }
#endif 
} 
 
#endif  
 
 unsigned char GetDiValue(unsigned char port)
 {unsigned char val;
    switch(port)
    {
    	case 1:  if(DI1_PIN==1)
    	           val=1;
    	         else
    	           val=0;  
    	         break;
    	         
    	case 2: if(DI2_PIN==1)
    	           val=1;
    	         else
    	           val=0;  
    	         break;       
    	case 3:  if(DI3_PIN==1)
    	           val=1;
    	         else
    	           val=0;  
    	         break;        
      case 4: if(DI4_PIN==1)
    	           val=1;
    	         else
    	           val=0;  
    	         break;          	
 	default:break;
   }		
 	return(val);
 	
 }	 
 
void GetDiValuePort( )
  {
#if  0
    unsigned char i; 	
    for(i=1;i<4;i++)
     { 	if(GetDiValue(i)==1)
      	 { ParaList[(cDI1+(i-1))*2]=0;	
  	   ParaList[(cDI1+(i-1))*2+1]=1;
  	 }
  	else
  	 { ParaList[(cDI1+(i-1))*2]=0;	
  	   ParaList[(cDI1+(i-1))*2+1]=0;
  	 }                  
     }	
#endif		 
  }

  
unsigned char  ReturnAlarmStatus()
 {
 	  
   return(Key_DI1.Alarm	);
   
}		


/********************************************************
函数功能：switch_Init(void)    拨码初始化            
入口参数：无
返回  值：无
*********************************************************/
void switch_Init(void)
{   
#ifdef __DoorMagic	
     GPIO_SetMode(PA, BIT2, GPIO_PMD_INPUT);
     GPIO_SetMode(PA, BIT3, GPIO_PMD_INPUT);
     GPIO_SetMode(PA, BIT4, GPIO_PMD_INPUT);
     GPIO_SetMode(PC, BIT7, GPIO_PMD_INPUT);
	
	   GPIO_ENABLE_PULL_UP(PA,BIT2);
	GPIO_ENABLE_PULL_UP(PA,BIT3);
	GPIO_ENABLE_PULL_UP(PA,BIT4);
	GPIO_ENABLE_PULL_UP(PC,BIT7);
	GPIO_ENABLE_PULL_UP(PB,BIT8);
	GPIO_ENABLE_PULL_UP(PD,BIT14);
	
#endif     
    
}
/********************************************************
函数功能：void  StateMachine_Switch()  拨码状态监测和消音按键处理         
入口参数：无
返回  值：无
*********************************************************/
void  StateMachine_Switch()
 {
   uint8_t  temp;	
//按下报警静音按钮,关闭声音报警,消音30分钟,  	
#ifdef __DoorMagic
 #if 1
	 if(PB8==0)
        {    while(PB8==0);
           if(Key_DI1.Alarm==2) //报警中
             { Key_Delay1.HideSound=0;  
	       DigitalOutput(BellLed,0);
	        Key_Delay1.Key_IntervalTimeOvr=0;
	        if(ParaList[cRuntimeDO1Ratio*2+1]==9)
	          Key_Delay1.Key_Interval=3;   //如果在0档,为测试档,消音10s
	        else
	          Key_Delay1.Key_Interval=1800;   //30分钟
      	        Key_Delay1.Key_IntervalBegin=1;
	     }  	
	}
#endif				
       if((Key_DI1.Alarm==2)&&(Key_Delay1.Key_IntervalTimeOvr==1))
        { Key_Delay1.Key_IntervalTimeOvr=0;
          Key_Delay1.HideSound=1;  
	  DigitalOutput(BellLed,1);
        }
//检查是当前设置的时间
temp=0;
 if(PA3==0)
   temp+=1;
 if(PA4==0)
   temp+=2; 
 if(PC7==0)
   temp+=4;  
 if(PA2==0)
   temp+=8; 
   ParaList[cRuntimeDO1Ratio*2+1]=temp; 
// Key_Delay1.Key_Interval=ParaList[cRuntimeDO1Ratio*2+1]*ParaList[cRuntimeDO1Ratio*2+1]*60;
 #endif          
 }		
 
 #endif
 