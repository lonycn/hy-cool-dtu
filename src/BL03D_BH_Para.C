#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"
#include <math.h>

unsigned char GetInitialFlag();
void  InitialPara();
void  BL03D_InitialFlag();
void BL03D_BH_ParaInitial();
void BL03D_Base_ParaInitial();
void BL03D_Base_Update();
void BL03D_Update();
void BL03D_Thermoberg_ParaInitial();

//--------------------------------------------------------------------------------------------
//Function: unsigned char GetInitialFlag()
//Input:   None
//Output:   1:已经初始化   0:没有初始化 
//Description：初始化参数
//--------------------------------------------------------------------------------------------
unsigned char GetInitialFlag()
  {
    if((ParaList[cInitialFlag*2]==cReset1)&&(ParaList[cInitialFlag*2+1]==cReset2))
         return(1);
    else
        return(0);      	
  }
//--------------------------------------------------------------------------------------------
//Function: void  InitialPara()
//Input:   None
//Output:   None 
//Description：初始化参数
//--------------------------------------------------------------------------------------------
void  InitialPara()
  {
      BL03D_InitialFlag();        //初始化标志参数放入到paralist中
      BL03D_Base_ParaInitial();	 //参数放入paralist中,根据是否修改device确定初始化device

#ifdef  _BL03D
    #ifdef    __Thermoberg03d 
       BL03D_Thermoberg_ParaInitial();     
    #else
      BL03D_BH_ParaInitial();   //参数放入到paralist中.   page/record 存入flash中
  #endif
#endif  
      BL02D_Special_Initial();   //参数放入到paralist中, 同时放入ram变量区
     
      
      ProgramEepromByte();      // 1:paralist-->cpu flash 2;base paralist-->ram  3: 02d paralsit-->ram
     // ReadEepromByte();         //读4次,如果没有初始化标识,进行初始化 1:  cpu flash-->paralist   2: 02d paralsit-->ram  
   
   Key_Alarm_inital();  //20220822
		//3:base paralist-->ram  3:读出page/record-->para-->ram
  }	

//--------------------------------------------------------------------------------------------
//Function: void  BL03D_InitialFlag()
//Input:   None
//Output:   None 
//Description：设置初始化标志
//--------------------------------------------------------------------------------------------                  
void  BL03D_InitialFlag()
        {
            ParaList[cInitialFlag*2]=cReset1;
            ParaList[cInitialFlag*2+1]=cReset2;			
         } 	
	    	
void BL03D_Thermoberg_ParaInitial()
 {
   	
     	uint8_t  i;   
 uint8_t  BL03D_Thermoberg_Initial[]={    	   
0,1,//#define cSaveDataFlag03D                    2010					//60
0,120,//#define cNormalHistoryRecInt               2011    
0,120,//#define cAlarmHistoryRecInt               2012
0,0,//#define cTempError03D                    2013
0,0,//#define cHumiError03D                   2014
0,0,     //#define  cSensorMode                   2015
0,80,//#define cTempPreUpLimit03D                   2016             
0,20,//#define cTempPreDownLimit03D                  2017          
0,0,//#define cTempPreUpLimitAlarmDelay03D          2018          
0,0,//#define cTempPreDownLimitAlarmDelay03D        2019
0,200,//#define cTempUpLimit03D                   2020                
0,10,//#define cTempDownLimit03D                 2021          
0,0,//#define cTempUpLimitAlarmDelay03D         2022     
0,0,//#define cTempDownLimitAlarmDelay03D       2023  
750/256,750%256,//#define cHumiPreUpLimit03D                   2024   
350/256,350%256,//#define cHumiPreDownLimit03D                 2025     
0,0,//#define cHumiPreUpLimitAlarmDelay03D         2026         
0,0,//#define cHumiPreDownLimitAlarmDelay03D       2027
800/256,800%256,//#define cHumiUpLimit03D                   2028   
300/256,300%256,//#define cHumiDownLimit03D                 2029          
0,0,//#define cHumiUpLimitAlarmDelay03D         2030         
0,0,//#define cHumiDownLimitAlarmDelay03D      2031  
0,0,//#define cAlarmOutControl03D              2032
0,50,//#define cPowerAlarmValue03D              2033         
0,0,//#define cTestMode03D                     2034 



0,1,//#define cSystemAlarmMode           2035
0,0,//#define cTempValid03D                     2036
0,0,//#define cHumiValid03D                   2037
0,0,//#define cTempPreUpLimitValid03D              2038
0,0,//#define cTempPreDownLimitValid03D            2039
0,0,//#define cTempUpLimitValid03D               2040 
0,0,//#define cTempDownLimitValid03D             2041
0,0,//#define cHumiPreUpLimitValid03D                 2042  //  (cTempDownLimitAlarmDelay03D+1)    
0,0,//#define cHumiPreDownLimitValid03D               2043
0,0,//#define cHumiUpLimitValid03D                   2044 
0,0,//#define cHumiDownLimitValid03D                2045
0,0,//#define cPowerOffAlarmValid                   2046
0,0,//#define cSensorWrongAlarmValid              2047
0,0,//#define cPowerLowAlarmValid                 2048
0,0,//#define cBellAlarmValid                     2049
0,0,//#define cStartStopAlarmValid               2050
0,3,//#define  cWirelessTransMode            2051
0,3,//#define  cWirelessTransBaud            2052
0,3,//#define  cWirelessTransChan           2053
0,3,//#define  cWirelessTransPower          2054
0,60,//#define  cWirelessTransWakeupTime     2055
}  ; 	   
     	
        for(i=0;i<47*2;i++)
          ParaList[(cSaveDataFlag03D-cParaActual)*2+i]=BL03D_Thermoberg_Initial[i];   
        #ifdef _BL03D    
           InitialPagePara();
        #endif   
        BL03D_Update();   	
 	
 	
 	
  }
//--------------------------------------------------------------------------------------------
//Function: void BL03D_BH_ParaInitial()
//Input:   None
//Output:   None 
//Description：bh 03D参数初始化
//--------------------------------------------------------------------------------------------
void BL03D_BH_ParaInitial()
     { 	
     	uint8_t  i;   
 uint8_t  BL03D_Initial[]={    	   
0,1,//#define cSaveDataFlag03D                    2010					//60
0,60,//#define cNormalHistoryRecInt               2011    
0,60,//#define cAlarmHistoryRecInt               2012
0,0,//#define cTempValid03D                     2013
0,0,//#define cTempError03D                    2014
0,0,//#define cHumiValid03D                   2015
0,0,//#define cHumiError03D                   2016              
0,1,//#define cTempPreUpLimitValid03D              2017
0,1,//#define cTempPreDownLimitValid03D            2018
0,80,//#define cTempPreUpLimit03D                   2019             
0,20,//#define cTempPreDownLimit03D                  2020          
0,0,//#define cTempPreUpLimitAlarmDelay03D          2021          
0,0,//#define cTempPreDownLimitAlarmDelay03D        2022
0,1,//#define cTempUpLimitValid03D               2023 
0,1,//#define cTempDownLimitValid03D             2024
0,200,//#define cTempUpLimit03D                   2025                
0,10,//#define cTempDownLimit03D                 2026           
0,0,//#define cTempUpLimitAlarmDelay03D         2027     
0,0,//#define cTempDownLimitAlarmDelay03D       2028     
0,1,//#define cHumiPreUpLimitValid03D                 2029  //  (cTempDownLimitAlarmDelay03D+1)    
0,1,//#define cHumiPreDownLimitValid03D               2030
750/256,750%256,//#define cHumiPreUpLimit03D                   2031   
350/256,350%256,//#define cHumiPreDownLimit03D                 2032     
0,0,//#define cHumiPreUpLimitAlarmDelay03D         2033         
0,0,//#define cHumiPreDownLimitAlarmDelay03D       2034
0,1,//#define cHumiUpLimitValid03D                   2035 
0,1,//#define cHumiDownLimitValid03D                2036
800/256,800%256,//#define cHumiUpLimit03D                   2037   
300/256,300%256,//#define cHumiDownLimit03D                 2038          
0,0,//#define cHumiUpLimitAlarmDelay03D         2039         
0,0,//#define cHumiDownLimitAlarmDelay03D      2040   
0,0,//#define cAlarmOutControl03D              2041 
0,50,//#define cPowerAlarmValue03D              2042          
0,0,//#define cTestMode03D                     2043  
0,0,//#define cSystemAlarmMode           2044
0,1,//#define cPowerOffAlarmValid                   2045
0,1,//#define cSensorWrongAlarmValid              2046
0,1,//#define cPowerLowAlarmValid                 2047
0,1,//#define cBellAlarmValid                     2048
0,1,//#define cStartStopAlarmValid               2049
0,3,//#define  cWirelessTransMode            2050
0,3,//#define  cWirelessTransBaud            2051
0,3,//#define  cWirelessTransChan           2052
0,3,//#define  cWirelessTransPower          2053
0,60,//#define  cWirelessTransWakeupTime     2054
}  ; 	   
     	
        for(i=0;i<47*2;i++)
          ParaList[(cSaveDataFlag03D-cParaActual)*2+i]=BL03D_Initial[i];   
        #ifdef _BL03D    
           InitialPagePara();
        #endif   
        BL03D_Update();   
          
    }           
                 

void BL03D_Update()
     { 			                 
       
    History.MiniterDelaySet[0]=ParaList[(cTempDownLimitAlarmDelay03D-cParaActual)*2+1];     //温度-报警下限 
    History.MiniterDelaySet[1]=ParaList[(cTempPreDownLimitAlarmDelay03D-cParaActual)*2+1] ; //温度-预警下限 
    History.MiniterDelaySet[2]=ParaList[(cTempUpLimitAlarmDelay03D -cParaActual)*2+1]  ;    //温度-报警上限 
    History.MiniterDelaySet[3]=ParaList[(cTempPreUpLimitAlarmDelay03D-cParaActual)*2+1];    //温度-预警上限 
    History.MiniterDelaySet[4]=ParaList[(cHumiDownLimitAlarmDelay03D-cParaActual)*2+1] ;    //湿度-报警下限 
    History.MiniterDelaySet[5]=ParaList[(cHumiPreDownLimitAlarmDelay03D-cParaActual)*2+1] ;  //湿度-预警下限   
    History.MiniterDelaySet[6]=ParaList[(cHumiUpLimitAlarmDelay03D-cParaActual)*2+1];        //湿度-报警上限 
    History.MiniterDelaySet[7]=ParaList[(cHumiPreUpLimitAlarmDelay03D-cParaActual)*2+1];
   #ifdef _BL03D
    ReadPagePara();
     #endif

 #ifdef __Thermoberg03d  
  if(ParaList[(cSensorMode-cParaActual)*2+1]==0)
     {ParaList[(cTempValid03D-cParaActual)*2+1]=0;
      ParaList[(cHumiValid03D-cParaActual)*2+1]=0;
     }
  if(ParaList[(cSensorMode-cParaActual)*2+1]==1)
     {ParaList[(cTempValid03D-cParaActual)*2+1]=0;
      ParaList[(cHumiValid03D-cParaActual)*2+1]=1;
     }
if(ParaList[(cSensorMode-cParaActual)*2+1]==2)
     {ParaList[(cTempValid03D-cParaActual)*2+1]=1;
      ParaList[(cHumiValid03D-cParaActual)*2+1]=0;
     }
#endif
   
        
   }

	