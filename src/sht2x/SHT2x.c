//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  SHT2x.c
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Sensor layer. Functions for sensor access
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#include "shx.h"
#define _GLOBAL_H
#include	"global.h"
#include <math.h>

extern void  SendByte(unsigned char Dat);
float filterT();
float filterRH();


#define POLYNOMIAL  0x131 //P(x)=x^8+x^5+x^4+1 = 100110001
#define   TRIG_T_MEASUREMENT_HM    0xE3 // command trig. temp meas. hold master
#define   TRIG_RH_MEASUREMENT_HM   0xE5 // command trig. humidity meas. hold master
#define   TRIG_T_MEASUREMENT_POLL  0xF3 // command trig. temp meas. no hold master
#define   TRIG_RH_MEASUREMENT_POLL 0xF5 // command trig. humidity meas. no hold master
#define   USER_REG_W               0xE6 // command writing user register
#define   USER_REG_R               0xE7 // command reading user register
#define   SOFT_RESET               0xFE  // command soft reset



#define   SHT2x_RES_12_14BIT       0x00 // RH=12bit, T=14bit
#define   SHT2x_RES_8_12BIT        0x01 // RH= 8bit, T=12bit
#define   SHT2x_RES_10_13BIT        0x80 // RH=10bit, T=13bit
#define   SHT2x_RES_11_11BIT       0x81 // RH=11bit, T=11bit
#define   SHT2x_RES_MASK           0x85  // Mask for res. bits (7,0) in user reg.



#define   SHT2x_EOB_ON             0x40 // end of battery
#define   SHT2x_EOB_MASK           0x40 // Mask for EOB bit(6) in user reg.


#define   SHT2x_HEATER_ON          0x04 // heater on
#define   SHT2x_HEATER_OFF         0x00 // heater off
#define   SHT2x_HEATER_MASK        0x04 // Mask for Heater bit(2) in user reg.


// measurement signal selection
#define HUMIDITYsta    0
#define  TEMPsta       1

#define  I2C_ADR_W                 128   // sensor I2C address + write bit
#define  I2C_ADR_R                129    // sensor I2C address + read bit

#if 1
struct SHT_XX
   {  
unsigned char  error;        //用来计算在获取STH21的过程中是否出现了错误的状态
float   temperature;  //保存的温.湿度值 
float   Humidity;  //保存的温.湿度值 
float   T_bak1[5];
float   HR_bak1[5];
float   T_bak2[5];
float   HR_bak2[5];
unsigned char TFlag;
uint16_t   HumiInt;     //读取湿度的模拟值
uint16_t   TempInt;     //读取温度的模拟值
unsigned char WrongCount;
unsigned char    SensorStatus;
unsigned char    userRegister;
unsigned char   SensorHeatFlag; 
unsigned char   SensorPowerEnd;  	 
   }; 
    
struct SHT_XX   SHT20;  


#endif

unsigned char GetSensorHeat()
  {
     return(SHT20.SensorHeatFlag);
}   
   
   
unsigned char GetSensoPower()
  {
     return(SHT20.SensorPowerEnd);
}      
   


unsigned char GetSensorStatus()
  {
  	return(SHT20.SensorStatus);
  }	
//==============================================================================

void DelayMicroSeconds (uint16_t nbrOfUs)
//==============================================================================
{uint16_t i;
  for( i=0;i<nbrOfUs;i++)
     WatchdogReset();
//  _nop_;

}	
//==============================================================================
unsigned char SHT2x_CheckCrc(unsigned char  dat[], unsigned char  nbrOfBytes, unsigned char  checksum)
//==============================================================================
{
  unsigned char crc = 0;
  unsigned char byteCtr;
  unsigned char Tempchar;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {  crc ^= (dat[byteCtr]);
    for ( Tempchar = 8;  Tempchar> 0; -- Tempchar)
      { if (crc & 0x80)
          crc = (crc << 1) ^ POLYNOMIAL;
      else
           crc = (crc << 1);
    }
  }
  if (crc != checksum)
     return CHECKSUM_ERROR;
  else
     return 0;
}

//===========================================================================
unsigned char SHT2x_ReadUserRegister()
//===========================================================================
{
  unsigned char checksum;   //variable for checksum byte
  //unsigned char SHT20.error=0;    //variable for SHT20.error code
  
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W);
  SHT20.error |= I2c_WriteByte (USER_REG_R);
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_R);
  SHT20.userRegister = I2c_ReadByte(ACK);
  checksum=I2c_ReadByte(NO_ACK);
  SHT20.error |= SHT2x_CheckCrc (&SHT20.userRegister,1,checksum);
  I2c_StopCondition();
  return (SHT20.error);  
}

//===========================================================================
unsigned char SHT2x_WriteUserRegister(unsigned char pRegisterValue)
//===========================================================================
{
  //unsigned char SHT20.error=0;   //variable for SHT20.error code

  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W);
  SHT20.error |= I2c_WriteByte (USER_REG_W);
  SHT20.error |= I2c_WriteByte (pRegisterValue);
  I2c_StopCondition();
  return SHT20.error;
}

//===========================================================================
unsigned char SHT2x_MeasureHM(unsigned char MeasuerType)
//===========================================================================
{
  unsigned char  checksum,dat[2];   //checksum
 // unsigned char  dat[2];    //data array for checksum verification
 // unsigned char  SHT20.error=0;    //SHT20.error variable
  uint16_t i;          //counting variable

  //-- write I2C sensor address and command --
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  switch(MeasuerType)
  { case HUMIDITYsta: SHT20.error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_HM); break;
    case TEMPsta    : SHT20.error |= I2c_WriteByte (TRIG_T_MEASUREMENT_HM);  break;
    default: break;
  }
  //-- wait until hold master is released --
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_R);
  SCL=HIGH;                     // set SCL I/O port as input
  for(i=0; i<1000; i++)         // wait until master hold is released or
  { DelayMicroSeconds(1000);    // a timeout (~1s) is reached
    if (SCL==1)
        break;
  }
  //-- check for timeout --
  if(SCL==0)
      SHT20.error |= TIME_OUT_ERROR;

  //-- read two data bytes and one checksum byte --
   dat[0]= I2c_ReadByte(ACK);
   dat[1]= I2c_ReadByte(ACK);
  i =dat[0]*256+dat[1];
  checksum=I2c_ReadByte(NO_ACK);

  switch(MeasuerType)
  { case HUMIDITYsta: SHT20.HumiInt=i; break;
    case TEMPsta    : SHT20.TempInt=i; break;
    default: break;
  }
  //-- verify checksum --
  SHT20.error |= SHT2x_CheckCrc (dat,2,checksum);
  I2c_StopCondition();
  return SHT20.error;
}


//===========================================================================
unsigned char SHT2x_MeasurePoll(unsigned char MeasuerType)
//===========================================================================
{
  unsigned char  checksum,dat[2];   //checksum
 // unsigned char  dat[2];    //data array for checksum verification
 // unsigned char  SHT20.error=0;    //SHT20.error variable
  uint16_t i=0;        //counting variable

  //-- write I2C sensor address and command --
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  switch(MeasuerType)
  { case HUMIDITYsta: SHT20.error |= I2c_WriteByte (TRIG_RH_MEASUREMENT_POLL); break;
    case TEMPsta    : SHT20.error |= I2c_WriteByte (TRIG_T_MEASUREMENT_POLL);  break;
    default: break;
  }
  //-- poll every 10ms for measurement ready. Timeout after 20 retries (200ms)--
  do
     {
     	 I2c_StartCondition();
         DelayMicroSeconds(10000);  //delay 10ms
        if(i++ >= 20)
         break;
     }
  while(I2c_WriteByte (I2C_ADR_R) == ACK_ERROR);
  if (i>=20)
      SHT20.error |= TIME_OUT_ERROR;

  //-- read two data bytes and one checksum byte --
   dat[0]= I2c_ReadByte(ACK);
   dat[1]= I2c_ReadByte(ACK);
  i =dat[0]*256+dat[1];
  checksum=I2c_ReadByte(NO_ACK);
  switch(MeasuerType)
  { case HUMIDITYsta: SHT20.HumiInt=i; break;
    case TEMPsta    : SHT20.TempInt=i; break;
    default: break;
  }
  //-- verify checksum --
  SHT20.error |= SHT2x_CheckCrc (dat,2,checksum);
  I2c_StopCondition();

  return SHT20.error;
}

//===========================================================================
unsigned char SHT2x_SoftReset()
//===========================================================================
{
 // unsigned char  SHT20.error=0;           //SHT20.error variable

  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W); // I2C Adr
  SHT20.error |= I2c_WriteByte (SOFT_RESET);                            // Command
  I2c_StopCondition();
  DelayMicroSeconds(15000); // wait till sensor has restarted
  return SHT20.error;
}

//==============================================================================
float SHT2x_CalcRH(uint16_t u16sRH)
//==============================================================================
{
  float humidityRH;              // variable for result
  u16sRH &= ~0x0003;          // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --

  humidityRH = -6.0 + 125.0/65536 *( (float)u16sRH); // RH= -6 + 125 * SRH/2^16
  return humidityRH;
}

//==============================================================================
float SHT2x_CalcTemperatureC(uint16_t u16sT)
//==============================================================================
{
  float temperatureC;            // variable for result

  u16sT &= ~0x0003;           // clear bits [1..0] (status bits)

  //-- calculate temperature [癈] --
  temperatureC= -46.85 + 175.72/65536 *((float)u16sT); //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}

//==============================================================================
unsigned char SHT2x_GetSerialNumber(unsigned char u8SerialNumber[])
//==============================================================================
{
  //unsigned char  SHT20.error=0;                          //SHT20.error variable

  //Read from memory location 1
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  SHT20.error |= I2c_WriteByte (0xFA);         //Command for readout on-chip memory
  SHT20.error |= I2c_WriteByte (0x0F);         //on-chip memory address
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  u8SerialNumber[5] = I2c_ReadByte(ACK); //Read SNB_3
  I2c_ReadByte(ACK);                     //Read CRC SNB_3 (CRC is not analyzed)
  u8SerialNumber[4] = I2c_ReadByte(ACK); //Read SNB_2
  I2c_ReadByte(ACK);                     //Read CRC SNB_2 (CRC is not analyzed)
  u8SerialNumber[3] = I2c_ReadByte(ACK); //Read SNB_1
  I2c_ReadByte(ACK);                     //Read CRC SNB_1 (CRC is not analyzed)
  u8SerialNumber[2] = I2c_ReadByte(ACK); //Read SNB_0
  I2c_ReadByte(NO_ACK);                  //Read CRC SNB_0 (CRC is not analyzed)
  I2c_StopCondition();

  //Read from memory location 2
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_W);    //I2C address
  SHT20.error |= I2c_WriteByte (0xFC);         //Command for readout on-chip memory
  SHT20.error |= I2c_WriteByte (0xC9);         //on-chip memory address
  I2c_StartCondition();
  SHT20.error |= I2c_WriteByte (I2C_ADR_R);    //I2C address
  u8SerialNumber[1] = I2c_ReadByte(ACK); //Read SNC_1
  u8SerialNumber[0] = I2c_ReadByte(ACK); //Read SNC_0
  I2c_ReadByte(ACK);                     //Read CRC SNC0/1 (CRC is not analyzed)
  u8SerialNumber[7] = I2c_ReadByte(ACK); //Read SNA_1
  u8SerialNumber[6] = I2c_ReadByte(ACK); //Read SNA_0
  I2c_ReadByte(NO_ACK);                  //Read CRC SNA0/1 (CRC is not analyzed)
  I2c_StopCondition();

  return SHT20.error;
}




//-----------------------------------------------------
void  LoopForSth21_sub(void)
  {	unsigned char i;
 //	unsigned char i,j;
        int16_t  temp;  //add 2014-02-21 11:42
        uint16_t   temp1;
        int16_t   temp2;
        
        float  floatTRh;

         SHT20.error=0;
   // --- Reset sensor by command ---
    SHT20.error |= SHT2x_SoftReset();
    WatchdogReset();
    // --- Read the sensors serial number (64bit) ---
  //  SHT20.error |= SHT2x_GetSerialNumber(SerialNumber_SHT2x);

    // --- Set Resolution e.g. RH 10bit, Temp 13bit ---
     SHT20.error |= SHT2x_ReadUserRegister();   //get actual user reg
#if 1
      if(SHT20.userRegister&SHT2x_HEATER_MASK==SHT2x_HEATER_ON)
      SHT20.SensorHeatFlag=1;
    else
      SHT20.SensorHeatFlag=0; 
      
   if(SHT20.userRegister&SHT2x_EOB_MASK==SHT2x_EOB_ON)
      SHT20.SensorPowerEnd=2;
    else
      SHT20.SensorPowerEnd=0;    
    VarList[7]=SHT20.SensorPowerEnd+SHT20.SensorHeatFlag; 
#endif 
    WatchdogReset();
    SHT20.userRegister = (SHT20.userRegister & (~SHT2x_RES_MASK)) |SHT2x_RES_12_14BIT;//SHT2x_RES_8_12BIT;//SHT2x_RES_10_13BIT;    //
    WatchdogReset();
    SHT20.error |= SHT2x_WriteUserRegister(SHT20.userRegister); //write changed user reg
    WatchdogReset();
    SHT20.userRegister=0xff;
    // --- measure humidity with "Hold Master Mode (HM)"  ---
    SHT20.error |= SHT2x_MeasureHM(HUMIDITYsta);
    WatchdogReset();
    // --- measure temperature with "Polling Mode" (no hold master) ---
    SHT20.error |= SHT2x_MeasurePoll(TEMPsta);
    WatchdogReset();
    //-- calculate humidity and temperature --
    SHT20.temperature = SHT2x_CalcTemperatureC(SHT20.TempInt);
    WatchdogReset();
    SHT20.Humidity   = SHT2x_CalcRH(SHT20.HumiInt);
    WatchdogReset();
 //add 2014-02-21 11:36   

 
 
 
 #if 1
 //---------ok value
 if   (SHT20.error==0)   //||(SHT20.error==0x04))
   {   SHT20.SensorStatus=1;
   	SHT20.WrongCount=0;

 //-----------------------------  
   if(SHT20.TFlag<5)
     { SHT20.T_bak2[SHT20.TFlag]= SHT20.temperature;
       SHT20.HR_bak2[SHT20.TFlag]= SHT20.Humidity;
	SHT20.TFlag++;
	for(i=SHT20.TFlag;i<5;i++)
	 { SHT20.T_bak2[i]= SHT20.temperature;
           SHT20.HR_bak2[i]= SHT20.Humidity;
	 }  
	   
     }
   else	
    {
	SHT20.T_bak2[0]=SHT20.T_bak2[1];
	SHT20.T_bak2[1]=SHT20.T_bak2[2];
	SHT20.T_bak2[2]=SHT20.T_bak2[3];
	SHT20.T_bak2[3]=SHT20.T_bak2[4];
	SHT20.T_bak2[4]=SHT20.temperature;
	
	SHT20.HR_bak2[0]=SHT20.HR_bak2[1];
	SHT20.HR_bak2[1]=SHT20.HR_bak2[2];
	SHT20.HR_bak2[2]=SHT20.HR_bak2[3];
	SHT20.HR_bak2[3]=SHT20.HR_bak2[4];
	SHT20.HR_bak2[4]=SHT20.Humidity;
      }
//--------------------------------------	
	
	SHT20.temperature=filterT();
	SHT20.Humidity=filterRH();	  
 
 
 //-----------------T-----------   
       temp=(int16_t)(fabs(SHT20.temperature)*10);
       if(SHT20.temperature<0)
       temp=-temp;
       temp2=(int16_t)(ParaList[24]*256+ParaList[25]);
       temp=temp+temp2;
       i=(temp+400)/100;
        temp2=(int16_t)(ParaList[(cTempOff1+i)*2]*256+ParaList[(cTempOff1+i)*2+1]);
       temp=temp+temp2;
     
        temp2=(int16_t)(ParaList[cTempError*2]*256+ParaList[cTempError*2+1]);
        temp=temp+temp2;

 
  	BL02D_Special_Temp(temp);


 //-----------------RH-------------------------
        temp=(int16_t)(fabs(SHT20.Humidity)*10);
        temp2=(int16_t)(ParaList[26]*256+ParaList[27]);
	 temp=temp+temp2;
	 
	 i=(temp)/100;
        temp2=(int16_t)(ParaList[(cHumiOff1+i)*2]*256+ParaList[(cHumiOff1+i)*2+1]);
       temp=temp+temp2; 
 
        
        temp2=(int16_t)(ParaList[cHumiError*2]*256+ParaList[cHumiError*2+1]);
        temp=temp+temp2;
  
     if(temp<0)
      temp=0;
     if(temp>990)    //modified  2016-10-10 10:03
      temp=990;


//add 2020-04-11 10:32  ----------------RH----AD----------
	BL02D_Special_Humi(temp);
       
  //---------------------------------       
  
  }  

  else    //wrong  value
   {
   	
   	SHT20.WrongCount++;
   	if(SHT20.WrongCount>=10)
   	 {  SHT20.SensorStatus=0;
   	    SHT20.WrongCount=0;
   	     temp=-cInValidTemp;
   	     temp1=(uint16_t)temp;
   	       VarList[cTemp*2]=temp1/256;
               VarList[cTemp*2+1]=temp1%256;
               VarList[cHumi*2]=temp1/256;
               VarList[cHumi*2+1]=temp1%256;
               VarList1[cTemp1*2]=temp1/256;
               VarList1[cTemp1*2+1]=temp1%256;
               VarList1[cHumi1*2]=temp1/256;
               VarList1[cHumi1*2+1]=temp1%256;
                  
            
            
     } 
  //---end of  deal wrong  
  } 
#endif 
 } 
  








//add 2014-10-06 20:43
float filterT()
{
unsigned char  i,j;
float     temp;

for (j=0;j<5;j++)
   SHT20.T_bak1[j]=SHT20.T_bak2[j];
for (j=0;j<4;j++)
{
  for (i=0;i<4-j;i++)
   {
   if ( SHT20.T_bak1[i]>SHT20.T_bak1[i+1] )
    {
      temp = SHT20.T_bak1[i];
      SHT20.T_bak1[i] = SHT20.T_bak1[i+1]; 
      SHT20.T_bak1[i+1] = temp;
    }
   }
   }
return SHT20.T_bak1[2];
}   


float filterRH()
{
unsigned char  i,j;
float    temp;

for (j=0;j<5;j++)
   SHT20.HR_bak1[j]=SHT20.HR_bak2[j];
for (j=0;j<4;j++)
{
  for (i=0;i<5-j;i++)
   {
   if ( SHT20.HR_bak1[i]>SHT20.HR_bak1[i+1] )
    {
      temp = SHT20.HR_bak1[i];
      SHT20.HR_bak1[i] = SHT20.HR_bak1[i+1]; 
      SHT20.HR_bak1[i+1] = temp;
    }
   }
   }
return SHT20.HR_bak1[2];
}   


void  LoopForSth21(void)
 {
 	
 #ifndef _Loramain 	
 	  LoopForSth21_sub();
  #endif	  

}


