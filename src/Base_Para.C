#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"
#include <math.h>

void BL03D_Base_Update();
void BL03D_Base_ParaInitial();
/*--------------------------------------------------------------------------------------------
//Function: void BL03D_Base_ParaInitial()
//Input:   None
//Output:   None 
//Description：bh 03D 基础参数初始化
//空间分配:  存储:0-19  读取:0-19
//--------------------------------------------------------------------------------------------*/                 
void BL03D_Base_ParaInitial()
     { 	uint8_t  i;		                 
        uint8_t  BasePara_Initial[]={
                                       0,1,
					                             0,3,
					                             0,0,
					                             0,0,
					                             0,2,  //device:253  :DeviceNum   baudrate:9600 T_error=0, TH_error=0, remote alarm
                                       500/256,500%256,
					                             65036/256,65036%256,  //2-8°
                                       0,0,
					                             0,0,  //alarm all, no alarm
                                       1000/256,1000%256,
					                             0/256,0%256, //humi limit
                                       0,0, //door  11
                                       0,0,0,0,//internal T Offset  初始化不可修改internal H Offset  初始化不可修改
                                       0,0,//alarm delay  14 :AlarmDelay
                                       0,0, //alarm depend 15       0:all  1:t  2:humi  :AlarmOutDepend
                                       0,1,//backlight  16
                                       0,0,//Version   17
                                       0,0, //reset time
                                       0,3, //DisplayMod  3:all  19
					                            
					                            
                                     };
      for(i=0;i<20;i++)
        {
         if(ResetFlag==0) 
          {
            if((i!=cDeviceNum)&&(i!=cInterTOffet)&&(i!=cInterHROffet))
             { 			
                ParaList[i*2]	=BasePara_Initial[i*2];
                ParaList[i*2+1]=BasePara_Initial[i*2+1];
             }   
       } 
       else
       {
           ParaList[i*2]	=BasePara_Initial[i*2];
           ParaList[i*2+1]=BasePara_Initial[i*2+1];
       }
		 } 
   BL03D_Base_Update();    
   }

void BL03D_Base_Update()
     { 			                 
      
                 DeviceNum=ParaList[cDeviceNum*2]*256+ParaList[cDeviceNum*2+1];
                 AlarmOutDepend=ParaList[cAlarmDepend*2+1];
                 AlarmOuMode=ParaList[cAlarmOutPut*2+1];
                 AlarmDelay=ParaList[cAlarmDelay*2]*256+ParaList[cAlarmDelay*2+1];
                 DisplayMode=ParaList[cDisplayMode*2]*256+ParaList[cDisplayMode*2+1];
   }

	