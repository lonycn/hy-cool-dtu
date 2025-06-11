/* NANO100B Series Linker Script for 512KB Flash with OTA Support */
/* Memory Layout:
   - Bootloader: 32KB  (0x08000000 - 0x08008000)
   - App1:       224KB (0x08008000 - 0x08040000) - Main Application
   - App2:       224KB (0x08040000 - 0x08078000) - OTA Update Area
   - Config:     32KB  (0x08078000 - 0x08080000) - Configuration Data
   - RAM:        64KB  (0x20000000 - 0x20010000)
*/

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08008000, LENGTH = 224K    /* App1 partition */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K     /* SRAM */
}

/* Entry point */
ENTRY(Reset_Handler)

/* Stack size */
_stack_size = 0x1000;  /* 4KB stack */

/* Heap size */
_heap_size = 0x800;    /* 2KB heap */

SECTIONS
{
    /* Vector table and startup code */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Program code and constants */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ARM unwinding information */
    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    /* Constructor/destructor tables */
    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(.fini_array*))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > FLASH

    /* Initialized data section */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* User heap and stack */
    ._user_heap_stack :
    {
        . = ALIGN(4);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _heap_size;
        . = . + _stack_size;
        . = ALIGN(4);
    } > RAM

    /* Stack pointer initialization */
    _estack = ORIGIN(RAM) + LENGTH(RAM);

    /* OTA specific symbols */
    _ota_app1_start = 0x08008000;
    _ota_app1_size  = 0x38000;     /* 224KB */
    _ota_app2_start = 0x08040000;
    _ota_app2_size  = 0x38000;     /* 224KB */
    _ota_config_start = 0x08078000;
    _ota_config_size  = 0x8000;    /* 32KB */

    /* Remove debugging information */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    /* ARM attributes */
    .ARM.attributes 0 : { *(.ARM.attributes) }
}

/* Provide weak aliases for fault handlers */
PROVIDE(NMI_Handler = Default_Handler);
PROVIDE(HardFault_Handler = Default_Handler);
PROVIDE(SVC_Handler = Default_Handler);
PROVIDE(PendSV_Handler = Default_Handler);
PROVIDE(SysTick_Handler = Default_Handler);

/* Default handler */
PROVIDE(Default_Handler = _exit);

/* Check memory usage */
ASSERT(_estack >= _ebss, "Stack overlaps with BSS section")
ASSERT((_etext + (_edata - _sdata)) <= ORIGIN(FLASH) + LENGTH(FLASH), "Flash overflow")
ASSERT((_ebss - _sbss + _heap_size + _stack_size) <= LENGTH(RAM), "RAM overflow") 