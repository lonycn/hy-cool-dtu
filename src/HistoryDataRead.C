#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include	"global.h"
#include <math.h>



//---------------------------------------------------------------
//Function:  void BeginTranslateHis()
//Input:    None
//Output:   None
//Description: 开始传输数据
//----------------------------------------------------------------
void BeginTranslateHis(unsigned char *pp)
 {uint16_t TempCrc;
  unsigned char i=0;	  //20150616
//  uchar Index2CurrentT[cRecordLength]  ;
   for(i=0;i<8;i++)
  	  SendTempBuffer[i]=pp[i];
    SendTempBuffer[1]=0x42; 
 #if 0
    j=SendTempBuffer[5];
    n=j;
    k=SendTempBuffer[3]*256+SendTempBuffer[4];
    if((k==CurrentWritePage)&&(j>CurrentReordNum))
      i=0;
    else if(((ParaList[cReordNumIndex*2+1])/2==0)&&(k>CurrentWritePage))
      {
          i=0;
      }
    else if(CurrentReordNum==0xff)
      {
          i=0;
      }
     else 
      {        
          
          load_page_to_buffer(k);
    //----------------------------------- 	
     	for(i=0;i<3;i++,j++)  //最多发3条
     	 {  
     	    if((k==CurrentWritePage)&&(j>CurrentReordNum))  	  //2015-10-25 19:46
                 break;
       //2015-11-19 9:51      if(k>CurrentWritePage)
            if(((ParaList[cReordNumIndex*2+1])/2==0)&&(k>CurrentWritePage))  //add 2015-11-19 9:51
                break;
             if(j>=cPageRecordNum)
               { j=0;
                 if((k==CurrentWritePage)&&(CurrentReordNum==cPageRecordNum-1))  
                   break;
                 if(k+1>=cMaxStorePage)
                   k=0;
                 else
                   k++;
                load_page_to_buffer(k);
               }
     	   
  //---------------------------  
             read_buffer(j*cRecordLength,Index2CurrentT,cRecordLength);
             
		 for(m=0;m<cRecordLength;m++)
     	           SendTempBuffer[i*cRecordLength+8+m]=Index2CurrentT[m];
     	 } 
       }
#endif	
 //-------------------------------------------         
      SendTempBuffer[1]=0x42; 
      SendTempBuffer[3]=i; 
     
   //add 2016-04-11 21:36
      SendTempBuffer[4]=CurrentWritePage_bak/256;  //page
      SendTempBuffer[5]=CurrentWritePage_bak%256;  
      SendTempBuffer[6]=CurrentReordNum_bak;         //record
      SendTempBuffer[7]= Loop_bak/8;  //2016-01-15 9:10  //loop
  //end of add 2016-04-11 21:37  
    
/*     delete 2016-04-11 21:36
      SendTempBuffer[4]=CurrentWritePage/256;
      SendTempBuffer[5]=CurrentWritePage%256;  
      SendTempBuffer[6]=CurrentReordNum;
      SendTempBuffer[7]= (ParaList[cReordNumIndex*2+1])/2;  //2016-01-15 9:10
*/     
     
      TempCrc=CRC(SendTempBuffer,8);
      SendTempBuffer[8]=TempCrc/256;  //High crc
      SendTempBuffer[9]=TempCrc%256;  //Low crc
      SendDataToBus1_uart0(SendTempBuffer,10);//10+i*cRecordLength);   2019-03-29 14:41
   }	                            


    


//-----------------------------------------------------------
//
//

  /*
//---------------------------------------
//CompareTime(uchar *p1,uchar *p2)
//  1: P1>=P2   0:P1<P2
//-------------------------------------- 
uchar CompareTime(uchar *p1,uchar *p2)
  { 
     if( *p1>*p2)  //year
         return(1);
        else if(*p1<*p2) 
         return(0);
        else
          {
            p1++;
            p2++;
            if( *p1>*p2)  //month
              return(1);
            else if(*p1<*p2) 
              return(0);	
            else
             {
             	p1++;
                p2++;
               if( *p1>*p2)  //day
                 return(1);
                else if(*p1<*p2) 
                  return(0);
                else
                 { p1++;
                p2++;
               if( *p1>*p2)  //hour
                 return(1);
                else if(*p1<*p2) 
                  return(0);
                else
                 {
                 	p1++;
                        p2++;
                        if( *p1>*p2)  //miniter
                         return(1);
                       else if(*p1<*p2) 
                         return(0);  
                       else
                         {
                           p1++;
                           p2++;
                        if( *p1>*p2)  //second
                           return(1);
                         else if(*p1<*p2) 
                           return(0);  	
                        else 
                           return(2); //2015-06-08 9:41 		
                        }
                    }
               }	
             }		
           } 
         
         
         
      }
   

uchar days[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
//---------------------------------------
//CompareTime(uchar *p1,uchar *p2)
//  1: P1>=P2   0:P1<P2
//-------------------------------------- 
uchar CheckTime(uchar *p1)
  { 
     if( p1[0]<100)  //year
      {    if(( p1[1]<13)&&( p1[1]>0))  //month
             {
               if(((p1[2]<=days[p1[1]])&&( p1[2]>0)&&(p1[1]!=2))||((p1[2]<=29)&&( p1[2]>0)&&(p1[1]==2)&&(p1[0]%4==0)))
                {  
                	
                   if( p1[3]<24)  //hour
                    {
                     if( p1[4]<60)  //min
                        if( p1[5]<60)  //second
                           return(1);
                   }
                 } 
              } 
          }       	
         return(0);
     }
   
  */


//---------------------------------------------------------------
//Function:  void DealWithMoreRegRead()
//Input:    None
//Output:   None
//Description: 回复modbus0x41
//----------------------------------------------------------------
void DealWithHisRead(unsigned char   *pp)
  {
  
   //  uchar i;
     /*   for(i=0;i<6;i++)
  	  StartTime[i]=SendTempBuffer[3+i];
  	for(i=0;i<6;i++)
  	  EndTime[i]=SendTempBuffer[9+i];    */
  	BeginTranslateHis(pp);
  }
  







