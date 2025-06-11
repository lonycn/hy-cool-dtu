#ifndef SYS_H
#define SYS_H

#include "Nano100Series.h"

// 系统时钟相关函数
extern void SYS_Init(void);
extern void SYS_UnlockReg(void);
extern void SYS_LockReg(void);
extern uint32_t SYS_GetExtClockFreq(void);
extern uint32_t SYS_GetHCLKFreq(void);
extern uint32_t SYS_GetPCLKFreq(void);

// 电源管理
extern void SYS_EnablePowerDown(void);
extern void SYS_DisablePowerDown(void);

// 复位相关
extern void SYS_ResetChip(void);
extern void SYS_ResetCPU(void);

#endif // SYS_H