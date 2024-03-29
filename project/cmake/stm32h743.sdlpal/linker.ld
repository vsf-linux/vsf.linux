/* Linker script for STM32H743 */

/* Linker script to configure memory regions. */
MEMORY
{
DTCMRAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 128K
RAM_D1 (xrw)    : ORIGIN = 0x24000000, LENGTH = 512K
RAM_D2 (xrw)    : ORIGIN = 0x30000000, LENGTH = 288K
RAM_D3 (xrw)    : ORIGIN = 0x38000000, LENGTH = 64K
ITCMRAM (xrw)   : ORIGIN = 0x00000000, LENGTH = 64K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 2048K
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        KEEP(*(.vectors))
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
       __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
       __exidx_end = .;
    } > FLASH

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__) >> 2)
        __copy_table_end__ = .;
    } > FLASH

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG ((__bss_end__ - __bss_start__) >> 2)
        __zero_table_end__ = .;
    } > FLASH

    __etext = .;
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array*)))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array*)))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > DTCMRAM

    .noinit (NOLOAD):
    {
        *(.bss.noinit* .noinit*)
    } > DTCMRAM

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM_D3

    .heap :
    {
        __end__ = .;
        *(.heap*)
        __HeapLimit = .;
    } > RAM_D3
    PROVIDE(__sbrk_start = ADDR(.heap));
    PROVIDE(__krbs_start = ADDR(.heap) + SIZEOF(.heap));

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
        *(.stack)
    } > RAM_D3

    /* Set stack top to end of IRAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM_D3) + LENGTH(RAM_D3);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM_D3 overflowed with stack")
}
