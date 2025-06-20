#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#include "sys.h"
#include "rtc.h"
#define _GLOBAL_H
#include	"global.h"

uint8_t ModeBeforeStatus[5]={100,100,100,100,100};
uint8_t    WorkFlag[10];
extern  uint8_t uart0_OrigPara_List[cContorlRegLen*20];
extern  uint8_t uart0_Var_List[320];
//---------------------------------------------------------------
//Function: void GetCoupleList()
//Input:    None
//Output:   None
//Description: 将机组的配对情况进行整合，按照顺序每对进行排列
//----------------------------------------------------------------
void GetCoupleList()
{
	uint8_t  i,k=0,List[10]={0,0,0,0,0,0,0,0,0,0}; 
   for(i=0;i<10;i++)
	   if(List[i]!=1)
			 {  
				  Couple[k++]=i+1;
				  Couple[k++]=ParaList[(cLoraNodeOffset+i*3+1)*2+1];
	        List[ParaList[(cLoraNodeOffset+i*3+1)*2+1]-1]=1;  //前面的机组先开，后面的机组不�
      }
}	


//---------------------------------------------------------------
//Function: void GetCoupleList()
//Input:    None
//Output:   None
//Description: 将机组的配对情况进行整合，按照顺序每对进行排列
//----------------------------------------------------------------
 uint8_t  LastCurrentTime=0; 
 void CountWatchdog()
 {//----------------20250217	
	if((ParaList[cHour*2+1]==0)&&(LastCurrentTime==23))
	{
		 ResetNormalWatch();
		
	}
	LastCurrentTime=ParaList[cHour*2+1];
//-----------------------	
 }
void CountWorkStatus(uint8_t i)

{
	uint16_t  No1Time,No2Time,currentTime;
	No1Time=(ParaList[No1StartHour*2+i*4]*256+ParaList[No1StartHour*2+i*4+1])*60+(ParaList[No1StartHour*2+i*4+2]*256+ParaList[No1StartHour*2+i*4+3]);
  No2Time=(ParaList[No1StartHour*2+i*4+4]*256+ParaList[No1StartHour*2+i*4+5])*60+(ParaList[No1StartHour*2+i*4+6]*256+ParaList[No1StartHour*2+i*4+7]);
  currentTime=(ParaList[cHour*2]*256+ParaList[cHour*2+1])*60+(ParaList[cHour*2+2]*256+ParaList[cHour*2+3]);

	
 if(i==0)
 {
	if(No1Time>No2Time) //24hour 中  1号机晚于2号机开启，切换时间
	{
		if((currentTime>=No1Time)||(currentTime<=No2Time))  //轮值时间
		{ 
			
          WorkFlag[i*2+1]=1;  //1号机组工作，二号机设置定时结束标志
			    WorkFlag[i*2]=0;  
				}
	 else
	 {
      WorkFlag[i*2+1]=0;  //2号机组工作，
			    WorkFlag[i*2]=1; //1号机设置定时结束标志

	 }	
 }
else
{
	 if((currentTime>=No1Time)&&(currentTime<=No2Time))
		{  WorkFlag[i*2+1]=1;  //1号机组工作，二号机设置定时结束标志
			    WorkFlag[i*2]=0;  
		}
	 else
	 {
      WorkFlag[i*2+1]=0;  //2号机组工作，
			    WorkFlag[i*2]=1; //1号机设置定时结束标志

	 }	
	
}
}
}

/*---------------------------------------
add  20230228 	将轮值时间到的部分更改为
-------------------------------------------*/	
uint8_t GetSwitchMiniterflag1(uint8_t i)
	 {
  	    	
  	return(WorkFlag[i]);
  }			

/*---------------------------------------------------------------
//Function: void DealWithTimerSwtich()
//Input:    None
//Output:   None
//Description: 双机切换模式----> 一对机组都设置为切换----> 都不在除霜状态----->1号机组定时到---> 2号无故障，关闭1号---> 2号为关闭状态，开启2号
                                                                                             --> 2号故障，继续启动1号  
                                                                        ----->1号机组定时到---> 1号无故障，关闭2号---> 1号为关闭状态，开启1号
                                                                                             --> 1号故障，继续启动2号  
//----------------------------------------------------------------*/
void DealWithTimerSwtich()
{	
   uint8_t  i,sta;
	   uint8_t ErrorTab[5]={0,0,0,0,0};	
	sta=0;
   for(i=0;i<5;i++)
    {  CountWorkStatus(i) ; 
     if(ParaList[(cRemoteControlmode+i)*2+1]==0) //自动轮值
		 {
			if((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]!=0) && (ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]!=0)) //	切换都处于激活
        {			  
	     
//20240303					
					if( ((GetConSensorStatus(Couple[i*2],cFrozenStatus)==0)&&(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==0))   )//不在化霜中
					 {	//-----------------------  
				  if(GetSwitchMiniterflag(Couple[i*2]-1)==1) //timer 1 over
					{ //20230723 SetCoolComposation(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
						if( GetAllError(Couple[i*2+1])==0)  //    2 ok
						{ if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)   //1 opem 
							 {  uart0_OpenOrClose(Couple[i*2],0);   //close 1
							  #ifdef Debuginfor
								  SendString_uart0("timer 1, 2 close, ,2 ok,close 1\r\n\r\n");
								 #endif
								 sta=1;ErrorTab[i]	=1;
							 }
							 if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0)  //2 close
							 { uart0_OpenOrClose(Couple[i*2+1],1); //2 open
								 SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
								  sta=1;ErrorTab[i]	=1;
                 ClrSwitchTimer(Couple[i*2]);
                 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
							 #ifdef Debuginfor
								  SendString_uart0("timer 1, 2 close, ,2 ok,open  2\r\n\r\n");
								 #endif
							 }
						 }
						else // 2 error  set timer1  
						{	SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
							#ifdef Debuginfor
							 SendString_uart0("timer 1, 2 error ,1 go on\r\n\r\n");
							#endif
						}
					 }
					
       if(GetSwitchMiniterflag(Couple[i*2+1]-1)==1)  //timer 2 arriver
					{
						if( GetAllError(Couple[i*2])==0)  // 1 ok
						{ if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1)   //2 open
							  { uart0_OpenOrClose(Couple[i*2+1],0);  //close 2
								#ifdef Debuginfor
									SendString_uart0("timer 2, 1 close,1 ok,close 2\r\n\r\n");
								#endif
									 sta=1;ErrorTab[i]	=1;
								}
							 if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)  //1 close
							 { uart0_OpenOrClose(Couple[i*2],1);  //open 1
                 ClrSwitchTimer(Couple[i*2+1]);
								 SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
                 SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
							  #ifdef Debuginfor
								  SendString_uart0("timer 2, 1 close,1 ok,open  1\r\n\r\n");
								 #endif
								  sta=1;ErrorTab[i]	=1;
							 }
						 }
						else
						{	SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
					   #ifdef Debuginfor
							 SendString_uart0("timer 1, 1 error ,2 go on\r\n\r\n");
							#endif
						}
					}
						 
			 	
			}
		}
	}
	 if(sta==1)
	{  
		for(i=0;i<5;i++)
		 {if(ErrorTab[i]==1)
		    { uart0_UpdateOpenOrClose(Couple[i*2]);
	        uart0_UpdateOpenOrClose(Couple[i*2+1]); 
					SetSwitchMiniter(10,2);//20s
		    }
	    }
		}
	else
		 SetStatusDelayFlag();  
}
}

/*---------------------------------------------------------------
//Function: DealWithFrozenCheckSwtich()
//Input:    None
//Output:   None
//Description: 化霜过程中是否启动备机切换,只要工作机在化霜操作中，如果备机没有启动，则启动备机
运行条件： 双机切换--》双机激活--》化霜切换备机使能---》当前工作机化霜--》备机无故障----》开启备机----》设置状态回读命令---》设置20s静置时间
//----------------------------------------------------------------*/
void  DealWithFrozenCheckSwtich()
 {
   uint8_t  i,sta;
	   uint8_t ErrorTab[5]={0,0,0,0,0};	
  sta=0;		 
  for(i=0;i<5;i++)
    {  
		
//----------------------------------------			
  if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//switch
		{	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //alive
                               {			
	     
				  if(ParaList[(cForzeOpenBakMotor+i)*2+1]==1) //frozen switch enable
				    {
				    	if(GetConSensorStatus(Couple[i*2],cFrozenStatus)==1) //	motor 1 forzen
					
					 {
					     if( GetAllError(Couple[i*2+1])==0)   //1 ok	
						if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0)
						     {
						     	ClrSwitchTimer(Couple[i*2]);
						     	uart0_OpenOrClose(Couple[i*2+1],1); //open 2
						     	sta=1;	 ErrorTab[i]	=1;
						     }
					}
				   
				      if(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==1) //	motor 1 forzen
					
					 {
					     if( GetAllError(Couple[i*2])==0)   //1 ok	
						if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)
						     {
						     	ClrSwitchTimer(Couple[i*2+1]);
						     	uart0_OpenOrClose(Couple[i*2],1); //open 2
						     	sta=1;	 ErrorTab[i]	=1;
						     }
					}
				   
				   
				   }
				}   
			}	   
		  if(sta==1)
	{  
		for(i=0;i<5;i++)
		 {if(ErrorTab[i]==1)
		    { uart0_UpdateOpenOrClose(Couple[i*2]);
	        uart0_UpdateOpenOrClose(Couple[i*2+1]); 
					SetSwitchMiniter(10,2);//20s
		    }
	    }
		}
	else
		 SetStatusDelayFlag(); 
		  }		  
}
  uint8_t  NormalFlag=0;

/*---------------------------------------------------------------
//Function:void  DealWithCoolCompensate()
//Input:    None
//Output:   None
//Description: 冷补偿功能
               双机模式---强1，强2，轮值模式---模式有切换，设置开启冷补偿时间
                                            ---没有切换，计算开启冷补偿时间到----制冷温度是否在设定稳定区间----不在----将模式切换为双强模式
//----------------------------------------------------------------*/
#if   0
void  DealWithCoolCompensate()
{  uint8_t  i;
	 uint16_t  m,n,k,l;
	 int16_t  Sensor,Error,SetTemp,Sensor1,Error1,SetTemp1;
	for(i=0;i<5;i++)
    {  
	
//----------------------------------------			
  	  
   	if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//一对机组切换功能使能
		{	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //双机组都激活使用
        {			
	       
						 if((ParaList[(cRemoteControlmode+i)*2+1]==0)||(ParaList[(cRemoteControlmode+i)*2+1]==1)||(ParaList[(cRemoteControlmode+i)*2+1]==2))
						    { //强1，强2，轮值模式
									if(ModeBeforeStatus[i]!=ParaList[(cRemoteControlmode+i)*2+1]) //状态发生改变
                  {   ModeBeforeStatus[i]=ParaList[(cRemoteControlmode+i)*2+1];
										  SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
										NormalFlag=0;
										uart0_Var_List[TempCompesentStatus*2+1]=0;
										uart0_Var_List[TempCompesentStatus*2+3]=0;
									}
								 else  
								 {   
                        //传感器温度
												 Sensor=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*4]*256+uart0_OrigPara_List[i*cContorlRegLen*4+1]);
												 Sensor1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+1]);
												 //温差
												 Error=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+12*2+1]);
												  Error1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2+1]);
												 //设定温度
												 SetTemp=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+47*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+47*2+1]);
											    SetTemp1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+47*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+47*2+1]);
												 
												 if(((Sensor>SetTemp+Error)||(Sensor1>SetTemp1+Error1))&(ParaList[cCoolCompensateEnable*2+1]==1))//over  tempeature
												 {   
												   
													 if(NormalFlag==0)
													 { NormalFlag=1;
														 SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
												   }
													 else
													 { 
													    if(CoolCompensateStatus(i)==1) //timer 1 over
															{  
																
																if((GetConSensorStatus(Couple[i*2],cFrozenStatus)==1)||(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==1))
																{
																}
																else
																{ //202501 ParaList[(cRemoteControlmode+i)*2+1]=3;
													       ModeBeforeStatus[i]=ParaList[(cRemoteControlmode+i)*2+1];
													       uart0_Var_List[TempCompesentStatus*2+1]=1;
													       if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)
														        uart0_Var_List[TempCompesentStatus*2+3]=Couple[i*2+1];
													      else  if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1)
														       uart0_Var_List[TempCompesentStatus*2+3]=Couple[i*2];
															}
												      }	
													 }
												 }
												 else   //normal  tempeature
												 { SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
												   NormalFlag=0;
											  }
                   }									 
							 }		
					  }
					}
				}
			}
			
			#endif
/*---------------------------------------------------------------
//Function:void DealWithCheckDoubleMotorSwtich()
//Input:    None
//Output:   None
//Description: 运行过程中，检测是否由于人为操作或者参数设置导致机组出现不运行状态
		            如果机组都在运行，定时都启动，但是不会执行切换
		            如果有切换，有一个机组启动，则需要检查当前机组的定时开启，另外一个机组的定时关闭
		            如果
运行条件： 双机切换--》双机激活--》自动切换使能---》双机没没有处于化霜状态或者化霜不切换---》根据故障状态，保证正常的机组有一组处于工作状态----》设置状态回读命令---》设置20s静置时间                          
//----------------------------------------------------------------*/
void DealWithCheckDoubleMotorSwtich()
{	
   uint8_t  i,	sta=0;
   uint8_t ErrorTab[5]={0,0,0,0,0};	
   for(i=0;i<5;i++)
    {  
	
//----------------------------------------			
  	  
   	if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//switch
		{	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //alive
        {			
	       
				  
					 if( (ParaList[(cForzeOpenBakMotor+i)*2+1]==0)||((GetConSensorStatus(Couple[i*2],cFrozenStatus)==0)&&(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==0))   )//frozen switch enable
				   {
				    	
 //-----------------------------------
						 if(ParaList[(cRemoteControlmode+i)*2+1]==0)  //turn
						 {

						 //-----------------------  //1 open  2CLOSE
				 if((GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)&&(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0))  //1 open  2CLOSE
					{
					   if( GetAllError(Couple[i*2])==0)   //1 ok
						 { ClrSwitchTimer(Couple[i*2+1]);
							 if(GetTimerOnStatus(Couple[i*2]-1)==0)
								 SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
						 } 
						else  //1 error
						{    GetAllError(Couple[i*2]);
               	if( GetAllError(Couple[i*2+1])==0)   //2 ok
						       { ClrSwitchTimer(Couple[i*2]);
										 
										 uart0_OpenOrClose(Couple[i*2],0);  //close 1
										 uart0_OpenOrClose(Couple[i*2+1],1); //open 2
									#ifdef Debuginfor
										 	 SendString_uart0("1 open, 2 close, 1 error ,2 ok,switch to 2\r\n\r\n");
                  #endif                                                           
										 sta=1;	  ErrorTab[i]	=1;
										 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
									 }
						}
					}
	 			
		else	if((GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)&&(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1))  //2 open  1CLOSE
					{
					   if( GetAllError(Couple[i*2+1])==0)   //2 ok
						 { ClrSwitchTimer(Couple[i*2]);
							 if(GetTimerOnStatus(Couple[i*2+1]-1)==0)
								 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
						 } 
						else  //2 error
						{
               	if( GetAllError(Couple[i*2])==0)   //1 ok
						       { ClrSwitchTimer(Couple[i*2+1]);
										 uart0_OpenOrClose(Couple[i*2+1],0); 
										 uart0_OpenOrClose(Couple[i*2],1);
                                                                                 sta=1;	ErrorTab[i]	=1;
										 SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
									   #ifdef Debuginfor
										 	 SendString_uart0("2 open, 1 close, 2 error ,1 ok ,switch to 1\r\n\r\n");
										#endif
									 }
						}
					}						 
//----------------------------------------------------	//2 close 1CLOSE						
else	if((GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)&&(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0))  //2 close 1CLOSE
					{
					   if( GetAllError(Couple[i*2+1])==0)   //2 ok
						 { ClrSwitchTimer(Couple[i*2]);
							 if(GetTimerOnStatus(Couple[i*2+1]-1)==0)
								 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
						   uart0_OpenOrClose(Couple[i*2+1],1);
						#ifdef Debuginfor
							  SendString_uart0("1 close, 2 close, 2 ok,switch to 2\r\n\r\n");
							#endif
							sta=1;
               ErrorTab[i]	=1;						 
						 } 
						else  //2 error
						{
               	if( GetAllError(Couple[i*2])==0)   //1 ok
						       { ClrSwitchTimer(Couple[i*2+1]);
										 uart0_OpenOrClose(Couple[i*2],1);
                     SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
								   #ifdef Debuginfor
										 		 SendString_uart0("1 close, 2 close, 1 ok,2 error switch to 1\r\n\r\n");
										#endif
										sta=1;	
										 ErrorTab[i]	=1;
									 }
						}
					}	
					
//----------------------------------------------------						
else	  //all open 
					{
					   if( GetAllError(Couple[i*2+1])==0)   //2 ok
						 { ClrSwitchTimer(Couple[i*2]);
							 if(GetTimerOnStatus(Couple[i*2+1]-1)==0)
								 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
						   uart0_OpenOrClose(Couple[i*2],0);
							 #ifdef Debuginfor
							 SendString_uart0("1 open, 2 open, 2 ok, close 1\r\n\r\n");
						#endif
							 sta=1;	
							 ErrorTab[i]	=1;
						 } 
						else  //2 error
						{
               	if( GetAllError(Couple[i*2])==0)   //1 ok
						       { ClrSwitchTimer(Couple[i*2+1]);
										 uart0_OpenOrClose(Couple[i*2+1],0);
								//		  SendString_uart0("1 open, 2 open, 1 ok,2 error, close 2\r\n\r\n");
										  sta=1;	
										 ErrorTab[i]	=1;
                     SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
									 }
						}
					}	
		}

//----------------------------------------------------		//force   motor1		
	else	if(ParaList[(cRemoteControlmode+i)*2+1]==1)
 {	 
			
	 
	     //-----------------------  //1 open 
				if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)
					{
					   if( GetAllError(Couple[i*2])==0)   //1 ok
						 {  ClrSwitchTimer(Couple[i*2+1]);
							 if(GetTimerOnStatus(Couple[i*2]-1)==0)
								 SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
						  if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1) //2 open
                 {  uart0_OpenOrClose(Couple[i*2+1],0); 
                   sta=1; 
								 ErrorTab[i]	=1;}	
						 } 
						else  //1 error
						{    GetAllError(Couple[i*2]);
               	if( GetAllError(Couple[i*2+1])==0)   //2 ok
						       { ClrSwitchTimer(Couple[i*2]);
										 uart0_OpenOrClose(Couple[i*2],0);  
										 uart0_OpenOrClose(Couple[i*2+1],1);
									 sta=1;
ErrorTab[i]	=1;										 
                     SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
									 }
						}
					}	
			 else  // 1close
			 {
				   if( GetAllError(Couple[i*2])==0)   //1 ok
						 {  ClrSwitchTimer(Couple[i*2+1]);
							 if(GetTimerOnStatus(Couple[i*2]-1)==0)
								 SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
						  if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1)
                                                          uart0_OpenOrClose(Couple[i*2+1],0);
                 
							uart0_OpenOrClose(Couple[i*2],1);  
							sta=1;	
							ErrorTab[i]	=1;
						 } 
						else  //1 error
						{    GetAllError(Couple[i*2]);
               	if( GetAllError(Couple[i*2+1])==0)   //2 ok
						       { ClrSwitchTimer(Couple[i*2]);
										 uart0_OpenOrClose(Couple[i*2],0);
										 sta=1;	
										 ErrorTab[i]	=1;
										 if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0)
										 { uart0_OpenOrClose(Couple[i*2+1],1);
									 sta=1;	
											 ErrorTab[i]	=1;
                     SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
									  }
						     }
							 }
				 
						 } 				 
		 }
		
//------------------------------------------------------ 	//force   motor2	 open
	else	if(ParaList[(cRemoteControlmode+i)*2+1]==2)
 {	 
			
	 
	     //-----------------------  //1 open  2CLOSE
				if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1)
					{
					   if( GetAllError(Couple[i*2+1])==0)   //1 ok
						 {  ClrSwitchTimer(Couple[i*2]);
							 if(GetTimerOnStatus(Couple[i*2+1]-1)==0)
								 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
						  if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)
                  { uart0_OpenOrClose(Couple[i*2],0);
                   sta=1;
									ErrorTab[i]	=1;}	 
						 } 
						else  //1 error
						{    GetAllError(Couple[i*2+1]);
               	if( GetAllError(Couple[i*2])==0)   //2 ok
						       { ClrSwitchTimer(Couple[i*2+1]);
										 uart0_OpenOrClose(Couple[i*2+1],0);  
										 uart0_OpenOrClose(Couple[i*2],1);
										 sta=1;	
										 ErrorTab[i]	=1;
                     SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
									 }
						}
					}	
			 else  // 1close
			 {
				   if( GetAllError(Couple[i*2+1])==0)   //1 ok
						 {  ClrSwitchTimer(Couple[i*2]);
							 if(GetTimerOnStatus(Couple[i*2+1]-1)==0)
								 SetSwitchMiniter(Couple[i*2+1]-1,ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3+2)*2+1]);
						  if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)
                   uart0_OpenOrClose(Couple[i*2],0);
							uart0_OpenOrClose(Couple[i*2+1],1);
							sta=1;
ErrorTab[i]	=1;							
						 } 
						else  //1 error
						{    GetAllError(Couple[i*2+1]);
               	if( GetAllError(Couple[i*2])==0)   //2 ok
						       { ClrSwitchTimer(Couple[i*2+1]);
										 uart0_OpenOrClose(Couple[i*2+1],0);
										 sta=1;	
										 ErrorTab[i]	=1;
										 if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)
										 { uart0_OpenOrClose(Couple[i*2],1);
										 sta=1;
ErrorTab[i]	=1;											 
                     SetSwitchMiniter(Couple[i*2]-1,ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2]*256+ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3+2)*2+1]);
									  }
						     }
							 }
				 
						 } 				 
		 }
		
//------------------------------------------------------ 		//force   motor1	and motor 2	  open
		 else	if(ParaList[(cRemoteControlmode+i)*2+1]==3) 
			
	    {
   
				//-----------------------  //1 close
				if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)
					{
					   if( GetAllError(Couple[i*2])==0)   //1 ok
						   {  uart0_OpenOrClose(Couple[i*2],1); 
						     sta=1; 
ErrorTab[i]	=1;							 }	
					 }
     
			
    //-----------------------  //  2CLOSE
				if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0)
					{
					   if( GetAllError(Couple[i*2+1])==0)   //1 ok
						  {   uart0_OpenOrClose(Couple[i*2+1],1); sta=1;ErrorTab[i]	=1;	}
					 }
      
			
			}
//------------------------------------------------------ 		 //force   motor1	and motor 2	 close
		 else	if(ParaList[(cRemoteControlmode+i)*2+1]==4) 
			
	    {  
	     //-----------------------  //1 close
				if(GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)
					{
					   
						     uart0_OpenOrClose(Couple[i*2],0);
						     sta=1;	 
						ErrorTab[i]	=1;
					 }
      
				if(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1)
					{
					  
						     uart0_OpenOrClose(Couple[i*2+1],0); 
						     sta=1;	
						ErrorTab[i]	=1;
					 }
   
			
			
		 }			
//------------------------------------------------			
		 
	 }	   //forzen
		 } //alive
	 }  //switch
 }  //for
	
	if(sta==1)
	{  
		for(i=0;i<5;i++)
		 {if(ErrorTab[i]==1)
		    { uart0_UpdateOpenOrClose(Couple[i*2]);
	        uart0_UpdateOpenOrClose(Couple[i*2+1]); 
					SetSwitchMiniter(10,2);//20s
		    }
	    }
		}
else
		 SetStatusDelayFlag(); 
	}


/*---------------------------------------------------------------
//Function:void DealWithCheckDoubleMotorSwtich()
//Input:    None
//Output:   None
//Description: 运行过程中，检测是否由于人为操作或者参数设置导致机组出现不运行状态
		            如果机组都在运行，定时都启动，但是不会执行切换
		            如果有切换，有一个机组启动，则需要检查当前机组的定时开启，另外一个机组的定时关闭
		            如果
运行条件： 双机切换--》双机激活--》自动切换使能---》双机没没有处于化霜状态或者化霜不切换---》根据故障状态，保证正常的机组有一组处于工作状态----》设置状态回读命令---》设置20s静置时间                          
//----------------------------------------------------------------*/
void DealWithCheckSingleMotorSwtich()
{	
   uint8_t  i,  sta=0;	
	 uint8_t ErrorTab[10]={0,0,0,0,0,0,0,0,0,0};	

	 
//----------single motor---------------------------------------------------		 
		  for(i=0;i<10;i++)
    {  if((ParaList[cMotorType*2+1]==0x01)||(ParaList[cMotorType*2+1]==0x03))
       {			
			if((ParaList[(cLoraNodeOffset+i*3)*2+1]==0)&&((ParaList[(cLoraNodeAliveSet+i)*2+1]&0x01)==1)) 		//not switch  ,and alive
			{  if(ParaList[(cSingleRemoteControlmode+i)*2+1]==0)  //auto open
					
					{
					   if( GetAllError(i+1)==0)   //1 ok  由原来的 GetAllError(i)==0修改为 GetAllError(i+1)  20250312
						 { 
							 if(GetConSensorStatus(i+1,cRemoteOpen)==0) 
								{ uart0_OpenOrClose(i+1,1); 
								 sta=1; ErrorTab[i]	=1;
								}	
						 }							 
					 }
				else if(ParaList[(cSingleRemoteControlmode+i)*2+1]==1)
				{ if(GetConSensorStatus(i+1,cRemoteOpen)==1) 	
				        {   uart0_OpenOrClose(i+1,0); 
				        sta=1;	ErrorTab[i]	=1;
				       }  	
				}
			}
		}
	}
 if(sta==1)     //open
	{  
		for(i=0;i<10;i++)
		{ if(ErrorTab[i]	==1)
			 {
		    SetSwitchMiniter(10,2); //20s 
	      uart0_UpdateOpenOrClose(i+1);	
	     }	
		 }
   }		
	else
		SetStatusDelayFlag(); 
				    
	//------------end single motor-------------------------------		 
	 }		

#define 	  idle  0
#define 	 cSetDelay   1
#define 	 cWaitNormal 2
uint8_t   CompensateStatus[5]={idle ,idle ,idle ,idle ,idle };

#if  1

uint8_t CountCompenSateConditon(uint8_t  i)
   {uint8_t  CompensateConTimes=0;
		  int16_t  Sensor,Error,SetTemp,Sensor1,Error1,SetTemp1;
						                                                
								                                                  //传感器温度
												                                              Sensor=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*4]*256+uart0_OrigPara_List[i*cContorlRegLen*4+1]);
												                                              Sensor1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+1]);
												                                               //温差
																																			Error=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+12*2+1]);
																																			Error1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2+1]);
																																				//设定温度
																																			SetTemp=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+47*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+47*2+1]);
																																			SetTemp1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+47*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+47*2+1]);
																																			CompensateConTimes=0;
																																			if(GetConSensorStatus(Couple[i*2],cConSensorError)==0)
																																				{  if(Sensor>SetTemp+Error)
																																								CompensateConTimes++;
												
																																				}
																																			if(GetConSensorStatus(Couple[i*2+1],cConSensorError)==0)
																																					{  if(Sensor1>SetTemp1+Error1)
																																								CompensateConTimes++;
												
																																						}	
																																			return(CompensateConTimes);
}

void    CoolCompensateQuit(uint8_t  i)
 {
     uint8_t  quitornot=0;
    
		 
		 if(ParaList[cCoolCompensateEnable*2+1]==0) //compensate eanble	 
		 
		       quitornot++;
		 if(ModeBeforeStatus[i]!=ParaList[(cRemoteControlmode+i)*2+1]) //状态发生改变
                  {   ModeBeforeStatus[i]=ParaList[(cRemoteControlmode+i)*2+1];
										   quitornot++;
									}
		 
	if(quitornot!=0)	 
		 {CompensateStatus[i]=idle;
																										    uart0_Var_List[TempCompesentStatus*2+1]=0;
																										    uart0_Var_List[TempCompesentStatus*2+3]=0;
				}

			}

void  DealWithCoolCompensate()
{  uint8_t  i;
	 uint16_t  m,n,k,l;
	
	for(i=0;i<1;i++)
    {  
	     
			 switch(CompensateStatus[i])
                                  {
																	case idle :  
																	if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//一对机组切换功能使能
		                                  {	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //双机组都激活使用
                                            {			
	                                              if((GetConSensorStatus(Couple[i*2],cFrozenStatus)==0)&&(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==0))  // not  frozen
					                                       {  if(ParaList[cCoolCompensateEnable*2+1]==1) //compensate eanble
					                                              {  
						                                                
								                                                  if(CountCompenSateConditon(i)>0)
																																	   if(GetConSensorStatus(Couple[i*2],cRemoteOpen)!=GetConSensorStatus(Couple[i*2+1],cRemoteOpen))
                                                                            {  CompensateStatus[i]=cSetDelay;
																	                                             SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));   
																																						}
																												 }  
																											 }
																										 }
																									 }																										 
                                               break;
																		 case cSetDelay:
                                               if(CoolCompensateStatus(i)==1) //timer 1 over      																							 
                                                  {   uart0_Var_List[TempCompesentStatus*2+1]=1;
																									   if((GetConSensorStatus(Couple[i*2],cRemoteOpen)==1)&&(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==0))
																									     {   uart0_Var_List[TempCompesentStatus*2+3]=Couple[i*2+1];
																											     uart0_OpenOrClose(Couple[i*2+1],1); 
																										   }
													                        else  if((GetConSensorStatus(Couple[i*2],cRemoteOpen)==0)&&(GetConSensorStatus(Couple[i*2+1],cRemoteOpen)==1))
																										   {   uart0_Var_List[TempCompesentStatus*2+3]=Couple[i*2];
																									         uart0_OpenOrClose(Couple[i*2],1); 
																									      }
																										 
																										   CompensateStatus[i]=cWaitNormal;
																								   }
                                                else
																								  {  if(CountCompenSateConditon(i)==0)
																									     {CompensateStatus[i]=idle;
																										    uart0_Var_List[TempCompesentStatus*2+1]=0;
																										    uart0_Var_List[TempCompesentStatus*2+3]=0;
																											}
																										CoolCompensateQuit(i)	; 
																									}
																								 break;
                                    case cWaitNormal:
                                                  if(CountCompenSateConditon(i)==0) 
                                                     {CompensateStatus[i]=idle;
																										    uart0_Var_List[TempCompesentStatus*2+1]=0;
																										    uart0_Var_List[TempCompesentStatus*2+3]=0;
																											}
																							CoolCompensateQuit(i)	; 																		
																											
                                                   break;
																		
																	}
																}
	}																
																	
			#endif
			
/*---------------------------------------------------------------
//Function: void DealWithSwtich()
//Input:    None
//Output:   None
//Description: 处理运行过程中的定时切换和故障切换
//----------------------------------------------------------------*/
void DealWithSwtich()
{   uart0_VolCurrentOverLoadAlarm();
	 DealWithCoolCompensate();
 if(CompensateStatus[0]== idle) 
 {if(GetSwitchMiniterflag(10)==1) //timer 10 over	
       DealWithFrozenCheckSwtich();

		if(GetSwitchMiniterflag(10)==1) //timer 10 over			 
				 DealWithTimerSwtich();
		if(GetSwitchMiniterflag(10)==1) //timer 10 over	
	       DealWithCheckDoubleMotorSwtich();
		if(GetSwitchMiniterflag(10)==1) //timer 10 over	
		   DealWithCheckSingleMotorSwtich();
	     
	}      
    
}			
			