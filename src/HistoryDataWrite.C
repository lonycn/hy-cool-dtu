#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#include <math.h>

unsigned char  sixtyMinFlag;
unsigned char  AlarmRecordFlag;
unsigned char  StoreFlashErrorFlag;//=0 正确,  =1 ,record 写入错误  2: page/recordNum写入错误

#define PagesOfOneLoop   4000
#define  cRecordLength	 14
#define  cPageRecordNum	    36
#define  cMaxStorePage      (PagesOfOneLoop*8)   //4000*8page
#define cMaxReordNumIndex   40   //4051-4094  total 44page for every record


#define    cIndexChange         4000  //考虑到每page写36次,总读写次数为10万次36*2000=72000次  w25q128可以每次忽略掉这个参数

#define cStartLowerBeginTime    0x01 
#define cEndOverNwestdata       0x10 
#define cMiddleData             0x00 
#define cAllData                0x11 	
#define  DF_PAGE_SIZE           504





unsigned char storeARecord1()
 {
   return(0);	
}
/*----------------------------------------------
Function:unsigned char storeARecord()
Input:  
Output:
Description:保存一条记录
Para:   CurrentReordNum
        CurrentWritePage:当前页范围为 PagesOfOneLoop*8,实际上显示或者被读取时按照CurrentWritePage%PagesOfOneLoop;
                         
------------------------------------------------*/  	
unsigned char storeARecord()
  {	   unsigned char  i;//j;	 //
           unsigned char  j;   //add 2016-04-13 21:07
           unsigned char  TempRecord[14];
           unsigned char  TimeTempVar1[14];
        
        CurrentReordNum++;
   //-------------------------- 
       if(CurrentWritePage>=cMaxStorePage) //如果超出了最大页数,需要重新读取参数,确定位置
        {
          ReadEepromByte();
        }     	
  //--------------------current page-----
      //换页 36条记录每页  
       if(CurrentReordNum>=cPageRecordNum)  
         {   
         
         	 CurrentReordNum=0x0000;
         	 CurrentWritePage++;
                 if(CurrentWritePage>=cMaxStorePage) //4000page换圈
                  { CurrentWritePage=0;
                    ParaList[cReordNumIndex*2+1]++;  //修改圈数
                    ProgramEepromByte()	;
                  }
	}
//-------------------------loop------------------			  
//modifid   CurrentReordNum position    
       #if 0    
       if((CurrentReordNum==0)&&(((CurrentWritePage%cIndexChange)==0)&&(CurrentWritePage!=0)) )//每4000page换圈
            {   ParaList[cReordNumIndex*2+1]++;
               if(ParaList[cReordNumIndex*2+1]>=240)
                  ParaList[cReordNumIndex*2+1]=0;
                ProgramEepromByte();  
            } 
        #endif      
//------------------------store  record data-------------------------  
            CopyTimeVar(TempRecord);        
#ifdef  _BoHuaiVer
               
               if(History.AlarmRecordFlag==0)  // 正常记录数据
                {
                   TempRecord[5]=0;   //restore 2015-05-30 15:36 不需要秒
                   VarList[cRealDataAct*2+2]=0xdf; //modified 2019-03-28 12:50                  
                 }  
                else if(History.AlarmRecordFlag==1)  //启停状态发生改变
                    VarList[cRealDataAct*2+2]|=0x20; 
                else if(History.AlarmRecordFlag==11)      //有报警数据
                 {   TempRecord[5]=0;        
                     VarList[cRealDataAct*2+2]&=0xdf;
                  }
#endif                 
                 for(i=0;i<8;i++)
                   TempRecord[6+i]=VarList[cRealDataAct*2+i];  //实时温湿度数据

//add 2016-04-13 20:56  
                 if(CurrentReordNum==18)
                    {
                    	LCD_string(2,11,"HalfPage Convert");
                    }	                
                for(i=0;i<3;i++)  
                 {    
			 W25QXX_Write(TempRecord,((ParaList[cReordNumIndex*2+1]*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
		//	 while(SPI_IS_BUSY(SPI_FLASH_PORT));
			 W25QXX_Read(TimeTempVar1,((ParaList[cReordNumIndex*2+1]*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
			 for(j=0;j<14;j++)
			  {if(TempRecord[j]!=TimeTempVar1[j])
                            break;
                           }
                        if(j==14)
                            break;
                 }
                if(i==3) 
                       StoreFlashErrorFlag=1;
                else  	
                       StoreFlashErrorFlag=0;        
/*--------------------------------------------------- 
原来的4000-4050页是用来存储每一页数据的开始数据
-----------------------------------------------------*/              
        //end of add test 2015-10-18 21:52         
//store   first index for every page
/*  delete 2015-11-10 12:09                if(CurrentReordNum==0x00)
                  {  load_page_to_buffer(PagesOfOneLoop+CurrentWritePage/80);
                     write_buffer((CurrentWritePage%80)*6,TimeTemp,6);
                     write_page_from_buffer(PagesOfOneLoop+CurrentWritePage/80);
                  
                    //store  second index for every first index page 
                    if((CurrentWritePage%80)==0)
                      {  load_page_to_buffer(4050);
                         write_buffer((CurrentWritePage/80)*6,TimeTemp,6);	
                         write_page_from_buffer(4050);	
                      }	
                  }   */
/*-------------------------page/record 记录到flash页中-------------------------------------*/           
           
                 TimeTempVar1[0]=CurrentReordNum/256;
                 TimeTempVar1[1]=CurrentReordNum%256;
                 
                 TimeTempVar1[2]=CurrentWritePage/256;
                 TimeTempVar1[3]=CurrentWritePage%256;
                for(i=0;i<3;i++)  
                 {       W25QXX_Write(TimeTempVar1,4096*cIndexPage+((ParaList[cReordNumIndex*2+1]%8)*500+CurrentWritePage/8)*4,4);
	//		 while(SPI_IS_BUSY(SPI_FLASH_PORT));
			 W25QXX_Read(TempRecord,4096*cIndexPage+((ParaList[cReordNumIndex*2+1]%8)*500+CurrentWritePage/8)*4,4);
			 for(j=0;j<4;j++)
			  {if(TempRecord[j]!=TimeTempVar1[j])
                            break;
                           }
                        if(j==4)
                            break;
                 }
                if(i==3) 
                       StoreFlashErrorFlag=2;
                else  	
                       StoreFlashErrorFlag=0;
                 //add 2015-12-05 14:12    
//------------------------------- 将记录存储到实时参数列表中-----------------------               
                 ParaList[cCurrentReordNum*2]=0;   
                 ParaList[cCurrentReordNum*2+1]=CurrentReordNum;   
               
                 ParaList[cCurrentReordPage*2]=CurrentWritePage/256;   
                 ParaList[cCurrentReordPage*2+1]=CurrentWritePage%256; 
                 
                 
                 if((CurrentWritePage%8==0)&&(CurrentReordNum==0)) //每8页开始更换一次页和记录的位置每次换位置的时候,进行一次参数存储
                   {
                      ParaList[cPagePos*2]=(CurrentWritePage/8)/256;   
                      ParaList[cPagePos*2+1]=(CurrentWritePage/8)%256; 
                      ProgramEepromByte();	
                   }
//add 2016-04-11 21:34                 
                 CurrentReordNum_bak=CurrentReordNum;
                 CurrentWritePage_bak=CurrentWritePage;
                 Loop_bak=ParaList[cReordNumIndex*2+1];
                 LcdDisplay();
          return(StoreFlashErrorFlag);   //add 2016-4-25 10:16
         }


//-----------------------------------------------------
//
//TempTimeData
//#define  cAlarmHistoryRecInt    0
//#define  cNormalHistoryRecInt    1
#define  Testbit   13 //9  2015-05-30 5:56
//-------------------------------------------------------
void  DealHistoryData()
  {	unsigned char    	recordint;
  //2015-12-16 12:32	if((VarList[cRealDataAct*2+1]&0x10)==0x00)  //start	
 
#ifdef _BL03D  
 if(( ParaList[(cTempValid03D-cParaActual)*2]*256+ParaList[(cTempValid03D-cParaActual)*2+1]==0x00)|| ( ParaList[(cHumiValid03D-cParaActual)*2]*256+ParaList[(cHumiValid03D-cParaActual)*2+1]==0x00))//温湿度使能
       {
       	
 
  if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0)  //记录启动
  	 {	
    if((ParaList[cSimGetData*2]==0)&&(ParaList[cSimGetData*2+1]==78))	
  	storeARecord();
  	
  	
  else 	
    {	if((VarList[cRealDataAct*2]&0xf0)!=0xf0)  //有报警记录
           //end 2015-05-30 18:27
              {  recordint= (ParaList[(cAlarmHistoryRecInt-cParaActual)*2]*256+ParaList[(cAlarmHistoryRecInt-cParaActual)*2+1])/60;
                 if(VarList[Testbit]%recordint==0)
                   { if(VarList[Testbit]!=RecordNextTime)
                        {  
                            if(storeARecord()==0)
                             {
                             	 RecordNextTime=VarList[Testbit];
                                // printf("Alarm Record OK \r\n");
                             }
                           else
                              ;// printf("Alarm Record error \r\n");
          	        } 
          	     else
          	       { if( recordint==60)   
          	           {   if(sixtyMinFlag==0)
          	           	 {  
                                   if( storeARecord()==0)
                                     {
                                      RecordNextTime=VarList[Testbit];
                                      sixtyMinFlag=1;
                                     } 
                                 }   
          	            } 
          	       }
          	   }
          	 else
          	    sixtyMinFlag=0; 
              } 
           else
             { 
          	 recordint= (ParaList[(cNormalHistoryRecInt-cParaActual)*2]*256+ParaList[(cNormalHistoryRecInt-cParaActual)*2+1])/60;
          	if(VarList[Testbit]%recordint==0)
                   { if(VarList[Testbit]!=RecordNextTime)
                        {  
                            if(storeARecord()==0)
                              { RecordNextTime=VarList[Testbit];
                                //printf("Alarm Record OK \r\n");
                              }
                             else
                              ; // printf("Alarm Record error \r\n"); 
                                
          	        } 
          	     else
          	       { if( recordint==60)   
          	           {   if(sixtyMinFlag==0)
          	           	 {  
                                   if( storeARecord()==0)
                                     {
                                      RecordNextTime=VarList[Testbit];
                                      sixtyMinFlag=1;
                                     } 
                                 }   
          	            } 
          	       }
          	   }
          	 else
          	    sixtyMinFlag=0; 
              }
               
         }

    }
    
}
 #endif 
  } 

//---------------------------------------------------------------
//Function:  void ReadPagePara()
//Input:    None
//Output:   None
//Description: 将spi flash的参数读出来
//----------------------------------------------------------------
void ReadPagePara()
 {
 	#if 1
 	
 	uint8_t  TemBuffer[4]; //每8页开始更换一次页和记录的位置每次换位置的时候,进行一次参数存储
 	         W25QXX_Read(TemBuffer,4096*cIndexPage+((ParaList[cReordNumIndex*2+1]%8)*PagesOfOneLoop+ParaList[cPagePos*2]*256+ParaList[cPagePos*2+1])*4,4);
 	         CurrentReordNum=TemBuffer[1];
                 ParaList[cCurrentReordNum*2]=0;   
                 ParaList[cCurrentReordNum*2+1]=CurrentReordNum;   
                 
                 CurrentWritePage=TemBuffer[2]*256+TemBuffer[3];
                 ParaList[cCurrentReordPage*2]=CurrentWritePage/256;   
                 ParaList[cCurrentReordPage*2+1]=CurrentWritePage%256; 
                
                 CurrentReordNum_bak=CurrentReordNum;
                 CurrentWritePage_bak=CurrentWritePage;
                 Loop_bak=ParaList[cReordNumIndex*2+1];
        #endif         
 }  
 
//---------------------------------------------------------------
//Function:  void UpdatePagePara()
//Input:    None
//Output:   None
//Description: 将参数写入到spi flash中
//---------------------------------------------------------------- 
void UpdatePagePara()
 {  
 	#if  0
 	uint8_t  TimeTempVari[4],i;
 	  
                 TimeTempVari[0]=CurrentReordNum/256;
                 TimeTempVari[1]=CurrentReordNum%256;
                 
                 TimeTempVari[2]=CurrentWritePage/256;
                 TimeTempVari[3]=CurrentWritePage%256;
                for(i=0;i<3;i++)  
                 {    if(W25QXX_Write(TimeTempVari,ParaList[cReordNumIndex*2+1]+32100*DF_PAGE_SIZE)==0)
                            break;
                 }
                 
                 ParaList[cCurrentReordNum*2]=0;   
                 ParaList[cCurrentReordNum*2+1]=CurrentReordNum;   
               
                 ParaList[cCurrentReordPage*2]=CurrentWritePage/256;   
                 ParaList[cCurrentReordPage*2+1]=CurrentWritePage%256; 
                
                 CurrentReordNum_bak=CurrentReordNum;
                 CurrentWritePage_bak=CurrentWritePage;
                 Loop_bak=ParaList[cReordNumIndex*2+1]; 
         #endif         
         }  
//---------------------------------------------------------------
//Function:  void InitialPagePara()
//Input:    None
//Output:   None
//Description: 初始化flash 参数
//----------------------------------------------------------------         
void InitialPagePara()
 {  
 	
 	uint8_t  TimeTempVari[4],i,TempRecord1[4],j;
 	         
                 TimeTempVari[0]=0;  //页
                 TimeTempVari[1]=0xff;  
                 CurrentReordNum=0xff;
                 TimeTempVari[2]=0;   //RECORD
                 TimeTempVari[3]=0;  //LOOP
                 CurrentWritePage=0;
                 CurrentReordNum=0Xff;
              #if  0  
               
                 W25QXX_Erase_Chip( );
                 SpiFlash_WaitReady();
                
              #endif    
                 
                 
                /*-------------------------page/record 记录到flash页中-------------------------------------*/           
             for(i=0;i<3;i++)  
                 {       W25QXX_Write(TimeTempVari,4096*cIndexPage,4);
                       //  while(SPI_IS_BUSY(SPI_FLASH_PORT));
			 W25QXX_Read(TempRecord1,4096*cIndexPage,4);
			 for(j=0;j<4;j++)
			  {if(TempRecord1[j]!=TimeTempVari[j])
                            break;
                           }
                        if(j==4)
                            break;  
                 }
                if(i==3) 
                       StoreFlashErrorFlag=2;
                else  	
                       StoreFlashErrorFlag=0; 
          
                 ParaList[cCurrentReordNum*2]=0;   
                 ParaList[cCurrentReordNum*2+1]=CurrentReordNum;   
               
                 ParaList[cCurrentReordPage*2]=CurrentWritePage/256;   
                 ParaList[cCurrentReordPage*2+1]=CurrentWritePage%256; 
                
                 CurrentReordNum_bak=CurrentReordNum;
                 CurrentWritePage_bak=CurrentWritePage;
                 Loop_bak=ParaList[cReordNumIndex*2+1]; 
                 
         
         } 
         

