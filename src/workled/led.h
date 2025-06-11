#ifndef __LED_H
#define __LED_H	 

#define  u8     unsigned char
#define  u16    uint16_t

#define  cLEDIdle   0
#define  cLEDOn     1
#define  cLEDOff    2


void  LEDInital();
void LEDOff();
void LEDOn();
void LEDWaitDelay();
void LEDLEDLED(u8 Times,u16 DuringOn,u16 DuringOff);
void LEDcontrol(u8  times, u16 Ondelays,u16 Offdelays );
void Timer_LEDControl();
void StatusMachine_LEDLED();
		 				    
#endif
