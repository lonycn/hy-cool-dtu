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
//Description: ½«»ú×éµÄÅä¶ÔÇé¿ö½øÐÐÕûºÏ£¬°´ÕÕË³ÐòÃ¿¶Ô½øÐÐÅÅÁÐ
//----------------------------------------------------------------
void GetCoupleList()
{
	uint8_t  i,k=0,List[10]={0,0,0,0,0,0,0,0,0,0}; 
   for(i=0;i<10;i++)
	   if(List[i]!=1)
			 {  
				  Couple[k++]=i+1;
				  Couple[k++]=ParaList[(cLoraNodeOffset+i*3+1)*2+1];
	        List[ParaList[(cLoraNodeOffset+i*3+1)*2+1]-1]=1;  //Ç°ÃæµÄ»ú×éÏÈ¿ª£¬ºóÃæµÄ»ú×é²»¿
      }
}	


//---------------------------------------------------------------
//Function: void GetCoupleList()
//Input:    None
//Output:   None
//Description: ½«»ú×éµÄÅä¶ÔÇé¿ö½øÐÐÕûºÏ£¬°´ÕÕË³ÐòÃ¿¶Ô½øÐÐÅÅÁÐ
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
	if(No1Time>No2Time) //24hour ÖÐ  1ºÅ»úÍíÓÚ2ºÅ»ú¿ªÆô£¬ÇÐ»»Ê±¼ä
	{
		if((currentTime>=No1Time)||(currentTime<=No2Time))  //ÂÖÖµÊ±¼ä
		{ 
			
          WorkFlag[i*2+1]=1;  //1ºÅ»ú×é¹¤×÷£¬¶þºÅ»úÉèÖÃ¶¨Ê±½áÊø±êÖ¾
			    WorkFlag[i*2]=0;  
				}
	 else
	 {
      WorkFlag[i*2+1]=0;  //2ºÅ»ú×é¹¤×÷£¬
			    WorkFlag[i*2]=1; //1ºÅ»úÉèÖÃ¶¨Ê±½áÊø±êÖ¾

	 }	
 }
else
{
	 if((currentTime>=No1Time)&&(currentTime<=No2Time))
		{  WorkFlag[i*2+1]=1;  //1ºÅ»ú×é¹¤×÷£¬¶þºÅ»úÉèÖÃ¶¨Ê±½áÊø±êÖ¾
			    WorkFlag[i*2]=0;  
		}
	 else
	 {
      WorkFlag[i*2+1]=0;  //2ºÅ»ú×é¹¤×÷£¬
			    WorkFlag[i*2]=1; //1ºÅ»úÉèÖÃ¶¨Ê±½áÊø±êÖ¾

	 }	
	
}
}
}

/*---------------------------------------
add  20230228 	½«ÂÖÖµÊ±¼äµ½µÄ²¿·Ö¸ü¸ÄÎª
-------------------------------------------*/	
uint8_t GetSwitchMiniterflag1(uint8_t i)
	 {
  	    	
  	return(WorkFlag[i]);
  }			

/*---------------------------------------------------------------
//Function: void DealWithTimerSwtich()
//Input:    None
//Output:   None
//Description: Ë«»úÇÐ»»Ä£Ê½----> Ò»¶Ô»ú×é¶¼ÉèÖÃÎªÇÐ»»----> ¶¼²»ÔÚ³ýËª×´Ì¬----->1ºÅ»ú×é¶¨Ê±µ½---> 2ºÅÎÞ¹ÊÕÏ£¬¹Ø±Õ1ºÅ---> 2ºÅÎª¹Ø±Õ×´Ì¬£¬¿ªÆô2ºÅ
                                                                                             --> 2ºÅ¹ÊÕÏ£¬¼ÌÐøÆô¶¯1ºÅ  
                                                                        ----->1ºÅ»ú×é¶¨Ê±µ½---> 1ºÅÎÞ¹ÊÕÏ£¬¹Ø±Õ2ºÅ---> 1ºÅÎª¹Ø±Õ×´Ì¬£¬¿ªÆô1ºÅ
                                                                                             --> 1ºÅ¹ÊÕÏ£¬¼ÌÐøÆô¶¯2ºÅ  
//----------------------------------------------------------------*/
void DealWithTimerSwtich()
{	
   uint8_t  i,sta;
	   uint8_t ErrorTab[5]={0,0,0,0,0};	
	sta=0;
   for(i=0;i<5;i++)
    {  CountWorkStatus(i) ; 
     if(ParaList[(cRemoteControlmode+i)*2+1]==0) //×Ô¶¯ÂÖÖµ
		 {
			if((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]!=0) && (ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]!=0)) //	ÇÐ»»¶¼´¦ÓÚ¼¤»î
        {			  
	     
//20240303					
					if( ((GetConSensorStatus(Couple[i*2],cFrozenStatus)==0)&&(GetConSensorStatus(Couple[i*2+1],cFrozenStatus)==0))   )//²»ÔÚ»¯ËªÖÐ
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
//Description: »¯Ëª¹ý³ÌÖÐÊÇ·ñÆô¶¯±¸»úÇÐ»»,Ö»Òª¹¤×÷»úÔÚ»¯Ëª²Ù×÷ÖÐ£¬Èç¹û±¸»úÃ»ÓÐÆô¶¯£¬ÔòÆô¶¯±¸»ú
ÔËÐÐÌõ¼þ£º Ë«»úÇÐ»»--¡·Ë«»ú¼¤»î--¡·»¯ËªÇÐ»»±¸»úÊ¹ÄÜ---¡·µ±Ç°¹¤×÷»ú»¯Ëª--¡·±¸»úÎÞ¹ÊÕÏ----¡·¿ªÆô±¸»ú----¡·ÉèÖÃ×´Ì¬»Ø¶ÁÃüÁî---¡·ÉèÖÃ20s¾²ÖÃÊ±¼ä
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
//Description: Àä²¹³¥¹¦ÄÜ
               Ë«»úÄ£Ê½---Ç¿1£¬Ç¿2£¬ÂÖÖµÄ£Ê½---Ä£Ê½ÓÐÇÐ»»£¬ÉèÖÃ¿ªÆôÀä²¹³¥Ê±¼ä
                                            ---Ã»ÓÐÇÐ»»£¬¼ÆËã¿ªÆôÀä²¹³¥Ê±¼äµ½----ÖÆÀäÎÂ¶ÈÊÇ·ñÔÚÉè¶¨ÎÈ¶¨Çø¼ä----²»ÔÚ----½«Ä£Ê½ÇÐ»»ÎªË«Ç¿Ä£Ê½
//----------------------------------------------------------------*/
#if   0
void  DealWithCoolCompensate()
{  uint8_t  i;
	 uint16_t  m,n,k,l;
	 int16_t  Sensor,Error,SetTemp,Sensor1,Error1,SetTemp1;
	for(i=0;i<5;i++)
    {  
	
//----------------------------------------			
  	  
   	if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//Ò»¶Ô»ú×éÇÐ»»¹¦ÄÜÊ¹ÄÜ
		{	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //Ë«»ú×é¶¼¼¤»îÊ¹ÓÃ
        {			
	       
						 if((ParaList[(cRemoteControlmode+i)*2+1]==0)||(ParaList[(cRemoteControlmode+i)*2+1]==1)||(ParaList[(cRemoteControlmode+i)*2+1]==2))
						    { //Ç¿1£¬Ç¿2£¬ÂÖÖµÄ£Ê½
									if(ModeBeforeStatus[i]!=ParaList[(cRemoteControlmode+i)*2+1]) //×´Ì¬·¢Éú¸Ä±ä
                  {   ModeBeforeStatus[i]=ParaList[(cRemoteControlmode+i)*2+1];
										  SetCoolCompensateTimer(i,(ParaList[cCoolCompensate*2]*256+ParaList[cCoolCompensate*2+1]));
										NormalFlag=0;
										uart0_Var_List[TempCompesentStatus*2+1]=0;
										uart0_Var_List[TempCompesentStatus*2+3]=0;
									}
								 else  
								 {   
                        //´«¸ÐÆ÷ÎÂ¶È
												 Sensor=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*4]*256+uart0_OrigPara_List[i*cContorlRegLen*4+1]);
												 Sensor1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+1]);
												 //ÎÂ²î
												 Error=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+12*2+1]);
												  Error1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2+1]);
												 //Éè¶¨ÎÂ¶È
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
//Description: ÔËÐÐ¹ý³ÌÖÐ£¬¼ì²âÊÇ·ñÓÉÓÚÈËÎª²Ù×÷»òÕß²ÎÊýÉèÖÃµ¼ÖÂ»ú×é³öÏÖ²»ÔËÐÐ×´Ì¬
		            Èç¹û»ú×é¶¼ÔÚÔËÐÐ£¬¶¨Ê±¶¼Æô¶¯£¬µ«ÊÇ²»»áÖ´ÐÐÇÐ»»
		            Èç¹ûÓÐÇÐ»»£¬ÓÐÒ»¸ö»ú×éÆô¶¯£¬ÔòÐèÒª¼ì²éµ±Ç°»ú×éµÄ¶¨Ê±¿ªÆô£¬ÁíÍâÒ»¸ö»ú×éµÄ¶¨Ê±¹Ø±Õ
		            Èç¹û
ÔËÐÐÌõ¼þ£º Ë«»úÇÐ»»--¡·Ë«»ú¼¤»î--¡·×Ô¶¯ÇÐ»»Ê¹ÄÜ---¡·Ë«»úÃ»Ã»ÓÐ´¦ÓÚ»¯Ëª×´Ì¬»òÕß»¯Ëª²»ÇÐ»»---¡·¸ù¾Ý¹ÊÕÏ×´Ì¬£¬±£Ö¤Õý³£µÄ»ú×éÓÐÒ»×é´¦ÓÚ¹¤×÷×´Ì¬----¡·ÉèÖÃ×´Ì¬»Ø¶ÁÃüÁî---¡·ÉèÖÃ20s¾²ÖÃÊ±¼ä                          
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
//Description: ÔËÐÐ¹ý³ÌÖÐ£¬¼ì²âÊÇ·ñÓÉÓÚÈËÎª²Ù×÷»òÕß²ÎÊýÉèÖÃµ¼ÖÂ»ú×é³öÏÖ²»ÔËÐÐ×´Ì¬
		            Èç¹û»ú×é¶¼ÔÚÔËÐÐ£¬¶¨Ê±¶¼Æô¶¯£¬µ«ÊÇ²»»áÖ´ÐÐÇÐ»»
		            Èç¹ûÓÐÇÐ»»£¬ÓÐÒ»¸ö»ú×éÆô¶¯£¬ÔòÐèÒª¼ì²éµ±Ç°»ú×éµÄ¶¨Ê±¿ªÆô£¬ÁíÍâÒ»¸ö»ú×éµÄ¶¨Ê±¹Ø±Õ
		            Èç¹û
ÔËÐÐÌõ¼þ£º Ë«»úÇÐ»»--¡·Ë«»ú¼¤»î--¡·×Ô¶¯ÇÐ»»Ê¹ÄÜ---¡·Ë«»úÃ»Ã»ÓÐ´¦ÓÚ»¯Ëª×´Ì¬»òÕß»¯Ëª²»ÇÐ»»---¡·¸ù¾Ý¹ÊÕÏ×´Ì¬£¬±£Ö¤Õý³£µÄ»ú×éÓÐÒ»×é´¦ÓÚ¹¤×÷×´Ì¬----¡·ÉèÖÃ×´Ì¬»Ø¶ÁÃüÁî---¡·ÉèÖÃ20s¾²ÖÃÊ±¼ä                          
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
					   if( GetAllError(i+1)==0)   //1 ok  ÓÉÔ­À´µÄ GetAllError(i)==0ÐÞ¸ÄÎª GetAllError(i+1)  20250312
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
						                                                
								                                                  //´«¸ÐÆ÷ÎÂ¶È
												                                              Sensor=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*4]*256+uart0_OrigPara_List[i*cContorlRegLen*4+1]);
												                                              Sensor1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+1]);
												                                               //ÎÂ²î
																																			Error=(int16_t)(uart0_OrigPara_List[i*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[i*cContorlRegLen*2+12*2+1]);
																																			Error1=(int16_t)(uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2]*256+uart0_OrigPara_List[(i+1)*cContorlRegLen*2+12*2+1]);
																																				//Éè¶¨ÎÂ¶È
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
		 if(ModeBeforeStatus[i]!=ParaList[(cRemoteControlmode+i)*2+1]) //×´Ì¬·¢Éú¸Ä±ä
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
																	if(((ParaList[(cLoraNodeOffset+(Couple[i*2]-1)*3)*2+1]&0x01)==1)&&((ParaList[(cLoraNodeOffset+(Couple[i*2+1]-1)*3)*2+1]&0x01)==1) )//Ò»¶Ô»ú×éÇÐ»»¹¦ÄÜÊ¹ÄÜ
		                                  {	if((ParaList[(cLoraNodeAliveSet+(Couple[i*2]-1))*2+1]!=0) && (ParaList[(cLoraNodeAliveSet+(Couple[i*2+1]-1))*2+1]!=0))  //Ë«»ú×é¶¼¼¤»îÊ¹ÓÃ
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
//Description: ´¦ÀíÔËÐÐ¹ý³ÌÖÐµÄ¶¨Ê±ÇÐ»»ºÍ¹ÊÕÏÇÐ»»
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
			