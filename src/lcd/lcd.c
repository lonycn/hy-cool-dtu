#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include <math.h>
#define _GLOBAL_H
#include	"global.h"
void    LCD4_Home( );
void	  LCD4_Home2( );
void    LCD4_Clear();

typedef	unsigned char	Byte;       //single byte   0xff
typedef	uint16_t	Word; 		//two byte 		0xffff
typedef	unsigned long	Dword;		//four byte		0xffffffff
typedef	uint16_t	uint;
typedef	unsigned char	uchar;
typedef unsigned char     u8	;
typedef uint16_t      u16	;
typedef signed   long     u32	;
/*#include "led.h"
 struct LED_Struct
   {  
   	 u8 LEDStatus;
  	 u8 LEDFlag;
  	 u16 LEDSetDelayCount; //单位计数
  	 u8 LEDDelay;  //用来统计多少个定时中断算作一个计算单元
         u16 AskLEDOnCount; //设置的on周期
         u16 AskLEDOffCount; //设置的off周期
         u8 AskLEDTime;      //设置的鸣响次数
         u8 LEDOnDelayFlag;  //machine中处于on周期标志
         u8 LEDOffDelayFlag;  //machine 中处于off周期标志
   	 u8 LEDtimes;
   	
   }; 
    
struct LED_Struct   LED;  
*/
/*LED程序的使用说明:
  调用方式一:  使用线程调用
   step1:  程序开始调用LEDInital()
   step2:  while主循环中调用StatusMachine_LEDLED()
   step3:  定时中断程序中调用Timer_LEDControl()
   step4:  需要LED操作的地方调用LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays )
           Ondelays实际时间=LEDDelay*Ondelays*timer定时时间  
 调用方式二:  直接延时调用
   step1:  程序开始调用LEDInital()
   step2:  无
   step3:  定时中断程序中调用Timer_LEDControl()
   step4:  需要LED操作的地方调用LEDLEDLED(unsigned char Times,u16 DuringOn,u16 DuringOff)
 */



void ClearRs(void)
 {
    PA14=0;	
 }	
void SetRs(void)
 {
 	PA14=1;
 }


void ClearRW(void)
 {
    PB9=0;	
 }	
void SetRW(void)
 {
    PB9=1;	
 }
void ClearCE(void)
 {
 	PE5=0;	
 }	
void SetCE(void)
 {
 	PE5=1;	
 } 
 
void SetBL(void)
 {
   PB7=1;	
 }
 
void ClrBL(void)
{
  PB7=0;	
} 
/*----------------------------------------------
Function:void  LEDInital()
Input:  
Output:
Description:初始化蜂鸣器LED PC4
------------------------------------------------*/  	
void  LCDInital(void)
  {
	
  	GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
  	
  	GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PB, BIT3, GPIO_PMD_OUTPUT);
  	GPIO_SetMode(PB, BIT7, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PB, BIT9, GPIO_PMD_OUTPUT);
	
	GPIO_SetMode(PC, BIT2, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PC, BIT3, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT6, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT7, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT14, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PD, BIT15, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PE, BIT5, GPIO_PMD_OUTPUT);
	SetBL();
	
		
   }	


 	 
 
void CodeLcdData(unsigned char dat)
 {unsigned char i=0;
    if((dat&(1<<i))!=0)
      PC2=1;
    else
      PC2=0; 
  //-----------1  
    i++;
    if((dat&(1<<i))!=0)
      PC3=1;
    else
      PC3=0;   
  //-----------2  
    i++;
   if((dat&(1<<i))!=0)
      PD15=1;
    else
      PD15=0;    
   //-----------3  
    i++;
    if((dat&(1<<i))!=0)
      PD14=1;
    else
      PD14=0;     
   //-----------4  
    i++;
    if((dat&(1<<i))!=0)
      PD7=1;
    else
      PD7=0;   
  //-----------5  
    i++;
   if((dat&(1<<i))!=0)
      PD6=1;
    else
      PD6=0;    
   //-----------6  
    i++;
    if((dat&(1<<i))!=0)
      PB3=1;
    else
      PB3=0;  
    //-----------7  
    i++;
   if((dat&(1<<i))!=0)
      PB2=1;
    else
      PB2=0;                  
  }	
 	 
/*----------------------------------------------
Function:void  LEDOff()
Input:  
Output:
Description:关闭蜂鸣器
------------------------------------------------*/  
#define  cRight    0

#define __NeedLcd
//-----------------------------------------------------------------
//Function   :void delay_50us(unsigned char Delay)
//Input      :unsigned char Delay:
//Output     :None
//Description:1,
//use IO     :
//-----------------------------------------------------------------
void delay_50us(unsigned char Delay)
{
 unsigned long i,j;

for (i=0;i<Delay;i++)
   {
   for (j=1;j<50;j++);

   }
}
//-----------------------------------------------------------------
//Function   :void waitMS(unsigned char tt)
//Input      :unsigned char tt:
//Output     :None
//Description:1,
//use IO     :
//-----------------------------------------------------------------
void waitMS(unsigned char tt)
{
 unsigned long j;

 for(j=0;j<tt;j++)
 {
   delay_50us(20);	 	       // delay 50us
 }
}
//---------------------------------------------====
//Function:void delay1ms(int16_t x)
//Input   :int16_t x:ms单位
//Output  :None
//Description: 1ms的延时
//---------------------------------------------====
void delay1ms(int16_t x)				 //单位（1ms）延时
{	int16_t i,j;
	for(i=0;i<x;i++)
		for(j=0;j<120;j++);
}

//---------------------------------------------
//Function:bit wait_until_ready(void)
//Input   :None
//Output  :None
//Description: 读取操作状态返回，超出一定的时间报错
//---------------------------------------------==
#ifdef __NeedLcd
unsigned char  wait_until_ready(void)
{uint16_t  TempCount=0;
     
	  ClearRs();
	  SetRW();
	  SetCE();
	//   INSCON&=Bin(10111111);
	   
	 GPIO_SetMode(PB, BIT2, GPIO_PMD_INPUT);
	  while(PB2==1)
	     {  TempCount++;
	        if(TempCount>100)
	          break;
	     	
	     }	
	  GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
	  ClearCE();
	
	  return(cRight);
	}
#else
  unsigned char  wait_until_ready(void)
{
	  return(cRight);
	}
#endif

//---------------------------------------------
//Function:bit LCD_cmd(unsigned char cmd)
//Input   :unsigned char cmd：写入的命令值
//Output  :None
//Description: 写命令
//---------------------------------------------==
unsigned char LCD_cmd(unsigned char cmd)
{unsigned char  i;
	 wait_until_ready();
	// INSCON|=Bin(01000000);
	 ClearRs();
	 ClearRW();
	 ClearCE();
	 ClearCE();
	// INSCON&=Bin(10111111);
	
	 CodeLcdData(cmd);
	
	// INSCON|=Bin(01000000);
	 ClearCE();
	 ClearCE();
	// INSCON&=Bin(10111111);
	
	  for(i=0;i<200;i++);
	// INSCON|=Bin(01000000);
	 SetCE();
	 SetCE();
	 ClearCE();
//	 INSCON&=Bin(10111111);
   return(cRight);
}
//---------------------------------------------
//Function:void LCD_init(void)
//Input   :None
//Output  :None
//Description: lcd初始化
//---------------------------------------------==
void LCD_init(void)
{  delay1ms(3);
   LCD_cmd(0x38);/*8位数据，2行显示*/
   //LCD_cmd(0x08);/*显示关闭*/
   //LCD_cmd(0x01);/*清屏*/
    /*清屏和光标归位需要较长的时间*/
   LCD_cmd(0x06);/*写入数据后光标右移*/
   LCD_cmd(0x0c);/*显示开，不显示光标*/
   LCD_cmd(0x0c);/*显示开，不显示光标*/
   LCD_cmd(0x01);/*清屏*/
   LCD_cmd(0x0c);/*显示开，不显示光标*/
	 LCD4_Home( );
	 LCD4_Home2( );
   LCD4_Clear();

 }

//---------------------------------------------
//Function:bit LCD_char(char str)
//Input   :unsigned char DAT：写入的数据
//Output  :None
//Description: 写数据
//---------------------------------------------==
unsigned char LCD_char(char str)
{  unsigned char  i;
	
	  wait_until_ready();
//-------------	
	// INSCON|=Bin(01000000);
	 SetRs();                             //指向数据寄存器
	 ClearRW();
	 ClearCE();
	//  INSCON&=Bin(10111111);
//---------------	 
	 CodeLcdData(str);
//---------------	 
	// INSCON|=Bin(01000000);
	 ClearCE();
	//  INSCON&=Bin(10111111);
//-------------	 
	 for(i=0;i<200;i++);
//----------	
	// INSCON|=Bin(01000000);
	SetCE();
	SetCE();
	ClearCE();
	// INSCON&=Bin(10111111);
//-------------	 
	return(cRight);
}

//---------------------------------------------
//Function:bit  setxy(char x,char y)
//Input   :char x：行x=1/2
//         char y: 列y=1~16的任意整数
//Output  :None
//Description:定位
//---------------------------------------------==
unsigned char setxy(char x,char y)
{ char temp;
    if(x==1)
      {
      	temp=0x80+y-1;
       LCD_cmd(temp);
     }
 else
    {
  	  temp=0xC0+y-1;
      LCD_cmd(temp);
    }
   return(cRight);
  }

//---------------------------------------------
//Function:bit  LCD_string(unsigned char x,unsigned char y,char *s)
//Input   :char x：行x=1/2
//         char y: 列y=1~16的任意整数
//         char *s:制定的字符串位置
//Output  :None
//Description:定位
//---------------------------------------------==
void  LCD_string(unsigned char x,unsigned char y,unsigned char *s)
   {
	    setxy(x,y);
	    while(*s!='\0')
           {
           	 LCD_char(*s++);
           }

       
   }


//-----------------------------------------------------------------
//Function   :void LCD4_Clear(void)
//Input      :none
//Output     :None
//Description:1,	清除lcd屏上的内容
//use IO     :
//-----------------------------------------------------------------
void LCD4_Clear(void)
{
   // LCD_cmd(0x01);
   // waitMS(5);
   LCD_string(1,1,"                ");
   LCD_string(2,1,"                ");
   
}

//-----------------------------------------------------------------
//Function   :void LCD4_On(void)
//Input      :none
//Output     :None
//Description:1,	开启lcd屏
//use IO     :
//-----------------------------------------------------------------
void LCD4_On(void)
{
    LCD_cmd(0x0e);
    waitMS(5);
}

//-----------------------------------------------------------------
//Function   :void LCD4_Off(void)
//Input      :none
//Output     :None
//Description:1,	关闭lcd屏
//use IO     :
//-----------------------------------------------------------------
void LCD4_Off(void)
{
    LCD_cmd(0x08);
    waitMS(5);
}

//-----------------------------------------------------------------
//Function   :void LCD4_Home(void)
//Input      :none
//Output     :None
//Description:1,	lcd光标移动到第一行的开始
//use IO     :
//-----------------------------------------------------------------
void LCD4_Home(void)
{
    LCD_cmd(0x02);
    waitMS(5);
}


//-----------------------------------------------------------------
//Function   :void LCD4_Home2(void)
//Input      :none
//Output     :None
//Description:1,	lcd光标移动到第二行的开始
//use IO     :
//-----------------------------------------------------------------
void LCD4_Home2(void)
{
    LCD_cmd(0xc0);
    waitMS(5);
}

//-----------------------------------------------------------------
//Function   :void LCD4_PutC(unsigned char data)
//Input      :none
//Output     :None
//Description:1,	发送一个字符数据
//use IO     :
//-----------------------------------------------------------------
void LCD4_PutC(unsigned char dat)
{
    LCD_char(dat);
}

//-----------------------------------------------------------------
//Function   :void LCD4_PutC_Const(const unsigned char data)
//Input      :none
//Output     :None
//Description:1,	发送一个常数字符
//use IO     :
//-----------------------------------------------------------------
void LCD4_PutC_Const(const unsigned char dat)
{
    LCD_char(dat);
}
//-----------------------------------------------------------------
//Function   :void LCD4_PutC_Const(const unsigned char data)
//Input      :none
//Output     :None
//Description:1,	发送一个字符串
//use IO     :
//-----------------------------------------------------------------
void LCD4_PutS(unsigned char *dat)
{
     while(*dat!='\0')
           {
           	 LCD_char(*dat++);
           }
}

//-----------------------------------------------------------------
//Function   :void DisplayTitle()
//Input      :none
//Output     :None
//Description:1,	在lcd屏上显示温度标题
//use IO     :
//-----------------------------------------------------------------
void DisplayTitle()
  {
    #ifdef _BL02D 	
        DisplayTitle_02D( );
    #endif
    
     #ifdef _BL03D 	
        DisplayTitle_03D();
    #endif     
}



void DisplayTitle_02D( )
  { 
    if( DisplayMode ==1)
     {LCD_string(1,1," T:");
      LCD_string(2,1,"                ");
     }
     else if( DisplayMode ==2)
      {  LCD_string(1,1,"                ");
        LCD_string(2,1,"RH:");
	  }
      else if( DisplayMode ==3)
        {
        LCD_string(1,1," T:");
        LCD_string(2,1,"RH:");
       } 
  } 
  
  
void DisplayTitle_03D()
  { 
  	//都不激活的情况下
    if((VarList[cRealDataAct*2]&0x03)==0x03)  //t and rh  not active
     {   //湿度未激活显示 温度未激活显示  
     	 if((ParaList[(cTempValid03D-cParaActual)*2+1]==0x01)&&(ParaList[(cHumiValid03D-cParaActual)*2+1]==0X01))   //温度未激活显示
      	   { LCD_string(1,1," T:             ");
      	     LCD_string(2,1," H:             "); 
      	   } 
      	//湿度未激活显示 温度未激活不显示 
     	else if((ParaList[(cHumiValid03D-cParaActual)*2+1]==0X01)&&(ParaList[(cTempValid03D-cParaActual)*2+1]==0x02))
     	  {
     	    LCD_string(1,1," H:             ");
            LCD_string(2,1,"               "); 
     	  }
     	//湿度未激活不显示 温度未激活显示 
     	else if((ParaList[(cHumiValid03D-cParaActual)*2+1]==0X02)&&(ParaList[(cTempValid03D-cParaActual)*2+1]==0x01))
     	  {
     	    LCD_string(1,1," T:             ");
            LCD_string(2,1,"               "); 
     	  } 
     	  //湿度未激活不显示 温度未激活不显示  
     	else
     	   { LCD_string(1,1,"   Not Use      ");
             LCD_string(2,1,"                ");   
      	   } 
     	
        		  
     	
      }
   else  if((VarList[cRealDataAct*2]&0x03)==0x01)    //only active t  
      { //湿度不激活显示  
      	if(ParaList[(cHumiValid03D-cParaActual)*2+1]==0x01)
      	 {
      	     LCD_string(1,1," T:             ");
      	     LCD_string(2,1," H:             "); 
      	 }
      	// 湿度不激活不显示 
      	else
      	 
      	{  LCD_string(1,1," T:             ");
           LCD_string(2,1,"                "); 
        }  
      }
  
   else  if((VarList[cRealDataAct*2]&0x03)==0x02)     //only active th  
      { if(ParaList[(cTempValid03D-cParaActual)*2+1]==0x02)
         { LCD_string(1,1," H:             ");
           LCD_string(2,1,"                ");  
         }
        else
         { 
             LCD_string(1,1," T:             ");
      	     LCD_string(2,1," H:             "); 	
         }
      }    	            
   else   //ALL active
     {       LCD_string(1,1," T:             ");
      	     LCD_string(2,1," H:             "); 
      }   
     	 
    
   }      
  
  

//-----------------------------------------------------------------
//Function   :void DisplayTitle()
//Input      :none
//Output     :None
//Description:1,	在lcd屏上显示温度标题
//use IO     :
//-----------------------------------------------------------------
void DisplayTemp()
  {
    #ifdef _BL02D 
        #ifdef _Loramain
           DisplayTemp_Lora( );
        #else   	
           DisplayTemp_02D( );
       #endif
    #endif
     #ifdef _BL03D 	
        DisplayTemp_03D();
    #endif 
    
   
}

void   DisplayTemp_02D( )
   {
     unsigned char Dis[15]; 
     DisplayTitle();
     if((VarList[0]*256+VarList1[1]==65536-cInValidTemp)||(VarList[2]*256+VarList1[3]==65536-cInValidTemp))  
          {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
                  LCD_string(1,5,Dis);
                  LCD_string(2,5,Dis);
             
          return;
        }	   
    if( DisplayMode ==1)
     {  FloatConvert2ASCII(VarList[0]*256+VarList[1],Dis,0);
     //  LCD_string(1,5,"            ");
       LCD_string(1,5,Dis);
     }
   else if  ( DisplayMode ==2)  
     {  FloatConvert2ASCII(VarList[2]*256+VarList[3],Dis,1);
     //  LCD_string(2,5,"            ");
       LCD_string(2,5,Dis);
     
     }
   else if  ( DisplayMode ==3)    
    { FloatConvert2ASCII(VarList[0]*256+VarList[1],Dis,0);
     //  LCD_string(1,5,"            ");
       LCD_string(1,5,Dis);
        FloatConvert2ASCII(VarList[2]*256+VarList[3],Dis,1);
     //  LCD_string(2,5,"            ");
       LCD_string(2,5,Dis);
   }
   else ;
}

//------------------------------------------------------------
void   DisplayTemp_Lora( ) 
 {
    unsigned char Dis[15]; 
     DisplayTitle();
     if((VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+1]==65536-cInValidTemp)||(VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+2]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+3]==65536-cInValidTemp))
          {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
                  LCD_string(1,5,Dis);
                  LCD_string(2,5,Dis);
            	
          return;
					}  
    if( DisplayMode ==1)
     {  FloatConvert2ASCII(VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+1],Dis,0);
     //  LCD_string(1,5,"            ");
       LCD_string(1,5,Dis);
     }
   else if  ( DisplayMode ==2)  
     {  FloatConvert2ASCII(VarList[cTempStartAddr*2+(DisplayTurn-AddrStart)*4+2]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+3],Dis,1);
     //  LCD_string(2,5,"            ");
       LCD_string(2,5,Dis);
     
     }
   else if  ( DisplayMode ==3)    
    { FloatConvert2ASCII(VarList[cTempStartAddr*2+(DisplayTurn-AddrStart)*4]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+1],Dis,0);
     //  LCD_string(1,5,"            ");
       LCD_string(1,5,Dis);
        FloatConvert2ASCII(VarList[cTempStartAddr*2+(DisplayTurn-AddrStart)*4+2]*256+VarList[(cTempStartAddr*2)+(DisplayTurn-AddrStart)*4+3],Dis,1);
     //  LCD_string(2,5,"            ");
       LCD_string(2,5,Dis);
   }
   else ;
    	
 
}

//----------------------------------------------
void   DisplayTemp_03D()
   {
#if  0

     uchar   Dis[15]; 
     uint  TempVar;
     
      DisplayTitle();
   //都不激活的情况下
    if((VarList[cRealDataAct*2]&0x03)==0x03)  //t and rh  not active
     {   //湿度未激活显示 温度未激活显示  
     	 if((ParaList[(cTempValid03D-cParaActual)*2+1]==0x01)&&(ParaList[(cHumiValid03D-cParaActual)*2+1]==0X01))   //温度未激活显示
      	   {  //---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
      	   //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(2,4,Dis);  
      	
      	   } 
      	//湿度未激活显示 温度未激活不显示 
     	else if((ParaList[(cHumiValid03D-cParaActual)*2+1]==0X01)&&(ParaList[(cTempValid03D-cParaActual)*2+1]==0x02))
     	  { //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(1,4,Dis);  
     	    LCD_string(2,1,"               "); 
     	  }
     	//湿度未激活不显示 温度未激活显示 
     	else if((ParaList[(cHumiValid03D-cParaActual)*2+1]==0X02)&&(ParaList[(cTempValid03D-cParaActual)*2+1]==0x01))
     	  {//---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
     	     LCD_string(2,1,"               "); 
     	  } 
     	  //湿度未激活不显示 温度未激活不显示  
   
	  else
     	   { LCD_string(1,1,"   Not Use      ");
             LCD_string(2,1,"               ");   
      	   } 
     	
        		  
     	
      }
   else  if((VarList[cRealDataAct*2]&0x03)==0x01)    //only active t  
      { //湿度不激活显示  
      	if(ParaList[(cHumiValid03D-cParaActual)*2+1]==0x01)
      	 {
      	     //---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
      	   //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(2,4,Dis);  
      	 }
      	// 湿度不激活不显示 
      	else
      	 
      	{  //---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
            LCD_string(2,1,"                "); 
        }  
      }
  
   else  if((VarList[cRealDataAct*2]&0x03)==0x02)     //only active th  
      { if(ParaList[(cTempValid03D-cParaActual)*2+1]==0x02)
         { //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(1,4,Dis);  
           LCD_string(2,1,"               ");  
         }
        else
         { 
             //---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
      	   //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(2,4,Dis);  	
         }
      }    	            
   else   //ALL active
     {    //---------t-------------------
      	   	  TempVar=VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1];
     	      if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cTempValid03D-cParaActual)*2+1]!=0x00))
               {
                  Dis[0]='-';	
                  Dis[1]='-';
                  Dis[2]='-';
                  Dis[3]='-';
                  Dis[4]='-';
                  Dis[5]=' ';
                  Dis[6]=0;
              }	
             else
              FloatConvert2ASCII(VarList1[cTemp1*2]*256+VarList1[cTemp1*2+1],Dis,0);
      	      LCD_string(1,4,Dis);
      	   //-----rh--------------
      	    TempVar=VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1];
         if((TempVar==65536-cInValidTemp)||(TempVar==65536-cInValidTempNouse)||(ParaList[(cHumiValid03D-cParaActual)*2+1]!=0))
         {
           Dis[0]='-';	
           Dis[1]='-';
           Dis[2]='-';
           Dis[3]='-';
           Dis[4]='-';
           Dis[5]=' ';
           Dis[6]=0;
         }	
        else
         FloatConvert2ASCII(VarList1[cHumi1*2]*256+VarList1[cHumi1*2+1],Dis,1);
      	  LCD_string(2,4,Dis);  
      }   
#endif
			
    
   }      





       
 //--------------------------------------------------------
//Function:  void FloatConvert2ASCII(float floatVal,unsigned char *p)
//Input:    float floatVal:待转浮点数
//          unsigned char *p:转换为ascii码存放地址
//Output:   None
//Description: 1:显示当前的实时数据
//----------------------------------------------------------
void FloatConvert2ASCII(int16_t floatVal,unsigned char *p,unsigned char type)
  {
    int16_t   temp;
    unsigned char  i,flag=0;
   i=0;
   if(floatVal<0)
     {   p[i]='-'; //若浮点数小于0，则符号位标志fu_hao清0
         i++;
     } 
    temp=fabs(floatVal); 
   if((temp/10000)!=0)
      { p[i]=temp/10000+0x30;
        i++;
        flag=1;
      }
//--------------------      
   if(flag==1)
      {
      	  p[i]=temp%10000/1000+0x30;
      	  i++;
      }	  
   else
      {     
         if((temp%10000/1000)!=0)
            { p[i]=temp%10000/1000+0x30;
              i++;
              flag=1;
           }
    }
//-----------------------------    
//--------------------      
   if(flag==1)
      {
      	  p[i]=temp%1000/100+0x30;
      	  i++;
      }	  
   else
      {     
         if((temp%1000/100)!=0)
            { p[i]=temp%1000/100+0x30;
              i++;
              flag=1;
           }
    }
//-----------------------------       

      	  p[i++]=temp%100/10+0x30;
//-----------------------------       
     p[i++]='.';
//-----------------------------    
    p[i++]=temp%10+0x30;
  if(floatVal==-cInValidTemp)
   {
     i=0;	
      p[i++]='-';	
      p[i++]='-';
      p[i++]='-';
      p[i++]='-';
     
   }	
  
   
   if( type==1)
    { p[i++]='%';
      p[i++]=' ';
    }  
  else
    { 
    	p[i++]=223;
    	p[i++]='C';
    }	
//-----------------------------   
     
       p[i++]=' ';
        p[i++]=' ';
         p[i++]=' ';
          p[i++]=' ';
          
//----------------------------      
      p[i]=0;
  }




//modified 2016-11-8 12:07






//--------------------------------------------------------
//Function:  void DisplayNo2(unsigned char m,unsigned char DeviceNum)
//Input:     unsigned char m,unsigned char DeviceNum
//Output:   None
//Description: 1:闪烁设备编号
//----------------------------------------------------------
void DisplayNoFlash(unsigned char m,unsigned char DeviceNum)
 {
 	unsigned char Dis[4];  
 	if((m%2)==0)
 	 {LCD_string(2,11,"NO:");
 	  Dis[0]=DeviceNum/100+0X30;
 	  Dis[1]=(DeviceNum%100)/10+0X30;
 	  Dis[2]=DeviceNum%10+0X30;
 	   Dis[3]=0;
          LCD_string(2,14,Dis);
        }
      else  
 	LCD_string(2,11,"      ");
}
//--------------------------------------------------------
//Function:  void DisplayNo1(uchar Pos)
//Input:     uchar Pos:显示位置
//Output:   None
//Description: 1:在指定位置显示设备编号
//----------------------------------------------------------
void DisplayNo_fixPos(uchar Pos)
 {	uchar  Dis[4];  
        Dis[0]=DeviceNum/100+0X30;
 	Dis[1]=(DeviceNum%100)/10+0X30;
 	Dis[2]=DeviceNum%10+0X30;
 	Dis[3]=0;
        LCD_string(2,Pos,Dis);//	
 	
 }	
//--------------------------------------------------------
//Function:  void DisplayNoTitle()
//Input:     None
//Output:   None
//Description: 1:在第二行显示编号title
//----------------------------------------------------------
void DisplayNoTitle()
 {
 	
 	LCD_string(2,0,"Device NO:");  //2015-08-13 11:13
 	DisplayNo_fixPos(11);
 	
}

void TestLcdPin()
 {
 	
 ClearRs();
 SetRs();
 ClearRW();
 SetRW();
 ClearCE();
 SetCE();
 PC2=0;
 PC2=1; 
 PC3=0;
 PC3=1;
 PD15=0;
 PD15=1; 
 PD14=0;
 PD14=1; 
  PD7=0;
 PD7=1; 
 PD6=0;
 PD6=1; 
  PB3=0;
 PB3=1; 
 PB2=0;
 PB2=1;
 
}
 
//--------------------------------------------------------
//Function:  oid DisplayTime2(unsigned char *VarList)
//Input:     unsigned char *VarList:月日时分秒
//Output:   None
//Description: 1:显示时间,byte顺序显示
//---------------------------------------------------------- 
void DisplayTime_Byte(unsigned char *Var)
 {
    unsigned char   Dis[4];     
   
         LCD4_Clear(); 
        Dis[0]=(Var[1]%100)/10+0X30;
 	Dis[1]=Var[1]%10+0X30;
 	Dis[2]='-';
 	Dis[3]=0;
        LCD_string(1,4-3,Dis);	
        
        Dis[0]=(Var[2]%100)/10+0X30;
 	Dis[1]=Var[2]%10+0X30;
 	Dis[2]=' ';
 	Dis[3]=0;
        LCD_string(1,7-3,Dis);
        
        Dis[0]=(Var[3]%100)/10+0X30;
 	Dis[1]=Var[3]%10+0X30;
 	Dis[2]=':';
 	Dis[3]=0;
        LCD_string(1,10-3,Dis);
        
        Dis[0]=(Var[4]%100)/10+0X30;
 	Dis[1]=Var[4]%10+0X30;
 	Dis[2]=':';
 	Dis[3]=0;
        LCD_string(1,13-3,Dis);	
 	Dis[0]=(Var[5]%100)/10+0X30;
 	Dis[1]=Var[5]%10+0X30;
 	Dis[2]=0;
 	LCD_string(1,13,Dis);	

        DisplayNo_fixPos(0);
     }   
//--------------------------------------------------------
//Function:  oid DisplayTime3(unsigned char *VarList)
//Input:     unsigned char *VarList:月日时分秒
//Output:   None
//Description: 1:显示时间,Word顺序显示
//----------------------------------------------------------     
void DisplayTime_Word()
 {
    uchar   Dis[4];     
    /*   
        Dis[0]=(VarList[1]%100)/10+0X30;
 	Dis[1]=VarList[1]%10+0X30;
 	Dis[2]='-';
 	Dis[3]=0;
        LCD_string(1,1,Dis); */	
         LCD4_Clear(); 
        Dis[0]=(VarList[cTimeVarOffset*2+3]%100)/10+0X30;
 	Dis[1]=VarList[cTimeVarOffset*2+3]%10+0X30;
 	Dis[2]='-';
 	Dis[3]=0;
        LCD_string(1,4-3,Dis);	
        
        Dis[0]=(VarList[cTimeVarOffset*2+5]%100)/10+0X30;
 	Dis[1]=VarList[cTimeVarOffset*2+5]%10+0X30;
 	Dis[2]=' ';
 	Dis[3]=0;
        LCD_string(1,7-3,Dis);
        
        Dis[0]=(VarList[cTimeVarOffset*2+7]%100)/10+0X30;
 	Dis[1]=VarList[cTimeVarOffset*2+7]%10+0X30;
 	Dis[2]=':';
 	Dis[3]=0;
        LCD_string(1,10-3,Dis);
        
        Dis[0]=(VarList[cTimeVarOffset*2+9]%100)/10+0X30;
 	Dis[1]=VarList[cTimeVarOffset*2+9]%10+0X30;
 	Dis[2]=':';
 	Dis[3]=0;
        LCD_string(1,13-3,Dis);	
 	Dis[0]=(VarList[cTimeVarOffset*2+11]%100)/10+0X30;
 	Dis[1]=VarList[cTimeVarOffset*2+11]%10+0X30;
 	Dis[2]=0;
 	LCD_string(1,13,Dis);	

       DisplayNo_fixPos(0);
     }  
     
 //modifed 2021-03-29 17:41 
 
 
//--------------------------------------------------------
//Function:  void      DisplayInfor()
//Input:     None
//Output:   None
//Description: 1:显示相关信息
//----------------------------------------------------------     
    
 void      DisplayInfor(uint8_t m)    	
  {   
  	 #ifdef _BL02D 
              #ifdef _Loramain
                  DisplayInfor_Lora(m );
                #else   	
                   DisplayInfor_02D(m );
               #endif
         #endif
     
     #ifdef _BL03D 	
        DisplayInfor_03D(m);
    #endif 
  }
	
void  DisplayInfor_02D(uint8_t  m) 
  {
       DisplayNoFlash(m%2,DeviceNum);	
  	 
  }
void  DisplayInfor_Lora(uint8_t  m) 
  {    DisplayNoFlash(0,DisplayTurn);
       
  }

void    DisplayInfor_03D(uint8_t  m)

  {  
     static unsigned char   FlashFlag;
        FlashFlag^=1;  
    if(FlashFlag==1)
       LCD_string(1,13,"    ");
    else
       {    
  if(VarList[21]>=80)
 	LCD_string(1,13,"****");
  else if (VarList[21]>=65)
        LCD_string(1,13," ***"); 
  else if (VarList[21]>=45)
        LCD_string(1,13,"  **");
  else if (VarList[21]>=25)
        LCD_string(1,13,"   *");              
  else 
      {  LCD_string(1,13,"   -");   
        //add 2021-05-15 11:20
       // PowerDownFunction();
       while(1)
         {if(PB14==1)
           {  Enter_PowerDown();
              Leave_PowerDown();
              if(PB14==1)  //无外电
              {
                if(WakeUp_CheckPowerVolume()>=50)  //电池电量够
                 break;
             }
            else
               break;
                
           } 
        else
           break;     
      
   }


       if(ParaList[(cSaveDataFlag03D-cParaActual)*2+1]==0) 
        { LCD_string(2,14,"Rec");
         
        } 
       else
        { LCD_string(2,14,"   ");  
		
        }		  
  }


}
}

//--------------------------------------------------------
//Function:  void     DisplayPage()
//Input:     None
//Output:   None
//Description: 1:显示存储页信息
//----------------------------------------------------------     
void      DisplayPage()
{  uchar   Dis[6];     

	LCD4_Clear(); 
 	LCD_string(1,1,"Page:");
 	
 	
 	
 	  Dis[0]=CurrentWritePage/10000+0x30;
 	  Dis[1]=CurrentWritePage%10000/1000+0x30;
 	  Dis[2]=CurrentWritePage%1000/100+0x30; 
 	  Dis[3]=CurrentWritePage%100/10+0x30;
 	  Dis[4]=CurrentWritePage%10+0X30;
 	 
 	  Dis[5]=0;
         LCD_string(1,6,Dis);
         
         if(GetSensorHeat())
            LCD_string(1,13,"Hon ");
         else
            LCD_string(1,13,"Hoff"); 
         
         if(GetSensoPower())
            LCD_string(2,13,"Pend");
         else
            LCD_string(2,13,"Pnor");
        
   
   
           LCD_string(2,1,"Rec:");
           Dis[0]=CurrentReordNum/100+0x30;
 	   Dis[1]=CurrentReordNum%100/10+0x30;
 	   Dis[2]=CurrentReordNum%10+0X30;
 	   Dis[3]=0;
          LCD_string(2,5,Dis);
    
  }


//--------------------------------------------------------
//Function:  void     DisplayPage()
//Input:     None
//Output:   None
//Description: 1:显示存储页信息
//----------------------------------------------------------   
void DisplayStatus()
  { uchar    TempDisDat[17];
     if((VarList[cRealDataAct*2]&0x80)==0x00)
        TempDisDat[0]='0';
     else
        TempDisDat[0]='1'; 
  
    if((VarList[cRealDataAct*2]&0x40)==0x00)
        TempDisDat[1]='0';
     else
        TempDisDat[1]='1'; 
  	
  if((VarList[cRealDataAct*2]&0x20)==0x00)
        TempDisDat[2]='0';
     else
        TempDisDat[2]='1';	
 
  if((VarList[cRealDataAct*2]&0x10)==0x00)
        TempDisDat[3]='0';
     else
        TempDisDat[3]='1';
        
        	
 if((VarList[cRealDataAct*2]&0x08)==0x00)
        TempDisDat[4]='0';
     else
        TempDisDat[4]='1';
 
    TempDisDat[5]=' ';
    TempDisDat[6]=' ';
    TempDisDat[7]=' ';
    
    if((VarList[cRealDataAct*2+1]&0x80)==0x00)
        TempDisDat[8]='0';
     else
        TempDisDat[8]='1'; 
  
    if((VarList[cRealDataAct*2+1]&0x40)==0x00)
        TempDisDat[9]='0';
     else
        TempDisDat[9]='1'; 
  	
  if((VarList[cRealDataAct*2+1]&0x20)==0x00)
        TempDisDat[10]='0';
     else
        TempDisDat[10]='1';	
 
  if((VarList[cRealDataAct*2+1]&0x10)==0x00)
        TempDisDat[11]='0';
     else
        TempDisDat[11]='1';
        
        	
 if((VarList[cRealDataAct*2+1]&0x08)==0x00)
        TempDisDat[12]='0';
     else
        TempDisDat[12]='1';
        
        
   if((VarList[cRealDataAct*2+1]&0x04)==0x00)
        TempDisDat[13]='0';
     else
        TempDisDat[13]='1';     
        
   if((VarList[cRealDataAct*2+1]&0x02)==0x00)
        TempDisDat[14]='0';
     else
        TempDisDat[14]='1';   
 
   if((VarList[cRealDataAct*2+1]&0x01)==0x00)
        TempDisDat[15]='0';
     else
        TempDisDat[15]='1';  
 
       TempDisDat[16]=0; 
    
 LCD_string(1,1,TempDisDat);
 //------------------------------------------
 if((VarList[cRealDataAct*2+2]&0x80)==0x00)
        TempDisDat[0]='0';
     else
        TempDisDat[0]='1'; 
  
    if((VarList[cRealDataAct*2+2]&0x40)==0x00)
        TempDisDat[1]='0';
     else
        TempDisDat[1]='1'; 
  	
  if((VarList[cRealDataAct*2+2]&0x20)==0x00)
        TempDisDat[2]='0';
     else
        TempDisDat[2]='1';	
 
  if((VarList[cRealDataAct*2+2]&0x10)==0x00)
        TempDisDat[3]='0';
     else
        TempDisDat[3]='1';
        
        	
 if((VarList[cRealDataAct*2+2]&0x08)==0x00)
        TempDisDat[4]='0';
     else
        TempDisDat[4]='1';
        
        
   if((VarList[cRealDataAct*2+2]&0x04)==0x00)
        TempDisDat[5]='0';
     else
        TempDisDat[5]='1';     
        
   if((VarList[cRealDataAct*2+2]&0x02)==0x00)
        TempDisDat[6]='0';
     else
        TempDisDat[6]='1';   
 
   if((VarList[cRealDataAct*2+2]&0x01)==0x00)
        TempDisDat[7]='0';
     else
        TempDisDat[7]='1';  
       TempDisDat[8]=' ';
       TempDisDat[9]=' ';
       TempDisDat[10]=' ';
       TempDisDat[11]=' ';
       TempDisDat[12]=' ';
       TempDisDat[13]=' ';
       TempDisDat[14]=' ';
       TempDisDat[15]=' ';
       TempDisDat[16]=' ';
        
       TempDisDat[17]=0; 
 LCD_string(2,1,TempDisDat);
 //------------------------------------------
}     



//--------------------------------------------------------
//Function:  void     void DisBase()
//Input:     None
//Output:   None
//Description: 1:显示存储页信息
//----------------------------------------------------------   
void DisBase()
    { unsigned char CountValid=0;
     // DisplayTurn++;			
 #ifdef _Loramain   
			while(1)
    	     {  
    	         DisplayTurn++;
						 
               if(DisplayTurn>=AddrLen+AddrStart)
                 DisplayTurn=AddrStart; 
             if(  ParaList[(cLoraNodeAliveSet+DisplayTurn-1)*2+1]==1)
                {  // 
     	            DisplayTemp(); 
		              DisplayInfor(DisplayTurn);
		            break;
		         } 
		        else
		          { CountValid++;
		             if(CountValid>=16)
		                  break;
		           }        
	        }
#else
   		           DisplayTemp(); 
		              DisplayInfor(DisplayTurn);			
#endif
					
		   
	 }  

void  ReceiveTest()
{
	if( GetRfDataFlag==1)
	{    GetRfDataFlag=0;
		   Bellcontrol(3,10,10);
//		AlarmLEDLEDLED(3,10,10);
		   DisplayTurn=GetRfnode;	
		DisplayTemp(); 
		DisplayInfor(DisplayTurn);
	}
}	
/*---------------------------------------------------------------------------------
Function: LcdDisplay()
Input:
OutPut:
Description: 显示lcd上的内容,DisBase();DisplayTime3();DisplayPage(); 
-----------------------------------------------------------------------------------*/   
	 void LcdDisplay_03D()
    {   static  unsigned char stage;
    	         stage++;
    	           if((stage%10)==0)
		            LCD_init();
		  
     	         if(ParaList[(cTestMode03D-cParaActual)*2+1]==0) 
     	             DisBase();
     	        //-----------------------------------
     	         else  if(ParaList[(cTestMode03D-cParaActual)*2+1]==1) 
     	         {    
		     
		   if(stage==1)  
		      DisplayTime_Word();  
		 
		  else if(stage==2) 
     	              DisplayPage();	
     	          else  if(stage>=3)
     	             {  DisBase();
     	               stage=0;   	     
     	             }
     	          }   
     	         else  if(ParaList[(cTestMode03D-cParaActual)*2+1]==2) 
     	             DisplayTime_Word();     
 	         //---------------------------------------------------  
     	         else  if(ParaList[(cTestMode03D-cParaActual)*2+1]==3) 
     	            DisplayPage();
     	              
     	              
 }   	


	 void LcdDisplay()
	 {
		  
#ifdef _BL02D 
            DisBase();
         #endif	 
	 #ifdef _BL03D 	
        LcdDisplay_03D();
    #endif
	 }		 

           