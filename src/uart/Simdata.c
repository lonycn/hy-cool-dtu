#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"

 uint8_t   SimBuffer[14];
 uint8_t   SimBufferRead[14];
 uint16_t   FailTimes=0;
 #if  0
 
 void DisplayInt(uint16_t Dat,uint8_t Pos,uint8_t BitWide)
 {	uint8_t  Dis[6];  
        Dis[0]=Dat/10000+0X30;
 	Dis[1]=(Dat%10000)/1000+0X30;
 	Dis[2]=(Dat%1000)/100+0X30;
 	Dis[3]=(Dat%100)/10+0X30;
 	Dis[4]=Dat%10+0X30;
 	Dis[5]=0;
 	switch(BitWide)
 	 {
 	   case 1:  LCD_string(2,Pos,&Dis[4]);break;
 	    case 2:  LCD_string(2,Pos,&Dis[3]);break;
 	     case 3:  LCD_string(2,Pos,&Dis[2]);break;
 	      case 4:  LCD_string(2,Pos,&Dis[1]);break;
 	       case 5:  LCD_string(2,Pos,Dis);break; 	
 	      default:break;	
        }	
 }
 
 
 void  InitialSimData()
   { uint8_t i=0;
     SimBuffer[i++]=21;	
     SimBuffer[i++]=5;
     SimBuffer[i++]=12;	
     SimBuffer[i++]=7;	
     SimBuffer[i++]=44;
     SimBuffer[i++]=0;	
     
     SimBuffer[i++]=0xff;	
     SimBuffer[i++]=0xff;
     SimBuffer[i++]=0xff;	
     SimBuffer[i++]=0xff;	
     SimBuffer[i++]=0;
     SimBuffer[i++]=1;
      SimBuffer[i++]=0;
     SimBuffer[i++]=50;	
   }	
 
void GetNextRecord()
 {
    SimBuffer[5]++;	
    if(SimBuffer[5]>=60)
     {
       SimBuffer[5]=0;
       SimBuffer[4]++;	
     	if(SimBuffer[4]>=60)
     {
       SimBuffer[4]=0;
       SimBuffer[3]++;	
 	if(SimBuffer[3]>=24)
     {
       SimBuffer[3]=0;
       SimBuffer[2]++;	
       if(SimBuffer[2]>=30)
     {
       SimBuffer[2]=0;
       SimBuffer[1]++;	
       if(SimBuffer[1]>12)
     {
       SimBuffer[1]=1;
       SimBuffer[0]++;
     }
     
}
}
}
}
SimBuffer[11]++;
if(SimBuffer[11]>50)
   SimBuffer[11]=0;
   
SimBuffer[13]++;
if(SimBuffer[13]>100)
   SimBuffer[113]=50;
  	
 } 	 
 
void  GenerateSimData(uint8_t  m) 
 {  uint8_t  i,j,k;  
 	
 	InitialSimData();
                    LCD4_Clear();
                    LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"                ");
  	            W25QXX_Erase_Chip( );
                    SpiFlash_WaitReady();	
  	            LCD_string(1,1," Flash Initial  ");
                    LCD_string(2,1,"    Over        ");	
 	            LCD4_Clear();
 	            LCD_string(1,1," Sim Data       ");
                    LCD_string(2,1,"                ");
     if(m==0)
      {  
       for(k=0;k<8;k++)
         { DisplayInt((uint16_t)k,1,1);
          for( CurrentWritePage=0;CurrentWritePage<4000;CurrentWritePage++)	
 	    {   DisplayInt(CurrentWritePage,3,4);
 	    	for(CurrentReordNum=0;CurrentReordNum<cPageRecordNum;CurrentReordNum++)
 	    	   {      DisplayInt((uint16_t)CurrentReordNum,8,2);
 	    	   	  GetNextRecord();
 	                  for(i=0;i<3;i++)  
                             {    
				 W25QXX_Write(SimBuffer,((k*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
		
			 	W25QXX_Read(SimBufferRead,((k*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
				 for(j=0;j<14;j++)
			 		 {if(SimBuffer[j]!=SimBufferRead[j])
                          			  break;
                          	 	}

                        	if(j==14)
                            		break;
                            
                            }
                           if(i==3)
                             {  FailTimes++;
															  DisplayInt((uint16_t)FailTimes,11,5);
                               	
 	                     }
 	
                      }
              }
       }
   }    
 
else
 {
    k=0;
         { DisplayInt((uint16_t)k,1,1);
          for( CurrentWritePage=0;CurrentWritePage<m;CurrentWritePage++)	
 	    {   DisplayInt(CurrentWritePage,3,4);
 	    	for(CurrentReordNum=0;CurrentReordNum<cPageRecordNum;CurrentReordNum++)
 	    	   {      DisplayInt((uint16_t)CurrentReordNum,8,2);
 	    	   	  GetNextRecord();
 	                  for(i=0;i<3;i++)  
                             {    
				 W25QXX_Write(SimBuffer,((k*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
		
			 	W25QXX_Read(SimBufferRead,((k*PagesOfOneLoop+CurrentWritePage)*cPageRecordNum+CurrentReordNum)*cRecordLength,cRecordLength);
				 for(j=0;j<14;j++)
			 		 {if(SimBuffer[j]!=SimBufferRead[j])
                          			  break;
                          	 	}

                        	if(j==14)
                            		break;
                            
                            }
                           if(i==3)
                             {  FailTimes++;
															  DisplayInt((uint16_t)FailTimes,11,5);
                               	
 	                     }
 	
                      }
              }
       }
   }    	
   	
 }
 
#endif
 
 	 
 