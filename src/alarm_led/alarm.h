#ifndef __LED_H
#define __LED_H	 

#define  u8     unsigned char
#define  u16    uint16_t

#define  cLEDIdle   0
#define  cLEDOn     1
#define  cLEDOff    2


void AlarmLEDInital();
void AlarmLEDOff();
void AlarmLEDOn();
void AlarmLEDWaitDelay();
void AlarmLEDLEDLED(u8 Times,u16 DuringOn,u16 DuringOff);
void AlarmLEDcontrol(u8  times, u16 Ondelays,u16 Offdelays );
void Timer_AlarmLEDControl();
void StatusMachine_AlarmLEDLED();
		 				    
#endif
