#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
//extern  Hisrory_Struct   History;

#define  cAlarmSta        0
#define  cPreAlarmSta     1  	 
#define  cNormalSta       2
#define  cNotAlarmSta     3
#define  cSensorWrongSta  4 

#define    u8  unsigned char
#define    u16  uint16_t
void SetAlarmLevel();
void DealWith_Alarm_Temp_PreAlarm();
void DealWith_Alarm_Humi_PreAlarm();
u8  DealWith_Temp_Up_Alarm();
u8  DealWith_Temp_Up_PreAlarm();
u8  DealWith_Temp_LowAlarm();
u8  DealWith_Temp_LowPreAlarm();
u8  DealWith_Humi_Up_Alarm();
u8  DealWith_Humi_Up_PreAlarm();
u8  DealWith_Humi_LowAlarm();
u8  DealWith_Humi_LowPreAlarm();

//  History.AlarmLevel  7      6    5    4   3      2    1   0    
//                     RHHA  RHLA  THA  TLA RHHP  RHLP  THP TLP
/*------------------------------------------------------------------
Function: void SetAlarmLevel()
Input:
Output:
Description:  根据比较值和激活状态设置当前的即时报警状态
               满足以下条件中一个则进行计算: 1: 报警设置为一直有效
                                             2: 系统处于启动状态
               温湿度报警没有激活:温湿度值为-3000
Var:               History.AlarmLevel                                                                     
---------------------------------------------------------------------*/  
void SetAlarmLevel()
 { 	 int16_t temp,temp1;
         u8  testbit,testbit1;
      //add 2016-4-29 0:16
        
      testbit=AlarmAlwaysValid();  //是否一直报警
      testbit1=CheckStartStopStatus(); //是否在启动状态
       if((testbit==1)||(testbit1==1))  //add 2019-04-10 18:04
        {
         //温湿度都没激活
         if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00)&& ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0x00))
            {
                History.AlarmLevel=0x00;
                Alarm_Initial();   
               //add 2019-04-26 23:12
  	        VarList[cTemp*2]=(-cInValidTemp)/256;
                VarList[cTemp*2+1]=(-cInValidTemp)%256;
                VarList[cHumi*2]=(-cInValidTemp)/256;
                VarList[cHumi*2+1]=(-cInValidTemp)%256;
               //end of add 2019-04-26 23:12       
             } 
          
              //温湿度激活或者部分激活    
              else  
              {  
         
     //-----------------------------------------------------------
     //if not  temp display is invalid,dont check temp overtemp or below temp
     //-----------------------------------------------------------
                //温度没激活 
                  if( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]!=0)
                    {
            	       
  	               TNormalHi();
                       TNormalLow();
  	               VarList[cTemp*2]=(-cInValidTemp)/256;
                       VarList[cTemp*2+1]=(-cInValidTemp)%256;
                     }
                   else //T激活
                     {
                     	DealWith_Alarm_Temp_PreAlarm();
                     	
                      }	  
                 //湿度没激活
  	       if( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]!=0)
                 {   RHNormalHi();
                     RHNormalLow();
                     VarList[cHumi*2]=(-cInValidTemp)/256;
                     VarList[cHumi*2+1]=(-cInValidTemp)%256;
               //end of add 2019-04-26 23:12 
           	  }
  		else
                   DealWith_Alarm_Humi_PreAlarm();
         }
     }
   else
     { TNormalHi();
       TNormalLow();
       RHNormalHi();
       RHNormalLow();
     }	
  }   	

/*------------------------------------------------------------------
Function: void DealWith_Alarm_PreAlarm()
Input:
Output:
Description:  处理温度和湿度的预警/报警                                     
---------------------------------------------------------------------*/   
void DealWith_Alarm_Temp_PreAlarm()
  {          
    if( DealWith_Temp_Up_Alarm()==cNotAlarmSta)
     	 DealWith_Temp_Up_PreAlarm();
     if(DealWith_Temp_LowAlarm()==cNotAlarmSta)
          DealWith_Temp_LowPreAlarm();
   }       
void DealWith_Alarm_Humi_PreAlarm()
  {                      
     if(DealWith_Humi_Up_Alarm() ==cNotAlarmSta)
        DealWith_Humi_Up_PreAlarm();
     if(DealWith_Humi_LowAlarm() ==cNotAlarmSta)
        DealWith_Humi_LowPreAlarm(); 
   }                    	 
/*------------------------------------------------------------------
Function: u8  DealWith_Temp_UpLimit()
Input:
Output:
Description:  比较是否出现了超温度报警上限
              如果---大于----报警上限, 报警上限激活, 设置超高温报警
                                       报警上限非激活,预警上限激活,设置超高温预警 
                                       都不激活,高温状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	 
u8  DealWith_Temp_Up_Alarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;    
  	       //计算温度报警上限
  	      temp1=(int16_t)(ParaList[(cTempUpLimit03D-cParaActual)*2]*256+ParaList[(cTempUpLimit03D-cParaActual)*2+1]); 
  	    //----check over alarm
  	     if(temp>temp1)  
  	       {  //温度上限激活
  	       	  if( ParaList[(cTempUpLimitValid03D-cParaActual)*2]*256+ParaList[(cTempUpLimitValid03D-cParaActual)*2+1]==0) 
  	             {
  	               
  	               TAlarmHi();
  	               return(cAlarmSta);
  	              }
  	           else //报警上限非激活,检查预警上限激活情况
  	            {  
  	               if( ParaList[(cTempPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreUpLimitValid03D-cParaActual)*2+1]==0) 
  	                     { 
  	                       TPreAlarmHi();
  	                       return(cPreAlarmSta);
  	                     }
  	               else
  	                   {   TNormalHi() ;
  	                       return(cNormalSta);
  	                   }      
  	             }
  	                
  	       }
  	      else
  	        return(cNotAlarmSta); 
  	    }
  	   History.TempCount++;
  	   return(cSensorWrongSta);
  	}   
  	
  	
/*------------------------------------------------------------------
Function: u8  DealWith_Temp_Up_PreAlarm()
Input:
Output:
Description:  比较是否出现了超温度预警上限
              如果---大于----预警上限,预警上限激活,设置超高温预警 
                                             不激活,高温状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	 
u8  DealWith_Temp_Up_PreAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;    
  	       //计算温度预警上限
  	      temp1=(int16_t)(ParaList[(cTempPreUpLimit03D-cParaActual)*2]*256+ParaList[(cTempPreUpLimit03D-cParaActual)*2+1]); 
  	    //----check over alarm
  	     if(temp>temp1)  
  	       { //检查预警上限激活情况
  	            
  	               if( ParaList[(cTempPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreUpLimitValid03D-cParaActual)*2+1]==0) 
  	                     { 
  	                       TPreAlarmHi();
  	                       return(cPreAlarmSta);
  	                     }
  	               else
  	                   {   TNormalHi() ;
  	                       return(cNormalSta);
  	                   }      
  	             
  	            }
  	                
  	      
  	      else
  	         {   TNormalHi() ;
  	              return(cNormalSta);
  	          }     
  	    }
  	   else 
  	    { 
  	   History.TempCount++;
  	   return(cSensorWrongSta);
  	  }     	
  	}
  	

  	  	          
//------------temp  low   
/*------------------------------------------------------------------
Function: u8  DealWith_Temp_LowAlarm()
Input:
Output:
Description:  比较是否出现了超温度报警下限
              如果---小于----报警下限, 报警下限激活, 设置超低温报警
                                       报警下限非激活,预警下限激活,设置超低温预警 
                                       都不激活,低温状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  
u8  DealWith_Temp_LowAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;		
  	 //end of add 2015-09-25 15:56
  	 if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;  
  	    temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
  	    temp1=(int16_t)(ParaList[(cTempDownLimit03D-cParaActual)*2]*256+ParaList[(cTempDownLimit03D-cParaActual)*2+1]);   
  	     if(temp<temp1)
  	      { 
  	      	 if( ParaList[(cTempDownLimitValid03D-cParaActual)*2]*256+ParaList[(cTempDownLimitValid03D-cParaActual)*2+1]==0) 
  	          {  TAlarmLow();
  	             return(cAlarmSta);       
  	           }
  	         else
  	          {
  	           
  	             if( ParaList[(cTempPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreDownLimitValid03D-cParaActual)*2+1]==0)   
  	               {       TPreAlarmLow();
  	                       return(cPreAlarmSta);       
  	          
  	               }  
  	             else
  	               {
  	               	  TNormalLow(); 
  	                 return(cNormalSta);       
  	                }          
  	           //end of add 2019-04-07 11:20        
  	           }
		 }
		else
		return(cNotAlarmSta);  	
		}
	     else
	     {  History.TempCount++;
	         return(cSensorWrongSta); 
	      }
	  }    
	      		 
	


/*------------------------------------------------------------------
Function: u8  u8  DealWith_Temp_LowPreAlarm()
Input:
Output:
Description:  比较是否出现了超温度预警下限
              如果---小于----预警下限,,预警下限激活,设置超低温预警 
                                       不激活,低温状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  
u8  DealWith_Temp_LowPreAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;		
  	 //end of add 2015-09-25 15:56
  	 if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;  
  	    temp=(int16_t)(VarList[cTemp*2]*256+VarList[cTemp*2+1]);
  	    temp1=(int16_t)(ParaList[(cTempPreDownLimit03D-cParaActual)*2]*256+ParaList[(cTempPreDownLimit03D-cParaActual)*2+1]);   
  	     if(temp<temp1)
  	      { 
  	             if( ParaList[(cTempPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cTempPreDownLimitValid03D-cParaActual)*2+1]==0)   
  	               {       TPreAlarmLow();
  	                       return(cPreAlarmSta);       
  	          
  	               }  
  	             else
  	               {
  	               	  TNormalLow(); 
  	                 return(cNormalSta);       
  	                }          
  	           //end of add 2019-04-07 11:20        
  	           }
		
		else
		{ 
			TNormalLow(); 
			return(cNotAlarmSta);  

		}			
		}
	     else
	     {  History.TempCount++;
	         return(cSensorWrongSta); 
	      }
	  }    	
	


/*------------------------------------------------------------------
Function: u8  DealWith_Humi_Up_Alarm()
Input:
Output:
Description:  比较是否出现了超湿度报警上限
              如果---大于----报警上限, 报警上限激活, 设置超高湿报警
                                       报警上限非激活,预警上限激活,设置超高湿预警 
                                       都不激活,高湿状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	 
u8  DealWith_Humi_Up_Alarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;    
  	       //计算温度报警上限
  	      temp1=(int16_t)(ParaList[(cHumiUpLimit03D-cParaActual)*2]*256+ParaList[(cHumiUpLimit03D-cParaActual)*2+1]); 
  	    //----check over alarm
  	     if(temp>temp1)  
  	       {  //温度上限激活
  	       	  if( ParaList[(cHumiUpLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiUpLimitValid03D-cParaActual)*2+1]==0) 
  	             {
  	               
  	               RHAlarmHi();
  	               return(cAlarmSta);
  	              }
  	           else //报警上限非激活,检查预警上限激活情况
  	            {  
  	               if( ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2+1]==0) 
  	                     { 
  	                       RHPreAlarmHi();
  	                       return(cPreAlarmSta);
  	                     }
  	               else
  	                   {   RHNormalHi() ;
  	                       return(cNormalSta);
  	                   }      
  	             }
  	                
  	       }
  	      else
  	        return(cNotAlarmSta); 
  	    }
  	   else
  	     {   RHNormalHi() ;
  	                       return(cNormalSta);
  	                   }  
  	}   
  	  	  
  	  
 
/*------------------------------------------------------------------
Function: u8  DealWith_Humi_Up_PreAlarm()
Input:
Output:
Description:  比较是否出现了超湿度预警上限
              如果---大于----预警上限, 预警上限激活,设置超高湿预警 
                                      不激活,高湿状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	 
u8  DealWith_Humi_Up_PreAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;	
            if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;    
  	       //计算温度报警上限
  	      temp1=(int16_t)(ParaList[(cHumiPreUpLimit03D-cParaActual)*2]*256+ParaList[(cHumiPreUpLimit03D-cParaActual)*2+1]); 
  	    //----check over alarm
  	     if(temp>temp1)  
  	       {  //,检查预警上限激活情况
  	             
  	               if( ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreUpLimitValid03D-cParaActual)*2+1]==0) 
  	                     { 
  	                       RHPreAlarmHi();
  	                       return(cPreAlarmSta);
  	                     }
  	               else
  	                   {   RHNormalHi() ;
  	                       return(cNormalSta);
  	                   }      
  	             }
					 else
						  
  	         {   RHNormalHi() ;
  	                       return(cNormalSta);
  	                   }      
  	    }
  	  else
  	   {
  	   	  
  	    History.TempCount++;
  	   return(cSensorWrongSta);
  	  } 
  	   
  	}   
  	  	   
 
 
 
 
/*------------------------------------------------------------------
Function: u8  DealWith_Humi_LowAlarm()
Input:
Output:
Description:  比较是否出现了超湿度报警下限
              如果---小于----报警下限, 报警下限激活, 设置超低湿报警
                                       报警下限非激活,预警下限激活,设置超低湿预警 
                                       都不激活,低湿状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	   

u8  DealWith_Humi_LowAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;		
  	 //end of add 2015-09-25 15:56
  	 if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;  
  	      temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
  	      temp1=(int16_t)(ParaList[(cHumiDownLimit03D-cParaActual)*2]*256+ParaList[(cHumiDownLimit03D-cParaActual)*2+1]);   
  	     if(temp<temp1)  
  	     {  //add 2016-4-29 0:28
  	     	if( ParaList[(cHumiDownLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiDownLimitValid03D-cParaActual)*2+1]==0)
  	          {  RHAlarmLow();
  	             return(cAlarmSta);       
  	           }
  	         else
  	          {
  	           
  	             if( ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2+1]==0)    
  	               {       RHPreAlarmLow();
  	                       return(cPreAlarmSta);       
  	          
  	               }  
  	             else
  	               {
  	               	  RHNormalLow(); 
  	                 return(cNormalSta);       
  	                }          
  	           //end of add 2019-04-07 11:20        
  	           }
		 }
		else
		return(cNotAlarmSta);  	
		}
	     else
	     {  History.TempCount++;
	         return(cSensorWrongSta); 
	      }
	  }    
	      		 
	


/*------------------------------------------------------------------
Function:u8  DealWith_Humi_LowPreAlarm()
Input:
Output:
Description:  比较是否出现了超湿度预警下限
              如果---小于----预警下限, 预警下限激活,设置超低湿预警 
                                       不激活,低湿状态正常
return:  0:设置为报警  1:设置为预警  2:设置为正常 3:不为报警  4:设置为探头故障                                       
---------------------------------------------------------------------*/  	   

u8  DealWith_Humi_LowPreAlarm()
    { int16_t temp,temp1;
    	     temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
//---------------temp  hi         
	     temp1=-cInValidTemp;		
  	 //end of add 2015-09-25 15:56
  	 if(temp!=temp1)//2015-05-30 8:36
             { History.TempCount=0;  
  	      temp=(int16_t)(VarList[cHumi*2]*256+VarList[cHumi*2+1]);
  	      temp1=(int16_t)(ParaList[(cHumiPreDownLimit03D-cParaActual)*2]*256+ParaList[(cHumiPreDownLimit03D-cParaActual)*2+1]);   
  	     if(temp<temp1)  
  	     {  //add 2016-4-29 0:28
  	     	
  	           
  	             if( ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2]*256+ParaList[(cHumiPreDownLimitValid03D-cParaActual)*2+1]==0)    
  	               {       RHPreAlarmLow();
  	                       return(cPreAlarmSta);       
  	          
  	               }  
  	             else
  	               {
  	               	  RHNormalLow(); 
  	                 return(cNormalSta);       
  	                }          
  	           //end of add 2019-04-07 11:20        
  	           }
  	        else
  	         {
  	              RHNormalLow(); 
  	                 return(cNormalSta); 
  	          }        	
  	     }    	
  	   
	     else
	     {  History.TempCount++;
	         return(cSensorWrongSta); 
	      }
	  }    



