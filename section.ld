OUTPUT_FORMAT(pei-i386)
OUTPUT(section.pe)

_atexit  = 0xA8211E;
__Znwj   = 0xA825B9;
__ZdlPvj = 0x958C40;
"__imp__GetModuleHandleA@4" = 0xC0F378;
"__imp__GetProcAddress@8" = 0xC0F48C;

SECTIONS {
  . = __image_base__ + 0x1000;
  .text : {
    *(.text*)
    *(.data*)
    *(.bss*)
    *(.rdata)
  }
  /DISCARD/ : {
    *(.rdata$zzz)
    *(.eh_frame*)
    *(.ctors)
    *(.reloc)
    *(.idata*)
  }
}