#ifndef __NANO100SERIES_H__
#define __NANO100SERIES_H__

/**
 * @file     Nano100Series.h
 * @brief    NANO100 Series Header File
 * @version  V1.0.0
 * @date     2024-06-11
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HSI (12000000UL) /*!< Internal High Speed oscillator */
#define __HXT (12000000UL) /*!< External High Speed oscillator */
#define __LXT (32768UL)    /*!< External Low Speed oscillator */
#define __LIRC (10000UL)   /*!< Internal Low Speed oscillator */

#define __SYSTEM_CLOCK (12000000UL) /*!< System Clock Frequency */

/*----------------------------------------------------------------------------
  Configuration of the Cortex-M0+ Processor and Core Peripherals
 *----------------------------------------------------------------------------*/
#ifndef __CM0PLUS_REV
#define __CM0PLUS_REV 0x0001 /*!< Core Revision r0p1 */
#endif
#ifndef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS 2 /*!< Number of Bits used for Priority Levels */
#endif
#ifndef __Vendor_SysTickConfig
#define __Vendor_SysTickConfig 0 /*!< Set to 1 if different SysTick Config is used */
#endif

/*----------------------------------------------------------------------------
  Include CMSIS core
 *----------------------------------------------------------------------------*/
#include "core_cm0plus.h" /* Cortex-M0+ processor and core peripherals */

/*----------------------------------------------------------------------------
  IO Type Definitions
 *----------------------------------------------------------------------------*/
#ifndef __IO
#define __IO volatile /*!< Defines 'read / write' permissions */
#endif

#ifndef __O
#define __O volatile /*!< Defines 'write only' permissions */
#endif

#ifndef __I
#define __I volatile const /*!< Defines 'read only' permissions */
#endif

/*----------------------------------------------------------------------------
  IO Type Definitions
 *----------------------------------------------------------------------------*/
#ifndef __IO
#define __IO volatile /*!< Defines 'read / write' permissions */
#endif

/*----------------------------------------------------------------------------
  Basic Data Types
 *----------------------------------------------------------------------------*/
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

/*----------------------------------------------------------------------------
  Memory Base Addresses
 *----------------------------------------------------------------------------*/
#ifndef FLASH_BASE
#define FLASH_BASE (0x00000000UL) /*!< Flash base address */
#endif
#ifndef SRAM_BASE
#define SRAM_BASE (0x20000000UL) /*!< SRAM base address */
#endif
#ifndef PERIPH_BASE
#define PERIPH_BASE (0x40000000UL) /*!< Peripheral base address */
#endif

/*----------------------------------------------------------------------------
  Peripheral Memory Map
 *----------------------------------------------------------------------------*/
#define UART0_BASE (PERIPH_BASE + 0x00050000UL)
#define UART1_BASE (PERIPH_BASE + 0x00051000UL)
#define TIMER0_BASE (PERIPH_BASE + 0x00010000UL)
#define WDT_BASE (PERIPH_BASE + 0x00004000UL)

/*----------------------------------------------------------------------------
  Peripheral Declarations
 *----------------------------------------------------------------------------*/
#define UART0 ((UART_T *)UART0_BASE)
#define UART1 ((UART_T *)UART1_BASE)
#define TIMER0 ((TIMER_T *)TIMER0_BASE)

/*----------------------------------------------------------------------------
  UART Structure
 *----------------------------------------------------------------------------*/
typedef struct {
    __IO uint32_t DAT;      /*!< UART Data Register */
    __IO uint32_t INTEN;    /*!< UART Interrupt Enable Register */
    __IO uint32_t FIFO;     /*!< UART FIFO Control Register */
    __IO uint32_t LINE;     /*!< UART Line Control Register */
    __IO uint32_t MODEM;    /*!< UART Modem Control Register */
    __IO uint32_t MODEMSTS; /*!< UART Modem Status Register */
    __IO uint32_t FIFOSTS;  /*!< UART FIFO Status Register */
    __IO uint32_t INTSTS;   /*!< UART Interrupt Status Register */
    __IO uint32_t TOUT;     /*!< UART Time-out Register */
    __IO uint32_t BAUD;     /*!< UART Baud Rate Divisor Register */
    __IO uint32_t IRCR;     /*!< UART IrDA Control Register */
    __IO uint32_t ALTCTL;   /*!< UART Alternate Control/Status Register */
    __IO uint32_t FUNSEL;   /*!< UART Function Select Register */
} UART_T;

/*----------------------------------------------------------------------------
  TIMER Structure
 *----------------------------------------------------------------------------*/
typedef struct {
    __IO uint32_t CTL;     /*!< Timer Control Register */
    __IO uint32_t CMP;     /*!< Timer Compare Register */
    __IO uint32_t INTSTS;  /*!< Timer Interrupt Status Register */
    __IO uint32_t CNT;     /*!< Timer Counter Register */
    __IO uint32_t CAP;     /*!< Timer Capture Data Register */
    __IO uint32_t EXTCTL;  /*!< Timer External Control Register */
    __IO uint32_t EINTSTS; /*!< Timer External Interrupt Status Register */
} TIMER_T;

/*----------------------------------------------------------------------------
  Function Declarations
 *----------------------------------------------------------------------------*/

// UART Functions
extern void UART_Open(UART_T *uart, uint32_t u32baudrate);
extern void UART_Close(UART_T *uart);
extern uint32_t UART_Read(UART_T *uart, uint8_t *pu8RxBuf, uint32_t u32ReadBytes);
extern uint32_t UART_Write(UART_T *uart, uint8_t *pu8TxBuf, uint32_t u32WriteBytes);
extern int32_t UART_GetChar(UART_T *uart);
extern void UART_PutChar(UART_T *uart, uint8_t ch);

// Timer Functions
extern void TIMER_Open(TIMER_T *timer, uint32_t u32Mode, uint32_t u32Freq);
extern void TIMER_Close(TIMER_T *timer);
extern void TIMER_Start(TIMER_T *timer);
extern void TIMER_Stop(TIMER_T *timer);
extern void TIMER_EnableInt(TIMER_T *timer);
extern void TIMER_DisableInt(TIMER_T *timer);
extern uint32_t TIMER_GetIntFlag(TIMER_T *timer);
extern void TIMER_ClearIntFlag(TIMER_T *timer);

// System Functions
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

// NVIC Functions (from CMSIS)
#define NVIC_SystemReset() __NVIC_SystemReset()

/*----------------------------------------------------------------------------
  Timer Mode Constants
 *----------------------------------------------------------------------------*/
#define TIMER_ONESHOT_MODE (0UL)    /*!< Timer working in one-shot mode */
#define TIMER_PERIODIC_MODE (1UL)   /*!< Timer working in periodic mode */
#define TIMER_TOGGLE_MODE (2UL)     /*!< Timer working in toggle mode */
#define TIMER_CONTINUOUS_MODE (3UL) /*!< Timer working in continuous counting mode */

/*----------------------------------------------------------------------------
  Global Variables
 *----------------------------------------------------------------------------*/
extern uint32_t SystemCoreClock; /*!< System Clock Frequency (Core Clock) */

#ifdef __cplusplus
}
#endif

#endif /* __NANO100SERIES_H__ */