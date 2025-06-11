#ifndef RTC_H
#define RTC_H

#include "Nano100Series.h"

// RTC 时间结构
typedef struct
{
    uint32_t u32Year;
    uint32_t u32Month;
    uint32_t u32Day;
    uint32_t u32DayOfWeek;
    uint32_t u32Hour;
    uint32_t u32Minute;
    uint32_t u32Second;
    uint32_t u32TimeScale;
} S_RTC_TIME_DATA_T;

// RTC 相关函数
extern void RTC_Open(S_RTC_TIME_DATA_T *sPt);
extern void RTC_Close(void);
extern void RTC_32KCalibration(int32_t i32FrequencyX100);
extern void RTC_GetDateAndTime(S_RTC_TIME_DATA_T *sPt);
extern void RTC_SetDateAndTime(S_RTC_TIME_DATA_T *sPt);
extern void RTC_SetAlarmDateAndTime(S_RTC_TIME_DATA_T *sPt);
extern void RTC_EnableInt(uint32_t u32IntFlagMask);
extern void RTC_DisableInt(uint32_t u32IntFlagMask);
extern uint32_t RTC_GetIntFlag(void);
extern void RTC_ClearIntFlag(uint32_t u32IntFlagMask);
extern uint32_t RTC_GetTick(void);

#endif // RTC_H