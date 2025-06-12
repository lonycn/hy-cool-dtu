#ifndef __CORE_CM0PLUS_H__
#define __CORE_CM0PLUS_H__

/**
 * @file core_cm0plus.h
 * @brief CMSIS Cortex-M0+ Core Peripheral Access Layer Header File
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/*----------------------------------------------------------------------------
  Configuration
 *----------------------------------------------------------------------------*/

#ifndef __CM0PLUS_REV
#define __CM0PLUS_REV 0x0000U /*!< Core revision r0p0 */
#endif
#ifndef __MPU_PRESENT
#define __MPU_PRESENT 0U /*!< No MPU */
#endif
#ifndef __VTOR_PRESENT
#define __VTOR_PRESENT 1U /*!< VTOR present */
#endif
#ifndef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS 2U /*!< Number of priority bits */
#endif
#ifndef __Vendor_SysTickConfig
#define __Vendor_SysTickConfig 0U /*!< Set to 1 if different SysTick Config */
#endif

/*----------------------------------------------------------------------------
  Interrupt Number Definition
 *----------------------------------------------------------------------------*/

typedef enum IRQn {
    NonMaskableInt_IRQn = -14, /*!< 2 Non Maskable Interrupt */
    HardFault_IRQn = -13,      /*!< 3 HardFault Interrupt */
    SVCall_IRQn = -5,          /*!< 11 SV Call Interrupt */
    PendSV_IRQn = -2,          /*!< 14 Pend SV Interrupt */
    SysTick_IRQn = -1,         /*!< 15 System Tick Interrupt */

    /* External Interrupts */
    BOD_IRQn = 0,        /*!< Brown Out detection */
    WDT_IRQn = 1,        /*!< Watch Dog Timer */
    EINT0_IRQn = 2,      /*!< External Interrupt 0 */
    EINT1_IRQn = 3,      /*!< External Interrupt 1 */
    GPIOP0P1_IRQn = 4,   /*!< GPIO Port 0/1 */
    GPIOP2P3P4_IRQn = 5, /*!< GPIO Port 2/3/4 */
    PWM0_IRQn = 6,       /*!< PWM0 */
    PWM1_IRQn = 7,       /*!< PWM1 */
    TMR0_IRQn = 8,       /*!< Timer 0 */
    TMR1_IRQn = 9,       /*!< Timer 1 */
    TMR2_IRQn = 10,      /*!< Timer 2 */
    TMR3_IRQn = 11,      /*!< Timer 3 */
    UART0_IRQn = 12,     /*!< UART0 */
    UART1_IRQn = 13,     /*!< UART1 */
    SPI0_IRQn = 14,      /*!< SPI0 */
    SPI1_IRQn = 15,      /*!< SPI1 */
    SPI2_IRQn = 16,      /*!< SPI2 */
    I2C0_IRQn = 18,      /*!< I2C0 */
    I2C1_IRQn = 19,      /*!< I2C1 */
    CAN_IRQn = 20,       /*!< CAN */
    SC2_IRQn = 21,       /*!< Smart Card 2 */
    SC0_IRQn = 22,       /*!< Smart Card 0 */
    SC1_IRQn = 23,       /*!< Smart Card 1 */
    USBD_IRQn = 24,      /*!< USB Device */
    LCD_IRQn = 25,       /*!< LCD */
    PDMA_IRQn = 26,      /*!< PDMA */
    I2S_IRQn = 27,       /*!< I2S */
    PDWU_IRQn = 28,      /*!< Power Down Wake Up */
    ADC_IRQn = 29,       /*!< ADC */
    DAC_IRQn = 30,       /*!< DAC */
    RTC_IRQn = 31        /*!< RTC */
} IRQn_Type;

/*----------------------------------------------------------------------------
  Processor and Core Peripheral Section
 *----------------------------------------------------------------------------*/

/* Configuration of the Cortex-M0+ Processor and Core Peripherals */
#define __CORTEX_M 0x00U /*!< Cortex-M Core */

/*----------------------------------------------------------------------------
  Memory mapping
 *----------------------------------------------------------------------------*/

#ifndef FLASH_BASE
#define FLASH_BASE 0x00000000UL /*!< FLASH base address */
#endif
#ifndef SRAM_BASE
#define SRAM_BASE 0x20000000UL /*!< SRAM base address */
#endif
#ifndef PERIPH_BASE
#define PERIPH_BASE 0x40000000UL /*!< Peripheral base address */
#endif

/*----------------------------------------------------------------------------
  System Control Block (SCB)
 *----------------------------------------------------------------------------*/

typedef struct {
    volatile uint32_t CPUID; /*!< CPU ID Base Register */
    volatile uint32_t ICSR;  /*!< Interrupt Control and State Register */
    volatile uint32_t VTOR;  /*!< Vector Table Offset Register */
    volatile uint32_t AIRCR; /*!< Application Interrupt and Reset Control Register */
    volatile uint32_t SCR;   /*!< System Control Register */
    volatile uint32_t CCR;   /*!< Configuration and Control Register */
    uint32_t RESERVED1;
    volatile uint32_t SHP[2U]; /*!< System Handlers Priority Registers */
    volatile uint32_t SHCSR;   /*!< System Handler Control and State Register */
} SCB_Type;

#define SCS_BASE (0xE000E000UL)        /*!< System Control Space Base Address */
#define SCB_BASE (SCS_BASE + 0x0D00UL) /*!< System Control Block Base Address */
#define SCB ((SCB_Type *)SCB_BASE)     /*!< SCB configuration struct */

/*----------------------------------------------------------------------------
  Nested Vectored Interrupt Controller (NVIC)
 *----------------------------------------------------------------------------*/

typedef struct {
    volatile uint32_t ISER[1U]; /*!< Interrupt Set Enable Register */
    uint32_t RESERVED0[31U];
    volatile uint32_t ICER[1U]; /*!< Interrupt Clear Enable Register */
    uint32_t RSERVED1[31U];
    volatile uint32_t ISPR[1U]; /*!< Interrupt Set Pending Register */
    uint32_t RESERVED2[31U];
    volatile uint32_t ICPR[1U]; /*!< Interrupt Clear Pending Register */
    uint32_t RESERVED3[31U];
    uint32_t RESERVED4[64U];
    volatile uint32_t IP[8U]; /*!< Interrupt Priority Register */
} NVIC_Type;

#define NVIC_BASE (SCS_BASE + 0x0100UL) /*!< NVIC Base Address */
#define NVIC ((NVIC_Type *)NVIC_BASE)   /*!< NVIC configuration struct */

/*----------------------------------------------------------------------------
  System Tick Timer (SysTick)
 *----------------------------------------------------------------------------*/

typedef struct {
    volatile uint32_t CTRL;  /*!< SysTick Control and Status Register */
    volatile uint32_t LOAD;  /*!< SysTick Reload Value Register */
    volatile uint32_t VAL;   /*!< SysTick Current Value Register */
    volatile uint32_t CALIB; /*!< SysTick Calibration Register */
} SysTick_Type;

#define SysTick_BASE (SCS_BASE + 0x0010UL)     /*!< SysTick Base Address */
#define SysTick ((SysTick_Type *)SysTick_BASE) /*!< SysTick configuration struct */

/*----------------------------------------------------------------------------
  Core Function Interface
 *----------------------------------------------------------------------------*/

/**
 * @brief  Enable IRQ Interrupts
 * @note   Enables IRQ interrupts by clearing the I-bit in the CPSR.
 */
static inline void __enable_irq(void) {
    __asm volatile("cpsie i" : : : "memory");
}

/**
 * @brief  Disable IRQ Interrupts
 * @note   Disables IRQ interrupts by setting the I-bit in the CPSR.
 */
static inline void __disable_irq(void) {
    __asm volatile("cpsid i" : : : "memory");
}

/**
 * @brief  No Operation
 * @note   No Operation does nothing. This instruction can be used for code alignment purposes.
 */
static inline void __NOP(void) {
    __asm volatile("nop");
}

/**
 * @brief  Wait For Interrupt
 * @note   Wait For Interrupt is a hint instruction that suspends execution until one of a number of events occurs.
 */
static inline void __WFI(void) {
    __asm volatile("wfi");
}

/**
 * @brief  Wait For Event
 * @note   Wait For Event is a hint instruction that permits the processor to enter a low-power state until one of a
 * number of events occurs.
 */
static inline void __WFE(void) {
    __asm volatile("wfe");
}

/**
 * @brief  Send Event
 * @note   Send Event is a hint instruction. It causes an event to be signaled to the CPU.
 */
static inline void __SEV(void) {
    __asm volatile("sev");
}

/**
 * @brief  Instruction Synchronization Barrier
 * @note   Instruction Synchronization Barrier flushes the pipeline in the processor, so that all instructions following
 * the ISB are fetched from cache or memory, after the instruction has been completed.
 */
static inline void __ISB(void) {
    __asm volatile("isb 0xF" : : : "memory");
}

/**
 * @brief  Data Synchronization Barrier
 * @note   Acts as a special kind of Data Memory Barrier. It completes when all explicit memory accesses before this
 * instruction complete.
 */
static inline void __DSB(void) {
    __asm volatile("dsb 0xF" : : : "memory");
}

/**
 * @brief  Data Memory Barrier
 * @note   Ensures the apparent order of the explicit memory operations before and after the instruction, without
 * ensuring their completion.
 */
static inline void __DMB(void) {
    __asm volatile("dmb 0xF" : : : "memory");
}

/*----------------------------------------------------------------------------
  NVIC Functions
 *----------------------------------------------------------------------------*/

/* Helper macros for NVIC_SetPriority */
#define _BIT_SHIFT(IRQn) (((((uint32_t)(int32_t)(IRQn))) & 0x03UL) * 8UL)
#define _SHP_IDX(IRQn) ((((((uint32_t)(int32_t)(IRQn)) & 0x0FUL) - 8UL) >> 2UL))
#define _IP_IDX(IRQn) ((((uint32_t)(int32_t)(IRQn)) >> 2UL))

/**
 * @brief  Enable External Interrupt
 * @param  IRQn  External interrupt number
 */
static inline void NVIC_EnableIRQ(IRQn_Type IRQn) {
    if ((int32_t)(IRQn) >= 0) {
        NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
    }
}

/**
 * @brief  Disable External Interrupt
 * @param  IRQn  External interrupt number
 */
static inline void NVIC_DisableIRQ(IRQn_Type IRQn) {
    if ((int32_t)(IRQn) >= 0) {
        NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
    }
}

/**
 * @brief  Set Pending Interrupt
 * @param  IRQn  Interrupt number
 */
static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
    if ((int32_t)(IRQn) >= 0) {
        NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
    }
}

/**
 * @brief  Clear Pending Interrupt
 * @param  IRQn  External interrupt number
 */
static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
    if ((int32_t)(IRQn) >= 0) {
        NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
    }
}

/**
 * @brief  Set Interrupt Priority
 * @param  IRQn  Interrupt number
 * @param  priority  Priority to set
 */
static inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
    if ((int32_t)(IRQn) >= 0) {
        NVIC->IP[_IP_IDX(IRQn)] = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
                                   (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
    } else {
        SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
                                    (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
    }
}

/**
 * @brief  System Reset
 * @note   Initiates a system reset request to reset the MCU.
 */
static inline void NVIC_SystemReset(void) {
    __DSB();

    SCB->AIRCR = ((0x5FAUL << 16U) | (SCB->AIRCR & (0x0700UL)) | (1UL << 2U));
    __DSB();

    for (;;) {
        __NOP();
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CM0PLUS_H__ */