/*
*****************************************************************************
**  File        : stm32_flash.ld
**  Abstract    : Linker script for STM32L476RG Device with
**                1024KByte FLASH, 128KByte RAM
**          Modif from JPO
**  Target      : STMicroelectronics STM32
*****************************************************************************
*/

/* Specify the memory areas */

/* JPO 17-07-2020: add a range for my vars*/
/* because me, in 1st place!!*/
/* RAM  96K maxi*/

MEMORY
{
 MYVARS (xrw)    : ORIGIN = 0x20000000, LENGTH = 10K
 RAM (xrw)       : ORIGIN = 0x20002800, LENGTH = 86K
 RAM2 (xrw)      : ORIGIN = 0x10000000, LENGTH = 32K
 FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
}

/* Define output sections */
SECTIONS

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH

  
  /* JPO: 18-07-2020: ajout fixed location variables (ne pas oublier AT > FLASH */
  /* be carefull order not really followed!! */
  .myvars :
  {
    . = ALIGN(4);
    _smyvars = .;       /* create global symbol at myvars start */
    KEEP(*(.myvars.aRxBuffer))
    KEEP(*(.myvars.ymx_VRx_PV))
    KEEP(*(.myvars.ymx_VRy_PV))
    KEEP(*(.myvars.ymx_VRx_trim))
    KEEP(*(.myvars.ymx_VRy_trim));
    /* *(.myvars); */
    
    . = ALIGN(4);
    _emyvars = .;        /* define a global symbol at myvars end */
  } > MYVARS AT > FLASH
 
  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
 
  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
 PROVIDE ( _min_heap_end = . );     /*JPO added in case of snprintf error*/
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM
 