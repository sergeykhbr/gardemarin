/*
 * Define the supported output formats - elf32-littlearm is the
 *  default
 */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
 
/* Define the target architecture */
OUTPUT_ARCH(arm)
 
/* Define the system entry point */
ENTRY(__VECTOR_TABLE)

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x00003000;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x00000400;

/* Specify the memory areas */
MEMORY
{
  FLASH                 (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00080000
  SRAM                  (RW)  : ORIGIN = 0x20000000, LENGTH = 0x00020000
  CCMRAM                (RW)  : ORIGIN = 0x10000000, LENGTH = 0x00010000
}

REGION_ALIAS("REGION_TEXT", FLASH);
REGION_ALIAS("REGION_DATA", SRAM);
REGION_ALIAS("REGION_BSS", SRAM);
 
/* Define the memory layout for the board */
SECTIONS
{
  /* text: test code section:  */
  .text :
  {
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))

    . = ALIGN(4);
    KEEP(*(.hard_reset))
    KEEP(*(.after_vectors .after_vectors.*))   /* Startup code */
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.init)                 /* section used in crti.o files */
    *(.fini)                 /* section used in crti.o files */
    . = ALIGN(4);

    /*
    * The preinit code, i.e. an array of pointers to initialisation 
    * functions to be performed before constructors.
    */
    PROVIDE_HIDDEN (__preinit_array_start = .);
        
    /** Used to run the SystemInit() before anything else. */
    KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
        
    /** Used for other platform inits. */
    KEEP(*(.preinit_array_platform .preinit_array_platform.*))
        
    /*
    * The application inits. If you need to enforce some order in 
    * execution, create new sections, as before.
    */
    KEEP(*(.preinit_array .preinit_array.*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);

    /** The init code, i.e. an array of pointers to static constructors. */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);

    /** The fini code, i.e. an array of pointers to static destructors. */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);

    *(vtable)                /* C++ virtual tables */
    *(.eh_frame)             /* section used in crtbegin.o files */
    . = ALIGN(4);
     __text_end = .;   	
  } > REGION_TEXT

  /* Section used by the libgcc.a library for fvp4 */
  .ARM.extab : ALIGN(4)
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > REGION_TEXT
   	
  . = ALIGN(4);
  __exidx_start = .;   	
  .ARM.exidx : ALIGN(4)
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > REGION_TEXT
  __exidx_end = .;
   	

  .rodata :
  {
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    rodata_end = .;
  } > REGION_TEXT


  . = ALIGN(4);
  .data : AT(rodata_end)
  {
    __DATA_START = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
  } > REGION_DATA
  __DATA_SIZE = SIZEOF(.data);
  __DATA_LOAD_START = LOADADDR(.data);


  /* Uninitialized data section. */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section. */
    . = ALIGN(4);
    __BSS_START = .;
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __BSS_END = .;
  } > REGION_BSS


   /* Put heap section after the program data */
  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    __heap_start__ = .;
    PROVIDE(end = .);
    PROVIDE(_end = .);
    PROVIDE(__end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .;
    __heap_end__ = .;
    __stack_limit = ORIGIN(REGION_DATA) + LENGTH(REGION_DATA) - HEAP_SIZE;
  } > REGION_DATA

  __stack_base = ORIGIN(REGION_DATA) + LENGTH(REGION_DATA);
}
