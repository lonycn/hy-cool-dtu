#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"

#define _GLOBAL_H
#include	"global.h"
#include <math.h>


#define cAllowHumi      20         //湿度压缩使能
#define cAllowTemp      21         //温度压缩使能
#define cHumiAdSatrt    22           //调整范围---湿度起始     
#define cHumiAdEnd      23           //调整范围---湿度结束 
#define cTempAdSatrt    24           //调整范围---温度起始 
#define cTempAdEnd      25           //调整范围---温度结束
#define cHumiAdToSatrt  26           //调整范围到---湿度起始 
#define cHumiAdToEnd    27           //调整范围到---湿度结束 
#define cTempAdToSatrt  28           //调整范围到---温度起始 
#define cTempAdToEnd    29            //调整范围到--温度起始 
#define cSpecialAllow  30             //特殊值使能 
#define cSpecialR_H    31        
#define cSpecialR_L    32
#define cSpecialT_H    33
#define cSpecialT_L    34
#define cTempAdjust    35
#define cLoraStartAddr    55
#define cLoraDeviceNum    56
#define cLoraNodeInterval 99

#define cOrigT         35
#define cOrigRH        36 


//将800--980的温度值压缩至850-950
#define cHumiAdSatrtV     800
#define cHumiAdEndV       980
#define cHumiAdToSatrtV   850
#define cHumiAdToEndV     950
//将400-800的温度值压缩至500-700
#define cTempAdSatrtV     400
#define cTempAdEndV       800
#define cTempAdToSatrtV   500
#define cTempAdToEndV     700 

void BL02D_Special_Update() ;


 struct BL02D_Special_Struct
   {  
   	unsigned char    TempAdAllow;
        int16_t   TempAllowAdLow;
        int16_t   TempAllowAdHigh;
        int16_t   TempAdHigh;
        int16_t   TempAdLow;
        unsigned char    HumiAdAllow;
        int16_t   HumiAllowAdLow;
        int16_t   HumiAllowAdHigh;
        int16_t   HumiAdHigh;
        int16_t   HumiAdLow;
        
        unsigned char  SpecialAlarmAllow;
        unsigned char  HighSpecilalFlag;
        int16_t   RSpeicalH;
        int16_t   RSpeicalL;
        int16_t   TSpeicalH;
        int16_t   TSpeicalL;
   	
   }; 
    
struct BL02D_Special_Struct   BL02D_Special;  

/*----------------------------------------------
Function:BL02D_Special_Temp(int16_t  temp)
Input:  int16_t  temp:读取传感器,经过偏差计算后,得到温度值
Output:  VarList[0...3]:处理后的温湿度放置,该变量为外部变量
Description:通过计算,判断温度是否在温度拉伸范围内,如果是正常模式,直接退出,
                                                  如果是拉伸模式,根据设定的值进行拉伸
                                                  如果是特定值模式,根据设定的特定值进行填充
------------------------------------------------*/   
void  BL02D_Special_Temp(int16_t  temp)
  {	
       
        uint16_t   temp1;
        int16_t   temp2;
        float  floatTRh;
//------------------------------------------------------
   BL02D_Special.HighSpecilalFlag=0;     
    if(BL02D_Special.TempAdAllow==1)
      {   temp1=temp;
      	if((temp>=BL02D_Special.TempAllowAdLow)&&(temp<=BL02D_Special.TempAllowAdHigh)) //压缩范围
          {   floatTRh=(float)(temp-BL02D_Special.TempAllowAdLow)*(float)(BL02D_Special.TempAdHigh-BL02D_Special.TempAdLow)/(float)(BL02D_Special.TempAllowAdHigh-BL02D_Special.TempAllowAdLow);
          
          	temp2=(int16_t)floatTRh+BL02D_Special.TempAdLow;
          }
       else if (temp>BL02D_Special.TempAllowAdHigh) //大于最高范围
         {
         	floatTRh=(float)(temp-BL02D_Special.TempAllowAdHigh)*(float)(1000-BL02D_Special.TempAdHigh)/(float)(1000-BL02D_Special.TempAllowAdHigh);
         	temp2=(int16_t)floatTRh+BL02D_Special.TempAdHigh;
					 
			    if(BL02D_Special.SpecialAlarmAllow==1)
               	    BL02D_Special.HighSpecilalFlag=1;
         }
       else if (temp<BL02D_Special.TempAllowAdLow)  //小于最低范围
        {
           floatTRh=(float)(temp-BL02D_Special.TempAllowAdLow)*(float)(BL02D_Special.TempAdLow+300)/(float)(BL02D_Special.TempAllowAdLow+300);
          
         	temp2=(int16_t)floatTRh+BL02D_Special.TempAdLow;	
			if(BL02D_Special.SpecialAlarmAllow==1)
               	    BL02D_Special.HighSpecilalFlag=2;
        		   
         }  
     temp1=(uint16_t)temp2;	
      }
   //------------   
      else
	   temp1=temp;
     
  //-----------------------------------------------------------   
     if(BL02D_Special.HighSpecilalFlag==1)
       {
       	 VarList1[cTemp1*2]=BL02D_Special.TSpeicalH/256;  //high
         VarList1[cTemp1*2+1]=BL02D_Special.TSpeicalH%256;//low
       	
       }
      else if(BL02D_Special.HighSpecilalFlag==2)
       {
       	 VarList1[cTemp1*2]=BL02D_Special.TSpeicalL/256; //high
         VarList1[cTemp1*2+1]=BL02D_Special.TSpeicalL%256;//low
       	
       }  
       
      else 
    
      {
        VarList1[cTemp1*2]=temp1/256;
        VarList1[cTemp1*2+1]=temp1%256;
 
  
     }
 //---------------------------------------------------------    
             
               VarList[cTemp*2]=temp1/256;
               VarList[cTemp*2+1]=temp1%256;
                  
 	     
 }	
/*----------------------------------------------
Function:void  BL02D_Special_Humi(int16_t  temp)
Input:  int16_t  temp:读取传感器,经过偏差计算后,得到湿度度值
Output:    VarList[0...3]:处理后的温湿度放置,该变量为外部变量
Description:通过计算,判断温度是否在湿度度拉伸范围内,如果是正常模式,直接退出,
                                                  如果是拉伸模式,根据设定的值进行拉伸
                                                  如果是特定值模式,根据设定的特定值进行填充
------------------------------------------------*/   
void  BL02D_Special_Humi(int16_t  temp)
{
	uint16_t   temp1;
        int16_t   temp2;
        float  floatTRh;
	
		BL02D_Special.HighSpecilalFlag=0;    
     if(BL02D_Special.HumiAdAllow==1)
      {
      	if((temp>=BL02D_Special.HumiAllowAdLow)&&(temp<=BL02D_Special.HumiAllowAdHigh))
          {       floatTRh=(float)(temp-BL02D_Special.HumiAllowAdLow)*(float)(BL02D_Special.HumiAdHigh-BL02D_Special.HumiAdLow)/(float)(BL02D_Special.HumiAllowAdHigh-BL02D_Special.HumiAllowAdLow);
          
          	temp2=(int16_t)floatTRh+BL02D_Special.HumiAdLow;
          
          } 
        
        
        else if (temp>BL02D_Special.HumiAllowAdHigh)
         {
         	if(BL02D_Special.SpecialAlarmAllow==1)
               	    BL02D_Special.HighSpecilalFlag=1;
          	
         	floatTRh=(float)(temp-BL02D_Special.HumiAllowAdHigh)*(float)(1000-BL02D_Special.HumiAdHigh)/(float)(1000-BL02D_Special.HumiAllowAdHigh);
         	temp2=(int16_t)floatTRh+BL02D_Special.HumiAdHigh;
         }
       else if (temp<BL02D_Special.HumiAllowAdLow) 
        {  
            if(BL02D_Special.SpecialAlarmAllow==1)
               	    BL02D_Special.HighSpecilalFlag=2;	
        	
           floatTRh=(float)(temp-BL02D_Special.HumiAllowAdLow)*(float)(BL02D_Special.HumiAdLow)/(float)(BL02D_Special.HumiAllowAdLow);
          
         	temp2=(int16_t)floatTRh+BL02D_Special.HumiAdLow;	
        	}	   
       	temp1=(uint16_t)temp2;	
      }
//----------------------
	else
		 temp1=temp;
    
      if(BL02D_Special.HighSpecilalFlag==1)
       {
       	 VarList1[cHumi1*2]=BL02D_Special.RSpeicalH/256;  //high
        VarList1[cHumi1*2+1]=BL02D_Special.RSpeicalH%256;  //low
       	
       }
      else if(BL02D_Special.HighSpecilalFlag==2)
       {
       	 VarList1[cHumi1*2]=BL02D_Special.RSpeicalL/256;  //high
         VarList1[cHumi1*2+1]=BL02D_Special.RSpeicalL%256; //low
       	
       }  
       
      else
        {     
        VarList1[cHumi1*2]=temp1/256;
        VarList1[cHumi1*2+1]=temp1%256;
        } 
           
               VarList[cHumi*2]=temp1/256;
               VarList[cHumi*2+1]=temp1%256; 
              
             
    }   
 


 #define  ParaNumFor02D  500
/*----------------------------------------------
Function:void BL02D_Special_Initial()
Input:  None
Output:    None
Description:特定情况下的参数初始化
------------------------------------------------*/    		
uint8_t menu1[24][20]={
{"菜品1"},
{"菜品2"},	
{"菜品3"},	
{"菜品4"},	
{"菜品5"},	
{"菜品6"},	
{"菜品7"},	
{"菜品8"},	
{"菜品9"},	
{"菜品10"},	
{"菜品11"},	
{"菜品12"},	
{"菜品13"},	
{"菜品14"},	
{"菜品15"},	
{"菜品16"},	
{"菜品17"},	
{"菜品18"},	
{"菜品19"},	
{"菜品20"},			
{"菜品21"},
{"菜品22"},
{"菜品23"},
{"菜品24"}
};
void  InitialMenu()
{ 	uint8_t  i,j;	
for(i=0;i<24;i++)
	{ for(j=0;j<20;j++)
    ParaList[cItemAddr*2+i*20+j]=menu1[i][j];
	}
}	
		
		
#define  Double		
//#define  single		
void BL02D_Special_Initial()
   {
  
   //允许调整的标志 0：不允许  1：允许 
		
 	uint16_t  i;	
#ifdef Double		 
        uint8_t  BL02D_Special_Initial[]={
                                       0,0,0,0, //允许调整的标志 0：不允许  1：允许    20 21
                                      cHumiAdSatrtV/256,cHumiAdSatrtV%256,  //22
                                      cHumiAdEndV/256,cHumiAdEndV%256,      //23
                                       0,0,  //24 选择的类型
																			 0,30,0,40, //25 1#设定值 3度，偏差值4度
																			 0,40,0,41, //27 2#设定值 3度，偏差值4度
																			 0,50,0,42, //29 3#设定值 3度，偏差值4度
																			 0,60,0,43, //31 4#设定值 3度，偏差值4度
																			 0,70,0,44, //33 5#设定值 3度，偏差值4度
																			 0,80,0,45, //35 6#设定值 3度，偏差值4度
																			 0,90,0,46, //37 7#设定值 3度，偏差值4度
																			 0,100,0,47, //39 8#设定值 3度，偏差值4度
																			 0,110,0,48, //41 9#设定值 3度，偏差值4度
																			 0,120,0,49, //43 10#设定值 3度，偏差值4度
																			 0,130,0,50, //45 11#设定值 3度，偏差值4度
																			 0,140,0,51, //47 12#设定值 3度，偏差值4度
																			 0,150,0,52, //49 13#设定值 3度，偏差值4度
																			 0,160,0,53, //51 14#设定值 3度，偏差值4度
																			 0,170,0,54, //53 15#设定值 3度，偏差值4度
																			 0,180,0,55, //55 16#设定值 3度，偏差值4度
																			 0,190,0,56, //57 17#设定值 3度，偏差值4度
																			 0,200,0,57, //59 18#设定值 3度，偏差值4度
																			 0,210,0,58, //61 19#设定值 3度，偏差值4度
																			 0,220,0,59, //63 20#设定值 3度，偏差值4度
																			 0,230,0,60, //65 21#设定值 3度，偏差值4度
																			 0,240,0,61, //67 22#设定值 3度，偏差值4度
																			 0,250,0,62, //69 23#设定值 3度，偏差值4度
																			 0,260,0,63,//71 24#设定值 3度，偏差值4度
                                     0,0,   //READ LORA  73 
                                     0,0,   //WRITE lora 74
                                  //   0,0,0,0,0,0,0,0,  //75-8   remote  alarm
																	    0,7,0,1,0,19,0,1,  //75-8   1号机组和二号机组的切换时间
                                    // 0,0,0,0,0,0,0,0,   //79-82
                                    // 0,0,0,0,0,0,0,0,   //83-86
                                     0,24,0,3,0,3,0,19,   //79-82  年月日时
                                     0,21,0,54,0,0,0,0,   //83-86   分秒


0,0,0,0,0,0,0,0,  //87...90
                                      0,21,//91
                                      0,3,  //92
                                      0,29, //93
                                      0,21, //94
                                      0,18,  //95
                                      0,01 , //91...96 21-3-19 21:18:01
                                     0,0,   //time trig  97
                                     0,0, //spi flash   98
                                     0,20,   //node interval  loop interval 30s   99
                                     cReset1,cReset2,//  //Initial flag  100
                                     
																		 0,0,0,0,0,0,0,0,0,0,  //101-105
                                     0,0,0,0, //106-7
																		 
																		 
																		 
																		 0,2, //108  gateway test  loop  device interval  100ms
																		300/256,300%256,//109    cloud send interval
                                     
																		 
																		//----------------------- 
																		 0,1,0,2,1440/256,1440%256, //110
                                     0,1,0,1,1440/256,1440%256, //113
                                     0,0,0,4,1440/256,1440%256, //116
                                     0,0,0,3,1440/256,1440%256, //119
                                     0,0,0,6,1440/256,1440%256, //122
                                     0,0,0,5,1440/256,1440%256, //125
                                     0,0,0,8,1440/256,1440%256, //128
                                     0,0,0,7,1440/256,1440%256, //131
                                     0,0,0,10,1440/256,1440%256, //134
                                     0,0,0,9,1440/256,1440%256, //137-9
                                     
                                     
																		 //------------------------------------
																		 430/256,430%256, 0,30,//140
                                     
																		 //---------------------------
                                     0,3,0,1,0,0,0,0,0,0, //142-151  active
                                     0,0,0,0,0,0,0,0,0,0,//
                                    //------------------------------ 
																		 0,4,0,4,0,4,0,4,0,4,//152-156  motortype
                                     0,4, //157
                                     0,4, //158
                                     0,4,0,4,0,4, //159-161
																		

                                    //-------------------------
                                     0,0, //162       //Double force  自动轮值
                                     0,0,0,0,0,0,0,0,   //163-166
                                    //--------------------------
																		 0,0,0,0,0,0,0,0,  //167-176  single force
                                     0,0,0,0,0,0,0,0,  
                                     0,0,0,0,//175-176
																		//-------------------- 
																		 0,0,0,0,  //177--181  frozen Enable
                                     0,0,0,0,0,0,
																		 
																	//---------------------	探头数量 
																		 0,2,   //182
                                     0,0,0,0,0,0,0,0,  //183-6
                                     0,0,0,0,0,0,0,0,  //187-90
                                     0,0, //191
																	//---------------------	探头类型 0：485  1：ntc 
																		 0,0,0,0,0,0,  //192-4
                                     0,0,0,0, //195-6
                                                
                                     0,0, //197  door  delay
                                     
                                     0,0,  //198 0:not use door  1:use door
																		 0,1, //199 enable tp set
																		 1234/256, 1234%256, //200 password
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0, //201-210   write 78 reset control
																		 
																		 0,0,   // 211 meter type  0:direct meter  1: coil meter
																		 0,0,0,0,0,0,0,0,0,0, //212-216
																		 0,0,0,0,0,0,0,0,0,0, //217-221
                                     0,0,0,0,0,0,0,0,0,0, //222-226
                                     0,0,0,0,0,0,         //227-229
                                    0x229/256,0x229%256,//230-232
																		0x8020/256,0x8020%256,
                                    0x8000/256,0x8000%256,
																		0,0,0,0,0,0,0,0,0,0,  //233-237
																		0,0,0,0,0,0,//238-240 dtu hard ver
																		0,0,0,0,0,0,//241-243 dtu software ver
																		0,0,//244 0:正常  1：欠费  2：常亮
																		1200/256,1200%256,  //245: coolCompensate time 1200s
																		0,1,//246, //246: coolCompensate Enable
																		0,1,//247  uart0<-->uart3  gps是否启用
																		0,0, //248  WATCHDOG
																		0,1   //249   //20250320   是否使用di
                                };
 #endif

#ifdef single		 
        uint8_t  BL02D_Special_Initial[228*2]={
                                       0,0,0,0, //允许调整的标志 0：不允许  1：允许    20 21
                                      cHumiAdSatrtV/256,cHumiAdSatrtV%256,  //22
                                      cHumiAdEndV/256,cHumiAdEndV%256,      //23
                                      0,0,  //24 选择的类型
																			 0,30,0,40, //25 1#设定值 3度，偏差值4度
																			 0,40,0,41, //27 2#设定值 3度，偏差值4度
																			 0,50,0,42, //29 3#设定值 3度，偏差值4度
																			 0,60,0,43, //31 4#设定值 3度，偏差值4度
																			 0,70,0,44, //33 5#设定值 3度，偏差值4度
																			 0,80,0,45, //35 6#设定值 3度，偏差值4度
																			 0,90,0,46, //37 7#设定值 3度，偏差值4度
																			 0,100,0,47, //39 8#设定值 3度，偏差值4度
																			 0,110,0,48, //41 9#设定值 3度，偏差值4度
																			 0,120,0,49, //43 10#设定值 3度，偏差值4度
																			 0,130,0,50, //45 11#设定值 3度，偏差值4度
																			 0,140,0,51, //47 12#设定值 3度，偏差值4度
																			 0,150,0,52, //49 13#设定值 3度，偏差值4度
																			 0,160,0,53, //51 14#设定值 3度，偏差值4度
																			 0,170,0,54, //53 15#设定值 3度，偏差值4度
																			 0,180,0,55, //55 16#设定值 3度，偏差值4度
																			 0,190,0,56, //57 17#设定值 3度，偏差值4度
																			 0,200,0,57, //59 18#设定值 3度，偏差值4度
																			 0,210,0,58, //61 19#设定值 3度，偏差值4度
																			 0,220,0,59, //63 20#设定值 3度，偏差值4度
																			 0,230,0,60, //65 21#设定值 3度，偏差值4度
																			 0,240,0,61, //67 22#设定值 3度，偏差值4度
																			 0,250,0,62, //69 23#设定值 3度，偏差值4度
																			 0,260,0,63,//71 24#设定值 3度，偏差值4度
                                     0,0,   //READ LORA  73 
                                     0,0,   //WRITE lora 74
                                     0,0,0,0,0,0,0,0,  //75   remote  alarm 
                                     0,0,0,0,0,0,0,0,   //79
                                     0,0,0,0,0,0,0,0,   //83
                                     0,0,0,0,0,0,0,0,  //87...90
                                     0,21,
                                      0,3,
                                      0,29,
                                      0,21,
                                      0,18,
                                      0,01 , //91...96 21-3-19 21:18:01
                                     0,0,   //time trig  97
                                     0,0, //spi flash   98
                                     0,20,   //node interval  loop interval 30s   99
                                     cReset1,cReset2,// //Initial flag  100
                                     0,0,0,0,0,0,0,0,0,0,  //101-105
                                     0,0,0,0,
																		 
																		 
																		 
																		 0,2, //108  gateway test  loop  device interval  100ms
																		300/256,300%256,//109    cloud send interval
                                     
																		 
																		//----------------机组切换时间------- 
																		 0,0,0,2,1440/256,1440%256, //110
                                     0,0,0,1,1440/256,1440%256, //113
                                     0,0,0,4,1440/256,1440%256, //116
                                     0,0,0,3,1440/256,1440%256, //119
                                     0,0,0,6,1440/256,1440%256, //122
                                     0,0,0,5,1440/256,1440%256, //125
                                     0,0,0,8,1440/256,1440%256, //128
                                     0,0,0,7,1440/256,1440%256, //131
                                     0,0,0,10,1440/256,1440%256, //134
                                     0,0,0,9,1440/256,1440%256, //137
                                     
                                     
																		 //------------------------------------
																		 430/256,430%256, 0,100,//140
                                     
																		 //---------------------------
                                     0,3,0,0,0,0,0,0,0,0, //142-151  active
                                     0,0,0,0,0,0,0,0,0,0,//
                                    //------------------------------ 
																		 0,3,0,3,0,3,0,3,0,3,//152-161  motortype
                                     0,3, //157
                                     0,3, //158
                                     0,3,0,3,0,3,
																		

                                    //-------------------------
                                     0,0, //162-166        //Double force
                                     0,0,0,0,0,0,0,0,   //163-166
                                    //--------------------------
																		 0,0,0,1,0,1,0,1,  //167-176  single force
                                     0,1,0,1,0,1,0,1,  
                                     0,1,0,1,//175-176
																		//-------------------- 
																		 0,0,0,0,  //177--181  frozen Enable
                                     0,0,0,0,0,0,
																		 
																	//---------------------	探头数量 
																		 0,2,   //182
                                     0,0,0,0,0,0,0,0,  //183
                                     0,0,0,0,0,0,0,0,  //187
                                     0,0, //191
																	//---------------------	探头类型 0：485  1：ntc 
																		 0,0,0,0,0,0,  //192
                                     0,0,0,0, //195
                                                
                                     0,0, //197  door  delay
                                     
                                     0,0,  //198 0:not use door  1:use door
																		 0,1, //  199 enable tp set
																		 1234/256, 1234%256,  //200  password
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0,
																		 0,0,0,0, //201-210   write 78 reset control
																		 
																		 0,0,   // 211 meter type  0:direct meter  1: coil meter
																		0,0,    // 212 机组类型  0：单机组   1：双机组
																		0,0,0,0,0,0,0,0, //213-216
																		0,0,0,0,0,0,0,0,0,0, //217-221
                                    0,0,0,0,0,0,0,0,0,0, //222-226
                                    0,0,0,0,0,0,
                                    0x229/256,0x229%256,  																		//227-229
																		0x8020/256,0x8020%256,
                                    0x8000/256,0x8000%256,
																		
																		//230-232
																		0,0,0,0,0,0,0,0,0,0,  //233-237
																		0,0,0,0,0,0,//238-240 dtu hard ver
																		0,0,0,0,0,0,//241-243 dtu software ver
																		0,0,//244 0:正常  1：欠费  2：常亮
																		1200/256,1200%256 , //245: coolCompensate time 1200s
																		0,1, //246: coolCompensate Enable
																		0,1,//247  uart0<-->uart3  gps是否启用
																		0,0, //248  WATCHDOG
																		0,1   //249   //20250320   是否使用di
                                };
 #endif																
                       
      for(i=0;i<ParaNumFor02D-cAllowHumi;i++)
       { 
      
			 {
				  ParaList[(i+cAllowHumi)*2]  =BL02D_Special_Initial[i*2];
          ParaList[(i+cAllowHumi)*2+1]=BL02D_Special_Initial[i*2+1];
			 }
		 }
InitialMenu();			 
   BL02D_Special_Update(); 
    }  
    
/*----------------------------------------------
Function:void BL02D_Special_Update()
Input:  None
Output:    None
Description:更新特定情况下的参数
------------------------------------------------*/    
void BL02D_Special_Update()    
    {//add 2020-04-11 12:02
               BL02D_Special.HumiAdAllow=ParaList[cAllowHumi*2+1];      
               BL02D_Special.TempAdAllow=ParaList[cAllowTemp*2+1];      
              
               
             // 需要调整的区间范围  
               BL02D_Special.HumiAllowAdLow=(int16_t)(ParaList[cHumiAdSatrt*2]*256+ParaList[cHumiAdSatrt*2+1]); 
                BL02D_Special.HumiAllowAdHigh=(int16_t)(ParaList[cHumiAdEnd*2]*256+ParaList[cHumiAdEnd*2+1]);
               
               BL02D_Special.TempAllowAdLow=(int16_t)(ParaList[cTempAdSatrt*2]*256+ParaList[cTempAdSatrt*2+1]); 
               BL02D_Special.TempAllowAdHigh=(int16_t)(ParaList[cTempAdEnd*2]*256+ParaList[cTempAdEnd*2+1]);
             //需要调整到的区间范围       
               BL02D_Special.HumiAdLow=(int16_t)(ParaList[cHumiAdToSatrt*2]*256+ParaList[cHumiAdToSatrt*2+1]); 
               BL02D_Special.HumiAdHigh=(int16_t)(ParaList[cHumiAdToEnd*2]*256+ParaList[cHumiAdToEnd*2+1]);
               
               BL02D_Special.TempAdLow=(int16_t)(ParaList[cTempAdToSatrt*2]*256+ParaList[cTempAdToSatrt*2+1]); 
               BL02D_Special.TempAdHigh=(int16_t)(ParaList[cTempAdToEnd*2]*256+ParaList[cTempAdToEnd*2+1]);
               
               //需要写入的特殊值
              BL02D_Special.RSpeicalH=(int16_t)(ParaList[cSpecialR_H*2]*256+ParaList[cSpecialR_H*2+1]); 
              BL02D_Special.RSpeicalL=(int16_t)(ParaList[cSpecialR_L*2]*256+ParaList[cSpecialR_L*2+1]); 
              BL02D_Special.TSpeicalH=(int16_t)(ParaList[cSpecialT_H*2]*256+ParaList[cSpecialT_H*2+1]); 
              BL02D_Special.TSpeicalL=(int16_t)(ParaList[cSpecialT_L*2]*256+ParaList[cSpecialT_L*2+1]);
              BL02D_Special.SpecialAlarmAllow=ParaList[cSpecialAllow*2+1];
             //add 2022           
						AddrStart=ParaList[cLoraStartAddr*2+1];	
            AddrLen=ParaList[cLoraDeviceNum*2+1];
						 AddrStart=1;	
             AddrLen=10;
//end of add 2022						 
             NodeInterval=ParaList[cLoraNodeInterval*2]*256+ParaList[cLoraNodeInterval*2+1];
        
    }                      
  
  
void  DisLcdTemp()
       {  DisplayTemp(&VarList1[DisplayTurn*2],DisplayMode); 
       }                  